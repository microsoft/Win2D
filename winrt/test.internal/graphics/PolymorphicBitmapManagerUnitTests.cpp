// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(PolymorphicBitmapManagerTests_FromD2DBitmap)
{
    struct Fixture
    {
        std::shared_ptr<PolymorphicBitmapManager> m_manager;
        ComPtr<StubCanvasDevice> m_canvasDevice;
        
        ComPtr<StubD2DBitmap> m_nonTargetBitmap;
        ComPtr<StubD2DBitmap> m_targetBitmap;

        Fixture()
        {
            m_manager = std::make_shared<PolymorphicBitmapManager>(std::make_shared<TestBitmapResourceCreationAdapter>());
            m_canvasDevice = Make<StubCanvasDevice>();
            m_nonTargetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE);
            m_targetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        }
    };

    TEST_METHOD_EX(D2DBitmapNonTarget_WrappedAsCanvasBitmap_CanvasBitmapReturned)
    {
        Fixture f;

        auto wrapped = f.m_manager->GetOrCreateBitmap(f.m_canvasDevice.Get(), f.m_nonTargetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap);
    }

    TEST_METHOD_EX(D2DBitmapNonTarget_WrappedAsCanvasRenderTarget_Fails)
    {
        Fixture f;

        ExpectHResultException(E_INVALIDARG, 
            [&]
            { 
                f.m_manager->GetOrCreateRenderTarget(f.m_canvasDevice.Get(), f.m_nonTargetBitmap.Get()); 
            });
    }

    TEST_METHOD_EX(D2DBitmapTarget_WrappedAsCanvasRenderTarget_CanvasRenderTargetReturned)
    {
        Fixture f;

        auto wrapped = f.m_manager->GetOrCreateRenderTarget(f.m_canvasDevice.Get(), f.m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }

    TEST_METHOD_EX(D2DBitmapTarget_WrappedAsCanvasBitmap_CanvasRenderTargetReturned)
    {
        Fixture f;

        auto wrapped = f.m_manager->GetOrCreateBitmap(f.m_canvasDevice.Get(), f.m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }
};
