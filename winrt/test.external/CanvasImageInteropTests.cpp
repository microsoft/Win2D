// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop;
using ABI::Microsoft::Graphics::Canvas::WIN2D_GET_DEVICE_ASSOCIATION_TYPE;
using ABI::Microsoft::Graphics::Canvas::WIN2D_GET_D2D_IMAGE_FLAGS;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::Devices::Enumeration;
using namespace Platform;

// A test shader that has no inputs and just sets all output pixels to 0:
//
// #define D2D_INPUT_COUNT 0
// 
// #include "d2d1effecthelpers.hlsli"
// 
// D2D_PS_ENTRY(Execute)
// {
//     return (float4)0;
// }
static unsigned char SetAllOutputPixelsToZeroShaderBytecode[] = { 0x44, 0x58, 0x42, 0x43, 0x05, 0xA9, 0xCD, 0x8B, 0xD6, 0x46, 0x6B, 0xAE, 0x85, 0xD9, 0x55, 0x8D, 0xE2, 0xC7, 0x1A, 0x0C, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x04, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x14, 0x01, 0x00, 0x00, 0x54, 0x01, 0x00, 0x00, 0xD0, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0x08, 0x81, 0x04, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x31, 0x30, 0x2E, 0x31, 0x00, 0x49, 0x53, 0x47, 0x4E, 0x54, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00, 0x53, 0x43, 0x45, 0x4E, 0x45, 0x5F, 0x50, 0x4F, 0x53, 0x49, 0x54, 0x49, 0x4F, 0x4E, 0x00, 0xAB, 0x4F, 0x53, 0x47, 0x4E, 0x2C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5F, 0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xAB, 0xAB, 0x53, 0x48, 0x44, 0x52, 0x38, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x74, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x52, 0x49, 0x56, 0x33, 0x02, 0x00, 0x00, 0x44, 0x58, 0x42, 0x43, 0xD6, 0x7A, 0x9D, 0x4D, 0x69, 0x56, 0x64, 0xE1, 0x9D, 0x8E, 0x77, 0x5A, 0xDF, 0xB8, 0x3B, 0xB2, 0x01, 0x00, 0x00, 0x00, 0x33, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xD8, 0x01, 0x00, 0x00, 0x4C, 0x49, 0x42, 0x46, 0xA8, 0x01, 0x00, 0x00, 0x44, 0x58, 0x42, 0x43, 0xD6, 0xF8, 0x28, 0x3A, 0x98, 0x66, 0x1E, 0x29, 0x38, 0xA0, 0xF7, 0x67, 0x3C, 0x3A, 0x35, 0x7A, 0x01, 0x00, 0x00, 0x00, 0xA8, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x9C, 0x00, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x52, 0x44, 0x45, 0x46, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46, 0x4C, 0x08, 0x81, 0x04, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x52, 0x44, 0x31, 0x31, 0x3C, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x31, 0x30, 0x2E, 0x31, 0x00, 0x4C, 0x46, 0x53, 0x30, 0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x32, 0x44, 0x5F, 0x66, 0x75, 0x6E, 0x63, 0x5F, 0x65, 0x6E, 0x74, 0x72, 0x79, 0x00, 0xAB, 0x53, 0x48, 0x44, 0x52, 0x38, 0x00, 0x00, 0x00, 0x40, 0x00, 0xF0, 0xFF, 0x0E, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x08, 0xF2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x01, 0x53, 0x54, 0x41, 0x54, 0x74, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x49, 0x42, 0x48, 0x53, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4C, 0x53, 0x4C, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x72, 0x20, 0x31, 0x30, 0x2E, 0x31, 0x00, 0x44, 0x32, 0x44, 0x5F, 0x66, 0x75, 0x6E, 0x63, 0x5F, 0x65, 0x6E, 0x74, 0x72, 0x79, 0x00 };

