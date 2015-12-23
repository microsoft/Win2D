// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace WinRTDirectX;

TEST_CLASS(Direct3DSurfaceInteropTests)
{
    CanvasDevice^ m_canvasDevice;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_canvasDevice = ref new CanvasDevice();
    }

    TEST_METHOD(CreateBitmapFromDirect3DSurface_RoundTripsDxgiSurface)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE);
        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        auto expectedDxgiSurface = GetDXGIInterface<IDXGISurface>(surface);
        auto actualDxgiSurface   = GetDXGIInterface<IDXGISurface>(bitmap);
        Assert::AreEqual(expectedDxgiSurface.Get(), actualDxgiSurface.Get());
    }

    TEST_METHOD(CreateBitmapFromDirect3DSurface_AlphaAndDpiAreSet)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE);
        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface, 35.0f, CanvasAlphaMode::Ignore);
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);

        VerifyDpiAndAlpha(d2dBitmap, 35.0f, D2D1_ALPHA_MODE_IGNORE);
    }

    TEST_METHOD(CreateBitmapFromNonRenderTargetDirect3DSurface_CreatesBitmap)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE);
        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        AssertTypeName<CanvasBitmap>(bitmap);
    }

    TEST_METHOD(CreateBitmapFromRenderTargetDirect3DSurface_CreatesRenderTarget)
    {
        auto surface = CreateSurface(D3D11_BIND_RENDER_TARGET);
        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        AssertTypeName<CanvasRenderTarget>(bitmap);
    }

    TEST_METHOD(CreateRenderTargetFromDirect3DSurface_RoundTripsDxgiSurface)
    {
        auto surface = CreateSurface(D3D11_BIND_RENDER_TARGET);
        auto renderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        auto expectedDxgiSurface = GetDXGIInterface<IDXGISurface>(surface);
        auto actualDxgiSurface   = GetDXGIInterface<IDXGISurface>(renderTarget);
        Assert::AreEqual(expectedDxgiSurface.Get(), actualDxgiSurface.Get());
    }

    TEST_METHOD(CreateRenderTargetFromNonRenderTargetDirect3DSurface_Fails)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasRenderTarget::CreateFromDirect3D11Surface(m_canvasDevice, surface);
            });
    }

    TEST_METHOD(CreateRenderTargetFromRenderTargetDirect3DSurface_CreatesRenderTarget)
    {
        auto surface = CreateSurface(D3D11_BIND_RENDER_TARGET);
        auto renderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        AssertTypeName<CanvasRenderTarget>(renderTarget);
    }

    TEST_METHOD(CreateBitmapFromDirect3DSurface_Subresource)
    {
        //
        // Verify bitmaps and render targets are createable off of subresourced surfaces.
        //
        // Mip slices are used for this test rather than an arrayed texture
        // because they are supported on a wider range of hardware.
        //
        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(GetDXGIInterface<ID3D11Device>(m_canvasDevice, &d3dDevice));

        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = 8;
        textureDesc.Height = 8;
        textureDesc.MipLevels = 3;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0;

        ComPtr<ID3D11Texture2D> texture2D;
        ThrowIfFailed(d3dDevice->CreateTexture2D(
            &textureDesc,
            nullptr,
            &texture2D));

        ComPtr<IDXGIResource1> resource;
        ThrowIfFailed(texture2D.As(&resource));

        ComPtr<IDXGISurface2> surface;
        ThrowIfFailed(resource->CreateSubresourceSurface(2, &surface));

        auto wrappedSurface = CreateDirect3DSurface(surface.Get());

        auto renderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(m_canvasDevice, wrappedSurface);

        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, wrappedSurface);

        // Verify each resource size reflects the slice size, not the whole texture.
        Assert::AreEqual(2u, renderTarget->SizeInPixels.Width);
        Assert::AreEqual(2u, renderTarget->SizeInPixels.Height);
        Assert::AreEqual(2u, bitmap->SizeInPixels.Width);
        Assert::AreEqual(2u, bitmap->SizeInPixels.Height);
    }

    TEST_METHOD(CreateRenderTargetFromDirect3DSurface_AlphaAndDpiAreSet)
    {
        auto surface = CreateSurface(D3D11_BIND_RENDER_TARGET);
        auto renderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(m_canvasDevice, surface, 75.0f, CanvasAlphaMode::Ignore);
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(renderTarget);

        VerifyDpiAndAlpha(d2dBitmap, 75.0f, D2D1_ALPHA_MODE_IGNORE);
    }

    TEST_METHOD(CanvasBitmapDescriptionMatchesDxgiSurfaceDescription)
    {
        auto surface = CreateSurface(static_cast<D3D11_BIND_FLAG>(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET));
        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        auto canvasDescription = bitmap->Description;

        auto dxgiSurface = GetDXGIInterface<IDXGISurface>(bitmap);
        DXGI_SURFACE_DESC dxgiDesc{};
        ThrowIfFailed(dxgiSurface->GetDesc(&dxgiDesc));

        Assert::AreEqual<uint32_t>(dxgiDesc.Width, canvasDescription.Width);
        Assert::AreEqual<uint32_t>(dxgiDesc.Height, canvasDescription.Height);
        Assert::AreEqual<uint32_t>(dxgiDesc.Format, static_cast<uint32_t>(canvasDescription.Format));
        Assert::AreEqual<uint32_t>(dxgiDesc.SampleDesc.Count, canvasDescription.MultisampleDescription.Count);
        Assert::AreEqual<uint32_t>(dxgiDesc.SampleDesc.Quality, canvasDescription.MultisampleDescription.Quality);
    }

    IDirect3DSurface^ CreateSurface(
        D3D11_BIND_FLAG bindFlags,
        DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM,
        CanvasDevice^ device = nullptr)
    {
        if (!device)
            device = m_canvasDevice;

        //
        // We create a new DXGI surface by creating Texture2D's from the same
        // D3D device that CanvasDevice is bound to.  This way we know that D2D
        // and Win2D will have no prior-knowledge of the surface we create.
        //
        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(GetDXGIInterface<ID3D11Device>(device, &d3dDevice));

        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.MipLevels = 0;
        textureDesc.ArraySize = 1;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = bindFlags;
        textureDesc.CPUAccessFlags = 0;

        ComPtr<ID3D11Texture2D> texture2D;
        ThrowIfFailed(d3dDevice->CreateTexture2D(
            &textureDesc,
            nullptr,
            &texture2D));

        return CreateDirect3DSurface(As<IDXGISurface>(texture2D).Get());
    }

    void CreateBitmapFromDirect3DSurface_InfersAlphaIgnore_TestCase(DirectXPixelFormat format)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE, static_cast<DXGI_FORMAT>(format));
        auto bitmap1 = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface);

        Assert::AreEqual(CanvasAlphaMode::Ignore, bitmap1->AlphaMode);

        auto bitmap2 = CanvasBitmap::CreateFromDirect3D11Surface(m_canvasDevice, surface, 100.0f);

        Assert::AreEqual(CanvasAlphaMode::Ignore, bitmap2->AlphaMode);
    }

    TEST_METHOD(CreateBitmapFromDirect3DSurface_InfersAlphaIgnoreOnEligibleFormats)
    {
        CreateBitmapFromDirect3DSurface_InfersAlphaIgnore_TestCase(DirectXPixelFormat::B8G8R8X8UIntNormalized);
        CreateBitmapFromDirect3DSurface_InfersAlphaIgnore_TestCase(DirectXPixelFormat::R8G8UIntNormalized);
        CreateBitmapFromDirect3DSurface_InfersAlphaIgnore_TestCase(DirectXPixelFormat::R8UIntNormalized);
    }

    void CreateBitmapFromDirect3DSurface_DoesNotInferAlphaMode_IfWrongAlphaModeIsExplicitlySpecified_TestCase(CanvasDevice^ device, DirectXPixelFormat format)
    {
        auto surface = CreateSurface(D3D11_BIND_SHADER_RESOURCE, static_cast<DXGI_FORMAT>(format), device);
        
        // The bad alpha mode should fall through and get validated by D2D.
        Assert::ExpectException<Platform::COMException^>(
            [&]
            {
                CanvasBitmap::CreateFromDirect3D11Surface(device, surface, 96.0f, CanvasAlphaMode::Premultiplied);
            });
    }

    TEST_METHOD(CreateBitmapFromDirect3DSurface_DoesNotInferAlphaMode_IfWrongAlphaModeIsExplicitlySpecified)
    {
        // We expect this test to hit debug layer validation failures, so must run it without the debug layer.
        DisableDebugLayer disableDebug;

        auto device = ref new CanvasDevice();

        CreateBitmapFromDirect3DSurface_DoesNotInferAlphaMode_IfWrongAlphaModeIsExplicitlySpecified_TestCase(device, DirectXPixelFormat::B8G8R8X8UIntNormalized);
        CreateBitmapFromDirect3DSurface_DoesNotInferAlphaMode_IfWrongAlphaModeIsExplicitlySpecified_TestCase(device, DirectXPixelFormat::R8G8UIntNormalized);
        CreateBitmapFromDirect3DSurface_DoesNotInferAlphaMode_IfWrongAlphaModeIsExplicitlySpecified_TestCase(device, DirectXPixelFormat::R8UIntNormalized);
    }
};
