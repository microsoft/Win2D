// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DBorderTransform : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1BorderTransform, ID2D1ConcreteTransform, ID2D1TransformNode>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetExtendModeXMethod, void(D2D1_EXTEND_MODE));
        CALL_COUNTER_WITH_MOCK(SetExtendModeYMethod, void(D2D1_EXTEND_MODE));
        CALL_COUNTER_WITH_MOCK(GetExtendModeXMethod, D2D1_EXTEND_MODE());
        CALL_COUNTER_WITH_MOCK(GetExtendModeYMethod, D2D1_EXTEND_MODE());

        CALL_COUNTER_WITH_MOCK(SetOutputBufferMethod, HRESULT(D2D1_BUFFER_PRECISION, D2D1_CHANNEL_DEPTH));
        CALL_COUNTER_WITH_MOCK(SetCachedMethod, void(BOOL));

        CALL_COUNTER_WITH_MOCK(GetInputCountMethod, UINT32());

        //
        // ID2D1BorderTransform
        //

        STDMETHOD_(void, SetExtendModeX)(
            D2D1_EXTEND_MODE extendMode) override
        {
            SetExtendModeXMethod.WasCalled(extendMode);
        }

        STDMETHOD_(void, SetExtendModeY)(
            D2D1_EXTEND_MODE extendMode) override
        {
            SetExtendModeYMethod.WasCalled(extendMode);
        }

        STDMETHOD_(D2D1_EXTEND_MODE, GetExtendModeX)() const override
        {
            return GetExtendModeXMethod.WasCalled();
        }

        STDMETHOD_(D2D1_EXTEND_MODE, GetExtendModeY)() const override
        {
            return GetExtendModeYMethod.WasCalled();
        }

        //
        // ID2D1ConcreteTransform
        //

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

        //
        // ID2D1TransformNode
        //

        STDMETHOD_(UINT32, GetInputCount)() const override
        {
            return GetInputCountMethod.WasCalled();
        }

    };
}
