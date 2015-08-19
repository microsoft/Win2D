// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;

TEST_CLASS(PolymorphicBitmapTests)
{
    CanvasDevice^ m_canvasDevice;
    ComPtr<ID2D1Bitmap1> m_nonTargetBitmap;
    ComPtr<ID2D1Bitmap1> m_targetBitmap;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_canvasDevice = ref new CanvasDevice();
        m_nonTargetBitmap = CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS_NONE);
        m_targetBitmap = CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS_TARGET);
    }

    TEST_METHOD(GetWrappedNonTarget_AsCanvasBitmap_CanvasBitmapReturned)
    {
        CanvasBitmap^ bitmap = GetOrCreate<CanvasBitmap>(m_canvasDevice, m_nonTargetBitmap.Get());
        AssertTypeName<CanvasBitmap>(bitmap);
    }

    TEST_METHOD(GetWrappedNonTarget_AsCanvasRenderTarget_Fails)
    {
        Assert::ExpectException<Platform::InvalidCastException^>(
            [&]
            {
                GetOrCreate<CanvasRenderTarget>(m_canvasDevice, m_nonTargetBitmap.Get());
            });
    }

    TEST_METHOD(GetWrappedTarget_AsCanvasBitmap_CanvasRenderTargetReturned)
    {
        CanvasBitmap^ bitmap = GetOrCreate<CanvasBitmap>(m_canvasDevice, m_targetBitmap.Get());
        AssertTypeName<CanvasRenderTarget>(bitmap);
    }

    TEST_METHOD(GetWrappedTarget_AsCanvasRenderTarget_CanvasRenderTargetReturned)
    {
        CanvasRenderTarget^ renderTarget = GetOrCreate<CanvasRenderTarget>(m_canvasDevice, m_targetBitmap.Get());
        AssertTypeName<CanvasRenderTarget>(renderTarget);
    }

    TEST_METHOD(WrappedAsBitmapAndRenderTarget_ReturnsSameObject)
    {
        // This test confirms that both CanvasBitmapFactory and
        // CanvasRenderTargetFactory are sharing the same underlying map of d2d
        // objects to WinRT objects
        CanvasBitmap^ bitmap = GetOrCreate<CanvasBitmap>(m_canvasDevice, m_targetBitmap.Get());
        CanvasRenderTarget^ renderTarget = GetOrCreate<CanvasRenderTarget>(m_canvasDevice, m_targetBitmap.Get());

        Assert::IsTrue(IsSameInstance(reinterpret_cast<IInspectable*>(bitmap), reinterpret_cast<IInspectable*>(renderTarget)));
    }

    TEST_METHOD(GetWrappedTarget_DoesNotRelyOnD3DProperties)
    {
        //
        // WIC bitmaps are special in that there is no explicit TARGET flag on
        // them during creation. 
        //
        // This validates that interop evaluates target eligibility the 'right'
        // way- i.e. through D2D's TARGET flag, not through some attribute of 
        // a D3D resource.
        //
        WicBitmapTestFixture f = CreateWicBitmapTestFixture();

        Assert::ExpectException<Platform::InvalidCastException^>(
            [&]
            {
                GetOrCreate<CanvasRenderTarget>(m_canvasDevice, f.Bitmap.Get());
            });

        //
        // Likewise, a WIC-based D2D render target's backing bitmap *will* have
        // the TARGET flag. 
        //
        ComPtr<ID2D1Image> targetD2DImage;
        f.RenderTarget->GetTarget(&targetD2DImage);
        auto targetD2DBitmap = As<ID2D1Bitmap1>(targetD2DImage);

        CanvasRenderTarget^ bitmap = GetOrCreate<CanvasRenderTarget>(m_canvasDevice, targetD2DBitmap.Get());
        AssertTypeName<CanvasRenderTarget>(bitmap);
    }
};
