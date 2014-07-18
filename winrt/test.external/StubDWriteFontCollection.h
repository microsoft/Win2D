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

class StubDWriteFontCollection : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteFontCollection>
{
public:
    IFACEMETHODIMP_(uint32_t) GetFontFamilyCount() override
    {
        return 0;
    }

    IFACEMETHODIMP GetFontFamily(uint32_t, IDWriteFontFamily**) override
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP FindFamilyName(const wchar_t*, uint32_t*, BOOL*) override
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetFontFromFontFace(IDWriteFontFace* ,IDWriteFont**) override
    {
        return E_NOTIMPL;
    }
};
