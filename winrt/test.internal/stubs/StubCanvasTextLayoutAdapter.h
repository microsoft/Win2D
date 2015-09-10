// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <lib/text/CanvasTextLayout.h>
#include <lib/text/CanvasTextFormat.h>
#include "mocks/MockDWriteFactory.h"
#include "mocks/MockDWriteTextLayout.h"
#include "mocks/MockDWriteFontCollection.h"
#include "stubs/StubStorageFileStatics.h"
#include "stubs/StubDWriteTextFormat.h"

namespace canvas
{
    class StubTextLayout : public MockDWriteTextLayout
    {
    public:
        StubTextLayout()
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
                [](DWRITE_LINE_SPACING_METHOD* lineSpacingMethod, FLOAT* lineSpacing, FLOAT* baseline)
            {
                *lineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;
                *lineSpacing = 0;
                *baseline = 0;
                return S_OK;
            });

            GetTrimmingMethod.AllowAnyCall();

        }
    };

    class StubCanvasTextLayoutAdapter : public CustomFontManagerAdapter
    {
        ComPtr<MockDWriteFactory> m_mockDwritefactory;

    public:

        ComPtr<StubTextLayout> MockTextLayout;
        ComPtr<StubStorageFileStatics> StorageFileStatics;

        StubCanvasTextLayoutAdapter()
            : m_mockDwritefactory(Make<MockDWriteFactory>())
            , MockTextLayout(Make<StubTextLayout>())
            , StorageFileStatics(Make<StubStorageFileStatics>())
        {

            m_mockDwritefactory->CreateTextLayoutMethod.AllowAnyCall(
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


            m_mockDwritefactory->CreateTextFormatMethod.AllowAnyCall(
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

            m_mockDwritefactory->RegisterFontCollectionLoaderMethod.AllowAnyCall();
        }

        virtual IStorageFileStatics* GetStorageFileStatics() override
        {
            return StorageFileStatics.Get();
        }

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override
        {
            return m_mockDwritefactory;
        }

        virtual ComPtr<MockDWriteFactory> GetMockDWriteFactory()
        {
            return m_mockDwritefactory;
        }
    };
}
