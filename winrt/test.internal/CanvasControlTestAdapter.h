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

#pragma once

#include "MockHelpers.h"
#include "MockWindow.h"

using ABI::Windows::Graphics::Display::DisplayInformation;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Windows::ApplicationModel::Core;

class CanvasControlTestAdapter : public ICanvasControlAdapter
{
    ComPtr<MockWindow> m_mockWindow;

public:
    CALL_COUNTER_WITH_MOCK(CreateCanvasDeviceMethod, ComPtr<ICanvasDevice>());
    ComPtr<MockEventSource<DpiChangedHandler>> DpiChangedEventSource;
    ComPtr<MockEventSourceUntyped> CompositionRenderingEventSource;
    ComPtr<MockEventSourceUntyped> SurfaceContentsLostEventSource;
    ComPtr<MockEventSource<IEventHandler<SuspendingEventArgs*>>> SuspendingEventSource;
    CALL_COUNTER_WITH_MOCK(CreateCanvasImageSourceMethod, ComPtr<CanvasImageSource>(ICanvasDevice*, int, int, CanvasBackground));

    CanvasControlTestAdapter()
        : m_mockWindow(Make<MockWindow>())
        , DpiChangedEventSource(Make<MockEventSource<DpiChangedHandler>>(L"DpiChanged"))
        , CompositionRenderingEventSource(Make<MockEventSourceUntyped>(L"CompositionRendering"))
        , SurfaceContentsLostEventSource(Make<MockEventSourceUntyped>(L"SurfaceContentsLost"))
        , SuspendingEventSource(Make<MockEventSource<IEventHandler<SuspendingEventArgs*>>>(L"Suspending"))
    {
        CreateCanvasDeviceMethod.AllowAnyCall();
    }

    virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override
    {
        auto control = Make<StubUserControl>();

        ComPtr<IInspectable> inspectableControl;
        ThrowIfFailed(control.As(&inspectableControl));

        return std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>>(inspectableControl, control);
    }

    virtual ComPtr<ICanvasDevice> CreateCanvasDevice() override
    {
        auto device = CreateCanvasDeviceMethod.WasCalled();
        if (device)
            return device;
        return Make<StubCanvasDevice>();
    }

    virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>* value) override
    {
        return SuspendingEventSource->Add(value);
    }

    virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>* value) override
    {
        return CompositionRenderingEventSource->Add(value);
    }

    void RaiseCompositionRenderingEvent()
    {
        IInspectable* sender = nullptr;
        IInspectable* arg = nullptr;
        ThrowIfFailed(CompositionRenderingEventSource->InvokeAll(sender, arg));
    }

    virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>* value) override
    {
        return SurfaceContentsLostEventSource->Add(value);
    }
    
    void RaiseSurfaceContentsLostEvent()
    {
        IInspectable* sender = nullptr;
        IInspectable* arg = nullptr;
        ThrowIfFailed(SurfaceContentsLostEventSource->InvokeAll(sender, arg));
    }

    // This function is called by the CanvasImageSourceDrawingSessionFactory
    // created by CreateCanvasImageSource before it returns the
    // MockCanvasDrawingSesion.  (It's quite tricky to inject this by other
    // means).
    std::function<ComPtr<MockCanvasDrawingSession>()> OnCanvasImageSourceDrawingSessionFactory_Create;

    virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(
        ICanvasDevice* device, 
        int width, 
        int height, 
        CanvasBackground backgroundMode) override
    {
        auto result = CreateCanvasImageSourceMethod.WasCalled(device, width, height, backgroundMode);
        if (result)
            return result;

        auto sisFactory = Make<MockSurfaceImageSourceFactory>();
        sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
            [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
        {
            return Make<StubSurfaceImageSource>();
        };

        auto dsFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        dsFactory->CreateMethod.AllowAnyCall(
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, RECT const&, float)
            {
                if (OnCanvasImageSourceDrawingSessionFactory_Create)
                {
                    // We call the function through a copy - this is so the
                    // function itself can modify the value of
                    // OnCanvasImageSourceDrawingSessionFactory_Create without
                    // self-destructing.
                    auto createFunction = OnCanvasImageSourceDrawingSessionFactory_Create;
                    return createFunction();
                }

                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasResourceCreator> resourceCreator;
        ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

        return Make<CanvasImageSource>(
            resourceCreator.Get(),
            width,
            height,
            backgroundMode,
            sisFactory.Get(),
            dsFactory);
    }

    virtual ComPtr<IImage> CreateImageControl() override
    {
        return Make<StubImageControl>();
    }

    virtual float GetLogicalDpi() override
    {
        return DEFAULT_DPI;
    }

    virtual RegisteredEvent AddDpiChangedCallback(DpiChangedHandler* value) override
    {
        return DpiChangedEventSource->Add(value);
    }

    void RaiseDpiChangedEvent()
    {
        ThrowIfFailed(DpiChangedEventSource->InvokeAll(nullptr, nullptr));
    }

    virtual RegisteredEvent AddVisibilityChangedCallback(IWindowVisibilityChangedEventHandler* value, IWindow* window)
    {
        MockWindow* mockWindow = dynamic_cast<MockWindow*>(window);
        return mockWindow->VisibilityChangedEventSource->Add(value);
    }

    virtual ComPtr<IWindow> GetCurrentWindow() override
    {
        return m_mockWindow;
    }

    ComPtr<MockWindow> GetCurrentMockWindow()
    {
        return m_mockWindow;
    }
};
