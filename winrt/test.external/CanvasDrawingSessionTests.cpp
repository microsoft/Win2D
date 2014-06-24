// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasDrawingSessionTests)
{
    //
    // CanvasDrawingSession interop is tested more directly in test.internal.
    // This exercises the published API.
    //
    TEST_METHOD(CanvasDrawingSession_Interop)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(canvasDevice);

        ComPtr<ID2D1DeviceContext1> context;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &context));

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());
        auto actualContext = GetWrappedResource<ID2D1DeviceContext1>(drawingSession);

        Assert::AreEqual(context.Get(), actualContext.Get());
    }
};

