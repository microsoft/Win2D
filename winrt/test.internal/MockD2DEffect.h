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
    class MockD2DEffect : public RuntimeClass <
        RuntimeClassFlags<ClassicCom>,
        ID2D1Image,
        ChainInterfaces < ID2D1Effect, ID2D1Properties >>
    {
    public:
        std::function<void(UINT32 index, ID2D1Image* input)> MockSetInput;
        std::function<HRESULT()> MockSetInputCount;
        std::function<HRESULT(UINT32 index, D2D1_PROPERTY_TYPE type, CONST BYTE* data, UINT32 dataSize)> MockSetValue;

        //
        // ID2D1Effect
        //

        STDMETHOD_(void, SetInput)(
            UINT32 index,
            _In_opt_ ID2D1Image *input,
            BOOL invalidate = TRUE
            )
        {
            if (!MockSetInput)
            {
                Assert::Fail(L"Unexpected call to SetInput");
                return;
            }
            MockSetInput(index, input);
        }

        STDMETHOD(SetInputCount)(
            UINT32 inputCount
            )
        {
            if (!MockSetInputCount)
            {
                Assert::Fail(L"Unexpected call to SetInputCount");
                return E_NOTIMPL;
            }
            return MockSetInputCount();
        }

        STDMETHOD_(void, GetInput)(
            UINT32 index,
            _Outptr_opt_ ID2D1Image **input
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetInput");
        }

            STDMETHOD_(UINT32, GetInputCount)(
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetInputCount");
            return 0;
        }

            STDMETHOD_(void, GetOutput)(
            _Outptr_ ID2D1Image **outputImage
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetOutput");
        }

        //
        // ID2D1Properties
        //

        STDMETHOD_(UINT32, GetPropertyCount)(
        ) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyCount");
            return 0;
        }

        STDMETHOD(GetPropertyName)(
            UINT32 index,
            _Out_writes_(nameCount) PWSTR name,
            UINT32 nameCount
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyName");
            return E_NOTIMPL;
        }

        STDMETHOD_(UINT32, GetPropertyNameLength)(
            UINT32 index
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyNameLength");
            return 0;
        }

        STDMETHOD_(D2D1_PROPERTY_TYPE, GetType)(
            UINT32 index
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetType");
            return D2D1_PROPERTY_TYPE_UNKNOWN;
        }

        STDMETHOD_(UINT32, GetPropertyIndex)(
            _In_ PCWSTR name
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetPropertyIndex");
            return 0;
        }

        STDMETHOD(SetValueByName)(
            _In_ PCWSTR name,
            D2D1_PROPERTY_TYPE type,
            _In_reads_(dataSize) CONST BYTE *data,
            UINT32 dataSize
            )
        {
            Assert::Fail(L"Unexpected call to SetValueByName");
            return E_NOTIMPL;
        }

        STDMETHOD(SetValue)(
            UINT32 index,
            D2D1_PROPERTY_TYPE type,
            _In_reads_(dataSize) CONST BYTE *data,
            UINT32 dataSize
            )
        {
            if (!MockSetValue)
            {
                Assert::Fail(L"Unexpected call to SetValue");
                return E_NOTIMPL;
            }
            return MockSetValue(index, type, data, dataSize);
        }

        STDMETHOD(GetValueByName)(
            _In_ PCWSTR name,
            D2D1_PROPERTY_TYPE type,
            _Out_writes_(dataSize) BYTE *data,
            UINT32 dataSize
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetValueByName");
            return E_NOTIMPL;
        }

        STDMETHOD(GetValue)(
            UINT32 index,
            D2D1_PROPERTY_TYPE type,
            _Out_writes_(dataSize) BYTE *data,
            UINT32 dataSize
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetValue");
            return E_NOTIMPL;
        }

        STDMETHOD_(UINT32, GetValueSize)(
            UINT32 index
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetValueSize");
            return 0;
        }

        STDMETHOD(GetSubProperties)(
            UINT32 index,
            _Outptr_opt_ ID2D1Properties **subProperties
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetSubProperties");
            return E_NOTIMPL;
        }

        //
        // ID2D1Resource
        //

        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **factory) const override
        {
            Assert::Fail(L"Unexpected call to GetFactory");
        }
    };


    class MockD2DEffectThatCountsCalls : public MockD2DEffect
    {
    public:
        IID m_effectId;

        int m_setInputCalls;
        int m_setValueCalls;

        std::vector<ComPtr<ID2D1Image>> m_inputs;
        std::vector<std::vector<byte>> m_properties;

        MockD2DEffectThatCountsCalls(IID const& effectId = CLSID_D2D1GaussianBlur)
          : m_effectId(effectId)
          , m_setInputCalls(0)
          , m_setValueCalls(0)
        {
            MockSetInputCount = []
            {
                return S_OK;
            };

            MockSetInput = [&](UINT32 index, ID2D1Image* input)
            {
                m_setInputCalls++;

                if (index >= m_inputs.size())
                    m_inputs.resize(index + 1);

                m_inputs[index] = input;
            };

            MockSetValue = [&](UINT32 index, D2D1_PROPERTY_TYPE type, CONST BYTE* data, UINT32 dataSize)
            {
                m_setValueCalls++;

                if (index >= m_properties.size())
                    m_properties.resize(index + 1);

                m_properties[index] = std::vector<byte>(data, data + dataSize);

                return S_OK;
            };
        }
    };
}