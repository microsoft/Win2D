// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

#include "CanvasTextLayout.h"
#include "CanvasTextFormat.h"
#include "MockDWriteFactory.h"
#include "MockDWriteTextLayout.h"
#include "MockDWriteFontCollection.h"
#include "StubStorageFileStatics.h"

namespace canvas
{
    class StubCanvasTextLayoutAdapter : public CanvasTextFormatAdapter
    {
        ComPtr<MockDWriteFactory> m_mockDwritefactory;

    public:

        ComPtr<MockDWriteTextLayout> MockTextLayout;
        ComPtr<StubStorageFileStatics> StorageFileStatics;

        StubCanvasTextLayoutAdapter()
            : m_mockDwritefactory(Make<MockDWriteFactory>())
            , MockTextLayout(Make<MockDWriteTextLayout>())
            , StorageFileStatics(Make<StubStorageFileStatics>())
        {
            MockTextLayout->GetFontCollection_BaseFormat_Method.AllowAnyCall(
                [](IDWriteFontCollection** fontCollection)
            {
                auto mockFontCollection = Make<MockDWriteFontCollection>();
                mockFontCollection.CopyTo(fontCollection);
                return S_OK;
            });

            MockTextLayout->GetFontFamilyNameLength_BaseFormat_Method.AllowAnyCall([] { return 1; });

            MockTextLayout->GetFontFamilyName_BaseFormat_Method.AllowAnyCall(
                [](WCHAR* localeName, UINT32 nameSize)
            {
                Assert::AreEqual(2u, nameSize); // Includes null term
                localeName[0] = L'A';
                localeName[1] = 0;
                return S_OK;
            });

            MockTextLayout->GetFlowDirectionMethod.AllowAnyCall([] { return DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM; });

            MockTextLayout->GetFontSize_BaseFormat_Method.AllowAnyCall([] { return 16.0f; });

            MockTextLayout->GetFontStretch_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_STRETCH_UNDEFINED; });

            MockTextLayout->GetFontStyle_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_STYLE_NORMAL; });

            MockTextLayout->GetFontWeight_BaseFormat_Method.AllowAnyCall([] { return DWRITE_FONT_WEIGHT_NORMAL; });

            MockTextLayout->GetIncrementalTabStopMethod.AllowAnyCall([] { return 0.0f; });

            MockTextLayout->GetLocaleNameLength_BaseFormat_Method.AllowAnyCall([] { return 1; });

            MockTextLayout->GetLocaleName_BaseFormat_Method.AllowAnyCall(
                [](WCHAR* localeName, UINT32 nameSize)
            {
                Assert::AreEqual(2u, nameSize);
                localeName[0] = L'A';
                return S_OK;
            });

            MockTextLayout->GetParagraphAlignmentMethod.AllowAnyCall([] { return DWRITE_PARAGRAPH_ALIGNMENT_CENTER; });

            MockTextLayout->GetTextAlignmentMethod.AllowAnyCall([] { return DWRITE_TEXT_ALIGNMENT_CENTER; });

            MockTextLayout->GetReadingDirectionMethod.AllowAnyCall([] { return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT; });

            MockTextLayout->GetWordWrappingMethod.AllowAnyCall([] { return DWRITE_WORD_WRAPPING_WRAP; });

            MockTextLayout->GetLineSpacingMethod.AllowAnyCall(
                [](DWRITE_LINE_SPACING_METHOD* lineSpacingMethod, FLOAT* lineSpacing, FLOAT* baseline)
            {
                *lineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;
                *lineSpacing = 0;
                *baseline = 0;
                return S_OK;
            });

            MockTextLayout->GetTrimmingMethod.AllowAnyCall();

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
