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
