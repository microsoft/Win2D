// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::UI;

TEST_CLASS(CanvasStrokeStyleTests)
{
    Windows::UI::Color m_anyColor;

public:
    CanvasStrokeStyleTests()
        : m_anyColor(Windows::UI::Color{1,2,3,4})
    {}

    TEST_METHOD(CanvasStrokeStyleTests_Construction)
    {
        CanvasStrokeStyle^ canvasStrokeStyle = ref new CanvasStrokeStyle();
    }

    TEST_METHOD(CanvasStrokeStyleTests_Interop)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();

        // Create a default stroke style, then interop Win2D -> D2D.
        auto strokeStyle = ref new CanvasStrokeStyle();

        auto d2dStrokeStyle = GetWrappedResource<ID2D1StrokeStyle1>(device1, strokeStyle);

        Assert::AreEqual(strokeStyle->MiterLimit, d2dStrokeStyle->GetMiterLimit());

        // Stroke style factory should match device1.
        ComPtr<ID2D1Factory> strokeStyleFactory, deviceFactory;

        d2dStrokeStyle->GetFactory(&strokeStyleFactory);
        GetWrappedResource<ID2D1Device>(device1)->GetFactory(&deviceFactory);

        Assert::AreEqual(strokeStyleFactory.Get(), deviceFactory.Get());

        // Repeated interop without changing the Win2D object should be idempotent.
        Assert::AreEqual(d2dStrokeStyle.Get(), GetWrappedResource<ID2D1StrokeStyle1>(device1, strokeStyle).Get());
        Assert::AreEqual(strokeStyle, GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get()));

        // If we change the Win2D object, interop should then return a new D2D instance.
        const float newMiterLimit = 1.234f;
        strokeStyle->MiterLimit = newMiterLimit;

        auto d2dStrokeStyleWithNewMiterLimit = GetWrappedResource<ID2D1StrokeStyle1>(device1, strokeStyle);

        Assert::AreNotEqual(d2dStrokeStyleWithNewMiterLimit.Get(), d2dStrokeStyle.Get());
        Assert::AreEqual(newMiterLimit, d2dStrokeStyleWithNewMiterLimit->GetMiterLimit());

        // Repeated interop should still be idempotent.
        Assert::AreEqual(d2dStrokeStyleWithNewMiterLimit.Get(), GetWrappedResource<ID2D1StrokeStyle1>(device1, strokeStyle).Get());
        Assert::AreEqual(strokeStyle, GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyleWithNewMiterLimit.Get()));

        // Interop onto a different device should return a new D2D instance.
        auto d2dStrokeStyleOnNewDevice = GetWrappedResource<ID2D1StrokeStyle1>(device2, strokeStyle);

        Assert::AreNotEqual(d2dStrokeStyleOnNewDevice.Get(), d2dStrokeStyle.Get());
        Assert::AreNotEqual(d2dStrokeStyleOnNewDevice.Get(), d2dStrokeStyleWithNewMiterLimit.Get());

        // Stroke style factory should now match device2.
        d2dStrokeStyleOnNewDevice->GetFactory(&strokeStyleFactory);
        GetWrappedResource<ID2D1Device>(device2)->GetFactory(&deviceFactory);

        Assert::AreEqual(strokeStyleFactory.Get(), deviceFactory.Get());

        // Repeated interop should still be idempotent.
        Assert::AreEqual(d2dStrokeStyleOnNewDevice.Get(), GetWrappedResource<ID2D1StrokeStyle1>(device2, strokeStyle).Get());
        Assert::AreEqual(strokeStyle, GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyleOnNewDevice.Get()));

        // D2D -> Win2D interop should be able to create new wrapper instances.
        auto strokeStyle2 = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());
        auto strokeStyle3 = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyleWithNewMiterLimit.Get());

        Assert::AreNotEqual(strokeStyle, strokeStyle2);
        Assert::AreNotEqual(strokeStyle, strokeStyle3);
        Assert::AreNotEqual(strokeStyle2, strokeStyle3);
        
        // But now the wrappers are created, repeated interop should return these existing copies.
        Assert::AreEqual(strokeStyle2, GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get()));
        Assert::AreEqual(strokeStyle3, GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyleWithNewMiterLimit.Get()));

        // If we change a Win2D object, then repeated interop from what used to be its D2D resource should create a new wrapper.
        strokeStyle2->EndCap = CanvasCapStyle::Triangle;

        auto strokeStyle4 = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());

        Assert::AreNotEqual(strokeStyle, strokeStyle4);
        Assert::AreNotEqual(strokeStyle2, strokeStyle4);
        Assert::AreNotEqual(strokeStyle3, strokeStyle4);

        // Failing to specify the device is not legal.
        ExpectCOMException(E_INVALIDARG, L"To unwrap this resource type, a device parameter must be passed to GetWrappedResource.",
            [&]
            {
                GetWrappedResource<ID2D1StrokeStyle1>(strokeStyle);
            });

        // Specifying arbitrary DPI values is fine and ignored.
        const float arbitraryDpi = 1234;

        Assert::AreEqual(d2dStrokeStyleOnNewDevice.Get(), GetWrappedResource<ID2D1StrokeStyle1>(device2, strokeStyle, arbitraryDpi).Get());
    }

    TEST_METHOD(CanvasStrokeStyleTests_Interop_FactoryMixing)
    {
        // This should yield an exception on deletion of the drawing session,
        // but right now it does not, seemingly due to a pre-existing issue.
        RunOnUIThread(
            [=]
            {
                // Create a drawing session on one factory.
                CanvasDrawingSession^ drawingSession;
                CanvasSolidColorBrush^ brush;
                {
                    auto canvasDevice = ref new CanvasDevice();
                    auto canvasImageSource = ref new CanvasImageSource(
                        canvasDevice,
                        1,
                        1,
                        DEFAULT_DPI);
                    brush = ref new CanvasSolidColorBrush(canvasDevice, Microsoft::UI::Colors::Red);
                    drawingSession = canvasImageSource->CreateDrawingSession(m_anyColor);
                }

                // ...Create a stroke style on another.
                CanvasStrokeStyle^ strokeStyle;
                {
                    ComPtr<ID2D1Factory1> d2dFactory;
                    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, d2dFactory.GetAddressOf()));
                    ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;
                    D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1();
                    ThrowIfFailed(d2dFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &d2dStrokeStyle));
                    strokeStyle = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());
                }

                float2 point{ 1, 1 };
                drawingSession->DrawLine(point, point, brush, 5, strokeStyle);

                delete drawingSession;
            });
    }

    TEST_METHOD(CanvasStrokeStyleTests_DrawForcesRealization)
    {
        RunOnUIThread(
            [=]
            {
                // Create a drawing session on one factory.
                CanvasDrawingSession^ drawingSession;
                CanvasSolidColorBrush^ brush;
                auto canvasDevice = ref new CanvasDevice();
                auto canvasImageSource = ref new CanvasImageSource(
                    canvasDevice,
                    1,
                    1,
                    DEFAULT_DPI);
                brush = ref new CanvasSolidColorBrush(canvasDevice, Microsoft::UI::Colors::Red);
                drawingSession = canvasImageSource->CreateDrawingSession(m_anyColor);

                CanvasStrokeStyle^ strokeStyle = ref new CanvasStrokeStyle();

                float2 point{ 1, 1 };
                drawingSession->DrawLine(point, point, brush, 5, strokeStyle);

                delete drawingSession;
            });
    }

    TEST_METHOD(CanvasStrokeStyleTests_Interop_DefaultDeviceContext)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(canvasDevice);

        ComPtr<ID2D1Factory> d2dFactoryBase;
        d2dDevice->GetFactory(&d2dFactoryBase);

        ComPtr<ID2D1Factory1> d2dFactory;
        ThrowIfFailed(d2dFactoryBase.As(&d2dFactory));

        ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;
        D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1();
        strokeStyleProperties.startCap = D2D1_CAP_STYLE_SQUARE;
        strokeStyleProperties.endCap = D2D1_CAP_STYLE_ROUND;
        strokeStyleProperties.dashCap = D2D1_CAP_STYLE_TRIANGLE;
        strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_BEVEL;
        strokeStyleProperties.miterLimit = 2;
        strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_CUSTOM;
        strokeStyleProperties.dashOffset = 6;
        strokeStyleProperties.transformType = D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE;
        float customDashArray[] = { 2, 4, 6, 8 };
        ThrowIfFailed(d2dFactory->CreateStrokeStyle(&strokeStyleProperties, customDashArray, 4, &d2dStrokeStyle));

        // Create a wrapper and ensure it has the correct properties.
        auto canvasStrokeStyle = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());

        Assert::AreEqual(CanvasCapStyle::Square, canvasStrokeStyle->StartCap);
        Assert::AreEqual(CanvasCapStyle::Round, canvasStrokeStyle->EndCap);
        Assert::AreEqual(CanvasCapStyle::Triangle, canvasStrokeStyle->DashCap);
        Assert::AreEqual(CanvasLineJoin::Bevel, canvasStrokeStyle->LineJoin);
        Assert::AreEqual(2.0f, canvasStrokeStyle->MiterLimit);

        // This getter won't ever return dash style CUSTOM, even if the wrapped resource had type CUSTOM.
        // Instead, by convention, it will return SOLID.
        Assert::AreEqual(CanvasDashStyle::Solid, canvasStrokeStyle->DashStyle);
        Assert::AreEqual(6.0f, canvasStrokeStyle->DashOffset);
        Assert::AreEqual(CanvasStrokeTransformBehavior::Hairline, canvasStrokeStyle->TransformBehavior);

        auto customDashStyle = canvasStrokeStyle->CustomDashStyle;
        Assert::AreEqual(4u, customDashStyle->Length);
        Assert::AreEqual(2.0f, customDashStyle[0]);
        Assert::AreEqual(4.0f, customDashStyle[1]);
        Assert::AreEqual(6.0f, customDashStyle[2]);
        Assert::AreEqual(8.0f, customDashStyle[3]);
    }

    TEST_METHOD(CanvasStrokeStyleTests_Interop_NativeDeviceContext)
    {
        ComPtr<ID2D1Factory1> d2dFactory;
        ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, d2dFactory.GetAddressOf()));

        ComPtr<ID3D11Device> d3dDevice = CreateD3DDevice();
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        ComPtr<ID2D1Device> d2dDevice;
        ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));

        ComPtr<ID2D1DeviceContext> d2dDeviceContext;
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
        auto drawingSession = GetOrCreate<CanvasDrawingSession>(d2dDeviceContext.Get());

        ComPtr<ID2D1SolidColorBrush> d2dSolidColorBrush;
        ThrowIfFailed(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0), &d2dSolidColorBrush));
        auto canvasBrush = GetOrCreate<CanvasSolidColorBrush>(As<ID2D1Device1>(d2dDevice).Get(), d2dSolidColorBrush.Get());

        ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;
        D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1();
        strokeStyleProperties.startCap = D2D1_CAP_STYLE_SQUARE;
        ThrowIfFailed(d2dFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &d2dStrokeStyle));
        auto canvasStrokeStyle = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());

        float2 point{ 0, 1 };
        drawingSession->DrawLine(point, point, canvasBrush, 5.0f, canvasStrokeStyle);

        delete drawingSession;
    }

    TEST_METHOD(CanvasStrokeStyle_CustomDashStyle)
    {
        //
        // This tests the following edge case:
        // A native stroke style has a non-custom D2D1_DASH_STYLE.
        // But it has a dash array. Stroke style is interopped.
        //
        // The Canvas stroke style should have the non-CUSTOM dash.
        // It should NOT have the dash array.
        //

        ComPtr<ID2D1Factory1> d2dFactory;
        ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, d2dFactory.GetAddressOf()));
        ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;
        D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1();
        strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_DASH;

        ThrowIfFailed(d2dFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &d2dStrokeStyle));

        CanvasStrokeStyle^ strokeStyle = GetOrCreate<CanvasStrokeStyle>(d2dStrokeStyle.Get());
        Assert::AreEqual(CanvasDashStyle::Dash, strokeStyle->DashStyle);
        
        auto dashArray = strokeStyle->CustomDashStyle;
        Assert::AreEqual(0u, dashArray->Length);
    }
};
