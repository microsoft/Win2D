// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "CanvasVirtualControl.h"
#include "CanvasVirtualImageSource.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ABI::Microsoft::UI::Dispatching;

class CanvasVirtualControlFactory;

ActivatableClassWithFactory(CanvasVirtualControl, CanvasVirtualControlFactory);


#pragma warning(disable: 4250)  // See CanvasControl.cpp for details

class CanvasVirtualControlAdapter : public BaseControlAdapter<CanvasVirtualControlTraits>
                                  , public ImageControlMixInAdapter
{
    ComPtr<ICanvasVirtualImageSourceFactory> m_canvasVirtualImageSourceFactory;
    
public:
    CanvasVirtualControlAdapter()
    {
        auto& module = Module<InProc>::GetModule();

        ComPtr<IActivationFactory> canvasVirtualImageSourceFactory;
        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasVirtualImageSource).Get(),
            &canvasVirtualImageSourceFactory));
        m_canvasVirtualImageSourceFactory = As<ICanvasVirtualImageSourceFactory>(canvasVirtualImageSourceFactory);
    }
    
    virtual ComPtr<ICanvasVirtualImageSource> CreateCanvasVirtualImageSource(
            ICanvasDevice* device,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode) override
    {
        ComPtr<ICanvasVirtualImageSource> imageSource;
        ThrowIfFailed(m_canvasVirtualImageSourceFactory->CreateWithWidthAndHeightAndDpiAndAlphaMode(
            As<ICanvasResourceCreator>(device).Get(),
            width,
            height,
            dpi,
            alphaMode,
            &imageSource));

        return imageSource;
    }
};

#pragma warning(default: 4250)

class CanvasVirtualControlFactory : public AgileActivationFactory<>
                                  , private LifespanTracker<CanvasVirtualControlFactory>
{
    std::weak_ptr<CanvasVirtualControlAdapter> m_adapter;
    
public:
    IFACEMETHODIMP ActivateInstance(IInspectable** obj) override
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(obj);

                auto adapter = m_adapter.lock();

                if (!adapter)
                    m_adapter = adapter = std::make_shared<CanvasVirtualControlAdapter>();

                auto control = Make<CanvasVirtualControl>(adapter);
                CheckMakeResult(control);

                ThrowIfFailed(control.CopyTo(obj));
            });
    };
};


CanvasVirtualControl::CanvasVirtualControl(std::shared_ptr<ICanvasVirtualControlAdapter> adapter)
    : BaseControl(adapter, true)
    , ImageControlMixIn(As<IUserControl>(GetComposableBase()).Get(), adapter.get())
    , m_lastImageSourceThatHasBeenDrawn(nullptr)
{
}

IFACEMETHODIMP CanvasVirtualControl::add_RegionsInvalidated(
    ITypedEventHandler<CanvasVirtualControl*, CanvasRegionsInvalidatedEventArgs*>* value,
    EventRegistrationToken* token)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            CheckInPointer(token);
            
            CheckIsOnUIThread();

            ThrowIfFailed(m_regionsInvalidatedEventSource.Add(value, token));

            //
            // If the image source has already been created then we may have
            // missed an event from the image source.  So we invalidate the
            // entire image source in order to give this event handler a chance
            // to draw.
            //
            auto imageSource = GetCurrentRenderTarget()->Target;
            if (imageSource)
                ThrowIfFailed(imageSource->Invalidate());
        });
}


IFACEMETHODIMP CanvasVirtualControl::remove_RegionsInvalidated(EventRegistrationToken token)
{
    return ExceptionBoundary(
        [&]
        {
            CheckIsOnUIThread();
            
            ThrowIfFailed(m_regionsInvalidatedEventSource.Remove(token));
        });
}


IFACEMETHODIMP CanvasVirtualControl::CreateDrawingSession(Rect updateRectangle, ICanvasDrawingSession** drawingSession)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(drawingSession);

            auto imageSource = GetCurrentRenderTarget()->Target;

            if (!IsReadyToDraw() || !imageSource)
            {
                ThrowHR(E_FAIL, Strings::CreateDrawingSessionCalledBeforeRegionsInvalidated);
            }

            auto clearColor = GetClearColor();
            ThrowIfFailed(imageSource->CreateDrawingSession(clearColor, updateRectangle, drawingSession));
        });
}


IFACEMETHODIMP CanvasVirtualControl::SuspendDrawingSession(ICanvasDrawingSession* ds)
{
    return ExceptionBoundary(
        [&]
        {
            auto imageSource = GetCurrentRenderTarget()->Target;
            if (imageSource)
                ThrowIfFailed(imageSource->SuspendDrawingSession(ds));
        });
}


