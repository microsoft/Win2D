// Copyright (c) Microsoft Corporation.  All rights reserved

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
        AssertTypeName(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, bitmap);
    }

    TEST_METHOD(GetWrappedNonTarget_AsCanvasRenderTarget_Fails)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                GetOrCreate<CanvasRenderTarget>(m_canvasDevice, m_nonTargetBitmap.Get());
            });
    }

    TEST_METHOD(GetWrappedTarget_AsCanvasBitmap_CanvasRenderTargetReturned)
    {
        CanvasBitmap^ bitmap = GetOrCreate<CanvasBitmap>(m_canvasDevice, m_targetBitmap.Get());
        AssertTypeName(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget, bitmap);
    }

    TEST_METHOD(GetWrappedTarget_AsCanvasRenderTarget_CanvasRenderTargetReturned)
    {
        CanvasRenderTarget^ renderTarget = GetOrCreate<CanvasRenderTarget>(m_canvasDevice, m_targetBitmap.Get());
        AssertTypeName(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget, renderTarget);
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

    template<typename T>
    void AssertTypeName(wchar_t const* expected, T^ obj)
    {
        Assert::AreEqual(ref new Platform::String(expected), obj->GetType()->FullName);
    }

    ComPtr<ID2D1Bitmap1> CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS options)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();
        ComPtr<ID2D1Bitmap1> d2dBitmap;

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.bitmapOptions = options;
        bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

        ThrowIfFailed(context->CreateBitmap(
            D2D1::SizeU(1, 1),
            nullptr, // data 
            0,  // data pitch
            &bitmapProperties,
            &d2dBitmap));

        return d2dBitmap;
    }
};
