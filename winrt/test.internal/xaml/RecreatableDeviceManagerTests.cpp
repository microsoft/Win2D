// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"

#include <RecreatableDeviceManager.impl.h>

#include "MockCanvasDeviceActivationFactory.h"

using Internal::AggregateType;

template<>
struct ITypedEventHandler<IInspectable*, CanvasCreateResourcesEventArgs*> 
    : ITypedEventHandler_impl<IInspectable*, AggregateType<CanvasCreateResourcesEventArgs*, ICanvasCreateResourcesEventArgs*>>
{
};

struct TestRecreatableDeviceManagerTraits
{
    typedef IInspectable control_t;
    typedef ITypedEventHandler<IInspectable*, CanvasCreateResourcesEventArgs*> createResourcesEventHandler_t;
};

typedef IRecreatableDeviceManager<TestRecreatableDeviceManagerTraits> ITestRecreatableDeviceManager;

static std::unique_ptr<ITestRecreatableDeviceManager> MakeRecreatableDeviceManager(IActivationFactory* factory)
{
    return std::make_unique<RecreatableDeviceManager<TestRecreatableDeviceManagerTraits>>(factory);
}

TEST_CLASS(RecreatableDeviceManagerTests)
{
public:
    typedef TestRecreatableDeviceManagerTraits::createResourcesEventHandler_t CreateResourcesHandler;

    struct Fixture
    {
        IInspectable* AnySender;
        ComPtr<MockCanvasDeviceActivationFactory> DeviceFactory;
        std::unique_ptr<ITestRecreatableDeviceManager> DeviceManager;

        CALL_COUNTER_WITH_MOCK(ChangedCallback, void(ChangeReason));
        CALL_COUNTER(RunWithDeviceFunction);

        Fixture()
            : AnySender(reinterpret_cast<IInspectable*>(0x1234))
            , DeviceFactory(Make<MockCanvasDeviceActivationFactory>())
            , DeviceManager(MakeRecreatableDeviceManager(DeviceFactory.Get()))
        {
            DeviceManager->SetChangedCallback(
                [=] (ChangeReason reason)
                {
                    ChangedCallback.WasCalled(reason);
                });

            RunWithDeviceFunction.AllowAnyCall();

            // AnySender is set to an arbitrary pointer since we don't
            // expect anything being tested here to dereference this pointer.
            // However, we do need to validate that the value is passed through
            // unchanged.
        }

        ComPtr<StubCanvasDevice> GetIntoStateWhereDeviceHasBeenCreated()
        {
            auto device = Make<StubCanvasDevice>();
            DeviceFactory->ExpectToActivateOne(device);
            CallRunWithDevice();
            return device;
        }

        void CallRunWithDevice(::RunWithDeviceFunction function)
        {
            this->RunWithDeviceFunction.SetExpectedCalls(1);

            DeviceManager->RunWithDevice(AnySender,
                [=](ICanvasDevice* device, RunWithDeviceFlags flags)
                {
                    this->RunWithDeviceFunction.WasCalled();
                    if (function)
                        function(device, flags);
                });
        }

        void CallRunWithDevice()
        {
            CallRunWithDevice(nullptr);
        }

        void CallRunWithDeviceExpectFlagsSet(ICanvasDevice* expectedDevice, RunWithDeviceFlags expectedFlags)
        {
            CallRunWithDevice(
                [=](ICanvasDevice* device, RunWithDeviceFlags flags)
                {
                    Assert::IsTrue(IsSameInstance(expectedDevice, device));
                    Assert::AreEqual(expectedFlags, flags & expectedFlags);
                });
        }

        void CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags expectedFlags)
        {
            CallRunWithDevice(
                [=](ICanvasDevice*, RunWithDeviceFlags flags)
                {
                    Assert::AreEqual(expectedFlags, flags & expectedFlags);
                });
        }

        void CallRunWithDeviceExpectFlagsNotSet(RunWithDeviceFlags expectedFlags)
        {
            CallRunWithDevice(
                [=](ICanvasDevice*, RunWithDeviceFlags flags)
                {
                    Assert::AreEqual(RunWithDeviceFlags::None, flags & expectedFlags);
                });
        }

        void CallRunWithDeviceExpectExactFlags(ICanvasDevice* expectedDevice, RunWithDeviceFlags expectedFlags)
        {
            CallRunWithDevice(
                [=](ICanvasDevice* device, RunWithDeviceFlags flags)
                {
                    Assert::IsTrue(IsSameInstance(expectedDevice, device));
                    Assert::AreEqual(expectedFlags, flags);
                });
        }        

        void CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags expectedFlags)
        {
            CallRunWithDevice(
                [=](ICanvasDevice*, RunWithDeviceFlags flags)
                {
                    Assert::AreEqual(expectedFlags, flags);
                });
        }        

        void CallRunWithDeviceDontExpectFunctionToBeCalled()
        {
            this->RunWithDeviceFunction.SetExpectedCalls(0);
            DeviceManager->RunWithDevice(AnySender,
                [=](ICanvasDevice*, RunWithDeviceFlags)
                {
                    this->RunWithDeviceFunction.WasCalled();
                });
        }

        void VerifyDeviceGetsRecreated()
        {
            auto secondDevice = Make<StubCanvasDevice>();
            DeviceFactory->ExpectToActivateOne(secondDevice);

            CallRunWithDeviceExpectFlagsSet(secondDevice.Get(), RunWithDeviceFlags::NewlyCreatedDevice);
        }

        void ExpectChangedCallback(ChangeReason expectedReason)
        {
            ChangedCallback.SetExpectedCalls(1,
                [=] (ChangeReason actualReason)
                {
                    Assert::AreEqual(expectedReason, actualReason);
                });
        }
    };

    //
    // Device creation
    //

    TEST_METHOD_EX(RecreatableDeviceManager_IsReadyToDraw_DefaultsToFalse)
    {
        Fixture f;
        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());
    }

    TEST_METHOD_EX(RecreatableDeviceManager_RunWithDevice_CreatesDeviceOnFirstCall_UsesDeviceOnSubsequentCalls)
    {
        Fixture f;

        auto anyDevice = Make<StubCanvasDevice>();

        f.DeviceFactory->ExpectToActivateOne(anyDevice);

        f.CallRunWithDeviceExpectFlagsSet(anyDevice.Get(), RunWithDeviceFlags::NewlyCreatedDevice);

        int anyNumberOfCalls = 10;
        for (int i = 0; i < anyNumberOfCalls; ++i)
        {
            f.CallRunWithDeviceExpectExactFlags(anyDevice.Get(), RunWithDeviceFlags::None);
        }
    }

    TEST_METHOD_EX(RecreatableDeviceManager_IsReadyToDraw_ReturnsTrueOnceDeviceHasBeenCreated)
    {
        Fixture f;

        f.DeviceFactory->ExpectToActivateOne();
        f.CallRunWithDevice();
        Assert::IsTrue(f.DeviceManager->IsReadyToDraw());
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceHasntBeenCreatedYet_GetDevice_ReturnsNull)
    {
        Fixture f;
        Assert::IsFalse(f.DeviceManager->GetDevice());
    }
    
    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceHasBeenCreated_GetDevice_ReturnsTheDevice)
    {
        Fixture f;
        
        auto anyDevice = Make<StubCanvasDevice>();
        f.DeviceFactory->ExpectToActivateOne(anyDevice);
        f.CallRunWithDevice();

        Assert::IsTrue(IsSameInstance(anyDevice.Get(), f.DeviceManager->GetDevice().Get()));
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsNotLostAndRunWithDeviceFunctionThrows_ThenExceptionIsPropagatedAndDeviceIsNotRecreated)
    {
        VerifyRunWithDeviceErrorPropagatedAndDeviceNotRecreated(DXGI_ERROR_DEVICE_REMOVED);
        VerifyRunWithDeviceErrorPropagatedAndDeviceNotRecreated(E_FAIL);
        VerifyRunWithDeviceErrorPropagatedAndDeviceNotRecreated(E_UNEXPECTED);
    }

    void VerifyRunWithDeviceErrorPropagatedAndDeviceNotRecreated(HRESULT error)
    {
        Fixture f;

        f.DeviceFactory->ExpectToActivateOne();
        f.ChangedCallback.SetExpectedCalls(0);

        ExpectHResultException(error,
            [&]
            {
                f.CallRunWithDevice(
                    [=](ICanvasDevice*, RunWithDeviceFlags)
                    {
                        ThrowHR(error);
                    });
            });

        f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::None);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenRunWithDeviceFunctionThrowsDeviceLost_AndTheDeviceReallyIsLost_ThenDeviceIsRecreated)
    {
        Fixture f;

        auto deviceThatGetsLost = Make<StubCanvasDevice>();
        f.DeviceFactory->ExpectToActivateOne(deviceThatGetsLost);

        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [=](ICanvasDevice*, RunWithDeviceFlags)
            {
                deviceThatGetsLost->MarkAsLost();
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        f.VerifyDeviceGetsRecreated();
    }

    //
    // SynchronousCreateResources
    //

    struct FixtureWithCreateResources : public Fixture
    {
        MockEventHandler<CreateResourcesHandler> OnCreateResources1;
        MockEventHandler<CreateResourcesHandler> OnCreateResources2;

        EventRegistrationToken OnCreateResourcesToken1;
        EventRegistrationToken OnCreateResourcesToken2;

        FixtureWithCreateResources()
            : OnCreateResources1(L"OnCreateResources1")
            , OnCreateResources2(L"OnCreateResources2")
        {
        }

        void AddCreateResources1(int expectedCalls, HRESULT nextResult = S_OK)
        {
            OnCreateResources1.SetExpectedCalls(expectedCalls, nextResult);
            OnCreateResourcesToken1 = DeviceManager->AddCreateResources(AnySender, OnCreateResources1.Get());
        }

        void AddCreateResources1(std::function<HRESULT(IInspectable*, ICanvasCreateResourcesEventArgs*)> fn)
        {
            OnCreateResources1.SetExpectedCalls(1, fn);
            OnCreateResourcesToken1 = DeviceManager->AddCreateResources(AnySender, OnCreateResources1.Get());
        }

        void AddCreateResources2(int expectedCalls, HRESULT nextResult = S_OK)
        {
            OnCreateResources2.SetExpectedCalls(expectedCalls, nextResult);
            OnCreateResourcesToken2 = DeviceManager->AddCreateResources(AnySender, OnCreateResources2.Get());
        }
    };

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceCreated_CreateResourcesAreCalledBeforeRunWithDeviceFunction)
    {
        FixtureWithCreateResources f;
        f.DeviceFactory->ExpectToActivateOne();

        f.AddCreateResources1(1);
        f.AddCreateResources2(1);
 
        f.CallRunWithDevice(
            [&](ICanvasDevice*, RunWithDeviceFlags)
            {
                f.OnCreateResources1.Validate();
                f.OnCreateResources2.Validate();
            });
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenCreateResourcesRemoved_ItIsNotCalledWhenDeviceCreated)
    {
        FixtureWithCreateResources f;
        f.DeviceFactory->ExpectToActivateOne();

        f.AddCreateResources1(1);

        f.AddCreateResources2(0);
        f.DeviceManager->RemoveCreateResources(f.OnCreateResourcesToken2);

        f.CallRunWithDevice(
            [&](ICanvasDevice*, RunWithDeviceFlags)
            {
                f.OnCreateResources1.Validate();
                f.OnCreateResources2.Validate();
            });
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenRunWithDeviceCallsCreateResources_TheSenderIsPassedThrough)
    {
        Fixture f;
        f.DeviceFactory->ExpectToActivateOne();

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable* sender, ICanvasCreateResourcesEventArgs*)
            {
                Assert::AreEqual<IInspectable*>(f.AnySender, sender);
                return S_OK;
            });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());
        f.CallRunWithDevice();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAddCreateResourcesCalledAfterDeviceCreated_ThenOnlyTheNewlyAddedOneIsCalledImmediately)
    {
        FixtureWithCreateResources f;
        f.AddCreateResources1(1);
        f.GetIntoStateWhereDeviceHasBeenCreated();

        f.AddCreateResources2(1);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAddCreateResourcesCalledAfterDeviceCreated_TheSenderIsPassedThrough)
    {
        FixtureWithCreateResources f;
        f.GetIntoStateWhereDeviceHasBeenCreated();

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable* sender, ICanvasCreateResourcesEventArgs*)
            {
                Assert::AreEqual<IInspectable*>(f.AnySender, sender);
                return S_OK;
            });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsNotLostAndCreateResourcesThrows_ThenExceptionIsPropagatedAndRunWithDeviceFunctionIsNotCalled)
    {
        VerifyCreateResourcesErrorPropagatedAndRunWithDeviceFunctionIsNotCalled(DXGI_ERROR_DEVICE_REMOVED);
        VerifyCreateResourcesErrorPropagatedAndRunWithDeviceFunctionIsNotCalled(E_FAIL);
        VerifyCreateResourcesErrorPropagatedAndRunWithDeviceFunctionIsNotCalled(E_UNEXPECTED);
    }
    
    void VerifyCreateResourcesErrorPropagatedAndRunWithDeviceFunctionIsNotCalled(HRESULT error)
    {
        FixtureWithCreateResources f;
        f.DeviceFactory->ExpectToActivateOne();

        f.AddCreateResources1(1, error);
        ExpectHResultException(error,
            [&] { f.CallRunWithDeviceDontExpectFunctionToBeCalled(); });

        // The next time round we should not create a new device, but we should
        // attempt to create resources again.
        f.OnCreateResources1.SetExpectedCalls(1);
        f.CallRunWithDeviceExpectFlagsNotSet(RunWithDeviceFlags::NewlyCreatedDevice);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsNotLostAndAddedCreateResourcesThrows_ThenExceptionIsPropagated)
    {
        VerifyAddedCreateResourcesErrorPropagated(DXGI_ERROR_DEVICE_REMOVED);
        VerifyAddedCreateResourcesErrorPropagated(E_FAIL);
        VerifyAddedCreateResourcesErrorPropagated(E_UNEXPECTED);
    }

    void VerifyAddedCreateResourcesErrorPropagated(HRESULT error)
    {
        FixtureWithCreateResources f;

        f.AddCreateResources1(1);
        auto device = f.GetIntoStateWhereDeviceHasBeenCreated();

        ExpectHResultException(error,
            [&]{ f.AddCreateResources2(1, error); });

        //
        // This handler should not actually have been added, so the next time
        // resources are created it shouldn't get called.
        //

        // To check this we first mark the device as lost:
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [=](ICanvasDevice*, RunWithDeviceFlags)
            {
                device->MarkAsLost();
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        // And then trigger device recreation
        f.DeviceFactory->ExpectToActivateOne();
        f.OnCreateResources1.SetExpectedCalls(1);
        f.OnCreateResources2.SetExpectedCalls(0);
        f.CallRunWithDevice();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenCreateResourcesThrowsDeviceLost_ThenDeviceIsRecreated)
    {
        FixtureWithCreateResources f;

        auto deviceThatGetsLost = Make<StubCanvasDevice>();
        f.DeviceFactory->ExpectToActivateOne(deviceThatGetsLost);

        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.AddCreateResources1(
            [=](IInspectable*, ICanvasCreateResourcesEventArgs*)
            {
                deviceThatGetsLost->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.CallRunWithDeviceDontExpectFunctionToBeCalled();

        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());

        f.OnCreateResources1.SetExpectedCalls(1);
        f.VerifyDeviceGetsRecreated();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAddedCreateResourcesThrowsDeviceLost_ThenDeviceIsRecreated)
    {
        FixtureWithCreateResources f;

        auto deviceThatGetsLost = Make<StubCanvasDevice>();
        f.DeviceFactory->ExpectToActivateOne(deviceThatGetsLost);

        f.CallRunWithDevice();

        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.AddCreateResources1(
            [=](IInspectable*, ICanvasCreateResourcesEventArgs*)
            {
                deviceThatGetsLost->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());

        f.OnCreateResources1.SetExpectedCalls(1);
        f.VerifyDeviceGetsRecreated();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenRunWithDeviceCallsCreateResources_TheReasonIsFirstTime)
    {
        FixtureWithCreateResources f;
        f.DeviceFactory->ExpectToActivateOne();

        f.AddCreateResources1(
            [](IInspectable*, ICanvasCreateResourcesEventArgs* args)
            {
                CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
                ThrowIfFailed(args->get_Reason(&reason));
                Assert::AreEqual(CanvasCreateResourcesReason::FirstTime, reason);
                return S_OK;
            });

        f.CallRunWithDevice();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAddCreateResourcesCalledAfterDeviceCreated_TheReasonIsFirstTime)
    {
        FixtureWithCreateResources f;
        f.GetIntoStateWhereDeviceHasBeenCreated();

        f.AddCreateResources1(
            [](IInspectable* sender, ICanvasCreateResourcesEventArgs* args)
            {
                CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
                ThrowIfFailed(args->get_Reason(&reason));
                Assert::AreEqual(CanvasCreateResourcesReason::FirstTime, reason);
                return S_OK;
            });
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenLostDeviceRecoveryCallsCreateResources_TheReasonIsNewDevice)
    {
        Fixture f;
        int createCount = 0;

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable* sender, ICanvasCreateResourcesEventArgs* args)
        {
            auto expectedReason = (createCount == 0) ? CanvasCreateResourcesReason::FirstTime : CanvasCreateResourcesReason::NewDevice;
            createCount++;

            CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
            ThrowIfFailed(args->get_Reason(&reason));
            Assert::AreEqual(expectedReason, reason);

            return S_OK;
        });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());

        // Create the first device.
        auto deviceThatGetsLost = Make<StubCanvasDevice>();
        f.DeviceFactory->ExpectToActivateOne(deviceThatGetsLost);
        Assert::AreEqual(0, createCount);
        f.CallRunWithDevice();
        Assert::AreEqual(1, createCount);

        // Cause the device to become lost.
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [=](ICanvasDevice*, RunWithDeviceFlags)
        {
            deviceThatGetsLost->MarkAsLost();
            ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
        });

        // Recover to a second device.
        f.DeviceFactory->ExpectToActivateOne();
        Assert::AreEqual(1, createCount);
        f.CallRunWithDevice();
        Assert::AreEqual(2, createCount);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_DpiChangeCallsCreateResources)
    {
        Fixture f;
        int createCount = 0;

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable* sender, ICanvasCreateResourcesEventArgs* args)
        {
            auto expectedReason = (createCount == 0) ? CanvasCreateResourcesReason::FirstTime : CanvasCreateResourcesReason::DpiChanged;
            createCount++;

            CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
            ThrowIfFailed(args->get_Reason(&reason));
            Assert::AreEqual(expectedReason, reason);

            return S_OK;
        });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());

        // Changing the DPI before initial device creation should be a no-op: we expect the first
        // CreateResources to be reported as FirstTime, subsuming the fact that DPI also changed.
        f.ExpectChangedCallback(ChangeReason::Other);

        f.DeviceManager->SetDpiChanged();

        // Create the first device.
        f.DeviceFactory->ExpectToActivateOne();
        Assert::AreEqual(0, createCount);
        f.CallRunWithDevice();
        Assert::AreEqual(1, createCount);

        // At this point there should be no pending DPI change state.
        f.CallRunWithDevice();
        Assert::AreEqual(1, createCount);

        // Change the DPI.
        f.ExpectChangedCallback(ChangeReason::Other);

        f.DeviceManager->SetDpiChanged();

        // This should not activate a new device, but should re-raise CreateResources with reason = DpiChanged.
        Assert::AreEqual(1, createCount);
        f.CallRunWithDevice();
        Assert::AreEqual(2, createCount);

        // At this point there should be no pending DPI change state.
        f.CallRunWithDevice();
        Assert::AreEqual(2, createCount);
    }

    //
    // Asynchronous CreateResources
    //

    struct FixtureWithCreateResourcesAsync : public Fixture
    {
        CALL_COUNTER(OnCreateResources);
        ComPtr<MockAsyncAction> Action;

        EventRegistrationToken token;

        FixtureWithCreateResourcesAsync()
            : token{}
        {
        }

        void SetCreateResourcesAsync()
        {
           auto onCreateResources = Callback<CreateResourcesHandler>(
               [=](IInspectable*, ICanvasCreateResourcesEventArgs* args)
               {
                   return ExceptionBoundary(
                       [=]
                       {
                           OnCreateResources.WasCalled();
                           Action = Make<MockAsyncAction>();
                           ThrowIfFailed(args->TrackAsyncAction(Action.Get()));
                       });
               });
           Assert::AreEqual(0LL, token.value);
           token = DeviceManager->AddCreateResources(AnySender, onCreateResources.Get());
        }

        ComPtr<StubCanvasDevice> CreateDeviceAndTriggerCreateResourcesAsync()
        {
            auto device = Make<StubCanvasDevice>();
            DeviceFactory->ExpectToActivateOne(device);
            SetCreateResourcesAsync();
            OnCreateResources.SetExpectedCalls(1);
            CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::ResourcesNotCreated);
            return device;
        }
    };
    
    TEST_METHOD_EX(RecreatableDeviceManager_WhenAsynchronousCreateResourcesHasNotCompleted_ResourceNotCreatedFlagIsSet)
    {
        FixtureWithCreateResourcesAsync f;

        f.CreateDeviceAndTriggerCreateResourcesAsync();

        for (int i = 0; i < 10; ++i)
        {
            f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::ResourcesNotCreated);
        }

        f.ExpectChangedCallback(ChangeReason::Other);

        f.Action->SetResult(S_OK);

        for (int i = 0; i < 10; ++i)
        {
            f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::None);
        }
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAsynchronousCreateResourcesHasNotCompleted_IsReadyToDrawReturnsFalse)
    {
        FixtureWithCreateResourcesAsync f;
        f.CreateDeviceAndTriggerCreateResourcesAsync();

        Assert::IsTrue(f.DeviceManager->GetDevice());
        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());

        f.ExpectChangedCallback(ChangeReason::Other);

        f.Action->SetResult(S_OK);

        Assert::IsTrue(f.DeviceManager->IsReadyToDraw());
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAsynchronousCreateResourcesIsSetAfterDeviceCreated_EverythingImmediatelyBehavesAsIfItIsWaitingForItToComplete)
    {
        FixtureWithCreateResourcesAsync f;
        f.GetIntoStateWhereDeviceHasBeenCreated();

        Assert::IsTrue(f.DeviceManager->IsReadyToDraw());

        f.OnCreateResources.SetExpectedCalls(1);
        f.SetCreateResourcesAsync();

        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());
        f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::ResourcesNotCreated);

        f.ExpectChangedCallback(ChangeReason::Other);

        f.Action->SetResult(S_OK);

        Assert::IsTrue(f.DeviceManager->IsReadyToDraw());

        f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::None);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsNotLostAndAsynchronousCreateResourcesActionThrows_ThenExceptionIsPropagatedFromRunWithDevice)
    {
        VerifyAsynchronousCreateResourcesActionErrorPropagatedFromRunWithDevice(DXGI_ERROR_DEVICE_REMOVED);
        VerifyAsynchronousCreateResourcesActionErrorPropagatedFromRunWithDevice(E_FAIL);
        VerifyAsynchronousCreateResourcesActionErrorPropagatedFromRunWithDevice(E_UNEXPECTED);
    }

    void VerifyAsynchronousCreateResourcesActionErrorPropagatedFromRunWithDevice(HRESULT error)
    {
        FixtureWithCreateResourcesAsync f;
        f.CreateDeviceAndTriggerCreateResourcesAsync();

        f.ExpectChangedCallback(ChangeReason::Other);

        auto errorText = L"Error Text";
        f.Action->SetResult(error, errorText);

        ValidateStoredErrorState(S_OK, nullptr);


        // Now call RunWithDevice
        ExpectHResultException(error,
            [&] { f.CallRunWithDeviceDontExpectFunctionToBeCalled(); });

        // check that the error text was propagated along with the error code
        ValidateStoredErrorState(error, errorText);

        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDevice();

        f.ExpectChangedCallback(ChangeReason::Other);

        f.Action->SetResult(S_OK);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAsynchronousCreateResourcesActionIsCanceled_ThenResourcesAreNotLoadedAndItIsReissued)
    {
        FixtureWithCreateResourcesAsync f;
        f.CreateDeviceAndTriggerCreateResourcesAsync();

        f.ExpectChangedCallback(ChangeReason::Other);
        ThrowIfFailed(f.Action->Cancel());
        f.Action->FireCompletion();

        // The action was canceled; the next RunWithDevice should report that
        // resources are not loaded and retrigger the async create resources
        
        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::ResourcesNotCreated);        
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsLostWhileExecutingAsynchronousCreateResourcesAction_ThenDeviceIsRecreated)
    {
        FixtureWithCreateResourcesAsync f; 

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();

        deviceThatGetsLost->MarkAsLost();
        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->SetResult(DXGI_ERROR_DEVICE_REMOVED);

        // This is where the device manager realizes that the device was lost.
        // The change callback is triggered - the device will be recreated on
        // the next RunWithDevice.
        f.ExpectChangedCallback(ChangeReason::DeviceLost);
        f.CallRunWithDeviceDontExpectFunctionToBeCalled();
        Assert::IsFalse(f.DeviceManager->IsReadyToDraw());

        f.OnCreateResources.SetExpectedCalls(1);
        f.DeviceFactory->ExpectToActivateOne();

        f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::NewlyCreatedDevice);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsLostWhileAsynchronousCreateResourcesActionExecuting_ThenActionIsCanceledAndDeviceIsNotRecreatedUntilActionCompletes)
    {
        FixtureWithCreateResourcesAsync f;

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();

        deviceThatGetsLost->MarkAsLost();
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [](ICanvasDevice*, RunWithDeviceFlags)
            {
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        AsyncStatus status;
        ThrowIfFailed(f.Action->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Canceled, status);

        f.DeviceFactory->ExpectToActivateOne();

        f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::ResourcesNotCreated);

        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->FireCompletion();
        
        f.OnCreateResources.SetExpectedCalls(1);

        f.CallRunWithDevice();        
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceIsLostAfterAsynchronousCreateResourcesHasSetItsResultButBeforeIsHasFiredCompletion_ThenDeviceIsOnlyRecreatedOnce)
    {
        FixtureWithCreateResourcesAsync f;

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();

        deviceThatGetsLost->MarkAsLost();
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [&](ICanvasDevice*, RunWithDeviceFlags)
            {
                f.Action->SetResultWithoutFireCompletion(DXGI_ERROR_DEVICE_REMOVED);
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        f.DeviceFactory->ExpectToActivateOne();

        f.CallRunWithDeviceExpectFlagsSet(RunWithDeviceFlags::NewlyCreatedDevice | RunWithDeviceFlags::ResourcesNotCreated);

        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->FireCompletion();
        
        // The next RunWithDevice call will pick up the device removed error
        // from the action that was just completed.  However, since we've
        // already recreated our device (and the new one hasn't been lost) we
        // won't get a changed callback.
        f.ChangedCallback.SetExpectedCalls(0);

        // It'll immediately attempt to recreate resources
        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::ResourcesNotCreated);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceLostWhileCreateResourcesRunning_GetDeviceReturnsTheLostDeviceUntilNextCreateResourcesStarted)
    {
        FixtureWithCreateResourcesAsync f;

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();
        deviceThatGetsLost->MarkAsLost();
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [](ICanvasDevice*, RunWithDeviceFlags)
            {
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        //
        // The device has been lost, but our create resources code is still
        // running.  This will query the device.  We want GetDevice() to
        // continue returning the same device for the duration of entire create
        // resources operation.
        //
        Assert::AreEqual<ICanvasDevice*>(deviceThatGetsLost.Get(), f.DeviceManager->GetDevice().Get());

        //
        // However, the callback to CallRunWithDevice needs to be passed a newly
        // created, valid, device (so that it can use that to render the clear
        // color).
        //
        f.DeviceFactory->ExpectToActivateOne();
        f.CallRunWithDevice(
            [&](ICanvasDevice* device, RunWithDeviceFlags flags)
            {
                Assert::AreEqual(RunWithDeviceFlags::NewlyCreatedDevice | RunWithDeviceFlags::ResourcesNotCreated, flags);
                Assert::IsFalse(IsSameInstance(deviceThatGetsLost.Get(), device));
                Assert::IsFalse(IsSameInstance(device, f.DeviceManager->GetDevice().Get()));
            });

        //
        // But the old device should still get reported by GetDevice()
        //
        Assert::AreEqual<ICanvasDevice*>(deviceThatGetsLost.Get(), f.DeviceManager->GetDevice().Get());

        //
        // Now the action has completed
        //
        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->FireCompletion();

        //
        // The device doesn't get updated until the next RunWithDevice
        //
        Assert::AreEqual<ICanvasDevice*>(deviceThatGetsLost.Get(), f.DeviceManager->GetDevice().Get());

        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDevice(
            [&](ICanvasDevice* device, RunWithDeviceFlags flags)
            {
                Assert::IsFalse(IsSameInstance(deviceThatGetsLost.Get(), device));
                Assert::IsTrue(IsSameInstance(device, f.DeviceManager->GetDevice().Get()));
                Assert::AreEqual(RunWithDeviceFlags::ResourcesNotCreated, flags);
            });
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceLostAfterCreateResourcesIsCompleteButBeforeResultIsProcessed_ThenResourcesAreRecreated)
    {
        FixtureWithCreateResourcesAsync f;

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();
        deviceThatGetsLost->MarkAsLost();
        f.ChangedCallback.SetExpectedCalls(2);

        f.CallRunWithDevice(
            [&](ICanvasDevice*, RunWithDeviceFlags)
            {
                f.Action->FireCompletion();
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        f.DeviceFactory->ExpectToActivateOne();
        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::NewlyCreatedDevice | RunWithDeviceFlags::ResourcesNotCreated);

        f.ChangedCallback.SetExpectedCalls(1);
        f.Action->FireCompletion();
        f.CallRunWithDeviceExpectExactFlags(RunWithDeviceFlags::None);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDeviceLostWhileCreateResourcesRunning_AddingCreateResourcesDoesNotInvokeItUntilPreviousOneCompletes)
    {
        FixtureWithCreateResourcesAsync f;

        auto deviceThatGetsLost = f.CreateDeviceAndTriggerCreateResourcesAsync();
        deviceThatGetsLost->MarkAsLost();
        f.ExpectChangedCallback(ChangeReason::DeviceLost);

        f.CallRunWithDevice(
            [](ICanvasDevice*, RunWithDeviceFlags)
            {
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            });

        //
        // We add this event handler, but we don't expect it to be called until
        // the the async action has completed and the device has been created.
        //
        MockEventHandler<CreateResourcesHandler> OnCreateResources(L"OnCreateResources");
        f.DeviceManager->AddCreateResources(f.AnySender, OnCreateResources.Get());

        f.DeviceFactory->ExpectToActivateOne();
        f.CallRunWithDevice();

        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->FireCompletion();

        // Now we expect OnCreateResources to get called
        OnCreateResources.SetExpectedCalls(1);
        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDevice();
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenAttemptingToTrackMultipleAsyncActions_AnExceptionIsThrown)
    {
        Fixture f;

        bool called = false;

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable*, ICanvasCreateResourcesEventArgs* args)
            {
                return ExceptionBoundary(
                    [&]
                    {
                        auto action1 = Make<MockAsyncAction>();
                        ThrowIfFailed(args->TrackAsyncAction(action1.Get()));

                        auto action2 = Make<MockAsyncAction>();
                        HRESULT hr = args->TrackAsyncAction(action2.Get());
                        Assert::AreEqual(hr, COR_E_NOTSUPPORTED);

                        ValidateStoredErrorState(COR_E_NOTSUPPORTED, Strings::MultipleAsyncCreateResourcesNotSupported);

                        called = true;

                        ThrowHR(hr);
                    });
            });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());

        f.DeviceFactory->ExpectToActivateOne();

        ExpectHResultException(COR_E_NOTSUPPORTED,
            [&] { f.CallRunWithDeviceDontExpectFunctionToBeCalled(); });

        Assert::IsTrue(called);
    }

    TEST_METHOD_EX(RecreatableDeviceManager_WhenDpiChangesDuringCreateResourcesAsync_ItIsDeferredUntilTheProperTime)
    {
        FixtureWithCreateResourcesAsync f;
        f.CreateDeviceAndTriggerCreateResourcesAsync();
        int createCount = 0;

        auto onCreateResources = Callback<CreateResourcesHandler>(
            [&](IInspectable* sender, ICanvasCreateResourcesEventArgs* args)
        {
            auto expectedReason = (createCount == 0) ? CanvasCreateResourcesReason::FirstTime : CanvasCreateResourcesReason::DpiChanged;
            createCount++;

            CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
            ThrowIfFailed(args->get_Reason(&reason));
            Assert::AreEqual(expectedReason, reason);

            return S_OK;
        });

        f.DeviceManager->AddCreateResources(f.AnySender, onCreateResources.Get());
        f.CallRunWithDevice();
        Assert::AreEqual(1, createCount);

        // Change the DPI.
        f.DeviceManager->SetDpiChanged();

        // This should not trigger CreateResources, because the previous async create is still running.
        f.CallRunWithDevice();
        Assert::AreEqual(1, createCount);

        // Complete the original async create.
        f.ExpectChangedCallback(ChangeReason::Other);
        f.Action->FireCompletion();
        Assert::AreEqual(1, createCount);

        // Now we expect the DPI change to be handled.
        f.OnCreateResources.SetExpectedCalls(1);
        f.CallRunWithDevice();
        Assert::AreEqual(2, createCount);
    }
};
