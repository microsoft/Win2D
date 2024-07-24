// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFontCollection : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDWriteFontCollection1, IDWriteFontCollection>>
    {
    public: 
        CALL_COUNTER_WITH_MOCK(GetFontFamilyCountMethod, uint32_t());
        CALL_COUNTER_WITH_MOCK(GetFontFamilyMethod, HRESULT(uint32_t, IDWriteFontFamily**));
        CALL_COUNTER_WITH_MOCK(FindFamilyNameMethod, HRESULT(WCHAR const*, uint32_t*, BOOL*));
        CALL_COUNTER_WITH_MOCK(GetFontFromFontFaceMethod, HRESULT(IDWriteFontFace*, IDWriteFont**));

        IFACEMETHODIMP_(uint32_t) GetFontFamilyCount() override
        {
            return GetFontFamilyCountMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontFamily(
            uint32_t index,
            IDWriteFontFamily** fontFamily) override
        {
            return GetFontFamilyMethod.WasCalled(index, fontFamily);
        }

        IFACEMETHODIMP FindFamilyName(
            WCHAR const* familyName,
            uint32_t* index,
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

        MOCK_METHOD1(GetFontSet, HRESULT(IDWriteFontSet**));

        IFACEMETHODIMP GetFontFamily(uint32_t, IDWriteFontFamily1**) override
        {
            Assert::Fail(L"Unexpected call to GetFontSet");
            return E_NOTIMPL;
        }
    };
}
