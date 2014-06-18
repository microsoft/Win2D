// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

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

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Color; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Color = Windows::UI::Colors::Orange; });

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Opacity; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Opacity = 3; });

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Transform; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Transform = identity; });

        // Test invalid creation parameters.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            []()
            {
                CanvasSolidColorBrush^ invalidBrush = ref new CanvasSolidColorBrush(nullptr, Windows::UI::Colors::White);
            });
    }

    TEST_METHOD(CanvasSolidColorBrush_Interop)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(canvasDevice);

        ComPtr<ID2D1DeviceContext> context;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &context));

        D2D1_COLOR_F d2dRed = D2D1::ColorF(1, 0, 0);

        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(context->CreateSolidColorBrush(d2dRed, &brush));

        auto canvasBrush = GetOrCreate<CanvasSolidColorBrush>(brush.Get());

        auto actualBrush = GetWrappedResource<ID2D1SolidColorBrush>(canvasBrush);

        Assert::AreEqual(brush.Get(), actualBrush.Get());
    }
};
