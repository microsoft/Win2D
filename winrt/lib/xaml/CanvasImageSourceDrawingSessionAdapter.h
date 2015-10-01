// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

    class CanvasImageSourceDrawingSessionAdapter : public DrawingSessionBaseAdapter,
                                                   private LifespanTracker<CanvasImageSourceDrawingSessionAdapter>
    {
        ComPtr<ISurfaceImageSourceNativeWithD2D> m_sisNative;

    public:
        static std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            D2D1_COLOR_F const& clearColor,
            RECT const& updateRect,
            float dpi,
            ID2D1DeviceContext1** outDeviceContext,
            D2D1_POINT_2F* outOffset);

        CanvasImageSourceDrawingSessionAdapter(
            ISurfaceImageSourceNativeWithD2D* sisNative);

        virtual ~CanvasImageSourceDrawingSessionAdapter() = default;

        virtual void EndDraw(ID2D1DeviceContext1*) override;
    };
}}}}}}
