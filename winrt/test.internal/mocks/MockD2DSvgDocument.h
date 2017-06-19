// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace canvas
{
    class MockD2DSvgDocument : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgDocument, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(SetViewportSize, HRESULT(D2D1_SIZE_F));
        MOCK_METHOD0_CONST(GetViewportSize, D2D1_SIZE_F());
        MOCK_METHOD1(SetRoot, HRESULT(ID2D1SvgElement*));
        MOCK_METHOD1(GetRoot, void(ID2D1SvgElement**));
        MOCK_METHOD2(FindElementById, HRESULT(PCWSTR, ID2D1SvgElement**));
        MOCK_METHOD2(Serialize, HRESULT(IStream*, ID2D1SvgElement*));
        MOCK_METHOD2(Deserialize, HRESULT(IStream*, ID2D1SvgElement**));
        MOCK_METHOD4(CreatePaint, HRESULT(D2D1_SVG_PAINT_TYPE, CONST D3DCOLORVALUE*, PCWSTR, ID2D1SvgPaint**));
        MOCK_METHOD3(CreateStrokeDashArray, HRESULT(CONST D2D1_SVG_LENGTH*, UINT32, ID2D1SvgStrokeDashArray**));
        MOCK_METHOD3(CreatePointCollection, HRESULT(CONST D2D1_POINT_2F*, UINT32, ID2D1SvgPointCollection**));
        MOCK_METHOD5(CreatePathData, HRESULT(CONST FLOAT*, UINT32, CONST D2D1_SVG_PATH_COMMAND*, UINT32, ID2D1SvgPathData**));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}

#endif