// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"

#include "CanvasControl.h"
#include "CanvasDevice.h"
#include "CanvasImageSource.h"
#include "RecreatableDeviceManager.impl.h"
#include "BaseControlAdapter.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;
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

    class CanvasControlAdapter : public BaseControlAdapter<CanvasControlTraits>
    {
        ComPtr<ICompositionTargetStatics> m_compositionTargetStatics;
        ComPtr<ICanvasImageSourceFactory> m_canvasImageSourceFactory;
        ComPtr<IActivationFactory> m_imageControlFactory;

    public:
        CanvasControlAdapter()
        {
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Media_CompositionTarget).Get(),
                &m_compositionTargetStatics));

            auto& module = Module<InProc>::GetModule();

            ComPtr<IActivationFactory> imageSourceActivationFactory;
            ThrowIfFailed(module.GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource).Get(),
                &imageSourceActivationFactory));
            ThrowIfFailed(imageSourceActivationFactory.As(&m_canvasImageSourceFactory));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Image).Get(),
                &m_imageControlFactory));
        }

        virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>* handler) override
        {
            return RegisteredEvent(
                m_compositionTargetStatics.Get(),
                &ICompositionTargetStatics::add_Rendering,
                &ICompositionTargetStatics::remove_Rendering,
                handler);
        }

        virtual RegisteredEvent AddVisibilityChangedCallback(IWindowVisibilityChangedEventHandler* handler, IWindow* window) override
        {
            // Don't register for the visiblity changed event if we're in design
            // mode.  Although we have been given a valid IWindow, we'll crash
            // if we attempt to call add_VisibilityChanged on it!
            if (IsDesignModeEnabled())
                return RegisteredEvent();

            return RegisteredEvent(
                window,
                &IWindow::add_VisibilityChanged,
                &IWindow::remove_VisibilityChanged,
                handler);
        }

        virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>* handler) override
        {
            return RegisteredEvent(
                m_compositionTargetStatics.Get(),
                &ICompositionTargetStatics::add_SurfaceContentsLost,
                &ICompositionTargetStatics::remove_SurfaceContentsLost,
                handler);
        }

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasBackground backgroundMode) override 
        {
            ComPtr<ICanvasResourceCreator> resourceCreator;
            ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

            ComPtr<ICanvasImageSource> imageSource;
            ThrowIfFailed(m_canvasImageSourceFactory->CreateWithDpiAndBackground(
                resourceCreator.Get(),
                width, 
                height,
                dpi,
                backgroundMode,
                &imageSource));

            // Since we know that CanvasImageSourceFactory will only ever return
            // CanvasImageSource instances, we can be certain that the
            // ICanvasImageSource we get back is actually a CanvasImageSource.
            return static_cast<CanvasImageSource*>(imageSource.Get());
        }

        virtual ComPtr<IImage> CreateImageControl() override 
        {
            ComPtr<IInspectable> inspectableImage;
            ThrowIfFailed(m_imageControlFactory->ActivateInstance(&inspectableImage));

            ComPtr<IImage> image;
            ThrowIfFailed(inspectableImage.As(&image));

            return image;
        }
    };


    class CanvasControlFactory : public ActivationFactory<>
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
        : BaseControl(adapter)
        , m_needToHookCompositionRendering(false)
    {
        CreateImageControl();
    }

    void CanvasControl::CreateImageControl()
    {
        m_imageControl = GetAdapter()->CreateImageControl();

        //
        // Set the stretch mode to Fill. This will ensure that on high DPI, the
        // layout will confine the control to the correct area even when the
        // backing image has a different physical size from the control's
        // device-independent size.
        //
        // The logic in EnsureSizeDependentResources ensures that the Source
        // assigned to the Image control matches the CanvasImageSource extents.
        //
        ThrowIfFailed(m_imageControl->put_Stretch(Stretch_Fill));

        //
        // Set the image control as the content of this control.
        //
        ComPtr<IUIElement> imageAsUIElement;
        ThrowIfFailed(m_imageControl.As(&imageAsUIElement));

        ComPtr<IUserControl> thisAsUserControl;
        ThrowIfFailed(GetComposableBase().As(&thisAsUserControl));
        thisAsUserControl->put_Content(imageAsUIElement.Get());
    }

    void CanvasControl::RegisterEventHandlers()
    {
        using namespace ABI::Windows::UI::Xaml::Controls;
        using namespace Windows::Foundation;

        m_windowVisibilityChangedEventRegistration = GetAdapter()->AddVisibilityChangedCallback(
            this, 
            &CanvasControl::OnWindowVisibilityChanged,
            GetWindow());

        m_surfaceContentsLostEventRegistration = GetAdapter()->AddSurfaceContentsLostCallback(this, &CanvasControl::OnSurfaceContentsLost);
    }

    void CanvasControl::UnregisterEventHandlers()
    {
        if (m_renderingEventRegistration)
        {
            m_renderingEventRegistration.Release();
            m_needToHookCompositionRendering = true;
        }

        m_windowVisibilityChangedEventRegistration.Release();
        m_surfaceContentsLostEventRegistration.Release();
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
                lock.unlock();

                if (!IsWindowVisible())
                    return;

                RunWithRenderTarget(GetCurrentSize(),
                    [&](CanvasImageSource* target, ICanvasDevice*, Color const& clearColor, bool callDrawHandlers)
                    {
                        if (!target)
                            return;
                        
                        Draw(target, clearColor, callDrawHandlers);
                    });
            });
    }

    void CanvasControl::CreateOrUpdateRenderTarget(
        ICanvasDevice* device,
        CanvasBackground newBackgroundMode,
        float newDpi,
        Size newSize,
        RenderTarget* renderTarget)
    {
        bool needsCreate = (renderTarget->Target == nullptr);
        needsCreate |= (renderTarget->BackgroundMode != newBackgroundMode);
        needsCreate |= (renderTarget->Dpi != newDpi);
        needsCreate |= (renderTarget->Size != newSize);

        if (!needsCreate)
            return;

        if (newSize.Width <= 0 || newSize.Height <= 0)
        {
            // Zero-sized controls don't have image sources
            *renderTarget = RenderTarget{};
            ThrowIfFailed(m_imageControl->put_Source(nullptr));
        }
        else
        {
            renderTarget->Target = GetAdapter()->CreateCanvasImageSource(
                device,
                newSize.Width,
                newSize.Height,
                newDpi,
                newBackgroundMode);
                
            renderTarget->BackgroundMode = newBackgroundMode;
            renderTarget->Dpi = newDpi;
            renderTarget->Size = newSize;
                
            auto baseImageSource = As<IImageSource>(renderTarget->Target);
            ThrowIfFailed(m_imageControl->put_Source(baseImageSource.Get()));
        }
    }

    ComPtr<CanvasDrawEventArgs> CanvasControl::CreateDrawEventArgs(ICanvasDrawingSession* drawingSession)
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

    IFACEMETHODIMP CanvasControl::MeasureOverride(
        Size availableSize, 
        Size* returnValue)
    {
        UNREFERENCED_PARAMETER(availableSize);

        return ExceptionBoundary(
            [&]
            {
                Size zeroSize{ 0, 0 };

                //
                // Call Measure on our children (in this case just the image control).
                //
                ThrowIfFailed(As<IUIElement>(m_imageControl)->Measure(zeroSize));
            
                //
                // Reply that we're happy to be sized however the layout engine wants to size us.
                //
                *returnValue = zeroSize;
            });
    }
    
    IFACEMETHODIMP CanvasControl::ArrangeOverride(
        Size finalSize, 
        Size* returnValue)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Call Arrange on our children (in this case just the image control).
                //
                ThrowIfFailed(As<IUIElement>(m_imageControl)->Arrange(Rect{ 0, 0, finalSize.Width, finalSize.Height }));

                //
                // Reply that we're happy to accept the size chosen by the layout engine.
                //
                *returnValue = finalSize;
        });
    }
    
    IFACEMETHODIMP CanvasControl::OnApplyTemplate()
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Allow base class to handle this
                //
                ComPtr<IFrameworkElementOverrides> base;
                ThrowIfFailed(GetComposableBase().As(&base));
                ThrowIfFailed(base->OnApplyTemplate());
            });
    }
    
    HRESULT CanvasControl::OnWindowVisibilityChanged(IInspectable*, IVisibilityChangedEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                boolean isVisible;
                ThrowIfFailed(args->get_Visible(&isVisible));

                auto lock = GetLock();

                if (isVisible)
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
            });
    }

    HRESULT CanvasControl::OnSurfaceContentsLost(IInspectable*, IInspectable*)
    {
        return ExceptionBoundary(
            [&]
            {
                ResetRenderTarget();
            });
    }

    ActivatableClassWithFactory(CanvasDrawEventArgs, CanvasDrawEventArgsFactory);
    ActivatableClassWithFactory(CanvasControl, CanvasControlFactory);
}}}}
