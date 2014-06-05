// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace Microsoft::WRL;

    [uuid(3A6BF1D2-731A-4EBB-AA40-1419A89302F6)]
    class ICanvasBrushInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Brush> GetD2DBrush() = 0;
    };
}
