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

#include "BasicControlFixture.h"
#include "CanvasControlTestAdapter.h"
#include "ControlFixtures.h"
#include "..\MockCanvasSwapChain.h"
#include "MockRecreatableDeviceManager.h"

#define TEST_SHARED_CONTROL_BEHAVIOR(NAME)          \
    TEST_METHOD_EX(CanvasControl_##NAME)            \
    {                                               \
        NAME<CanvasControlTraits>();                \
    }                                               \
                                                    \
    TEST_METHOD_EX(CanvasAnimatedControl_##NAME)    \
    {                                               \
        NAME<CanvasAnimatedControlTraits>();        \
    }                                               \
                                                    \
    template<typename TRAITS>                       \
    void NAME()



TEST_CLASS(CanvasSharedControlTests_ClearColor)
{
    TEST_SHARED_CONTROL_BEHAVIOR(DefaultClearColorIsTransparentBlack)
    {
        ClearColorFixture<TRAITS> f;

        Color expectedColor{ 0, 0, 0, 0 };
        
        Color actualColor{ 0xFF, 0xFF, 0xFF, 0xFF };
        
        ThrowIfFailed(f.Control->get_ClearColor(&actualColor));
        
        Assert::AreEqual(expectedColor, actualColor);
    }

    TEST_SHARED_CONTROL_BEHAVIOR(ClearColorValueIsPersisted)
    {
        ClearColorFixture<TRAITS> f;
        
        Color anyColor{ 1, 2, 3, 4 };
        
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
        
        Color actualColor{ 0xFF, 0xFF, 0xFF, 0xFF };
        ThrowIfFailed(f.Control->get_ClearColor(&actualColor));

        Assert::AreEqual(anyColor, actualColor);
    }

    TEST_SHARED_CONTROL_BEHAVIOR(DeviceContextIsClearedToCorrectColorBeforeDrawHandlerIsCalled)
    {
        ClearColorFixture<TRAITS> f;
        f.RegisterOnDraw();

        Color anyColor{ 1, 2, 3, 4 };

        f.OnDraw.SetExpectedCalls(0);

        f.DeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
                f.OnDraw.SetExpectedCalls(1);
            });

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        f.RaiseLoadedEvent();
        f.RenderAnyNumberOfFrames();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(DeviceContextIsClearedToCorrectColorEvenIfNoDrawHandlersRegistered)
    {
        ClearColorFixture<TRAITS> f;

        Color anyColor{ 1, 2, 3, 4 };

        f.DeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
            });
        
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        f.RaiseLoadedEvent();
        f.RenderAnyNumberOfFrames();
    }

};

