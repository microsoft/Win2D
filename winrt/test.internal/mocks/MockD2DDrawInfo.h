// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DDrawInfo : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1DrawInfo, ID2D1RenderInfo>>
    {
    public:
        MOCK_METHOD2(SetPixelShaderConstantBuffer, HRESULT(CONST BYTE*, UINT32));
        MOCK_METHOD2(SetResourceTexture, HRESULT(UINT32, ID2D1ResourceTexture*));
        MOCK_METHOD2(SetVertexShaderConstantBuffer, HRESULT(CONST BYTE*, UINT32));
        MOCK_METHOD2(SetPixelShader, HRESULT(REFGUID, D2D1_PIXEL_OPTIONS));
        MOCK_METHOD5(SetVertexProcessing, HRESULT(ID2D1VertexBuffer*, D2D1_VERTEX_OPTIONS, CONST D2D1_BLEND_DESCRIPTION*, CONST D2D1_VERTEX_RANGE*, CONST GUID*));

        MOCK_METHOD2(SetInputDescription, HRESULT(UINT32, D2D1_INPUT_DESCRIPTION));
        MOCK_METHOD2(SetOutputBuffer, HRESULT(D2D1_BUFFER_PRECISION, D2D1_CHANNEL_DEPTH));
        MOCK_METHOD1(SetCached, void(BOOL));
        MOCK_METHOD1(SetInstructionCountHint, void(UINT32));

    };
}
