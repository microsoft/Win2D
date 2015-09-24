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
        CALL_COUNTER_WITH_MOCK(SetPixelShaderConstantBufferMethod, HRESULT(CONST BYTE*, UINT32));
        CALL_COUNTER_WITH_MOCK(SetResourceTextureMethod, HRESULT(UINT32, ID2D1ResourceTexture*));
        CALL_COUNTER_WITH_MOCK(SetVertexShaderConstantBufferMethod, HRESULT(CONST BYTE*, UINT32));
        CALL_COUNTER_WITH_MOCK(SetPixelShaderMethod, HRESULT(REFGUID, D2D1_PIXEL_OPTIONS));
        CALL_COUNTER_WITH_MOCK(SetVertexProcessingMethod, HRESULT(ID2D1VertexBuffer*, D2D1_VERTEX_OPTIONS, CONST D2D1_BLEND_DESCRIPTION*, CONST D2D1_VERTEX_RANGE*, CONST GUID*));

        CALL_COUNTER_WITH_MOCK(SetInputDescriptionMethod, HRESULT(UINT32, D2D1_INPUT_DESCRIPTION));
        CALL_COUNTER_WITH_MOCK(SetOutputBufferMethod, HRESULT(D2D1_BUFFER_PRECISION, D2D1_CHANNEL_DEPTH));
        CALL_COUNTER_WITH_MOCK(SetCachedMethod, void(BOOL));
        CALL_COUNTER_WITH_MOCK(SetInstructionCountHintMethod, void(UINT32));

        //
        // ID2D1DrawInfo
        //

        STDMETHOD(SetPixelShaderConstantBuffer)(
            CONST BYTE* buffer,
            UINT32 bufferCount) override
        {
            return SetPixelShaderConstantBufferMethod.WasCalled(buffer, bufferCount);
        }

        STDMETHOD(SetResourceTexture)(
            UINT32 textureIndex,
            ID2D1ResourceTexture* resourceTexture) override
        {
            return SetResourceTextureMethod.WasCalled(textureIndex, resourceTexture);
        }

        STDMETHOD(SetVertexShaderConstantBuffer)(
            CONST BYTE* buffer,
            UINT32 bufferCount) override
        {
            return SetVertexShaderConstantBufferMethod.WasCalled(buffer, bufferCount);
        }

        STDMETHOD(SetPixelShader)(
            REFGUID shaderId,
            D2D1_PIXEL_OPTIONS pixelOptions) override
        {
            return SetPixelShaderMethod.WasCalled(shaderId, pixelOptions);
        }

        STDMETHOD(SetVertexProcessing)(
            ID2D1VertexBuffer* vertexBuffer,
            D2D1_VERTEX_OPTIONS vertexOptions,
            CONST D2D1_BLEND_DESCRIPTION* blendDescription,
            CONST D2D1_VERTEX_RANGE* vertexRange,
            CONST GUID* vertexShader) override
        {
            return SetVertexProcessingMethod.WasCalled(vertexBuffer, vertexOptions, blendDescription, vertexRange, vertexShader);
        }

        //
        // ID2D1RenderInfo
        //

        STDMETHOD(SetInputDescription)(
            UINT32 inputIndex,
            D2D1_INPUT_DESCRIPTION inputDescription) override
        {
            return SetInputDescriptionMethod.WasCalled(inputIndex, inputDescription);
        }

        STDMETHOD(SetOutputBuffer)(
            D2D1_BUFFER_PRECISION bufferPrecision,
            D2D1_CHANNEL_DEPTH channelDepth) override
        {
            return SetOutputBufferMethod.WasCalled(bufferPrecision, channelDepth);
        }

        STDMETHOD_(void, SetCached)(
            BOOL isCached) override
        {
            SetCachedMethod.WasCalled(isCached);
        }

        STDMETHOD_(void, SetInstructionCountHint)(
            UINT32 instructionCount) override
        {
            SetInstructionCountHintMethod.WasCalled(instructionCount);
        }

    };
}
