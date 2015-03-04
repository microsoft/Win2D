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
