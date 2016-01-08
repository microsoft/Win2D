// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <lib/text/CanvasTextLayout.h>
#include <lib/text/CanvasTextFormat.h>
#include "mocks/MockDWriteFactory.h"
#include "mocks/MockDWriteTextLayout.h"
#include "mocks/MockDWriteFontCollection.h"
#include "mocks/MockDWriteInlineObject.h"
#include "mocks/MockDWriteRenderingParams.h"
#include "stubs/StubStorageFileStatics.h"
#include "stubs/StubDWriteTextFormat.h"

namespace canvas
{
    class StubTextLayout : public MockDWriteTextLayout
    {
        DWRITE_LINE_SPACING_METHOD m_lineSpacingMethod;
        float m_lineSpacing;
        float m_baseline;
        DWRITE_TRIMMING m_trimming;
        ComPtr<IDWriteInlineObject> m_trimmingSign;

    public:
        StubTextLayout()
            : m_lineSpacingMethod(DWRITE_LINE_SPACING_METHOD_DEFAULT)
            , m_lineSpacing(0)
            , m_baseline(0)
            , m_trimming(DWRITE_TRIMMING{})
        {
            GetFontCollection_BaseFormat_Method.AllowAnyCall(
                [](IDWriteFontCollection** fontCollection)
                {
                    auto mockFontCollection = Make<MockDWriteFontCollection>();
                    mockFontCollection.CopyTo(fontCollection);
                    return S_OK;
                });

            GetFontFamilyNameLength_BaseFormat_Method.AllowAnyCall([] { return 1; });

            GetFontFamilyName_BaseFormat_Method.AllowAnyCall(
                [](WCHAR* localeName, UINT32 nameSize)
                {
                    Assert::AreEqual(2u, nameSize); // Includes null term
                    localeName[0] = L'A';
                    localeName[1] = 0;
                    return S_OK;
                });

            GetFlowDirectionMethod.AllowAnyCall([] { return DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM; });

            GetFontSize_BaseFormat_Method.AllowAnyCall([] { return 16.0f; });

            GetFontStretch_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_STRETCH_UNDEFINED; });

