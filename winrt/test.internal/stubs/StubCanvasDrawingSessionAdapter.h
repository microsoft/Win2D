// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
    public:
        StubCanvasDrawingSessionAdapter()
        {}

        virtual void EndDraw(ID2D1DeviceContext1*) override
        {
            // nothing
        }
    };
}
