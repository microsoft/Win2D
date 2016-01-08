// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockDWriteTextFormat.h"

namespace canvas
{
    class StubDWriteTextFormat : public MockDWriteTextFormat
    {
        DWRITE_TEXT_ALIGNMENT m_textAlignment;
        DWRITE_PARAGRAPH_ALIGNMENT m_paragraphAlignment;
        DWRITE_WORD_WRAPPING m_wordWrapping;
        DWRITE_READING_DIRECTION m_readingDirection;
        DWRITE_FLOW_DIRECTION m_flowDirection;
        float m_incrementalTabStop;

        DWRITE_TRIMMING m_trimming;
        ComPtr<IDWriteInlineObject> m_trimmingObject;
        DWRITE_LINE_SPACING_METHOD m_lineSpacingMethod;
        float m_lineSpacing;
        float m_baseline;

        ComPtr<IDWriteFontCollection> m_fontCollection;
        DWRITE_FONT_WEIGHT m_fontWeight;
        DWRITE_FONT_STYLE m_fontStyle;
        DWRITE_FONT_STRETCH m_fontStretch;
        float m_fontSize;

        std::wstring m_fontFamilyName;
        std::wstring m_localeName;

        DWRITE_VERTICAL_GLYPH_ORIENTATION m_verticalGlyphOrientation;
        DWRITE_OPTICAL_ALIGNMENT m_opticalAlignment;
        bool m_lastLineWrapping;

    public:
        StubDWriteTextFormat(
            WCHAR const* fontFamilyName,
            IDWriteFontCollection* fontCollection,
            DWRITE_FONT_WEIGHT fontWeight,
            DWRITE_FONT_STYLE fontStyle,
            DWRITE_FONT_STRETCH fontStretch,
            FLOAT fontSize,
            WCHAR const* localeName)
            : m_fontFamilyName(fontFamilyName)
            , m_fontCollection(fontCollection)
            , m_fontWeight(fontWeight)
            , m_fontStyle(fontStyle)
            , m_fontStretch(fontStretch)
            , m_fontSize(fontSize)
            , m_localeName(localeName)
        {
            SetTextAlignmentMethod.AllowAnyCall([&](DWRITE_TEXT_ALIGNMENT newValue) {m_textAlignment = newValue; return S_OK; });
            SetParagraphAlignmentMethod.AllowAnyCall([&](DWRITE_PARAGRAPH_ALIGNMENT newValue) {m_paragraphAlignment = newValue; return S_OK; });
            SetWordWrappingMethod.AllowAnyCall([&](DWRITE_WORD_WRAPPING newValue) {m_wordWrapping = newValue; return S_OK; });
            SetReadingDirectionMethod.AllowAnyCall([&](DWRITE_READING_DIRECTION direction) {m_readingDirection = direction; return S_OK; });
            SetFlowDirectionMethod.AllowAnyCall([&](DWRITE_FLOW_DIRECTION direction) {m_flowDirection = direction; return S_OK; });
            SetIncrementalTabStopMethod.AllowAnyCall([&](float stop) {m_incrementalTabStop = stop; return S_OK; });
            SetVerticalGlyphOrientationMethod.AllowAnyCall([&](DWRITE_VERTICAL_GLYPH_ORIENTATION newValue) {m_verticalGlyphOrientation = newValue; return S_OK; });
            SetOpticalAlignmentMethod.AllowAnyCall([&](DWRITE_OPTICAL_ALIGNMENT newValue) {m_opticalAlignment = newValue; return S_OK; });
            SetLastLineWrappingMethod.AllowAnyCall([&](BOOL newValue) {m_lastLineWrapping = !!newValue; return S_OK; });

            SetTrimmingMethod.AllowAnyCall(
                [&](DWRITE_TRIMMING const* trimming, IDWriteInlineObject* obj) 
                {
                    m_trimming = *trimming; 
                    m_trimmingObject = obj;
                    return S_OK;
                });
            SetLineSpacingMethod.AllowAnyCall(
                [&](DWRITE_LINE_SPACING_METHOD method, FLOAT lineSpacing, FLOAT baseline) 
                {
                    m_lineSpacingMethod = method;
                    m_lineSpacing = lineSpacing;
                    m_baseline = baseline;
                    return S_OK; 
                });

            GetTextAlignmentMethod.AllowAnyCall([&] {return m_textAlignment; });
            GetParagraphAlignmentMethod.AllowAnyCall([&] {return m_paragraphAlignment; });
            GetWordWrappingMethod.AllowAnyCall([&] {return m_wordWrapping; });
            GetReadingDirectionMethod.AllowAnyCall([&] {return m_readingDirection; });
            GetFlowDirectionMethod.AllowAnyCall([&] {return m_flowDirection; });
            GetIncrementalTabStopMethod.AllowAnyCall([&] {return m_incrementalTabStop; });

            GetTrimmingMethod.AllowAnyCall(
                [&](DWRITE_TRIMMING* trimming, IDWriteInlineObject** obj) 
                {
                    *trimming = m_trimming;
                    m_trimmingObject.CopyTo(obj);
                    return S_OK;
                });
            
            GetLineSpacingMethod.AllowAnyCall(
                [&](DWRITE_LINE_SPACING_METHOD* method, FLOAT* lineSpacing, FLOAT* baseline)
                {
                    *method = m_lineSpacingMethod;
                    *lineSpacing = m_lineSpacing;
                    *baseline = m_baseline;
                    return S_OK; 
                });

            GetFontCollectionMethod.AllowAnyCall(
                [&](IDWriteFontCollection** fontCollection)
                {
                    m_fontCollection.CopyTo(fontCollection);
                    return S_OK;
                });

            GetFontFamilyNameLengthMethod.AllowAnyCall([&] {return static_cast<uint32_t>(m_fontFamilyName.length()); });

            GetFontFamilyNameMethod.AllowAnyCall(
                [&](WCHAR* buf, UINT32 length)
                {
                    Assert::IsTrue(length == m_fontFamilyName.length() + 1); // accounts for null term
                    StringCchCopyN(buf, length, &m_fontFamilyName[0], length);
                    return S_OK;
                });

            GetFontWeightMethod.AllowAnyCall([&] {return m_fontWeight; });
            GetFontStyleMethod.AllowAnyCall([&] {return m_fontStyle; });
            GetFontStretchMethod.AllowAnyCall([&] {return m_fontStretch; });
            GetFontSizeMethod.AllowAnyCall([&] {return m_fontSize; });

            GetLocaleNameLengthMethod.AllowAnyCall([&] {return static_cast<uint32_t>(m_localeName.length()); });

            GetLocaleNameMethod.AllowAnyCall(
                [&](WCHAR* buf, UINT32 length)
                {
                    Assert::IsTrue(length == m_localeName.length() + 1); // accounts for null term
                    StringCchCopyN(buf, length, &m_localeName[0], length);

                    return S_OK;
                });

            GetVerticalGlyphOrientationMethod.AllowAnyCall([&] { return m_verticalGlyphOrientation; });
            GetOpticalAlignmentMethod.AllowAnyCall([&] { return m_opticalAlignment; });
            GetLastLineWrappingMethod.AllowAnyCall([&] { return m_lastLineWrapping; });
        }
    };
}