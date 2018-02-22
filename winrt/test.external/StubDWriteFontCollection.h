// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
