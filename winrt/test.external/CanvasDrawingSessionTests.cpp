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

