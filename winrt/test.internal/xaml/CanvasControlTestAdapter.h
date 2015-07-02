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

#include "BaseControlTestAdapter.h"

class CanvasControlTestAdapter : 
    public BaseControlTestAdapter<CanvasControlTraits>
{
    ComPtr<MockWindow> m_mockWindow;

public:
    ComPtr<MockEventSourceUntyped> SurfaceContentsLostEventSource;
    ComPtr<MockEventSourceUntyped> CompositionRenderingEventSource;
    CALL_COUNTER_WITH_MOCK(CreateCanvasImageSourceMethod, ComPtr<CanvasImageSource>(ICanvasDevice*, float, float, float, CanvasAlphaMode));


    CanvasControlTestAdapter()
        : SurfaceContentsLostEventSource(Make<MockEventSourceUntyped>(L"SurfaceContentsLost"))
        , CompositionRenderingEventSource(Make<MockEventSourceUntyped>(L"CompositionRendering"))
    {
        DeviceFactory->GetSharedDeviceMethod.AllowAnyCall(
            [&](boolean, ICanvasDevice** device)
            {
                auto stubDevice = Make<StubCanvasDevice>();
                stubDevice.CopyTo(device);
                return S_OK;
            });
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
        float width, 
        float height,
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        {
            auto result = CreateCanvasImageSourceMethod.WasCalled(device, width, height, dpi, alphaMode);
            if (result)
                return result;
        }

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
                    auto result = createFunction();
                    if (result)
                        return result;
                }

                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasResourceCreator> resourceCreator;
        ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

        return Make<CanvasImageSource>(
            resourceCreator.Get(),
            width,
            height,
            dpi,
            alphaMode,
            sisFactory.Get(),
            dsFactory);
    }

    virtual ComPtr<IImage> CreateImageControl() override
    {
        return Make<StubImageControl>();
    }
};
