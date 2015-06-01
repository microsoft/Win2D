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

#include "CanvasAnimatedControlTestAdapter.h"

std::shared_ptr<CanvasAnimatedControlTestAdapter> CreateAnimatedControlTestAdapter(
    ComPtr<MockD2DDeviceContext> const& deviceContext,
    ComPtr<StubD2DDevice> const& mockD2DDevice,
    ComPtr<StubCanvasDevice> const& canvasDevice);

class CanvasControlTestAdapter_InjectDeviceContext : public CanvasControlTestAdapter
{
    ComPtr<ID2D1DeviceContext> m_deviceContext;

public:
    CanvasControlTestAdapter_InjectDeviceContext(ID2D1DeviceContext* deviceContext)
        : m_deviceContext(deviceContext)
    {
    }

    virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(
        ICanvasDevice* device, 
        float width, 
        float height, 
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        auto result = CreateCanvasImageSourceMethod.WasCalled(device, width, height, dpi, alphaMode);
        if (result)
            return result;

        auto sisFactory = Make<MockSurfaceImageSourceFactory>();
        sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
            [=](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
            {
                auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
                                        
                mockSurfaceImageSource->BeginDrawMethod.AllowAnyCall(
                    [=](RECT const&, IID const& iid, void** updateObject, POINT*)
                    {
                        return m_deviceContext.CopyTo(iid, updateObject);
                    });

                mockSurfaceImageSource->SetDeviceMethod.AllowAnyCall();
                mockSurfaceImageSource->EndDrawMethod.AllowAnyCall();

                return mockSurfaceImageSource;
            };

        auto dsFactory = std::make_shared<CanvasImageSourceDrawingSessionFactory>();

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
};

template<typename TRAITS>
struct TestAdapter;

template<>
struct TestAdapter<CanvasControlTraits>
{
    typedef CanvasControlTestAdapter type;
    typedef CanvasControlTestAdapter_InjectDeviceContext injectDeviceContextType;

    static std::shared_ptr<type> Create()
    {
        return std::make_shared<CanvasControlTestAdapter>();
    }

    static std::shared_ptr<injectDeviceContextType> Create(MockD2DDeviceContext* deviceContext)
    {
        auto adapter = std::make_shared<CanvasControlTestAdapter_InjectDeviceContext>(deviceContext);
        adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        return adapter;
    }
};

template<>
struct TestAdapter<CanvasAnimatedControlTraits>
{
    typedef CanvasAnimatedControlTestAdapter type;
    typedef CanvasAnimatedControlTestAdapter injectDeviceContextType;

    static std::shared_ptr<CanvasAnimatedControlTestAdapter> Create()
    {
        return std::make_shared<CanvasAnimatedControlTestAdapter>();
    }

    static std::shared_ptr<CanvasAnimatedControlTestAdapter> Create(MockD2DDeviceContext* deviceContext)
    {
        auto mockD2DDevice = Make<StubD2DDevice>();
        auto canvasDevice = Make<StubCanvasDevice>(mockD2DDevice);
        return CreateAnimatedControlTestAdapter(deviceContext, mockD2DDevice, canvasDevice);
    }
};



template<typename TRAITS>
struct ClearColorFixture
{
    typedef typename TestAdapter<TRAITS>::injectDeviceContextType adapter_t;
    typedef typename TRAITS::control_t control_t;
    typedef typename TRAITS::drawEventHandler_t drawEventHandler_t;

    ComPtr<MockD2DDeviceContext> DeviceContext;

    std::shared_ptr<adapter_t> Adapter;

    ComPtr<control_t> Control;

    ComPtr<StubUserControl> UserControl;

    MockEventHandler<drawEventHandler_t> OnDraw;

    ClearColorFixture()
        : OnDraw(L"Draw")
    {
        DeviceContext = Make<MockD2DDeviceContext>();
        DeviceContext->ClearMethod.AllowAnyCall();
        DeviceContext->SetTransformMethod.AllowAnyCall();
        DeviceContext->SetDpiMethod.AllowAnyCall();
        DeviceContext->SetTextAntialiasModeMethod.AllowAnyCall();

        Adapter = TestAdapter<TRAITS>::Create(DeviceContext.Get());        

        Control = Make<control_t>(Adapter);

        UserControl = static_cast<StubUserControl*>(As<IUserControl>(Control).Get());
    }

    void Load();

    void RegisterOnDraw()
    {
        EventRegistrationToken ignoredToken;
        ThrowIfFailed(Control->add_Draw(OnDraw.Get(), &ignoredToken));
    }

    void RenderAnyNumberOfFrames();
};


inline void ClearColorFixture<CanvasControlTraits>::Load()
{
    UserControl->Resize(Size{ 100, 200 });
    ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
}


inline void ClearColorFixture<CanvasAnimatedControlTraits>::Load()
{
    // Set the control to variable timestep mode.  This means that
    // RenderAnyNumberOfFrames can trigger updates & renders without having to
    // fake time moving forwards.
    ThrowIfFailed(Control->put_IsFixedTimeStep(FALSE));

    UserControl->Resize(Size{ 100, 200 });
    ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));

    Adapter->DoChanged();
}


inline void ClearColorFixture<CanvasControlTraits>::RenderAnyNumberOfFrames()
{
    int anyNumberOfTimes = 5;
    for (auto i = 0; i < anyNumberOfTimes; ++i)
    {
        Adapter->RaiseCompositionRenderingEvent();
    }            
}

inline void ClearColorFixture<CanvasAnimatedControlTraits>::RenderAnyNumberOfFrames()
{
    int anyNumberOfTimes = 5;
    for (auto i = 0; i < anyNumberOfTimes; ++i)
    {
        Adapter->Tick();        
    }            
}


