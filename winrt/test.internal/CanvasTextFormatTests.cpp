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

namespace canvas
{
    //
    // Function that tests a simple property on CanvasTextFormat.
    //
    // It first checks that simple roundtripping works.
    //
    // Then it forces the dwrite object to get realized and it verifies
    // that the dwrite object has the correct value on it.
    //
    // Then it sets the property on the dwrite object and verifies that this
    // is reflected on the CanvasTextFormat object.
    //
    template<typename CANVAS_TYPE, typename DW_TYPE>
    void TestSimpleProperty(
        std::function<CANVAS_TYPE(CanvasTextFormat*)> canvasGetter,
        std::function<void(CanvasTextFormat*, CANVAS_TYPE)> canvasSetter,
        std::function<DW_TYPE(IDWriteTextFormat*)> dwriteGetter,
        std::function<void(IDWriteTextFormat*, DW_TYPE)> dwriteSetter,
        CANVAS_TYPE expectedValue,
        DW_TYPE realizedValue,
        DW_TYPE setRealizedValue,
        CANVAS_TYPE expectedRealizedValue)
    {
        auto ctf = Make<CanvasTextFormat>();

        //
        // Test roundtripping on unrealized format
        //
        canvasSetter(ctf.Get(), expectedValue);
        
        CANVAS_TYPE actualValue = canvasGetter(ctf.Get());

        Assert::AreEqual(expectedValue, actualValue);

        //
        // Check the realized object
        //        
        ComPtr<IDWriteTextFormat> dwf = ctf->GetRealizedTextFormat();

        auto actualDWriteValue = dwriteGetter(dwf.Get());
        Assert::AreEqual(realizedValue, actualDWriteValue);

        //
        // canvasGetter still works against a realized object
        //
        actualValue = canvasGetter(ctf.Get());
        Assert::AreEqual(expectedValue, actualValue);

        if (dwriteSetter) // not all properties can be set via dwrite
        {
            //
            // Modifying the realized object changes what the canvas object returns
            //
            dwriteSetter(dwf.Get(), setRealizedValue);
            
            actualValue = canvasGetter(ctf.Get());
            Assert::AreEqual(expectedRealizedValue, actualValue);
        }

        //
        // Check round-tripping on a realized format
        //
        ctf = Make<CanvasTextFormat>();
        dwf = ctf->GetRealizedTextFormat();

        canvasSetter(ctf.Get(), expectedValue);
        actualValue = canvasGetter(ctf.Get());
        Assert::AreEqual(expectedValue, actualValue);

        //
        // Setting a property that has a dwriteSetter should not cause the
        // property to be re-realized.
        //
        if (dwriteSetter)
        {
            canvasSetter(ctf.Get(), expectedRealizedValue);
            Assert::AreEqual(dwf.Get(), ctf->GetRealizedTextFormat().Get());
        }

        //
        // Setting a property that doesn't have a dwriteSetter would normally
        // cause the property to be re-realized -- unless we set it to exactly
        // the same value that it had previously in which case we want to leave
        // it alone.
        //
        if (!dwriteSetter)
        {
            dwf = ctf->GetRealizedTextFormat();
            canvasSetter(ctf.Get(), canvasGetter(ctf.Get()));
            Assert::AreEqual(dwf.Get(), ctf->GetRealizedTextFormat().Get());
        }

        //
        // Closing should cause any attempt to get/set to fail
        //
        ThrowIfFailed(ctf->Close());
        Assert::ExpectException<ObjectDisposedException>([&]() { canvasSetter(ctf.Get(), expectedValue); });
        Assert::ExpectException<ObjectDisposedException>([&]() { canvasGetter(ctf.Get()); });
    }

#define SIMPLE_CANVAS_GETTER(PROPERTY, TYPE)            \
    [](CanvasTextFormat* ctf)                           \
    {                                                   \
        TYPE value;                                     \
        ThrowIfFailed(ctf->get_ ## PROPERTY(&value));   \
        return value;                                   \
    }

#define SIMPLE_CANVAS_SETTER(PROPERTY, TYPE)            \
    [](CanvasTextFormat* ctf, TYPE value)               \
    {                                                   \
        ThrowIfFailed(ctf->put_ ## PROPERTY(value));    \
    }

#define SIMPLE_DWRITE_GETTER(PROPERTY)          \
    [](IDWriteTextFormat* dwf)                  \
    {                                           \
        return dwf->Get ## PROPERTY();          \
    }

#define SIMPLE_DWRITE_SETTER(PROPERTY, TYPE)        \
    [](IDWriteTextFormat* dwf, TYPE value)          \
    {                                               \
        ThrowIfFailed(dwf->Set ## PROPERTY(value)); \
    }

#define TEST_PROPERTY(CANVAS_PROPERTY, DWRITE_PROPERTY, EXPECTED, REALIZED, SET_REALIZED, EXPECTED_REALIZED) \
    TestSimpleProperty<decltype(EXPECTED), decltype(REALIZED)>(         \
        SIMPLE_CANVAS_GETTER(CANVAS_PROPERTY, decltype(EXPECTED)),      \
        SIMPLE_CANVAS_SETTER(CANVAS_PROPERTY, decltype(EXPECTED)),      \
        SIMPLE_DWRITE_GETTER(DWRITE_PROPERTY),                          \
        SIMPLE_DWRITE_SETTER(DWRITE_PROPERTY, decltype(REALIZED)),      \
        EXPECTED,                                                       \
        REALIZED,                                                       \
        SET_REALIZED,                                                   \
        EXPECTED_REALIZED)

#define TEST_SIMPLE_PROPERTY(PROPERTY, EXPECTED, REALIZED, SET_REALIZED, EXPECTED_REALIZED) \
    TEST_PROPERTY(PROPERTY, PROPERTY, EXPECTED, REALIZED, SET_REALIZED, EXPECTED_REALIZED)

#define TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(PROPERTY, EXPECTED, REALIZED) \
    TestSimpleProperty<decltype(EXPECTED), decltype(REALIZED)>(         \
        SIMPLE_CANVAS_GETTER(PROPERTY, decltype(EXPECTED)),             \
        SIMPLE_CANVAS_SETTER(PROPERTY, decltype(EXPECTED)),             \
        SIMPLE_DWRITE_GETTER(PROPERTY),                                 \
        nullptr,                                                        \
        EXPECTED,                                                       \
        REALIZED,                                                       \
        REALIZED,                                                       \
        EXPECTED)

    template<typename CANVAS_TYPE>
    void TestInvalidProperties(
        std::function<void(CanvasTextFormat*, CANVAS_TYPE)>&& canvasSetter,
        std::vector<CANVAS_TYPE> values)
    {
        auto ctf = Make<CanvasTextFormat>();

        for (auto& value : values)
        {
            Assert::ExpectException<InvalidArgException>(
                [&]() 
                {
                    canvasSetter(ctf.Get(), value); 
                });
        }
    }

    template<typename CANVAS_TYPE>
    void TestInvalidProperties(
        std::function<void(CanvasTextFormat*, CANVAS_TYPE)> canvasSetter,
        CANVAS_TYPE value1)
    {
        TestInvalidProperties(
            std::move(canvasSetter), 
            std::vector<CANVAS_TYPE>{value1});
    }

    template<typename CANVAS_TYPE>
    void TestInvalidProperties(
        std::function<void(CanvasTextFormat*, CANVAS_TYPE)> canvasSetter,
        CANVAS_TYPE value1,
        CANVAS_TYPE value2)
    {
        TestInvalidProperties(
            std::move(canvasSetter), 
            std::vector<CANVAS_TYPE>{value1, value2});
    }

#define TEST_INVALID_PROPERTIES(PROPERTY, INVALID1, ...)    \
    TestInvalidProperties<decltype(INVALID1)>(              \
        SIMPLE_CANVAS_SETTER(PROPERTY, decltype(INVALID1)), \
        INVALID1,                                           \
        __VA_ARGS__)    

    TEST_CLASS(CanvasTextFormatTests)
    {
    public:
        TEST_METHOD(CanvasTextFormat_Implements_Expected_Interfaces)
        {
            auto ctf = Make<CanvasTextFormat>();

            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasTextFormat);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasResourceWrapperNative);
        }

        TEST_METHOD(CanvasTextFormat_FlowDirection)
        {
            TEST_SIMPLE_PROPERTY(
                FlowDirection,
                CanvasTextDirection::BottomToTop,
                DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP,
                DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT,
                CanvasTextDirection::LeftToRight);

            TEST_INVALID_PROPERTIES(
                FlowDirection,
                static_cast<CanvasTextDirection>(999));
        }

        TEST_METHOD(CanvasTextFormat_FontFamilyName)
        {
            auto canvasGetter =
                [](CanvasTextFormat* ctf)
                {
                    WinString value;
                    ThrowIfFailed(ctf->get_FontFamilyName(value.GetAddressOf()));
                    return std::wstring(static_cast<const wchar_t*>(value));
                };

            auto canvasSetter =
                [](CanvasTextFormat* ctf, std::wstring value)
                {
                    ThrowIfFailed(ctf->put_FontFamilyName(WinString(value)));
                };

            auto dwriteGetter =
                [](IDWriteTextFormat* dwf)
                {
                    auto length = dwf->GetFontFamilyNameLength() + 1; // + 1 for NULL terminator
                    std::vector<wchar_t> buf(length);
                    ThrowIfFailed(dwf->GetFontFamilyName(&buf.front(), length));
                    return std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string
                };

            auto dwriteSetter = nullptr; // FontFamilyName can't be set from dwrite

            TestSimpleProperty<std::wstring, std::wstring>(
                canvasGetter,
                canvasSetter,
                dwriteGetter,
                dwriteSetter,
                L"font family",
                L"font family",
                L"realized font family",
                L"realized font family");
        }

        TEST_METHOD(CanvasTextFormat_FontSize)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontSize,
                1.0f,
                1.0f);

            TEST_INVALID_PROPERTIES(FontSize, nanf(""), -1.0f);
        }

