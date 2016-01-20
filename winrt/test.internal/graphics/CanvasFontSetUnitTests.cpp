// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "mocks/MockDWriteFont.h"
#include "mocks/MockDWriteFontFace.h"
#include "mocks/MockDWriteFontFaceReference.h"
#include "mocks/MockDWriteFontFamily.h"
#include "mocks/MockDWriteFontSet.h"
#include "mocks/MockDWriteFontCollection.h"
#include "mocks/MockDWriteFontFile.h"
#include "stubs/LocalizedFontNames.h"
#include "stubs/StubDWriteFontSetBuilder.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "stubs/StubUri.h"
#include <lib/text/CanvasFontFace.h>
#include <lib/text/CanvasFontSet.h>

#if WINVER > _WIN32_WINNT_WINBLUE
static const struct TestFontProperty
{
    TestFontProperty(CanvasFontPropertyIdentifier id, wchar_t const* value, wchar_t const* locale)
        : Locale(locale)
        , Value(value)
    {
        Win2DProperty.Identifier = id;
        Win2DProperty.Locale = Locale;
        Win2DProperty.Value = Value;
        DWriteProperty.propertyId = static_cast<DWRITE_FONT_PROPERTY_ID>(id);
        DWriteProperty.localeName = WindowsGetStringRawBuffer(Locale, nullptr);
        DWriteProperty.propertyValue = WindowsGetStringRawBuffer(Value, nullptr);
    }
    CanvasFontProperty Win2DProperty;
    DWRITE_FONT_PROPERTY DWriteProperty;
    WinString Locale, Value;
    } sc_testProperties[]{
        TestFontProperty(CanvasFontPropertyIdentifier::SemanticTag, L"SomeName1", L"xx-aa"),
        TestFontProperty(CanvasFontPropertyIdentifier::DesignScriptLanguageTag, L"SomeName2", L"xx-bb"),
        TestFontProperty(CanvasFontPropertyIdentifier::PostscriptName, L"SomeName3", L"xx-cc"),
    };
#endif
    

#if WINVER > _WIN32_WINNT_WINBLUE
    struct FontSetFixture
    {
        ComPtr<MockDWriteFontFaceReference> DWriteFontFaceResources[3];
        ComPtr<MockDWriteFontSet> DWriteResource;
        ComPtr<MockDWriteFontFace> RealizedDWriteFontFace;

        FontSetFixture()
        {
            for (int i = 0; i < 3; ++i)
            {
                DWriteFontFaceResources[i] = Make<MockDWriteFontFaceReference>();

                RealizedDWriteFontFace = Make<MockDWriteFontFace>();
                DWriteFontFaceResources[i]->CreateFontFaceMethod.AllowAnyCall(
                    [&](IDWriteFontFace3** out)
                    {
                        return RealizedDWriteFontFace.CopyTo(out);
                    });
            }

            DWriteResource = Make<MockDWriteFontSet>();
        }

        void ExpectGetFonts()
        {
            DWriteResource->GetFontCountMethod.SetExpectedCalls(1, [&] {return 3; });
            DWriteResource->GetFontFaceReferenceMethod.SetExpectedCalls(3,
                [&](UINT32 index, IDWriteFontFaceReference** out)
                {
                    Assert::IsTrue(index < 3);
                    return DWriteFontFaceResources[index].CopyTo(out);
                });
        }

        void ExpectFindThirdFont()
        {
            DWriteResource->FindFontFaceMethod.SetExpectedCalls(1,
                [&](IDWriteFontFace* fontFace, UINT32* index, BOOL* exists)
                {
                    Assert::IsTrue(IsSameInstance(RealizedDWriteFontFace.Get(), fontFace));
                    *index = 2;
                    *exists = TRUE;
                    return S_OK;
                });
        }

        void ExpectFindNoFont()
        {
            DWriteResource->FindFontFaceMethod.SetExpectedCalls(1,
                [&](IDWriteFontFace* fontFace, UINT32* index, BOOL* exists)
                {
                    *exists = FALSE;
                    return S_OK;
                });
        }

        ComPtr<CanvasFontFace> CreateSimpleFontFace()
        {
            auto resource = Make<MockDWriteFontFaceReference>();

            resource->CreateFontFaceMethod.AllowAnyCall(
                [&](IDWriteFontFace3** out)
                {
                    ComPtr<MockDWriteFontFace> fontFace = Make<MockDWriteFontFace>();
                    return fontFace.CopyTo(out);
                });

            return Make<CanvasFontFace>(resource.Get());

        }
    };
