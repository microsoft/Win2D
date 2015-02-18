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

TEST_CLASS(RefCountTests)
{
    template<typename T>
    static int GetRefCount(T^ const& hatPointer)
    {
        IInspectable* asInspectable = reinterpret_cast<IInspectable*>(hatPointer);
        
        asInspectable->AddRef();
        return (int)asInspectable->Release();
    }

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
