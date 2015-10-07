// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteTextFormat : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDWriteTextFormat1, IDWriteTextFormat>>
    {
    public:
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
        CALL_COUNTER_WITH_MOCK(GetFontCollectionMethod, HRESULT(IDWriteFontCollection**));
        CALL_COUNTER_WITH_MOCK(GetFontFamilyNameLengthMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetFontFamilyNameMethod, HRESULT(WCHAR*, UINT32));
        CALL_COUNTER_WITH_MOCK(GetFontWeightMethod, DWRITE_FONT_WEIGHT());
        CALL_COUNTER_WITH_MOCK(GetFontStyleMethod, DWRITE_FONT_STYLE());
        CALL_COUNTER_WITH_MOCK(GetFontStretchMethod, DWRITE_FONT_STRETCH());
        CALL_COUNTER_WITH_MOCK(GetFontSizeMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetLocaleNameLengthMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetLocaleNameMethod, HRESULT(WCHAR*, UINT32));
        CALL_COUNTER_WITH_MOCK(SetVerticalGlyphOrientationMethod, HRESULT(DWRITE_VERTICAL_GLYPH_ORIENTATION));
        CALL_COUNTER_WITH_MOCK(GetVerticalGlyphOrientationMethod, DWRITE_VERTICAL_GLYPH_ORIENTATION());
        CALL_COUNTER_WITH_MOCK(SetLastLineWrappingMethod, HRESULT(BOOL));
        CALL_COUNTER_WITH_MOCK(GetLastLineWrappingMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(SetOpticalAlignmentMethod, HRESULT(DWRITE_OPTICAL_ALIGNMENT));
        CALL_COUNTER_WITH_MOCK(GetOpticalAlignmentMethod, DWRITE_OPTICAL_ALIGNMENT());
        CALL_COUNTER_WITH_MOCK(SetFontFallbackMethod, HRESULT(IDWriteFontFallback*));
        CALL_COUNTER_WITH_MOCK(GetFontFallbackMethod, HRESULT(IDWriteFontFallback**));


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
            return GetFontCollectionMethod.WasCalled(fontCollection);
        }

        IFACEMETHODIMP_(UINT32) GetFontFamilyNameLength(
            ) override
        {
            return GetFontFamilyNameLengthMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontFamilyName(
            WCHAR* fontFamilyName, UINT32 nameSize) override
        {
            return GetFontFamilyNameMethod.WasCalled(fontFamilyName, nameSize);
        }

        IFACEMETHODIMP_(DWRITE_FONT_WEIGHT) GetFontWeight(
            ) override
        {
            return GetFontWeightMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STYLE) GetFontStyle(
            ) override
        {
            return GetFontStyleMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STRETCH) GetFontStretch(
            ) override
        {
            return GetFontStretchMethod.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetFontSize(
            ) override
        {
            return GetFontSizeMethod.WasCalled();
        }

        IFACEMETHODIMP_(UINT32) GetLocaleNameLength(
            ) override
        {
            return GetLocaleNameLengthMethod.WasCalled();
        }

        IFACEMETHODIMP GetLocaleName(
            WCHAR* localeName, UINT32 nameSize) override
        {
            return GetLocaleNameMethod.WasCalled(localeName, nameSize);
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
    };
}