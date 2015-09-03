// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

class CountedD2DDeviceContext : public MockD2DDeviceContext
{
    int* m_counter;
    
public:
    CountedD2DDeviceContext(int* counter)
        : m_counter(counter)
    {
        (*m_counter)++;
    }

    virtual ~CountedD2DDeviceContext() override
    {
        (*m_counter)--;
    }
};


TEST_CLASS(DeviceContextPoolUnitTests)
{
public:
    struct Fixture
    {
        ComPtr<MockD2DDevice> Device;
        DeviceContextPool Pool;

        CALL_COUNTER(CreateDeviceContextMethod);
        int NumberOfActiveDeviceContexts;

        Fixture()
            : Device(Make<MockD2DDevice>())
            , Pool(Device.Get())
            , NumberOfActiveDeviceContexts(0)
        {
            Device->MockCreateDeviceContext =
                [=] (D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** deviceContext)
                {
                    CreateDeviceContextMethod.WasCalled();

                    auto mockDeviceContext = Make<CountedD2DDeviceContext>(&NumberOfActiveDeviceContexts);
                    mockDeviceContext.CopyTo(deviceContext);
                };
        }

        void PopulatePool()
        {
            // Create many concurrent leases and then release them all.
            int const count = 100;

            CreateDeviceContextMethod.SetExpectedCalls(count);

            std::vector<DeviceContextLease> leases;

            for (int i = 0; i < count; ++i)
            {
                leases.push_back(Pool.TakeLease());
            }
        }
    };
    
    TEST_METHOD_EX(DeviceContextPool_Creation_DoesNotCreateContext)
    {
        Fixture f;
    }

    TEST_METHOD_EX(DeviceContextPool_WhenOnlyOneLeaseActiveAtATime_OnlyOneDeviceContextCreated)
    {
        Fixture f;

        f.CreateDeviceContextMethod.SetExpectedCalls(1);

        ID2D1DeviceContext1* lastRetrievedContext = nullptr;

        for (int i = 0; i < 1000; ++i)
        {
            auto lease = f.Pool.TakeLease();

            if (lastRetrievedContext)
            {
                Assert::AreEqual(lastRetrievedContext, lease.Get());
            }
            else
            {
                Assert::IsNotNull(lease.Get());
                lastRetrievedContext = lease.Get();
            }
        }
    }

    TEST_METHOD_EX(DeviceContextPool_DefaultConstructedDeviceContextLease_IsFine)
    {
        DeviceContextLease lease;
    }

    TEST_METHOD_EX(DeviceContextPool_LeasesCanBeMoveAssigned)
    {
        Fixture f;
        f.CreateDeviceContextMethod.SetExpectedCalls(1);

        auto lease1 = f.Pool.TakeLease();
        auto lease2 = std::move(lease1);

        Assert::IsNull(lease1.Get());
        Assert::IsNotNull(lease2.Get());
    }

    TEST_METHOD_EX(DeviceContextPool_LeasesCanBeMoveConstructed)
    {
        Fixture f;
        f.CreateDeviceContextMethod.SetExpectedCalls(1);

        auto lease1 = f.Pool.TakeLease();
        DeviceContextLease lease2(std::move(lease1));

        Assert::IsNull(lease1.Get());
        Assert::IsNotNull(lease2.Get());
    }

    TEST_METHOD_EX(DeviceContextPool_SimulataneousLeases_CreateMultipleContexts_ButFinalPoolShrinksToReasonableSize)
    {
        Fixture f;

        f.PopulatePool();
        
        Assert::AreEqual<int>(std::thread::hardware_concurrency(), f.NumberOfActiveDeviceContexts);
    }

    TEST_METHOD_EX(DeviceContextPool_WhenClosed_PoolIsEmptied)
    {
        Fixture f;

        f.PopulatePool();

        Assert::IsTrue(f.NumberOfActiveDeviceContexts > 0);

        f.Pool.Close();
        Assert::AreEqual(0, f.NumberOfActiveDeviceContexts);
    }

    TEST_METHOD_EX(DeviceContextPool_WhenClosed_AndLeaseIsReturned_DeviceContextIsDestroyed)
    {
        Fixture f;
        f.CreateDeviceContextMethod.SetExpectedCalls(1);

        {
            auto lease = f.Pool.TakeLease();
            
            f.Pool.Close();

            Assert::AreEqual(1, f.NumberOfActiveDeviceContexts);
        }

        Assert::AreEqual(0, f.NumberOfActiveDeviceContexts);
    }

    TEST_METHOD_EX(DeviceContextPool_WhenClosed_TakeLease_Fails)
    {
        Fixture f;
        f.Pool.Close();

        ExpectHResultException(RO_E_CLOSED, [&] { f.Pool.TakeLease(); });
    }
};