#else
    struct FontSetFixture
    {
        //
        // This sets up a font collection with two families, three fonts in total.
        //
        ComPtr<MockDWriteFont> DWriteFontFaceResources[3];
        ComPtr<MockDWriteFontCollection> DWriteResource;
        ComPtr<MockDWriteFontFamily> FontFamilies[2];
        ComPtr<MockDWriteFontFace> RealizedDWriteFontFace;

        FontSetFixture()
        {
            for (int i = 0; i < 3; ++i)
            {
                DWriteFontFaceResources[i] = Make<MockDWriteFont>();

                RealizedDWriteFontFace = Make<MockDWriteFontFace>();
                DWriteFontFaceResources[i]->CreateFontFaceMethod.AllowAnyCall(
                    [&](IDWriteFontFace** out)
                    {
                        return RealizedDWriteFontFace.CopyTo(out);
                    });
            }

            DWriteResource = Make<MockDWriteFontCollection>();
        }

        void ExpectFindThirdFont()
        {
            ExpectRealization();
        }

        void ExpectFindNoFont()
        {
            ExpectRealization();
        }

        void ExpectGetFonts()
        {
            ExpectRealization();
        }

        ComPtr<CanvasFontFace> CreateSimpleFontFace()
        {
            auto resource = Make<MockDWriteFont>();

            resource->CreateFontFaceMethod.AllowAnyCall(
                [&](IDWriteFontFace** out)
                {
                    ComPtr<MockDWriteFontFace> fontFace = Make<MockDWriteFontFace>();
                    return fontFace.CopyTo(out);
                });

            return Make<CanvasFontFace>(resource.Get());
        }

    private:
        void ExpectRealization()
        {
            FontFamilies[0] = Make<MockDWriteFontFamily>();
            FontFamilies[0]->GetFontCountMethod.AllowAnyCall([&] {return 2u; });
            FontFamilies[0]->GetFontMethod.AllowAnyCall(
                [&](UINT32 index, IDWriteFont** out)
                {
                    Assert::IsTrue(index == 0 || index == 1);
                    return DWriteFontFaceResources[index].CopyTo(out);
                });

            FontFamilies[1] = Make<MockDWriteFontFamily>();
            FontFamilies[1]->GetFontCountMethod.AllowAnyCall([&] {return 1u; });
            FontFamilies[1]->GetFontMethod.AllowAnyCall(
                [&](UINT32 index, IDWriteFont** out)
                {
                    Assert::AreEqual(0u, index);
                    return DWriteFontFaceResources[2].CopyTo(out);
                });

            DWriteResource->GetFontFamilyCountMethod.SetExpectedCalls(1, [&] {return 2; });
            DWriteResource->GetFontFamilyMethod.SetExpectedCalls(2,
                [&](uint32_t index, IDWriteFontFamily** out)
                {
                    Assert::IsTrue(index == 0 || index == 1);
                    return FontFamilies[index].CopyTo(out);
                });
        }
    };
#endif

