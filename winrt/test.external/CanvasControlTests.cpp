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

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasControlTests)
{
    // This is in a ref class because because non-ref classes cannot have member functions registered as callbacks.
    ref class CallbackVerifier sealed
    {
    public:

        void VerifyCallbacks()
        {
            CanvasControl^ canvasControl = ref new CanvasControl();

            Windows::Foundation::EventRegistrationToken creatingResourcesRegistrationToken;

            creatingResourcesRegistrationToken = canvasControl->CreatingResources += ref new Windows::Foundation::TypedEventHandler<CanvasControl^, Object^>(this, &CallbackVerifier::OnCreatingResources);

            canvasControl->CreatingResources -= creatingResourcesRegistrationToken;

            Windows::Foundation::EventRegistrationToken drawingRegistrationToken;

            drawingRegistrationToken = canvasControl->Drawing += ref new Windows::Foundation::TypedEventHandler<CanvasControl^, CanvasDrawingEventArgs^>(this, &CallbackVerifier::OnDrawing);

            canvasControl->Drawing -= drawingRegistrationToken;
        }

        void OnCreatingResources(CanvasControl^ sender, Object^ args)
        {
        }

        void OnDrawing(CanvasControl^ sender, CanvasDrawingEventArgs^ args)
        {
        }
        
    };

    TEST_METHOD(CanvasControl_Construction_And_Closure)
    {
        RunOnUIThread(
            []()
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
            []()
            {
                Assert::ExpectException< Platform::InvalidCastException^>(
                    []()
                    {
                        CanvasControl^ canvasControl = ref new CanvasControl();
                        GetWrappedResource<ID2D1Factory>(canvasControl);
                    });
            });
    }

    TEST_METHOD(CanvasControl_EventArgs_Construction)
    {
        RunOnUIThread(
            []()
            {
                auto device = ref new CanvasDevice();
                auto imageSource = ref new CanvasImageSource(device, 1, 1);
                auto drawingSession = imageSource->CreateDrawingSession();

                CanvasDrawingEventArgs^ drawingEventArgs = ref new CanvasDrawingEventArgs(drawingSession);
            });
    }

    TEST_METHOD(CanvasControl_ControlAsResourceAllocator)
    {
        RunOnUIThread(
            []()
            {
                CanvasControl^ canvasControl = ref new CanvasControl();

                CanvasSolidColorBrush^ brush = ref new CanvasSolidColorBrush(
                    canvasControl,
                    Windows::UI::Colors::Magenta
                    );
            });
    }
};
