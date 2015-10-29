// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DEffectContext : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ID2D1EffectContext>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetDpiMethod, void(FLOAT*, FLOAT*));
        CALL_COUNTER_WITH_MOCK(CreateEffectMethod, HRESULT(REFCLSID, ID2D1Effect**));
        CALL_COUNTER_WITH_MOCK(GetMaximumSupportedFeatureLevelMethod, HRESULT(CONST D3D_FEATURE_LEVEL*, UINT32, D3D_FEATURE_LEVEL*));
        CALL_COUNTER_WITH_MOCK(CreateTransformNodeFromEffectMethod, HRESULT(ID2D1Effect*, ID2D1TransformNode**));
        CALL_COUNTER_WITH_MOCK(CreateBlendTransformMethod, HRESULT(UINT32, CONST D2D1_BLEND_DESCRIPTION*, ID2D1BlendTransform**));
        CALL_COUNTER_WITH_MOCK(CreateBorderTransformMethod, HRESULT(D2D1_EXTEND_MODE, D2D1_EXTEND_MODE, ID2D1BorderTransform**));
        CALL_COUNTER_WITH_MOCK(CreateOffsetTransformMethod, HRESULT(POINT, ID2D1OffsetTransform**));
        CALL_COUNTER_WITH_MOCK(CreateBoundsAdjustmentTransformMethod, HRESULT(CONST RECT*, ID2D1BoundsAdjustmentTransform**));
        CALL_COUNTER_WITH_MOCK(LoadPixelShaderMethod, HRESULT(REFGUID, CONST BYTE*, UINT32));
        CALL_COUNTER_WITH_MOCK(LoadVertexShaderMethod, HRESULT(REFGUID, CONST BYTE*, UINT32));
        CALL_COUNTER_WITH_MOCK(LoadComputeShaderMethod, HRESULT(REFGUID, CONST BYTE*, UINT32));
        CALL_COUNTER_WITH_MOCK(IsShaderLoadedMethod, BOOL(REFGUID));
        CALL_COUNTER_WITH_MOCK(CreateResourceTextureMethod, HRESULT(CONST GUID*, CONST D2D1_RESOURCE_TEXTURE_PROPERTIES*, CONST BYTE*, CONST UINT32*, UINT32, ID2D1ResourceTexture**));
        CALL_COUNTER_WITH_MOCK(FindResourceTextureMethod, HRESULT(CONST GUID*, ID2D1ResourceTexture**));
        CALL_COUNTER_WITH_MOCK(CreateVertexBufferMethod, HRESULT(CONST D2D1_VERTEX_BUFFER_PROPERTIES*, CONST GUID*, CONST D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES*, ID2D1VertexBuffer**));
        CALL_COUNTER_WITH_MOCK(FindVertexBufferMethod, HRESULT(CONST GUID*, ID2D1VertexBuffer**));
        CALL_COUNTER_WITH_MOCK(CreateColorContextMethod, HRESULT(D2D1_COLOR_SPACE, CONST BYTE*, UINT32, ID2D1ColorContext**));
        CALL_COUNTER_WITH_MOCK(CreateColorContextFromFilenameMethod, HRESULT(PCWSTR, ID2D1ColorContext**));
        CALL_COUNTER_WITH_MOCK(CreateColorContextFromWicColorContextMethod, HRESULT(IWICColorContext*, ID2D1ColorContext**));
        CALL_COUNTER_WITH_MOCK(CheckFeatureSupportMethod, HRESULT(D2D1_FEATURE, void*, UINT32));
        CALL_COUNTER_WITH_MOCK(IsBufferPrecisionSupportedMethod, BOOL(D2D1_BUFFER_PRECISION));

        //
        // ID2D1EffectContext
        //

        STDMETHOD_(void, GetDpi)(
            FLOAT* dpiX,
            FLOAT* dpiY) const override
        {
            GetDpiMethod.WasCalled(dpiX, dpiY);
        }

        STDMETHOD(CreateEffect)(
            REFCLSID effectId,
            ID2D1Effect** effect) override
        {
            return CreateEffectMethod.WasCalled(effectId, effect);
        }

        STDMETHOD(GetMaximumSupportedFeatureLevel)(
            CONST D3D_FEATURE_LEVEL* featureLevels,
            UINT32 featureLevelsCount,
            D3D_FEATURE_LEVEL* maximumSupportedFeatureLevel) const override
        {
            return GetMaximumSupportedFeatureLevelMethod.WasCalled(featureLevels, featureLevelsCount, maximumSupportedFeatureLevel);
        }

        STDMETHOD(CreateTransformNodeFromEffect)(
            ID2D1Effect* effect,
            ID2D1TransformNode** transformNode) override
        {
            return CreateTransformNodeFromEffectMethod.WasCalled(effect, transformNode);
        }

        STDMETHOD(CreateBlendTransform)(
            UINT32 numInputs,
            CONST D2D1_BLEND_DESCRIPTION* blendDescription,
            ID2D1BlendTransform** transform) override
        {
            return CreateBlendTransformMethod.WasCalled(numInputs, blendDescription, transform);
        }

        STDMETHOD(CreateBorderTransform)(
            D2D1_EXTEND_MODE extendModeX,
            D2D1_EXTEND_MODE extendModeY,
            ID2D1BorderTransform** transform) override
        {
            return CreateBorderTransformMethod.WasCalled(extendModeX, extendModeY, transform);
        }

        STDMETHOD(CreateOffsetTransform)(
            POINT offset,
            ID2D1OffsetTransform** transform) override
        {
            return CreateOffsetTransformMethod.WasCalled(offset, transform);
        }

        STDMETHOD(CreateBoundsAdjustmentTransform)(
            CONST RECT* outputRectangle,
            ID2D1BoundsAdjustmentTransform** transform) override
        {
            return CreateBoundsAdjustmentTransformMethod.WasCalled(outputRectangle, transform);
        }

        STDMETHOD(LoadPixelShader)(
            REFGUID shaderId,
            CONST BYTE* shaderBuffer,
            UINT32 shaderBufferCount) override
        {
            return LoadPixelShaderMethod.WasCalled(shaderId, shaderBuffer, shaderBufferCount);
        }

        STDMETHOD(LoadVertexShader)(
            REFGUID resourceId,
            CONST BYTE* shaderBuffer,
            UINT32 shaderBufferCount) override
        {
            return LoadVertexShaderMethod.WasCalled(resourceId, shaderBuffer, shaderBufferCount);
        }

        STDMETHOD(LoadComputeShader)(
            REFGUID resourceId,
            CONST BYTE* shaderBuffer,
            UINT32 shaderBufferCount) override
        {
            return LoadComputeShaderMethod.WasCalled(resourceId, shaderBuffer, shaderBufferCount);
        }

        STDMETHOD_(BOOL, IsShaderLoaded)(
            REFGUID shaderId) override
        {
            return IsShaderLoadedMethod.WasCalled(shaderId);
        }

        STDMETHOD(CreateResourceTexture)(
            CONST GUID* resourceId,
            CONST D2D1_RESOURCE_TEXTURE_PROPERTIES* resourceTextureProperties,
            CONST BYTE* data,
            CONST UINT32* strides,
            UINT32 dataSize,
            ID2D1ResourceTexture** resourceTexture) override
        {
            return CreateResourceTextureMethod.WasCalled(resourceId, resourceTextureProperties, data, strides, dataSize, resourceTexture);
        }

        STDMETHOD(FindResourceTexture)(
            CONST GUID* resourceId,
            ID2D1ResourceTexture** resourceTexture) override
        {
            return FindResourceTextureMethod.WasCalled(resourceId, resourceTexture);
        }

        STDMETHOD(CreateVertexBuffer)(
            CONST D2D1_VERTEX_BUFFER_PROPERTIES* vertexBufferProperties,
            CONST GUID* resourceId,
            CONST D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES* customVertexBufferProperties,
            ID2D1VertexBuffer** buffer) override
        {
            return CreateVertexBufferMethod.WasCalled(vertexBufferProperties, resourceId, customVertexBufferProperties, buffer);
        }

        STDMETHOD(FindVertexBuffer)(
            CONST GUID* resourceId,
            ID2D1VertexBuffer** buffer) override
        {
            return FindVertexBufferMethod.WasCalled(resourceId, buffer);
        }

        STDMETHOD(CreateColorContext)(
            D2D1_COLOR_SPACE space,
            CONST BYTE* profile,
            UINT32 profileSize,
            ID2D1ColorContext** colorContext) override
        {
            return CreateColorContextMethod.WasCalled(space, profile, profileSize, colorContext);
        }

        STDMETHOD(CreateColorContextFromFilename)(
            PCWSTR filename,
            ID2D1ColorContext** colorContext) override
        {
            return CreateColorContextFromFilenameMethod.WasCalled(filename, colorContext);
        }

        STDMETHOD(CreateColorContextFromWicColorContext)(
            IWICColorContext* wicColorContext,
            ID2D1ColorContext** colorContext) override
        {
            return CreateColorContextFromWicColorContextMethod.WasCalled(wicColorContext, colorContext);
        }

        STDMETHOD(CheckFeatureSupport)(
            D2D1_FEATURE feature,
            void* featureSupportData,
            UINT32 featureSupportDataSize) const override
        {
            return CheckFeatureSupportMethod.WasCalled(feature, featureSupportData, featureSupportDataSize);
        }

        STDMETHOD_(BOOL, IsBufferPrecisionSupported)(
            D2D1_BUFFER_PRECISION bufferPrecision) const override
        {
            return IsBufferPrecisionSupportedMethod.WasCalled(bufferPrecision);
        }

    };
}
