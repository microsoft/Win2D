// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "PixelShaderEffectImpl.h"
#include "PixelShaderTransform.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PixelShaderEffectImpl::PixelShaderEffectImpl()
        : m_constantsDirty(false)
    { }


#define XML(xml) TEXT(#xml)

    const wchar_t effectXml[] = XML
    (
        <?xml version='1.0'?>
        <Effect>
            <Property name='DisplayName' type='string' value='PixelShaderEffect'/>
            <Property name='Author'      type='string' value='Microsoft Corporation'/>
            <Property name='Category'    type='string' value='Win2D'/>
            <Property name='Description' type='string' value='Applies a custom pixel shader.'/>
            <Inputs minimum = '0' maximum = '8'>
                <Input name='Source1'/>
                <Input name='Source2'/>
                <Input name='Source3'/>
                <Input name='Source4'/>
                <Input name='Source5'/>
                <Input name='Source6'/>
                <Input name='Source7'/>
                <Input name='Source8'/>
            </Inputs>
            <Property name='SharedState' type='iunknown'>
                <Property name='DisplayName' type='string' value='SharedState'/>
                <Property name='Default' type='iunknown' value='null'/>
            </Property>
            <Property name='Constants' type='blob'>
                <Property name='DisplayName' type='string' value='Constants'/>
                <Property name='Default' type='blob' value=''/>
            </Property>
        </Effect>
    );


    static HRESULT STDMETHODCALLTYPE PixelShaderEffectImplFactory(IUnknown** result)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<PixelShaderEffectImpl>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(result));
        });
    }


    static bool IsEffectRegistered(ID2D1Factory1* factory, IID const& effectId)
    {
        // Size query.
        UINT32 returnedCount, registeredCount;

        ThrowIfFailed(factory->GetRegisteredEffects(nullptr, 0, &returnedCount, &registeredCount));

        // Read the data.
        std::vector<IID> ids(registeredCount);

        ThrowIfFailed(factory->GetRegisteredEffects(ids.data(), registeredCount, &returnedCount, &registeredCount));

        return std::find(ids.begin(), ids.end(), effectId) != ids.end();
    }


    void PixelShaderEffectImpl::Register(ID2D1Factory1* factory)
    {
        if (IsEffectRegistered(factory, CLSID_PixelShaderEffect))
            return;

        static const D2D1_PROPERTY_BINDING bindings[] =
        {
            D2D1_VALUE_TYPE_BINDING(L"SharedState", &SetSharedStateProperty, &GetSharedStateProperty),
            D2D1_BLOB_TYPE_BINDING (L"Constants",   &SetConstantsProperty,   &GetConstantsProperty)
        };

        ThrowIfFailed(factory->RegisterEffectFromString(CLSID_PixelShaderEffect, effectXml, bindings, _countof(bindings), PixelShaderEffectImplFactory));
    }


    IFACEMETHODIMP PixelShaderEffectImpl::Initialize(ID2D1EffectContext* effectContext, ID2D1TransformGraph* transformGraph)
    {
        m_effectContext = effectContext;
        m_transformGraph = transformGraph;

        return S_OK;
    }


    IFACEMETHODIMP PixelShaderEffectImpl::SetGraph(ID2D1TransformGraph* transformGraph)
    {
        // Our input count is write-once - we don't support changing the graph after it has been configured.
        if (m_shaderTransform)
            return E_FAIL;

        m_transformGraph = transformGraph;

        return S_OK;
    }


    IFACEMETHODIMP PixelShaderEffectImpl::PrepareForRender(D2D1_CHANGE_TYPE)
    {
        return ExceptionBoundary([&]
        {
            if (!m_shaderTransform)
            {
                // Shared state must be set before the effect can be drawn.
                if (!m_sharedState)
                    ThrowHR(E_FAIL);

                // Load our pixel shader into D2D.
                auto& shader = m_sharedState->Shader();

                HRESULT hr = m_effectContext->LoadPixelShader(shader.Hash, shader.Code.data(), static_cast<UINT32>(shader.Code.size()));

                if (FAILED(hr))
                    ThrowHR(hr, Strings::CustomEffectBadShader);

                // Configure the effect transform graph.
                m_shaderTransform = Make<PixelShaderTransform>(m_sharedState.Get());
                CheckMakeResult(m_shaderTransform);

                ThrowIfFailed(m_transformGraph->SetSingleTransformNode(As<ID2D1TransformNode>(m_shaderTransform).Get()));
            }

            // Update D2D with our latest shader constants.
            if (m_constantsDirty)
            {
                m_shaderTransform->SetConstants(m_constants);
                m_constantsDirty = false;
            }
        });
    }


    HRESULT PixelShaderEffectImpl::SetSharedStateProperty(IUnknown* sharedState)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(sharedState);

            // Shared state is a write-once property.
            if (m_sharedState)
                ThrowHR(E_FAIL);

            m_sharedState = As<ISharedShaderState>(sharedState);
        });
    }


    IUnknown* PixelShaderEffectImpl::GetSharedStateProperty() const
    {
        return m_sharedState ? As<IUnknown>(m_sharedState).Detach() : nullptr;
    }


    HRESULT PixelShaderEffectImpl::SetConstantsProperty(BYTE const* data, UINT32 dataSize)
    {
        return ExceptionBoundary([&]
        {
            m_constants.assign(data, data + dataSize);
            m_constantsDirty = true;
        });
    }


    HRESULT PixelShaderEffectImpl::GetConstantsProperty(BYTE* data, UINT32 dataSize, UINT32 *actualSize) const
    {
        if (actualSize)
            *actualSize = static_cast<UINT32>(m_constants.size());

        if (data)
            memcpy(data, m_constants.data(), std::min<size_t>(dataSize, m_constants.size()));

        return S_OK;
    }

}}}}}
