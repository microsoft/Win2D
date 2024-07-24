// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "mocks/MockDWriteFactory.h"
#include "mocks/MockDWriteFontCollection.h"
#include "mocks/MockDWriteFontFile.h"
#include "mocks/MockDWriteFontFamily.h"
#include "mocks/MockDWriteLocalizedStrings.h"
#include "mocks/MockDWriteTextRenderer.h"
#include "stubs/CustomInlineObject.h"
#include "stubs/CustomTextRenderer.h"
#include "stubs/LocalizedFontNames.h"
#include "stubs/StubStorageFileStatics.h"
#include "stubs/StubFontManagerAdapter.h"
#include "stubs/StubDWriteTextFormat.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "utils/TextHelpers.h"

#include <lib/brushes/CanvasSolidColorBrush.h>

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
        std::function<DW_TYPE(IDWriteTextFormat1*)> dwriteGetter,
        std::function<void(IDWriteTextFormat1*, DW_TYPE)> dwriteSetter,
        CANVAS_TYPE expectedValue,
        DW_TYPE realizedValue,
        DW_TYPE setRealizedValue,
        CANVAS_TYPE expectedRealizedValue)
    {
        auto adapter = std::make_shared<StubFontManagerAdapter>();
        CustomFontManagerAdapter::SetInstance(adapter);
        
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
        ComPtr<IDWriteTextFormat1> dwf = ctf->GetRealizedTextFormat();

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
        ExpectHResultException(RO_E_CLOSED, [&] { canvasSetter(ctf.Get(), expectedValue); });
        ExpectHResultException(RO_E_CLOSED, [&] { canvasGetter(ctf.Get()); });
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
    [](IDWriteTextFormat1* dwf)                 \
    {                                           \
        return dwf->Get ## PROPERTY();          \
    }

#define SIMPLE_DWRITE_SETTER(PROPERTY, TYPE)        \
    [](IDWriteTextFormat1* dwf, TYPE value)         \
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
            ExpectHResultException(E_INVALIDARG,
                [&] 
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
            std::vector<CANVAS_TYPE>{ value1 });
    }

    template<typename CANVAS_TYPE>
    void TestInvalidProperties(
        std::function<void(CanvasTextFormat*, CANVAS_TYPE)> canvasSetter,
        CANVAS_TYPE value1,
        CANVAS_TYPE value2)
    {
        TestInvalidProperties(
            std::move(canvasSetter), 
            std::vector<CANVAS_TYPE>{ value1, value2 });
    }

