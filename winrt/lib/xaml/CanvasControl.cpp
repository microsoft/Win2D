// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasControl.h"
#include "RecreatableDeviceManager.impl.h"
#include "BaseControlAdapter.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Microsoft::UI::Dispatching;
using namespace ABI::Windows::Graphics::Display;
using namespace ABI::Microsoft::UI::Xaml::Media;
using namespace ABI::Microsoft::UI::Xaml;

IFACEMETHODIMP CanvasDrawEventArgsFactory::Create(
    ICanvasDrawingSession* drawingSession,
    ICanvasDrawEventArgs** drawEventArgs)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(drawEventArgs);

            auto newCanvasDrawEventArgs = Make<CanvasDrawEventArgs>(drawingSession);
            CheckMakeResult(newCanvasDrawEventArgs);

            ThrowIfFailed(newCanvasDrawEventArgs.CopyTo(drawEventArgs));
        });
}

CanvasDrawEventArgs::CanvasDrawEventArgs(ICanvasDrawingSession* drawingSession) 
    : m_drawingSession(drawingSession)
{}

IFACEMETHODIMP CanvasDrawEventArgs::get_DrawingSession(ICanvasDrawingSession** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            ComPtr<ICanvasDrawingSession> drawingSession = m_drawingSession.EnsureNotClosed();
            ThrowIfFailed(drawingSession.CopyTo(value));
        });
}

//
// Warning C4250 is incorrectly triggered when we use virtual inheritance with
// pure-virtual functions.  In this case we have the functions defined in
// IImageControlMixInAdapter that are implemented by ImageControlMixInAdapter.
// The inheritance graph looks like this:
//
//                        /-- BaseControlAdapter -- ICanvasControlAdapter --\
//                       /                                                   \
//  CanvasControlAdapter                                                      -- IImageControlMixInAdapter
//                       \                                                   /
//                        \-- ImageControlMixInAdapter ---------------------/
//
// The warning is only relevant if the functions from IImageControlMixInAdapter
// are non-pure.
//
#pragma warning(disable: 4250)

class CanvasControlAdapter : public BaseControlAdapter<CanvasControlTraits>
                           , public ImageControlMixInAdapter
{
    ComPtr<ICanvasImageSourceFactory> m_canvasImageSourceFactory;

public:
    CanvasControlAdapter()
    {
        auto& module = Module<InProc>::GetModule();

        ComPtr<IActivationFactory> imageSourceActivationFactory;
        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasImageSource).Get(),
            &imageSourceActivationFactory));
        ThrowIfFailed(imageSourceActivationFactory.As(&m_canvasImageSourceFactory));
    }

    virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>* handler) override
    {
        return RegisteredEvent(
            GetCompositionTargetStatics(),
            &ICompositionTargetStatics::add_Rendering,
            &ICompositionTargetStatics::remove_Rendering,
            handler);
    }

    virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode) override 
    {
        ComPtr<ICanvasResourceCreator> resourceCreator;
        ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

        ComPtr<ICanvasImageSource> imageSource;
        ThrowIfFailed(m_canvasImageSourceFactory->CreateWithWidthAndHeightAndDpiAndAlphaMode(
            resourceCreator.Get(),
            width, 
            height,
            dpi,
            alphaMode,
            &imageSource));

        // Since we know that CanvasImageSourceFactory will only ever return
        // CanvasImageSource instances, we can be certain that the
        // ICanvasImageSource we get back is actually a CanvasImageSource.
        return static_cast<CanvasImageSource*>(imageSource.Get());
    }
};

#pragma warning(default: 4250)


class CanvasControlFactory : public AgileActivationFactory<>,
                             private LifespanTracker<CanvasControlFactory>
{
    std::weak_ptr<CanvasControlAdapter> m_adapter;

public:
    IFACEMETHODIMP ActivateInstance(IInspectable** obj) override
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(obj);

                auto adapter = m_adapter.lock();

                if (!adapter)
                    m_adapter = adapter = std::make_shared<CanvasControlAdapter>();

                auto control = Make<CanvasControl>(adapter);
                CheckMakeResult(control);

                ThrowIfFailed(control.CopyTo(obj));
            });
    }
};    

CanvasControl::CanvasControl(
    std::shared_ptr<ICanvasControlAdapter> adapter)
    : BaseControlWithDrawHandler(adapter, true)
    , ImageControlMixIn(As<IUserControl>(GetComposableBase()).Get(), adapter.get())
    , m_needToHookCompositionRendering(false)
{
}

void CanvasControl::RegisterEventHandlers()
{
    ImageControlMixIn::RegisterEventHandlers<CanvasControl>(GetAdapter().get());
}

void CanvasControl::UnregisterEventHandlers()
{
    if (m_renderingEventRegistration)
    {
        m_renderingEventRegistration.Release();
        m_needToHookCompositionRendering = true;
    }

    ImageControlMixIn::UnregisterEventHandlers();
}


IFACEMETHODIMP CanvasControl::Invalidate()
{
    return ExceptionBoundary(
        [&]
        {
            Changed(ChangeReason::Other);
        });
}


HRESULT CanvasControl::OnCompositionRendering(IInspectable*, IInspectable*)
{
    return ExceptionBoundary(
        [&]
        {
            if (!IsLoaded())
                return;

            auto lock = Lock(m_renderingEventMutex);
            m_renderingEventRegistration.Release();
            lock.unlock();

            if (!IsVisible())
                return;

            try
            {
                DrawControl();
            }
            catch (HResultException const& e)
            {
                // Sometimes, the XAML SurfaceImageSource gets into a state
                // where it returns E_SURFACE_CONTENTS_LOST, even though it
                // probably shouldn't.  We handle this case by recreating the
                // SurfaceImageSource and retrying the draw.  If it doesn't work
                // the second time we'll allow the error to bubble out.
                if (e.GetHr() == E_SURFACE_CONTENTS_LOST)
                {
                    GetCurrentRenderTarget()->Target = nullptr; // force the SiS to get recreated on the next draw
                    DrawControl();
                }
                else
                {
                    // Rethrow other errors.
                    throw;
                }
            }
        });
}


