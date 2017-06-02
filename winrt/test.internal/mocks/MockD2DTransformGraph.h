// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DTransformGraph : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ID2D1TransformGraph>
    {
    public:
        MOCK_METHOD0_CONST(GetInputCount, UINT32());
        MOCK_METHOD1(SetSingleTransformNode, HRESULT(ID2D1TransformNode*));
        MOCK_METHOD1(AddNode, HRESULT(ID2D1TransformNode*));
        MOCK_METHOD1(RemoveNode, HRESULT(ID2D1TransformNode*));
        MOCK_METHOD1(SetOutputNode, HRESULT(ID2D1TransformNode*));
        MOCK_METHOD3(ConnectNode, HRESULT(ID2D1TransformNode*, ID2D1TransformNode*, UINT32));
        MOCK_METHOD3(ConnectToEffectInput, HRESULT(UINT32, ID2D1TransformNode*, UINT32));
        MOCK_METHOD0(Clear, void());
        MOCK_METHOD1(SetPassthroughGraph, HRESULT(UINT32));

    };
}
