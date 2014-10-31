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
    class MockD3D11Device : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ID3D11Device, 
        ChainInterfaces<IDXGIDevice3, IDXGIDevice2, IDXGIDevice1, IDXGIDevice> >
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetDeviceRemovedReasonMethod, HRESULT());

        MockD3D11Device()
        {
            GetDeviceRemovedReasonMethod.AllowAnyCall();
        }

        HRESULT STDMETHODCALLTYPE CreateBuffer(
            _In_  const D3D11_BUFFER_DESC *pDesc,
            _In_opt_  const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_  ID3D11Buffer **ppBuffer)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateTexture1D(
            _In_  const D3D11_TEXTURE1D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_  ID3D11Texture1D **ppTexture1D)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateTexture2D(
            _In_  const D3D11_TEXTURE2D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_  ID3D11Texture2D **ppTexture2D)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateTexture3D(
            _In_  const D3D11_TEXTURE3D_DESC *pDesc,
            _In_reads_opt_(_Inexpressible_(pDesc->MipLevels))  const D3D11_SUBRESOURCE_DATA *pInitialData,
            _Out_opt_  ID3D11Texture3D **ppTexture3D)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateShaderResourceView(
            _In_  ID3D11Resource *pResource,
            _In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
            _Out_opt_  ID3D11ShaderResourceView **ppSRView)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(
            _In_  ID3D11Resource *pResource,
            _In_opt_  const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
            _Out_opt_  ID3D11UnorderedAccessView **ppUAView)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateRenderTargetView(
            _In_  ID3D11Resource *pResource,
            _In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
            _Out_opt_  ID3D11RenderTargetView **ppRTView)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateDepthStencilView(
            _In_  ID3D11Resource *pResource,
            _In_opt_  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
            _Out_opt_  ID3D11DepthStencilView **ppDepthStencilView)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateInputLayout(
            _In_reads_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
            _In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,
            _In_reads_(BytecodeLength)  const void *pShaderBytecodeWithInputSignature,
            _In_  SIZE_T BytecodeLength,
            _Out_opt_  ID3D11InputLayout **ppInputLayout)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateVertexShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11VertexShader **ppVertexShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateGeometryShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11GeometryShader **ppGeometryShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_reads_opt_(NumEntries)  const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
            _In_range_(0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT)  UINT NumEntries,
            _In_reads_opt_(NumStrides)  const UINT *pBufferStrides,
            _In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumStrides,
            _In_  UINT RasterizedStream,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11GeometryShader **ppGeometryShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreatePixelShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11PixelShader **ppPixelShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateHullShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11HullShader **ppHullShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateDomainShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11DomainShader **ppDomainShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateComputeShader(
            _In_reads_(BytecodeLength)  const void *pShaderBytecode,
            _In_  SIZE_T BytecodeLength,
            _In_opt_  ID3D11ClassLinkage *pClassLinkage,
            _Out_opt_  ID3D11ComputeShader **ppComputeShader)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateClassLinkage(
            _Out_  ID3D11ClassLinkage **ppLinkage)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateBlendState(
            _In_  const D3D11_BLEND_DESC *pBlendStateDesc,
            _Out_opt_  ID3D11BlendState **ppBlendState)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateDepthStencilState(
            _In_  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
            _Out_opt_  ID3D11DepthStencilState **ppDepthStencilState)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateRasterizerState(
            _In_  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
            _Out_opt_  ID3D11RasterizerState **ppRasterizerState)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateSamplerState(
            _In_  const D3D11_SAMPLER_DESC *pSamplerDesc,
            _Out_opt_  ID3D11SamplerState **ppSamplerState)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateQuery(
            _In_  const D3D11_QUERY_DESC *pQueryDesc,
            _Out_opt_  ID3D11Query **ppQuery)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreatePredicate(
            _In_  const D3D11_QUERY_DESC *pPredicateDesc,
            _Out_opt_  ID3D11Predicate **ppPredicate)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateCounter(
            _In_  const D3D11_COUNTER_DESC *pCounterDesc,
            _Out_opt_  ID3D11Counter **ppCounter)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateDeferredContext(
            UINT ContextFlags,
            _Out_opt_  ID3D11DeviceContext **ppDeferredContext)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE OpenSharedResource(
            _In_  HANDLE hResource,
            _In_  REFIID ReturnedInterface,
            _Out_opt_  void **ppResource)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CheckFormatSupport(
            _In_  DXGI_FORMAT Format,
            _Out_  UINT *pFormatSupport)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(
            _In_  DXGI_FORMAT Format,
            _In_  UINT SampleCount,
            _Out_  UINT *pNumQualityLevels)
        {
            return E_NOTIMPL;
        }

        void STDMETHODCALLTYPE CheckCounterInfo(
            _Out_  D3D11_COUNTER_INFO *pCounterInfo)
        {
        }

        HRESULT STDMETHODCALLTYPE CheckCounter(
            _In_  const D3D11_COUNTER_DESC *pDesc,
            _Out_  D3D11_COUNTER_TYPE *pType,
            _Out_  UINT *pActiveCounters,
            _Out_writes_opt_(*pNameLength)  LPSTR szName,
            _Inout_opt_  UINT *pNameLength,
            _Out_writes_opt_(*pUnitsLength)  LPSTR szUnits,
            _Inout_opt_  UINT *pUnitsLength,
            _Out_writes_opt_(*pDescriptionLength)  LPSTR szDescription,
            _Inout_opt_  UINT *pDescriptionLength)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CheckFeatureSupport(
            D3D11_FEATURE Feature,
            _Out_writes_bytes_(FeatureSupportDataSize)  void *pFeatureSupportData,
            UINT FeatureSupportDataSize)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE GetPrivateData(
            _In_  REFGUID guid,
            _Inout_  UINT *pDataSize,
            _Out_writes_bytes_opt_(*pDataSize)  void *pData)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE SetPrivateData(
            _In_  REFGUID guid,
            _In_  UINT DataSize,
            _In_reads_bytes_opt_(DataSize)  const void *pData)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(
            _In_  REFGUID guid,
            _In_opt_  const IUnknown *pData)
        {
            return E_NOTIMPL;
        }

        D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel(void)
        {
            return static_cast<D3D_FEATURE_LEVEL>(-1);
        }

        UINT STDMETHODCALLTYPE GetCreationFlags(void)
        {
            return 0;
        }

        HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason()
        {
            return GetDeviceRemovedReasonMethod.WasCalled();
        }

        void STDMETHODCALLTYPE GetImmediateContext(
            _Out_  ID3D11DeviceContext **ppImmediateContext)
        {
        }

        HRESULT STDMETHODCALLTYPE SetExceptionMode(
            UINT RaiseFlags)
        {
            return E_NOTIMPL;
        }

        UINT STDMETHODCALLTYPE GetExceptionMode(void)
        {
            return 0;
        }

        // Below are DXGI functions.

        HRESULT STDMETHODCALLTYPE GetAdapter(
            _Out_  IDXGIAdapter **pAdapter)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE CreateSurface(
            _In_  const DXGI_SURFACE_DESC *pDesc,
            UINT NumSurfaces,
            DXGI_USAGE Usage,
            _In_opt_  const DXGI_SHARED_RESOURCE *pSharedResource,
            _Out_  IDXGISurface **ppSurface)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE QueryResourceResidency(
            _In_reads_(NumResources)  IUnknown *const *ppResources,
            _Out_writes_(NumResources)  DXGI_RESIDENCY *pResidencyStatus,
            UINT NumResources)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(
            INT Priority)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(
            _Out_ INT *pPriority)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE GetParent(
            _In_  REFIID riid,
            _Out_  void **ppParent)
        {
            return E_NOTIMPL;
        }

        // IDXGIDevice1
        HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(
            /* [in] */ UINT MaxLatency)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
            /* [annotation][out] */
            _Out_  UINT *pMaxLatency)
        {
            return E_NOTIMPL;
        }

        // IDXGIDevice2
        HRESULT STDMETHODCALLTYPE OfferResources(
            /* [annotation][in] */
            _In_  UINT NumResources,
            /* [annotation][size_is][in] */
            _In_reads_(NumResources)  IDXGIResource *const *ppResources,
            /* [annotation][in] */
            _In_  DXGI_OFFER_RESOURCE_PRIORITY Priority)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE ReclaimResources(
            /* [annotation][in] */
            _In_  UINT NumResources,
            /* [annotation][size_is][in] */
            _In_reads_(NumResources)  IDXGIResource *const *ppResources,
            /* [annotation][size_is][out] */
            _Out_writes_all_opt_(NumResources)  BOOL *pDiscarded)
        {
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE EnqueueSetEvent(
            /* [annotation][in] */
            _In_  HANDLE hEvent)
        {
            return E_NOTIMPL;
        }

        // IDXGIDevice3
        virtual void STDMETHODCALLTYPE Trim(void)
        {
        }

    };
}
