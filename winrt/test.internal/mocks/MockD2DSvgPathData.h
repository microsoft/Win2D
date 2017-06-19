// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DSvgPathData : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgPathData, ID2D1SvgAttribute, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(RemoveSegmentDataAtEnd, HRESULT(UINT32));
        MOCK_METHOD3(UpdateSegmentData, HRESULT(CONST FLOAT*, UINT32, UINT32));
        MOCK_METHOD3(GetSegmentData, HRESULT(FLOAT*, UINT32, UINT32));
        MOCK_METHOD0(GetSegmentDataCount, UINT32());
        MOCK_METHOD1(RemoveCommandsAtEnd, HRESULT(UINT32));
        MOCK_METHOD3(UpdateCommands, HRESULT(CONST D2D1_SVG_PATH_COMMAND*, UINT32, UINT32));
        MOCK_METHOD3(GetCommands, HRESULT(D2D1_SVG_PATH_COMMAND*, UINT32, UINT32));
        MOCK_METHOD0(GetCommandsCount, UINT32());
        MOCK_METHOD2(CreatePathGeometry, HRESULT(D2D1_FILL_MODE, ID2D1PathGeometry1**));

        MOCK_METHOD1(GetElement, void(ID2D1SvgElement**));
        MOCK_METHOD1(Clone, HRESULT(ID2D1SvgAttribute**));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