IFACEMETHODIMP CanvasVirtualControl::ResumeDrawingSession(ICanvasDrawingSession* ds)
{
    return ExceptionBoundary(
        [&]
        {
            auto imageSource = GetCurrentRenderTarget()->Target;
            if (imageSource)
                ThrowIfFailed(imageSource->ResumeDrawingSession(ds));
        });
}


IFACEMETHODIMP CanvasVirtualControl::Invalidate()
{
    return ExceptionBoundary(
        [&]
        {
            auto imageSource = GetCurrentRenderTarget()->Target;
            if (imageSource)
                ThrowIfFailed(imageSource->Invalidate());
        });
}


IFACEMETHODIMP CanvasVirtualControl::InvalidateRegion(Rect region)
{
    return ExceptionBoundary(
        [&]
        {
            auto imageSource = GetCurrentRenderTarget()->Target;
            if (imageSource)
                ThrowIfFailed(imageSource->InvalidateRegion(region));
        });
}


void CanvasVirtualControl::CreateOrUpdateRenderTarget(
    ICanvasDevice* device,
    CanvasAlphaMode newAlphaMode,
    float newDpi,
    Size newSize,
    RenderTarget* renderTarget)
{
    bool renderTargetNotCreated = (renderTarget->Target == nullptr);
    bool alphaModeChanged = (renderTarget->AlphaMode != newAlphaMode);
    bool dpiChanged = (renderTarget->Dpi != newDpi);
    bool sizeChanged = (renderTarget->Size != newSize);

    bool newSizeIsEmpty = (newSize.Width <= 0 || newSize.Height <= 0);

    if (newSizeIsEmpty)
    {
        // Zero-sized controls don't have image sources
        *renderTarget = RenderTarget{};
        SetImageSource(nullptr);
    }
    else if (renderTargetNotCreated || alphaModeChanged)
    {
        renderTarget->Target = GetAdapter()->CreateCanvasVirtualImageSource(
            device,
            newSize.Width,
            newSize.Height,
            newDpi,
            newAlphaMode);

        renderTarget->AlphaMode = newAlphaMode;
        renderTarget->Dpi = newDpi;
        renderTarget->Size = newSize;

        auto onRegionsInvalidated = Callback<ImageSourceRegionsInvalidatedHandler>(this, &CanvasVirtualControl::OnRegionsInvalidated);
        CheckMakeResult(onRegionsInvalidated);
        
        m_regionsInvalidatedEventRegistration = RegisteredEvent(
            renderTarget->Target.Get(),
            &ICanvasVirtualImageSource::add_RegionsInvalidated,
            &ICanvasVirtualImageSource::remove_RegionsInvalidated,
            onRegionsInvalidated.Get());

        ComPtr<IVirtualSurfaceImageSource> vsis;
        ThrowIfFailed(renderTarget->Target->get_Source(&vsis));

        SetImageSource(As<IImageSource>(vsis).Get());

        m_lastImageSourceThatHasBeenDrawn = renderTarget->Target;
    }
    else if (dpiChanged || sizeChanged)
    {
        assert(!newSizeIsEmpty);
        assert(!renderTargetNotCreated);
        assert(!alphaModeChanged);

        ThrowIfFailed(renderTarget->Target->ResizeWithWidthAndHeightAndDpi(
            newSize.Width,
            newSize.Height,
            newDpi));
        renderTarget->Dpi = newDpi;
        renderTarget->Size = newSize;

        if (dpiChanged)
            ThrowIfFailed(renderTarget->Target->Invalidate());
    }
}


void CanvasVirtualControl::Changed(ChangeReason reason)
{
    if (!IsLoaded())
        return;
            
    bool deferCallingChangedImpl;

    //
    // IWindow will not be available in Xaml island scenarios, so prefer
    // IDependencyObject for getting the dispatcher.
    //
    ComPtr<IDispatcherQueue> dispatcher;
    auto control = GetControl();
    if (auto dependencyObject = MaybeAs<IDependencyObject>(control))
    {
        ThrowIfFailed(dependencyObject->get_DispatcherQueue(dispatcher.ReleaseAndGetAddressOf()));
    }
    else
    {
        ThrowIfFailed(GetWindow()->get_DispatcherQueue(dispatcher.ReleaseAndGetAddressOf()));
    }

    if (!dispatcher)
    {
        // There's no dispatcher but get_Dispatcher didn't fail - this could
        // mean we're running in the designer.
        assert(GetAdapter()->IsDesignModeEnabled());
        deferCallingChangedImpl = false;
    }
    else if (reason == ChangeReason::DeviceLost)
    {
        //
        // When a device lost has happened we are likely to have a large
        // callstack above us for whatever it was that spotted the device lost.
        // Also, at this point, the recreatable device manager isn't in a state
        // where it can recreate the device since it's still in the middle of
        // handling the lost device exception.
        //
        // So in this case we use the dispatcher to re-issue the change once the
        // rest of the device lost mechanism has completed.
        //
        deferCallingChangedImpl = true;
    }
    else
    {
        //
        // This function may be called on any thread.  However, much of what we
        // need to do can only be done on the UI thread.  So we check to see if
        // we're on the UI thread.  If we're not then we use the dispatcher to
        // marshal to the UI thread.
        //        
        boolean hasThreadAccess;
        ThrowIfFailed(As<IDispatcherQueue2>(dispatcher)->get_HasThreadAccess(&hasThreadAccess));
        
        deferCallingChangedImpl = !hasThreadAccess;
    }

    if (!deferCallingChangedImpl)
    {
        ChangedImpl(reason);
    }
    else
    {
        WeakRef weakSelf = AsWeak(this);
        auto callback = Callback<AddFtmBase<IDispatcherQueueHandler>::Type>(
            [weakSelf, reason]() mutable
            {
                return ExceptionBoundary(
                    [&]
                    {
                        auto strongSelf = LockWeakRef<ICanvasVirtualControl>(weakSelf);
                        auto self = static_cast<CanvasVirtualControl*>(strongSelf.Get());

                        if (self)
                        {
                            self->ChangedImpl(reason);
                        }
                    });
            });
        boolean result = true;
        dispatcher->TryEnqueueWithPriority(ABI::Microsoft::UI::Dispatching::DispatcherQueuePriority_Normal, callback.Get(), &result);
    }
}


