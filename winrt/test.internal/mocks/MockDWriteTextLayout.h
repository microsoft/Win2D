// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteTextLayout : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
        IDWriteTextLayout3,
#endif
        IDWriteTextLayout2, IDWriteTextLayout1, IDWriteTextLayout, IDWriteTextFormat>>
    {
    public:
        // IDWriteTextFormat
        CALL_COUNTER_WITH_MOCK(SetTextAlignmentMethod, HRESULT(DWRITE_TEXT_ALIGNMENT));
        CALL_COUNTER_WITH_MOCK(SetParagraphAlignmentMethod, HRESULT(DWRITE_PARAGRAPH_ALIGNMENT));
        CALL_COUNTER_WITH_MOCK(SetWordWrappingMethod, HRESULT(DWRITE_WORD_WRAPPING));
        CALL_COUNTER_WITH_MOCK(SetReadingDirectionMethod, HRESULT(DWRITE_READING_DIRECTION));
        CALL_COUNTER_WITH_MOCK(SetFlowDirectionMethod, HRESULT(DWRITE_FLOW_DIRECTION));
        CALL_COUNTER_WITH_MOCK(SetIncrementalTabStopMethod, HRESULT(FLOAT));
        CALL_COUNTER_WITH_MOCK(SetTrimmingMethod, HRESULT(DWRITE_TRIMMING const*, IDWriteInlineObject*));
        CALL_COUNTER_WITH_MOCK(SetLineSpacingMethod, HRESULT(DWRITE_LINE_SPACING_METHOD, FLOAT, FLOAT));
        CALL_COUNTER_WITH_MOCK(GetTextAlignmentMethod, DWRITE_TEXT_ALIGNMENT());
        CALL_COUNTER_WITH_MOCK(GetParagraphAlignmentMethod, DWRITE_PARAGRAPH_ALIGNMENT());
        CALL_COUNTER_WITH_MOCK(GetWordWrappingMethod, DWRITE_WORD_WRAPPING());
        CALL_COUNTER_WITH_MOCK(GetReadingDirectionMethod, DWRITE_READING_DIRECTION());
        CALL_COUNTER_WITH_MOCK(GetFlowDirectionMethod, DWRITE_FLOW_DIRECTION());
        CALL_COUNTER_WITH_MOCK(GetIncrementalTabStopMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetTrimmingMethod, HRESULT(DWRITE_TRIMMING*, IDWriteInlineObject**));
        CALL_COUNTER_WITH_MOCK(GetLineSpacingMethod, HRESULT(DWRITE_LINE_SPACING_METHOD*, FLOAT*, FLOAT*));
        CALL_COUNTER_WITH_MOCK(GetFontCollection_BaseFormat_Method, HRESULT(IDWriteFontCollection**));
        CALL_COUNTER_WITH_MOCK(GetFontFamilyNameLength_BaseFormat_Method, UINT32());
        CALL_COUNTER_WITH_MOCK(GetFontFamilyName_BaseFormat_Method, HRESULT(WCHAR*, UINT32));
        CALL_COUNTER_WITH_MOCK(GetFontWeight_BaseFormat_Method, DWRITE_FONT_WEIGHT());
        CALL_COUNTER_WITH_MOCK(GetFontStyle_BaseFormat_Method, DWRITE_FONT_STYLE());
        CALL_COUNTER_WITH_MOCK(GetFontStretch_BaseFormat_Method, DWRITE_FONT_STRETCH());
        CALL_COUNTER_WITH_MOCK(GetFontSize_BaseFormat_Method, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetLocaleNameLength_BaseFormat_Method, UINT32());
        CALL_COUNTER_WITH_MOCK(GetLocaleName_BaseFormat_Method, HRESULT(WCHAR*, UINT32));

        // IDWriteTextLayout
        CALL_COUNTER_WITH_MOCK(SetMaxWidthMethod, HRESULT(FLOAT));
        CALL_COUNTER_WITH_MOCK(SetMaxHeightMethod, HRESULT(FLOAT));
        CALL_COUNTER_WITH_MOCK(SetFontCollectionMethod, HRESULT(IDWriteFontCollection*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetFontFamilyNameMethod, HRESULT(WCHAR const*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetFontWeightMethod, HRESULT(DWRITE_FONT_WEIGHT, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetFontStyleMethod, HRESULT(DWRITE_FONT_STYLE, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetFontStretchMethod, HRESULT(DWRITE_FONT_STRETCH, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetFontSizeMethod, HRESULT(FLOAT, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetUnderlineMethod, HRESULT(BOOL, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetStrikethroughMethod, HRESULT(BOOL, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetDrawingEffectMethod, HRESULT(IUnknown*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetInlineObjectMethod, HRESULT(IDWriteInlineObject*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetTypographyMethod, HRESULT(IDWriteTypography*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(SetLocaleNameMethod, HRESULT(WCHAR const*, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(GetMaxWidthMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetMaxHeightMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetFontCollectionMethod, HRESULT(UINT32, IDWriteFontCollection**, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontFamilyNameLengthMethod, HRESULT(UINT32, UINT32*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontFamilyNameMethod, HRESULT(UINT32, WCHAR*, UINT32, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontWeightMethod, HRESULT(UINT32, DWRITE_FONT_WEIGHT*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontStyleMethod, HRESULT(UINT32, DWRITE_FONT_STYLE*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontStretchMethod, HRESULT(UINT32, DWRITE_FONT_STRETCH*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetFontSizeMethod, HRESULT(UINT32, FLOAT*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetUnderlineMethod, HRESULT(UINT32, BOOL*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetStrikethroughMethod, HRESULT(UINT32, BOOL*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetDrawingEffectMethod, HRESULT(UINT32, IUnknown**, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetInlineObjectMethod, HRESULT(UINT32, IDWriteInlineObject**, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetTypographyMethod, HRESULT(UINT32, IDWriteTypography**, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetLocaleNameLengthMethod, HRESULT(UINT32, UINT32*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(GetLocaleNameMethod, HRESULT(UINT32, WCHAR*, UINT32, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(DrawMethod, HRESULT(void*, IDWriteTextRenderer*, FLOAT, FLOAT));
        CALL_COUNTER_WITH_MOCK(GetLineMetricsMethod, HRESULT(DWRITE_LINE_METRICS*, UINT32, UINT32*));
        CALL_COUNTER_WITH_MOCK(GetMetrics_BaseFormat_Method, HRESULT(DWRITE_TEXT_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetOverhangMetricsMethod, HRESULT(DWRITE_OVERHANG_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetClusterMetricsMethod, HRESULT(DWRITE_CLUSTER_METRICS*, UINT32, UINT32*));
        CALL_COUNTER_WITH_MOCK(DetermineMinWidthMethod, HRESULT(FLOAT*));
        CALL_COUNTER_WITH_MOCK(HitTestPointMethod, HRESULT(FLOAT, FLOAT, BOOL*, BOOL*, DWRITE_HIT_TEST_METRICS*));
        CALL_COUNTER_WITH_MOCK(HitTestTextPositionMethod, HRESULT(UINT32 textPosition, BOOL, FLOAT*, FLOAT*, DWRITE_HIT_TEST_METRICS*));
        CALL_COUNTER_WITH_MOCK(HitTestTextRangeMethod, HRESULT(UINT32, UINT32, FLOAT, FLOAT, DWRITE_HIT_TEST_METRICS*, UINT32, UINT32*));

        // IDWriteTextLayout1
        CALL_COUNTER_WITH_MOCK(SetPairKerningMethod, HRESULT(BOOL, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(GetPairKerningMethod, HRESULT(UINT32, BOOL*, DWRITE_TEXT_RANGE*));
        CALL_COUNTER_WITH_MOCK(SetCharacterSpacingMethod, HRESULT(FLOAT, FLOAT, FLOAT, DWRITE_TEXT_RANGE));
        CALL_COUNTER_WITH_MOCK(GetCharacterSpacingMethod, HRESULT(UINT32, FLOAT*, FLOAT*, FLOAT*, DWRITE_TEXT_RANGE*));

        // IDWriteTextLayout2
        CALL_COUNTER_WITH_MOCK(GetMetricsMethod, HRESULT(DWRITE_TEXT_METRICS1*));
        CALL_COUNTER_WITH_MOCK(SetVerticalGlyphOrientationMethod, HRESULT(DWRITE_VERTICAL_GLYPH_ORIENTATION));
        CALL_COUNTER_WITH_MOCK(GetVerticalGlyphOrientationMethod, DWRITE_VERTICAL_GLYPH_ORIENTATION());
        CALL_COUNTER_WITH_MOCK(SetLastLineWrappingMethod, HRESULT(BOOL));
        CALL_COUNTER_WITH_MOCK(GetLastLineWrappingMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(SetOpticalAlignmentMethod, HRESULT(DWRITE_OPTICAL_ALIGNMENT));
        CALL_COUNTER_WITH_MOCK(GetOpticalAlignmentMethod, DWRITE_OPTICAL_ALIGNMENT());
        CALL_COUNTER_WITH_MOCK(SetFontFallbackMethod, HRESULT(IDWriteFontFallback*));
        CALL_COUNTER_WITH_MOCK(GetFontFallbackMethod, HRESULT(IDWriteFontFallback**));

#if WINVER > _WIN32_WINNT_WINBLUE
        // IDWriteTextLayout3
        CALL_COUNTER_WITH_MOCK(GetLineMetricsMethod1, HRESULT(DWRITE_LINE_METRICS1*, UINT32, UINT32*));
#endif


        ///////////////////////////////////////////////////////////////////////////////////////////

        // IDWriteTextFormat

        IFACEMETHODIMP SetTextAlignment(
            DWRITE_TEXT_ALIGNMENT textAlignment) override
        {
            return SetTextAlignmentMethod.WasCalled(textAlignment);
        }

        IFACEMETHODIMP SetParagraphAlignment(
            DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment) override
        {
            return SetParagraphAlignmentMethod.WasCalled(paragraphAlignment);
        }

        IFACEMETHODIMP SetWordWrapping(
            DWRITE_WORD_WRAPPING wordWrapping) override
        {
            return SetWordWrappingMethod.WasCalled(wordWrapping);
        }

        IFACEMETHODIMP SetReadingDirection(
            DWRITE_READING_DIRECTION readingDirection) override
        {
            return SetReadingDirectionMethod.WasCalled(readingDirection);
        }

        IFACEMETHODIMP SetFlowDirection(
            DWRITE_FLOW_DIRECTION flowDirection) override
        {
            return SetFlowDirectionMethod.WasCalled(flowDirection);
        }

        IFACEMETHODIMP SetIncrementalTabStop(
            FLOAT incrementalTabStop) override
        {
            return SetIncrementalTabStopMethod.WasCalled(incrementalTabStop);
        }

        IFACEMETHODIMP SetTrimming(
            DWRITE_TRIMMING const* trimmingOptions, IDWriteInlineObject* trimmingSign) override
        {
            return SetTrimmingMethod.WasCalled(trimmingOptions, trimmingSign);
        }

        IFACEMETHODIMP SetLineSpacing(
            DWRITE_LINE_SPACING_METHOD lineSpacingMethod, FLOAT lineSpacing, FLOAT baseline) override
        {
            return SetLineSpacingMethod.WasCalled(lineSpacingMethod, lineSpacing, baseline);
        }

        IFACEMETHODIMP_(DWRITE_TEXT_ALIGNMENT) GetTextAlignment(
          ) override
        {
            return GetTextAlignmentMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_PARAGRAPH_ALIGNMENT) GetParagraphAlignment(
          ) override
        {
            return GetParagraphAlignmentMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_WORD_WRAPPING) GetWordWrapping(
          ) override
        {
            return GetWordWrappingMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_READING_DIRECTION) GetReadingDirection(
          ) override
        {
            return GetReadingDirectionMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FLOW_DIRECTION) GetFlowDirection(
          ) override
        {
            return GetFlowDirectionMethod.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetIncrementalTabStop(
          ) override
        {
            return GetIncrementalTabStopMethod.WasCalled();
        }

        IFACEMETHODIMP GetTrimming(
            DWRITE_TRIMMING* trimmingOptions, IDWriteInlineObject** trimmingSign) override
        {
            return GetTrimmingMethod.WasCalled(trimmingOptions, trimmingSign);
        }

        IFACEMETHODIMP GetLineSpacing(
            DWRITE_LINE_SPACING_METHOD* lineSpacingMethod, FLOAT* lineSpacing, FLOAT* baseline) override
        {
            return GetLineSpacingMethod.WasCalled(lineSpacingMethod, lineSpacing, baseline);
        }

        IFACEMETHODIMP GetFontCollection(
            IDWriteFontCollection** fontCollection) override
        {
            return GetFontCollection_BaseFormat_Method.WasCalled(fontCollection);
        }

        IFACEMETHODIMP_(UINT32) GetFontFamilyNameLength(
          ) override
        {
            return GetFontFamilyNameLength_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP GetFontFamilyName(
            WCHAR* fontFamilyName, UINT32 nameSize) override
        {
            return GetFontFamilyName_BaseFormat_Method.WasCalled(fontFamilyName, nameSize);
        }

        IFACEMETHODIMP_(DWRITE_FONT_WEIGHT) GetFontWeight(
          ) override
        {
            return GetFontWeight_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STYLE) GetFontStyle(
          ) override
        {
            return GetFontStyle_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STRETCH) GetFontStretch(
          ) override
        {
            return GetFontStretch_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetFontSize(
          ) override
        {
            return GetFontSize_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP_(UINT32) GetLocaleNameLength(
          ) override
        {
            return GetLocaleNameLength_BaseFormat_Method.WasCalled();
        }

        IFACEMETHODIMP GetLocaleName(
            WCHAR* localeName, UINT32 nameSize) override
        {
            return GetLocaleName_BaseFormat_Method.WasCalled(localeName, nameSize);
        }

        // IDWriteTextLayout

        IFACEMETHODIMP SetMaxWidth(
            FLOAT maxWidth) override
        {
            return SetMaxWidthMethod.WasCalled(maxWidth);
        }

        IFACEMETHODIMP SetMaxHeight(
            FLOAT maxHeight) override
        {
            return SetMaxHeightMethod.WasCalled(maxHeight);
        }

        IFACEMETHODIMP SetFontCollection(
            IDWriteFontCollection* fontCollection, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontCollectionMethod.WasCalled(fontCollection, textRange);
        }

        IFACEMETHODIMP SetFontFamilyName(
            WCHAR const* fontFamilyName, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontFamilyNameMethod.WasCalled(fontFamilyName, textRange);
        }

        IFACEMETHODIMP SetFontWeight(
            DWRITE_FONT_WEIGHT fontWeight, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontWeightMethod.WasCalled(fontWeight, textRange);
        }

        IFACEMETHODIMP SetFontStyle(
            DWRITE_FONT_STYLE fontStyle, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontStyleMethod.WasCalled(fontStyle, textRange);
        }

        IFACEMETHODIMP SetFontStretch(
            DWRITE_FONT_STRETCH fontStretch, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontStretchMethod.WasCalled(fontStretch, textRange);
        }

        IFACEMETHODIMP SetFontSize(
            FLOAT fontSize, DWRITE_TEXT_RANGE textRange) override
        {
            return SetFontSizeMethod.WasCalled(fontSize, textRange);
        }

        IFACEMETHODIMP SetUnderline(
            BOOL hasUnderline, DWRITE_TEXT_RANGE textRange) override
        {
            return SetUnderlineMethod.WasCalled(hasUnderline, textRange);
        }

        IFACEMETHODIMP SetStrikethrough(
            BOOL hasStrikethrough, DWRITE_TEXT_RANGE textRange) override
        {
            return SetStrikethroughMethod.WasCalled(hasStrikethrough, textRange);
        }

        IFACEMETHODIMP SetDrawingEffect(
            IUnknown* drawingEffect, DWRITE_TEXT_RANGE textRange) override
        {
            return SetDrawingEffectMethod.WasCalled(drawingEffect, textRange);
        }

        IFACEMETHODIMP SetInlineObject(
            IDWriteInlineObject* inlineObject, DWRITE_TEXT_RANGE textRange) override
        {
            return SetInlineObjectMethod.WasCalled(inlineObject, textRange);
        }

        IFACEMETHODIMP SetTypography(
            IDWriteTypography* typography, DWRITE_TEXT_RANGE textRange) override
        {
            return SetTypographyMethod.WasCalled(typography, textRange);
        }

        IFACEMETHODIMP SetLocaleName(
            WCHAR const* localeName, DWRITE_TEXT_RANGE textRange) override
        {
            return SetLocaleNameMethod.WasCalled(localeName, textRange);
        }

        IFACEMETHODIMP_(FLOAT) GetMaxWidth(
          ) override
        {
            return GetMaxWidthMethod.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetMaxHeight(
          ) override
        {
            return GetMaxHeightMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontCollection(
            UINT32 currentPosition, IDWriteFontCollection** fontCollection, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontCollectionMethod.WasCalled(currentPosition, fontCollection, textRange);
        }

        IFACEMETHODIMP GetFontFamilyNameLength(
            UINT32 currentPosition, UINT32* nameLength, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontFamilyNameLengthMethod.WasCalled(currentPosition, nameLength, textRange);
        }

        IFACEMETHODIMP GetFontFamilyName(
            UINT32 currentPosition, WCHAR* fontFamilyName, UINT32 nameSize, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontFamilyNameMethod.WasCalled(currentPosition, fontFamilyName, nameSize, textRange);
        }

        IFACEMETHODIMP GetFontWeight(
            UINT32 currentPosition, DWRITE_FONT_WEIGHT* fontWeight, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontWeightMethod.WasCalled(currentPosition, fontWeight, textRange);
        }

        IFACEMETHODIMP GetFontStyle(
            UINT32 currentPosition, DWRITE_FONT_STYLE* fontStyle, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontStyleMethod.WasCalled(currentPosition, fontStyle, textRange);
        }

        IFACEMETHODIMP GetFontStretch(
            UINT32 currentPosition, DWRITE_FONT_STRETCH* fontStretch, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontStretchMethod.WasCalled(currentPosition, fontStretch, textRange);
        }

        IFACEMETHODIMP GetFontSize(
            UINT32 currentPosition, FLOAT* fontSize, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetFontSizeMethod.WasCalled(currentPosition, fontSize, textRange);
        }

        IFACEMETHODIMP GetUnderline(
            UINT32 currentPosition, BOOL* hasUnderline, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetUnderlineMethod.WasCalled(currentPosition, hasUnderline, textRange);
        }

        IFACEMETHODIMP GetStrikethrough(
            UINT32 currentPosition, BOOL* hasStrikethrough, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetStrikethroughMethod.WasCalled(currentPosition, hasStrikethrough, textRange);
        }

        IFACEMETHODIMP GetDrawingEffect(
            UINT32 currentPosition, IUnknown** drawingEffect, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetDrawingEffectMethod.WasCalled(currentPosition, drawingEffect, textRange);
        }

        IFACEMETHODIMP GetInlineObject(
            UINT32 currentPosition, IDWriteInlineObject** inlineObject, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetInlineObjectMethod.WasCalled(currentPosition, inlineObject, textRange);
        }

        IFACEMETHODIMP GetTypography(
            UINT32 currentPosition, IDWriteTypography** typography, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetTypographyMethod.WasCalled(currentPosition, typography, textRange);
        }

        IFACEMETHODIMP GetLocaleNameLength(
            UINT32 currentPosition, UINT32* nameLength, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetLocaleNameLengthMethod.WasCalled(currentPosition, nameLength, textRange);
        }

        IFACEMETHODIMP GetLocaleName(
            UINT32 currentPosition, WCHAR* localeName, UINT32 nameSize, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetLocaleNameMethod.WasCalled(currentPosition, localeName, nameSize, textRange);
        }

        IFACEMETHODIMP Draw(
            void* clientDrawingContext, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY) override
        {
            return DrawMethod.WasCalled(clientDrawingContext, renderer, originX, originY);
        }

        IFACEMETHODIMP GetLineMetrics(
            DWRITE_LINE_METRICS* lineMetrics, UINT32 maxLineCount, UINT32* actualLineCount) override
        {
            return GetLineMetricsMethod.WasCalled(lineMetrics, maxLineCount, actualLineCount);
        }

        IFACEMETHODIMP GetMetrics(
            DWRITE_TEXT_METRICS* textMetrics) override
        {
            return GetMetrics_BaseFormat_Method.WasCalled(textMetrics);
        }

        IFACEMETHODIMP GetOverhangMetrics(
            DWRITE_OVERHANG_METRICS* overhangs) override
        {
            return GetOverhangMetricsMethod.WasCalled(overhangs);
        }

        IFACEMETHODIMP GetClusterMetrics(
            DWRITE_CLUSTER_METRICS* clusterMetrics, UINT32 maxClusterCount, UINT32* actualClusterCount) override
        {
            return GetClusterMetricsMethod.WasCalled(clusterMetrics, maxClusterCount, actualClusterCount);
        }

        IFACEMETHODIMP DetermineMinWidth(
            FLOAT* minWidth) override
        {
            return DetermineMinWidthMethod.WasCalled(minWidth);
        }

        IFACEMETHODIMP HitTestPoint(
            FLOAT pointX, FLOAT pointY, BOOL* isTrailingHit, BOOL* isInside, DWRITE_HIT_TEST_METRICS* hitTestMetrics) override
        {
            return HitTestPointMethod.WasCalled(pointX, pointY, isTrailingHit, isInside, hitTestMetrics);
        }

        IFACEMETHODIMP HitTestTextPosition(
            UINT32 textPosition, BOOL isTrailingHit, FLOAT* pointX, FLOAT* pointY, DWRITE_HIT_TEST_METRICS* hitTestMetrics) override
        {
            return HitTestTextPositionMethod.WasCalled(textPosition, isTrailingHit, pointX, pointY, hitTestMetrics);
        }

        IFACEMETHODIMP HitTestTextRange(
            UINT32 textPosition, UINT32 textLength, FLOAT originX, FLOAT originY, DWRITE_HIT_TEST_METRICS* hitTestMetrics, UINT32 maxHitTestMetricsCount, UINT32* actualHitTestMetricsCount) override
        {
            return HitTestTextRangeMethod.WasCalled(textPosition, textLength, originX, originY, hitTestMetrics, maxHitTestMetricsCount, actualHitTestMetricsCount);
        }

        // IDWriteTextLayout1

        IFACEMETHODIMP SetPairKerning(
            BOOL isPairKerningEnabled, DWRITE_TEXT_RANGE textRange) override
        {
            return SetPairKerningMethod.WasCalled(isPairKerningEnabled, textRange);
        }

        IFACEMETHODIMP GetPairKerning(
            UINT32 currentPosition, BOOL* isPairKerningEnabled, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetPairKerningMethod.WasCalled(currentPosition, isPairKerningEnabled, textRange);
        }

        IFACEMETHODIMP SetCharacterSpacing(
            FLOAT leadingSpacing, FLOAT trailingSpacing, FLOAT minimumAdvanceWidth, DWRITE_TEXT_RANGE textRange) override
        {
            return SetCharacterSpacingMethod.WasCalled(leadingSpacing, trailingSpacing, minimumAdvanceWidth, textRange);
        }

        IFACEMETHODIMP GetCharacterSpacing(
            UINT32 currentPosition, FLOAT* leadingSpacing, FLOAT* trailingSpacing, FLOAT* minimumAdvanceWidth, DWRITE_TEXT_RANGE* textRange) override
        {
            return GetCharacterSpacingMethod.WasCalled(currentPosition, leadingSpacing, trailingSpacing, minimumAdvanceWidth, textRange);
        }

        // IDWriteTextLayout2

        IFACEMETHODIMP GetMetrics(
            DWRITE_TEXT_METRICS1* textMetrics) override
        {
            return GetMetricsMethod.WasCalled(textMetrics);
        }

        IFACEMETHODIMP SetVerticalGlyphOrientation(
            DWRITE_VERTICAL_GLYPH_ORIENTATION glyphOrientation) override
        {
            return SetVerticalGlyphOrientationMethod.WasCalled(glyphOrientation);
        }

        IFACEMETHODIMP_(DWRITE_VERTICAL_GLYPH_ORIENTATION) GetVerticalGlyphOrientation(
          ) override
        {
            return GetVerticalGlyphOrientationMethod.WasCalled();
        }

        IFACEMETHODIMP SetLastLineWrapping(
            BOOL isLastLineWrappingEnabled) override
        {
            return SetLastLineWrappingMethod.WasCalled(isLastLineWrappingEnabled);
        }

        IFACEMETHODIMP_(BOOL) GetLastLineWrapping(
          ) override
        {
            return GetLastLineWrappingMethod.WasCalled();
        }

        IFACEMETHODIMP SetOpticalAlignment(
            DWRITE_OPTICAL_ALIGNMENT opticalAlignment) override
        {
            return SetOpticalAlignmentMethod.WasCalled(opticalAlignment);
        }

        IFACEMETHODIMP_(DWRITE_OPTICAL_ALIGNMENT) GetOpticalAlignment(
          ) override
        {
            return GetOpticalAlignmentMethod.WasCalled();
        }

        IFACEMETHODIMP SetFontFallback(
            IDWriteFontFallback* fontFallback) override
        {
            return SetFontFallbackMethod.WasCalled(fontFallback);
        }

        IFACEMETHODIMP GetFontFallback(
            IDWriteFontFallback** fontFallback) override
        {
            return GetFontFallbackMethod.WasCalled(fontFallback);
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        //
        // IDWriteTextLayout3
        //
        IFACEMETHODIMP InvalidateLayout() override
        {
            Assert::Fail(L"Unexpected call to InvalidateLayout");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP SetLineSpacing(DWRITE_LINE_SPACING const*) override
        {
            Assert::Fail(L"Unexpected call to SetLineSpacing");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetLineSpacing(DWRITE_LINE_SPACING*) override
        {
            Assert::Fail(L"Unexpected call to GetLineSpacing");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetLineMetrics(
            DWRITE_LINE_METRICS1* lineMetrics, 
            uint32_t maxLineCount, 
            uint32_t* actualLineCount) override
        {
            return GetLineMetricsMethod1.WasCalled(lineMetrics, maxLineCount, actualLineCount);
        }
#endif
    };
}