#define TEST_INVALID_PROPERTIES(PROPERTY, INVALID1, ...)    \
    TestInvalidProperties<decltype(INVALID1)>(              \
        SIMPLE_CANVAS_SETTER(PROPERTY, decltype(INVALID1)), \
        INVALID1,                                           \
        __VA_ARGS__)    

    static const CanvasTrimmingSign sc_trimmingSigns[] = { CanvasTrimmingSign::None, CanvasTrimmingSign::Ellipsis };

    TEST_CLASS(CanvasTextFormatTests)
    {
    public:
        TEST_METHOD_EX(CanvasTextFormat_Implements_Expected_Interfaces)
        {
            auto ctf = Make<CanvasTextFormat>();

            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasTextFormat);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasTextFormat_Direction_DefaultValue)
        {
            auto ctf = Make<CanvasTextFormat>();
            CanvasTextDirection actualDirection;
            ThrowIfFailed(ctf->get_Direction(&actualDirection));
            Assert::AreEqual(CanvasTextDirection::LeftToRightThenTopToBottom, actualDirection);
        }

        TEST_METHOD_EX(CanvasTextFormat_Direction_ValidValues)
        {
            struct Case
            {
                DWRITE_READING_DIRECTION DReading;
                DWRITE_FLOW_DIRECTION DFlow;
                CanvasTextDirection CDirection;
            };

            Case cases[] = 
                { 
                    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM, CanvasTextDirection::LeftToRightThenTopToBottom },
                    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM, CanvasTextDirection::RightToLeftThenTopToBottom },
                    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP, CanvasTextDirection::LeftToRightThenBottomToTop },
                    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP, CanvasTextDirection::RightToLeftThenBottomToTop },
                    { DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT, CanvasTextDirection::TopToBottomThenLeftToRight },
                    { DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT, CanvasTextDirection::BottomToTopThenLeftToRight },
                    { DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, CanvasTextDirection::TopToBottomThenRightToLeft },
                    { DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, CanvasTextDirection::BottomToTopThenRightToLeft },
                };

            for (int i = 0; i < _countof(cases); ++i)
            {
                auto const& testCase = cases[i];
                auto const& otherTestCase = cases[(i+1) % _countof(cases)];

                auto ctf = Make<CanvasTextFormat>();

                // Setting CanvasTextFormat.Direction and then getting the
                // DWrite object should set the correct values
                ThrowIfFailed(ctf->put_Direction(testCase.CDirection));

                auto dtf = GetWrappedResource<IDWriteTextFormat>(ctf);

                auto actualReadingDirection = dtf->GetReadingDirection();
                auto actualFlowDirection = dtf->GetFlowDirection();

                Assert::AreEqual(testCase.DReading, actualReadingDirection);
                Assert::AreEqual(testCase.DFlow, actualFlowDirection);


                // Setting the DWrite properties should update the
                // CanvasTextFormat property
                dtf->SetReadingDirection(testCase.DReading);
                dtf->SetFlowDirection(testCase.DFlow);

                CanvasTextDirection actualTextDirection;
                ThrowIfFailed(ctf->get_Direction(&actualTextDirection));
                Assert::AreEqual(testCase.CDirection, actualTextDirection);

                // Test writing to CanvasTextFormat should be reflected in the
                // DWrite properties.  
                ThrowIfFailed(ctf->put_Direction(otherTestCase.CDirection));
                actualReadingDirection = dtf->GetReadingDirection();
                actualFlowDirection = dtf->GetFlowDirection();

                Assert::AreEqual(otherTestCase.DReading, actualReadingDirection);
                Assert::AreEqual(otherTestCase.DFlow, actualFlowDirection);
            }
        }

        static std::wstring GetFontFamilyName(IDWriteTextFormat* dwf)
        {
            auto length = dwf->GetFontFamilyNameLength() + 1; // + 1 for NULL terminator
            std::vector<wchar_t> buf(length);
            ThrowIfFailed(dwf->GetFontFamilyName(&buf.front(), length));
            return std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string
        }

        TEST_METHOD_EX(CanvasTextFormat_FontFamily)
        {
            auto canvasGetter =
                [](CanvasTextFormat* ctf)
                {
                    WinString value;
                    ThrowIfFailed(ctf->get_FontFamily(value.GetAddressOf()));
                    return std::wstring(static_cast<const wchar_t*>(value));
                };

            auto canvasSetter =
                [](CanvasTextFormat* ctf, std::wstring value)
                {
                    ThrowIfFailed(ctf->put_FontFamily(WinString(value)));
                };

            auto dwriteGetter = GetFontFamilyName;
            auto dwriteSetter = nullptr; // FontFamily name can't be set from dwrite

            TestSimpleProperty<std::wstring, std::wstring>(
                canvasGetter,
                canvasSetter,
                dwriteGetter,
                dwriteSetter,
                L"font family",
                L"font family",
                L"<<unused>>",
                L"<<unused>>");

            TestSimpleProperty<std::wstring, std::wstring>(
                canvasGetter,
                canvasSetter,
                dwriteGetter,
                dwriteSetter,
                L"uri#family",
                L"family",
                L"<<unused>>",
                L"<<unused>>");
        }

        TEST_METHOD_EX(CanvasTextFormat_FontSize)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontSize,
                1.0f,
                1.0f);

            TEST_INVALID_PROPERTIES(FontSize, nanf(""), -1.0f);
        }

        TEST_METHOD_EX(CanvasTextFormat_FontStretch)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontStretch,
                ABI::Windows::UI::Text::FontStretch_Condensed,
                DWRITE_FONT_STRETCH_CONDENSED);

            TEST_INVALID_PROPERTIES(FontStretch, static_cast<ABI::Windows::UI::Text::FontStretch>(999));
        }

        TEST_METHOD_EX(CanvasTextFormat_FontStyle)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontStyle,
                ABI::Windows::UI::Text::FontStyle_Italic,
                DWRITE_FONT_STYLE_ITALIC);

            TEST_INVALID_PROPERTIES(FontStyle, static_cast<ABI::Windows::UI::Text::FontStyle>(999));
        }

        TEST_METHOD_EX(CanvasTextFormat_FontWeight)
        {
            TEST_SIMPLE_DWRITE_IMMUTABLE_PROPERTY(
                FontWeight,
                ABI::Windows::UI::Text::FontWeight{ 123 },
                static_cast<DWRITE_FONT_WEIGHT>(123));

            TEST_INVALID_PROPERTIES(
                FontWeight,
                ABI::Windows::UI::Text::FontWeight{ 0 },
                ABI::Windows::UI::Text::FontWeight{ 1000 });
        }

        TEST_METHOD_EX(CanvasTextFormat_IncrementalTabStop)
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

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_DefaultValue)
        {
            auto ctf = Make<CanvasTextFormat>();

            float lineSpacing{};
            ThrowIfFailed(ctf->get_LineSpacing(&lineSpacing));

            Assert::IsTrue(lineSpacing < 0, L"LineSpacing defaults to a negative value");
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_DefaultMethod_AllValuesRoundTripToAndFromDWrite)
        {
            auto values = { -100.0f, -50.0f, -1.0f, 0.0f, 0.1f, 1.0f, 100.0f };

            for (auto value : values)
            {
                // Create a CanvasTextFormat with LineSpacing set to the value
                auto ctf = Make<CanvasTextFormat>();
                ThrowIfFailed(ctf->put_LineSpacing(value));

                // Get the underlying IDWriteTextFormat, and wrap a new
                // CanvasTextFormat around it
                auto dtf = GetWrappedResource<IDWriteTextFormat1>(ctf);

                ctf.Reset();

                ctf = Make<CanvasTextFormat>(dtf.Get());

                // The LineSpacing value should have made it back again
                float roundTrippedValue;
                ThrowIfFailed(ctf->get_LineSpacing(&roundTrippedValue));

                Assert::AreEqual(value, roundTrippedValue);

                // Even after unrealizing 
                ThrowIfFailed(ctf->put_FontSize(123));

                ThrowIfFailed(ctf->get_LineSpacing(&roundTrippedValue));
                Assert::AreEqual(value, roundTrippedValue);

            }
        }


        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_InteropToAndFromDWrite_AllSpacingMethods)
        {
            struct TestCase
            {
                CanvasLineSpacingMode OriginalLineSpacingMode;
                float OriginalLineSpacing;

                DWRITE_LINE_SPACING_METHOD UnwrappedLineSpacingMethod;
                float UnwrappedLineSpacing;

                CanvasLineSpacingMode WrappedLineSpacingMode;
                float WrappedLineSpacing;
            } testCases[]
            {
                { CanvasLineSpacingMode::Default, 5.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 5.0f, CanvasLineSpacingMode::Default, 5.0f},
                { CanvasLineSpacingMode::Default, -1.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT, 1.0f, CanvasLineSpacingMode::Default, -1.0f },
                
                { CanvasLineSpacingMode::Uniform, 5.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 5.0f, CanvasLineSpacingMode::Default, 5.0f},
                { CanvasLineSpacingMode::Uniform, -11.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 11.0f, CanvasLineSpacingMode::Default, 11.0f },
                
                { CanvasLineSpacingMode::Proportional, 5.0f, DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 5.0f, CanvasLineSpacingMode::Proportional, 5.0f},
                { CanvasLineSpacingMode::Proportional, -22.0f, DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 22.0f, CanvasLineSpacingMode::Proportional, 22.0f }
            };

            for (auto testCase : testCases)
            {
                // Create a CanvasTextFormat with LineSpacing set to the value
                auto ctf = Make<CanvasTextFormat>();
                ThrowIfFailed(ctf->put_LineSpacing(testCase.OriginalLineSpacing));
                ThrowIfFailed(ctf->put_LineSpacingMode(testCase.OriginalLineSpacingMode));

                // Get the underlying IDWriteTextFormat, and wrap a new
                // CanvasTextFormat around it
                auto dtf = GetWrappedResource<IDWriteTextFormat1>(ctf);

                DWRITE_LINE_SPACING_METHOD dwriteMethod;
                float dwriteSpacing, unusedBaseline;
                ThrowIfFailed(dtf->GetLineSpacing(&dwriteMethod, &dwriteSpacing, &unusedBaseline));
                Assert::AreEqual(testCase.UnwrappedLineSpacing, dwriteSpacing);
                Assert::AreEqual(testCase.UnwrappedLineSpacingMethod, dwriteMethod);

                ctf.Reset();

                ctf = Make<CanvasTextFormat>(dtf.Get());

                float wrappedSpacingValue;
                ThrowIfFailed(ctf->get_LineSpacing(&wrappedSpacingValue));
                Assert::AreEqual(testCase.WrappedLineSpacing, wrappedSpacingValue);

                CanvasLineSpacingMode wrappedSpacingMode;
                ThrowIfFailed(ctf->get_LineSpacingMode(&wrappedSpacingMode));
                Assert::AreEqual(testCase.WrappedLineSpacingMode, wrappedSpacingMode);

                // Even after unrealizing 
                ThrowIfFailed(ctf->put_FontSize(123));

                ThrowIfFailed(ctf->get_LineSpacing(&wrappedSpacingValue));
                Assert::AreEqual(testCase.WrappedLineSpacing, wrappedSpacingValue);

                ThrowIfFailed(ctf->get_LineSpacingMode(&wrappedSpacingMode));
                Assert::AreEqual(testCase.WrappedLineSpacingMode, wrappedSpacingMode);
            }
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_RealizationDoesntClobberUniform)
        {
            auto ctf = Make<CanvasTextFormat>();
            ThrowIfFailed(ctf->put_LineSpacing(5.0f));
            ThrowIfFailed(ctf->put_LineSpacingMode(CanvasLineSpacingMode::Uniform));

            // Force realization.
            auto dtf = GetWrappedResource<IDWriteTextFormat>(ctf);

            // Shouldn't've changed from Uniform.
            CanvasLineSpacingMode spacingMode;
            ThrowIfFailed(ctf->get_LineSpacingMode(&spacingMode));
            Assert::AreEqual(CanvasLineSpacingMode::Uniform, spacingMode);
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacingMode_ReadBackAfterRealization)
        {
            for (int i = 0; i < 2; ++i)
            {
                auto ctf = Make<CanvasTextFormat>();
                ThrowIfFailed(ctf->put_LineSpacingMode(CanvasLineSpacingMode::Uniform));
                ThrowIfFailed(ctf->put_LineSpacing(i==0? -1.0f : 5.0f));                

                // Force realization.
                auto dtf = GetWrappedResource<IDWriteTextFormat>(ctf);

                ThrowIfFailed(ctf->put_LineSpacingMode(CanvasLineSpacingMode::Default));

                CanvasLineSpacingMode spacingMode;
                ThrowIfFailed(ctf->get_LineSpacingMode(&spacingMode));
                Assert::AreEqual(CanvasLineSpacingMode::Default, spacingMode);
            }
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_MethodIsDeterminedByValueOfLineSpacing)
        {
            struct Case { float Value; DWRITE_LINE_SPACING_METHOD Method; };
            Case cases[] = { { -100.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT },
                             {   -1.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT },
                             {    0.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM },
                             {    1.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM },
                             {  100.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM } };
            
            for (auto c : cases)
            {
                auto ctf = Make<CanvasTextFormat>();
                ThrowIfFailed(ctf->put_LineSpacing(c.Value));

                auto dtf = GetWrappedResource<IDWriteTextFormat>(ctf);

                DWriteLineSpacing dwriteLineSpacing(dtf.Get());
                
                Assert::AreEqual(c.Method, dwriteLineSpacing.Method);
                Assert::AreEqual(fabs(c.Value), dwriteLineSpacing.Spacing);
            }
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing_WrappedFormatWithDefaultMethodAndZeroLineSpacingWorksAsExpected)
        {
            // Create a IDWriteTextFormat...
            auto ctf = Make<CanvasTextFormat>();
            auto dtf = GetWrappedResource<IDWriteTextFormat1>(ctf);

            // Set the line spacing to DEFAULT/0.
            ThrowIfFailed(dtf->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 0.0f, 0.0f));

            // Wrap a new CanvasTextFormat around it
            ctf.Reset();

            ctf = Make<CanvasTextFormat>(dtf.Get());

            // Force it to re-realize
            ThrowIfFailed(ctf->put_FontSize(123));

            dtf = GetWrappedResource<IDWriteTextFormat1>(ctf);

            // Verify that the line spacing method is still DEFAULT
            DWriteLineSpacing spacing(dtf.Get());
            Assert::AreEqual(DWRITE_LINE_SPACING_METHOD_DEFAULT, spacing.Method);
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacing)
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
                nanf(""));
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacingBaseline_DefaultValueIs1)
        {
            auto ctf = Make<CanvasTextFormat>();

            float baseline;
            ThrowIfFailed(ctf->get_LineSpacingBaseline(&baseline));

            Assert::AreEqual(1.0f, baseline);
        }

        TEST_METHOD_EX(CanvasTextFormat_LineSpacingBaseline)
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

        TEST_METHOD_EX(CanvasTextFormat_LocaleName)
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

        TEST_METHOD_EX(CanvasTextFormat_VerticalAlignment)
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

        TEST_METHOD_EX(CanvasTextFormat_HorizontalAlignment)
        {
            TEST_PROPERTY(
                HorizontalAlignment,
                TextAlignment,
                CanvasHorizontalAlignment::Center,
                DWRITE_TEXT_ALIGNMENT_CENTER,
                DWRITE_TEXT_ALIGNMENT_JUSTIFIED,
                CanvasHorizontalAlignment::Justified);

            TEST_INVALID_PROPERTIES(
                HorizontalAlignment,
                static_cast<CanvasHorizontalAlignment>(999));
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingGranularity)
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

        TEST_METHOD_EX(CanvasTextFormat_TrimmingDelimiter)
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


        TEST_METHOD_EX(CanvasTextFormat_TrimmingDelimiterCount)
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

        TEST_METHOD_EX(CanvasTextFormat_WordWrapping)
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

        TEST_METHOD_EX(CanvasTextFormat_Options)
        {
            // 'Options' isn't part of IDWriteTextFormat, and so we can't use
            // TestSimpleProperty with it.

            auto ctf = Make<CanvasTextFormat>();

            // Check the default value
            CanvasDrawTextOptions actualDefault;
            ThrowIfFailed(ctf->get_Options(&actualDefault));            
            Assert::AreEqual(CanvasDrawTextOptions::Default, actualDefault);

            // Check a round-trip
            CanvasDrawTextOptions expected = CanvasDrawTextOptions::NoPixelSnap;
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

        TEST_METHOD_EX(CanvasTextFormat_VerticalGlyphOrientation)
        {
            //
            // For properties with only two settings where one of them is the
            // default, this tests both orderings of the properties to ensure
            // things aren't somehow working by accident.
            //
            TEST_PROPERTY(
                VerticalGlyphOrientation,
                VerticalGlyphOrientation,
                CanvasVerticalGlyphOrientation::Stacked,
                DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED,
                DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT,
                CanvasVerticalGlyphOrientation::Default);

            TEST_PROPERTY(
                VerticalGlyphOrientation,
                VerticalGlyphOrientation,
                CanvasVerticalGlyphOrientation::Default,
                DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT,
                DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED,
                CanvasVerticalGlyphOrientation::Stacked);

            TEST_INVALID_PROPERTIES(
                VerticalGlyphOrientation,
                static_cast<CanvasVerticalGlyphOrientation>(999));
        }

        TEST_METHOD_EX(CanvasTextFormat_OpticalAlignment)
        {
            TEST_PROPERTY(
                OpticalAlignment,
                OpticalAlignment,
                CanvasOpticalAlignment::NoSideBearings,
                DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS,
                DWRITE_OPTICAL_ALIGNMENT_NONE,
                CanvasOpticalAlignment::Default);

            TEST_PROPERTY(
                OpticalAlignment,
                OpticalAlignment,
                CanvasOpticalAlignment::Default,
                DWRITE_OPTICAL_ALIGNMENT_NONE,
                DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS,
                CanvasOpticalAlignment::NoSideBearings);

            TEST_INVALID_PROPERTIES(
                OpticalAlignment,
                static_cast<CanvasOpticalAlignment>(999));
        }

        TEST_METHOD_EX(CanvasTextFormat_LastLineWrapping)
        {
            TEST_PROPERTY(
                LastLineWrapping,
                LastLineWrapping,
                static_cast<boolean>(false),
                FALSE,
                TRUE,
                static_cast<boolean>(true));

            TEST_PROPERTY(
                LastLineWrapping,
                LastLineWrapping,
                static_cast<boolean>(true),
                TRUE,
                FALSE,
                static_cast<boolean>(false));
        }

        TEST_METHOD_EX(CanvasTextFormat_GetResourceFailsWhenClosed)
        {
            auto ctf = Make<CanvasTextFormat>();
            ThrowIfFailed(ctf->Close());
            
            ComPtr<IUnknown> resource;
            Assert::AreEqual(RO_E_CLOSED, ctf->GetNativeResource(nullptr, 0, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())));
        }

