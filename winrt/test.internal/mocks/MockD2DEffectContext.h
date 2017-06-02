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
        MOCK_METHOD2_CONST(GetDpi, void(FLOAT*, FLOAT*));
        MOCK_METHOD2(CreateEffect, HRESULT(REFCLSID, ID2D1Effect**));
        MOCK_METHOD3_CONST(GetMaximumSupportedFeatureLevel, HRESULT(CONST D3D_FEATURE_LEVEL*, UINT32, D3D_FEATURE_LEVEL*));
        MOCK_METHOD2(CreateTransformNodeFromEffect, HRESULT(ID2D1Effect*, ID2D1TransformNode**));
        MOCK_METHOD3(CreateBlendTransform, HRESULT(UINT32, CONST D2D1_BLEND_DESCRIPTION*, ID2D1BlendTransform**));
        MOCK_METHOD3(CreateBorderTransform, HRESULT(D2D1_EXTEND_MODE, D2D1_EXTEND_MODE, ID2D1BorderTransform**));
        MOCK_METHOD2(CreateOffsetTransform, HRESULT(POINT, ID2D1OffsetTransform**));
        MOCK_METHOD2(CreateBoundsAdjustmentTransform, HRESULT(CONST RECT*, ID2D1BoundsAdjustmentTransform**));
        MOCK_METHOD3(LoadPixelShader, HRESULT(REFGUID, CONST BYTE*, UINT32));
        MOCK_METHOD3(LoadVertexShader, HRESULT(REFGUID, CONST BYTE*, UINT32));
        MOCK_METHOD3(LoadComputeShader, HRESULT(REFGUID, CONST BYTE*, UINT32));
        MOCK_METHOD1(IsShaderLoaded, BOOL(REFGUID));
        MOCK_METHOD6(CreateResourceTexture, HRESULT(CONST GUID*, CONST D2D1_RESOURCE_TEXTURE_PROPERTIES*, CONST BYTE*, CONST UINT32*, UINT32, ID2D1ResourceTexture**));
        MOCK_METHOD2(FindResourceTexture, HRESULT(CONST GUID*, ID2D1ResourceTexture**));
        MOCK_METHOD4(CreateVertexBuffer, HRESULT(CONST D2D1_VERTEX_BUFFER_PROPERTIES*, CONST GUID*, CONST D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES*, ID2D1VertexBuffer**));
        MOCK_METHOD2(FindVertexBuffer, HRESULT(CONST GUID*, ID2D1VertexBuffer**));
        MOCK_METHOD4(CreateColorContext, HRESULT(D2D1_COLOR_SPACE, CONST BYTE*, UINT32, ID2D1ColorContext**));
        MOCK_METHOD2(CreateColorContextFromFilename, HRESULT(PCWSTR, ID2D1ColorContext**));
        MOCK_METHOD2(CreateColorContextFromWicColorContext, HRESULT(IWICColorContext*, ID2D1ColorContext**));
        MOCK_METHOD3_CONST(CheckFeatureSupport, HRESULT(D2D1_FEATURE, void*, UINT32));
        MOCK_METHOD1_CONST(IsBufferPrecisionSupported, BOOL(D2D1_BUFFER_PRECISION));

    };
}