TEST_CLASS(CanvasFontSetTests)
{
    ComPtr<CanvasFontSet> CreateSimpleTestFontSet()
    {
#if WINVER > _WIN32_WINNT_WINBLUE
        auto resource = Make<MockDWriteFontSet>();
#else
        auto resource = Make<MockDWriteFontCollection>();
#endif
        return Make<CanvasFontSet>(resource.Get());
    }

    TEST_METHOD_EX(CanvasFontSet_ImplementsExpectedInterfaces)
    {
        auto canvasFontSet = CreateSimpleTestFontSet();

        ASSERT_IMPLEMENTS_INTERFACE(canvasFontSet, ICanvasFontSet);
        ASSERT_IMPLEMENTS_INTERFACE(canvasFontSet, ABI::Windows::Foundation::IClosable);
    }

    TEST_METHOD_EX(CanvasFontSet_NullArgs)
    {
        int i{};
        boolean b{};

        auto factory = Make<CanvasFontSetFactory>();
        Assert::AreEqual(E_INVALIDARG, factory->GetSystemFontSet(nullptr));

        auto canvasFontSet = CreateSimpleTestFontSet();
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->TryFindFontFace(nullptr, &i, &b));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->TryFindFontFace(reinterpret_cast<ICanvasFontFace*>(0x1234), nullptr, &b));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->TryFindFontFace(reinterpret_cast<ICanvasFontFace*>(0x1234), &i, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontSet->get_Fonts(nullptr));

#if WINVER > _WIN32_WINNT_WINBLUE
        CanvasFontProperty fp{};
        CanvasFontProperty* fpArray{};
        uint32_t u{};

        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetMatchingFontsFromProperties(1, &fp, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetMatchingFontsFromWwsFamily(WinString(L""), FontWeight{ 100 }, FontStretch_Normal, FontStyle_Normal, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->CountFontsMatchingProperty(fp, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetPropertyValuesFromIndex(0, CanvasFontPropertyIdentifier::FaceName, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetPropertyValuesFromIdentifier(CanvasFontPropertyIdentifier::FaceName, WinString(L""), &u, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetPropertyValuesFromIdentifier(CanvasFontPropertyIdentifier::FaceName, WinString(L""), nullptr, &fpArray));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetPropertyValues(CanvasFontPropertyIdentifier::FaceName, &u, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontSet->GetPropertyValues(CanvasFontPropertyIdentifier::FaceName, nullptr, &fpArray));
#endif
    }

    TEST_METHOD_EX(CanvasFontSet_Closed)
    {
        int i{};
        boolean b{};
        ComPtr<ICanvasFontSet> fontSet;
        ComPtr<IVectorView<CanvasFontFace*>> vector;
        ComPtr<IMapView<HSTRING, HSTRING>> map;

        auto canvasFontSet = CreateSimpleTestFontSet();

        Assert::AreEqual(S_OK, canvasFontSet->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->TryFindFontFace(reinterpret_cast<ICanvasFontFace*>(0x1234), &i, &b));

        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->get_Fonts(&vector));

#if WINVER > _WIN32_WINNT_WINBLUE
        CanvasFontProperty fp{};
        CanvasFontProperty* fpArray{};
        uint32_t u{};
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->GetMatchingFontsFromProperties(1, &fp, &fontSet));
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->GetMatchingFontsFromWwsFamily(WinString(L""), FontWeight{ 100 }, FontStretch_Normal, FontStyle_Normal, &fontSet));
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->CountFontsMatchingProperty(fp, &u));
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->GetPropertyValuesFromIndex(0, CanvasFontPropertyIdentifier::FaceName, &map));
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->GetPropertyValuesFromIdentifier(CanvasFontPropertyIdentifier::FaceName, WinString(L""), &u, &fpArray));
        Assert::AreEqual(RO_E_CLOSED, canvasFontSet->GetPropertyValues(CanvasFontPropertyIdentifier::FaceName, &u, &fpArray));
