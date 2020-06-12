// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(RefCountTests)
{
    TEST_METHOD(CanvasControl_RefCount)
    {
        int refCount = -1;

        RunOnUIThread([&]
        {
            CanvasControl^ control = ref new CanvasControl();
            refCount = GetRefCount(control);
        });
     
        Assert::AreEqual(1, refCount);
    }

#ifdef CANVAS_ANIMATED_CONTROL_IS_ENABLED
    TEST_METHOD(CanvasAnimatedControl_RefCount)
    {
        int refCount = -1;

        RunOnUIThread([&]
        {
            CanvasAnimatedControl^ control = ref new CanvasAnimatedControl();
            refCount = GetRefCount(control);
        });

        Assert::AreEqual(1, refCount);
    }
#endif

    TEST_METHOD(CanvasSwapChainPanel_RefCount)
    {
        int refCount = -1;

        RunOnUIThread([&]
        {
            CanvasSwapChainPanel^ swapChainPanel = ref new CanvasSwapChainPanel();
            refCount = GetRefCount(swapChainPanel);
        });

        Assert::AreEqual(1, refCount);
    }

    TEST_METHOD(CanvasImageSource_RefCount)
    {
        int refCount = -1;

        RunOnUIThread([&]
        {
            CanvasDevice^ device = ref new CanvasDevice();
            CanvasImageSource^ imageSource = ref new CanvasImageSource(device, 1, 1, 96);
            refCount = GetRefCount(imageSource);
        });

        Assert::AreEqual(1, refCount);
    }
};
