// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas::DirectX::Direct3D11;
using namespace Microsoft::WRL;

TEST_CLASS(DeviceTests)
{
public:
    TEST_METHOD(RoundtripDxgiDevice)
    {
        //
        // Create a DXGI device from D3D
        //
        ComPtr<ID3D11Device> d3dDevice = CreateD3DDevice();
    
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        //
        // Create a Direct3DDevice based on this
        //
        Direct3DDevice^ graphicsDevice = CreateDirect3DDevice(dxgiDevice.Get());

        //
        // Convert it back to a DXGI device
        //
        ComPtr<IDXGIDevice> roundTrippedDxgiDevice;
        ThrowIfFailed(GetDXGIInterface(
            graphicsDevice,
            roundTrippedDxgiDevice.GetAddressOf()));

        Assert::AreEqual(dxgiDevice.Get(), roundTrippedDxgiDevice.Get());

        //
        // Convert it directly to a D3D device
        //
        ComPtr<ID3D11Device> roundTrippedD3DDevice;
        ThrowIfFailed(GetDXGIInterface(
            graphicsDevice,
            roundTrippedD3DDevice.GetAddressOf()));

        Assert::AreEqual(d3dDevice.Get(), roundTrippedD3DDevice.Get());
    }

    TEST_METHOD(CreateDirect3DDeviceFailsOnNullDxgiDevice)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            []()
            {
                CreateDirect3DDevice(nullptr);
            });
    }

    TEST_METHOD(Direct3DDeviceImplementsClosableCorrectly)
    {
        //
        // Create a mock IDXGIDevice implementation.  The token is used so
        // we can tell when the IDXGIDevice instance is destroyed.
        //

        class DxgiDeviceWithToken : public MockDxgiDevice
        {
            std::shared_ptr<bool> m_Token;

        public:
            DxgiDeviceWithToken(std::shared_ptr<bool> token)
                : m_Token(token)
            {
            }
        };

        ComPtr<IDXGIDevice> mockDxgiDevice;
        std::shared_ptr<bool> strongToken = std::make_shared<bool>(true);
        std::weak_ptr<bool> weakToken(strongToken);

        ThrowIfFailed(Make<DxgiDeviceWithToken>(strongToken).As(&mockDxgiDevice));
        strongToken.reset();

        Assert::IsFalse(weakToken.expired());

        //
        // Wrap a Direct3DDevice around mockDxgiDevice.  This takes ownership of
        // the mockDxgiDevice, so we release our reference to it.
        //
        Direct3DDevice^ graphicsDevice = CreateDirect3DDevice(mockDxgiDevice.Get());
        mockDxgiDevice.Reset();
        Assert::IsFalse(weakToken.expired());
        
        //
        // Now explicitly close the Direct3DDevice ('delete foo' in C++/CX calls
        // Close()).  This should release the token, making our weakToken
        // invalid.
        //
        delete graphicsDevice;
        Assert::IsTrue(weakToken.expired());

        //
        // Attempting to close a second time is a no-op
        //
        delete graphicsDevice;

        //
        // Attempting to call any other method gives RO_E_CLOSED
        //
        Assert::ExpectException<Platform::ObjectDisposedException^>(
            [&]()
            {
                graphicsDevice->Trim();
            });
    }

    TEST_METHOD(Direct3DDeviceCallsTrim)
    {
        //
        // Create a mock DXGI device that tracks whether or not trim was called.
        //

        ComPtr<MockDxgiDevice> mockDxgiDevice = Make<MockDxgiDevice>();

        bool trimCalled = false;
        mockDxgiDevice->MockTrim =
            [&trimCalled]()
            {
                Assert::IsFalse(trimCalled);
                trimCalled=true;
            };

        //
        // Wrap a Direct3DDevice around it
        //
        Direct3DDevice^ graphicsDevice = CreateDirect3DDevice(mockDxgiDevice.Get());

        //
        // Now call Direct3DDevice::Trim() and verify that Trim() was called on
        // the DXGI device.
        //
        graphicsDevice->Trim();
        Assert::IsTrue(trimCalled);
    }
};
