// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "TestBitmapResourceCreationAdapter.h"

TEST_CLASS(D2DBitmapInteropUnitTests)
{
    std::shared_ptr<PolymorphicBitmapManager> m_manager;
    ComPtr<StubCanvasDevice> m_canvasDevice;

    ComPtr<StubD2DBitmap> m_nonTargetBitmap;
    ComPtr<StubD2DBitmap> m_targetBitmap;

public:
    TEST_METHOD_INITIALIZE(Reset)
    {
        m_manager = std::make_shared<PolymorphicBitmapManager>(std::make_shared<TestBitmapResourceCreationAdapter>());
        m_canvasDevice = Make<StubCanvasDevice>();
        m_nonTargetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE);
        m_targetBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
    }

    TEST_METHOD(D2DBitmapNonTarget_WrappedAsCanvasBitmap_CanvasBitmapReturned)
    {
        auto wrapped = m_manager->GetOrCreateBitmap(m_canvasDevice.Get(), m_nonTargetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap);
    }

    TEST_METHOD(D2DBitmapNonTarget_WrappedAsCanvasRenderTarget_Fails)
    {
        ExpectHResultException(E_INVALIDARG, 
            [&]
            { 
                m_manager->GetOrCreateRenderTarget(m_canvasDevice.Get(), m_nonTargetBitmap.Get()); 
            });
    }

    TEST_METHOD(D2DBitmapTarget_WrappedAsCanvasRenderTarget_CanvasRenderTargetReturned)
    {
        auto wrapped = m_manager->GetOrCreateRenderTarget(m_canvasDevice.Get(), m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }

    TEST_METHOD(D2DBitmapTarget_WrappedAsCanvasBitmap_CanvasRenderTargetReturned)
    {
        auto wrapped = m_manager->GetOrCreateBitmap(m_canvasDevice.Get(), m_targetBitmap.Get());

        AssertClassName(wrapped, RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget);
    }


    template<typename T>
    void AssertClassName(ComPtr<T> obj, wchar_t const* expectedClassName)
    {
        ComPtr<IInspectable> inspectable;
        ThrowIfFailed(obj.As(&inspectable));

        WinString className;
        ThrowIfFailed(inspectable->GetRuntimeClassName(className.GetAddressOf()));

        Assert::AreEqual(expectedClassName, static_cast<wchar_t const*>(className));
    }
};
