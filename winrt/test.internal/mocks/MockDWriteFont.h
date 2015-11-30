// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFont
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>,
           ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
               IDWriteFont3,
#endif
               IDWriteFont2,
               IDWriteFont1,
               IDWriteFont>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetFontFamilyMethod, HRESULT(IDWriteFontFamily**));
        CALL_COUNTER_WITH_MOCK(GetWeightMethod, DWRITE_FONT_WEIGHT());
        CALL_COUNTER_WITH_MOCK(GetStretchMethod, DWRITE_FONT_STRETCH());
        CALL_COUNTER_WITH_MOCK(GetStyleMethod, DWRITE_FONT_STYLE());
        CALL_COUNTER_WITH_MOCK(IsSymbolFontMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetFaceNamesMethod, HRESULT(IDWriteLocalizedStrings**));
        CALL_COUNTER_WITH_MOCK(GetInformationalStringsMethod, HRESULT(DWRITE_INFORMATIONAL_STRING_ID, IDWriteLocalizedStrings**, BOOL*));
        CALL_COUNTER_WITH_MOCK(GetSimulationsMethod, DWRITE_FONT_SIMULATIONS());
        CALL_COUNTER_WITH_MOCK(GetMetricsMethod, void(DWRITE_FONT_METRICS*));
        CALL_COUNTER_WITH_MOCK(HasCharacterMethod, HRESULT(uint32_t, BOOL*));
        CALL_COUNTER_WITH_MOCK(CreateFontFaceMethod, HRESULT(IDWriteFontFace**));
        CALL_COUNTER_WITH_MOCK(GetPanoseMethod, void(DWRITE_PANOSE*));
#if WINVER > _WIN32_WINNT_WINBLUE
        CALL_COUNTER_WITH_MOCK(GetFontFaceReferenceMethod, HRESULT(IDWriteFontFaceReference** fontFaceReference));
        CALL_COUNTER_WITH_MOCK(CreateFontFaceMethod1, HRESULT(IDWriteFontFace3**));
#endif
        
        IFACEMETHODIMP GetFontFamily(
            IDWriteFontFamily** fontFamily)
        {
            return GetFontFamilyMethod.WasCalled(fontFamily);
        }

        IFACEMETHODIMP_(DWRITE_FONT_WEIGHT) GetWeight()
        {
            return GetWeightMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STRETCH) GetStretch()
        {
            return GetStretchMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STYLE) GetStyle()
        {
            return GetStyleMethod.WasCalled();
        }

        IFACEMETHODIMP_(BOOL) IsSymbolFont()
        {
            return IsSymbolFontMethod.WasCalled();
        }

        IFACEMETHODIMP GetFaceNames(
            IDWriteLocalizedStrings** names)
        {
            return GetFaceNamesMethod.WasCalled(names);
        }

        IFACEMETHODIMP GetInformationalStrings(
            DWRITE_INFORMATIONAL_STRING_ID informationalStringID,
            IDWriteLocalizedStrings** informationalStrings,
            BOOL* exists)
        {
            return GetInformationalStringsMethod.WasCalled(informationalStringID, informationalStrings, exists);
        }

        IFACEMETHODIMP_(DWRITE_FONT_SIMULATIONS) GetSimulations()
        {
            return GetSimulationsMethod.WasCalled();
        }

        IFACEMETHODIMP_(void) GetMetrics(
            DWRITE_FONT_METRICS* fontMetrics)
        {
            return GetMetricsMethod.WasCalled(fontMetrics);
        }

        IFACEMETHODIMP HasCharacter(
            uint32_t unicodeValue,
            BOOL* exists)
        {
            return HasCharacterMethod.WasCalled(unicodeValue, exists);
        }

        IFACEMETHODIMP CreateFontFace(
            IDWriteFontFace** fontFace)
        {
            return CreateFontFaceMethod.WasCalled(fontFace);
        }

        IFACEMETHODIMP_(void) GetMetrics(DWRITE_FONT_METRICS1* fontMetrics)
        {
            Assert::Fail(L"Unexpected call to GetMetrics");
        }

        IFACEMETHODIMP_(void) GetPanose(DWRITE_PANOSE* panose)
        {
            GetPanoseMethod.WasCalled(panose);
        }

        IFACEMETHODIMP GetUnicodeRanges(
            UINT32 maxRangeCount,
            DWRITE_UNICODE_RANGE* unicodeRanges,
            UINT32* actualRangeCount)
        {
            Assert::Fail(L"Unexpected call to GetUnicodeRanges");
            return E_NOTIMPL;
        }


        IFACEMETHODIMP_(BOOL) IsMonospacedFont()
        {
            Assert::Fail(L"Unexpected call to IsMonospacedFont");
            return FALSE;
        }

        IFACEMETHODIMP_(BOOL) IsColorFont()
        {
            Assert::Fail(L"Unexpected call to IsColorFont");
            return FALSE;
        }

#if WINVER > _WIN32_WINNT_WINBLUE

        IFACEMETHODIMP CreateFontFace(IDWriteFontFace3** fontFace)
        {
            return CreateFontFaceMethod1.WasCalled(fontFace);
        }

        IFACEMETHODIMP_(BOOL) Equals(IDWriteFont* font)
        {
            Assert::Fail(L"Unexpected call to Equals");
            return FALSE;
        }

        IFACEMETHODIMP GetFontFaceReference(IDWriteFontFaceReference** fontFaceReference)
        {
            return GetFontFaceReferenceMethod.WasCalled(fontFaceReference);
        }

        IFACEMETHODIMP_(BOOL) HasCharacter(UINT32 unicodeValue)
        {
            Assert::Fail(L"Unexpected call to HasCharacter");
            return FALSE;
        }

        IFACEMETHODIMP_(DWRITE_LOCALITY) GetLocality()
        {
            Assert::Fail(L"Unexpected call to GetLocality");
            return DWRITE_LOCALITY_LOCAL;
        }
#endif

    };
}