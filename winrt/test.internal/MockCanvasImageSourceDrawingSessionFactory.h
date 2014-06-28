// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    class MockCanvasImageSourceDrawingSessionFactory : public ICanvasImageSourceDrawingSessionFactory
    {
    public:
        std::function<ComPtr<ICanvasDrawingSession>(ISurfaceImageSourceNativeWithD2D*,const Rect&)> MockCreate;

        virtual ComPtr<ICanvasDrawingSession> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const Rect& updateRect) const override
        {
            if (!MockCreate)
            {
                Assert::Fail(L"Unexpected call to Create");
                ThrowHR(E_NOTIMPL);
            }

            return MockCreate(sisNative, updateRect);
        }
    };
}
