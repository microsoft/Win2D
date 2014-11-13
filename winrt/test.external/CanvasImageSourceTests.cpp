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
using namespace Windows::UI;

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
            []
            {
                Color anyColor{1,2,3,4};

                // Verify creation off of a device.
                auto canvasDevice = ref new CanvasDevice();
                    
                auto canvasImageSource = ref new CanvasImageSource(
                    canvasDevice,
                    1,
                    1,
                    DEFAULT_DPI);
                    
                auto drawingSession = canvasImageSource->CreateDrawingSession(anyColor);
                Assert::AreEqual(drawingSession->Device, canvasImageSource->Device);
                delete drawingSession;

                delete canvasImageSource;
            });
    }
};

