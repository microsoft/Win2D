// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasImageSourceTests)
{
    //
    // CanvasImageSource has more extensive testing in test.internal.  This
    // checks that it works in a more realistic setting where it is interacting
    // with real XAML types and is using the actual activation factory.
    //
    TEST_METHOD(CanvasImageSource_Exercise)
    {
        RunOnUIThread(
            []()
            {
                auto canvasDevice = ref new CanvasDevice();
                    
                auto canvasImageSource = ref new CanvasImageSource(
                    canvasDevice,
                    1,
                    1);
                    
                auto drawingSession = canvasImageSource->CreateDrawingSession();
                delete drawingSession;
            });
    }
};

