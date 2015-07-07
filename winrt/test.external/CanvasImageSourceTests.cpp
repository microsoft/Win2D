// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

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


    TEST_METHOD(CanvasImageSource_Constructors)
    {
        Size size1{ 0, 0 };
        Size size2{ 0, 0 };

        RunOnUIThread(
            [&]
            {
                auto creator = ref new StubResourceCreatorWithDpi(ref new CanvasDevice());

                auto imageSource1 = ref new CanvasImageSource(creator, 23, 42);
                auto imageSource2 = ref new CanvasImageSource(creator, Size{ 7, 21 });

                size1 = imageSource1->Size;
                size2 = imageSource2->Size;
            });

        Assert::AreEqual(size1, Size{ 23, 42 });
        Assert::AreEqual(size2, Size{ 7, 21 });
    }
};

