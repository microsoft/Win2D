// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class StubD2DEffect : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1Effect, ID2D1Image>
{
    std::map<uint32_t, std::vector<uint8_t>> m_properties;
    std::vector<ComPtr<ID2D1Image>> m_inputs;

public:
    StubD2DEffect(IID const& iid)
    {
        std::vector<uint8_t> v;
        v.resize(sizeof(iid));
        memcpy(v.data(), &iid, sizeof(iid));

        m_properties[static_cast<uint32_t>(D2D1_PROPERTY_CLSID)] = v;
    }
    
    //
    // ID2D1Properties
    //

    IFACEMETHODIMP_(UINT32) GetPropertyCount() const
    {
        return static_cast<uint32_t>(m_properties.size());
    }

    IFACEMETHODIMP GetPropertyName(UINT32,PWSTR,UINT32) const
    {
        Assert::Fail(L"Unexpected call to GetPropertyName");
        return E_NOTIMPL;
    }

    IFACEMETHODIMP_(UINT32) GetPropertyNameLength(UINT32) const
    {
        Assert::Fail(L"Unexpected call to GetPropertyNameLength");
        return 0;
    }

    IFACEMETHODIMP_(D2D1_PROPERTY_TYPE) GetType(UINT32) const
    {
        Assert::Fail(L"Unexpected call to GetType");
        return (D2D1_PROPERTY_TYPE)0;
    }

    IFACEMETHODIMP_(UINT32) GetPropertyIndex(PCWSTR) const
    {
        Assert::Fail(L"Unexpected call to GetPropertyIndex");
        return 0;
    }

    IFACEMETHODIMP SetValueByName(PCWSTR,D2D1_PROPERTY_TYPE,const BYTE *,UINT32)
    {
        Assert::Fail(L"Unexpected call to SetValueByName");
        return E_NOTIMPL;
    }

    IFACEMETHODIMP SetValue(UINT32 index, D2D1_PROPERTY_TYPE, const BYTE* data, UINT32 dataSize)
    {
        std::vector<uint8_t> v;
        v.resize(dataSize);
        memcpy(v.data(), data, dataSize);

        m_properties[index] = v;

        return S_OK;
    }

    IFACEMETHODIMP GetValueByName(PCWSTR,D2D1_PROPERTY_TYPE,BYTE *,UINT32) const
    {
        Assert::Fail(L"Unexpected call to GetValueByName");
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetValue(UINT32 index, D2D1_PROPERTY_TYPE, BYTE* data, UINT32 dataSize) const
    {
        auto vit = m_properties.find(index);
        if (vit == m_properties.end())
            return D2DERR_INVALID_PROPERTY;

        auto& v = vit->second;
        if (v.size() != dataSize)
            return E_INVALIDARG;

        memcpy(data, v.data(), dataSize);

        return S_OK;
    }

    IFACEMETHODIMP_(UINT32) GetValueSize(UINT32) const
    {
        Assert::Fail(L"Unexpected call to GetValueSize");
        return 0;
    }

    IFACEMETHODIMP GetSubProperties(UINT32,ID2D1Properties **) const
    {
        Assert::Fail(L"Unexpected call to GetSubProperties");
        return E_NOTIMPL;
    }

    //
    // ID2D1Effect
    //

    IFACEMETHODIMP_(void) SetInput(UINT32 index, ID2D1Image* input, BOOL)
    {
        m_inputs.resize(index+1);
        m_inputs[index] = input;
    }

    IFACEMETHODIMP SetInputCount(UINT32 count)
    {
        m_inputs.resize(count);
        return S_OK;
    }

    IFACEMETHODIMP_(void) GetInput(UINT32 index, ID2D1Image** image) const
    {
        *image = nullptr;

        if (index < m_inputs.size())
            m_inputs[index].CopyTo(image);
    }

    IFACEMETHODIMP_(UINT32) GetInputCount() const
    {
        return static_cast<uint32_t>(m_inputs.size());
    }

    IFACEMETHODIMP_(void) GetOutput(ID2D1Image** image) const
    {
        ComPtr<ID2D1Image> i(const_cast<StubD2DEffect*>(this));
        i.CopyTo(image);
    }

    IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **) const
    {
        Assert::Fail(L"Unexpected call to ID2D1Resource");
    }
};
