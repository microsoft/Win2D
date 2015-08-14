// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockCanvasImageSourceDrawingSessionFactory : public ICanvasImageSourceDrawingSessionFactory
    {
    public:
        CALL_COUNTER_WITH_MOCK(CreateMethod, ComPtr<ICanvasDrawingSession>(ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, Rect const&, float));

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            Color const& clearColor,
            Rect const& updateRectangleInDips,
            float dpi) const override
        {
            return CreateMethod.WasCalled(owner, sisNative, clearColor, updateRectangleInDips, dpi);
        }
    };
}
