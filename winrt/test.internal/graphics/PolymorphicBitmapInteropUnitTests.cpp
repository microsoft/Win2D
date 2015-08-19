// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(PolymorphicBitmapInteropTests_FromD2DBitmap)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;
        
        ComPtr<StubD2DBitmap> m_nonTargetBitmap;
        ComPtr<StubD2DBitmap> m_targetBitmap;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            m_nonTargetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE);
            m_targetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        }
    };

    TEST_METHOD_EX(D2DBitmapNonTarget_WrappedAsCanvasBitmap_CanvasBitmapReturned)
    {
        Fixture f;

        auto wrapped = ResourceManager::GetOrCreate<ICanvasBitmap>(f.m_canvasDevice.Get(), f.m_nonTargetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap);
    }

    TEST_METHOD_EX(D2DBitmapNonTarget_WrappedAsCanvasRenderTarget_Fails)
    {
        Fixture f;

        ExpectHResultException(E_NOINTERFACE, 
            [&]
            { 
                ResourceManager::GetOrCreate<ICanvasRenderTarget>(f.m_canvasDevice.Get(), f.m_nonTargetBitmap.Get());
            });
    }

    TEST_METHOD_EX(D2DBitmapTarget_WrappedAsCanvasRenderTarget_CanvasRenderTargetReturned)
    {
        Fixture f;

        auto wrapped = ResourceManager::GetOrCreate<ICanvasRenderTarget>(f.m_canvasDevice.Get(), f.m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }

    TEST_METHOD_EX(D2DBitmapTarget_WrappedAsCanvasBitmap_CanvasRenderTargetReturned)
    {
        Fixture f;

        auto wrapped = ResourceManager::GetOrCreate<ICanvasBitmap>(f.m_canvasDevice.Get(), f.m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }
};