TEST_CLASS(CanvasSharedControlTests_InteractionWithRecreatableDeviceManager)
{
    template<class TRAITS>
    struct Fixture : public ControlFixture<TRAITS>
    {
        MockRecreatableDeviceManager<TRAITS>* DeviceManager;
        std::function<void()> ChangedCallback;

        Fixture()
            : DeviceManager(nullptr)
        {
            CreateAdapter();

            Adapter->CreateRecreatableDeviceManagerMethod.SetExpectedCalls(1,
                [=]
                {
                    Assert::IsNull(DeviceManager);
                    auto manager = std::make_unique<MockRecreatableDeviceManager<TRAITS>>();
                    manager->SetChangedCallbackMethod.SetExpectedCalls(1,
                        [=](std::function<void()> fn)
                        {
                            ChangedCallback = fn;
                        });

                    DeviceManager = manager.get();
                    return manager;
                });

            CreateControl();
        }
                
        void EnsureChangedCallback()
        {
            EnsureChangedCallbackImpl<TRAITS>();
        }

    private:

        template<typename T>
        void EnsureChangedCallbackImpl() {}

        template<>
        void EnsureChangedCallbackImpl<CanvasAnimatedControlTraits>()
        {
            Adapter->DoChanged();
        }
    };

    TEST_SHARED_CONTROL_BEHAVIOR(WhenSuspendingEventRaised_TrimCalledOnDevice)
    {
        Fixture<TRAITS> f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        anyDevice->TrimMethod.SetExpectedCalls(1);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenSuspendingEventRaisedAndThereIsNoDevice_NothingBadHappens)
    {
        Fixture<TRAITS> f;

        ComPtr<ICanvasDevice> nullDevice;
        f.DeviceManager->SetDevice(nullDevice);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenDpiChangedEventRaised_ForwardsToDeviceManager)
    {
        Fixture<TRAITS> f;

        // DPI change event without an actual change to the value should be ignored.
        f.Adapter->RaiseDpiChangedEvent();

        // But if the value changes, this should be passed on to the device manager.
        f.Adapter->LogicalDpi = 100;

        f.DeviceManager->SetDpiChangedMethod.SetExpectedCalls(1);
        f.Adapter->RaiseDpiChangedEvent();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(add_CreateResources_ForwardsToDeviceManager)
    {
        Fixture<TRAITS> f;

        EventRegistrationToken anyToken{0x1234567890ABCDEF};
        auto anyHandler = Callback<Fixture<TRAITS>::createResourcesEventHandler_t>(
            [](typename TRAITS::controlInterface_t*, IInspectable*) { return S_OK; } );

        f.DeviceManager->AddCreateResourcesMethod.SetExpectedCalls(1,
            [&](typename TRAITS::control_t* control, typename TRAITS::createResourcesEventHandler_t* handler)
            {
                Assert::IsTrue(IsSameInstance(f.Control.Get(), control));
                Assert::IsTrue(IsSameInstance(anyHandler.Get(), handler));
                return anyToken;
            });

        EventRegistrationToken actualToken;
        ThrowIfFailed(f.Control->add_CreateResources(anyHandler.Get(), &actualToken));

        Assert::AreEqual(anyToken.value, actualToken.value);
    }

    TEST_SHARED_CONTROL_BEHAVIOR(remove_CreateResources_ForwardsToDeviceManager)
    {
        Fixture<TRAITS> f;

        EventRegistrationToken anyToken{0x1234567890ABCDEF};
        f.DeviceManager->RemoveCreateResourcesMethod.SetExpectedCalls(1,
            [&](EventRegistrationToken token)
            {
                Assert::AreEqual(anyToken.value, token.value);
            });

        ThrowIfFailed(f.Control->remove_CreateResources(anyToken));
    }

    TEST_SHARED_CONTROL_BEHAVIOR(get_ReadyToDraw_ForwardsToDeviceManager)
    {
        Fixture<TRAITS> f;

        f.DeviceManager->IsReadyToDrawMethod.SetExpectedCalls(1,
            [] { return true; });

        boolean value;
        ThrowIfFailed(f.Control->get_ReadyToDraw(&value));
        Assert::IsTrue(!!value);

        f.DeviceManager->IsReadyToDrawMethod.SetExpectedCalls(1,
            [] { return false; });

        ThrowIfFailed(f.Control->get_ReadyToDraw(&value));
        Assert::IsFalse(!!value);
    }

    TEST_SHARED_CONTROL_BEHAVIOR(get_Device_ForwardsToDeviceManager)
    {
        Fixture<TRAITS> f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(f.Control->get_Device(&device));

        Assert::IsTrue(IsSameInstance(anyDevice.Get(), device.Get()));
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenGetDeviceReturnsNull_get_Device_ReportsUnderstandableErrorMessage)
    {
        Fixture<TRAITS> f;

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(E_INVALIDARG, f.Control->get_Device(&device));

        ValidateStoredErrorState(E_INVALIDARG, L"The control does not currently have a CanvasDevice associated with it. "
            L"Ensure that resources are created from a CreateResources or Draw event handler.");
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenDrawing_RunWithDrawIsPassedTheCorrectControl)
    {
        Fixture<TRAITS> f;

        f.DeviceManager->RunWithDeviceMethod.SetExpectedCalls(1,
            [&](typename TRAITS::control_t* control, RunWithDeviceFunction)
            {
                Assert::IsTrue(IsSameInstance(f.Control.Get(), control));
            });

        f.RaiseLoadedEvent();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
    }

    template<typename TRAITS>
    struct DrawFixture : public Fixture<TRAITS>
    {
        ComPtr<StubCanvasDevice> Device;
        MockEventHandler<typename TRAITS::drawEventHandler_t> OnDraw;

        DrawFixture()
            : Device(Make<StubCanvasDevice>())
            , OnDraw(L"OnDraw")
        {
            PrepareAdapter<TRAITS>();

            AddDrawHandler(OnDraw.Get());

            DeviceManager->SetDevice(Device);
        }

        template<typename T>
        void PrepareAdapter() {}

        template<>
        void PrepareAdapter<CanvasControlTraits>()
        {
            Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        }

        template<>
        void PrepareAdapter<CanvasAnimatedControlTraits>()
        {
            auto swapChainManager = std::make_shared<MockCanvasSwapChainManager>();

            Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
                [=] (ICanvasDevice*, float, float, float, CanvasAlphaMode)
                { 
                    auto mockSwapChain = swapChainManager->CreateMock(); 
                    mockSwapChain->CreateDrawingSessionMethod.AllowAnyCall(
                        [] (Color, ICanvasDrawingSession** value)
                        {
                            auto ds = Make<MockCanvasDrawingSession>();
                            return ds.CopyTo(value);
                        });
                    mockSwapChain->PresentMethod.AllowAnyCall();

                    mockSwapChain->put_TransformMethod.AllowAnyCall();

                    return mockSwapChain;
                });
        }

        void EnsureChangedCallback()
        {
            EnsureChangedCallbackImpl<TRAITS>();
        }

    private:

        template<typename T>
        void EnsureChangedCallbackImpl() {}

        template<>
        void EnsureChangedCallbackImpl<CanvasAnimatedControlTraits>()
        {
            Adapter->DoChanged();
        }
    };


    TEST_SHARED_CONTROL_BEHAVIOR(WhenResourcesAreNotCreated_DrawHandlersAreNotCalled)
    {
        DrawFixture<TRAITS> f;

        f.OnDraw.SetExpectedCalls(0);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::ResourcesNotCreated, 1);

        f.RaiseLoadedEvent();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenResourcesAreCreated_DrawHandlersAreCalled)
    {
        DrawFixture<TRAITS> f;

        f.OnDraw.SetExpectedCalls(1);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 1);

        f.RaiseLoadedEvent();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenChangedCallbackIsCalled_RedrawIsTriggered)
    {
        DrawFixture<TRAITS> f;
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None);

        f.OnDraw.SetExpectedCalls(1);

        f.RaiseLoadedEvent();
        f.EnsureChangedCallback();
        f.RenderAnyNumberOfFrames();

        f.ChangedCallback();
        f.EnsureChangedCallback();

        f.OnDraw.SetExpectedCalls(1);
        f.RenderAnyNumberOfFrames();        
    }
};

TEST_CLASS(CanvasSharedControlTests_CommonAdapter)
{
    TEST_SHARED_CONTROL_BEHAVIOR(DpiProperties)
    {
        BasicControlFixture<TRAITS> f;

        const float dpi = 144;

        f.CreateAdapter();

        f.Adapter->LogicalDpi = dpi;
        
        f.CreateControl();

        float actualDpi = 0;
        ThrowIfFailed(f.Control->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(f.Control->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(f.Control->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }
};
