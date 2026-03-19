// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(CanvasGeometryTests)
{
    CanvasDevice^ m_device;

public:
    CanvasGeometryTests()
        : m_device(ref new CanvasDevice())
    {
    }

    TEST_METHOD(CanvasGeometry_InteropFailAfterClosure)
    {
        auto canvasGeometry = CanvasGeometry::CreateRectangle(m_device, Rect{});

        delete canvasGeometry;

        ExpectObjectClosed([&]{ GetWrappedResource<ID2D1RectangleGeometry>(canvasGeometry); });
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Rectangle)
    {
        ComPtr<ID2D1RectangleGeometry> originalRectangleGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(1, 2, 3, 4), &originalRectangleGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalRectangleGeometry.Get());

        auto retrievedRectangleGeometry = GetWrappedResource<ID2D1RectangleGeometry>(canvasGeometry);

        Assert::AreEqual(originalRectangleGeometry.Get(), retrievedRectangleGeometry.Get());

    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Ellipse)
    {
        ComPtr<ID2D1EllipseGeometry> originalEllipseGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(1, 2), 3, 4), &originalEllipseGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalEllipseGeometry.Get());

        auto retrievedEllipseGeometry = GetWrappedResource<ID2D1EllipseGeometry>(canvasGeometry);

        Assert::AreEqual(originalEllipseGeometry.Get(), retrievedEllipseGeometry.Get());
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_RoundedRectangle)
    {
        ComPtr<ID2D1RoundedRectangleGeometry> originalRoundedRectangleGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(1, 2, 3, 4), 5, 6), &originalRoundedRectangleGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalRoundedRectangleGeometry.Get());

        auto retrievedRoundedRectangleGeometry = GetWrappedResource<ID2D1RoundedRectangleGeometry>(canvasGeometry);

        Assert::AreEqual(originalRoundedRectangleGeometry.Get(), retrievedRoundedRectangleGeometry.Get());
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Path)
    {
        ComPtr<ID2D1PathGeometry> originalPathGeometry;

        GetD2DFactory()->CreatePathGeometry(&originalPathGeometry);

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalPathGeometry.Get());

        auto retrievedPathGeometry = GetWrappedResource<ID2D1PathGeometry>(canvasGeometry);

        Assert::AreEqual(originalPathGeometry.Get(), retrievedPathGeometry.Get());
    }

    TEST_METHOD(CanvasGeometry_ZeroSizedGeometryGroup_InteropWorks)
    {
        auto canvasGeometry = CanvasGeometry::CreateGroup(m_device, nullptr);

        auto d2dResource = GetWrappedResource<ID2D1GeometryGroup>(canvasGeometry);

        Assert::IsNotNull(d2dResource.Get());
    }

    TEST_METHOD(CanvasGeometry_ComputeFlatteningTolerance)
    {
        struct TestCase
        {
            float ZoomLevel;
            float Dpi;
            float3x2 Transform;
        } testCases[]
        {
            { 1, 96, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 1, 96, float3x2{ 0, 0, 0, 0, 0, 0 } },
            { 1, 96, float3x2{ 1, 0, 0, 0, 0, 0 } },
            { 1, 96, float3x2{ 0, 1, 0, 0, 0, 0 } },
            { 1, 96, float3x2{ 0, 0, 1, 0, 0, 0 } },
            { 1, 96, float3x2{ 0, 0, 0, 1, 0, 0 } },
            { 1, 96, float3x2{ -1, 0, 0, -1, 0, 0 } },
            { 1, 96, float3x2{ 0, 1, -1, 0, 0, 0 } },
            { 1, 96, float3x2{ 2, 0, 0, 3, 0, 0 } },
            { 1, 96, float3x2{ 3, 0, 0, 2, 0, 0 } },
            { 1, 96, float3x2{ -2, 0, 0, -3, 0, 0 } },
            { 1, 96, float3x2{ -3, 0, 0, -2, 0, 0 } },
            { 1, 96, float3x2{ 1, 1, 1, 1, 0, 0 } },
            { -1, 96, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 1, -96, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 2, 96, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 1, 123, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 123.0f, 456.0f, float3x2{ 1, 2, 3, 4, 5, 6 } },
            { -1, -1, float3x2{ 1, 0, 0, 1, 0, 0 } },
            { 1, 0, float3x2{ 1, 0, 0, 1, 0, 0 } },
        };

        for (TestCase testCase : testCases)
        {
            const float d2dTolerance = D2D1::ComputeFlatteningTolerance(
                *(reinterpret_cast<D2D1_MATRIX_3X2_F*>(&testCase.Transform)),
                testCase.Dpi,
                testCase.Dpi,
                testCase.ZoomLevel);

            const float canvasTolerance = CanvasGeometry::ComputeFlatteningTolerance(
                testCase.Dpi,
                testCase.ZoomLevel,
                testCase.Transform);

            Assert::AreEqual(d2dTolerance, canvasTolerance);
        }

        for (TestCase testCase : testCases)
        {
            const float d2dTolerance = D2D1::ComputeFlatteningTolerance(
                D2D1::Matrix3x2F::Identity(),
                testCase.Dpi,
                testCase.Dpi,
                testCase.ZoomLevel);

            const float canvasTolerance = CanvasGeometry::ComputeFlatteningTolerance(
                testCase.Dpi,
                testCase.ZoomLevel);

            Assert::AreEqual(d2dTolerance, canvasTolerance);
        }
    }

    TEST_METHOD(CanvasGeometry_CreateCircle_NoDevice)
    {
        auto geometry1 = CanvasGeometry::CreateCircle(nullptr, 1, 2, 3);
        auto geometry2 = CanvasGeometry::CreateCircle(nullptr, float2(1, 2), 3);

        Assert::IsNull(geometry1->Device);
        Assert::IsNull(geometry2->Device);

        delete geometry1;
        delete geometry2;

        ExpectObjectClosed([&] { geometry1->Device; });
        ExpectObjectClosed([&] { geometry2->Device; });
    }

    TEST_METHOD(CanvasGeometry_CreateEllipse_NoDevice)
    {
        auto geometry1 = CanvasGeometry::CreateEllipse(nullptr, 1, 2, 3, 4);
        auto geometry2 = CanvasGeometry::CreateEllipse(nullptr, float2(1, 2), 3, 4);

        Assert::IsNull(geometry1->Device);
        Assert::IsNull(geometry2->Device);

        delete geometry1;
        delete geometry2;

        ExpectObjectClosed([&] { geometry1->Device; });
        ExpectObjectClosed([&] { geometry2->Device; });
    }

    TEST_METHOD(CanvasGeometry_CreateGlyphRun_NoDevice)
    {
        auto glyphArray = ref new Platform::Array<CanvasGlyph>(1);
        auto someFontFace = CanvasFontSet::GetSystemFontSet()->Fonts->GetAt(0);

        auto geometry = CanvasGeometry::CreateGlyphRun(
            nullptr, 
            float2{},
            someFontFace,
            16.0f,
            glyphArray,
            false,
            0,
            CanvasTextMeasuringMode::Natural,
            CanvasGlyphOrientation::Upright);

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }
    
    TEST_METHOD(CanvasGeometry_CreateGroup_NoDevice)
    {
        CanvasGeometry^ geometries[] =
        {
            CanvasGeometry::CreateEllipse(nullptr, 1, 2, 3, 4),
            CanvasGeometry::CreateRectangle(nullptr, Rect(1, 2, 3, 4))
        };

        auto geometry = CanvasGeometry::CreateGroup(nullptr, ref new Platform::Array<CanvasGeometry^>(geometries, _countof(geometries)));

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }

    TEST_METHOD(CanvasGeometry_CreatePolygon_NoDevice)
    {
        float2 points[] =
        {
            float2(1, 2),
            float2(3, 4),
            float2(5, 6)
        };

        auto geometry = CanvasGeometry::CreatePolygon(nullptr, ref new Platform::Array<float2>(points, _countof(points)));

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }

    TEST_METHOD(CanvasGeometry_CreateRectangle_NoDevice)
    {
        auto geometry1 = CanvasGeometry::CreateRectangle(nullptr, 1, 2, 3, 4);
        auto geometry2 = CanvasGeometry::CreateRectangle(nullptr, Rect(1, 2, 3, 4));

        Assert::IsNull(geometry1->Device);
        Assert::IsNull(geometry2->Device);

        delete geometry1;
        delete geometry2;

        ExpectObjectClosed([&] { geometry1->Device; });
        ExpectObjectClosed([&] { geometry2->Device; });
    }

    TEST_METHOD(CanvasGeometry_CreateRoundedRectangle_NoDevice)
    {
        auto geometry1 = CanvasGeometry::CreateRoundedRectangle(nullptr, 1, 2, 3, 4, 5, 6);
        auto geometry2 = CanvasGeometry::CreateRoundedRectangle(nullptr, Rect(1, 2, 3, 4), 5, 6);

        Assert::IsNull(geometry1->Device);
        Assert::IsNull(geometry2->Device);

        delete geometry1;
        delete geometry2;

        ExpectObjectClosed([&] { geometry1->Device; });
        ExpectObjectClosed([&] { geometry2->Device; });
    }

    TEST_METHOD(CanvasGeometry_Transform_NoDevice)
    {
        auto source = CanvasGeometry::CreateRectangle(nullptr, 1, 2, 3, 4);
        auto geometry = source->Transform(float3x2::identity());

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }

    TEST_METHOD(CanvasGeometry_Stroke_NoDevice)
    {
        auto source = CanvasGeometry::CreateRectangle(nullptr, 1, 2, 3, 4);
        auto geometry = source->Stroke(23);

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }

#ifdef WINUI3_SUPPORTS_INKING
    TEST_METHOD(CanvasGeometry_CreateInk_NoDevice_Fails)
    {
        using namespace Microsoft::UI::Input::Inking;

        auto inkStrokes = ref new Platform::Collections::Vector<InkStroke^>();

        // Ink processing requires a D2D device, so (unlike other geometry creation APIs)
        // CreateInk does not allow a null resource creator.
        Assert::ExpectException< Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasGeometry::CreateInk(nullptr, inkStrokes);
            });
    }
#endif

    TEST_METHOD(CanvasCachedGeometry_CreateFill_NoDevice_Fails)
    {
        auto geometry = CanvasGeometry::CreateRectangle(nullptr, 1, 2, 3, 4);

        Assert::IsNull(geometry->Device);

        Assert::ExpectException< Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasCachedGeometry::CreateFill(geometry);
            });
    }

    TEST_METHOD(CanvasCachedGeometry_CreateStroke_NoDevice_Fails)
    {
        auto geometry = CanvasGeometry::CreateRectangle(nullptr, 1, 2, 3, 4);

        Assert::IsNull(geometry->Device);

        Assert::ExpectException< Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasCachedGeometry::CreateFill(geometry);
            });
    }

    TEST_METHOD(CanvasPathBuilder_NoDevice)
    {        
        auto builder = ref new CanvasPathBuilder(nullptr);

        builder->BeginFigure(0, 0);
        builder->AddLine(1, 2);
        builder->AddLine(3, 4);
        builder->EndFigure(CanvasFigureLoop::Closed);

        auto geometry = CanvasGeometry::CreatePath(builder);

        Assert::IsNull(geometry->Device);

        delete geometry;

        ExpectObjectClosed([&] { geometry->Device; });
    }

    TEST_METHOD(CanvasPathBuilder_NoInterop)
    {
        // 
        // PathBuilder wraps an ID2D1GeometrySink, and ID2D1PathGeometry,
        // but interop for these types should not exist.
        //
        auto canvasPathBuilder = ref new CanvasPathBuilder(m_device);
        
        Assert::ExpectException< Platform::InvalidCastException^>(
            [=]
            {
                GetWrappedResource<ID2D1GeometrySink>(canvasPathBuilder);
            });
        
        Assert::ExpectException< Platform::InvalidCastException^>(
            [=]
            {
                GetWrappedResource<ID2D1PathGeometry1>(canvasPathBuilder);
            });
    }

    TEST_METHOD(CanvasCachedGeometry_NativeInterop)
    {
        ComPtr<ID2D1RectangleGeometry> rectangleGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(1, 2, 3, 4), &rectangleGeometry));

        auto d2dDevice = GetWrappedResource<ID2D1Device1>(m_device);

        ComPtr<ID2D1DeviceContext> d2dDeviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext));

        auto d2dDeviceContext1 = As<ID2D1DeviceContext1>(d2dDeviceContext);

        ComPtr<ID2D1GeometryRealization> d2dGeometryRealization;
        ThrowIfFailed(d2dDeviceContext1->CreateFilledGeometryRealization(
            rectangleGeometry.Get(), 
            D2D1_DEFAULT_FLATTENING_TOLERANCE, 
            &d2dGeometryRealization));

        auto canvasCachedGeometry = GetOrCreate<CanvasCachedGeometry>(m_device, d2dGeometryRealization.Get());

        auto retrievedGeometryRealization = GetWrappedResource<ID2D1GeometryRealization>(canvasCachedGeometry);

        Assert::AreEqual(d2dGeometryRealization.Get(), retrievedGeometryRealization.Get());
    }

    TEST_METHOD(CanvasCachedGeometry_NullStrokeStyle_ShouldFail)
    {
        auto canvasGeometry = CanvasGeometry::CreateRectangle(m_device, Rect{});

        Assert::ExpectException< Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasCachedGeometry::CreateStroke(canvasGeometry, 5.0f, nullptr);
            });
    }

    TEST_METHOD(CanvasGeometry_CreateText_NullArg)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasGeometry::CreateText(nullptr);
            });
    }

    TEST_METHOD(CanvasGeometry_CreateText_NoGlyphs_NothingBadHappens)
    {
        auto textFormat = ref new CanvasTextFormat();
        auto textLayout = ref new CanvasTextLayout(m_device, L"", textFormat, 0, 0);

        CanvasGeometry::CreateText(textLayout);
    }

    TEST_METHOD(CanvasGeometry_CreateGlyphRun_NullArg)
    {
        auto glyphArray = ref new Platform::Array<CanvasGlyph>(1);
        auto someFontFace = CanvasFontSet::GetSystemFontSet()->Fonts->GetAt(0);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasGeometry::CreateGlyphRun(
                    m_device,
                    float2{},
                    nullptr,
                    16.0f,
                    glyphArray,
                    false,
                    0,
                    CanvasTextMeasuringMode::Natural,
                    CanvasGlyphOrientation::Upright);
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [=]
            {
                CanvasGeometry::CreateGlyphRun(
                    m_device,
                    float2{},
                    someFontFace,
                    16.0f,
                    nullptr,
                    false,
                    0,
                    CanvasTextMeasuringMode::Natural,
                    CanvasGlyphOrientation::Upright);
            });
    }

private:
    ComPtr<ID2D1Factory> GetD2DFactory()
    {
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(m_device);

        ComPtr<ID2D1Factory> d2dFactory;
        d2dDevice->GetFactory(&d2dFactory);

        return d2dFactory;
    }
};