        TEST_METHOD(CanvasTextFormat_FontStretch)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontStretch,
                ABI::Windows::UI::Text::FontStretch_Condensed,
                DWRITE_FONT_STRETCH_CONDENSED);

            TEST_INVALID_PROPERTIES(FontStretch, static_cast<ABI::Windows::UI::Text::FontStretch>(999));
        }

        TEST_METHOD(CanvasTextFormat_FontStyle)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontStyle,
                ABI::Windows::UI::Text::FontStyle_Italic,
                DWRITE_FONT_STYLE_ITALIC);

            TEST_INVALID_PROPERTIES(FontStyle, static_cast<ABI::Windows::UI::Text::FontStyle>(999));
        }

        TEST_METHOD(CanvasTextFormat_FontWeight)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontWeight,
                ABI::Windows::UI::Text::FontWeight{123},
                static_cast<DWRITE_FONT_WEIGHT>(123));

            TEST_INVALID_PROPERTIES(
                FontWeight,
                ABI::Windows::UI::Text::FontWeight{0},
                ABI::Windows::UI::Text::FontWeight{1000});
        }

        TEST_METHOD(CanvasTextFormat_IncrementalTabStop)
        {
            TEST_SIMPLE_PROPERTY(
                IncrementalTabStop,
                2.0f,
                2.0f,
                4.0f,
                4.0f);

            TEST_INVALID_PROPERTIES(
                IncrementalTabStop,
                -1.234f,
                nanf(""));
        }

        TEST_METHOD(CanvasTextFormat_LineSpacingMethod)
        {
            auto dwriteSetLineSpacingMethod = 
                [](IDWriteTextFormat* dwf, DWRITE_LINE_SPACING_METHOD value)
                {
                    DWriteLineSpacing current(dwf);
                    ThrowIfFailed(dwf->SetLineSpacing(value, current.Spacing, current.Baseline));
                };

            auto dwriteGetLineSpacingMethod =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteLineSpacing(dwf).Method;
                };

            TestSimpleProperty<CanvasLineSpacingMethod, DWRITE_LINE_SPACING_METHOD>(
                SIMPLE_CANVAS_GETTER(LineSpacingMethod, CanvasLineSpacingMethod),
                SIMPLE_CANVAS_SETTER(LineSpacingMethod, CanvasLineSpacingMethod),
                dwriteGetLineSpacingMethod,
                dwriteSetLineSpacingMethod,
                CanvasLineSpacingMethod::Default,
                DWRITE_LINE_SPACING_METHOD_DEFAULT,
                DWRITE_LINE_SPACING_METHOD_UNIFORM,
                CanvasLineSpacingMethod::Uniform);

            TEST_INVALID_PROPERTIES(
                LineSpacingMethod,
                static_cast<CanvasLineSpacingMethod>(999));
        }

        TEST_METHOD(CanvasTextFormat_LineSpacing)
        {
            auto dwriteSetLineSpacing = 
                [](IDWriteTextFormat* dwf, float value)
                {
                    DWriteLineSpacing current(dwf);
                    ThrowIfFailed(dwf->SetLineSpacing(current.Method, value, current.Baseline));
                };

            auto dwriteGetLineSpacing =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteLineSpacing(dwf).Spacing;
                };

            TestSimpleProperty<float, float>(
                SIMPLE_CANVAS_GETTER(LineSpacing, float),
                SIMPLE_CANVAS_SETTER(LineSpacing, float),
                dwriteGetLineSpacing,
                dwriteSetLineSpacing,
                2.0f,
                2.0f,
                4.0f,
                4.0f);

            TEST_INVALID_PROPERTIES(
                LineSpacing,
                -1.234f,
                nanf(""));
        }

        TEST_METHOD(CanvasTextFormat_LineSpacingBaseline)
        {
            auto dwriteSetLineSpacingBaseline = 
                [](IDWriteTextFormat* dwf, float value)
                {
                    DWriteLineSpacing current(dwf);
                    ThrowIfFailed(dwf->SetLineSpacing(current.Method, current.Spacing, value));
                };

            auto dwriteGetLineSpacingBaseline =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteLineSpacing(dwf).Baseline;
                };

            TestSimpleProperty<float, float>(
                SIMPLE_CANVAS_GETTER(LineSpacingBaseline, float),
                SIMPLE_CANVAS_SETTER(LineSpacingBaseline, float),
                dwriteGetLineSpacingBaseline,
                dwriteSetLineSpacingBaseline,
                2.0f,
                2.0f,
                4.0f,
                4.0f);

            TEST_INVALID_PROPERTIES(
                LineSpacing,
                nanf(""));
        }

        TEST_METHOD(CanvasTextFormat_LocaleName)
        {
            auto canvasGetter =
                [](CanvasTextFormat* ctf)
                {
                    WinString value;
                    ThrowIfFailed(ctf->get_LocaleName(value.GetAddressOf()));
                    return std::wstring(static_cast<const wchar_t*>(value));
                };

            auto canvasSetter =
                [](CanvasTextFormat* ctf, std::wstring value)
                {
                    ThrowIfFailed(ctf->put_LocaleName(WinString(value)));
                };

            auto dwriteGetter =
                [](IDWriteTextFormat* dwf)
                {
                    auto length = dwf->GetLocaleNameLength() + 1; // + 1 for NULL terminator
                    std::vector<wchar_t> buf(length);
                    ThrowIfFailed(dwf->GetLocaleName(&buf.front(), length));
                    return std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string
                };

            auto dwriteSetter = nullptr; // LocaleName can't be set from dwrite

            TestSimpleProperty<std::wstring, std::wstring>(
                canvasGetter,
                canvasSetter,
                dwriteGetter,
                dwriteSetter,
                L"en-us",
                L"en-us",
                L"en-us",
                L"en-us");            
        }

        TEST_METHOD(CanvasTextFormat_VerticalAlignment)
        {
            TEST_PROPERTY(
                VerticalAlignment,
                ParagraphAlignment,
                CanvasVerticalAlignment::Center,
                DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
                DWRITE_PARAGRAPH_ALIGNMENT_FAR,
                CanvasVerticalAlignment::Bottom);

            TEST_INVALID_PROPERTIES(
                VerticalAlignment,
                static_cast<CanvasVerticalAlignment>(999));
        }

        TEST_METHOD(CanvasTextFormat_ReadingDirection)
        {
            TEST_SIMPLE_PROPERTY(
                ReadingDirection,
                CanvasTextDirection::RightToLeft,
                DWRITE_READING_DIRECTION_RIGHT_TO_LEFT,
                DWRITE_READING_DIRECTION_TOP_TO_BOTTOM,
                CanvasTextDirection::TopToBottom);

            TEST_INVALID_PROPERTIES(
                ReadingDirection,
                static_cast<CanvasTextDirection>(999));
        }

        TEST_METHOD(CanvasTextFormat_ParagraphAlignment)
        {
            TEST_PROPERTY(
                ParagraphAlignment,
                TextAlignment,
                ABI::Windows::UI::Text::ParagraphAlignment_Center,
                DWRITE_TEXT_ALIGNMENT_CENTER,
                DWRITE_TEXT_ALIGNMENT_JUSTIFIED,
                ABI::Windows::UI::Text::ParagraphAlignment_Justify);

            TEST_INVALID_PROPERTIES(
                ParagraphAlignment,
                static_cast<ABI::Windows::UI::Text::ParagraphAlignment>(999));
        }

        TEST_METHOD(CanvasTextFormat_TrimmingGranularity)
        {
            auto dwriteGetter =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteTrimming(dwf).Options.granularity;
                };

            auto dwriteSetter =
                [](IDWriteTextFormat* dwf, DWRITE_TRIMMING_GRANULARITY value)
                {
                    DWriteTrimming current(dwf);
                    current.Options.granularity = value;
                    ThrowIfFailed(dwf->SetTrimming(&current.Options, current.Sign.Get()));
                };

            TestSimpleProperty<CanvasTextTrimmingGranularity, DWRITE_TRIMMING_GRANULARITY>(
                SIMPLE_CANVAS_GETTER(TrimmingGranularity, CanvasTextTrimmingGranularity),
                SIMPLE_CANVAS_SETTER(TrimmingGranularity, CanvasTextTrimmingGranularity),
                dwriteGetter,
                dwriteSetter,
                CanvasTextTrimmingGranularity::Character,
                DWRITE_TRIMMING_GRANULARITY_CHARACTER,
                DWRITE_TRIMMING_GRANULARITY_WORD,
                CanvasTextTrimmingGranularity::Word);

            TEST_INVALID_PROPERTIES(
                TrimmingGranularity,
                static_cast<CanvasTextTrimmingGranularity>(999));
        }

        TEST_METHOD(CanvasTextFormat_TrimmingDelimiter)
        {
            auto canvasGetter =
                [](CanvasTextFormat* ctf)
                {
                    WinString value;
                    ThrowIfFailed(ctf->get_TrimmingDelimiter(value.GetAddressOf()));
                    return std::wstring(static_cast<const wchar_t*>(value));
                };

            auto canvasSetter =
                [](CanvasTextFormat* ctf, std::wstring value)
                {
                    ThrowIfFailed(ctf->put_TrimmingDelimiter(WinString(value)));
                };

            auto dwriteGetter =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteTrimming(dwf).Options.delimiter;
                };

            auto dwriteSetter =
                [](IDWriteTextFormat* dwf, uint32_t value)
                {
                    DWriteTrimming current(dwf);
                    current.Options.delimiter = value;
                    ThrowIfFailed(dwf->SetTrimming(&current.Options, current.Sign.Get()));
                };

            TestSimpleProperty<std::wstring, uint32_t>(
                canvasGetter,
                canvasSetter,
                dwriteGetter,
                dwriteSetter,
                L"\u2026",      // horizontal ellipsis
                0x2026,
                0x22EE,         // vertical ellipsis
                L"\u22EE");

            TEST_INVALID_PROPERTIES(
                TrimmingDelimiter,
                WinString(L"123"));
        }


        TEST_METHOD(CanvasTextFormat_TrimmingDelimiterCount)
        {
            auto dwriteGetter =
                [](IDWriteTextFormat* dwf)
                {
                    return DWriteTrimming(dwf).Options.delimiterCount;
                };

            auto dwriteSetter =
                [](IDWriteTextFormat* dwf, uint32_t value)
                {
                    DWriteTrimming current(dwf);
                    current.Options.delimiterCount = value;
                    ThrowIfFailed(dwf->SetTrimming(&current.Options, current.Sign.Get()));
                };

            TestSimpleProperty<int32_t, uint32_t>(
                SIMPLE_CANVAS_GETTER(TrimmingDelimiterCount, int32_t),
                SIMPLE_CANVAS_SETTER(TrimmingDelimiterCount, int32_t),
                dwriteGetter,
                dwriteSetter,
                10,
                10U,
                20U,
                20);
            
            TEST_INVALID_PROPERTIES(
                TrimmingDelimiterCount,
                -1);
        }

        TEST_METHOD(CanvasTextFormat_WordWrapping)
        {
            TEST_SIMPLE_PROPERTY(
                WordWrapping,
                CanvasWordWrapping::EmergencyBreak,
                DWRITE_WORD_WRAPPING_EMERGENCY_BREAK,
                DWRITE_WORD_WRAPPING_WHOLE_WORD,
                CanvasWordWrapping::WholeWord);

            TEST_INVALID_PROPERTIES(
                WordWrapping,
                static_cast<CanvasWordWrapping>(999));
        }

        TEST_METHOD(CanvasTextFormat_Options)
        {
            // 'Options' isn't part of IDWriteTextFormat, and so we can't use
            // TestSimpleProperty with it.

            auto ctf = Make<CanvasTextFormat>();

            // Check the default value
            CanvasDrawTextOptions actualDefault;
            ThrowIfFailed(ctf->get_Options(&actualDefault));            
            Assert::AreEqual(CanvasDrawTextOptions::Default, actualDefault);

            // Check a round-trip
            CanvasDrawTextOptions expected = CanvasDrawTextOptions::NoSnap;
            CanvasDrawTextOptions actual{};
            ThrowIfFailed(ctf->put_Options(expected));
            ThrowIfFailed(ctf->get_Options(&actual));
            Assert::AreEqual(expected, actual);

            // Check it fails when closed
            ThrowIfFailed(ctf->Close());
            Assert::AreEqual(RO_E_CLOSED, ctf->put_Options(expected));
            Assert::AreEqual(RO_E_CLOSED, ctf->get_Options(&actual));

            TEST_INVALID_PROPERTIES(
                Options,
                static_cast<CanvasDrawTextOptions>(999));
        }

        //
        // TODO #1663: interop tests - create a IDWriteTextFormat with a bunch of
        // settings, wrap a CanvasTextFormat around it, do something that make
        // it re-realized, and check things match.
        //

    };

#undef TEST_SIMPLE_PROPERTY
#undef SIMPLE_DWRITE_SETTER
#undef SIMPLE_DWRITE_GETTER
#undef SIMPLE_CANVAS_SETTER
#undef SIMPLE_CANVAS_GETTER
}
