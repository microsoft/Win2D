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
        ds->FillRectangle(bounds, Windows::UI::Colors::White);

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
};