#endif
    }

    struct SystemFontSetFixture
    {
        std::shared_ptr<StubCanvasTextLayoutAdapter> m_adapter;

#if WINVER > _WIN32_WINNT_WINBLUE
        ComPtr<StubDWriteFontSet> m_systemFontSet;
#else
        ComPtr<MockDWriteFontCollection> m_systemFontSet;
#endif

        SystemFontSetFixture()
        {
            m_adapter = std::make_shared<StubCanvasTextLayoutAdapter>();
            CustomFontManagerAdapter::SetInstance(m_adapter);

#if WINVER > _WIN32_WINNT_WINBLUE
            m_adapter->GetMockDWriteFactory()->CreateFontSetBuilderMethod.AllowAnyCall(
                [&](IDWriteFontSetBuilder** out)
                {
                    auto stubFontSetBuilder = Make<StubDWriteFontSetBuilder>();
                    stubFontSetBuilder.CopyTo(out);
                    return S_OK;
                });

            m_systemFontSet = Make<StubDWriteFontSet>();

            m_adapter->GetMockDWriteFactory()->GetSystemFontSetMethod.SetExpectedCalls(1,
                [&](IDWriteFontSet** out)
                {
                    return m_systemFontSet.CopyTo(out);
                });
#else
            m_systemFontSet = Make<MockDWriteFontCollection>();

            m_adapter->GetMockDWriteFactory()->GetSystemFontCollectionMethod.SetExpectedCalls(1,
                [&](IDWriteFontCollection** out, BOOL)
                {
                    return m_systemFontSet.CopyTo(out);
                });
#endif
        }
    };

    TEST_METHOD_EX(CanvasFontSet_GetSystemFontSet)
    {
        SystemFontSetFixture f;

        auto factory = Make<CanvasFontSetFactory>();
        ComPtr<ICanvasFontSet> actualCanvasFontSet;
        Assert::AreEqual(S_OK, factory->GetSystemFontSet(&actualCanvasFontSet));

#if WINVER > _WIN32_WINNT_WINBLUE
        auto actualFontCollection = GetWrappedResource<IDWriteFontSet>(actualCanvasFontSet);
        // The font set that we get out should contain the same fonts as our stub font set.
        ComPtr<StubDWriteFontSet> actualStubFontSet = static_cast<StubDWriteFontSet*>(actualFontCollection.Get());
        for (int i = 0; i < 3; ++i)
        {
            Assert::IsTrue(IsSameInstance(f.m_systemFontSet->GetFontFaceReferenceInternal(i).Get(), actualStubFontSet->GetFontFaceReferenceInternal(i).Get()));
        }
#else
        auto actualFontCollection = GetWrappedResource<IDWriteFontCollection>(actualCanvasFontSet);

        Assert::IsTrue(IsSameInstance(f.m_systemFontSet.Get(), actualFontCollection.Get()));
#endif
    }

#if WINVER > _WIN32_WINNT_WINBLUE
    TEST_METHOD_EX(CanvasFontSet_GetSystemFontSet_RemovesRemoteFonts)
    {
        DWRITE_LOCALITY localities[] = { DWRITE_LOCALITY_PARTIAL, DWRITE_LOCALITY_REMOTE };
        for (auto locality : localities)
        {
            SystemFontSetFixture f;

            //
            // In our mock system font collection which has three fonts,
            // make the second font a remote one.
            //
            static_cast<StubDWriteFontFaceReference*>(f.m_systemFontSet->GetFontFaceReferenceInternal(1).Get())->SetLocality(locality);

            auto factory = Make<CanvasFontSetFactory>();
            ComPtr<ICanvasFontSet> actualCanvasFontSet;
            Assert::AreEqual(S_OK, factory->GetSystemFontSet(&actualCanvasFontSet));

            auto actualFontCollection = GetWrappedResource<IDWriteFontSet>(actualCanvasFontSet);
            ComPtr<StubDWriteFontSet> actualStubFontSet = static_cast<StubDWriteFontSet*>(actualFontCollection.Get());

            Assert::AreEqual(2u, actualStubFontSet->GetFontCount());
            Assert::IsTrue(IsSameInstance(f.m_systemFontSet->GetFontFaceReferenceInternal(0).Get(), actualStubFontSet->GetFontFaceReferenceInternal(0).Get()));
            Assert::IsTrue(IsSameInstance(f.m_systemFontSet->GetFontFaceReferenceInternal(2).Get(), actualStubFontSet->GetFontFaceReferenceInternal(1).Get()));
        }
    }
