// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

class MockD2DSpriteBatch : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    ID2D1SpriteBatch>
{
public:
    //
    // ID2D1Resource
    //

    MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    //
    // ID2D1SpriteBatch
    //
    
    MOCK_METHOD9(AddSprites, HRESULT(UINT32, CONST D2D1_RECT_F*, CONST D2D1_RECT_U*, CONST D2D1_COLOR_F*, CONST D2D1_MATRIX_3X2_F*, UINT32, UINT32, UINT32, UINT32));
    MOCK_METHOD10(SetSprites, HRESULT(UINT32, UINT32, CONST D2D1_RECT_F*, CONST D2D1_RECT_U*, CONST D2D1_COLOR_F*, CONST D2D1_MATRIX_3X2_F*, UINT32, UINT32, UINT32, UINT32));
    MOCK_METHOD6_CONST(GetSprites, HRESULT(UINT32, UINT32, D2D1_RECT_F*, D2D1_RECT_U*, D2D1_COLOR_F*, D2D1_MATRIX_3X2_F*));
    MOCK_METHOD0_CONST(GetSpriteCount, UINT32());
    MOCK_METHOD0(Clear, void());
};

#endif
