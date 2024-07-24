// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Windows::UI::Text;

    typedef IDWriteFontFaceReference DWriteFontReferenceType;
    typedef IDWriteFontFace3 DWriteFontFaceType;
    typedef IDWriteFontFace3 DWritePhysicalFontPropertyContainer;
    typedef DWRITE_RENDERING_MODE1 DWriteRenderingMode;
    
    class __declspec(uuid("0A165926-BCBD-4B02-BF60-F5FC46C22B58"))
    ICanvasFontFaceInternal : public IUnknown
    {
    public:
        virtual ComPtr<DWriteFontFaceType> const& GetRealizedFontFace() = 0;
    };

    class CanvasFontFace : RESOURCE_WRAPPER_RUNTIME_CLASS(
        DWriteFontReferenceType,
        CanvasFontFace,
        ICanvasFontFace,
        CloakedIid<ICanvasFontFaceInternal>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasFontFace, BaseTrust);

        ComPtr<DWriteFontFaceType> m_realizedFontFace;

    public:
        CanvasFontFace(DWriteFontReferenceType* fontFace);

        static ComPtr<ICanvasFontFace> GetOrCreate(IDWriteFontFace2* fontFaceInstance);

        IFACEMETHOD(GetRecommendedRenderingMode)(
            float fontSize,
            float dpi,
            CanvasTextMeasuringMode measuringMode,
            ICanvasTextRenderingParameters* renderingParameters,
            CanvasTextRenderingMode* renderingMode) override;

        IFACEMETHOD(GetRecommendedRenderingModeWithAllOptions)(
            float fontSize,
            float dpi,
            CanvasTextMeasuringMode measuringMode,
            ICanvasTextRenderingParameters* renderingParameters,
            Matrix3x2 transform,
            boolean isSideways,
            CanvasAntialiasing outlineThreshold,
            CanvasTextRenderingMode* renderingMode) override;

        IFACEMETHOD(GetRecommendedGridFit)(
            float fontSize,
            float dpi,
            CanvasTextMeasuringMode measuringMode,
            ICanvasTextRenderingParameters* renderingParameters,
            Matrix3x2 transform,
            boolean isSideways,
            CanvasAntialiasing outlineThreshold,
            CanvasTextGridFit* gridFit) override;

        IFACEMETHOD(get_GlyphBox)(Rect* value) override;

        IFACEMETHOD(get_SubscriptPosition)(Vector2* value) override;
        IFACEMETHOD(get_SubscriptSize)(Size* value) override;
        IFACEMETHOD(get_SuperscriptPosition)(Vector2* value) override;
        IFACEMETHOD(get_SuperscriptSize)(Size* value) override;

        IFACEMETHOD(get_HasTypographicMetrics)(boolean* value) override;

        IFACEMETHOD(get_Ascent)(float* value) override;
        IFACEMETHOD(get_Descent)(float* value) override;
        IFACEMETHOD(get_LineGap)(float* value) override;
        IFACEMETHOD(get_CapHeight)(float* value) override;
        IFACEMETHOD(get_LowercaseLetterHeight)(float* value) override;
        IFACEMETHOD(get_UnderlinePosition)(float* value) override;
        IFACEMETHOD(get_UnderlineThickness)(float* value) override;
        IFACEMETHOD(get_StrikethroughPosition)(float* value) override;
        IFACEMETHOD(get_StrikethroughThickness)(float* value) override;

        IFACEMETHOD(get_CaretSlopeRise)(float* value) override;
        IFACEMETHOD(get_CaretSlopeRun)(float* value) override;
        IFACEMETHOD(get_CaretOffset)(float* value) override;

        IFACEMETHOD(get_UnicodeRanges)(
            uint32_t* valueCount,
            CanvasUnicodeRange** valueElements) override;

        IFACEMETHOD(get_IsMonospaced)(boolean* value) override;

        IFACEMETHOD(GetVerticalGlyphVariants)(
            uint32_t inputCount,
            int* inputElements,
            uint32_t* outputCount,
            int** outputElements) override;

        IFACEMETHOD(get_HasVerticalGlyphVariants)(boolean* value) override;

        IFACEMETHOD(get_FileFormatType)(CanvasFontFileFormatType* value) override;

        IFACEMETHOD(get_Simulations)(CanvasFontSimulations* value) override;

        IFACEMETHOD(get_IsSymbolFont)(boolean* value) override;

        IFACEMETHOD(get_GlyphCount)(uint32_t* value) override;

        IFACEMETHOD(GetGlyphIndices)(
            uint32_t inputCount,
            uint32_t* inputElements,
            uint32_t* outputCount,
            int** outputElements) override;

        IFACEMETHOD(GetGlyphMetrics)(
          uint32_t inputCount,
          int* inputElements,
          boolean isSideways,
          uint32_t* outputCount,
          CanvasGlyphMetrics** outputElements) override;

        IFACEMETHOD(GetGdiCompatibleGlyphMetrics)(
          float fontSize,
          float dpi,
          Matrix3x2 transform,
          boolean useGdiNatural,
          uint32_t inputCount,
          int* inputElements,
          boolean isSideways,
          uint32_t* outputCount,
          CanvasGlyphMetrics** outputElements) override;

        IFACEMETHOD(get_Weight)(FontWeight* value) override;
        IFACEMETHOD(get_Stretch)(FontStretch* value) override;
        IFACEMETHOD(get_Style)(FontStyle* value) override;

        IFACEMETHOD(get_FamilyNames)(IMapView<HSTRING, HSTRING>** values) override;
        IFACEMETHOD(get_FaceNames)(IMapView<HSTRING, HSTRING>** values) override;

        IFACEMETHOD(GetInformationalStrings)(
            CanvasFontInformation fontInformation,
            IMapView<HSTRING, HSTRING>** values) override;

        IFACEMETHOD(HasCharacter)(uint32_t unicodeValue, boolean* value) override;
        
        IFACEMETHOD(GetGlyphRunBounds)(
            ICanvasDrawingSession* drawingSession,
            Vector2 point,
            float fontSize,
            uint32_t glyphCount,
            CanvasGlyph* glyphs,
            boolean isSideways,
            uint32_t bidiLevel,
            Rect* bounds) override;

        IFACEMETHOD(GetGlyphRunBoundsWithMeasuringMode)(
            ICanvasDrawingSession* drawingSession,
            Vector2 point,
            float fontSize,
            uint32_t glyphCount,
            CanvasGlyph* glyphs,
            boolean isSideways,
            uint32_t bidiLevel,
            CanvasTextMeasuringMode measuringMode,
            Rect* bounds) override;

        IFACEMETHOD(get_Panose)(uint32_t* valueCount, uint8_t** values) override;

        IFACEMETHOD(GetSupportedTypographicFeatureNames)(
            CanvasAnalyzedScript script,
            uint32_t* valueCount,
            CanvasTypographyFeatureName** valueElements) override;

        IFACEMETHOD(GetSupportedTypographicFeatureNamesWithLocale)(
            CanvasAnalyzedScript script,
            HSTRING locale,
            uint32_t* valueCount,
            CanvasTypographyFeatureName** valueElements) override;

        IFACEMETHOD(GetTypographicFeatureGlyphSupport)(
            CanvasAnalyzedScript script,
            CanvasTypographyFeatureName typographicFeatureName,
            uint32_t glyphsCount,
            CanvasGlyph* glyphsElements,
            uint32_t* valueCount,
            boolean** valueElements) override;

        IFACEMETHOD(GetTypographicFeatureGlyphSupportWithLocale)(
            CanvasAnalyzedScript script,
            CanvasTypographyFeatureName typographicFeatureName,
            uint32_t glyphsCount,
            CanvasGlyph* glyphsElements,
            HSTRING locale,
            uint32_t* valueCount,
            boolean** valueElements) override;

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // Internal
        //
        virtual ComPtr<DWriteFontFaceType> const& GetRealizedFontFace() override;

    private:
        float DesignSpaceToEmSpace(int designSpaceUnits, unsigned short designUnitsPerEm);

        ComPtr<DWritePhysicalFontPropertyContainer> GetPhysicalPropertyContainer();
    };

}}}}}