#endif

    TEST_METHOD_EX(CanvasFontSet_FindFontFace)
    {
        FontSetFixture f;
        auto thirdFontFace = Make<CanvasFontFace>(f.DWriteFontFaceResources[2].Get());

        auto canvasFontSet = Make<CanvasFontSet>(f.DWriteResource.Get());

        f.ExpectFindThirdFont();

        int value;
        boolean succeeded;
        Assert::AreEqual(S_OK, canvasFontSet->TryFindFontFace(thirdFontFace.Get(), &value, &succeeded));
        Assert::IsTrue(!!succeeded);
        Assert::AreEqual(2, value);
    }

    TEST_METHOD_EX(CanvasFontSet_FindFontFace_NotThere)
    {
        FontSetFixture f;

        auto someFontFace = f.CreateSimpleFontFace();

        auto canvasFontSet = Make<CanvasFontSet>(f.DWriteResource.Get());

        f.ExpectFindNoFont();

        int value;
        boolean succeeded;
        Assert::AreEqual(S_OK, canvasFontSet->TryFindFontFace(someFontFace.Get(), &value, &succeeded));
        Assert::IsFalse(!!succeeded);
    }

    TEST_METHOD_EX(CanvasFontSet_get_Fonts)
    {
        ComPtr<CanvasFontFace> fontFaces[3];

        FontSetFixture f;
        for (int i = 0; i < 3; ++i)
        {
            fontFaces[i] = Make<CanvasFontFace>(f.DWriteFontFaceResources[i].Get());
        }

        auto canvasFontSet = Make<CanvasFontSet>(f.DWriteResource.Get());

        f.ExpectGetFonts();

        ComPtr<IVectorView<CanvasFontFace*>> fonts;
        Assert::AreEqual(S_OK, canvasFontSet->get_Fonts(&fonts));

        uint32_t actualSize;
        ThrowIfFailed(fonts->get_Size(&actualSize));
        Assert::AreEqual(3u, actualSize);

        ComPtr<ICanvasFontFace> items[3];
        for (int i = 0; i < 3; ++i)
        {
            ThrowIfFailed(fonts->GetAt(i, &items[i]));
            Assert::IsTrue(IsSameInstance(fontFaces[i].Get(), items[i].Get()));
        }
    }

