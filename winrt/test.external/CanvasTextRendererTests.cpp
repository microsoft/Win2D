// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"


using namespace Windows::UI;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Text;

#if WINVER > _WIN32_WINNT_WINBLUE
    typedef Windows::Foundation::Numerics::float3x2 MatrixType;
    typedef Windows::Foundation::Numerics::float2 Vector2Type;
#else
    typedef Microsoft::Graphics::Canvas::Numerics::Matrix3x2 MatrixType;
    typedef Microsoft::Graphics::Canvas::Numerics::Vector2 Vector2Type;
#endif

static float const sc_fontSize = 15.0f;

ref class CustomTextRenderer sealed : public ICanvasTextRenderer
{
    Platform::Object^ m_expectedBrush;
    int m_drawGlyphRunCallCount;
    int m_drawStrikethroughCallCount;
    int m_drawUnderlineCallCount;
    int m_drawInlineObjectCount;
    bool m_continuousGlyphRun;

public:
    CustomTextRenderer(Platform::Object^ expectedBrush, bool continuousGlyphRun)
        : m_expectedBrush(expectedBrush)
        , m_drawGlyphRunCallCount(0)
        , m_drawStrikethroughCallCount(0)
        , m_drawUnderlineCallCount(0)
        , m_drawInlineObjectCount(0)
        , m_continuousGlyphRun(continuousGlyphRun)
    {}

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
        //
        // We don't assert that the baseline origin is equal to the 
        // text position we passed to DrawToTextRenderer, since they
        // aren't actually meant to be the same quantity. DWrite computes this
        // based on the font.
        //
        Assert::IsNotNull(fontFace);
        Assert::AreEqual(sc_fontSize, fontSize);
        Assert::AreEqual(m_continuousGlyphRun ? 4u : 2u, glyphs->Length); // Strikethrough/underline divides up the glyph run
        Assert::AreEqual(false, isSideways);
        Assert::AreEqual(0u, bidiLevel);
        Assert::IsTrue(ReferenceEquals(m_expectedBrush, brush));
        Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);

        m_drawGlyphRunCallCount++;
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
        Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);
        Assert::AreEqual(CanvasTextDirection::LeftToRightThenTopToBottom, textDirection);
        Assert::AreEqual(CanvasTextMeasuringMode::Natural, measuringMode);
        Assert::IsTrue(ReferenceEquals(m_expectedBrush, brush));

        m_drawStrikethroughCallCount++;
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
        Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);
        Assert::AreEqual(CanvasTextDirection::LeftToRightThenTopToBottom, textDirection);
        Assert::AreEqual(CanvasTextMeasuringMode::Natural, measuringMode);
        Assert::IsTrue(ReferenceEquals(m_expectedBrush, brush));

        m_drawUnderlineCallCount++;
    }

    virtual void DrawInlineObject(
        Vector2Type baselineOrigin,
        ICanvasTextInlineObject^ inlineObject,
        bool isSideways,
        bool isRightToLeft,
        Platform::Object^ brush,
        CanvasGlyphOrientation glyphOrientation)
    {
        m_drawInlineObjectCount++;
    }

    virtual property float Dpi {float get() { return 0; }}

    virtual property bool PixelSnappingDisabled {bool get() { return true; }}

    virtual property MatrixType Transform {MatrixType get() { return{ 1, 0, 0, 1, 0, 0 }; }}

    property int DrawGlyphRunCallCount {int get() { return m_drawGlyphRunCallCount; } }

    property int DrawStrikethroughCallCount {int get() { return m_drawStrikethroughCallCount; } }

    property int DrawUnderlineCallCount {int get() { return m_drawUnderlineCallCount; } }
};

