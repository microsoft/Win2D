// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

class MockCompositionDrawingSurface : public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>,
    ICompositionDrawingSurface,
    ICompositionDrawingSurfaceInterop>
{
public:
    // ICompositionDrawingSurface
    MOCK_METHOD1(get_AlphaMode   , HRESULT(DirectXAlphaMode* value));
    MOCK_METHOD1(get_PixelFormat , HRESULT(DirectXPixelFormat* value));
    MOCK_METHOD1(get_Size        , HRESULT(Size *value));
    
    // ICompositionDrawingSurfaceInterop
    MOCK_METHOD4(BeginDraw, HRESULT(const RECT* updateRect, REFIID iid, void** updateObject, POINT* updateOffset));
    MOCK_METHOD0(EndDraw, HRESULT());
    MOCK_METHOD1(Resize, HRESULT(SIZE pixelSize));
    MOCK_METHOD4(Scroll, HRESULT(const RECT* scrollRect, const RECT* clipRect, int offsetX, int offsetY));
    MOCK_METHOD0(ResumeDraw, HRESULT());
    MOCK_METHOD0(SuspendDraw, HRESULT());
};

#endif