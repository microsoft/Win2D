// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteStringList: public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteStringList>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetCountMethod, uint32_t());
        CALL_COUNTER_WITH_MOCK(GetLocaleNameLengthMethod, HRESULT(uint32_t, uint32_t*));
        CALL_COUNTER_WITH_MOCK(GetLocaleNameMethod, HRESULT(uint32_t, wchar_t*, uint32_t));
        CALL_COUNTER_WITH_MOCK(GetStringLengthMethod, HRESULT(uint32_t, uint32_t*));
        CALL_COUNTER_WITH_MOCK(GetStringMethod, HRESULT(uint32_t, wchar_t*, uint32_t));

        IFACEMETHODIMP_(uint32_t) GetCount()
        {
            return GetCountMethod.WasCalled();
        }

        STDMETHOD(GetLocaleNameLength)(
            UINT32 index,
            UINT32* length)
        {
            return GetLocaleNameLengthMethod.WasCalled(index, length);
        }

        STDMETHOD(GetLocaleName)(
            uint32_t index,
            WCHAR* localeName,
            uint32_t size)
        {
            return GetLocaleNameMethod.WasCalled(index, localeName, size);
        }

        STDMETHOD(GetStringLength)(
            UINT32 index,
            UINT32* length)
        {
            return GetStringLengthMethod.WasCalled(index, length);
        }

        STDMETHOD(GetString)(
            UINT32 index,
            WCHAR* stringBuffer,
            UINT32 size)
        {
            return GetStringMethod.WasCalled(index, stringBuffer, size);
        }

    };
}