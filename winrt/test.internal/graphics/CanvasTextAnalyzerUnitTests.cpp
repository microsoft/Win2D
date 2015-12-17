// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "mocks/MockDWriteFont.h"
#include "mocks/MockDWriteFontCollection.h"
#include "mocks/MockDWriteFontFace.h"
#include "mocks/MockDWriteFontFaceReference.h"
#include "mocks/MockDWriteFontSet.h"
#include "mocks/MockDWriteTextAnalyzer.h"
#include <lib/text/CanvasScaledFont.h>
#include <lib/text/CanvasNumberSubstitution.h>
#include <lib/text/CanvasTextAnalyzer.h>
#include <lib/text/CanvasFontFace.h>

#if WINVER > _WIN32_WINNT_WINBLUE
typedef MockDWriteFontFaceReference MockDWriteFontFaceContainer;
typedef IDWriteFontFace3 RealizedFontFaceType;
#else
typedef MockDWriteFont MockDWriteFontFaceContainer;
typedef IDWriteFontFace RealizedFontFaceType;
#endif

struct TestCaseForCharacterRange
{
    uint32_t Position;
    uint32_t ExpectedCharacterCount;
} testCasesForCharacterRanges[] = {
    { 0, 3 },
    { 1, 2 },
    { 2, 1 },
    { 3, 0 },
    { 999, 0 }
};

