// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    class MockCanvasImageSourceDrawingSessionFactory : public CanvasImageSourceDrawingSessionFactory
    {
    public:
        std::function<ComPtr<ICanvasDrawingSession>(ICanvasDeviceInternal*,const Rect&)> MockCreate;

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDeviceInternal* canvasDevice,
            const Rect& updateRect) override
        {
            if (!MockCreate)
            {
                Assert::Fail(L"Unexpected call to Create");
                throw ComException(E_NOTIMPL);
            }

            return MockCreate(canvasDevice, updateRect);
        }
    };
}
