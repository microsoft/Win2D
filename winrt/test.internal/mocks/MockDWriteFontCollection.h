// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFontCollection : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteFontCollection>
    {
    public: 
        CALL_COUNTER_WITH_MOCK(GetFontFamilyCountMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetFontFamilyMethod, HRESULT(UINT32, IDWriteFontFamily**));
        CALL_COUNTER_WITH_MOCK(FindFamilyNameMethod, HRESULT(WCHAR const*, UINT32*, BOOL*));
        CALL_COUNTER_WITH_MOCK(GetFontFromFontFaceMethod, HRESULT(IDWriteFontFace*, IDWriteFont**));

        IFACEMETHODIMP_(UINT32) GetFontFamilyCount() override
        {
            return GetFontFamilyCountMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontFamily(
            UINT32 index,
            IDWriteFontFamily** fontFamily) override
        {
            return GetFontFamilyMethod.WasCalled(index, fontFamily);
        }

        IFACEMETHODIMP FindFamilyName(
            WCHAR const* familyName,
            UINT32* index,
            BOOL* exists) override
        {
            return FindFamilyNameMethod.WasCalled(familyName, index, exists);
        }

        IFACEMETHODIMP GetFontFromFontFace(
            IDWriteFontFace* fontFace,
            IDWriteFont** font) override
        {
            return GetFontFromFontFaceMethod.WasCalled(fontFace, font);
        }
    };
}
