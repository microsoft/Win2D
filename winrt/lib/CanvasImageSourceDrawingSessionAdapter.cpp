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

#include "CanvasImageSourceDrawingSessionAdapter.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    // static
    std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> CanvasImageSourceDrawingSessionAdapter::Create(
        ISurfaceImageSourceNativeWithD2D* sisNative,
        RECT const& updateRect,
        float dpi,
        ID2D1DeviceContext1** outDeviceContext)
    {
        if (dpi <= 0)
            ThrowHR(E_INVALIDARG);

        //
        // ISurfaceImageSourceNativeWithD2D needs exactly the right IID passed
        // in, so we first ask for a ID2D1DeviceContext and we'll then take the
        // resulting interface and QI for ID2D1DeviceContext1.
        //
        ComPtr<ID2D1DeviceContext> deviceContext;
        POINT offset;
        ThrowIfFailed(sisNative->BeginDraw(
            updateRect,
            IID_PPV_ARGS(&deviceContext),
            &offset));

        //
        // If this function fails then we need to call EndDraw
        //
        auto endDrawWarden = MakeScopeWarden([&] { sisNative->EndDraw(); });

        ThrowIfFailed(deviceContext.CopyTo(outDeviceContext));

        //
        // ISurfaceImageSourceNativeWithD2D is operating in batched mode.  This
        // means that BeginDraw() has returned a device context set up to render
        // to an atlased surface (ie a surface containing multiple images).  The
        // image that we wish to render to appears at some offset in this
        // surface -- and this is what has been given to us in offset.  This
        // call sets the transform on the device context to compensate for this,
        // so when we ask to render to (0,0) we actually render to the
        // appropriate location in the atlased surface.
        //
        const float dpiScalingFactor = dpi / DEFAULT_DPI;

        const D2D1_POINT_2F renderingSurfaceOffset = D2D1::Point2F(
            static_cast<float>(offset.x / dpiScalingFactor),
            static_cast<float>(offset.y / dpiScalingFactor));

        auto adapter = std::make_shared<CanvasImageSourceDrawingSessionAdapter>(
            sisNative,
            renderingSurfaceOffset);

        //
        // TODO #2140 Use a separate code path, responsible for resetting
        // transforms for non-SiS drawing sessions.  As part of #2140, also add
        // tests verifying that the offset is scaled by the DPI appropriately.
        //
        deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(
            renderingSurfaceOffset.x,
            renderingSurfaceOffset.y));

        deviceContext->SetDpi(dpi, dpi);

        //
        // This function can't fail now, so we can dismiss the end draw warden.
        //
        endDrawWarden.Dismiss();

        return adapter;
    }


    CanvasImageSourceDrawingSessionAdapter::CanvasImageSourceDrawingSessionAdapter(
        ISurfaceImageSourceNativeWithD2D* sisNative,
        D2D1_POINT_2F const& renderingSurfaceOffset)
        : m_sisNative(sisNative)
        , m_renderingSurfaceOffset(renderingSurfaceOffset)
    {
        CheckInPointer(sisNative);
    }


    void CanvasImageSourceDrawingSessionAdapter::EndDraw()
    {
        ThrowIfFailed(m_sisNative->EndDraw());
    }

    D2D1_POINT_2F CanvasImageSourceDrawingSessionAdapter::GetRenderingSurfaceOffset()
    {
        return m_renderingSurfaceOffset;
    }
}}}}
