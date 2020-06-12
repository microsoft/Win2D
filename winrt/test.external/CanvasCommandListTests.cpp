// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using Windows::Foundation::Rect;

TEST_CLASS(CanvasCommandListTests)
{
    CanvasDevice^ m_device;

public:
    CanvasCommandListTests()
        : m_device(ref new CanvasDevice())
    {
    }

    TEST_METHOD(CanvasCommandList_Construct)
    {
        auto cl = ref new CanvasCommandList(m_device);
    }

    TEST_METHOD(CanvasCommandList_GetBounds)
    {
        // GetBounds shares the same implementation as for images and effects,
        // so this is just a superficial test to kick the tires.

        auto cl = ref new CanvasCommandList(m_device);
        auto ds = cl->CreateDrawingSession();

        Rect bounds{-10, -10, 20, 20};
        ds->FillRectangle(bounds, Microsoft::UI::Colors::White);

        delete ds;

        // The command list doesn't know what DPI is was generated at, so
        // regardless of the DPI of the drawing session passed to GetBounds it
        // returns the same bounds.
        float dpis[] { 32, 96, 144 };

        for (auto dpi : dpis)
        {
            auto rt = ref new CanvasRenderTarget(m_device, 100, 100, dpi);
            Rect retrievedBounds = cl->GetBounds(rt->CreateDrawingSession());
            
            Assert::AreEqual(bounds, retrievedBounds);
        }
    }

    TEST_METHOD(CanvasCommandList_InteropWrapAClosedCommandList_ThenTryToDraw)
    {
        auto canvasDevice = ref new CanvasDevice();
        ComPtr<ID2D1DeviceContext1> d2dDeviceContext = CreateTestD2DDeviceContext(canvasDevice);

        ComPtr<ID2D1CommandList> d2dCommandList;
        ThrowIfFailed(d2dDeviceContext->CreateCommandList(&d2dCommandList));

        d2dDeviceContext->SetTarget(d2dCommandList.Get());
        d2dDeviceContext->BeginDraw();
        d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        ThrowIfFailed(d2dDeviceContext->EndDraw());
        d2dDeviceContext->SetTarget(nullptr);

        ThrowIfFailed(d2dCommandList->Close());

        auto canvasCommandList = GetOrCreate<CanvasCommandList>(canvasDevice, d2dCommandList.Get());

        auto somewhereToDrawTo = ref new CanvasCommandList(canvasDevice);
        auto drawingSession = somewhereToDrawTo->CreateDrawingSession();

        //
        // Win2D's command list is designed to hide the notion of a 'closed'
        // state from the app. It closes the D2D command list when the app tries
        // to draw it. However, trying to close an already-closed D2D command list
        // will put the device context into an error state. Win2D should
        // handle this appropriately, and not try to re-close it.
        //
        drawingSession->DrawImage(canvasCommandList);
        delete drawingSession;
    }

    TEST_METHOD(CanvasCommandList_ClosedCommandListLaterInteropped_ThenTryToDraw)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasCommandList = ref new CanvasCommandList(canvasDevice);

        auto somewhereToDrawTo = ref new CanvasCommandList(canvasDevice);
        auto drawingSession = somewhereToDrawTo->CreateDrawingSession();

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(canvasCommandList);
        ThrowIfFailed(d2dCommandList->Close());

        drawingSession->DrawImage(canvasCommandList);
        drawingSession->DrawImage(canvasCommandList);
        delete drawingSession;
    }

    TEST_METHOD(CanvasCommandList_DrawTwice)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasCommandList = ref new CanvasCommandList(canvasDevice);

        auto somewhereToDrawTo = ref new CanvasCommandList(canvasDevice);
        auto drawingSession = somewhereToDrawTo->CreateDrawingSession();

        //
        // Ensure we don't try to somehow double-close the command list.
        //
        drawingSession->DrawImage(canvasCommandList);
        drawingSession->DrawImage(canvasCommandList);
        delete drawingSession;
    }

    TEST_METHOD(CanvasCommandList_NestedBeginDraw)
    {
        auto device = ref new CanvasDevice();
        auto commandList = ref new CanvasCommandList(device);
        auto drawingSession = commandList->CreateDrawingSession();

        ExpectCOMException(
            E_FAIL, 
            L"The last drawing session returned by CreateDrawingSession must be disposed before a new one can be created.",
            [&]
            {
                commandList->CreateDrawingSession();
            });
    }
};
