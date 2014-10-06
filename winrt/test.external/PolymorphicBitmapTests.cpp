// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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
        Assert::ExpectException<Platform::InvalidArgumentException^>(
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
};
