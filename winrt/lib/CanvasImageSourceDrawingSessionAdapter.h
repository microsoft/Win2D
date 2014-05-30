// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "CanvasDrawingSession.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

    class CanvasImageSourceDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
        ComPtr<ISurfaceImageSourceNativeWithD2D> m_sisNative;

    public:
        static std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const RECT& updateRect,
            ID2D1DeviceContext1** outDeviceContext);

        CanvasImageSourceDrawingSessionAdapter(
            ISurfaceImageSourceNativeWithD2D* sisNative);

        virtual ~CanvasImageSourceDrawingSessionAdapter() = default;

        virtual void EndDraw() override;
    };
}
