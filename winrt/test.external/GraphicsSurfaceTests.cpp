// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(DirectX11SurfaceTests)
{
public:
    TEST_METHOD(RoundtripDxgiSurface)
    {
        ComPtr<ID3D11Texture2D> texture = CreateTexture2D();

        ComPtr<IDXGISurface> dxgiSurface;
        ThrowIfFailed(texture.As(&dxgiSurface));

        //
        // Create a DirectX11Surface based on this
        //
        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(dxgiSurface.Get());

        //
        // Convert it back to a DXGI surface
        //
        ComPtr<IDXGISurface> roundTrippedDxgiSurface;
        ThrowIfFailed(GetDXGIInterface(
            graphicsSurface,
            roundTrippedDxgiSurface.GetAddressOf()));

        Assert::AreEqual(dxgiSurface.Get(), roundTrippedDxgiSurface.Get());

        //
        // Convert it directly to a D3D texture
        //
        ComPtr<ID3D11Texture2D> roundTrippedTexture;
        ThrowIfFailed(GetDXGIInterface(
            graphicsSurface,
            roundTrippedTexture.GetAddressOf()));

        Assert::AreEqual(texture.Get(), roundTrippedTexture.Get());
    }

    TEST_METHOD(CreateDirectX11SurfaceFailsOnNullDxgiSurface)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            []() 
            { 
                CreateDirectX11Surface(nullptr);
            });
    }

    TEST_METHOD(DirectX11SurfaceImplementsClosableCorrectly)
    {
        //
        // Create a mock IDXGISurface implementation.  The token is used so we
        // can tell when the IDXGISurface instance is destroyed.
        //

        class DxgiSurfaceWithToken : public MockDxgiSurface
        {
            std::shared_ptr<bool> m_Token;

        public:
            DxgiSurfaceWithToken(std::shared_ptr<bool> token)
                : m_Token(token)
            {
            }
        };

        std::shared_ptr<bool> strongToken = std::make_shared<bool>(true);
        std::weak_ptr<bool> weakToken(strongToken);

        ComPtr<IDXGISurface> mockDxgiSurface = Make<DxgiSurfaceWithToken>(strongToken);
        strongToken.reset();

        Assert::IsFalse(weakToken.expired());

        //
        // Wrap a DirectX11Surface around mockDxgiSurface.  This takes ownership
        // of the mockDxgiSurface, so we release our reference to it.
        //
        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(mockDxgiSurface.Get());
        mockDxgiSurface.Reset();
        Assert::IsFalse(weakToken.expired());

        //
        // Now explicitly close the DirectX11Surface ('delete foo' in C++/CX
        // calls Close()).  This should release the token, making our weakToken
        // invalid.
        //
        delete graphicsSurface;
        Assert::IsTrue(weakToken.expired());

        //
        // Attempting to close a second time is a no-op
        //
        delete graphicsSurface;

        //
        // Attempting to call any other method gives RO_E_CLOSED
        //
#define EXPECT_OBJECT_CLOSED(CODE) Assert::ExpectException<Platform::ObjectDisposedException^>([&graphicsSurface]() { CODE; })

        EXPECT_OBJECT_CLOSED(graphicsSurface->Description);
        EXPECT_OBJECT_CLOSED(graphicsSurface->EvictionPriority);
        EXPECT_OBJECT_CLOSED(graphicsSurface->EvictionPriority = 1);

#undef EXPECT_OBJECT_CLOSED
    }

    TEST_METHOD(DirectX11SurfaceGetDescriptionCallsGetDesc)
    {
        //
        // Create a mock DXGI surface that tracks whether or not GetDesc is
        // called and can return a pre-canned DXGI_SURFACE_DESC.
        //

        ComPtr<MockDxgiSurface> mockDxgiSurface = Make<MockDxgiSurface>();

        bool getDescCalled = false;
        DXGI_SURFACE_DESC expectedDesc = {};
        expectedDesc.Width = 123;
        expectedDesc.Height = 456;
        expectedDesc.Format = DXGI_FORMAT_BC3_UNORM_SRGB; // 78
        expectedDesc.SampleDesc.Count = 9;
        expectedDesc.SampleDesc.Quality = 10;

        mockDxgiSurface->MockGetDesc = 
            [&]()
            {
                Assert::IsFalse(getDescCalled);
                getDescCalled = true;
                return expectedDesc;
            };

        //
        // Wrap a DirectX11Surface around it
        //
        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(mockDxgiSurface.Get());

        //
        // Get the Description property and compare that with what we expected.
        //
        DirectX11SurfaceDescription actualDesc = graphicsSurface->Description;

        Assert::IsTrue(getDescCalled);
        Assert::AreEqual(expectedDesc.Width, actualDesc.Width);
        Assert::AreEqual(expectedDesc.Height, actualDesc.Height);
        Assert::AreEqual(static_cast<int>(expectedDesc.Format), static_cast<int>(actualDesc.Format));
        Assert::AreEqual(expectedDesc.SampleDesc.Count, actualDesc.MultisampleDescription.Count);
        Assert::AreEqual(expectedDesc.SampleDesc.Quality, actualDesc.MultisampleDescription.Quality);
    }

    TEST_METHOD(DirectX11SurfaceGetDescriptionFailsIfPassedNull)
    {
        ComPtr<IDXGISurface> mockDxgiSurface = Make<MockDxgiSurface>();
        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(mockDxgiSurface.Get());

        ABI::Microsoft::Graphics::Canvas::IDirectX11Surface* rawDirectX11Surface =
            reinterpret_cast<ABI::Microsoft::Graphics::Canvas::IDirectX11Surface*>(graphicsSurface);

        Assert::AreEqual(E_INVALIDARG, rawDirectX11Surface->get_Description(nullptr));
    }

    TEST_METHOD(DirectX11SurfaceGetEvictionPriorityCallsDxgiResource)
    {
        ComPtr<MockDxgiSurface> mockDxgiSurface = Make<MockDxgiSurface>();

        bool getEvictionPriorityCalled = false;
        uint32_t expectedEvictionPriority = 123;

        mockDxgiSurface->MockGetEvictionPriority =
            [&]()
            {
                Assert::IsFalse(getEvictionPriorityCalled);
                getEvictionPriorityCalled = true;
                return expectedEvictionPriority;
            };

        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(mockDxgiSurface.Get());

        Assert::AreEqual(expectedEvictionPriority, graphicsSurface->EvictionPriority);
        Assert::IsTrue(getEvictionPriorityCalled);
    }

    TEST_METHOD(DirectX11SurfaceSetEvictionPriorityCallsDxgiResource)
    {
        ComPtr<MockDxgiSurface> mockDxgiSurface = Make<MockDxgiSurface>();

        bool setEvictionPriorityCalled = false;
        uint32_t expectedEvictionPriority = 123;
        uint32_t actualEvictionPriority = 0;

        mockDxgiSurface->MockSetEvictionPriority =
            [&](uint32_t value)
            {
                Assert::IsFalse(setEvictionPriorityCalled);
                setEvictionPriorityCalled = true;
                actualEvictionPriority = value;
            };

        DirectX11Surface^ graphicsSurface = CreateDirectX11Surface(mockDxgiSurface.Get());

        graphicsSurface->EvictionPriority = expectedEvictionPriority;
        Assert::IsTrue(setEvictionPriorityCalled);
        Assert::AreEqual(expectedEvictionPriority, actualEvictionPriority);            
    }

    TEST_METHOD(DirectX11SurfaceResourcePriorityPropertiesAreAvailable)
    {
        //
        // The various DXGI_RESOURCE_* constants are defined in the IDL to be
        // UINT, but midl emits these as #defines.  The constants aren't marked
        // up to be unsigned.
        //
        // The intent, though, is very much that these are unsigned values so
        // casting them is the right thing to do here.
        //
        // For example, the IDL:
        //
        //   const UINT DXGI_RESOURCE_PRIORITY_MAXIMUM = 0xc8000000;
        //
        // Ends up as:
        //
        //   #define	DXGI_RESOURCE_PRIORITY_MAXIMUM	( 0xc8000000 )
        //

        Assert::AreEqual(static_cast<uint32_t>(DXGI_RESOURCE_PRIORITY_MINIMUM) , DirectX11Surface::ResourcePriorityMinimum);
        Assert::AreEqual(static_cast<uint32_t>(DXGI_RESOURCE_PRIORITY_LOW)     , DirectX11Surface::ResourcePriorityLow);
        Assert::AreEqual(static_cast<uint32_t>(DXGI_RESOURCE_PRIORITY_NORMAL)  , DirectX11Surface::ResourcePriorityNormal);
        Assert::AreEqual(static_cast<uint32_t>(DXGI_RESOURCE_PRIORITY_HIGH)    , DirectX11Surface::ResourcePriorityHigh);
        Assert::AreEqual(static_cast<uint32_t>(DXGI_RESOURCE_PRIORITY_MAXIMUM) , DirectX11Surface::ResourcePriorityMaximum);
    }

private:
    ComPtr<ID3D11Texture2D> CreateTexture2D()
    {
        //
        // Create a texture that will be the surface that gets roundtripped
        //
        ComPtr<ID3D11Device> d3dDevice = CreateD3DDevice();
        
        CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R32G32B32_FLOAT, 1, 1);

        ComPtr<ID3D11Texture2D> texture;
        ThrowIfFailed(d3dDevice->CreateTexture2D(
            &textureDesc,
            nullptr,
            &texture));

        return texture;
    }
};
