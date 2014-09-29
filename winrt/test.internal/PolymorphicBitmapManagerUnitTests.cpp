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

#include "TestBitmapResourceCreationAdapter.h"

TEST_CLASS(PolymorphicBitmapManagerTests_FromD2DBitmap)
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
};
