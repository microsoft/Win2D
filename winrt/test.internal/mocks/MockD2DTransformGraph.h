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
        CALL_COUNTER_WITH_MOCK(GetInputCountMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(SetSingleTransformNodeMethod, HRESULT(ID2D1TransformNode*));
        CALL_COUNTER_WITH_MOCK(AddNodeMethod, HRESULT(ID2D1TransformNode*));
        CALL_COUNTER_WITH_MOCK(RemoveNodeMethod, HRESULT(ID2D1TransformNode*));
        CALL_COUNTER_WITH_MOCK(SetOutputNodeMethod, HRESULT(ID2D1TransformNode*));
        CALL_COUNTER_WITH_MOCK(ConnectNodeMethod, HRESULT(ID2D1TransformNode*, ID2D1TransformNode*, UINT32));
        CALL_COUNTER_WITH_MOCK(ConnectToEffectInputMethod, HRESULT(UINT32, ID2D1TransformNode*, UINT32));
        CALL_COUNTER_WITH_MOCK(ClearMethod, void());
        CALL_COUNTER_WITH_MOCK(SetPassthroughGraphMethod, HRESULT(UINT32));

        //
        // ID2D1TransformGraph
        //

        STDMETHOD_(UINT32, GetInputCount)() const override
        {
            return GetInputCountMethod.WasCalled();
        }

        STDMETHOD(SetSingleTransformNode)(
            ID2D1TransformNode* node) override
        {
            return SetSingleTransformNodeMethod.WasCalled(node);
        }

        STDMETHOD(AddNode)(
            ID2D1TransformNode* node) override
        {
            return AddNodeMethod.WasCalled(node);
        }

        STDMETHOD(RemoveNode)(
            ID2D1TransformNode* node) override
        {
            return RemoveNodeMethod.WasCalled(node);
        }

        STDMETHOD(SetOutputNode)(
            ID2D1TransformNode* node) override
        {
            return SetOutputNodeMethod.WasCalled(node);
        }

        STDMETHOD(ConnectNode)(
            ID2D1TransformNode* fromNode,
            ID2D1TransformNode* toNode,
            UINT32 toNodeInputIndex) override
        {
            return ConnectNodeMethod.WasCalled(fromNode, toNode, toNodeInputIndex);
        }

        STDMETHOD(ConnectToEffectInput)(
            UINT32 toEffectInputIndex,
            ID2D1TransformNode* node,
            UINT32 toNodeInputIndex) override
        {
            return ConnectToEffectInputMethod.WasCalled(toEffectInputIndex, node, toNodeInputIndex);
        }

        STDMETHOD_(void, Clear)() override
        {
            ClearMethod.WasCalled();
        }

        STDMETHOD(SetPassthroughGraph)(
            UINT32 effectInputIndex) override
        {
            return SetPassthroughGraphMethod.WasCalled(effectInputIndex);
        }

    };
}
