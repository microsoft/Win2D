// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Foundation;
    
    static Rect GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession *drawingSession,
        Numerics::Matrix3x2 const* transform)
    {
        auto drawingSessionResourceWrapper = As<ICanvasResourceWrapperNative>(drawingSession);

        ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
        ThrowIfFailed(drawingSessionResourceWrapper->GetResource(IID_PPV_ARGS(&d2dDeviceContext)));

        auto d2dImage = imageInternal->GetD2DImage(d2dDeviceContext.Get());

        D2D1_RECT_F d2dBounds;
        
        D2D1_MATRIX_3X2_F previousTransform;
        d2dDeviceContext->GetTransform(&previousTransform);

        auto restoreTransformWarden = MakeScopeWarden([&] { d2dDeviceContext->SetTransform(previousTransform); });

        d2dDeviceContext->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F const*>(transform));

        ThrowIfFailed(d2dDeviceContext->GetImageWorldBounds(d2dImage.Get(), &d2dBounds));

        return FromD2DRect(d2dBounds);
    }

    HRESULT GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession* drawingSession,
        Numerics::Matrix3x2 const* transform,
        Rect* bounds)
    {
        static Numerics::Matrix3x2 identity = { 1, 0, 0, 1, 0, 0 };

        if (!transform)
            transform = &identity;

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(drawingSession);
                CheckInPointer(bounds);

                *bounds = GetImageBoundsImpl(imageInternal, drawingSession, transform);
            });
    }
}}}}