#undef TEST_SIMPLE_PROPERTY
#undef SIMPLE_DWRITE_SETTER
#undef SIMPLE_DWRITE_GETTER
#undef SIMPLE_CANVAS_SETTER
#undef SIMPLE_CANVAS_GETTER

        class StubFontManagerAdapterWithDWriteFactory : public StubFontManagerAdapter
        {
        public:
            ComPtr<MockDWriteFactory> DWriteFactory;

            StubFontManagerAdapterWithDWriteFactory()
                : DWriteFactory(Make<MockDWriteFactory>())
            {
            }

            virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override
            {
                return DWriteFactory;
            }
        };

        struct CustomFontFixture
        {
            std::shared_ptr<StubFontManagerAdapterWithDWriteFactory> Adapter;

            WinString AnyFullFontFamilyName;
            std::wstring AnyPath;
            WinString AnyFontFamily;

            WinString AnyOtherFullFontFamilyName;
            std::wstring AnyOtherPath;

            CustomFontFixture()
                : Adapter(std::make_shared<StubFontManagerAdapterWithDWriteFactory>())
                , AnyFullFontFamilyName(L"any_uri#any_font_family")
                , AnyPath(StubStorageFileStatics::GetFakePath(WinString(L"ms-appx:///any_uri")))
                , AnyFontFamily(L"any_font_family")
                , AnyOtherFullFontFamilyName(L"any_other_uri#any_other_font_family")
                , AnyOtherPath(StubStorageFileStatics::GetFakePath(WinString(L"ms-appx:///any_other_uri")))
            {
                CustomFontManagerAdapter::SetInstance(Adapter);

                Adapter->DWriteFactory->RegisterFontCollectionLoaderMethod.AllowAnyCall();
                
                Adapter->DWriteFactory->CreateTextFormatMethod.AllowAnyCall(
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
            }

            ComPtr<MockDWriteFontCollection> ExpectCreateCustomFontCollection(std::wstring expectedFilename)
            {
                auto collection = Make<MockDWriteFontCollection>();
                auto fontFile = Make<MockDWriteFontFile>();

                Adapter->DWriteFactory->CreateCustomFontCollectionMethod.SetExpectedCalls(1,
                    [=] (IDWriteFontCollectionLoader* loader, void const* key, uint32_t keySize, IDWriteFontCollection** outCollection)
                    {
                        std::wstring actualFilename(static_cast<wchar_t const*>(key), keySize / 2);
                        Assert::AreEqual(expectedFilename, actualFilename);

                        // DWriteFactory will call methods on the loader to try
                        // and load the font.  We can validate here that they
                        // behave as expected.

                        ComPtr<IDWriteFontFileEnumerator> enumerator;
                        ThrowIfFailed(loader->CreateEnumeratorFromKey(
                            Adapter->DWriteFactory.Get(),
                            key,
                            keySize,
                            &enumerator));

                        // We only expect the enumerator to return one file
                        BOOL hasFile = FALSE;
                        ThrowIfFailed(enumerator->MoveNext(&hasFile));
                        Assert::IsTrue(!!hasFile);

                        ComPtr<IDWriteFontFile> currentFile;
                        ThrowIfFailed(enumerator->GetCurrentFontFile(&currentFile));
                        Assert::IsTrue(IsSameInstance(currentFile.Get(), fontFile.Get()));

                        ThrowIfFailed(enumerator->MoveNext(&hasFile));
                        Assert::IsFalse(!!hasFile);

                        return collection.CopyTo(outCollection);
                    });

                // We expect that when we use the font file enumerator above
                // that it'll call CreateFontFileReference to reference the font
                // file.
                Adapter->DWriteFactory->CreateFontFileReferenceMethod.SetExpectedCalls(1,
                    [=] (wchar_t const* filePath, FILETIME const* lastWriteTime, IDWriteFontFile** outFontFile)
                    {
                        Assert::AreEqual(expectedFilename.c_str(), filePath);
                        Assert::IsNull(lastWriteTime);
                        return fontFile.CopyTo(outFontFile);
                    });

                return collection;
            }

            void DontExpectCreateCustomFontCollection()
            {
                Adapter->DWriteFactory->CreateCustomFontCollectionMethod.SetExpectedCalls(0);
                Adapter->DWriteFactory->CreateFontFileReferenceMethod.SetExpectedCalls(0);
            }
        };

        TEST_METHOD_EX(CanvasTextFormat_WhenFontFamilyNameDoesNotContainAUri_CustomFontCollectionIsNotUsed)
        {
            CustomFontFixture f;
            f.DontExpectCreateCustomFontCollection();

            auto cf = Make<CanvasTextFormat>();
            ThrowIfFailed(cf->put_FontFamily(WinString(L"family with no uri")));
            cf->GetRealizedTextFormat();
        }

        TEST_METHOD_EX(CanvasTextFormat_WhenFontFamilyContainsABlankUri_CustomFontCollectionIsNotUsed)
        {
            CustomFontFixture f;
            f.DontExpectCreateCustomFontCollection();

            auto cf = Make<CanvasTextFormat>();
            ThrowIfFailed(cf->put_FontFamily(WinString(L"#family with blank uri")));
            cf->GetRealizedTextFormat();
        }
        

        TEST_METHOD_EX(CanvasTextFormat_WhenFontFamilyNameContainsAUri_CustomFontCollectionIsUsed_And_UriStrippedFromRealizedName)
        {
            CustomFontFixture f;

            auto fontCollectionReturnedFromAdapter = f.ExpectCreateCustomFontCollection(f.AnyPath);

            auto cf = Make<CanvasTextFormat>();
            ThrowIfFailed(cf->put_FontFamily(f.AnyFullFontFamilyName));
            auto df = cf->GetRealizedTextFormat();

            ComPtr<IDWriteFontCollection> fontCollection;
            ThrowIfFailed(df->GetFontCollection(&fontCollection));

            Assert::IsTrue(IsSameInstance(fontCollectionReturnedFromAdapter.Get(), fontCollection.Get()));

            auto realizedFontFamily = GetFontFamilyName(df.Get());
            Assert::AreEqual(static_cast<wchar_t const*>(f.AnyFontFamily), realizedFontFamily.c_str());
        }

        TEST_METHOD_EX(CanvasTextFormat_FontCollectionIsPreservedIfFontFamilyNameUnchanged)
        {
            CustomFontFixture f;

            f.ExpectCreateCustomFontCollection(f.AnyPath);

            auto cf1 = Make<CanvasTextFormat>();
            ThrowIfFailed(cf1->put_FontFamily(f.AnyFullFontFamilyName));
            ThrowIfFailed(cf1->put_FontSize(1));

            auto df1 = cf1->GetRealizedTextFormat();

            f.DontExpectCreateCustomFontCollection();

            ThrowIfFailed(cf1->put_FontSize(2));
            
            auto df2 = cf1->GetRealizedTextFormat();

            // We have a new IDWriteTextFormat (since the font size changed)
            Assert::IsFalse(IsSameInstance(df1.Get(), df2.Get()));

            // ...but they should both refer to the same font collection
            ComPtr<IDWriteFontCollection> fc1;
            ThrowIfFailed(df1->GetFontCollection(&fc1));

            ComPtr<IDWriteFontCollection> fc2;
            ThrowIfFailed(df2->GetFontCollection(&fc2));

            Assert::IsTrue(IsSameInstance(fc1.Get(), fc2.Get()));
        }

        TEST_METHOD_EX(CanvasTextFormat_FontCollectionIsUpdatedIfFontFamilyNameChanged)
        {
            CustomFontFixture f;

            auto cf1 = Make<CanvasTextFormat>();
            ThrowIfFailed(cf1->put_FontFamily(f.AnyFullFontFamilyName));

            f.ExpectCreateCustomFontCollection(f.AnyPath);
            auto df1 = cf1->GetRealizedTextFormat();

            ThrowIfFailed(cf1->put_FontFamily(f.AnyOtherFullFontFamilyName));
                
            f.ExpectCreateCustomFontCollection(f.AnyOtherPath);
            auto df2 = cf1->GetRealizedTextFormat();

            // We have a new IDWriteTextFormat (since the font family changed)
            Assert::IsFalse(IsSameInstance(df1.Get(), df2.Get()));

            // ...and they refer to different font collections
            ComPtr<IDWriteFontCollection> fc1;
            ThrowIfFailed(df1->GetFontCollection(&fc1));

            ComPtr<IDWriteFontCollection> fc2;
            ThrowIfFailed(df2->GetFontCollection(&fc2));

            Assert::IsFalse(IsSameInstance(fc1.Get(), fc2.Get()));
        }

        TEST_METHOD_EX(CanvasTextFormat_WhenTextFormatRealized_FontFamilyNameIsUnmodified)
        {
            CustomFontFixture f;

            auto cf = Make<CanvasTextFormat>();
            ThrowIfFailed(cf->put_FontFamily(f.AnyFullFontFamilyName));
            
            WinString actualFontFamily;
            ThrowIfFailed(cf->get_FontFamily(actualFontFamily.GetAddressOf()));
            Assert::AreEqual(static_cast<wchar_t const*>(f.AnyFullFontFamilyName), static_cast<wchar_t const*>(actualFontFamily));

            f.ExpectCreateCustomFontCollection(f.AnyPath);
            cf->GetRealizedTextFormat();

            ThrowIfFailed(cf->get_FontFamily(actualFontFamily.GetAddressOf()));
            Assert::AreEqual(static_cast<wchar_t const*>(f.AnyFullFontFamilyName), static_cast<wchar_t const*>(actualFontFamily));
        }

        TEST_METHOD_EX(CanvasTextFormat_WhenGetFileFromApplicationUriFails_HelpfulErrorMessageIsThrown)
        {
            auto adapter = std::make_shared<StubFontManagerAdapter>();

            CustomFontManagerAdapter::SetInstance(adapter);

            adapter->StorageFileStatics->GetFileFromApplicationUriAsyncMethod.SetExpectedCalls(1, 
                [] (IUriRuntimeClass*, IAsyncOperation<StorageFile*>**)
                {
                    return E_INVALIDARG;
                });

            auto cf = Make<CanvasTextFormat>();
            ThrowIfFailed(cf->put_FontFamily(WinString(L"uri#family the actual value doesn't matter as long as there is a #")));
            ExpectHResultException(E_INVALIDARG, [&] { cf->GetRealizedTextFormat(); });
            ValidateStoredErrorState(E_INVALIDARG, Strings::InvalidFontFamilyUri);
        }

        TEST_METHOD_EX(CanvasTextFormat_PutFontFamily_ValidatesUriSchema)
        {
            auto cf = Make<CanvasTextFormat>();

            auto validFamilies = { L"", L"any family name with no uri", L"#family" };

            for (auto family : validFamilies)
            {
                Assert::AreEqual(S_OK, cf->put_FontFamily(WinString(family)), family);
            }

            auto makeFamilyName = 
                [] (wchar_t const* uri) 
                { 
                    return std::wstring(uri) + L"#anyfamily"; 
                };

            auto validUris = { L"foo/bar", L"foo", L"ms-appx:///foo", L"ms-appdata:///local/foo", L"filename with spaces", L"file://foo"};

            for (auto uri : validUris)
            {
                auto familyName = makeFamilyName(uri);
                Assert::AreEqual(S_OK, cf->put_FontFamily(WinString(familyName)), familyName.c_str());
            }

            auto invalidUriSchemes = { L"http://foo", L"anything:" };

            for (auto uri : invalidUriSchemes)
            {
                auto familyName = makeFamilyName(uri);
                Assert::AreEqual(E_INVALIDARG, cf->put_FontFamily(WinString(familyName)), familyName.c_str());
                ValidateStoredErrorState(E_INVALIDARG, Strings::InvalidFontFamilyUriScheme);
            }
        }

        class LocaleList : public Vector<HSTRING>
        {
        public:

            LocaleList(wchar_t const* str)
            {
                this->Append(WinString(str));
            }

            LocaleList(std::wstring str1, std::wstring str2)
            {
                this->Append(WinString(str1));
                this->Append(WinString(str2));
            }

            LocaleList(std::wstring str1, std::wstring str2, std::wstring str3)
            {
                this->Append(WinString(str1));
                this->Append(WinString(str2));
                this->Append(WinString(str3));
            }
        };

        static ComPtr<IVectorView<HSTRING>> GetVectorView(ComPtr<LocaleList> const& list)
        {
            ComPtr<IVectorView<HSTRING>> view;
            ThrowIfFailed(list->GetView(&view));
            return view;
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamilies_NullArg)
        {
            auto factory = Make<CanvasTextFormatFactory>();

            auto localeList = Make<LocaleList>(L"");
            uint32_t elementCount;
            HSTRING* element;

            Assert::AreEqual(E_INVALIDARG, factory->GetSystemFontFamilies(&elementCount, nullptr));
            Assert::AreEqual(E_INVALIDARG, factory->GetSystemFontFamilies(nullptr, &element));

            Assert::AreEqual(E_INVALIDARG, factory->GetSystemFontFamiliesFromLocaleList(GetVectorView(localeList).Get(), &elementCount, nullptr));
            Assert::AreEqual(E_INVALIDARG, factory->GetSystemFontFamiliesFromLocaleList(GetVectorView(localeList).Get(), nullptr, &element));
        }

        class SystemFontFamiliesFixture
        {
            std::shared_ptr<StubCanvasTextLayoutAdapter> m_adapter;
            ComPtr<MockDWriteFontCollection> m_fontCollection;
            ComPtr<MockDWriteFontFamily> m_fontFamily; // Only one is used here.
            ComPtr<LocalizedFontNames> m_localizedFontNames; // Only one

        public:
            SystemFontFamiliesFixture(int fontCollectionSize, ComPtr<LocalizedFontNames> const& localizedFontNames)
            {
                m_adapter = std::make_shared<StubCanvasTextLayoutAdapter>();
                m_localizedFontNames = localizedFontNames;

                m_fontCollection = Make<MockDWriteFontCollection>();

                m_fontCollection->GetFontFamilyCountMethod.SetExpectedCalls(1,
                    [fontCollectionSize]
                    {
                        return fontCollectionSize; 
                    });

                m_fontFamily = Make<MockDWriteFontFamily>();
                m_fontFamily->GetFamilyNamesMethod.SetExpectedCalls(fontCollectionSize,
                    [=](IDWriteLocalizedStrings** out)
                    {
                        return m_localizedFontNames.CopyTo(out);
                    });

                m_fontCollection->GetFontFamilyMethod.SetExpectedCalls(fontCollectionSize,
                    [=](uint32_t index, IDWriteFontFamily** out)
                    {
                        // Same font family gets returned each time, just because
                        return m_fontFamily.CopyTo(out);
                    });

                m_adapter->GetMockDWriteFactory()->GetSystemFontCollectionMethod.SetExpectedCalls(1,
                    [=](IDWriteFontCollection** out, BOOL)
                    {
                        return m_fontCollection.CopyTo(out);
                    });

                CustomFontManagerAdapter::SetInstance(m_adapter);
            }

            void Verify(wchar_t const* expectedLanguage)
            {
                auto factory = Make<CanvasTextFormatFactory>();

                uint32_t elementCount;
                HSTRING* elements;

                Assert::AreEqual(S_OK, factory->GetSystemFontFamilies(&elementCount, &elements));

                Assert::AreEqual(1u, elementCount);

                WinString elementName(*elements);
                Assert::AreEqual(0, wcscmp(expectedLanguage, static_cast<wchar_t const*>(elementName)));

            }

            void VerifyFromLocaleList(wchar_t const* expectedLanguage, ComPtr<LocaleList> const& localeList)
            {
                auto factory = Make<CanvasTextFormatFactory>();

                uint32_t elementCount;
                HSTRING* elements;

                Assert::AreEqual(S_OK, factory->GetSystemFontFamiliesFromLocaleList(GetVectorView(localeList).Get(), &elementCount, &elements));

                Assert::AreEqual(1u, elementCount);

                WinString elementName(*elements);
                Assert::AreEqual(0, wcscmp(expectedLanguage, static_cast<wchar_t const*>(elementName)));

            }
        };

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamilies_OnlyEnglishAvailable_ReturnsEnglish)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"English", L"en-us"));

            f.Verify(L"English");
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamilies_MultipleAvailable_ReturnsEnglish)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"SomeLanguage", L"xx-xx",
                L"English", L"en-us"));

            f.Verify(L"English");
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamilies_NoEnglish_ReturnsFirstAvailable)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"SomeLanguage1", L"xx-xa",
                L"SomeLanguage2", L"xx-xb"));

            f.Verify(L"SomeLanguage1");
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamiliesFromLocaleList_UseFirstChoice)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"English", L"en-us",
                L"SomeLanguage", L"xx-xx"));

            f.VerifyFromLocaleList(L"SomeLanguage", Make<LocaleList>(
                L"xx-xx",
                L"aa-aa"));
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamiliesFromLocaleList_UseSecondChoice)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"English", L"en-us",
                L"SomeLanguage", L"xx-xx"));

            f.VerifyFromLocaleList(L"SomeLanguage", Make<LocaleList>(
                L"aa-aa",
                L"xx-xx"));
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamiliesFromLocaleList_NothingInListWasAvailable_UseEnglish)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"SomeLanguage", L"xx-xx",
                L"English", L"en-us"));

            f.VerifyFromLocaleList(L"English", Make<LocaleList>(
                L"aa-aa",
                L"bb-bb",
                L"cc-cc"));
        }

        TEST_METHOD_EX(CanvasTextFormat_GetSystemFontFamiliesFromLocaleList_NothingInListWasAvailable_NonEnglish)
        {
            SystemFontFamiliesFixture f(1, Make<LocalizedFontNames>(
                L"SomeLanguage1", L"xx-aa",
                L"SomeLanguage2", L"xx-bb"));

            f.VerifyFromLocaleList(L"SomeLanguage1", Make<LocaleList>(
                L"aa-aa",
                L"bb-bb",
                L"cc-cc"));
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingSign_Property)
        {
            for (auto expected : sc_trimmingSigns)
            {
                auto ctf = Make<CanvasTextFormat>();

                ThrowIfFailed(ctf->put_TrimmingSign(expected));

                CanvasTrimmingSign actual;
                ThrowIfFailed(ctf->get_TrimmingSign(&actual));

                Assert::AreEqual(expected, actual);
            }
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingSign_NullArg)
        {
            auto ctf = Make<CanvasTextFormat>();

            Assert::AreEqual(E_INVALIDARG, ctf->get_TrimmingSign(nullptr));
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingSign_DefaultIsNone)
        {
            auto ctf = Make<CanvasTextFormat>();

            CanvasTrimmingSign sign;
            ThrowIfFailed(ctf->get_TrimmingSign(&sign));

            Assert::AreEqual(CanvasTrimmingSign::None, sign);
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingSign_ChangingSignDoesntCauseFormatReRealization)
        {
            auto ctf = Make<CanvasTextFormat>();

            auto dtf1 = ctf->GetRealizedTextFormat();

            ThrowIfFailed(ctf->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));

            auto dtf2 = ctf->GetRealizedTextFormat();

            Assert::AreEqual(dtf1.Get(), dtf2.Get());
        }

        ComPtr<IDWriteInlineObject> GetTrimmingSign(ComPtr<IDWriteTextFormat> const& textFormat)
        {
            DWRITE_TRIMMING unused;

            ComPtr<IDWriteInlineObject> trimmingSign;
            ThrowIfFailed(textFormat->GetTrimming(&unused, &trimmingSign));

            return trimmingSign;
        }

        template<typename SetterType>
        void CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
            HRESULT (__stdcall CanvasTextFormat::*setFn)(SetterType value),
            SetterType setTo,
            bool setterRequiresRecreatingEllipsisTrimmingSign = true)
        {
            for (auto expected : sc_trimmingSigns)
            {
                auto ctf = Make<CanvasTextFormat>();

                ctf->GetRealizedTextFormat(); // Force realization

                ThrowIfFailed(ctf->put_TrimmingSign(expected));
                auto sign1 = GetTrimmingSign(ctf->GetRealizedTextFormat());

                ((*ctf.Get()).*setFn)(setTo);

                CanvasTrimmingSign actual;
                ThrowIfFailed(ctf->get_TrimmingSign(&actual));
                auto sign2 = GetTrimmingSign(ctf->GetRealizedTextFormat());

                Assert::AreEqual(expected, actual);

                //
                // For ellipsis, the trimming sign object should still exist,
                // but has been changed.
                // It isn't enough to just check whether we created a new
                // text format; there's cases where we need
                // to create a new trimming sign object, even when the
                // underlying text format resource stays the same.
                //
                if (expected == CanvasTrimmingSign::Ellipsis)
                {
                    Assert::IsNotNull(sign1.Get());
                    Assert::IsNotNull(sign2.Get());
                    if (setterRequiresRecreatingEllipsisTrimmingSign)
                    {
                        Assert::AreNotEqual(sign1.Get(), sign2.Get());
                    }
                    else
                    {
                        Assert::AreEqual(sign1.Get(), sign2.Get());
                    }
                }
            }
        }

        TEST_METHOD_EX(CanvasTextFormat_TrimmingSign_AffectsTextFormatState)
        {
            //
            // These tests verify that calling the various setters of CanvasTextFormat
            // will cause the format's trimming sign to be recreated or not recreated,
            // as expected for the situation.
            //

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_Direction,
                CanvasTextDirection::RightToLeftThenBottomToTop);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_FontFamily,
                static_cast<HSTRING>(WinString(L"SomeOtherFamily")));

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_FontSize,
                456.0f);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_FontStretch,
                ABI::Windows::UI::Text::FontStretch_Condensed);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_FontStyle,
                ABI::Windows::UI::Text::FontStyle_Oblique);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_FontWeight,
                ABI::Windows::UI::Text::FontWeight{ 900 });

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_HorizontalAlignment,
                CanvasHorizontalAlignment::Center);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_IncrementalTabStop,
                3.9f);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_LastLineWrapping,
                static_cast<boolean>(false));

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_LineSpacing,
                201.0f);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_LineSpacingBaseline,
                202.0f);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_LineSpacingMode,
                CanvasLineSpacingMode::Proportional);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_LocaleName,
                static_cast<HSTRING>(WinString(L"xx_12")));

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_OpticalAlignment,
                CanvasOpticalAlignment::NoSideBearings);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_Options,
                CanvasDrawTextOptions::EnableColorFont,
                false);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_TrimmingDelimiter,
                static_cast<HSTRING>(WinString(L"K")));
            
            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_TrimmingDelimiterCount,
                2);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_TrimmingGranularity,
                CanvasTextTrimmingGranularity::Character);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_VerticalAlignment,
                CanvasVerticalAlignment::Bottom);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_VerticalGlyphOrientation,
                CanvasVerticalGlyphOrientation::Stacked);

            CanvasTextFormat_TrimmingSign_AffectsTextFormatState_TestCase(
                &CanvasTextFormat::put_WordWrapping,
                CanvasWordWrapping::EmergencyBreak);
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_NullArg)
        {
            auto ctf = Make<CanvasTextFormat>();

            Assert::AreEqual(E_INVALIDARG, ctf->get_CustomTrimmingSign(nullptr));
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_FailsWhenClosed)
        {
            auto ctf = Make<CanvasTextFormat>();
            ThrowIfFailed(ctf->Close());

            ComPtr<ICanvasTextInlineObject> value;
            Assert::AreEqual(RO_E_CLOSED, ctf->get_CustomTrimmingSign(&value));
            Assert::AreEqual(RO_E_CLOSED, ctf->put_CustomTrimmingSign(value.Get()));
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_DefaultIsNull)
        {
            auto ctf = Make<CanvasTextFormat>();

            ComPtr<ICanvasTextInlineObject> value;
            Assert::AreEqual(S_OK, ctf->get_CustomTrimmingSign(&value));

            Assert::IsNull(value.Get());
        }

        TEST_METHOD_EX(CanvasTextFormat_put_CustomTrimmingSign_NullIsOk)
        {
            auto ctf = Make<CanvasTextFormat>();

            Assert::AreEqual(S_OK, ctf->put_CustomTrimmingSign(nullptr));
        }

        struct CustomTrimmingSignFixture
        {
            ComPtr<CanvasTextFormat> TextFormat;
            ComPtr<CustomInlineObject> InlineObject;

            CustomTrimmingSignFixture()
                : TextFormat(Make<CanvasTextFormat>())
                , InlineObject(Make<CustomInlineObject>())
            {
            }

            ComPtr<ICanvasTextInlineObject> get_TrimmingSign()
            {
                ComPtr<ICanvasTextInlineObject> actualTrimmingSign;
                Assert::AreEqual(S_OK, TextFormat->get_CustomTrimmingSign(&actualTrimmingSign));

                return actualTrimmingSign;
            }

            void ReRealize()
            {
                ThrowIfFailed(TextFormat->put_FontFamily(WinString(L"SomeNewFontFamily")));
            }

            void VerifyTrimmingPropertySet()
            {
                auto actualTrimmingSign = get_TrimmingSign();

                Assert::IsTrue(IsSameInstance(InlineObject.Get(), actualTrimmingSign.Get()));
            }
        };

        ComPtr<IDWriteInlineObject> GetTrimmingSignFromDWriteTextFormat(ComPtr<IDWriteTextFormat> const& dtf)
        {
            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> trimmingSign;
            ThrowIfFailed(dtf->GetTrimming(&trimming, &trimmingSign));
            return trimmingSign;
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_Property)
        {
            CustomTrimmingSignFixture f;

            Assert::AreEqual(S_OK, f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));
            
            f.VerifyTrimmingPropertySet();
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_IsAppliedToNativeResource)
        {
            CustomTrimmingSignFixture f;

            ThrowIfFailed(f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));
            auto trimmingSign = GetTrimmingSignFromDWriteTextFormat(GetWrappedResource<IDWriteTextFormat>(f.TextFormat));

            f.InlineObject->DrawMethod.SetExpectedCalls(1);
            auto dwriteTextRenderer = Make<MockDWriteTextRenderer>();
            trimmingSign->Draw(nullptr, dwriteTextRenderer.Get(), 0, 0, FALSE, FALSE, nullptr);
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_PropertyUnaffectedByEllipsis)
        {
            CustomTrimmingSignFixture f;

            ThrowIfFailed(f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));
            ThrowIfFailed(f.TextFormat->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));

            f.VerifyTrimmingPropertySet();
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_DWriteResourceUnaffectedByEllipsis)
        {
            CustomTrimmingSignFixture f;

            ThrowIfFailed(f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));

            auto dtf = GetWrappedResource<IDWriteTextFormat>(f.TextFormat);
            auto trimmingSign1 = GetTrimmingSignFromDWriteTextFormat(dtf);

            ThrowIfFailed(f.TextFormat->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));

            auto trimmingSign2 = GetTrimmingSignFromDWriteTextFormat(dtf);

            Assert::IsTrue(IsSameInstance(trimmingSign1.Get(), trimmingSign2.Get()));
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_StillSameAfterReRealization)
        {
            CustomTrimmingSignFixture f;

            ThrowIfFailed(f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));

            f.ReRealize();

            f.VerifyTrimmingPropertySet();
        }

        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_SetTrimmingSignOnDWriteResource_get_CustomTrimmingSignThrows)
        {
            CustomTrimmingSignFixture f;

            auto dtf = GetWrappedResource<IDWriteTextFormat>(f.TextFormat);

            auto dwriteInlineObject = Make<MockDWriteInlineObject>();

            DWRITE_TRIMMING trimming{};
            ThrowIfFailed(dtf->SetTrimming(&trimming, dwriteInlineObject.Get()));

            ComPtr<ICanvasTextInlineObject> value;
            Assert::AreEqual(E_NOINTERFACE, f.TextFormat->get_CustomTrimmingSign(&value));
        }

        struct CustomTrimmingSignFixtureWithDrawingSession : public CustomTrimmingSignFixture
        {
            ComPtr<CanvasDevice> Device;
            ComPtr<CanvasSolidColorBrush> SolidColorBrush;
            ComPtr<ID2D1SolidColorBrush> D2DSolidColorBrush;

            CustomTrimmingSignFixtureWithDrawingSession()
            {
                Device = CanvasDevice::CreateNew(false);

                SolidColorBrush = CanvasSolidColorBrush::CreateNew(Device.Get(), Color{ 1, 2, 3, 4 });
                D2DSolidColorBrush = GetWrappedResource<ID2D1SolidColorBrush>(SolidColorBrush);
            }

            void DrawTextWhichGetsTrimmed()
            {
                auto textLayout = CanvasTextLayout::CreateNew(Device.Get(), WinString(L"A string that will get trimmed"), TextFormat.Get(), 50.0f, 50.0f);
                textLayout->put_WordWrapping(CanvasWordWrapping::NoWrap);
                textLayout->put_TrimmingGranularity(CanvasTextTrimmingGranularity::Word);

                auto dtl = GetWrappedResource<IDWriteTextLayout>(textLayout);
                ThrowIfFailed(dtl->SetDrawingEffect(D2DSolidColorBrush.Get(), DWRITE_TEXT_RANGE{ 0, 100 }));

                auto renderTarget = CanvasRenderTarget::CreateNew(
                    Device.Get(),
                    1.0f,
                    1.0f,
                    DEFAULT_DPI,
                    PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                    CanvasAlphaMode::Premultiplied);

                ComPtr<ICanvasDrawingSession> drawingSession;
                ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

                ThrowIfFailed(drawingSession->DrawTextLayoutAtCoordsWithColor(textLayout.Get(), 0, 0, Color{}));

                ThrowIfFailed(static_cast<CanvasDrawingSession*>(drawingSession.Get())->Close());
            }
        };


        TEST_METHOD_EX(CanvasTextFormat_CustomTrimmingSign_SetBeforeLayoutExists_NativeInteropSetsBrush_CallbackSeesWrappedBrush)
        {
            CustomTrimmingSignFixtureWithDrawingSession f;

            f.InlineObject->DrawMethod.SetExpectedCalls(1,
                [&](
                ICanvasTextRenderer*,
                Vector2,
                boolean,
                boolean,
                IInspectable* brush)
                {
                    Assert::IsTrue(IsSameInstance(f.SolidColorBrush.Get(), brush));

                    return S_OK;
                });

            ThrowIfFailed(f.TextFormat->put_CustomTrimmingSign(f.InlineObject.Get()));

            f.DrawTextWhichGetsTrimmed();
        }

        TEST_METHOD(CanvasTextFormat_TrimmingDelimiterValidation)
        {
            auto textFormat = Make<CanvasTextFormat>();

            // 
            // Forcing a realization is necessary if we want to exercise the conversion.
            //
            auto dtf = GetWrappedResource<IDWriteTextFormat>(textFormat);

            TrimmingDelimiterValidationTest(textFormat);
        }
    };
}