TEST_CLASS(CanvasScaledFontTests)
{
    ComPtr<CanvasFontFace> CreateSimpleFontFace()
    {
        auto resource = Make<MockDWriteFontFaceContainer>();
        return Make<CanvasFontFace>(resource.Get());
    }

    TEST_METHOD_EX(CanvasScaledFont_ImplementsExpectedInterfaces)
    {
        auto canvasCharacterRangeFont = Make<CanvasScaledFont>(nullptr, 0.0f);

        ASSERT_IMPLEMENTS_INTERFACE(canvasCharacterRangeFont, ICanvasScaledFont);
    }

    TEST_METHOD_EX(CanvasScaledFont_NullArgs)
    {
        auto canvasCharacterRangeFont = Make<CanvasScaledFont>(nullptr, 0.0f);

        Assert::AreEqual(E_INVALIDARG, canvasCharacterRangeFont->get_FontFace(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasCharacterRangeFont->get_ScaleFactor(nullptr));
    }

    TEST_METHOD_EX(CanvasScaledFont_Properties)
    {
        auto expectedFontFace = CreateSimpleFontFace();
        auto canvasCharacterRangeFont = Make<CanvasScaledFont>(expectedFontFace.Get(), 7.8f);

        ComPtr<ICanvasFontFace> fontFace;
        Assert::AreEqual(S_OK, canvasCharacterRangeFont->get_FontFace(&fontFace));
        Assert::IsTrue(IsSameInstance(expectedFontFace.Get(), fontFace.Get()));

        float scale{};
        Assert::AreEqual(S_OK, canvasCharacterRangeFont->get_ScaleFactor(&scale));
        Assert::AreEqual(7.8f, scale);
    }
};

class MockDWriteNumberSubstitution : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IDWriteNumberSubstitution>
{
};

TEST_CLASS(CanvasNumberSubstitutionTests)
{
    class Fixture
    {
        std::shared_ptr<StubCanvasTextLayoutAdapter> m_adapter;
        ComPtr<CanvasNumberSubstitutionFactory> m_factory;
        ComPtr<MockDWriteNumberSubstitution> m_mockDWriteNumberSubstitution;
    public:

        Fixture()
        {
            m_adapter = std::make_shared<StubCanvasTextLayoutAdapter>();
            CustomFontManagerAdapter::SetInstance(m_adapter);

            m_factory = Make<CanvasNumberSubstitutionFactory>();

            m_mockDWriteNumberSubstitution = Make<MockDWriteNumberSubstitution>();
        }

        void ExpectCreateNumberSubstitution(
            DWRITE_NUMBER_SUBSTITUTION_METHOD expectedMethod,
            wchar_t const* expectedLocale,
            bool expectedOverrides)
        {
            std::wstring expectedLocaleString = expectedLocale;

            m_adapter->GetMockDWriteFactory()->CreateNumberSubstitutionMethod.SetExpectedCalls(1,
                [=](DWRITE_NUMBER_SUBSTITUTION_METHOD method, WCHAR const* locale, BOOL overrides, IDWriteNumberSubstitution** out)
                {
                    Assert::AreEqual(expectedMethod, method);
                    Assert::AreEqual(expectedLocaleString.c_str(), locale);
                    Assert::AreEqual(expectedOverrides, !!overrides);

                    return m_mockDWriteNumberSubstitution.CopyTo(out);
                });
        }

        ComPtr<CanvasNumberSubstitutionFactory> const& GetFactory() 
        {
            return m_factory;
        }

        void ValidateCorrectObjectWrapped(ComPtr<ICanvasNumberSubstitution> const& canvasNumberSubstitution)
        {
            auto wrappedObject = GetWrappedResource<IDWriteNumberSubstitution>(canvasNumberSubstitution);
            Assert::IsTrue(IsSameInstance(m_mockDWriteNumberSubstitution.Get(), wrappedObject.Get()));
        }
    };

    TEST_METHOD_EX(CanvasNumberSubstitution_Create)
    {
        Fixture f;

        f.ExpectCreateNumberSubstitution(DWRITE_NUMBER_SUBSTITUTION_METHOD_NATIONAL, L"", false);

        ComPtr<ICanvasNumberSubstitution> numberSubstitution;
        f.GetFactory()->Create(CanvasNumberSubstitutionMethod::National, &numberSubstitution);

        f.ValidateCorrectObjectWrapped(numberSubstitution);
    }

    TEST_METHOD_EX(CanvasNumberSubstitution_CreateWithLocaleAndIgnoreOverrides)
    {
        Fixture f;

        f.ExpectCreateNumberSubstitution(DWRITE_NUMBER_SUBSTITUTION_METHOD_TRADITIONAL, L"xx-yy", true);

        ComPtr<ICanvasNumberSubstitution> numberSubstitution;
        f.GetFactory()->CreateWithLocaleAndIgnoreOverrides(CanvasNumberSubstitutionMethod::Traditional, WinString(L"xx-yy"), true, &numberSubstitution);

        f.ValidateCorrectObjectWrapped(numberSubstitution);
    }
};

TEST_CLASS(CanvasTextAnalyzerTests)
{
    class MockDWriteFontFallback : public RuntimeClass <
        RuntimeClassFlags<ClassicCom>,
        IDWriteFontFallback >
    {
    public:
        MOCK_METHOD11(MapCharacters, HRESULT(IDWriteTextAnalysisSource*, UINT32, UINT32, IDWriteFontCollection*, wchar_t const*, DWRITE_FONT_WEIGHT, DWRITE_FONT_STYLE, DWRITE_FONT_STRETCH, UINT32*, IDWriteFont**, FLOAT*));
    };

    class TextAnalyzerOptions : public RuntimeClass <
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasTextAnalyzerOptions >
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetLocaleNameMethod, HRESULT(int, int*, HSTRING*));
        CALL_COUNTER_WITH_MOCK(GetNumberSubstitutionMethod, HRESULT(int, int*, ICanvasNumberSubstitution**));
        CALL_COUNTER_WITH_MOCK(GetVerticalGlyphOrientationMethod, HRESULT(int, int*, CanvasVerticalGlyphOrientation*));
        CALL_COUNTER_WITH_MOCK(GetBidiLevelMethod, HRESULT(int, int*, uint32_t*));

        IFACEMETHODIMP GetLocaleName(int characterIndex, int* characterCount, HSTRING* out) override
        {
            return GetLocaleNameMethod.WasCalled(characterIndex, characterCount, out);
        }

        IFACEMETHODIMP GetNumberSubstitution(int characterIndex, int* characterCount, ICanvasNumberSubstitution** out) override
        {
            return GetNumberSubstitutionMethod.WasCalled(characterIndex, characterCount, out);
        }

        IFACEMETHODIMP GetVerticalGlyphOrientation(int characterIndex, int* characterCount, CanvasVerticalGlyphOrientation* out) override
        {
            return GetVerticalGlyphOrientationMethod.WasCalled(characterIndex, characterCount, out);
        }

        IFACEMETHODIMP GetBidiLevel(int characterIndex, int* characterCount, uint32_t* out) override
        {
            return GetBidiLevelMethod.WasCalled(characterIndex, characterCount, out);
        }
    };

    struct Fixture
    {
        std::shared_ptr<StubCanvasTextLayoutAdapter> m_adapter;
        ComPtr<MockDWriteFontCollection> m_mockSystemFontCollection;
        ComPtr<MockDWriteFontFallback> m_mockSystemFontFallback;
        ComPtr<CanvasTextAnalyzerFactory> m_textAnalyzerFactory;

    public:

        UINT32 ExpectedTextPosition;
        UINT32 ExpectedTextLength;
        ComPtr<IDWriteFontCollection> ExpectedFontCollection;
        std::wstring ExpectedFamilyName;
        DWRITE_FONT_WEIGHT ExpectedWeight;
        DWRITE_FONT_STYLE ExpectedStyle;
        DWRITE_FONT_STRETCH ExpectedStretch;

        std::function<void(IDWriteTextAnalysisSource1*)> MockDoingThingsToTextAnalysisSource;

        UINT32 ReturnedMappedLength;
        ComPtr<MockDWriteFont> ReturnedFont;
        float ReturnedScale;

        ComPtr<MockDWriteTextAnalyzer> TextAnalyzer;

        //
        // Passed to any new analyzers created through this fixture.
        //
        std::wstring Text;
        CanvasTextDirection TextDirection;
        ComPtr<TextAnalyzerOptions> Options;
        ComPtr<ICanvasNumberSubstitution> NumberSubstitution;
        CanvasVerticalGlyphOrientation VerticalGlyphOrientation;
        uint32_t BidiLevel;

        //
        // Passed to ChooseFonts.
        //
        ComPtr<ICanvasTextFormat> TextFormat;

        //
        // Passed to GetGlyphs.
        //
        float FontSize;
        bool IsSideways;
        bool IsRightToLeft;
        std::wstring Locale;
        ComPtr<MockDWriteFontFace> RealizedFontFace;
        ComPtr<MockDWriteFontFaceContainer> DWriteFontResource;
        ComPtr<ICanvasFontFace> FontFace;
        CanvasAnalyzedScript AnalyzedScript;
        ComPtr<MockDWriteNumberSubstitution> DWriteNumberSubstitution;
        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>*>> TypographyRanges;
        int AdditionalGlyphsToAddDuringGetGlyphs;
        CanvasCharacterRange CharacterRange;

        Fixture()
        {
            m_adapter = std::make_shared<StubCanvasTextLayoutAdapter>();
            CustomFontManagerAdapter::SetInstance(m_adapter);

            m_mockSystemFontCollection = Make<MockDWriteFontCollection>();

            m_mockSystemFontFallback = Make<MockDWriteFontFallback>();

            TextAnalyzer = Make<MockDWriteTextAnalyzer>();

            m_adapter->GetMockDWriteFactory()->GetSystemFontCollectionMethod.AllowAnyCall(
                [=](IDWriteFontCollection** out, BOOL)
                {
                    m_mockSystemFontCollection.CopyTo(out);
                    return S_OK;
                });

            m_adapter->GetMockDWriteFactory()->GetSystemFontFallbackMethod.AllowAnyCall(
                [=](IDWriteFontFallback** out)
                {
                    m_mockSystemFontFallback.CopyTo(out);
                    return S_OK;
                });

            m_adapter->GetMockDWriteFactory()->CreateTextAnalyzerMethod.AllowAnyCall(
                [=](IDWriteTextAnalyzer** out)
                {
                    TextAnalyzer.CopyTo(out);
                    return S_OK;
                });

            ExpectedTextPosition = 0;
            ExpectedTextLength = 3;
            ExpectedFontCollection = m_mockSystemFontCollection;
            ExpectedFamilyName = L"Segoe UI";
            ExpectedWeight = DWRITE_FONT_WEIGHT_NORMAL;
            ExpectedStyle = DWRITE_FONT_STYLE_NORMAL;
            ExpectedStretch = DWRITE_FONT_STRETCH_NORMAL;

            MockDoingThingsToTextAnalysisSource = [](IDWriteTextAnalysisSource1*){};

            ReturnedMappedLength = 3;
            ReturnedFont = Make<MockDWriteFont>();
            ReturnedScale = 1.0f;

            m_textAnalyzerFactory = Make<CanvasTextAnalyzerFactory>();

            Text = L"abc";
            TextDirection = CanvasTextDirection::LeftToRightThenTopToBottom;
            VerticalGlyphOrientation = CanvasVerticalGlyphOrientation::Default;
            BidiLevel = 0;
            Options = Make<TextAnalyzerOptions>();

            TextFormat = Make<CanvasTextFormat>();

            Options->GetBidiLevelMethod.AllowAnyCall(
                [&](int characterPosition, int* characterCount, uint32_t* out)
                {
                    *out = 0u;
                    *characterCount = static_cast<uint32_t>(this->Text.length()) - characterPosition;
                    return S_OK;
                });

            Options->GetVerticalGlyphOrientationMethod.AllowAnyCall(
                [&](int characterPosition, int* characterCount, CanvasVerticalGlyphOrientation* out)
                {
                    *out = CanvasVerticalGlyphOrientation::Default;
                    *characterCount = static_cast<uint32_t>(this->Text.length()) - characterPosition;
                    return S_OK;
                });

#if WINVER > _WIN32_WINNT_WINBLUE
            ReturnedFont->GetFontFaceReferenceMethod.AllowAnyCall(
                [&](IDWriteFontFaceReference** out)
                {
                    auto mockFontFaceReference = Make<MockDWriteFontFaceReference>();
                    ThrowIfFailed(mockFontFaceReference.CopyTo(out));
                    return S_OK;
                });
#endif
            TextAnalyzer->AnalyzeScriptMethod.AllowAnyCall();

            RealizedFontFace = Make<MockDWriteFontFace>();

            DWriteFontResource = Make<MockDWriteFontFaceContainer>();
            DWriteFontResource->CreateFontFaceMethod.AllowAnyCall(
                [this](RealizedFontFaceType** out)
                {
                    ThrowIfFailed(RealizedFontFace.CopyTo(out));
                    return S_OK;
                });
            FontFace = Make<CanvasFontFace>(DWriteFontResource.Get());

            AnalyzedScript.ScriptIdentifier = 123;
            AnalyzedScript.Shape = CanvasScriptShape::NoVisual;

            FontSize = 123.4f;
            IsSideways = true;
            IsRightToLeft = true;
            Locale = L"";
            AdditionalGlyphsToAddDuringGetGlyphs = 2;
            CharacterRange.CharacterIndex = 0;
            CharacterRange.CharacterCount = static_cast<int>(this->Text.length());
        }

        std::shared_ptr<StubCanvasTextLayoutAdapter>& GetAdapter() { return m_adapter; }

        void ExpectGetSystemFontCollection()
        {
            m_adapter->GetMockDWriteFactory()->GetSystemFontCollectionMethod.SetExpectedCalls(1,
                [=](IDWriteFontCollection** out, BOOL)
            {
                m_mockSystemFontCollection.CopyTo(out);
                return S_OK;
            });
        }

        void UseNumberSubstitution()
        {
            DWriteNumberSubstitution = Make<MockDWriteNumberSubstitution>();
            NumberSubstitution = ResourceManager::GetOrCreate<ICanvasNumberSubstitution>(DWriteNumberSubstitution.Get());
        }

        void UseTypographyRanges()
        {
            typedef KeyValuePair<CanvasCharacterRange, CanvasTypography*, DefaultMapTraits<CanvasCharacterRange, ICanvasTypography*>> TypographyRangeValue;

            auto typographyRangesVector = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>*>>();

            ComPtr<IDWriteTypography> dwriteTypographies[2];

            ComPtr<IDWriteFactory> realDWriteFactory;
            ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &realDWriteFactory));

            ThrowIfFailed(realDWriteFactory->CreateTypography(&dwriteTypographies[0]));
            ThrowIfFailed(dwriteTypographies[0]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_1, 100u }));
            ThrowIfFailed(dwriteTypographies[0]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2, 200u }));
            ThrowIfFailed(dwriteTypographies[0]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_3, 300u }));
            auto canvasTypography0 = ResourceManager::GetOrCreate<ICanvasTypography>(dwriteTypographies[0].Get());
            auto range0 = Make<TypographyRangeValue>(CanvasCharacterRange{ 0, 1 }, canvasTypography0.Get());
            ThrowIfFailed(typographyRangesVector->Append(range0.Get()));

            ThrowIfFailed(realDWriteFactory->CreateTypography(&dwriteTypographies[1]));
            ThrowIfFailed(dwriteTypographies[1]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_4, 400u }));
            ThrowIfFailed(dwriteTypographies[1]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_5, 500u }));
            ThrowIfFailed(dwriteTypographies[1]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_6, 600u }));
            ThrowIfFailed(dwriteTypographies[1]->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 700u }));
            auto canvasTypography1 = ResourceManager::GetOrCreate<ICanvasTypography>(dwriteTypographies[1].Get());
            auto range1 = Make<TypographyRangeValue>(CanvasCharacterRange{ 1, 2 }, canvasTypography1.Get());
            ThrowIfFailed(typographyRangesVector->Append(range1.Get()));

            ThrowIfFailed(typographyRangesVector->GetView(&TypographyRanges));
        }

        void ExpectMapCharacters()
        {
            m_mockSystemFontFallback->MapCharactersMethod.SetExpectedCalls(1,
                [=](IDWriteTextAnalysisSource* analysisSource,
                UINT32 textPosition,
                UINT32 textLength,
                IDWriteFontCollection* baseFontCollection,
                wchar_t const* baseFamilyName,
                DWRITE_FONT_WEIGHT baseWeight,
                DWRITE_FONT_STYLE baseStyle,
                DWRITE_FONT_STRETCH baseStretch,
                UINT32* mappedLength,
                IDWriteFont** mappedFont,
                FLOAT* scale)
            {
                Assert::AreEqual(ExpectedTextPosition, textPosition);
                Assert::AreEqual(ExpectedTextLength, textLength);
                Assert::IsTrue(IsSameInstance(ExpectedFontCollection.Get(), baseFontCollection));
                Assert::AreEqual(0, wcscmp(ExpectedFamilyName.c_str(), baseFamilyName));
                Assert::AreEqual(ExpectedTextLength, textLength);
                Assert::AreEqual(ExpectedWeight, baseWeight);
                Assert::AreEqual(ExpectedStyle, baseStyle);
                Assert::AreEqual(ExpectedStretch, baseStretch);

                auto source1 = As<IDWriteTextAnalysisSource1>(analysisSource);
                MockDoingThingsToTextAnalysisSource(source1.Get());

                *mappedLength = ReturnedMappedLength;
                ReturnedFont.CopyTo(mappedFont);
                *scale = ReturnedScale;

                return S_OK;
            });
        }

        void VerifyNumberSubstitution(
            IDWriteNumberSubstitution* numberSubstitution)
        {
            if (DWriteNumberSubstitution)
                Assert::IsTrue(IsSameInstance(DWriteNumberSubstitution.Get(), numberSubstitution));
            else
                Assert::IsNull(numberSubstitution);
        }

        void VerifyFeatureRanges(
            DWRITE_TYPOGRAPHIC_FEATURES const** features,
            uint32_t const* featureRangeLengths,
            uint32_t featureRanges)
        {
            if (TypographyRanges)
            {
                Assert::AreEqual(1u, featureRangeLengths[0]);
                Assert::AreEqual(3u, features[0]->featureCount);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_1, features[0]->features[0].nameTag);
                Assert::AreEqual(100u, features[0]->features[0].parameter);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2, features[0]->features[1].nameTag);
                Assert::AreEqual(200u, features[0]->features[1].parameter);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_3, features[0]->features[2].nameTag);
                Assert::AreEqual(300u, features[0]->features[2].parameter);

                Assert::AreEqual(2u, featureRangeLengths[1]);
                Assert::AreEqual(4u, features[1]->featureCount);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_4, features[1]->features[0].nameTag);
                Assert::AreEqual(400u, features[1]->features[0].parameter);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_5, features[1]->features[1].nameTag);
                Assert::AreEqual(500u, features[1]->features[1].parameter);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_6, features[1]->features[2].nameTag);
                Assert::AreEqual(600u, features[1]->features[2].parameter);
                Assert::AreEqual(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, features[1]->features[3].nameTag);
                Assert::AreEqual(700u, features[1]->features[3].parameter);
            }
            else
            {
                Assert::IsNull(features);
                Assert::IsNull(featureRangeLengths);
                Assert::AreEqual(0u, featureRanges);
            }
        }

        void ExpectGetGlyphs(int getGlyphCount=1, int getGlyphPlacementsCount=1)
        {
            TextAnalyzer->GetGlyphsMethod.SetExpectedCalls(getGlyphCount,
                [&](WCHAR const* textString,
                    uint32_t textLength,
                    IDWriteFontFace* fontFace,
                    BOOL isSideways,
                    BOOL isRightToLeft,
                    DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis,
                    WCHAR const* localeName,
                    IDWriteNumberSubstitution* numberSubstitution,
                    DWRITE_TYPOGRAPHIC_FEATURES const** features,
                    uint32_t const* featureRangeLengths,
                    uint32_t featureRanges,
                    uint32_t maxGlyphCount,
                    UINT16* clusterMap,
                    DWRITE_SHAPING_TEXT_PROPERTIES* textProps,
                    UINT16* glyphIndices,
                    DWRITE_SHAPING_GLYPH_PROPERTIES* glyphProps,
                    uint32_t* actualGlyphCount)
                {
                    Assert::AreEqual(0, wcscmp(this->Text.c_str(), textString));
                    Assert::AreEqual(static_cast<uint32_t>(this->Text.length()), textLength);
                    Assert::IsTrue(IsSameInstance(RealizedFontFace.Get(), fontFace));

                    Assert::AreEqual(IsSideways, !!isSideways);
                    Assert::AreEqual(IsRightToLeft, !!isRightToLeft);

                    Assert::AreEqual(static_cast<uint16_t>(123), scriptAnalysis->script);
                    Assert::AreEqual(DWRITE_SCRIPT_SHAPES_NO_VISUAL, scriptAnalysis->shapes);

                    Assert::AreEqual(0, wcscmp(Locale.c_str(), localeName));

                    VerifyNumberSubstitution(numberSubstitution);

                    VerifyFeatureRanges(features, featureRangeLengths, featureRanges);

                    //
                    // For these contrived tests, the glyph breakdown will add some extra glyphs.
                    //
                    const int extraGlyphs = AdditionalGlyphsToAddDuringGetGlyphs;
                    const int calculatedGlyphCount = static_cast<int>(this->Text.length()) + extraGlyphs;

                    if (calculatedGlyphCount > static_cast<int>(maxGlyphCount))
                        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

                    for (int i = 0; i < static_cast<int>(this->Text.length()); ++i)
                    {
                        clusterMap[i] = static_cast<uint16_t>(i * 2 + 1);
                        textProps[i].isShapedAlone = i % 2 == 0;
                    }

                    *actualGlyphCount = calculatedGlyphCount;

                    for (int i = 0; i < calculatedGlyphCount; ++i)
                    {
                        glyphIndices[i] = static_cast<uint16_t>(i * 3 + 2);
                        glyphProps[i].justification = 15;
                        glyphProps[i].isClusterStart = i % 2 == 0;
                        glyphProps[i].isDiacritic = i % 2 != 0;
                        glyphProps[i].isZeroWidthSpace = i % 2 == 0;
                    }

                    return S_OK;
                });
            
            TextAnalyzer->GetGlyphPlacementsMethod.SetExpectedCalls(getGlyphPlacementsCount,
                [&](WCHAR const* textString,
                    UINT16 const* clusterMap,
                    DWRITE_SHAPING_TEXT_PROPERTIES* textProps,
                    UINT32 textLength,
                    UINT16 const* glyphIndices,
                    DWRITE_SHAPING_GLYPH_PROPERTIES const* glyphProps,
                    UINT32 glyphCount,
                    IDWriteFontFace * fontFace,
                    FLOAT fontEmSize,
                    BOOL isSideways,
                    BOOL isRightToLeft,
                    DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis,
                    WCHAR const* localeName,
                    DWRITE_TYPOGRAPHIC_FEATURES const** features,
                    UINT32 const* featureRangeLengths,
                    UINT32 featureRanges,
                    FLOAT* glyphAdvances,
                    DWRITE_GLYPH_OFFSET* glyphOffsets)
                {
                    Assert::AreEqual(0, wcscmp(this->Text.c_str(), textString));

                    for (int i = 0; i < static_cast<int>(this->Text.length()); ++i)
                    {
                        Assert::AreEqual(static_cast<uint16_t>(i * 2 + 1), clusterMap[i]);
                        Assert::AreEqual(i % 2 == 0, static_cast<bool>(textProps[i].isShapedAlone));
                    }

                    Assert::AreEqual(static_cast<uint32_t>(this->Text.length()), textLength);

                    for (uint32_t i = 0; i < glyphCount; i++)
                    {
                        Assert::AreEqual(static_cast<uint16_t>(i * 3 + 2), glyphIndices[i]);
                        Assert::AreEqual(static_cast<uint16_t>(15), glyphProps[i].justification);
                        Assert::AreEqual(i % 2 == 0, static_cast<bool>(glyphProps[i].isClusterStart));
                        Assert::AreEqual(i % 2 != 0, static_cast<bool>(glyphProps[i].isDiacritic));
                        Assert::AreEqual(i % 2 == 0, static_cast<bool>(glyphProps[i].isZeroWidthSpace));
                    }

                    const int extraGlyphs = AdditionalGlyphsToAddDuringGetGlyphs;
                    Assert::AreEqual(static_cast<int>(this->Text.length()) + extraGlyphs, static_cast<int>(glyphCount));

                    Assert::IsTrue(IsSameInstance(RealizedFontFace.Get(), fontFace));

                    Assert::AreEqual(FontSize, fontEmSize);
                    Assert::AreEqual(IsSideways, !!isSideways);
                    Assert::AreEqual(IsRightToLeft, !!isRightToLeft);

                    Assert::AreEqual(static_cast<uint16_t>(123), scriptAnalysis->script);
                    Assert::AreEqual(DWRITE_SCRIPT_SHAPES_NO_VISUAL, scriptAnalysis->shapes);

                    Assert::AreEqual(0, wcscmp(Locale.c_str(), localeName));

                    VerifyFeatureRanges(features, featureRangeLengths, featureRanges);

                    for (uint32_t i = 0; i < glyphCount; ++i)
                    {
                        glyphAdvances[i] = static_cast<float>(i) * 11.0f + 1.0f;
                        glyphOffsets[i].advanceOffset = static_cast<float>(i)* 12.0f + 2.0f;
                        glyphOffsets[i].ascenderOffset = static_cast<float>(i)* 13.0f + 3.0f;
                    }

                    return S_OK;
                });
        }

        void GetGlyphs(ComPtr<ICanvasTextAnalyzer> const& textAnalyzer)
        {
            uint32_t glyphCount;
            CanvasGlyph* glyphElements;

            Assert::AreEqual(S_OK, textAnalyzer->GetGlyphs(
                CharacterRange,
                FontFace.Get(),
                FontSize,
                IsSideways,
                IsRightToLeft,
                AnalyzedScript,
                &glyphCount,
                &glyphElements));

            const int extraGlyphs = AdditionalGlyphsToAddDuringGetGlyphs;
            Assert::AreEqual(static_cast<int>(this->Text.length()) + extraGlyphs, static_cast<int>(glyphCount));

            for (int i = 0; i < static_cast<int>(Text.length()) + extraGlyphs; ++i)
            {
                Assert::AreEqual(i * 3 + 2, glyphElements[i].Index);
                Assert::AreEqual(static_cast<float>(i)* 11.0f + 1.0f, glyphElements[i].Advance);
                Assert::AreEqual(static_cast<float>(i)* 12.0f + 2.0f, glyphElements[i].AdvanceOffset);
                Assert::AreEqual(static_cast<float>(i)* 13.0f + 3.0f, glyphElements[i].AscenderOffset);
            }
        }

        void GetGlyphsWithAllOptions(ComPtr<ICanvasTextAnalyzer> const& textAnalyzer)
        {
            uint32_t clusterMapIndexCount;
            int* clusterMapIndexElements;

            uint32_t isShapedAloneCount;
            boolean* isShapedAloneElements;

            uint32_t glyphShapingCount;
            CanvasGlyphShaping* glyphShapingElements;

            uint32_t glyphCount;
            CanvasGlyph* glyphElements;
            
            Assert::AreEqual(S_OK, textAnalyzer->GetGlyphsWithAllOptions(
                CharacterRange,
                FontFace.Get(),
                FontSize,
                IsSideways,
                IsRightToLeft,
                AnalyzedScript,
                WinString(Locale.c_str()),
                NumberSubstitution.Get(),
                TypographyRanges.Get(),
                &clusterMapIndexCount,
                &clusterMapIndexElements,
                &isShapedAloneCount,
                &isShapedAloneElements,
                &glyphShapingCount,
                &glyphShapingElements,
                &glyphCount,
                &glyphElements));
            
            Assert::AreEqual(static_cast<uint32_t>(this->Text.length()), clusterMapIndexCount);
            Assert::AreEqual(static_cast<uint32_t>(this->Text.length()), isShapedAloneCount);

            const int extraGlyphs = AdditionalGlyphsToAddDuringGetGlyphs;
            Assert::AreEqual(static_cast<int>(this->Text.length()) + extraGlyphs, static_cast<int>(glyphShapingCount));
            Assert::AreEqual(static_cast<int>(this->Text.length()) + extraGlyphs, static_cast<int>(glyphCount));

            for (int i = 0; i < static_cast<int>(Text.length()); ++i)
            {
                Assert::AreEqual(i * 2 + 1, clusterMapIndexElements[i]);
                Assert::AreEqual(i % 2 == 0, !!isShapedAloneElements[i]);
            }

            for (int i = 0; i < static_cast<int>(Text.length()) + extraGlyphs; ++i)
            {
                Assert::AreEqual(15, static_cast<int>(glyphShapingElements[i].Justification));
                Assert::AreEqual(i % 2 == 0, !!glyphShapingElements[i].IsClusterStart);
                Assert::AreEqual(i % 2 != 0, !!glyphShapingElements[i].IsDiacritic);
                Assert::AreEqual(i % 2 == 0, !!glyphShapingElements[i].IsZeroWidthSpace);

                Assert::AreEqual(i * 3 + 2, glyphElements[i].Index);
                Assert::AreEqual(static_cast<float>(i)* 11.0f + 1.0f, glyphElements[i].Advance);
                Assert::AreEqual(static_cast<float>(i)* 12.0f + 2.0f, glyphElements[i].AdvanceOffset);
                Assert::AreEqual(static_cast<float>(i)* 13.0f + 3.0f, glyphElements[i].AscenderOffset);
            }
        }

        ComPtr<ICanvasTextAnalyzer> Create()
        {
            ComPtr<ICanvasTextAnalyzer> analyzer;
            ThrowIfFailed(m_textAnalyzerFactory->Create(WinString(Text.c_str()), TextDirection, &analyzer));

            return analyzer;
        }

        ComPtr<ICanvasTextAnalyzer> CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel()
        {
            ComPtr<ICanvasTextAnalyzer> analyzer;
            ThrowIfFailed(m_textAnalyzerFactory->CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel(
                WinString(Text.c_str()),
                TextDirection,
                NumberSubstitution.Get(),
                VerticalGlyphOrientation,
                BidiLevel,
                &analyzer));

            return analyzer;
        }

        ComPtr<ICanvasTextAnalyzer> CreateWithOptions()
        {
            ComPtr<ICanvasTextAnalyzer> analyzer;
            ThrowIfFailed(m_textAnalyzerFactory->CreateWithOptions(
                WinString(Text.c_str()),
                TextDirection,
                Options.Get(),
                &analyzer));

            return analyzer;
        }

        void CreateAnalyzerAndChooseFonts()
        {
            auto textAnalyzer = Create();

            ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
            Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(TextFormat.Get(), &result));
        }
    };

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_UsesSystemFontCollection)
    {
        Fixture f;

        f.ExpectGetSystemFontCollection();
        f.ExpectMapCharacters();

        f.CreateAnalyzerAndChooseFonts();
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_NullFontSet_UsesSystemFontCollection)
    {
        Fixture f;

        f.ExpectGetSystemFontCollection();
        f.ExpectMapCharacters();

        auto textAnalyzer = f.Create();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFonts(Make<CanvasTextFormat>().Get(), nullptr, &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_PassesThrough_TextFormatProperties)
    {
        Fixture f;

        f.ExpectedWeight = DWRITE_FONT_WEIGHT_ULTRA_BOLD;
        f.ExpectedStyle = DWRITE_FONT_STYLE_ITALIC;
        f.ExpectedStretch = DWRITE_FONT_STRETCH_EXPANDED;
        f.ExpectMapCharacters();

        ThrowIfFailed(f.TextFormat->put_FontWeight(ABI::Windows::UI::Text::FontWeight{ 800 }));
        ThrowIfFailed(f.TextFormat->put_FontStyle(ABI::Windows::UI::Text::FontStyle_Italic));
        ThrowIfFailed(f.TextFormat->put_FontStretch(ABI::Windows::UI::Text::FontStretch_Expanded));

        f.CreateAnalyzerAndChooseFonts();
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_PassesThrough_TextFormatFontFamily)
    {
        Fixture f;

        f.ExpectedFamilyName = L"Parchment";
        f.ExpectMapCharacters();

        ThrowIfFailed(f.TextFormat->put_FontFamily(WinString(L"Parchment")));

        f.CreateAnalyzerAndChooseFonts();
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFontsUsingSystemFontSet_PassesThroughCorrectFontSet)
    {
        Fixture f;

        auto dwriteFontCollection = Make<MockDWriteFontCollection>();
        f.ExpectedFontCollection = dwriteFontCollection;
        f.ExpectMapCharacters();

        auto textAnalyzer = f.Create();

#if WINVER > _WIN32_WINNT_WINBLUE
        auto dwriteFontSet = Make<MockDWriteFontSet>();
        f.GetAdapter()->GetMockDWriteFactory()->CreateFontCollectionFromFontSetMethod.SetExpectedCalls(1,
            [&](IDWriteFontSet* fontSet, IDWriteFontCollection1** fontCollection)
        {
            Assert::IsTrue(IsSameInstance(dwriteFontSet.Get(), fontSet));
            dwriteFontCollection.CopyTo(fontCollection);
            return S_OK;
        });
        auto canvasFontSet = ResourceManager::GetOrCreate<ICanvasFontSet>(dwriteFontSet.Get());
#else
        auto canvasFontSet = ResourceManager::GetOrCreate<ICanvasFontSet>(dwriteFontCollection.Get());
#endif

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFonts(f.TextFormat.Get(), canvasFontSet.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_UsesCorrectTextPositionAndLength)
    {
        Fixture f;

        f.Text = L"Some text";

        f.ExpectedTextPosition = 0;
        f.ExpectedTextLength = static_cast<uint32_t>(f.Text.length());
        f.ReturnedMappedLength = static_cast<uint32_t>(f.Text.length());
        f.ExpectMapCharacters();

        auto textAnalyzer = f.Create();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_TextAtPosition)
    {
        struct TestCase
        {
            uint32_t Position;
            wchar_t const* ExpectedText;
            uint32_t ExpectedCharacterCount;
        } testCases[] = {
            { 0, L"abc", 3 },
            { 1, L"bc", 2 },
            { 2, L"c", 1 },
            { 3, nullptr, 0 },
            { 999, nullptr, 0 }
        };

        for (auto testCase : testCases)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
            {
                wchar_t const* result;
                uint32_t characterCount;
                Assert::AreEqual(S_OK, source->GetTextAtPosition(testCase.Position, &result, &characterCount));
                Assert::AreEqual(testCase.ExpectedText, result);
                Assert::AreEqual(testCase.ExpectedCharacterCount, characterCount);
            };
            f.ExpectMapCharacters();

            f.CreateAnalyzerAndChooseFonts();
        }
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_TextBeforePosition)
    {
        struct TestCase
        {
            uint32_t Position;
            wchar_t const* ExpectedText;
            uint32_t ExpectedCharacterCount;
        } testCases[] = {
            { 0, nullptr, 0 },
            { 1, L"abc", 1 },
            { 2, L"abc", 2 },
            { 3, L"abc", 3 },
            { 999, nullptr, 0 }
        };

        for (auto testCase : testCases)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
            {
                wchar_t const* result;
                uint32_t characterCount;
                Assert::AreEqual(S_OK, source->GetTextBeforePosition(testCase.Position, &result, &characterCount));
                Assert::AreEqual(testCase.ExpectedText, result);
                Assert::AreEqual(testCase.ExpectedCharacterCount, characterCount);
            };
            f.ExpectMapCharacters();

            f.CreateAnalyzerAndChooseFonts();
        }
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_ParagraphReadingDirection)
    {
        Fixture f;

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
        {
            Assert::AreEqual(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, source->GetParagraphReadingDirection());
        };
        f.ExpectMapCharacters();

        f.TextDirection = CanvasTextDirection::BottomToTopThenRightToLeft;

        auto textAnalyzer = f.Create();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(Make<CanvasTextFormat>().Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_LocaleName_FromTextFormat)
    {
        for (auto testCase : testCasesForCharacterRanges)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
            {
                wchar_t const* result;
                uint32_t characterCount;
                Assert::AreEqual(S_OK, source->GetLocaleName(testCase.Position, &characterCount, &result));
                Assert::AreEqual(L"xx-yy", result);
                Assert::AreEqual(testCase.ExpectedCharacterCount, characterCount);
            };
            f.ExpectMapCharacters();

            ThrowIfFailed(f.TextFormat->put_LocaleName(WinString(L"xx-yy")));

            f.CreateAnalyzerAndChooseFonts();
        }
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_LocaleName_FromOptions_FormatChangeAtEachCharacter)
    {
        Fixture f;

        f.Options->GetLocaleNameMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, HSTRING* out)
        {
            static WinString locales[] = { WinString(L"00-00"), WinString(L"11-11"), WinString(L"22-22") };

            Assert::IsTrue(index >= 0 && index < 3);
            *out = locales[index];

            *characterCount = 1;

            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
            {
                wchar_t const* result;
                uint32_t characterCount;

                Assert::AreEqual(S_OK, source->GetLocaleName(0, &characterCount, &result));
                Assert::AreEqual(L"00-00", result);
                Assert::AreEqual(1u, characterCount);

                Assert::AreEqual(S_OK, source->GetLocaleName(1, &characterCount, &result));
                Assert::AreEqual(L"11-11", result);
                Assert::AreEqual(1u, characterCount);

                Assert::AreEqual(S_OK, source->GetLocaleName(2, &characterCount, &result));
                Assert::AreEqual(L"22-22", result);
                Assert::AreEqual(1u, characterCount);
            };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_LocaleName_FromOptions_TwoCharactersWithOneFormatThenOneWithAnother)
    {
        Fixture f;

        f.Options->GetLocaleNameMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, HSTRING* out)
        {
            static WinString locales[] = { WinString(L"00-00"), WinString(L"11-11") };
            if (index == 0)
            {
                *out = locales[0];
                *characterCount = 2;
            }
            else if (index == 1)
            {
                *out = locales[0];
                *characterCount = 1;
            }
            else if (index == 2)
            {
                *out = locales[1];
                *characterCount = 1;
            }

            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
            {
                wchar_t const* result;
                uint32_t characterCount;

                Assert::AreEqual(S_OK, source->GetLocaleName(0, &characterCount, &result));
                Assert::AreEqual(L"00-00", result);
                Assert::AreEqual(2u, characterCount);

                Assert::AreEqual(S_OK, source->GetLocaleName(1, &characterCount, &result));
                Assert::AreEqual(L"00-00", result);
                Assert::AreEqual(1u, characterCount);

                Assert::AreEqual(S_OK, source->GetLocaleName(2, &characterCount, &result));
                Assert::AreEqual(L"11-11", result);
                Assert::AreEqual(1u, characterCount);
            };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_NumberSubstitution_FromDefault)
    {
        auto dwriteNumberSubstitution = Make<MockDWriteNumberSubstitution>();
        auto numberSubstitution = ResourceManager::GetOrCreate<ICanvasNumberSubstitution>(dwriteNumberSubstitution.Get());

        for (auto testCase : testCasesForCharacterRanges)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
                {
                    ComPtr<IDWriteNumberSubstitution> result;
                    uint32_t characterCount;
                    Assert::AreEqual(S_OK, source->GetNumberSubstitution(testCase.Position, &characterCount, &result));
                    Assert::IsTrue(IsSameInstance(dwriteNumberSubstitution.Get(), result.Get()));
                    Assert::AreEqual(testCase.ExpectedCharacterCount, characterCount);
                };
            f.ExpectMapCharacters();

            f.NumberSubstitution = numberSubstitution;

            auto textAnalyzer = f.CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel();

            ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
            Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
        }
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_NumberSubstitution_FromOptions_FormatChangeAtEachCharacter)
    {
        ComPtr<MockDWriteNumberSubstitution> dwriteNumberSubstitutions[3];
        ComPtr<ICanvasNumberSubstitution> numberSubstitutions[3];
        for (int i = 0; i < 3; ++i)
        {
            dwriteNumberSubstitutions[i] = Make<MockDWriteNumberSubstitution>();
            numberSubstitutions[i] = ResourceManager::GetOrCreate<ICanvasNumberSubstitution>(dwriteNumberSubstitutions[i].Get());
        }

        Fixture f;

        f.Options->GetNumberSubstitutionMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, ICanvasNumberSubstitution** out)
        {
            Assert::IsTrue(index >= 0 && index < 3);
            numberSubstitutions[index].CopyTo(out);
            *characterCount = 1;
            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [&](IDWriteTextAnalysisSource1* source)
        {
            ComPtr<IDWriteNumberSubstitution> result;
            uint32_t characterCount;

            for (int i = 0; i < 3; i++)
            {
                Assert::AreEqual(S_OK, source->GetNumberSubstitution(i, &characterCount, &result));
                Assert::IsTrue(IsSameInstance(dwriteNumberSubstitutions[i].Get(), result.Get()));
                Assert::AreEqual(1, static_cast<int>(characterCount));
            }
        };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_NumberSubstitution_FromOptions_TwoCharactersWithOneFormatThenOneWithAnother)
    {
        ComPtr<MockDWriteNumberSubstitution> dwriteNumberSubstitutions[2];
        ComPtr<ICanvasNumberSubstitution> numberSubstitutions[2];
        for (int i = 0; i < 2; ++i)
        {
            dwriteNumberSubstitutions[i] = Make<MockDWriteNumberSubstitution>();
            numberSubstitutions[i] = ResourceManager::GetOrCreate<ICanvasNumberSubstitution>(dwriteNumberSubstitutions[i].Get());
        }

        Fixture f;

        f.Options->GetNumberSubstitutionMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, ICanvasNumberSubstitution** out)
        {
            if (index == 0)
            {
                numberSubstitutions[0].CopyTo(out);
                *characterCount = 2;
            }
            else if (index == 1)
            {
                numberSubstitutions[0].CopyTo(out);
                *characterCount = 1;
            }
            else if (index == 2)
            {
                numberSubstitutions[1].CopyTo(out);
                *characterCount = 1;
            }
            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [&](IDWriteTextAnalysisSource1* source)
        {
            ComPtr<IDWriteNumberSubstitution> result;
            uint32_t characterCount;

            Assert::AreEqual(S_OK, source->GetNumberSubstitution(0, &characterCount, &result));
            Assert::IsTrue(IsSameInstance(dwriteNumberSubstitutions[0].Get(), result.Get()));
            Assert::AreEqual(2, static_cast<int>(characterCount));

            Assert::AreEqual(S_OK, source->GetNumberSubstitution(1, &characterCount, &result));
            Assert::IsTrue(IsSameInstance(dwriteNumberSubstitutions[0].Get(), result.Get()));
            Assert::AreEqual(1, static_cast<int>(characterCount));

            Assert::AreEqual(S_OK, source->GetNumberSubstitution(2, &characterCount, &result));
            Assert::IsTrue(IsSameInstance(dwriteNumberSubstitutions[1].Get(), result.Get()));
            Assert::AreEqual(1, static_cast<int>(characterCount));
        };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_VerticalGlyphOrientation_FromDefault)
    {
        for (auto testCase : testCasesForCharacterRanges)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
            {
                DWRITE_VERTICAL_GLYPH_ORIENTATION result;
                uint32_t characterCount;
                uint8_t unused;
                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(testCase.Position, &characterCount, &result, &unused));
                Assert::AreEqual(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, result);
                Assert::AreEqual(testCase.ExpectedCharacterCount, characterCount);
            };
            f.ExpectMapCharacters();

            f.VerticalGlyphOrientation = CanvasVerticalGlyphOrientation::Stacked;

            auto textAnalyzer = f.CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel();

            ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
            Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
        }
    }


    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_VerticalGlyphOrientation_FromOptions_FormatChangeAtEachCharacter)
    {
        Fixture f;

        f.Options->GetVerticalGlyphOrientationMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, CanvasVerticalGlyphOrientation* out)
        {
            *out = index % 2 == 0 ? CanvasVerticalGlyphOrientation::Stacked : CanvasVerticalGlyphOrientation::Default;
            *characterCount = 1;
            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
        {
            DWRITE_VERTICAL_GLYPH_ORIENTATION result;
            uint8_t unused;
            uint32_t characterCount;

            for (int i = 0; i < 3; i++)
            {
                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(i, &characterCount, &result, &unused));
                Assert::AreEqual(i % 2 == 0 ? DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED : DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT, result);
                Assert::AreEqual(1, static_cast<int>(characterCount));
            }
        };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_VerticalGlyphOrientation_FromOptions_TwoCharactersWithOneFormatThenOneWithAnother)
    {
        Fixture f;

        f.Options->GetVerticalGlyphOrientationMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, CanvasVerticalGlyphOrientation* out)
        {
            if (index == 0)
            {
                *out = CanvasVerticalGlyphOrientation::Stacked;
                *characterCount = 2;
            }
            else if (index == 1)
            {
                *out = CanvasVerticalGlyphOrientation::Stacked;
                *characterCount = 1;
            }
            else if (index == 2)
            {
                *out = CanvasVerticalGlyphOrientation::Default;
                *characterCount = 1;
            }

            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
        {
            DWRITE_VERTICAL_GLYPH_ORIENTATION result;
            uint8_t unused;
            uint32_t characterCount;

            Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(0, &characterCount, &result, &unused));
            Assert::AreEqual(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, result);
            Assert::AreEqual(2, static_cast<int>(characterCount));

            Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(1, &characterCount, &result, &unused));
            Assert::AreEqual(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, result);
            Assert::AreEqual(1, static_cast<int>(characterCount));

            Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(2, &characterCount, &result, &unused));
            Assert::AreEqual(DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT, result);
            Assert::AreEqual(1, static_cast<int>(characterCount));
        };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }


    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_Bidi_FromDefault)
    {
        for (auto testCase : testCasesForCharacterRanges)
        {
            Fixture f;

            f.MockDoingThingsToTextAnalysisSource =
                [&](IDWriteTextAnalysisSource1* source)
            {
                DWRITE_VERTICAL_GLYPH_ORIENTATION unused;
                uint32_t characterCount;
                uint8_t result;
                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(testCase.Position, &characterCount, &unused, &result));
                Assert::AreEqual(123, static_cast<int>(result));
            };
            f.ExpectMapCharacters();

            f.BidiLevel = 123;

            auto textAnalyzer = f.CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel();

            ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
            Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
        }
    }


    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_BidiLevel_FromOptions_FormatChangeAtEachCharacter)
    {
        Fixture f;

        f.Options->GetBidiLevelMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, uint32_t* out)
        {
            *out = static_cast<uint32_t>(index * 3 + 1);
            *characterCount = 1;
            return S_OK;
        });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
        {
            DWRITE_VERTICAL_GLYPH_ORIENTATION unused;
            uint8_t result;
            uint32_t characterCount;

            for (int i = 0; i < 3; i++)
            {
                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(i, &characterCount, &unused, &result));
                Assert::AreEqual(i * 3 + 1, static_cast<int>(result));
                Assert::AreEqual(1, static_cast<int>(characterCount));
            }
        };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }


    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_BidiLevel_FromOptions_TwoCharactersWithOneFormatThenOneWithAnother)
    {
        Fixture f;


        f.Options->GetBidiLevelMethod.SetExpectedCalls(3,
            [&](int index, int* characterCount, uint32_t* out)
            {
                if (index == 0)
                {
                    *out = 12;
                    *characterCount = 2;
                }
                else if (index == 1)
                {
                    *out = 12;
                    *characterCount = 1;
                }
                else if (index == 2)
                {
                    *out = 34;
                    *characterCount = 1;
                }
                return S_OK;
            });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
            {
                DWRITE_VERTICAL_GLYPH_ORIENTATION unused;
                uint8_t result;
                uint32_t characterCount;

                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(0, &characterCount, &unused, &result));
                Assert::AreEqual(12, static_cast<int>(result));
                Assert::AreEqual(2, static_cast<int>(characterCount));

                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(1, &characterCount, &unused, &result));
                Assert::AreEqual(12, static_cast<int>(result));
                Assert::AreEqual(1, static_cast<int>(characterCount));

                Assert::AreEqual(S_OK, source->GetVerticalGlyphOrientation(2, &characterCount, &unused, &result));
                Assert::AreEqual(34, static_cast<int>(result));
                Assert::AreEqual(1, static_cast<int>(characterCount));
            };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_ChooseFonts_AppReturnsBadBidi_CausesError)
    {
        Fixture f;

        f.Options->GetBidiLevelMethod.SetExpectedCalls(1,
            [&](int index, int* characterCount, uint32_t* out)
            {
                *out = UINT8_MAX + 1;
                *characterCount = static_cast<uint32_t>(f.Text.length()) - index;
                return S_OK;
            });

        f.MockDoingThingsToTextAnalysisSource =
            [](IDWriteTextAnalysisSource1* source)
            {
                uint32_t characterCount;
                DWRITE_VERTICAL_GLYPH_ORIENTATION result;
                uint8_t unused;
                Assert::AreEqual(E_INVALIDARG, source->GetVerticalGlyphOrientation(0, &characterCount, &result, &unused));
            };
        f.ExpectMapCharacters();

        auto textAnalyzer = f.CreateWithOptions();

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->ChooseFontsUsingSystemFontSet(f.TextFormat.Get(), &result));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_AnalyzeScript_BadArg)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        Assert::AreEqual(E_INVALIDARG, textAnalyzer->AnalyzeScript(nullptr));
        Assert::AreEqual(E_INVALIDARG, textAnalyzer->AnalyzeScriptWithLocale(WinString(L""), nullptr));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_AnalyzeScript_UniformSpan)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.TextAnalyzer->AnalyzeScriptMethod.SetExpectedCalls(1,
            [&](IDWriteTextAnalysisSource*, UINT32 textPosition, UINT32 textLength, IDWriteTextAnalysisSink* sink)
            {
                Assert::AreEqual(0u, textPosition);
                Assert::AreEqual(static_cast<uint32_t>(f.Text.length()), textLength);

                DWRITE_SCRIPT_ANALYSIS scriptAnalysis{};
                scriptAnalysis.script = 123;
                scriptAnalysis.shapes = DWRITE_SCRIPT_SHAPES_NO_VISUAL;
                ThrowIfFailed(sink->SetScriptAnalysis(textPosition, textLength, &scriptAnalysis));

                return S_OK;
            });

        ComPtr<IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> result;
        Assert::AreEqual(S_OK, textAnalyzer->AnalyzeScript(&result));

        uint32_t size;
        Assert::AreEqual(S_OK, result->get_Size(&size));
        Assert::AreEqual(1u, size);

        ComPtr<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>> element;
        ThrowIfFailed(result->GetAt(0, &element));

        CanvasAnalyzedScript analyzedScript{};
        ThrowIfFailed(element->get_Value(&analyzedScript));

        Assert::AreEqual(123, analyzedScript.ScriptIdentifier);
        Assert::AreEqual(CanvasScriptShape::NoVisual, analyzedScript.Shape);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetScriptProperties_BadArg)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        CanvasAnalyzedScript analyzedScript{};
        Assert::AreEqual(E_INVALIDARG, textAnalyzer->GetScriptProperties(analyzedScript, nullptr));
    }

    uint32_t MakeIsoScriptCode(wchar_t const* name)
    {
        return
            (static_cast<uint32_t>(name[0]) << 0) |
            (static_cast<uint32_t>(name[1]) << 8) |
            (static_cast<uint32_t>(name[2]) << 16) |
            (static_cast<uint32_t>(name[3]) << 24);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetScriptProperties_ValuesPassedThrough)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.TextAnalyzer->GetScriptPropertiesMethod.SetExpectedCalls(1,
            [&](DWRITE_SCRIPT_ANALYSIS scriptAnalysis, DWRITE_SCRIPT_PROPERTIES* scriptProperties)
            {
                DWRITE_SCRIPT_PROPERTIES result{};

                result.isoScriptCode = MakeIsoScriptCode(L"Zzzz");
                result.isoScriptNumber = 123;
                result.clusterLookahead = 456;
                result.justificationCharacter = '%';

                result.restrictCaretToClusters = TRUE;
                result.isDiscreteWriting = TRUE;
                result.isDistributedWithinCluster = TRUE;

                result.usesWordDividers = FALSE;
                result.isBlockWriting = FALSE;
                result.isConnectedWriting = FALSE;
                result.isCursiveWriting = FALSE;
                *scriptProperties = result;

                return S_OK;
            });

        CanvasAnalyzedScript analyzedScript{};
        CanvasScriptProperties scriptProperties{};
        Assert::AreEqual(S_OK, textAnalyzer->GetScriptProperties(analyzedScript, &scriptProperties));

        Assert::AreEqual(0, wcscmp(L"Zzzz", static_cast<wchar_t const*>(WinString(scriptProperties.IsoScriptCode))));

        Assert::AreEqual(123, scriptProperties.IsoScriptNumber);
        Assert::AreEqual(456, scriptProperties.ClusterLookahead);

        Assert::AreEqual(0, wcscmp(L"%", static_cast<wchar_t const*>(WinString(scriptProperties.JustificationCharacter))));

        Assert::IsTrue(!!scriptProperties.RestrictCaretToClusters);
        Assert::IsTrue(!!scriptProperties.IsDiscreteWriting);
        Assert::IsTrue(!!scriptProperties.IsDistributedWithinCluster);

        Assert::IsFalse(!!scriptProperties.UsesWordDividers);
        Assert::IsFalse(!!scriptProperties.IsBlockWriting);
        Assert::IsFalse(!!scriptProperties.IsConnectedWriting);
        Assert::IsFalse(!!scriptProperties.IsCursiveWriting);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetScriptProperties_IsoScriptNameConversion)
    {
        wchar_t const* isoScriptCodeNames[] = { // From http://unicode.org/iso15924/iso15924-codes.html
            L"Adlm", L"Afak", L"Aghb", L"Ahom", L"Arab", L"Aran", L"Armi", L"Armn", L"Avst", L"Bali", L"Bamu", L"Bass", L"Batk", L"Beng", L"Bhks", L"Blis", L"Bopo",
            L"Brah", L"Brai", L"Bugi", L"Buhd", L"Cakm", L"Cans", L"Cari", L"Cham", L"Cher", L"Cirt", L"Copt", L"Cprt", L"Cyrl", L"Cyrs", L"Deva", L"Dsrt", L"Dupl",
            L"Egyd", L"Egyh", L"Egyp", L"Elba", L"Ethi", L"Geok", L"Geor", L"Glag", L"Goth", L"Gran", L"Grek", L"Gujr", L"Guru", L"Hang", L"Hani", L"Hano", L"Hans",
            L"Hant", L"Hatr", L"Hebr", L"Hira", L"Hluw", L"Hmng", L"Hrkt", L"Hung", L"Inds", L"Ital", L"Java", L"Jpan", L"Jurc", L"Kali", L"Kana", L"Khar", L"Khmr",
            L"Khoj", L"Kitl", L"Kits", L"Knda", L"Kore", L"Kpel", L"Kthi", L"Lana", L"Laoo", L"Latf", L"Latg", L"Latn", L"Leke", L"Lepc", L"Limb", L"Lina", L"Linb",
            L"Lisu", L"Loma", L"Lyci", L"Lydi", L"Mahj", L"Mand", L"Mani", L"Marc", L"Maya", L"Mend", L"Merc", L"Mero", L"Mlym", L"Modi", L"Mong", L"Moon", L"Mroo",
            L"Mtei", L"Mult", L"Mymr", L"Narb", L"Nbat", L"Nkgb", L"Nkoo", L"Nshu", L"Ogam", L"Olck", L"Orkh", L"Orya", L"Osge", L"Osma", L"Palm", L"Pauc", L"Perm",
            L"Phag", L"Phli", L"Phlp", L"Phlv", L"Phnx", L"Plrd", L"Prti", L"Qaaa", L"Qabx", L"Rjng", L"Roro", L"Runr", L"Samr", L"Sara", L"Sarb", L"Saur", L"Sgnw",
            L"Shaw", L"Shrd", L"Sidd", L"Sind", L"Sinh", L"Sora", L"Sund", L"Sylo", L"Syrc", L"Syre", L"Syrj", L"Syrn", L"Tagb", L"Takr", L"Tale", L"Talu", L"Taml",
            L"Tang", L"Tavt", L"Telu", L"Teng", L"Tfng", L"Tglg", L"Thaa", L"Thai", L"Tibt", L"Tirh", L"Ugar", L"Vaii", L"Visp", L"Wara", L"Wole", L"Xpeo", L"Xsux",
            L"Yiii", L"Zinh", L"Zmth", L"Zsym", L"Zxxx", L"Zyyy", L"Zzzz"
        };

        for (auto name : isoScriptCodeNames)
        {
            Fixture f;
            auto textAnalyzer = f.Create();

            f.TextAnalyzer->GetScriptPropertiesMethod.SetExpectedCalls(1,
                [&](DWRITE_SCRIPT_ANALYSIS scriptAnalysis, DWRITE_SCRIPT_PROPERTIES* scriptProperties)
                {
                    DWRITE_SCRIPT_PROPERTIES result{};
                    result.isoScriptCode = MakeIsoScriptCode(name);
                    *scriptProperties = result;

                    return S_OK;
                });

            CanvasAnalyzedScript analyzedScript{};
            CanvasScriptProperties scriptProperties{};
            Assert::AreEqual(S_OK, textAnalyzer->GetScriptProperties(analyzedScript, &scriptProperties));

            Assert::AreEqual(0, wcscmp(name, static_cast<wchar_t const*>(WinString(scriptProperties.IsoScriptCode))));
        }
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetScriptProperties_TwoCodeUnitJustificationCharacter)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.TextAnalyzer->GetScriptPropertiesMethod.SetExpectedCalls(1,
            [&](DWRITE_SCRIPT_ANALYSIS scriptAnalysis, DWRITE_SCRIPT_PROPERTIES* scriptProperties)
        {
            DWRITE_SCRIPT_PROPERTIES result{};
            result.justificationCharacter = 0x1f197; // 🆗
            *scriptProperties = result;

            return S_OK;
        });

        CanvasAnalyzedScript analyzedScript{};
        CanvasScriptProperties scriptProperties{};
        Assert::AreEqual(S_OK, textAnalyzer->GetScriptProperties(analyzedScript, &scriptProperties));
        Assert::AreEqual(0, wcscmp(L"🆗", static_cast<wchar_t const*>(WinString(scriptProperties.JustificationCharacter))));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphs_BasicTest)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs();
        f.GetGlyphs(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphsWithAllOptions_BasicTest)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs();
        f.GetGlyphsWithAllOptions(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphsWithAllOptions_NumberSubstitution)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs();
        f.UseNumberSubstitution();
        f.GetGlyphsWithAllOptions(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphsWithAllOptions_TypographyRanges)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs();
        f.UseTypographyRanges();
        f.GetGlyphsWithAllOptions(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphsWithAllOptions_Locale)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs();
        f.Locale = L"xx-yy";
        f.GetGlyphsWithAllOptions(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasGlyphJustification_Values)
    {
        Assert::AreEqual(0, static_cast<int>(CanvasGlyphJustification::None));
        Assert::AreEqual(1, static_cast<int>(CanvasGlyphJustification::ArabicBlank));
        Assert::AreEqual(2, static_cast<int>(CanvasGlyphJustification::Character));
        Assert::AreEqual(4, static_cast<int>(CanvasGlyphJustification::Blank));
        Assert::AreEqual(7, static_cast<int>(CanvasGlyphJustification::ArabicNormal));
        Assert::AreEqual(8, static_cast<int>(CanvasGlyphJustification::ArabicKashida));
        Assert::AreEqual(9, static_cast<int>(CanvasGlyphJustification::ArabicAlef));
        Assert::AreEqual(10, static_cast<int>(CanvasGlyphJustification::ArabicHa));
        Assert::AreEqual(11, static_cast<int>(CanvasGlyphJustification::ArabicRa));
        Assert::AreEqual(12, static_cast<int>(CanvasGlyphJustification::ArabicBa));
        Assert::AreEqual(13, static_cast<int>(CanvasGlyphJustification::ArabicBara));
        Assert::AreEqual(14, static_cast<int>(CanvasGlyphJustification::ArabicSeen));
        Assert::AreEqual(15, static_cast<int>(CanvasGlyphJustification::ArabicSeenM));
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphs_NeedsToResizeBuffer)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        const int bufferSize = 3 * static_cast<int>(f.Text.length()) / 2 + 16;

        f.ExpectGetGlyphs(2);
        f.AdditionalGlyphsToAddDuringGetGlyphs = bufferSize - static_cast<int>(f.Text.length()) + 1;

        f.GetGlyphs(textAnalyzer);
    }

    TEST_METHOD_EX(CanvasTextAnalyzer_GetGlyphs_GlyphCountTooHigh)
    {
        Fixture f;
        auto textAnalyzer = f.Create();

        f.ExpectGetGlyphs(3, 0);
        f.AdditionalGlyphsToAddDuringGetGlyphs = 999;

        uint32_t glyphCount;
        CanvasGlyph* glyphElements;

        CanvasCharacterRange characterRange{ 0, static_cast<int>(f.Text.length()) };
        Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER), textAnalyzer->GetGlyphs(
            characterRange,
            f.FontFace.Get(),
            f.FontSize,
            f.IsSideways,
            f.IsRightToLeft,
            f.AnalyzedScript,
            &glyphCount,
            &glyphElements));
    }
};