void CanvasVirtualControl::ChangedImpl(ChangeReason reason)
{
    //
    // This is called on the UI thread (Changed() makes sure of it)
    //

    if (!IsVisible())
        return;

    RunWithRenderTarget(
        [&] (ICanvasVirtualImageSource* imageSource, ICanvasDevice*, Color const&, bool areResourcesCreated)
        {
            if (!imageSource)
                return;

            switch (reason)
            {
            case ChangeReason::ClearColor:
                break;

            case ChangeReason::Other:
                if (!areResourcesCreated)
                    return;
                if (imageSource == m_lastImageSourceThatHasBeenDrawn.Get())
                    return;
                break;

            default:
                return;
            }
                        
            ThrowIfFailed(imageSource->Invalidate());
        });
}


void CanvasVirtualControl::Loaded()
{
    ImageControlMixIn::RegisterEventHandlers<CanvasVirtualControl>(GetAdapter().get());
}


void CanvasVirtualControl::Unloaded()
{
    ImageControlMixIn::UnregisterEventHandlers();
    m_regionsInvalidatedEventRegistration.Release();
    ResetRenderTarget();
}


void CanvasVirtualControl::ApplicationSuspending(ISuspendingEventArgs*)
{
    Trim();
}


void CanvasVirtualControl::ApplicationResuming()
{
    // nothing
}


void CanvasVirtualControl::WindowVisibilityChanged()
{
    if (IsVisible())
    {
        auto imageSource = GetCurrentRenderTarget()->Target;
        if (imageSource)
        {
            ThrowIfFailed(imageSource->RaiseRegionsInvalidatedIfAny());
        }
        else
        {
            Changed(ChangeReason::Other);
        }
    }
}


HRESULT CanvasVirtualControl::OnRegionsInvalidated(ICanvasVirtualImageSource* sender, ICanvasRegionsInvalidatedEventArgs* args)
{
    return ExceptionBoundary([&] { OnRegionsInvalidatedImpl(sender, args); });
}


void CanvasVirtualControl::OnRegionsInvalidatedImpl(ICanvasVirtualImageSource*, ICanvasRegionsInvalidatedEventArgs* args)
{
    if (!IsVisible())
        return;
    
    RunWithCurrentRenderTarget(
        [=] (ICanvasVirtualImageSource* imageSource, Color const& clearColor, bool areResourcesCreated)
        {
            if (!areResourcesCreated || m_regionsInvalidatedEventSource.GetSize() == 0)
            {
                m_lastImageSourceThatHasBeenDrawn = nullptr;
                ClearAllRegions(imageSource, clearColor, args);
            }
            else
            {
                m_lastImageSourceThatHasBeenDrawn = imageSource;
                ThrowIfFailed(m_regionsInvalidatedEventSource.InvokeAll(this, args));
            }
        });
}


/*static*/
void CanvasVirtualControl::ClearAllRegions(
    ICanvasVirtualImageSource* imageSource,
    Color const& clearColor,
    ICanvasRegionsInvalidatedEventArgs* args)
{    
    ComArray<Rect> regions;
    ThrowIfFailed(args->get_InvalidatedRegions(regions.GetAddressOfSize(), regions.GetAddressOfData()));

    for (auto const& region : regions)
    {
        ComPtr<ICanvasDrawingSession> ds;
        ThrowIfFailed(imageSource->CreateDrawingSession(clearColor, region, &ds));
        ThrowIfFailed(As<IClosable>(ds)->Close());
    }    
}
