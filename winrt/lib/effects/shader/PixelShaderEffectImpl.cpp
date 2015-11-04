// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "PixelShaderEffectImpl.h"
#include "PixelShaderTransform.h"
#include "ClipTransform.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PixelShaderEffectImpl::PixelShaderEffectImpl()
        : m_constantsDirty(false)
        , m_coordinateMapping(std::make_shared<CoordinateMappingState>())
        , m_sourceInterpolation(std::make_unique<SourceInterpolationState>())
        , m_sourceInterpolationDirty(false)
        , m_graphDirty(true)
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
            <Property name='CoordinateMapping' type='blob'>
                <Property name='DisplayName' type='string' value='CoordinateMapping'/>
                <Property name='Default' type='blob' value=''/>
            </Property>
            <Property name='SourceInterpolation' type='blob'>
                <Property name='DisplayName' type='string' value='SourceInterpolation'/>
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
            D2D1_VALUE_TYPE_BINDING(L"SharedState",         &SetSharedStateProperty,         &GetSharedStateProperty),
            D2D1_BLOB_TYPE_BINDING (L"Constants",           &SetConstantsProperty,           &GetConstantsProperty),
            D2D1_BLOB_TYPE_BINDING (L"CoordinateMapping",   &SetCoordinateMappingProperty,   &GetCoordinateMappingProperty),
            D2D1_BLOB_TYPE_BINDING (L"SourceInterpolation", &SetSourceInterpolationProperty, &GetSourceInterpolationProperty),
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
            // One-time initialize.
            if (!m_shaderTransform)
            {
                PrepareForFirstDraw();
            }

            // Make sure our transform graph is up to date.
            if (m_graphDirty)
            {
                ConfigureTransformGraph();
                m_graphDirty = false;
            }

            // Update D2D with our latest shader constants.
            if (m_constantsDirty)
            {
                m_shaderTransform->SetConstants(m_constants);
                m_constantsDirty = false;
            }

            // Update D2D with our latest filter mode settings.
            if (m_sourceInterpolationDirty)
            {
                m_shaderTransform->SetSourceInterpolation(m_sourceInterpolation.get());
                m_sourceInterpolationDirty = false;
            }
        });
    }


    void PixelShaderEffectImpl::PrepareForFirstDraw()
    {
        // Shared state must be set before the effect can be drawn.
        if (!m_sharedState)
            ThrowHR(E_FAIL);

        // Load our pixel shader into D2D.
        auto& shader = m_sharedState->Shader();

        HRESULT hr = m_effectContext->LoadPixelShader(shader.Hash, shader.Code.data(), static_cast<UINT32>(shader.Code.size()));

        if (FAILED(hr))
            ThrowHR(hr, Strings::CustomEffectBadShader);

        // Create our shader and clip transform nodes.
        m_shaderTransform = Make<PixelShaderTransform>(m_sharedState.Get(), m_coordinateMapping);
        CheckMakeResult(m_shaderTransform);

        m_clipTransform = Make<ClipTransform>(m_sharedState.Get(), m_coordinateMapping);
        CheckMakeResult(m_clipTransform);
    }


    void PixelShaderEffectImpl::ConfigureTransformGraph()
    {
        m_transformGraph->Clear();

        // Add our pixel shader and clip nodes.
        auto shaderTransform = As<ID2D1TransformNode>(m_shaderTransform);
        auto clipTransform = As<ID2D1TransformNode>(m_clipTransform);

        ThrowIfFailed(m_transformGraph->AddNode(shaderTransform.Get()));
        ThrowIfFailed(m_transformGraph->AddNode(clipTransform.Get()));

        // Connect pixel shader -> clip node -> effect output.
        ThrowIfFailed(m_transformGraph->ConnectNode(shaderTransform.Get(), clipTransform.Get(), 0));
        ThrowIfFailed(m_transformGraph->SetOutputNode(clipTransform.Get()));

        for (unsigned i = 0; i < m_sharedState->Shader().InputCount; i++)
        {
            switch (m_coordinateMapping->BorderMode[i])
            {
                case EffectBorderMode::Soft:
                    {
                        // In soft border mode, connect effect inputs directly to the pixel shader.
                        ThrowIfFailed(m_transformGraph->ConnectToEffectInput(i, shaderTransform.Get(), i));
                    }
                    break;

                case EffectBorderMode::Hard:
                    {
                        // For hard borders, connect effect input -> border transform -> pixel shader.
                        ComPtr<ID2D1BorderTransform> borderTransform;
                        ThrowIfFailed(m_effectContext->CreateBorderTransform(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, &borderTransform));
                        ThrowIfFailed(m_transformGraph->AddNode(borderTransform.Get()));

                        ThrowIfFailed(m_transformGraph->ConnectToEffectInput(i, borderTransform.Get(), 0));
                        ThrowIfFailed(m_transformGraph->ConnectNode(borderTransform.Get(), shaderTransform.Get(), i));
                    }
                    break;

                default:
                    ThrowHR(E_INVALIDARG);
            }

            // Also connect the original effect inputs to the clip transform.
            // This lets it access their original extents before any border transforms were applied.
            ThrowIfFailed(m_transformGraph->ConnectToEffectInput(i, clipTransform.Get(), i + 1));
        }
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


    HRESULT PixelShaderEffectImpl::SetCoordinateMappingProperty(BYTE const* data, UINT32 dataSize)
    {
        if (dataSize != sizeof(CoordinateMappingState))
            return E_NOT_SUFFICIENT_BUFFER;
        
        auto newMapping = reinterpret_cast<CoordinateMappingState const*>(data);

        // If any border settings have changed, mark that the transform graph needs to be reconfigured.
        for (int i = 0; i < MaxShaderInputs; i++)
        {
            if (newMapping->BorderMode[i] != m_coordinateMapping->BorderMode[i])
            {
                m_graphDirty = true;
                break;
            }
        }

        *m_coordinateMapping = *newMapping;

        return S_OK;
    }


    HRESULT PixelShaderEffectImpl::GetCoordinateMappingProperty(BYTE* data, UINT32 dataSize, UINT32 *actualSize) const
    {
        if (actualSize)
            *actualSize = sizeof(CoordinateMappingState);

        if (data)
        {
            if (dataSize != sizeof(CoordinateMappingState))
                return E_NOT_SUFFICIENT_BUFFER;

            *reinterpret_cast<CoordinateMappingState*>(data) = *m_coordinateMapping;
        }

        return S_OK;
    }


    HRESULT PixelShaderEffectImpl::SetSourceInterpolationProperty(BYTE const* data, UINT32 dataSize)
    {
        if (dataSize != sizeof(SourceInterpolationState))
            return E_NOT_SUFFICIENT_BUFFER;

        *m_sourceInterpolation = *reinterpret_cast<SourceInterpolationState const*>(data);
        m_sourceInterpolationDirty = true;

        return S_OK;
    }


    HRESULT PixelShaderEffectImpl::GetSourceInterpolationProperty(BYTE* data, UINT32 dataSize, UINT32 *actualSize) const
    {
        if (actualSize)
            *actualSize = sizeof(SourceInterpolationState);

        if (data)
        {
            if (dataSize != sizeof(SourceInterpolationState))
                return E_NOT_SUFFICIENT_BUFFER;

            *reinterpret_cast<SourceInterpolationState*>(data) = *m_sourceInterpolation;
        }

        return S_OK;
    }

}}}}}
