// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "CanvasImageSourceDrawingSessionAdapter.h"

namespace canvas
{
    // static
    std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> CanvasImageSourceDrawingSessionAdapter::Create(
        ISurfaceImageSourceNativeWithD2D* sisNative,
        const RECT& updateRect,
        ID2D1DeviceContext1** outDeviceContext)
    {
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
        auto endDrawWarden = MakeScopeWarden([&]() { sisNative->EndDraw(); });

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
        deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x),
            static_cast<float>(offset.y)));            

        auto adapter = std::make_shared<CanvasImageSourceDrawingSessionAdapter>(
            sisNative);

        //
        // This function can't fail now, so we can dismiss the end draw warden.
        //
        endDrawWarden.Dismiss();

        return adapter;
    }


    CanvasImageSourceDrawingSessionAdapter::CanvasImageSourceDrawingSessionAdapter(
        ISurfaceImageSourceNativeWithD2D* sisNative)
        : m_sisNative(sisNative)
    {
        CheckInPointer(sisNative);
    }


    void CanvasImageSourceDrawingSessionAdapter::EndDraw()
    {
        ThrowIfFailed(m_sisNative->EndDraw());
    }
}
