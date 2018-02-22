// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "StubDWriteFontCollection.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasTextLayoutTests)
{
    CanvasDevice^ m_device;
    CanvasTextFormat^ m_defaultFormat;

public:
    CanvasTextLayoutTests()
        : m_device(ref new CanvasDevice())
        , m_defaultFormat(ref new CanvasTextFormat())
    {
    }

    TEST_METHOD(CanvasTextLayout_Construction)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"Asdf", m_defaultFormat, 0, 0);
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
        std::wstring text = L"Asdf";
        ThrowIfFailed(factory->CreateTextLayout(
            text.c_str(),
            static_cast<uint32_t>(text.length()),
            dwriteTextFormat.Get(),
            0,
            0,
            &dwriteTextLayout));

        auto canvasTextLayout = GetOrCreate<CanvasTextLayout>(m_device, dwriteTextLayout.Get());

        auto wrappedTextLayout = GetWrappedResource<IDWriteTextLayout>(canvasTextLayout);
        Assert::AreEqual<void*>(dwriteTextLayout.Get(), wrappedTextLayout.Get());
    }

    TEST_METHOD(CanvasTextLayoutTests_FormatChangePositions)
    {
        struct TestCase
        {
            Platform::String^ Str;
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

        for (auto testCase : testCases)
        {
            auto textLayout = ref new CanvasTextLayout(m_device, testCase.Str, m_defaultFormat, 0, 0);

            for (unsigned int i = 0; i < testCase.StrikethroughHere.size(); ++i)
            {
                textLayout->SetStrikethrough(testCase.StrikethroughHere[i], 1, true);
            }

            auto formatChangeElements = textLayout->GetFormatChangeIndices();

            Assert::AreEqual(static_cast<uint32_t>(testCase.ExpectedFormatChangeArray.size()), formatChangeElements->Length);
            for (unsigned int i = 0; i < formatChangeElements->Length; ++i)
            {
                Assert::AreEqual(testCase.ExpectedFormatChangeArray[i], formatChangeElements[i]);
            }
        }
    }

    TEST_METHOD(CanvasTextLayoutTests_get_LineMetrics_EmptyString)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"", m_defaultFormat, 0, 0);

        auto lineMetrics = layout->LineMetrics;

        Assert::AreEqual(1u, lineMetrics->Length);
        Assert::AreEqual(0, lineMetrics[0].CharacterCount);
    }

    TEST_METHOD(CanvasTextLayoutTests_get_ClusterMetrics_EmptyString)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"", m_defaultFormat, 0, 0);

        auto clusterMetrics = layout->ClusterMetrics;

        Assert::AreEqual(0u, clusterMetrics->Length);
    }

    TEST_METHOD(CanvasTextLayoutTests_get_ClusterMetrics_CharacterCount)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"นี้", m_defaultFormat, 0, 0);

        auto clusterMetrics = layout->ClusterMetrics;

        Assert::AreEqual(1u, clusterMetrics->Length);
        Assert::AreEqual(3, clusterMetrics[0].CharacterCount);
    }

    TEST_METHOD(CanvasTextLayoutTests_MaximumBidiReorderingDepth_DefaultIs1)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"", m_defaultFormat, 0, 0);

        Assert::AreEqual(1, layout->MaximumBidiReorderingDepth);

        auto layout2 = ref new CanvasTextLayout(m_device, L"Abc", m_defaultFormat, 0, 0);

        Assert::AreEqual(1, layout2->MaximumBidiReorderingDepth);
    }


    TEST_METHOD(CanvasTextLayoutTests_DefaultTypographyIsNull)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"Abc", m_defaultFormat, 0, 0);

        for (int i=0; i<3; ++i)
        {
            auto typography = layout->GetTypography(i);
            Assert::IsNull(typography);
        }
    }


    TEST_METHOD(CanvasTextLayoutTests_TypographyIsMutable)
    {
        auto layout = ref new CanvasTextLayout(m_device, L"Abc", m_defaultFormat, 0, 0);

        auto typography = ref new CanvasTypography();

        layout->SetTypography(2, 1, typography);

        typography->AddFeature(CanvasTypographyFeatureName::Default, 1);

        //
        // Make sure the same typography is set to the layout, even though we added a feature.
        //
        auto retrievedTypography = layout->GetTypography(2);

        Assert::AreEqual(typography, retrievedTypography);
    }
};