void CanvasControl::DrawControl()
{
    RunWithRenderTarget(
        [=](CanvasImageSource* target, ICanvasDevice*, Color const& clearColor, bool callDrawHandlers)
        {
            if (!target)
                return;
                            
            Draw(target, clearColor, callDrawHandlers, false);
        });
}


void CanvasControl::CreateOrUpdateRenderTarget(
    ICanvasDevice* device,
    CanvasAlphaMode newAlphaMode,
    float newDpi,
    Size newSize,
    RenderTarget* renderTarget)
{
    bool needsCreate = (renderTarget->Target == nullptr);
    needsCreate |= (renderTarget->AlphaMode != newAlphaMode);
    needsCreate |= (renderTarget->Dpi != newDpi);
    needsCreate |= (renderTarget->Size != newSize);

    if (!needsCreate)
        return;

    if (newSize.Width <= 0 || newSize.Height <= 0)
    {
        // Zero-sized controls don't have image sources
        *renderTarget = RenderTarget{};
        SetImageSource(nullptr);
    }
    else
    {
        renderTarget->Target = GetAdapter()->CreateCanvasImageSource(
            device,
            newSize.Width,
            newSize.Height,
            newDpi,
            newAlphaMode);

        renderTarget->AlphaMode = newAlphaMode;
        renderTarget->Dpi = newDpi;
        renderTarget->Size = newSize;

        SetImageSource(As<IImageSource>(renderTarget->Target).Get());
    }
}

ComPtr<CanvasDrawEventArgs> CanvasControl::CreateDrawEventArgs(ICanvasDrawingSession* drawingSession, bool)
{
    auto drawEventArgs = Make<CanvasDrawEventArgs>(drawingSession);
    CheckMakeResult(drawEventArgs);
    return drawEventArgs;
}

void CanvasControl::Changed(ChangeReason)
{
    if (!IsLoaded())
        return;

    // Are we on the UI thread?
    boolean hasThreadAccess;

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

    if (dispatcher)
    {
        ThrowIfFailed(As<IDispatcherQueue2>(dispatcher)->get_HasThreadAccess(&hasThreadAccess));
    }
    else
    {
        // Running in the designer.
        assert(GetAdapter()->IsDesignModeEnabled());
        hasThreadAccess = true;
    }

    if (hasThreadAccess)
    {
        // Do the work immediately.
        ChangedImpl();
    }
    else
    {
        // Marshal back to the UI thread.
        WeakRef weakSelf = AsWeak(this);
        auto callback = Callback<AddFtmBase<IDispatcherQueueHandler>::Type>([weakSelf]() mutable
        {
            return ExceptionBoundary([&]
            {
                auto strongSelf = LockWeakRef<ICanvasControl>(weakSelf);
                auto self = static_cast<CanvasControl*>(strongSelf.Get());

                if (self)
                {
                    self->ChangedImpl();
                }
            });
        });
        CheckMakeResult(callback);
        boolean result = true;
        dispatcher->TryEnqueueWithPriority(ABI::Microsoft::UI::Dispatching::DispatcherQueuePriority_Normal, callback.Get(), &result);
    }
}

void CanvasControl::ChangedImpl()
{
    //
    // This is called on the UI thread (Changed() makes sure of it)
    //

    auto lock = Lock(m_renderingEventMutex);

    if (m_renderingEventRegistration)
        return;

    m_needToHookCompositionRendering = true;

    if (!IsVisible())
        return;

    HookCompositionRenderingIfNecessary(lock);
}

void CanvasControl::Loaded()
{
    RegisterEventHandlers();
}

void CanvasControl::Unloaded()
{
    UnregisterEventHandlers();
}

void CanvasControl::ApplicationSuspending(ISuspendingEventArgs*)
{
    Trim();
}

void CanvasControl::ApplicationResuming()
{
}

void CanvasControl::HookCompositionRenderingIfNecessary(Lock const& renderingEventLock)
{
    MustOwnLock(renderingEventLock);
    
    if (m_renderingEventRegistration)
        return;

    if (!m_needToHookCompositionRendering)
        return;

    m_renderingEventRegistration = GetAdapter()->AddCompositionRenderingCallback(
        this,
        &CanvasControl::OnCompositionRendering);

    m_needToHookCompositionRendering = false;
}

void CanvasControl::WindowVisibilityChanged()
{
    auto lock = Lock(m_renderingEventMutex);
    
    if (IsVisible())
    {
        HookCompositionRenderingIfNecessary(lock);
    }
    else
    {
        if (m_renderingEventRegistration)
        {
            //
            // The window is invisible.  This means that
            // OnCompositionRendering() will not do anything.  However, XAML
            // will run composition if any handlers are registered on the
            // rendering event.  Since we want to avoid the system doing
            // unnecessary work we take steps to unhook the event when we're
            // not making good use of it.
            //
            m_renderingEventRegistration.Release();
            m_needToHookCompositionRendering = true;
        }
    }
}

ActivatableClassWithFactory(CanvasDrawEventArgs, CanvasDrawEventArgsFactory);
ActivatableClassWithFactory(CanvasControl, CanvasControlFactory);
