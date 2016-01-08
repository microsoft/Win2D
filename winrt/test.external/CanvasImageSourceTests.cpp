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


    TEST_METHOD(CanvasImageSource_MaxSizeError)
    {
        auto device = ref new CanvasDevice();
        auto maxSize = device->MaximumBitmapSizeInPixels;
        auto tooBig = NextValueRepresentableAsFloat(maxSize);
        wchar_t msg[256];

        Platform::COMException^ exception1;
        Platform::COMException^ exception2;
        Platform::COMException^ exception3;

        RunOnUIThread(
            [&]
            {
                try
                {
                    ref new CanvasImageSource(device, static_cast<float>(tooBig), 1, 96);
                }
                catch (Platform::COMException^ e)
                {
                    exception1 = e;
                }

                try
                {
                    ref new CanvasImageSource(device, 1, static_cast<float>(tooBig), 96);
                }
                catch (Platform::COMException^ e)
                {
                    exception2 = e;
                }
        
                try
                {
                    ref new CanvasImageSource(device, static_cast<float>(tooBig) / 2, 1, 192);
                }
                catch (Platform::COMException^ e)
                {
                    exception3 = e;
                }
            });

        swprintf_s(msg, L"Cannot create CanvasImageSource sized %d x 1; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { throw exception1; });

        swprintf_s(msg, L"Cannot create CanvasImageSource sized 1 x %d; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { throw exception2; });

        swprintf_s(msg, L"Cannot create CanvasImageSource sized %d x 2; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { throw exception3; });
    }


    TEST_METHOD(CanvasImageSource_DrawOnWrongThread)
    {
        auto device = ref new CanvasDevice();

        CanvasImageSource^ imageSource;
        CanvasVirtualImageSource^ virtualImageSource;

        RunOnUIThread(
            [&]
            {
                imageSource = ref new CanvasImageSource(device, 1, 1, 96);
                virtualImageSource = ref new CanvasVirtualImageSource(device, 1, 1, 96);
            });

        ExpectCOMException(RPC_E_WRONG_THREAD,
            [&]
            {
                imageSource->CreateDrawingSession(Colors::Black);
            });

        ExpectCOMException(RPC_E_WRONG_THREAD,
            [&]
            {
                virtualImageSource->CreateDrawingSession(Colors::Black, Rect{ 0, 0, 1, 1 });
            });
    }


    TEST_METHOD(CanvasImageSource_NestedBeginDraw)
    {
        Platform::COMException^ exception1;
        Platform::COMException^ exception2;

        RunOnUIThread(
            [&]
            {
                auto device = ref new CanvasDevice();

                auto imageSource = ref new CanvasImageSource(device, 1, 1, 96);
                auto virtualImageSource = ref new CanvasVirtualImageSource(device, 1, 1, 96);

                auto drawingSession = imageSource->CreateDrawingSession(Colors::Black);

                try
                {
                    imageSource->CreateDrawingSession(Colors::Black);
                }
                catch (Platform::COMException^ e)
                {
                    exception1 = e;
                }

                delete drawingSession;

                drawingSession = virtualImageSource->CreateDrawingSession(Colors::Black, Rect{ 0, 0, 1, 1 });

                try
                {
                    virtualImageSource->CreateDrawingSession(Colors::Black, Rect{ 0, 0, 1, 1 });
                }
                catch (Platform::COMException^ e)
                {
                    exception2 = e;
                }
        
                delete drawingSession;
        });

        auto msg = L"The last drawing session returned by CreateDrawingSession must be disposed before a new one can be created.";

        ExpectCOMException(E_FAIL, msg, [&] { throw exception1; });
        ExpectCOMException(E_FAIL, msg, [&] { throw exception2; });
    }
};
