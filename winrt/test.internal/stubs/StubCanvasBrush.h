// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "StubD2DBrush.h"

namespace canvas
{
    class StubCanvasBrush : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasBrush,
        ICanvasBrushInternal,
        CloakedIid<ICanvasResourceWrapperNative>>
    {
        ComPtr<ID2D1Brush> m_brush;

    public:
        StubCanvasBrush(ComPtr<ID2D1Brush> d2dBrush = nullptr)
            : m_brush(d2dBrush ? d2dBrush : Make<StubD2DBrush>())
        {
        }

        //
        // ICanvasBrushInternal
        //

        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override
        {
            return m_brush;
        }

        IFACEMETHOD(get_Opacity)(float *value) override
        {
            Assert::Fail(L"Unexpected call to get_Opacity");
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_Opacity)(float value) override
        {
            Assert::Fail(L"Unexpected call to put_Opacity");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Transform)(Numerics::Matrix3x2 *value) override
        {
            Assert::Fail(L"Unexpected call to get_Transform");
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_Transform)(Numerics::Matrix3x2 value) override
        {
            Assert::Fail(L"Unexpected call to put_Transform");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Device)(ICanvasDevice** value) override
        {
            Assert::Fail(L"Unexpected call to get_Device");
            return E_NOTIMPL;
        }

        IFACEMETHOD(GetNativeResource)(ICanvasDevice*, float, REFIID iid, void** resource) override
        {
            Assert::IsTrue(
                iid == __uuidof(ID2D1Brush) ||
                iid == __uuidof(ID2D1Resource));

            return m_brush.CopyTo(iid, resource);
        }
    };
}
