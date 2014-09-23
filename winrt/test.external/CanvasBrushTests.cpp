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

        Numerics::Matrix3x2 identity = { 1, 0, 0, 1, 0, 0 };
        Numerics::Matrix3x2 scaleAndTranslate = { 3, 0, 0, 3, -4, -2 };
        Assert::AreEqual(identity, canvasSolidColorBrush->Transform);
        canvasSolidColorBrush->Transform = scaleAndTranslate;
        Assert::AreEqual(scaleAndTranslate, canvasSolidColorBrush->Transform);

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

        D2D1_COLOR_F d2dRed = D2D1::ColorF(1, 0, 0);

        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(context->CreateSolidColorBrush(d2dRed, &brush));

        auto canvasBrush = GetOrCreate<CanvasSolidColorBrush>(brush.Get());

        auto actualBrush = GetWrappedResource<ID2D1SolidColorBrush>(canvasBrush);

        Assert::AreEqual(brush.Get(), actualBrush.Get());
    }

    TEST_METHOD(CanvasImageBrush_Construction_Interop)
    {        
        RunOnUIThread(
            []()
            {
                auto device = ref new CanvasDevice();
                auto imageSource = ref new CanvasImageSource(device, 1, 1);
                auto drawingSession = imageSource->CreateDrawingSession();

                ICanvasBitmap^ canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(device, L"Assets/imageTiger.jpg"));

                // Test both constructors, and creation through drawing session.
                CanvasImageBrush^ imageBrush;
                
                imageBrush = ref new CanvasImageBrush(drawingSession);

                imageBrush = ref new CanvasImageBrush(device, canvasBitmap);

                // Verify that the interop path fails.                
                Assert::ExpectException<Platform::InvalidCastException^>(
                    [&imageBrush]()
                    {
                        GetWrappedResource<ID2D1ImageBrush>(imageBrush);
                    });

                // Verify the other direction.
                ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();
                ComPtr<ID2D1ImageBrush> d2dImageBrush;
                ThrowIfFailed(context->CreateImageBrush(nullptr, D2D1::ImageBrushProperties(D2D1::RectF()), &d2dImageBrush));    
                    
                Assert::ExpectException<Platform::NotImplementedException^>(
                    [&d2dImageBrush]()
                    {
                        GetOrCreate<CanvasImageBrush>(d2dImageBrush.Get());
                    });
            });
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

        auto stopCollection = CreateTestStopCollection(context.Get());

        ComPtr<ID2D1LinearGradientBrush> d2dLinearGradientBrush;
        ThrowIfFailed(context->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(),
                D2D1::Point2F()),
            stopCollection.Get(),
            &d2dLinearGradientBrush));

        auto canvasLinearGradientBrushBrush =
            GetOrCreate<CanvasLinearGradientBrush>(d2dLinearGradientBrush.Get());

        auto actualBrush = GetWrappedResource<ID2D1LinearGradientBrush>(canvasLinearGradientBrushBrush);

        Assert::AreEqual(d2dLinearGradientBrush.Get(), actualBrush.Get());

        VerifyCanvasStopsMatchTestStops(canvasLinearGradientBrushBrush->Stops);
    }

    TEST_METHOD(CanvasRadialGradientBrush_ConstructionAndInterop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

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
            GetOrCreate<CanvasRadialGradientBrush>(d2dRadialGradientBrush.Get());

        auto actualBrush = GetWrappedResource<ID2D1RadialGradientBrush>(canvasRadialGradientBrushBrush);

        Assert::AreEqual(d2dRadialGradientBrush.Get(), actualBrush.Get());

        VerifyCanvasStopsMatchTestStops(canvasRadialGradientBrushBrush->Stops);
    }

    TEST_METHOD(CanvasGradientBrush_ThrowOnChannelIgnore)
    {
        auto device = ref new CanvasDevice();
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<CanvasGradientStop>^ gradientStopArray = ref new Platform::Array<CanvasGradientStop>(1);
                CanvasLinearGradientBrush^ linearGradientBrush = ref new CanvasLinearGradientBrush(
                    device,
                    gradientStopArray,
                    CanvasEdgeBehavior::Clamp,
                    CanvasAlphaBehavior::Ignore,
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

        CanvasRadialGradientBrush^ radialGradientBrush = CanvasRadialGradientBrush::CreateRainbow(device, 100.0f);
        Assert::AreEqual(7u, radialGradientBrush->Stops->Length);
    }
};
