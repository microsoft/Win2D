// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "ClosablePtr.h"

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::Foundation;
    using namespace Microsoft::WRL;

    class ICanvasDrawingSessionAdapter
    {
    public:
        virtual ~ICanvasDrawingSessionAdapter() = default;
        virtual void EndDraw() = 0;
    };

    class CanvasDrawingSession : public RuntimeClass<
        ICanvasDrawingSession,
        IClosable>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingSession, BaseTrust);

        ClosablePtr<ID2D1DeviceContext1> m_deviceContext;
        std::shared_ptr<ICanvasDrawingSessionAdapter> m_adapter;

    public:
        CanvasDrawingSession(
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        virtual ~CanvasDrawingSession();

        // IClosable

        IFACEMETHOD(Close)() override;

        // ICanvasDrawingSession

        IFACEMETHOD(Clear)(ABI::Windows::UI::Color color) override;
    };
}
