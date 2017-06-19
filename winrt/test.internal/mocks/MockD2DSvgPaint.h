// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DSvgPaint : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgPaint, ID2D1SvgAttribute, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(SetPaintType, HRESULT(D2D1_SVG_PAINT_TYPE));
        MOCK_METHOD0(GetPaintType, D2D1_SVG_PAINT_TYPE());
        MOCK_METHOD1(SetColor, HRESULT(CONST D3DCOLORVALUE*));
        MOCK_METHOD1(GetColor, void(D3DCOLORVALUE*));
        MOCK_METHOD1(SetId, HRESULT(PCWSTR));
        MOCK_METHOD2(GetId, HRESULT(PWSTR, UINT32));
        MOCK_METHOD0(GetIdLength, UINT32());

        MOCK_METHOD1(GetElement, void(ID2D1SvgElement**));
        MOCK_METHOD1(Clone, HRESULT(ID2D1SvgAttribute**));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));
    };
}
