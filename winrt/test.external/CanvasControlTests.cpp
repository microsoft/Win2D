// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace WinRTDirectX;

TEST_CLASS(CanvasControlTests)
{
    // This is in a ref class because because non-ref classes cannot have member functions registered as callbacks.
    ref class CallbackVerifier sealed
    {
    public:

        void VerifyCallbacks()
        {
            CanvasControl^ canvasControl = ref new CanvasControl();

            Windows::Foundation::EventRegistrationToken createResourcesRegistrationToken;

            createResourcesRegistrationToken = canvasControl->CreateResources += ref new Windows::Foundation::TypedEventHandler<CanvasControl^, CanvasCreateResourcesEventArgs^>(this, &CallbackVerifier::OnCreateResources);

            canvasControl->CreateResources -= createResourcesRegistrationToken;

            Windows::Foundation::EventRegistrationToken drawRegistrationToken;

            drawRegistrationToken = canvasControl->Draw += ref new Windows::Foundation::TypedEventHandler<CanvasControl^, CanvasDrawEventArgs^>(this, &CallbackVerifier::OnDraw);

            canvasControl->Draw -= drawRegistrationToken;
        }

        void OnCreateResources(CanvasControl^ sender, CanvasCreateResourcesEventArgs^ args)
        {
        }

        void OnDraw(CanvasControl^ sender, CanvasDrawEventArgs^ args)
        {
        }
        
    };

    TEST_METHOD(CanvasControl_Construction_And_Closure)
    {
        RunOnUIThread(
            []
            {
                CallbackVerifier verifier;
                verifier.VerifyCallbacks();
            });
    }

    TEST_METHOD(CanvasControl_NotInteropable)
    {
        // Verify that interop directly with canvas control fails.
        //
        // GetOrCreate is intentionally not tested here. There 
        // isn't a way of, say, iterating over every native type in the 
        // world, and verifying it fails with CanvasControl type.
        // 
        // The other direction, however, is verified in this test.
        //
        RunOnUIThread(
            []
            {
                Assert::ExpectException< Platform::InvalidCastException^>(
                    []
                    {
                        CanvasControl^ canvasControl = ref new CanvasControl();
                        GetWrappedResource<ID2D1Factory>(canvasControl);
                    });
            });
    }

    TEST_METHOD(CanvasControl_EventArgs_Construction)
    {
        RunOnUIThread(
            []
            {
                auto device = ref new CanvasDevice();
                auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
                auto drawingSession = renderTarget->CreateDrawingSession();

                CanvasDrawEventArgs^ drawEventArgs = ref new CanvasDrawEventArgs(drawingSession);
            });
    }
};
