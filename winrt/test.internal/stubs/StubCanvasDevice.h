// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockD2DRectangleGeometry.h"
#include "mocks/MockD2DGeometryRealization.h"
#include "mocks/MockD2DGradientMesh.h"

namespace canvas
{
    //
    // The stub CanvasDevice provides basic functionality to allow it to be used
    // in tests that don't care about interactions with CanvasDevice.
    //
    class StubCanvasDevice : public MockCanvasDevice
    {
        ComPtr<ID2D1Device1> m_d2DDevice;
        ComPtr<MockD3D11Device> m_d3dDevice;
        ComPtr<MockEventSource<DeviceLostHandlerType>> m_deviceLostEventSource;
        DeviceContextPool m_deviceContextPool;
        
    public:
        StubCanvasDevice(ComPtr<ID2D1Device1> device = Make<StubD2DDevice>(), ComPtr<MockD3D11Device> d3dDevice = nullptr)
            : m_d2DDevice(device)
            , m_d3dDevice(d3dDevice)
            , m_deviceLostEventSource(Make<MockEventSource<DeviceLostHandlerType>>(L"DeviceLost"))
            , m_deviceContextPool(m_d2DDevice.Get())
        {
            GetInterfaceMethod.AllowAnyCall();
            
            CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
                [=]
                {
                    ComPtr<ID2D1DeviceContext1> dc;
                    ThrowIfFailed(m_d2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc));
                    return dc;
                });

            CreateRectangleGeometryMethod.AllowAnyCall(
                [](D2D1_RECT_F const&)
                {
                    return Make<MockD2DRectangleGeometry>();
                });

            CreateFilledGeometryRealizationMethod.AllowAnyCall(
                [=](ID2D1Geometry*, FLOAT)
                {
                    return Make<MockD2DGeometryRealization>();
                });

            CreateStrokedGeometryRealizationMethod.AllowAnyCall(
                [=](ID2D1Geometry*, FLOAT, ID2D1StrokeStyle*, FLOAT)
                {
                    return Make<MockD2DGeometryRealization>();
                });

            GetResourceCreationDeviceContextMethod.AllowAnyCall(
                [=]
                {
                    return m_deviceContextPool.TakeLease();
                });

            GetPrimaryDisplayOutputMethod.AllowAnyCall(
                [=]
                {
                    auto mockDxgiOutput = Make<MockDxgiOutput>();
                    mockDxgiOutput->WaitForVBlankMethod.AllowAnyCall();
                    return mockDxgiOutput;
                });

            add_DeviceLostMethod.AllowAnyCall(
                [=](DeviceLostHandlerType* value, EventRegistrationToken* token)
                {
                    return m_deviceLostEventSource->add_Event(value, token);
                });

            remove_DeviceLostMethod.AllowAnyCall(
                [=](EventRegistrationToken token)
                {
                    return m_deviceLostEventSource->remove_Event(token);
                });

            RaiseDeviceLostMethod.AllowAnyCall(
                [=]
                {
                    return m_deviceLostEventSource->InvokeAll(this, nullptr);
                });

            IsDeviceLostMethod.AllowAnyCall(
                [=](int, boolean* out)
                {
                    *out = false;
                    return S_OK;
                });

#if WINVER > _WIN32_WINNT_WINBLUE
            CreateGradientMeshMethod.AllowAnyCall(
                [=](D2D1_GRADIENT_MESH_PATCH const*, UINT32)
                {
                    return Make<MockD2DGradientMesh>();
                });
#endif
        }

        void MarkAsLost()
        {
            auto d3dDevice = GetDXGIInterfaceFromResourceCreator<ID3D11Device>(this);
            auto mockD3DDevice = static_cast<MockD3D11Device*>(d3dDevice.Get());
            mockD3DDevice->GetDeviceRemovedReasonMethod.AllowAnyCall(
                [] { return DXGI_ERROR_DEVICE_REMOVED; });
        }

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            return m_d2DDevice;
        }

        IFACEMETHODIMP get_Device(ICanvasDevice** value) override
        {
            ComPtr<ICanvasDevice> device(this);
            return device.CopyTo(value);
        }

        IFACEMETHODIMP GetInterface(REFIID iid, void** p) override
        {
            HRESULT hr = __super::GetInterface(iid, p);
            if (SUCCEEDED(hr) && !*p)
            {
                if (!m_d3dDevice)
                    m_d3dDevice = Make<MockD3D11Device>();
                return m_d3dDevice.CopyTo(iid, p);
            }
            else
            {
                return hr;
            }
        }
    };
}
