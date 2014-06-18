// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

TEST_CLASS(ConversionUnitTests)
{
    TEST_METHOD(Uint8_NormalizedFloat)
    {
        // Check some known values
        Assert::AreEqual(0.0f, ToNormalizedFloat(0));
        Assert::AreEqual(1.0f, ToNormalizedFloat(255));
        Assert::AreEqual(0.5f, ToNormalizedFloat(128), 0.1f);

        Assert::AreEqual<uint8_t>(0, NormalizedToUint8(0.0f));
        Assert::AreEqual<uint8_t>(255, NormalizedToUint8(1.0f));
        Assert::AreEqual<uint8_t>(127, NormalizedToUint8(0.5f));

        // Check roundtrip
        for (int i = 0; i < 256; ++i)
        {
            Assert::AreEqual<uint8_t>(i, NormalizedToUint8(ToNormalizedFloat(i)));
        }
    }

    TEST_METHOD(ColorToD2DColor)
    {
        using canvas::ToD2DColor;

        //
        // Check the components are passed through as expected, above tests
        // convince us that the per-component conversion is correct.
        //

        ABI::Windows::UI::Color alpha{255,  0,  0,  0};
        ABI::Windows::UI::Color red  {  0,255,  0,  0};
        ABI::Windows::UI::Color green{  0,  0,255,  0};
        ABI::Windows::UI::Color blue {  0,  0,  0,255};

        auto d2dAlpha = ToD2DColor(alpha);
        auto d2dRed   = ToD2DColor(red);
        auto d2dGreen = ToD2DColor(green);
        auto d2dBlue  = ToD2DColor(blue);

        Assert::AreEqual<float>(d2dAlpha.r, 0);
        Assert::AreEqual<float>(d2dAlpha.g, 0);
        Assert::AreEqual<float>(d2dAlpha.b, 0);
        Assert::AreEqual<float>(d2dAlpha.a, 1);

        Assert::AreEqual<float>(d2dRed.r, 1);
        Assert::AreEqual<float>(d2dRed.g, 0);
        Assert::AreEqual<float>(d2dRed.b, 0);
        Assert::AreEqual<float>(d2dRed.a, 0);

        Assert::AreEqual<float>(d2dGreen.r, 0);
        Assert::AreEqual<float>(d2dGreen.g, 1);
        Assert::AreEqual<float>(d2dGreen.b, 0);
        Assert::AreEqual<float>(d2dGreen.a, 0);

        Assert::AreEqual<float>(d2dBlue.r, 0);
        Assert::AreEqual<float>(d2dBlue.g, 0);
        Assert::AreEqual<float>(d2dBlue.b, 1);
        Assert::AreEqual<float>(d2dBlue.a, 0);
    }

    TEST_METHOD(D2DColorToWindowsColor)
    {
        using canvas::ToWindowsColor;

        ABI::Windows::UI::Color alpha{ 255, 0, 0, 0 };
        Assert::AreEqual(alpha, ToWindowsColor(D2D1::ColorF(0, 0, 0, 1)));

        ABI::Windows::UI::Color red{ 0, 255, 0, 0 };
        Assert::AreEqual(red, ToWindowsColor(D2D1::ColorF(1, 0, 0, 0)));

        ABI::Windows::UI::Color green{ 0, 0, 255, 0 };
        Assert::AreEqual(green, ToWindowsColor(D2D1::ColorF(0, 1, 0, 0)));

        ABI::Windows::UI::Color blue{ 0, 0, 0, 255 };
        Assert::AreEqual(blue, ToWindowsColor(D2D1::ColorF(0, 0, 1, 0)));

        // Verify truncation of high values.
        Assert::AreEqual(alpha, ToWindowsColor(D2D1::ColorF(0, 0, 0, 2)));
        Assert::AreEqual(red, ToWindowsColor(D2D1::ColorF(3, 0, 0, 0)));
        Assert::AreEqual(green, ToWindowsColor(D2D1::ColorF(0, 4, 0, 0)));
        Assert::AreEqual(blue, ToWindowsColor(D2D1::ColorF(0, 0, 5, 0)));

        // Verify truncation of low values.
        ABI::Windows::UI::Color black{ 0, 0, 0, 0 };
        Assert::AreEqual(black, ToWindowsColor(D2D1::ColorF(-6, 0, 0, 0)));
        Assert::AreEqual(black, ToWindowsColor(D2D1::ColorF(0, -7, 0, 0)));
        Assert::AreEqual(black, ToWindowsColor(D2D1::ColorF(0, 0, -8, 0)));
        Assert::AreEqual(black, ToWindowsColor(D2D1::ColorF(0, 0, 0, -9)));

    }

    TEST_METHOD(RectToRECT)
    {
        using ABI::Windows::Foundation::Rect;

        Assert::AreEqual(RECT{1,2,4,6}, ToRECT(Rect{1,2,3,4}));
        
        //
        // floats can losslessly store integers up to magnitude 2^24 (16777216).
        // This causes us a problem because VirtualImageSources may want to
        // specify regions with values up to 17000000 and we cannot reliably
        // convert Rect to RECT for these.
        //
        // ToRECT will fail with E_INVALIDARG if you attempt to convert to a
        // RECT with values outside of this range.
        //
        const uint32_t mv = 16777216;
        const float mvf = static_cast<float>(mv);
        Assert::AreEqual(static_cast<uint32_t>(mvf), mv);
        Assert::AreEqual(RECT{0,0,mv,mv}, ToRECT(Rect{0,0,mvf,mvf}));

        Assert::ExpectException<InvalidArgException>(
            [&]() { ToRECT(Rect{0,0,mvf+2,mvf}); });

        Assert::ExpectException<InvalidArgException>(
            [&]() { ToRECT(Rect{0,0,mvf,mvf+2}); });

        Assert::ExpectException<InvalidArgException>(
            [&]() { ToRECT(Rect{2,0,mvf,mvf}); });

        Assert::ExpectException<InvalidArgException>(
            [&]() { ToRECT(Rect{0,2,mvf,mvf}); });
    }

    TEST_METHOD(RectToD2DRect)
    {
        using ABI::Windows::Foundation::Rect;

        Assert::AreEqual(D2D_RECT_F{1,2,3,4}, ToD2DRect(Rect{1,2,2,2}));
    }

    TEST_METHOD(CanvasRoundedRectangleToD2DRoundedRect)
    {
        using ABI::Windows::Foundation::Rect;

        Assert::AreEqual(
            D2D1_ROUNDED_RECT{D2D_RECT_F{1,2,3,4},5,6},
            ToD2DRoundedRect(CanvasRoundedRectangle{Rect{1,2,2,2},5,6}));
    }

    TEST_METHOD(CanvasEllipseToD2DEllipse)
    {
        Assert::AreEqual(
            D2D1_ELLIPSE{D2D_POINT_2F{1,2},3,4},
            *ReinterpretAs<D2D1_ELLIPSE*>(&CanvasEllipse{ Point{ 1, 2 }, 3, 4 }));
    }
};

