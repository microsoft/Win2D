// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFont
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>,
           IDWriteFont>
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
    };
}