#if WINVER > _WIN32_WINNT_WINBLUE

    TEST_METHOD_EX(CanvasFontSet_GetMatchingFontsFromProperties)
    {
        auto filteredDWriteResource = Make<MockDWriteFontSet>();

        auto dwriteResource = Make<MockDWriteFontSet>();

        dwriteResource->GetMatchingFontsMethod0.SetExpectedCalls(1,
            [&](DWRITE_FONT_PROPERTY const* properties, UINT32 propertyCount, IDWriteFontSet** filteredSet)
            {
                Assert::AreEqual(2u, propertyCount);

                Assert::AreEqual(properties[0], sc_testProperties[0].DWriteProperty);
                Assert::AreEqual(properties[1], sc_testProperties[1].DWriteProperty);

                ThrowIfFailed(filteredDWriteResource.CopyTo(filteredSet));
                return S_OK;
            });

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        CanvasFontProperty properties[2] = { sc_testProperties[0].Win2DProperty, sc_testProperties[1].Win2DProperty };

        ComPtr<ICanvasFontSet> matchingFonts;
        Assert::AreEqual(S_OK, canvasFontSet->GetMatchingFontsFromProperties(2, properties, &matchingFonts));

        auto matchingFontsInnerResource = GetWrappedResource<IDWriteFontSet>(matchingFonts);
        Assert::IsTrue(IsSameInstance(filteredDWriteResource.Get(), matchingFontsInnerResource.Get()));
    }

    TEST_METHOD_EX(CanvasFontSet_GetMatchingFontsFromWwsFamily)
    {
        auto filteredDWriteResource = Make<MockDWriteFontSet>();

        auto dwriteResource = Make<MockDWriteFontSet>();

        dwriteResource->GetMatchingFontsMethod1.SetExpectedCalls(1,
            [&](WCHAR const* familyName, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STRETCH fontStretch, DWRITE_FONT_STYLE fontStyle, IDWriteFontSet** filteredSet)
            {
                Assert::AreEqual(L"SomeFamily", familyName);
                Assert::AreEqual(DWRITE_FONT_WEIGHT_ULTRA_BOLD, fontWeight);
                Assert::AreEqual(DWRITE_FONT_STRETCH_SEMI_CONDENSED, fontStretch);
                Assert::AreEqual(DWRITE_FONT_STYLE_ITALIC, fontStyle);

                ThrowIfFailed(filteredDWriteResource.CopyTo(filteredSet));
                return S_OK;
            });

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        ComPtr<ICanvasFontSet> matchingFonts;
        Assert::AreEqual(S_OK, canvasFontSet->GetMatchingFontsFromWwsFamily(WinString(L"SomeFamily"), FontWeight{ 800 }, FontStretch::FontStretch_SemiCondensed, FontStyle::FontStyle_Italic, &matchingFonts));

        auto matchingFontsInnerResource = GetWrappedResource<IDWriteFontSet>(matchingFonts);
        Assert::IsTrue(IsSameInstance(filteredDWriteResource.Get(), matchingFontsInnerResource.Get()));
    }

    TEST_METHOD_EX(CanvasFontSet_CountFontsMatchingProperty)
    {
        auto dwriteResource = Make<MockDWriteFontSet>();

        dwriteResource->GetPropertyOccurrenceCountMethod.SetExpectedCalls(1,
            [&](DWRITE_FONT_PROPERTY const* property, UINT32* out)
            {
                Assert::AreEqual(*property, sc_testProperties[0].DWriteProperty);
                *out = 42;
                return S_OK;
            });

        CanvasFontProperty property = sc_testProperties[0].Win2DProperty;

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        uint32_t count;
        Assert::AreEqual(S_OK, canvasFontSet->CountFontsMatchingProperty(property, &count));

        Assert::AreEqual(42u, count);
    }

    TEST_METHOD_EX(CanvasFontSet_GetPropertyValuesFromIndex)
    {
        auto dwriteResource = Make<MockDWriteFontSet>();

        dwriteResource->GetPropertyValuesMethod0.SetExpectedCalls(1,
            [&](UINT32 listIndex, DWRITE_FONT_PROPERTY_ID propertyId, BOOL* exists, IDWriteLocalizedStrings** values)
            {
                Assert::AreEqual(123u, listIndex);
                Assert::AreEqual(DWRITE_FONT_PROPERTY_ID_SEMANTIC_TAG, propertyId);

                *exists = true;

                auto localizedNames = Make<LocalizedFontNames>(
                    L"SomeName1", L"xx-aa",
                    L"SomeName2", L"xx-bb");
                ThrowIfFailed(localizedNames.CopyTo(values));
                return S_OK;
            });

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        ComPtr<IMapView<HSTRING, HSTRING>> map;
        Assert::AreEqual(S_OK, canvasFontSet->GetPropertyValuesFromIndex(123u, CanvasFontPropertyIdentifier::SemanticTag, &map));

        VerifyStringMapContainsKeyValue(map, L"xx-aa", L"SomeName1");
        VerifyStringMapContainsKeyValue(map, L"xx-bb", L"SomeName2");
    }

    TEST_METHOD_EX(CanvasFontSet_GetPropertyValuesFromIndex_NoSuchProperty)
    {
        auto dwriteResource = Make<MockDWriteFontSet>();

        dwriteResource->GetPropertyValuesMethod0.SetExpectedCalls(1,
            [&](UINT32, DWRITE_FONT_PROPERTY_ID, BOOL* exists, IDWriteLocalizedStrings**)
            {
                *exists = false;
                return S_OK;
            });

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        ComPtr<IMapView<HSTRING, HSTRING>> map;
        Assert::AreEqual(S_OK, canvasFontSet->GetPropertyValuesFromIndex(123u, CanvasFontPropertyIdentifier::SemanticTag, &map));

        Assert::IsNotNull(map.Get());

        uint32_t retrievedSize;
        ThrowIfFailed(map->get_Size(&retrievedSize));
        Assert::AreEqual(0u, retrievedSize);
    }

    void AssertStringsEqual(HSTRING a, HSTRING b)
    {
        std::wstring s1 = WindowsGetStringRawBuffer(a, nullptr);
        std::wstring s2 = WindowsGetStringRawBuffer(b, nullptr);
        Assert::AreEqual(s1, s2);
    }

    TEST_METHOD_EX(CanvasFontSet_GetPropertyValuesFromIdentifier)
    {
        auto dwriteResource = Make<MockDWriteFontSet>();

        auto localizedNames = Make<StringList>(
            sc_testProperties[0].DWriteProperty.propertyValue, sc_testProperties[0].DWriteProperty.localeName,
            sc_testProperties[1].DWriteProperty.propertyValue, sc_testProperties[1].DWriteProperty.localeName);

        dwriteResource->GetPropertyValuesMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_PROPERTY_ID propertyIdentifier, WCHAR const* preferredLocaleNames, IDWriteStringList** out)
            {
                Assert::AreEqual(DWRITE_FONT_PROPERTY_ID_SEMANTIC_TAG, propertyIdentifier);
                Assert::AreEqual(L"xx-aa; xx-bb;", preferredLocaleNames);
                ThrowIfFailed(localizedNames.CopyTo(out));
                return S_OK;
            });

        auto canvasFontSet = Make<CanvasFontSet>(dwriteResource.Get());

        uint32_t valueCount;
        CanvasFontProperty* valueElements;
        Assert::AreEqual(S_OK, canvasFontSet->GetPropertyValuesFromIdentifier(CanvasFontPropertyIdentifier::SemanticTag, WinString(L"xx-aa; xx-bb;"), &valueCount, &valueElements));
        
        Assert::AreEqual(2u, valueCount);

        Assert::AreEqual(CanvasFontPropertyIdentifier::SemanticTag, valueElements[0].Identifier);
        AssertStringsEqual(sc_testProperties[0].Win2DProperty.Locale, valueElements[0].Locale);
        AssertStringsEqual(sc_testProperties[0].Win2DProperty.Value, valueElements[0].Value);

        Assert::AreEqual(CanvasFontPropertyIdentifier::SemanticTag, valueElements[1].Identifier);
        AssertStringsEqual(sc_testProperties[1].Win2DProperty.Locale, valueElements[1].Locale);
        AssertStringsEqual(sc_testProperties[1].Win2DProperty.Value, valueElements[1].Value);
    }