TEST_CLASS(CanvasImageInteropTests)
{
    // This class is a thin wrapper around an input effect, which forwards all API calls directly to it.
    // It's used to validate that other Win2D APIs can accept an input that is only an ICanvasImageInterop.
    class WrappedEffect : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::ICanvasImage,
        ABI::Windows::Graphics::Effects::IGraphicsEffectSource,
        ABI::Windows::Foundation::IClosable,
        ICanvasImageInterop>
    {
        InspectableClass(L"EffectsAbi::IGraphicsEffectD2D1Interop::WrappedEffect", TrustLevel::BaseTrust);

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasImage> m_canvasImage;

    public:
        WrappedEffect(Microsoft::Graphics::Canvas::ICanvasImage^ canvasImage)
        {
            As<IUnknown>(canvasImage).CopyTo(&m_canvasImage);
        }

        // ICanvasImage

        IFACEMETHODIMP GetBounds(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            return m_canvasImage->GetBounds(resourceCreator, bounds);
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Numerics::Matrix3x2 transform,
            ABI::Windows::Foundation::Rect* bounds
        ) override
        {
            return m_canvasImage->GetBoundsWithTransform(resourceCreator, transform, bounds);
        }

        // IClosable

        IFACEMETHODIMP Close() override
        {
            return As<ABI::Windows::Foundation::IClosable>(m_canvasImage.Get())->Close();
        }

        // ICanvasImageInterop

        IFACEMETHODIMP GetDevice(ABI::Microsoft::Graphics::Canvas::ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type) override
        {
            return As<ICanvasImageInterop>(m_canvasImage.Get())->GetDevice(device, type);
        }

        IFACEMETHODIMP GetD2DImage(
            ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags,
            float targetDpi,
            float* realizeDpi,
            ID2D1Image** ppImage) override
        {
            return As<ICanvasImageInterop>(m_canvasImage.Get())->GetD2DImage(
                device,
                deviceContext,
                flags,
                targetDpi,
                realizeDpi,
                ppImage);
        }
    };

    TEST_METHOD(WrappedEffect_FromCanvasBitmap_CanvasDrawingSessionDrawImage)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto wrappedEffect = Make<WrappedEffect>(canvasBitmap);

        ICanvasImage^ canvasImage = reinterpret_cast<ICanvasImage^>(As<ABI::Microsoft::Graphics::Canvas::ICanvasImage>(wrappedEffect.Get()).Get());

        // Draw an image passing the wrapping effect implementing ICanvasImage externally
        ComPtr<ID2D1CommandList> d2dCommandList;
        ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
        deviceContext->SetTarget(d2dCommandList.Get());
        deviceContext->BeginDraw();
        drawingSession->DrawImage(canvasImage);
        ThrowIfFailed(deviceContext->EndDraw());
    }

    TEST_METHOD(WrappedEffect_FromCanvasBitmap_CanvasDrawingSessionDrawImage_OnRentedContext)
    {
        auto canvasDevice = ref new CanvasDevice();

        // Get an ID2D1DeviceContextLease object
        ComPtr<ABI::Microsoft::Graphics::Canvas::ID2D1DeviceContextLease> deviceContextLease;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ID2D1DeviceContextPool>(canvasDevice)->GetDeviceContextLease(&deviceContextLease));

        // Get the underlying context from the lease
        ComPtr<ID2D1DeviceContext> deviceContext;
        ThrowIfFailed(deviceContextLease->GetD2DDeviceContext(&deviceContext));

        // Same test as before, but using the rented device context
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(As<ID2D1DeviceContext1>(deviceContext.Get()).Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto wrappedEffect = Make<WrappedEffect>(canvasBitmap);

        ICanvasImage^ canvasImage = reinterpret_cast<ICanvasImage^>(As<ABI::Microsoft::Graphics::Canvas::ICanvasImage>(wrappedEffect.Get()).Get());

        ComPtr<ID2D1CommandList> d2dCommandList;
        ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
        deviceContext->SetTarget(d2dCommandList.Get());
        deviceContext->BeginDraw();
        drawingSession->DrawImage(canvasImage);
        ThrowIfFailed(deviceContext->EndDraw());
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_CanvasDrawingSessionDrawImage)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto blurEffect = ref new GaussianBlurEffect();

        blurEffect->Source = canvasBitmap;

        // Create a wrapper around GaussianBlurEffect to validate that ICanvasImageInterop also works correctly through a CanvasEffect-derived type
        auto wrappedEffect = Make<WrappedEffect>(blurEffect);

        ICanvasImage^ canvasImage = reinterpret_cast<ICanvasImage^>(As<ABI::Microsoft::Graphics::Canvas::ICanvasImage>(wrappedEffect.Get()).Get());

        ComPtr<ID2D1CommandList> d2dCommandList;
        ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
        deviceContext->SetTarget(d2dCommandList.Get());
        deviceContext->BeginDraw();
        drawingSession->DrawImage(canvasImage);
        ThrowIfFailed(deviceContext->EndDraw());
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_CanRealizeOnDifferentDevices_CanvasDrawingSessionDrawImage)
    {
        auto dummyShaderEffect = ref new PixelShaderEffect(ref new Array<unsigned char>(SetAllOutputPixelsToZeroShaderBytecode, sizeof(SetAllOutputPixelsToZeroShaderBytecode)));

        // Create a wrapper around the dummy effect (this is needed so the whole tree can be realized again, as there's no node bound to a device)
        auto wrappedEffect = Make<WrappedEffect>(dummyShaderEffect);

        // Draw the same effect twice (the first time it'll be realized on the device, the second time it'll unrealize and realize again on the new device)
        for (auto i = 0; i < 2; i++)
        {
            auto deviceContext = CreateTestD2DDeviceContext();
            auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

            ICanvasImage^ canvasImage = reinterpret_cast<ICanvasImage^>(As<ABI::Microsoft::Graphics::Canvas::ICanvasImage>(wrappedEffect.Get()).Get());

            ComPtr<ID2D1CommandList> d2dCommandList;
            ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
            deviceContext->SetTarget(d2dCommandList.Get());
            deviceContext->BeginDraw();
            drawingSession->DrawImage(canvasImage);
            ThrowIfFailed(deviceContext->EndDraw());
        }
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_CanRealizeOnDifferentDevicesWhenUsedAsEffectSource_CanvasDrawingSessionDrawImage)
    {
        auto dummyShaderEffect = ref new PixelShaderEffect(ref new Array<unsigned char>(SetAllOutputPixelsToZeroShaderBytecode, sizeof(SetAllOutputPixelsToZeroShaderBytecode)));

        // This test is the same as above, except this time the ICanvasImageInterop is used as a source for another effect.
        // Once again, this has to ensure that the root effect can handle cases where the interop effect is realized on a
        // different device. Since the device is not bound to a resource, it should still work and just unrealize/realize.
        auto wrappedEffect = Make<WrappedEffect>(dummyShaderEffect);

        auto blurEffect = ref new GaussianBlurEffect();

        blurEffect->Source = reinterpret_cast<ICanvasImage^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(wrappedEffect.Get()).Get());

        for (auto i = 0; i < 2; i++)
        {
            auto deviceContext = CreateTestD2DDeviceContext();
            auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

            ICanvasImage^ canvasImage = reinterpret_cast<ICanvasImage^>(As<ABI::Microsoft::Graphics::Canvas::ICanvasImage>(blurEffect).Get());

            ComPtr<ID2D1CommandList> d2dCommandList;
            ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
            deviceContext->SetTarget(d2dCommandList.Get());
            deviceContext->BeginDraw();
            drawingSession->DrawImage(canvasImage);
            ThrowIfFailed(deviceContext->EndDraw());
        }
    }

    TEST_METHOD(WrappedEffect_FromCanvasBitmap_AsOtherEffectSource_CanvasDrawingSessionDrawImage)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto wrappedEffect = Make<WrappedEffect>(canvasBitmap);

        auto blurEffect = ref new GaussianBlurEffect();

        IGraphicsEffectSource^ effectSource = reinterpret_cast<IGraphicsEffectSource^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(wrappedEffect.Get()).Get());

        // Assign the wrapper as source for an effect to validate that built-in effects can also accept external ICanvasImage objects
        blurEffect->Source = effectSource;

        ComPtr<ID2D1CommandList> d2dCommandList;
        ThrowIfFailed(deviceContext->CreateCommandList(&d2dCommandList));
        deviceContext->SetTarget(d2dCommandList.Get());
        deviceContext->BeginDraw();
        drawingSession->DrawImage(blurEffect);
        ThrowIfFailed(deviceContext->EndDraw());
    }

    TEST_METHOD(WrappedEffect_FromCanvasBitmap_GetBoundsForICanvasImageInterop)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto wrappedEffect = Make<WrappedEffect>(canvasBitmap);
        WIN2D_GET_DEVICE_ASSOCIATION_TYPE deviceType = WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED;

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasDevice> canvasDevice;
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator> canvasResourceCreator;
        ThrowIfFailed(wrappedEffect->GetDevice(&canvasDevice, &deviceType));
        ThrowIfFailed(canvasDevice.CopyTo(IID_PPV_ARGS(&canvasResourceCreator)));

        // A canvas bitmap has an associated device from creation
        Assert::IsNotNull(canvasDevice.Get());
        Assert::IsNotNull(canvasResourceCreator.Get());
        Assert::IsTrue(deviceType == WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE);

        auto canvasImageInterop = As<ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop>(canvasBitmap);

        ABI::Windows::Foundation::Rect rect;

        // Test the GetBoundsForICanvasImageInterop export with an external effect wrapping a CanvasBitmap
        ThrowIfFailed(ABI::Microsoft::Graphics::Canvas::GetBoundsForICanvasImageInterop(
            canvasResourceCreator.Get(),
            canvasImageInterop.Get(),
            nullptr,
            &rect));

        ABI::Windows::Foundation::Rect expectedBounds;
        expectedBounds.X = 0;
        expectedBounds.Y = 0;
        expectedBounds.Width = canvasBitmap->Size.Width;
        expectedBounds.Height = canvasBitmap->Size.Height;

        Assert::AreEqual(*reinterpret_cast<Rect*>(&expectedBounds), *reinterpret_cast<Rect*>(&rect));
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_GetBoundsForICanvasImageInterop)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto blurEffect = ref new GaussianBlurEffect();

        blurEffect->Source = canvasBitmap;
        blurEffect->BorderMode = EffectBorderMode::Hard;

        auto wrappedEffect = Make<WrappedEffect>(blurEffect);
        WIN2D_GET_DEVICE_ASSOCIATION_TYPE deviceType = WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED;

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasDevice> canvasDevice;
        ThrowIfFailed(wrappedEffect->GetDevice(&canvasDevice, &deviceType));

        // An effect has no associated device until it's realized (and it isn't at this point yet)
        Assert::IsNull(canvasDevice.Get());
        Assert::IsTrue(deviceType == WIN2D_GET_DEVICE_ASSOCIATION_TYPE::WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE);

        auto canvasImageInterop = As<ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop>(canvasBitmap);

        ABI::Windows::Foundation::Rect rect;

        // Test GetBoundsForICanvasImageInterop with an effect going through a second Win2D effect first
        ThrowIfFailed(ABI::Microsoft::Graphics::Canvas::GetBoundsForICanvasImageInterop(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator>(drawingSession).Get(),
            canvasImageInterop.Get(),
            nullptr,
            &rect));

        ABI::Windows::Foundation::Rect expectedBounds;
        expectedBounds.X = 0;
        expectedBounds.Y = 0;
        expectedBounds.Width = canvasBitmap->Size.Width;
        expectedBounds.Height = canvasBitmap->Size.Height;

        Assert::AreEqual(*reinterpret_cast<Rect*>(&expectedBounds), *reinterpret_cast<Rect*>(&rect));
    }

    TEST_METHOD(WrappedEffect_FromCanvasBitmap_InvalidateSourceRectangleForICanvasImageInterop_ThrowE_NOINTERFACE)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto wrappedEffect = Make<WrappedEffect>(canvasBitmap);

        Rect invalidRectangle = { 0, 0, 0, 0 };

        // This wrapped effect wraps a canvas bitmap, meaning when ICanvasImageInterop::GetD2DImage is called, the
        // resulting ID2D1Image will not also be an ID2D1Effect. In this case, an E_NOINTERFACE error is expected.
        HRESULT result = ABI::Microsoft::Graphics::Canvas::Effects::InvalidateSourceRectangleForICanvasImageInterop(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreatorWithDpi>(drawingSession).Get(),
            As<ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop>(wrappedEffect.Get()).Get(),
            0,
            reinterpret_cast<ABI::Windows::Foundation::Rect*>(&invalidRectangle));

        Assert::AreEqual(result, E_NOINTERFACE);
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_InvalidateSourceRectangleForICanvasImageInterop)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto blurEffect = ref new GaussianBlurEffect();

        blurEffect->Source = canvasBitmap;
        blurEffect->BorderMode = EffectBorderMode::Hard;

        auto wrappedEffect = Make<WrappedEffect>(blurEffect);

        ABI::Windows::Foundation::Rect invalidRectangle = { 0, 0, 0, 0 };

        // This test and the one below simply check that the C exports to support ICanvasEffect also work correctly
        // when an effect that only implements ICanvasImageInterop (and not ICanvasImageInternal) is passed. This is
        // just a sanity check, and the real tests with validation of all params/results are in the internal unit tests.
        ThrowIfFailed(ABI::Microsoft::Graphics::Canvas::Effects::InvalidateSourceRectangleForICanvasImageInterop(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreatorWithDpi>(drawingSession).Get(),
            As<ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop>(wrappedEffect.Get()).Get(),
            0,
            &invalidRectangle));
    }

    TEST_METHOD(WrappedEffect_FromCanvasEffect_GetInvalidRectanglesForICanvasImageInterop)
    {
        auto deviceContext = CreateTestD2DDeviceContext();
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(deviceContext.Get());

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            drawingSession->Device,
            ref new Platform::Array<Color>(256 * 256),
            256,
            256,
            96.0f,
            CanvasAlphaMode::Ignore);

        auto blurEffect = ref new GaussianBlurEffect();

        blurEffect->Source = canvasBitmap;
        blurEffect->BorderMode = EffectBorderMode::Hard;

        auto wrappedEffect = Make<WrappedEffect>(blurEffect);

        ComArray<ABI::Windows::Foundation::Rect> result;

        ThrowIfFailed(ABI::Microsoft::Graphics::Canvas::Effects::GetInvalidRectanglesForICanvasImageInterop(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreatorWithDpi>(drawingSession).Get(),
            As<ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop>(wrappedEffect.Get()).Get(),
            result.GetAddressOfSize(),
            result.GetAddressOfData()));
    }
};
