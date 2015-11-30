// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Platform::Collections;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;

#if WINVER > _WIN32_WINNT_WINBLUE
	typedef Windows::Foundation::Numerics::float3x2 MatrixType;
	typedef Windows::Foundation::Numerics::float2 Vector2Type;
#else
	typedef Microsoft::Graphics::Canvas::Numerics::Matrix3x2 MatrixType;
	typedef Microsoft::Graphics::Canvas::Numerics::Vector2 Vector2Type;
#endif

#if WINVER > _WIN32_WINNT_WINBLUE
    typedef IDWriteFontFaceReference DWriteFontContainerType;
#else
    typedef IDWriteFont DWriteFontContainerType;
#endif

ref class GlyphRun sealed
{
    CanvasFontFace^ m_fontFace;
    Platform::Array<CanvasGlyph>^ m_glyphs;

public:
    GlyphRun(
        CanvasFontFace^ fontFace,
        Platform::Array<CanvasGlyph> const^ glyphs)
    {
        m_glyphs = ref new Platform::Array<CanvasGlyph>(glyphs);
        m_fontFace = fontFace;
    }

    CanvasFontFace^ GetFontFace() { return m_fontFace; }

    unsigned int GetGlyphCount() { return m_glyphs->Length; }

    CanvasGlyph GetGlyph(unsigned int index) { return m_glyphs[index]; }
};

TEST_CLASS(CanvasFontFaceTests)
{
    ref class GlyphRunListBuilder sealed : public ICanvasTextRenderer
    {
        Vector<GlyphRun^>^ m_list;

    public:

        GlyphRunListBuilder()
        {
            m_list = ref new Vector<GlyphRun^>();
        }

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
            GlyphRun^ glyphRun = ref new GlyphRun(fontFace, glyphs);
            m_list->Append(glyphRun);
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

        virtual property float Dpi {float get() { return 96; }}

        virtual property bool PixelSnappingDisabled {bool get() { return true; }}

        virtual property MatrixType Transform {MatrixType get() { return{ 1, 0, 0, 1, 0, 0 }; }}

        unsigned int GetGlyphRunCount() { return m_list->Size; }

        GlyphRun^ GetGlyphRun(unsigned int index) { return m_list->GetAt(index); }
    };

    GlyphRunListBuilder^ GetSimpleGlyphRunList()
    {
        CanvasDevice^ device = ref new CanvasDevice();
        CanvasTextFormat^ format = ref new CanvasTextFormat();
        format->FontFamily = L"Arial";
        auto layout = ref new CanvasTextLayout(device, L"123", format, 9999.0f, 0);

        auto listBuilder = ref new GlyphRunListBuilder();
        layout->DrawToTextRenderer(listBuilder, 0, 0);
        Assert::AreEqual(1u, listBuilder->GetGlyphRunCount());
        Assert::IsTrue(listBuilder->GetGlyphRun(0)->GetGlyphCount() > 0);

        return listBuilder;
    }

    TEST_METHOD(CanvasFontFace_DefaultFontFace_Panose)
    {
        auto listBuilder = GetSimpleGlyphRunList();

        auto panose = listBuilder->GetGlyphRun(0)->GetFontFace()->Panose;

        Assert::AreEqual(10u, panose->Length);

        //
        // PANOSE values, which are decided by the font author, are very unlikely
        // to change for a built-in font.
        //
        unsigned char expectedPanose[] = { 2, 11, 6, 4, 2, 2, 2, 2, 2, 4 };

        for (unsigned int i = 0; i < 10; ++i)
            Assert::AreEqual(panose[i], expectedPanose[i]);
    }

    ComPtr<DWriteFontContainerType> GetTestFont()
    {
        ComPtr<IDWriteFactory> factory;
        ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(factory), &factory));

        ComPtr<IDWriteFontCollection> dwriteSystemFontSet;
        ThrowIfFailed(factory->GetSystemFontCollection(&dwriteSystemFontSet));

        //
        // While we could arbitrarily pick the Nth font in the system font collection,
        // different environments could have malformed fonts or so forth,
        // so we choose Arial here.
        //

        const uint32_t familyCount = dwriteSystemFontSet->GetFontFamilyCount();
        for (uint32_t i = 0; i < familyCount; ++i)
        {
            ComPtr<IDWriteFontFamily> fontFamily;
            ThrowIfFailed(dwriteSystemFontSet->GetFontFamily(i, &fontFamily));

            ComPtr<IDWriteLocalizedStrings> localizedStrings;
            ThrowIfFailed(fontFamily->GetFamilyNames(&localizedStrings));

            uint32_t index;
            BOOL exists;
            ThrowIfFailed(localizedStrings->FindLocaleName(L"en-us", &index, &exists));

            if (!exists) continue;

            uint32_t familyNameLength;
            ThrowIfFailed(localizedStrings->GetStringLength(index, &familyNameLength));

            std::wstring familyName;
            familyName.resize(familyNameLength + 1);
            ThrowIfFailed(localizedStrings->GetString(index, &familyName[0], static_cast<uint32_t>(familyName.length())));

            familyName.pop_back(); // Pop off the null term

            if (familyName.compare(L"Arial") == 0)
            {
                ComPtr<IDWriteFont> font;
                ThrowIfFailed(fontFamily->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL, &font));

#if WINVER > _WIN32_WINNT_WINBLUE
                ComPtr<IDWriteFontFaceReference> fontFaceReference;
                ThrowIfFailed(As<IDWriteFont3>(font)->GetFontFaceReference(&fontFaceReference));
                return fontFaceReference;
#else
                return font;
#endif
            }
        }

        Assert::Fail();
        return nullptr;
    }


    TEST_METHOD(CanvasFontFace_Interop)
    {
        auto font = GetTestFont();

        auto wrapper = GetOrCreate<CanvasFontFace>(font.Get());

        auto unwrapped = GetWrappedResource<DWriteFontContainerType>(wrapper);

        Assert::AreEqual<void*>(font.Get(), unwrapped.Get());
    }
};
