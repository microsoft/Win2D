// Copyright (c) Microsoft Corporation.  All rights reserved

// This device is used for creating mock devices which include some basic functionality, but do not call any actual D2D/D3D.

class TestDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter
{
    // If not set, fail all attempts at creating a device with CanvasHardwareAcceleration::On.
    bool m_allowHardware;

public:

    TestDeviceResourceCreationAdapter()
        : m_numD2DFactoryCreationCalls(0)
        , m_debugLevel(static_cast<CanvasDebugLevel>(-1))
        , m_numD3dDeviceCreationCalls(0)
        , m_allowHardware(true)
        , m_retrievableHarwareAcceleration(CanvasHardwareAcceleration::Auto)
    {}

    ComPtr<ID2D1Factory2> CreateD2DFactory(
        CanvasDebugLevel debugLevel
        )
    {
        m_numD2DFactoryCreationCalls++;
        m_debugLevel = debugLevel;

        // Mock factory is used here, because, in execution of these tests, product code will 
        // actually call methods on the factory and expect them to succeed.
        return Make<MockD2DFactory>();
    }
    virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device)
    {
        if (!device)
        {
            Assert::Fail(L"Unexpected null device parameter in TryCreateD3DDevice");
            return false;
        }

        m_retrievableHarwareAcceleration = hardwareAcceleration;

        if (hardwareAcceleration == CanvasHardwareAcceleration::On && !m_allowHardware)
        {
            return false;
        }
        else
        {
            m_numD3dDeviceCreationCalls++;

            // Mock factory is used here, because, in execution of these tests, product code will 
            // actually call methods on the factory and expect them to succeed.
            *device = Make<MockD3D11Device>();
            return true;
        }
    }

    void SetHardwareEnabled(bool enable)
    {
        m_allowHardware = enable;
    }

    // These are left public because it's test code and it's convenient to do so.
    int m_numD2DFactoryCreationCalls;
    CanvasDebugLevel m_debugLevel;

    int m_numD3dDeviceCreationCalls;
    CanvasHardwareAcceleration m_retrievableHarwareAcceleration;
};