#endif
    
    struct CustomFontFixture
    {
        std::shared_ptr<StubCanvasTextLayoutAdapter> m_adapter;

#if WINVER > _WIN32_WINNT_WINBLUE
        ComPtr<MockDWriteFontSet> m_mockDWriteFontSet;
#endif

        CustomFontFixture()
            : m_adapter(std::make_shared<StubCanvasTextLayoutAdapter>())
        {
            CustomFontManagerAdapter::SetInstance(m_adapter);

            m_adapter->GetMockDWriteFactory()->RegisterFontCollectionLoaderMethod.AllowAnyCall();

#if WINVER > _WIN32_WINNT_WINBLUE
            m_mockDWriteFontSet = Make<MockDWriteFontSet>();
#endif
        }

        ComPtr<DWriteFontSetType> ExpectCreateCustomFontCollection(std::wstring expectedFilename)
        {
            auto collection = Make<MockDWriteFontCollection>();
            auto fontFile = Make<MockDWriteFontFile>();

            m_adapter->GetMockDWriteFactory()->CreateCustomFontCollectionMethod.SetExpectedCalls(1,
                [=] (IDWriteFontCollectionLoader* loader, void const* key, uint32_t keySize, IDWriteFontCollection** outCollection)
                {
                    std::wstring actualFilename(static_cast<wchar_t const*>(key), keySize / 2);
                    Assert::AreEqual(expectedFilename, actualFilename);

                    //
                    // DWriteFactory will call methods on the loader to try
                    // and load the one font file.  We imitate that behavior here.
                    //

                    ComPtr<IDWriteFontFileEnumerator> enumerator;
                    ThrowIfFailed(loader->CreateEnumeratorFromKey(
                        m_adapter->GetMockDWriteFactory().Get(),
                        key,
                        keySize,
                        &enumerator));

                    BOOL hasFile = FALSE;
                    ThrowIfFailed(enumerator->MoveNext(&hasFile));

                    ComPtr<IDWriteFontFile> currentFile;
                    ThrowIfFailed(enumerator->GetCurrentFontFile(&currentFile));

                    ThrowIfFailed(enumerator->MoveNext(&hasFile));

                    return collection.CopyTo(outCollection);
                });

            m_adapter->GetMockDWriteFactory()->CreateFontFileReferenceMethod.SetExpectedCalls(1,
                [=] (wchar_t const* filePath, FILETIME const* lastWriteTime, IDWriteFontFile** outFontFile)
                {
                    Assert::AreEqual(expectedFilename.c_str(), filePath);
                    Assert::IsNull(lastWriteTime);
                    return fontFile.CopyTo(outFontFile);
                });

#if WINVER > _WIN32_WINNT_WINBLUE
            collection->GetFontSetMethod.SetExpectedCalls(1,
                [&](IDWriteFontSet** fontSet)
                {
                    ThrowIfFailed(m_mockDWriteFontSet.CopyTo(fontSet));
                    return S_OK;
                });
            return m_mockDWriteFontSet;
#else
            return collection;
#endif
        }
    };

    TEST_METHOD_EX(CanvasFontSet_CreateFromUri)
    {
        CustomFontFixture f;

        WinString somePath(L"ms-appx:///any_uri");

        auto expectedDWriteResource = f.ExpectCreateCustomFontCollection(StubStorageFileStatics::GetFakePath(somePath));

        auto factory = Make<CanvasFontSetFactory>();
        ComPtr<ICanvasFontSet> fontSet;

        auto uri = Make<StubUri>(somePath);
        Assert::AreEqual(S_OK, factory->Create(uri.Get(), &fontSet));

        auto dwriteResource = GetWrappedResource<DWriteFontSetType>(fontSet.Get());
        Assert::IsTrue(IsSameInstance(expectedDWriteResource.Get(), dwriteResource.Get()));
    }

    TEST_METHOD_EX(CanvasFontSet_CreateFromUri_InvalidArg)
    {
        auto factory = Make<CanvasFontSetFactory>();
        ComPtr<ICanvasFontSet> fontSet;

        Assert::AreEqual(E_INVALIDARG, factory->Create(nullptr, &fontSet));
    }

    TEST_METHOD_EX(CanvasFontSet_CreateFromUri_WhenGetFileFromApplicationUriFails_HelpfulErrorMessageIsThrown)
    {
        auto adapter = std::make_shared<StubCanvasTextLayoutAdapter>();

        CustomFontManagerAdapter::SetInstance(adapter);

        adapter->StorageFileStatics->GetFileFromApplicationUriAsyncMethod.SetExpectedCalls(1,
            [](IUriRuntimeClass*, IAsyncOperation<StorageFile*>**)
            {
                // Pretend that this was a valid-constructed URI, but it's not an application URI.
                return E_INVALIDARG;
            });

        auto factory = Make<CanvasFontSetFactory>();
        ComPtr<ICanvasFontSet> fontSet;
        auto uri = Make<StubUri>(nullptr);
        Assert::AreEqual(E_INVALIDARG, factory->Create(uri.Get(), &fontSet));
        ValidateStoredErrorState(E_INVALIDARG, Strings::InvalidFontFamilyUri);
    }
};
