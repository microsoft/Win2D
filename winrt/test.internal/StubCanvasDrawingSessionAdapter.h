// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    class StubCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
    public:
        StubCanvasDrawingSessionAdapter()
        {}

        virtual void EndDraw() override
        {
            // nothing
        }
    };
}
