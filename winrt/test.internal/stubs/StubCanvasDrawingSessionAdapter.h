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

        virtual void EndDraw() override
        {
            // nothing
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        virtual ComPtr<IInkD2DRenderer> CreateInkRenderer() override
        {
            return nullptr;
        }

        virtual bool IsHighContrastEnabled() override
        {
            return false;
        }
#endif
    };
}
