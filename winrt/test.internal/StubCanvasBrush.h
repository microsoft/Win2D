// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "StubD2DBrush.h"

namespace canvas
{
    class StubCanvasBrush : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasBrush,
        ICanvasBrushInternal>
    {
        ComPtr<ID2D1Brush> m_brush;

    public:
        StubCanvasBrush()
            : m_brush(Make<StubD2DBrush>())
        {
        }

        //
        // ICanvasBrushInternal
        //

        virtual ComPtr<ID2D1Brush> GetD2DBrush() override
        {
            return m_brush;
        }

        IFACEMETHOD(get_Opacity)(_Out_ float *value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_Opacity)(_In_ float value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Transform)(_Out_ Numerics::Matrix3x2 *value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_Transform)(_In_ Numerics::Matrix3x2 value) override
        {
            return E_NOTIMPL;
        }
    };
}
