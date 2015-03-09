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

#include "pch.h"

#include "StubDWriteFontCollection.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasTextLayoutTests)
{
    TEST_METHOD(CanvasTextLayout_Construction)
    {
        auto format = ref new CanvasTextFormat();

        auto layout = ref new CanvasTextLayout(L"Asdf", format.Get(), 0, 0);
    }

    TEST_METHOD(CanvasTextLayout_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));
        auto fontCollection = Make<StubDWriteFontCollection>();

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(factory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        ComPtr<IDWriteTextLayout> dwriteTextLayout;
        std::wstring text = L"Asdf;"
        ThrowIfFailed(factory->CreateTextLayout(
            text.c_str(),
            text.length(),
            dwriteTextFormat.Get(),
            0,
            0,
            &dwriteTextLayout));

        auto canvasTextLayout = GetOrCreate<CanvasTextLayout>(dwriteTextLayout.Get());

        auto wrappedTextLayout = GetWrappedResource<IDWriteTextLayout>(canvasTextLayout);
        Assert::AreEqual<void*>(dwriteTextLayout.Get(), wrappedTextLayout.Get());
    }

    TEST_METHOD(CanvasTextLayoutTests_FormatChangePositions)
    {
        struct TestCase
        {
            std::wstring Str;
            std::vector<INT32> StrikethroughHere;
            std::vector<INT32> ExpectedFormatChangeArray;
        } testCases[]
        {
                L"", {}, { 0 },
                L"", { 999 }, { 0, 999, 1000 }, // Goes past length of string.

                L"a", {}, { 0 },
                L"a", { 0 }, { 0, 1 },

                L"Abcd", {}, { 0 },
                L"Abcd", { 0 }, { 0, 1 },
                L"Abcd", { 0, 2 }, { 0, 1, 2, 3 },
                L"Abcd", { 1, 3 }, { 0, 1, 2, 3, 4 },
                L"Abcd", { 1, 2 }, { 0, 1, 3 },
                L"Abcd", { 0, 1, 2, 3 }, { 0, 4 },
        };
        auto format = ref new CanvasTextFormat();

        for (auto testCase : testCases)
        {
            auto textLayout = ref new CanvasTextLayout(testCase.Str.c_str(), format.Get(), 0, 0);

            for (unsigned int i = 0; i < testCase.StrikethroughHere.size(); ++i)
            {
                Assert::AreEqual(S_OK, textLayout->SetStrikethrough(testCase.StrikethroughHere[i], 1, true));
            }

            auto formatChangeElements = textLayout->GetFormatChangePositions();

            Assert::AreEqual(testCase.ExpectedFormatChangeArray.size(), formatChangeElements.Count);
            for (unsigned int i = 0; i < formatChangeElementCount; ++i)
            {
                Assert::AreEqual(testCase.ExpectedFormatChangeArray[i], formatChangeElements[i]);
            }
        }
    }
};
