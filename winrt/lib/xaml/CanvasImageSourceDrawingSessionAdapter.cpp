// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasImageSourceDrawingSessionAdapter.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;

// static
std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> CanvasImageSourceDrawingSessionAdapter::Create(
    ISurfaceImageSourceNativeWithD2D* sisNative,
    D2D1_COLOR_F const& clearColor,
    RECT const& updateRect,
    float dpi,
    ID2D1DeviceContext1** outDeviceContext,
    D2D1_POINT_2F* outOffset)
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
    HRESULT hr = sisNative->BeginDraw(
        updateRect,
        IID_PPV_ARGS(&deviceContext),
        &offset);

    if (FAILED(hr))
    {
        // BeginDraw returns an an unhelpful E_FAIL if it is called from the wrong thread.
        // We check for that and translate to RPC_E_WRONG_THREAD instead.
        if (hr == E_FAIL)
        {
            ComPtr<IDispatcherQueue> dispatcher;
            ThrowIfFailed(As<IDependencyObject>(sisNative)->get_DispatcherQueue(&dispatcher));

            boolean hasThreadAccess;
            ThrowIfFailed(As<IDispatcherQueue2>(dispatcher)->get_HasThreadAccess(&hasThreadAccess));

            if (!hasThreadAccess)
                hr = RPC_E_WRONG_THREAD;
        }

        ThrowHR(hr);
    }

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

    // Update the offset to account for the updateRect (so the resulting
    // drawing session has the same coordinate system relative to the entire
    // image, rather than relative to the updateRect).
    offset.x -= updateRect.left;
    offset.y -= updateRect.top;

    *outOffset = D2D1_POINT_2F{
        static_cast<float>(offset.x / dpiScalingFactor),
        static_cast<float>(offset.y / dpiScalingFactor) };

    auto adapter = std::make_shared<CanvasImageSourceDrawingSessionAdapter>(
        sisNative);

    //
    // XAML has given us a surface to render to, but it doesn't make any
    // guarantees about what's currently on this surface.  So the only safe
    // thing to do here is to clear it.
    //
    deviceContext->Clear(&clearColor);

    deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(
        outOffset->x,
        outOffset->y));

    deviceContext->SetDpi(dpi, dpi);

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


void CanvasImageSourceDrawingSessionAdapter::EndDraw(ID2D1DeviceContext1*)
{
    ThrowIfFailed(m_sisNative->EndDraw());
}
