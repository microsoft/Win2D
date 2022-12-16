// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using ABI::Microsoft::Graphics::Canvas::ICanvasImageInterop;
using ABI::Microsoft::Graphics::Canvas::WIN2D_GET_D2D_IMAGE_FLAGS;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::Devices::Enumeration;
using namespace Platform;

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

        IFACEMETHODIMP GetDevice(ABI::Microsoft::Graphics::Canvas::ICanvasDevice** device) override
        {
            return As<ICanvasImageInterop>(m_canvasImage.Get())->GetDevice(device);
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

        IGraphicsEffectSource^ effectSource = reinterpret_cast<ICanvasImage^>(As<ABI::Windows::Graphics::Effects::IGraphicsEffectSource>(wrappedEffect.Get()).Get());

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

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasDevice> canvasDevice;
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasResourceCreator> canvasResourceCreator;
        ThrowIfFailed(wrappedEffect->GetDevice(&canvasDevice));
        ThrowIfFailed(canvasDevice.CopyTo(IID_PPV_ARGS(&canvasResourceCreator)));

        // A canvas bitmap has an associated device from creation
        Assert::IsNotNull(canvasDevice.Get());
        Assert::IsNotNull(canvasResourceCreator.Get());

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

        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasDevice> canvasDevice;
        ThrowIfFailed(wrappedEffect->GetDevice(&canvasDevice));

        // An effect has no associated device until it's realized (and it isn't at this point yet)
        Assert::IsNull(canvasDevice.Get());

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
};
