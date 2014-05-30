// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;

    class MockCanvasDrawingSession : public RuntimeClass<
        ICanvasDrawingSession>
    {
    public:
        IFACEMETHODIMP Clear(ABI::Windows::UI::Color color) override
        {
            Assert::Fail(L"Unexpected call to Clear");
            return E_NOTIMPL;
        }
    };
}