            GetFontStyle_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_STYLE_NORMAL; });

            GetFontWeight_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_WEIGHT_NORMAL; });

            GetIncrementalTabStopMethod.AllowAnyCall([] { return 0.0f; });

            GetLocaleNameLength_BaseFormat_Method.AllowAnyCall([] { return 1; });

            GetLocaleName_BaseFormat_Method.AllowAnyCall(
                [](WCHAR* localeName, UINT32 nameSize)
                {
                    Assert::AreEqual(2u, nameSize);
                    localeName[0] = L'A';
                    return S_OK;
                });

            GetParagraphAlignmentMethod.AllowAnyCall([] { return DWRITE_PARAGRAPH_ALIGNMENT_CENTER; });

            GetTextAlignmentMethod.AllowAnyCall([] { return DWRITE_TEXT_ALIGNMENT_CENTER; });

            GetReadingDirectionMethod.AllowAnyCall([] { return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT; });

            GetWordWrappingMethod.AllowAnyCall([] { return DWRITE_WORD_WRAPPING_WRAP; });

            GetLineSpacingMethod.AllowAnyCall(
                [=](DWRITE_LINE_SPACING_METHOD* lineSpacingMethod, FLOAT* lineSpacing, FLOAT* baseline)
                {
                    *lineSpacingMethod = m_lineSpacingMethod;
                    *lineSpacing = m_lineSpacing;
                    *baseline = m_baseline;
                    return S_OK;
                });

            SetLineSpacingMethod.AllowAnyCall(
                [=](DWRITE_LINE_SPACING_METHOD lineSpacingMethod, FLOAT lineSpacing, FLOAT baseline)
                {
                    m_lineSpacingMethod = lineSpacingMethod;
                    m_lineSpacing = lineSpacing;
                    m_baseline = baseline;
                    return S_OK;
                });

            GetTrimmingMethod.AllowAnyCall(
                [=](DWRITE_TRIMMING* trimming, IDWriteInlineObject** sign)
                {
                    *trimming = m_trimming;
                    m_trimmingSign.CopyTo(sign);
                    return S_OK;
                });

            SetTrimmingMethod.AllowAnyCall(
                [=](DWRITE_TRIMMING const* trimming, IDWriteInlineObject* sign)
                {
                    m_trimming = *trimming;
                    m_trimmingSign = sign;
                    return S_OK;
                });

        }
    };

    class StubCanvasTextLayoutAdapter : public CustomFontManagerAdapter
    {
        ComPtr<MockDWriteFactory> m_mockDWritefactory;

    public:

        ComPtr<StubTextLayout> MockTextLayout;
        ComPtr<StubStorageFileStatics> StorageFileStatics;

        StubCanvasTextLayoutAdapter()
            : m_mockDWritefactory(Make<MockDWriteFactory>())
            , MockTextLayout(Make<StubTextLayout>())
            , StorageFileStatics(Make<StubStorageFileStatics>())
        {

            m_mockDWritefactory->CreateTextLayoutMethod.AllowAnyCall(
                [&](
                WCHAR const* string,
                UINT32 stringLength,
                IDWriteTextFormat* textFormat,
                FLOAT maxWidth,
                FLOAT maxHeight,
                IDWriteTextLayout** textLayout)
            {
                MockTextLayout.CopyTo(textLayout);
                return S_OK;
            });


            m_mockDWritefactory->CreateTextFormatMethod.AllowAnyCall(
                [&]
                (WCHAR const* fontFamilyName,
                IDWriteFontCollection* fontCollection,
                DWRITE_FONT_WEIGHT fontWeight,
                DWRITE_FONT_STYLE fontStyle,
                DWRITE_FONT_STRETCH fontStretch,
                FLOAT fontSize,
                WCHAR const* localeName,
                IDWriteTextFormat** textFormat)
                {
                    auto stubTextFormat = Make<StubDWriteTextFormat>(
                        fontFamilyName,
                        fontCollection,
                        fontWeight,
                        fontStyle,
                        fontStretch,
                        fontSize,
                        localeName);
                    stubTextFormat.CopyTo(textFormat);
                    return S_OK;
                });

            m_mockDWritefactory->RegisterFontCollectionLoaderMethod.AllowAnyCall();

            m_mockDWritefactory->UnregisterFontCollectionLoaderMethod.AllowAnyCall();

            m_mockDWritefactory->CreateEllipsisTrimmingSignMethod.AllowAnyCall(
                [&](IDWriteTextFormat*, IDWriteInlineObject** out)
                {
                    auto mockInlineObject = Make<MockDWriteInlineObject>();

                    ThrowIfFailed(mockInlineObject.CopyTo(out));

                    return S_OK;
                });

            m_mockDWritefactory->CreateRenderingParamsMethod.AllowAnyCall(
                [&](IDWriteRenderingParams** renderingParams)
                {
                    auto mockDWriteParams = Make<MockDWriteRenderingParams>();

                    mockDWriteParams->GetGammaMethod.AllowAnyCall([&](){ return 2.2f; });
                    mockDWriteParams->GetEnhancedContrastMethod.AllowAnyCall([&](){ return 1.0f; });
                    mockDWriteParams->GetGrayscaleEnhancedContrastMethod.AllowAnyCall([&](){ return 1.0f; });
                    mockDWriteParams->GetClearTypeLevelMethod.AllowAnyCall([&](){ return 1.0f; });
                    mockDWriteParams->GetPixelGeometryMethod.AllowAnyCall([&](){ return DWRITE_PIXEL_GEOMETRY_FLAT; });

                    ThrowIfFailed(mockDWriteParams.CopyTo(renderingParams));

                    return S_OK;
                });

            m_mockDWritefactory->CreateCustomRenderingParamsMethod1.AllowAnyCall(
                [&](IDWriteRenderingParams2** renderingParams)
                {
                    auto mockDWriteParams = Make<MockDWriteRenderingParams>();

                    ThrowIfFailed(mockDWriteParams.CopyTo(renderingParams));

                    return S_OK;
                });

#if WINVER > _WIN32_WINNT_WINBLUE
            m_mockDWritefactory->CreateCustomRenderingParamsMethod2.AllowAnyCall(
                [&](FLOAT gamma, FLOAT enhancedContrast, FLOAT grayscaleEnhancedContrast, FLOAT clearTypeLevel, DWRITE_PIXEL_GEOMETRY pixelGeometry, DWRITE_RENDERING_MODE1 renderingMode, DWRITE_GRID_FIT_MODE gridFitMode, IDWriteRenderingParams3** renderingParams)
                {
                    auto mockDWriteParams = Make<MockDWriteRenderingParams>();

                    ThrowIfFailed(mockDWriteParams.CopyTo(renderingParams));

                    return S_OK;
                });
#endif
            }

        virtual IStorageFileStatics* GetStorageFileStatics() override
        {
            return StorageFileStatics.Get();
        }

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override
        {
            return m_mockDWritefactory;
        }

        virtual ComPtr<MockDWriteFactory> GetMockDWriteFactory()
        {
            return m_mockDWritefactory;
        }
    };
}
