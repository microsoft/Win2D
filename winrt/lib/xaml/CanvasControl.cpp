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
using namespace ABI::Windows::Graphics::Display;
using namespace ABI::Windows::UI::Xaml::Media;
using namespace ABI::Windows::UI::Xaml;

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


class CanvasControlFactory : public ActivationFactory<>,
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

CanvasControl::~CanvasControl()
{
}

HRESULT CanvasControl::OnCompositionRendering(IInspectable*, IInspectable*)
{
    return ExceptionBoundary(
        [&]
        {
            if (!IsLoaded())
                return;

            auto lock = GetLock();
            m_renderingEventRegistration.Release();
            DeviceCreationOptions deviceCreationOptions = GetDeviceCreationOptions(lock);

            Color clearColor;
            Size currentSize;
            float currentDpi;
            GetSharedState(lock, &clearColor, &currentSize, &currentDpi);
            
            lock.unlock();

            if (!IsWindowVisible())
                return;

            RunWithRenderTarget(clearColor, currentSize, currentDpi, deviceCreationOptions,
                [&](CanvasImageSource* target, ICanvasDevice*, Color const& clearColor, bool callDrawHandlers)
                {
                    if (!target)
                        return;

                    Draw(target, clearColor, callDrawHandlers, false);
                });
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

void CanvasControl::Changed(Lock const& lock, ChangeReason)
{
    MustOwnLock(lock);

    if (!IsLoaded())
        return;

    if (m_renderingEventRegistration)
        return;

    m_needToHookCompositionRendering = true;

    if (!IsWindowVisible())
        return;

    HookCompositionRenderingIfNecessary();
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

void CanvasControl::HookCompositionRenderingIfNecessary()
{
    if (m_renderingEventRegistration)
        return;

    if (!m_needToHookCompositionRendering)
        return;

    m_renderingEventRegistration = GetAdapter()->AddCompositionRenderingCallback(
        this,
        &CanvasControl::OnCompositionRendering);

    m_needToHookCompositionRendering = false;
}

void CanvasControl::WindowVisibilityChanged(Lock const&)
{
    // Note that this is always called with ownership of the lock.

    if (IsVisible())
    {
        HookCompositionRenderingIfNecessary();
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
