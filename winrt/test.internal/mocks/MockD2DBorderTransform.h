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
        MOCK_METHOD1(SetExtendModeX, void(D2D1_EXTEND_MODE));
        MOCK_METHOD1(SetExtendModeY, void(D2D1_EXTEND_MODE));
        MOCK_METHOD0_CONST(GetExtendModeX, D2D1_EXTEND_MODE());
        MOCK_METHOD0_CONST(GetExtendModeY, D2D1_EXTEND_MODE());

        MOCK_METHOD2(SetOutputBuffer, HRESULT(D2D1_BUFFER_PRECISION, D2D1_CHANNEL_DEPTH));
        MOCK_METHOD1(SetCached, void(BOOL));

        MOCK_METHOD0_CONST(GetInputCount, UINT32());

    };
}
