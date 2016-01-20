// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class GetBoundsFixture
{
    std::shared_ptr<TestDeviceAdapter> m_deviceAdapter;
    ComPtr<MockD2DDevice> m_d2dDevice;
    ComPtr<CanvasDevice> m_device;

public:
    GetBoundsFixture()
        : m_deviceAdapter(std::make_shared<TestDeviceAdapter>())
    {
        CanvasDeviceAdapter::SetInstance(m_deviceAdapter);
        m_d2dDevice = Make<MockD2DDevice>();
        m_device = Make<CanvasDevice>(m_d2dDevice.Get());
    }

    virtual ~GetBoundsFixture() = default;

    void TestGetBoundsPassingDrawingSession()
    {
        TestGetBounds(true);
    }

    void TestGetBoundsPassingDevice()
    {
        TestGetBounds(false);
    }

    void TestGetBounds(bool useDrawingSession)
    {
        // The call to GetBounds will result in the device context's
        // transform being set to a particular value and then restored to
        // its original value.
        //
        // The way the device context is chosen depends on whether GetBounds
        // is passed CanvasDrawingSession or not.  This test primes a single
        // device context that is either wrapped in a CanvasDrawingSession
        // or returned by D2DDevice::CreateDeviceContext.
            
        auto deviceContext = Make<MockD2DDeviceContext>();
        deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(m_d2dDevice);

        D2D1_MATRIX_3X2_F originalTransform{ 1, 2, 3, 4, 5, 6 };
        deviceContext->GetTransformMethod.SetExpectedCalls(1,
            [=](D2D1_MATRIX_3X2_F* matrix)
            {
                *matrix = originalTransform;
            });

        D2D1_MATRIX_3X2_F transform{ 7, 8, 9, 10, 11, 12 };
        D2D1_RECT_F bounds{ 1, 2, 3, 4 };
        deviceContext->SetTransformMethod.SetExpectedCalls(1,
            [=](D2D1_MATRIX_3X2_F const* matrix)
            {
                Assert::AreEqual(transform, *matrix);

                ExpectGetBounds(deviceContext.Get(), bounds,
                    [=]
                    {
                        deviceContext->SetTransformMethod.SetExpectedCalls(1,
                            [&](D2D1_MATRIX_3X2_F const* matrix2)
                            {
                                Assert::AreEqual(originalTransform, *matrix2);
                            });
                    });
            });

        ComPtr<ICanvasResourceCreator> resourceCreator;
        CallCounter<> createDeviceContextCallCounter(L"CreateDeviceContext");
        
        if (useDrawingSession)
        {
            resourceCreator = Make<CanvasDrawingSession>(deviceContext.Get());
        }
        else
        {
            createDeviceContextCallCounter.SetExpectedCalls(1);
            m_d2dDevice->MockCreateDeviceContext =
                [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** returnedDeviceContext)
                {
                    createDeviceContextCallCounter.WasCalled();
                    *returnedDeviceContext = deviceContext.Get();
                };
            resourceCreator = m_device;
        }

        Rect actualBounds;
        CallGetBounds(resourceCreator.Get(), *reinterpret_cast<Matrix3x2*>(&transform), &actualBounds);

        Assert::AreEqual(FromD2DRect(bounds), actualBounds);
    }

protected:
    virtual void ExpectGetBounds(MockD2DDeviceContext*, D2D1_RECT_F, std::function<void()>) = 0;
    virtual void CallGetBounds(ICanvasResourceCreator*, Matrix3x2, Rect*) = 0;

    ICanvasDevice* GetDevice()
    {
        return m_device.Get();
    }
};

