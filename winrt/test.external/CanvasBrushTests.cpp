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

        Math::Matrix3x2 identity = { 1, 0, 0, 1, 0, 0 };
        Math::Matrix3x2 scaleAndTranslate = {3, 0, 0, 3, -4, -2};
        Assert::AreEqual(identity, canvasSolidColorBrush->Transform);
        canvasSolidColorBrush->Transform = scaleAndTranslate;
        Assert::AreEqual(scaleAndTranslate, canvasSolidColorBrush->Transform);

        Assert::AreEqual(canvasDevice, canvasSolidColorBrush->Device);

        delete canvasSolidColorBrush;

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Color; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Color = Windows::UI::Colors::Orange; });

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Opacity; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Opacity = 3; });

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Transform; });
        ExpectObjectClosed([&](){ canvasSolidColorBrush->Transform = identity; });

        ExpectObjectClosed([&](){ canvasSolidColorBrush->Device; });

        // Test invalid creation parameters.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            []()
            {
                CanvasSolidColorBrush^ invalidBrush = ref new CanvasSolidColorBrush(nullptr, Windows::UI::Colors::White);
            });
    }
};

#undef EXPECT_OBJECT_CLOSED