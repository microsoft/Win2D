// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::UI;

TEST_CLASS(CanvasBrushTests)
{
    TEST_METHOD(CanvasSolidColorBrush_PropertiesAndClose)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        
        CanvasSolidColorBrush^ canvasSolidColorBrush = ref new CanvasSolidColorBrush(
            canvasDevice, 
            Windows::UI::Colors::Aquamarine
            );

        Assert::AreEqual(Windows::UI::Colors::Aquamarine, canvasSolidColorBrush->Color);
        canvasSolidColorBrush->Color = Windows::UI::Colors::Blue;
        Assert::AreEqual(Windows::UI::Colors::Blue, canvasSolidColorBrush->Color);

        Assert::AreEqual(1.0f, canvasSolidColorBrush->Opacity);
        canvasSolidColorBrush->Opacity = 0.8f;
        Assert::AreEqual(0.8f, canvasSolidColorBrush->Opacity);

        float3x2 identity = { 1, 0, 0, 1, 0, 0 };
        float3x2 scaleAndTranslate = { 3, 0, 0, 3, -4, -2 };
        Assert::AreEqual<float3x2>(identity, canvasSolidColorBrush->Transform);
        canvasSolidColorBrush->Transform = scaleAndTranslate;
        Assert::AreEqual<float3x2>(scaleAndTranslate, canvasSolidColorBrush->Transform);

        delete canvasSolidColorBrush;

        ExpectObjectClosed([&]{ canvasSolidColorBrush->Color; });
        ExpectObjectClosed([&]{ canvasSolidColorBrush->Color = Windows::UI::Colors::Orange; });

        ExpectObjectClosed([&]{ canvasSolidColorBrush->Opacity; });
        ExpectObjectClosed([&]{ canvasSolidColorBrush->Opacity = 3; });

        ExpectObjectClosed([&]{ canvasSolidColorBrush->Transform; });
        ExpectObjectClosed([&]{ canvasSolidColorBrush->Transform = identity; });

        // Test invalid creation parameters.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            []
            {
                CanvasSolidColorBrush^ invalidBrush = ref new CanvasSolidColorBrush(nullptr, Windows::UI::Colors::White);
            });
    }

    TEST_METHOD(CanvasSolidColorBrush_Interop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        ComPtr<ID2D1Device> device;
        context->GetDevice(&device);

        D2D1_COLOR_F d2dRed = D2D1::ColorF(1, 0, 0);

        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(context->CreateSolidColorBrush(d2dRed, &brush));

        auto canvasBrush = GetOrCreate<CanvasSolidColorBrush>(As<ID2D1Device1>(device).Get(), brush.Get());

        auto actualBrush = GetWrappedResource<ID2D1SolidColorBrush>(canvasBrush);

        Assert::AreEqual(brush.Get(), actualBrush.Get());
    }

    ComPtr<ID2D1GradientStopCollection1> CreateTestStopCollection(
        ID2D1DeviceContext1* context)
    {
        ComPtr<ID2D1GradientStopCollection1> stopCollection;
        D2D1_GRADIENT_STOP stops[3];
        stops[0] = { 0.1f, D2D1::ColorF(1, 0, 0) };
        stops[1] = { 0.4f, D2D1::ColorF(0, 1, 0) };
        stops[2] = { 0.7f, D2D1::ColorF(0, 0, 1) };

        ThrowIfFailed(context->CreateGradientStopCollection(
            stops,
            _countof(stops),
            D2D1_COLOR_SPACE_SRGB,
            D2D1_COLOR_SPACE_SRGB,
            D2D1_BUFFER_PRECISION_8BPC_UNORM,
            D2D1_EXTEND_MODE_MIRROR,
            D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED,
            &stopCollection));
        return stopCollection;
    }

    void VerifyCanvasStopsMatchTestStops(Platform::Array<CanvasGradientStop>^ stops)
    {
        Assert::AreEqual(3u, stops->Length);

        Assert::AreEqual(0.1f, stops[0].Position);
        Assert::AreEqual(0.4f, stops[1].Position);
        Assert::AreEqual(0.7f, stops[2].Position);

        Assert::AreEqual(ColorHelper::FromArgb(255, 255, 0, 0), stops[0].Color);
        Assert::AreEqual(ColorHelper::FromArgb(255, 0, 255, 0), stops[1].Color);
        Assert::AreEqual(ColorHelper::FromArgb(255, 0, 0, 255), stops[2].Color);
    }

    TEST_METHOD(CanvasLinearGradientBrush_ConstructionAndInterop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        ComPtr<ID2D1Device> device;
        context->GetDevice(&device);

        auto stopCollection = CreateTestStopCollection(context.Get());

        ComPtr<ID2D1LinearGradientBrush> d2dLinearGradientBrush;
        ThrowIfFailed(context->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(),
                D2D1::Point2F()),
            stopCollection.Get(),
            &d2dLinearGradientBrush));

        auto canvasLinearGradientBrushBrush =
            GetOrCreate<CanvasLinearGradientBrush>(As<ID2D1Device1>(device).Get(), d2dLinearGradientBrush.Get());

        auto actualBrush = GetWrappedResource<ID2D1LinearGradientBrush>(canvasLinearGradientBrushBrush);

        Assert::AreEqual(d2dLinearGradientBrush.Get(), actualBrush.Get());

        VerifyCanvasStopsMatchTestStops(canvasLinearGradientBrushBrush->Stops);
    }

    TEST_METHOD(CanvasRadialGradientBrush_ConstructionAndInterop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        ComPtr<ID2D1Device> device;
        context->GetDevice(&device);

        auto stopCollection = CreateTestStopCollection(context.Get());

        ComPtr<ID2D1RadialGradientBrush> d2dRadialGradientBrush;
        ThrowIfFailed(context->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(
                D2D1::Point2F(),
                D2D1::Point2F(),
                0,
                0),
            stopCollection.Get(),
            &d2dRadialGradientBrush));

        auto canvasRadialGradientBrushBrush =
            GetOrCreate<CanvasRadialGradientBrush>(As<ID2D1Device1>(device).Get(), d2dRadialGradientBrush.Get());

        auto actualBrush = GetWrappedResource<ID2D1RadialGradientBrush>(canvasRadialGradientBrushBrush);

        Assert::AreEqual(d2dRadialGradientBrush.Get(), actualBrush.Get());

        VerifyCanvasStopsMatchTestStops(canvasRadialGradientBrushBrush->Stops);
    }

    TEST_METHOD(CanvasGradientBrush_ThrowOnChannelIgnore)
    {
        // We expect this test to hit debug layer validation failures, so must run it without the debug layer.
        DisableDebugLayer disableDebug;

        auto device = ref new CanvasDevice();
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<CanvasGradientStop>^ gradientStopArray = ref new Platform::Array<CanvasGradientStop>(1);
                CanvasLinearGradientBrush^ linearGradientBrush = ref new CanvasLinearGradientBrush(
                    device,
                    gradientStopArray,
                    CanvasEdgeBehavior::Clamp,
                    CanvasAlphaMode::Ignore,
                    CanvasColorSpace::Srgb,
                    CanvasColorSpace::Srgb,
                    CanvasBufferPrecision::Precision8UIntNormalizedSrgb);
            });
    }

    TEST_METHOD(CanvasGradientBrush_Rainbow)
    {
        CanvasDevice^ device = ref new CanvasDevice();

        CanvasLinearGradientBrush^ linearGradientBrush = CanvasLinearGradientBrush::CreateRainbow(device, 0.0f);
        Assert::AreEqual(7u, linearGradientBrush->Stops->Length);
        Assert::IsTrue(linearGradientBrush->EdgeBehavior == CanvasEdgeBehavior::Clamp);

        CanvasRadialGradientBrush^ radialGradientBrush = CanvasRadialGradientBrush::CreateRainbow(device, 100.0f);
        Assert::AreEqual(7u, radialGradientBrush->Stops->Length);
        Assert::IsTrue(radialGradientBrush->EdgeBehavior == CanvasEdgeBehavior::Clamp);
    }
};
