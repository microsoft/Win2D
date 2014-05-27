// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    //
    // The stub CanvasDevice provides basic functionality to allow it to be used
    // in tests that don't care about interactions with CanvasDevice.
    //
    class StubCanvasDevice : public MockCanvasDevice
    {
        ComPtr<ID2D1Device1> m_D2DDevice;

    public:
        StubCanvasDevice(ComPtr<ID2D1Device1> device = Make<MockD2DDevice>())
            : m_D2DDevice(device)
        {
        }

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            return m_D2DDevice;
        }
    };
}
