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
