// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

#if WINVER > _WIN32_WINNT_WINBLUE
typedef Windows::Foundation::Numerics::float3x2 MatrixType;
typedef Windows::Foundation::Numerics::float2 Vector2Type;
using namespace Microsoft::Graphics::Canvas::Svg;

#else
typedef Microsoft::Graphics::Canvas::Numerics::Matrix3x2 MatrixType;
typedef Microsoft::Graphics::Canvas::Numerics::Vector2 Vector2Type;
#endif

TEST_CLASS(CanvasDrawingSessionTests)
{
    //
    // CanvasDrawingSession interop is tested more directly in test.internal.
    // This exercises the published API.
    //
    TEST_METHOD(CanvasDrawingSession_Interop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());
        auto actualContext = GetWrappedResource<ID2D1DeviceContext1>(drawingSession);

        Assert::AreEqual(context.Get(), actualContext.Get());
    }

    //
    // Tests the projections of the state properties, and that they actually 
    // call through to D2D.
    //
    TEST_METHOD(CanvasDrawingSession_StateProperties)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        float3x2 someTransform = { 1, 2, 3, 4, 5, 6 };
        float3x2 identity = { 1, 0, 0, 1, 0, 0};

        context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        context->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_COPY);
        context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
        context->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(&someTransform));
        context->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());

        Assert::AreEqual(CanvasAntialiasing::Aliased, drawingSession->Antialiasing);
        Assert::AreEqual(CanvasBlend::Copy, drawingSession->Blend);
        Assert::AreEqual(CanvasTextAntialiasing::Aliased, drawingSession->TextAntialiasing);
        Assert::AreEqual<float3x2>(someTransform, drawingSession->Transform);
        Assert::AreEqual(CanvasUnits::Pixels, drawingSession->Units);

        drawingSession->Antialiasing = CanvasAntialiasing::Antialiased;
        drawingSession->Blend = CanvasBlend::SourceOver;
        drawingSession->TextAntialiasing = CanvasTextAntialiasing::Auto;
        drawingSession->Transform = identity;
        drawingSession->Units = CanvasUnits::Dips;

        Assert::AreEqual(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, context->GetAntialiasMode());
        Assert::AreEqual(D2D1_PRIMITIVE_BLEND_SOURCE_OVER, context->GetPrimitiveBlend());
        Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT, context->GetTextAntialiasMode());
        Assert::AreEqual(D2D1_UNIT_MODE_DIPS, context->GetUnitMode());

        float3x2 verifyTransform;
        context->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(&verifyTransform));
        Assert::AreEqual(identity, verifyTransform);
    }

    TEST_METHOD(CanvasDrawingSession_Interop_get_Device)
    {
        //
        // This verifes that the device retrieved from get_Device returns 
        // something which is actually compatible with the drawing session. 
        //
        CanvasDevice^ canvasDeviceA = ref new CanvasDevice();
        auto d2dDeviceA = GetWrappedResource<ID2D1Device1>(canvasDeviceA);
        ComPtr<ID2D1DeviceContext1> d2dDeviceContextA;
        ThrowIfFailed(d2dDeviceA->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2dDeviceContextA));

        auto drawingSessionA = GetOrCreate<CanvasDrawingSession>(d2dDeviceContextA.Get());

        CanvasDevice^ deviceB = drawingSessionA->Device;
        auto d2dDeviceB = GetWrappedResource<ID2D1Device1>(deviceB);

        Assert::AreEqual(d2dDeviceA.Get(), d2dDeviceB.Get());
    }

    TEST_METHOD(CanvasDrawingSession_TextRenderingParameters_DefaultIsNull)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());

        Assert::IsNull(drawingSession->TextRenderingParameters);

        // Ensure the setter doesn't mess up anything
        drawingSession->TextRenderingParameters = nullptr;
        Assert::IsNull(drawingSession->TextRenderingParameters);
    }

    ref class DefaultTextParametersLookup sealed : public ICanvasTextRenderer
    {
        CanvasFontFace^ m_fontFace;
        Platform::Array<CanvasGlyph> const^ m_glyphs;

    public:

        virtual void DrawGlyphRun(
            Vector2Type baselinePosition,
            CanvasFontFace^ fontFace,
            float fontSize,
            Platform::Array<CanvasGlyph> const^ glyphs,
            bool isSideways,
            unsigned int bidiLevel,
            Platform::Object^ brush,
            CanvasTextMeasuringMode,
            Platform::String^ locale,
            Platform::String^ text,
            Platform::Array<int> const^ clusterMap,
            unsigned int characterIndex,
            CanvasGlyphOrientation glyphOrientation)
        {
            m_fontFace = fontFace;
            m_glyphs = glyphs;
        }

        virtual void DrawStrikethrough(
            Vector2Type baselineOrigin,
            float width,
            float thickness,
            float offset,
            CanvasTextDirection textDirection,
            Platform::Object^ brush,
            CanvasTextMeasuringMode measuringMode,
            Platform::String^ locale,
            CanvasGlyphOrientation glyphOrientation)
        {
        }

        virtual void DrawUnderline(
            Vector2Type baselineOrigin,
            float width,
            float thickness,
            float offset,
            float runHeight,
            CanvasTextDirection textDirection,
            Platform::Object^ brush,
            CanvasTextMeasuringMode measuringMode,
            Platform::String^ locale,
            CanvasGlyphOrientation glyphOrientation)
        {
        }

        virtual void DrawInlineObject(
            Vector2Type baselineOrigin,
            ICanvasTextInlineObject^ inlineObject,
            bool isSideways,
            bool isRightToLeft,
            Platform::Object^ brush,
            CanvasGlyphOrientation glyphOrientation)
        {
        }

        virtual property float Dpi {float get() { return 0; }}

        virtual property bool PixelSnappingDisabled {bool get() { return true; }}

        virtual property MatrixType Transform {MatrixType get() { return{ 1, 0, 0, 1, 0, 0 }; }}

        CanvasFontFace^ GetFontFace()
        {
            return m_fontFace;
        }

        Platform::Array<CanvasGlyph> const^ GetGlyphs()
        {
            return m_glyphs;
        }
    };

    TEST_METHOD(CanvasDrawingSession_DrawGlyphRun_DefaultRenderer_NullBrush)
    {
        auto device = ref new CanvasDevice();

        auto d2dDevice = GetWrappedResource<ID2D1Device1>(device);

        ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2dDeviceContext));

        auto textFormat = ref new CanvasTextFormat();

        auto textLayout = ref new CanvasTextLayout(device, L"A", textFormat, 0, 0);

        //
        // Get reasonable values to use as default font face and set of CanvasGlyphs.
        //
        auto defaultTextParametersLookup = ref new DefaultTextParametersLookup();
        textLayout->DrawToTextRenderer(defaultTextParametersLookup, 0, 0);
        auto fontFace = defaultTextParametersLookup->GetFontFace();
        auto glyphs = defaultTextParametersLookup->GetGlyphs();

        Assert::IsNotNull(fontFace);
        Assert::IsNotNull(glyphs);

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(d2dDeviceContext.Get());
        
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                drawingSession->DrawGlyphRun(Vector2Type{}, fontFace, 1.0f, glyphs, false, 0, nullptr);
            });
    }


#if WINVER > _WIN32_WINNT_WINBLUE
    TEST_METHOD(CanvasDrawingSession_DrawSvgDocument_ZeroSizedViewportIsInvalid)
    {
        auto device = ref new CanvasDevice();

        auto d2dDevice = GetWrappedResource<ID2D1Device1>(device);

        ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2dDeviceContext));

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(d2dDeviceContext.Get());

        auto svgDocument = CanvasSvgDocument::LoadFromXml(device, "<svg/>");
        
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                drawingSession->DrawSvg(svgDocument, Size{});
            });
    }
#endif
};

