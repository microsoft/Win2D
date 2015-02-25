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

#include "MockDWriteFactory.h"
#include "MockDWriteFontCollection.h"
#include "MockDWriteFontFile.h"
#include "StubStorageFileStatics.h"

namespace canvas
{
    class StubCanvasTextFormatAdapter : public CanvasTextFormatAdapter
    {
    public:
        ComPtr<StubStorageFileStatics> StorageFileStatics;
        
        StubCanvasTextFormatAdapter()
            : StorageFileStatics(Make<StubStorageFileStatics>())
        {
        }

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override
        {
            ComPtr<IDWriteFactory> factory;
            ThrowIfFailed(DWriteCreateFactory(type, __uuidof(factory), &factory));
            return factory;
        }

        virtual IStorageFileStatics* GetStorageFileStatics() override
        {
            return StorageFileStatics.Get();
        }
    };

    std::shared_ptr<CanvasTextFormatManager> CreateTestManager()
    {
        auto adapter = std::make_shared<StubCanvasTextFormatAdapter>();
        return std::make_shared<CanvasTextFormatManager>(adapter);
    }

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
        auto manager = CreateTestManager();
        auto ctf = manager->Create();

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
        ctf = manager->Create();
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
        auto ctf = CreateTestManager()->Create();

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

    TEST_CLASS(CanvasTextFormatTests)
    {
    public:
        TEST_METHOD_EX(CanvasTextFormat_Implements_Expected_Interfaces)
        {
            auto ctf = CreateTestManager()->Create();

            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasTextFormat);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(ctf, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasTextFormat_FlowDirection)
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

        TEST_METHOD_EX(CanvasTextFormat_LineSpacingMethod)
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
                -1.234f,
                nanf(""));
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

        TEST_METHOD_EX(CanvasTextFormat_ReadingDirection)
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

        TEST_METHOD_EX(CanvasTextFormat_ParagraphAlignment)
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

            auto ctf = CreateTestManager()->Create();

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

#undef TEST_SIMPLE_PROPERTY
#undef SIMPLE_DWRITE_SETTER
#undef SIMPLE_DWRITE_GETTER
#undef SIMPLE_CANVAS_SETTER
#undef SIMPLE_CANVAS_GETTER

        class StubCanvasTextFormatAdapterWithDWriteFactory : public StubCanvasTextFormatAdapter
        {
        public:
            ComPtr<MockDWriteFactory> DWriteFactory;

            StubCanvasTextFormatAdapterWithDWriteFactory()
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
            std::shared_ptr<StubCanvasTextFormatAdapterWithDWriteFactory> Adapter;
            std::shared_ptr<CanvasTextFormatManager> Manager;

            WinString AnyFullFontFamilyName;
            std::wstring AnyPath;
            WinString AnyFontFamily;

            WinString AnyOtherFullFontFamilyName;
            std::wstring AnyOtherPath;

            CustomFontFixture()
                : Adapter(std::make_shared<StubCanvasTextFormatAdapterWithDWriteFactory>())
                , Manager(std::make_shared<CanvasTextFormatManager>(Adapter))
                , AnyFullFontFamilyName(L"any_uri#any_font_family")
                , AnyPath(StubStorageFileStatics::GetFakePath(WinString(L"ms-appx:///any_uri")))
                , AnyFontFamily(L"any_font_family")
                , AnyOtherFullFontFamilyName(L"any_other_uri#any_other_font_family")
                , AnyOtherPath(StubStorageFileStatics::GetFakePath(WinString(L"ms-appx:///any_other_uri")))
            {
                Adapter->DWriteFactory->RegisterFontCollectionLoaderMethod.AllowAnyCall();
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
        

        TEST_METHOD_EX(CanvasTextFormat_WhenFontFamilyNameContainsAUri_CustomFontCollectionIsUsed_And_UriStrippedFromRealizedName)
        {
            CustomFontFixture f;

            auto fontCollectionReturnedFromAdapter = f.ExpectCreateCustomFontCollection(f.AnyPath);

            auto cf = f.Manager->Create();
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

            auto cf1 = f.Manager->Create();
            ThrowIfFailed(cf1->put_FontFamily(f.AnyFullFontFamilyName));
            ThrowIfFailed(cf1->put_FontSize(1));

            auto df1 = cf1->GetRealizedTextFormat();

            f.DontExpectCreateCustomFontCollection();

            auto cf2 = f.Manager->Create(df1.Get());
            ThrowIfFailed(cf2->put_FontSize(2));
            
            auto df2 = cf2->GetRealizedTextFormat();

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

            auto cf1 = f.Manager->Create();
            ThrowIfFailed(cf1->put_FontFamily(f.AnyFullFontFamilyName));

            f.ExpectCreateCustomFontCollection(f.AnyPath);
            auto df1 = cf1->GetRealizedTextFormat();

            auto cf2 = f.Manager->Create(df1.Get());
            ThrowIfFailed(cf2->put_FontFamily(f.AnyOtherFullFontFamilyName));
                
            f.ExpectCreateCustomFontCollection(f.AnyOtherPath);
            auto df2 = cf2->GetRealizedTextFormat();

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

            auto cf = f.Manager->Create();
            ThrowIfFailed(cf->put_FontFamily(f.AnyFullFontFamilyName));
            
            WinString actualFontFamily;
            ThrowIfFailed(cf->get_FontFamily(actualFontFamily.GetAddressOf()));
            Assert::AreEqual(static_cast<wchar_t const*>(f.AnyFullFontFamilyName), static_cast<wchar_t const*>(actualFontFamily));

            f.ExpectCreateCustomFontCollection(f.AnyPath);
            cf->GetRealizedTextFormat();

            ThrowIfFailed(cf->get_FontFamily(actualFontFamily.GetAddressOf()));
            Assert::AreEqual(static_cast<wchar_t const*>(f.AnyFullFontFamilyName), static_cast<wchar_t const*>(actualFontFamily));
        }
    };
}