TEST_CLASS(CanvasTextRendererTests)
{
    CanvasDevice^ m_device;

public:
    CanvasTextRendererTests()
        : m_device(ref new CanvasDevice())
    {
    }

    CanvasTextLayout^ CreateTextLayout()
    {
        auto format = ref new CanvasTextFormat();
        format->WordWrapping = CanvasWordWrapping::NoWrap;
        format->FontSize = sc_fontSize;

        return ref new CanvasTextLayout(m_device, L"Asdf", format, 0, 0);
    }

    TEST_METHOD(CanvasTextRenderer_CallbacksWork)
    {
        auto layout = CreateTextLayout();

        auto brush = ref new CanvasSolidColorBrush(m_device, Colors::Magenta);

        layout->SetStrikethrough(0, 2, true);
        layout->SetUnderline(2, 2, true);
        layout->SetBrush(0, 4, brush);

        auto textRenderer = ref new CustomTextRenderer(static_cast<ICanvasBrush^>(brush), false);

        layout->DrawToTextRenderer(textRenderer, { 5, 6 } );

        Assert::AreEqual(2, textRenderer->DrawGlyphRunCallCount);
        Assert::AreEqual(1, textRenderer->DrawStrikethroughCallCount);
        Assert::AreEqual(1, textRenderer->DrawUnderlineCallCount);
    }

    TEST_METHOD(CanvasTextRenderer_NonBrushDrawingEffect_SetUsingInterop)
    {
        for (int i = 0; i < 2; ++i)
        {
            auto layout = CreateTextLayout();

            auto dwriteTextLayout = GetWrappedResource<IDWriteTextLayout>(layout);

            ComPtr<IUnknown> drawingEffect;
            Platform::Object^ expectedBrush;

            if (i == 0)
            {
                // Pick some non-brush COM object to be a drawing effect. Device arbitrarily chosen here
                auto d2dDevice = GetWrappedResource<ID2D1Device>(m_device);
                drawingEffect = d2dDevice.Get();
                expectedBrush = nullptr;
            }
            else
            {
                // Pick some arbitrary IInspectable.
                auto strokeStyle = ref new CanvasStrokeStyle();
                drawingEffect = reinterpret_cast<IInspectable*>(strokeStyle);
                expectedBrush = strokeStyle;
            }

            DWRITE_TEXT_RANGE range;
            range.startPosition = 0;
            range.length = 4;
            dwriteTextLayout->SetDrawingEffect(drawingEffect.Get(), range);

            auto textRenderer = ref new CustomTextRenderer(expectedBrush, true);

            // Enable strikethrough/underline to exercise that all callbacks see the
            // correct brush.
            layout->SetStrikethrough(0, 4, true);
            layout->SetUnderline(0, 4, true);

            layout->DrawToTextRenderer(textRenderer, 0, 0);

            Assert::AreEqual(1, textRenderer->DrawGlyphRunCallCount);
            Assert::AreEqual(1, textRenderer->DrawStrikethroughCallCount);
            Assert::AreEqual(1, textRenderer->DrawUnderlineCallCount);
        }
    }

    TEST_METHOD(CanvasTextRenderer_NonBrushDrawingEffect_SetCustomBrush)
    {
        auto layout = CreateTextLayout();

        auto dwriteTextLayout = GetWrappedResource<IDWriteTextLayout>(layout);

        ComPtr<IUnknown> drawingEffect;

        // Pick some arbitrary IInspectable to be a custom brush.
        auto strokeStyle = ref new CanvasStrokeStyle();
        layout->SetCustomBrush(0, 4, strokeStyle);

        auto textRenderer = ref new CustomTextRenderer(strokeStyle, true);

        // Enable strikethrough/underline to exercise that all callbacks see the
        // correct brush.
        layout->SetStrikethrough(0, 4, true);
        layout->SetUnderline(0, 4, true);

        layout->DrawToTextRenderer(textRenderer, 0, 0);

        Assert::AreEqual(1, textRenderer->DrawGlyphRunCallCount);
        Assert::AreEqual(1, textRenderer->DrawStrikethroughCallCount);
        Assert::AreEqual(1, textRenderer->DrawUnderlineCallCount);
    }
};
