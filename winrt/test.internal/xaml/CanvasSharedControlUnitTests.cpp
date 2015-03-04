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

        f.DeviceContext->ClearMethod.ExpectAtLeastOneCall(
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
                f.OnDraw.SetExpectedCalls(1);
            });

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        f.Load();
        f.RenderAnyNumberOfFrames();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(DeviceContextIsClearedToCorrectColorEvenIfNoDrawHandlersRegistered)
    {
        ClearColorFixture<TRAITS> f;

        Color anyColor{ 1, 2, 3, 4 };

        f.DeviceContext->ClearMethod.ExpectAtLeastOneCall(
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
            });
        
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        f.Load();
        f.RenderAnyNumberOfFrames();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(DeviceContextIsClearedToCorrectColorWhenColorChanges)
    {
        ClearColorFixture<TRAITS> f;

        Color anyColor{ 1, 2, 3, 4 };

        f.DeviceContext->ClearMethod.ExpectAtLeastOneCall(
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
            });
        
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        f.Load();
        f.RenderAnyNumberOfFrames();

        Color anyOtherColor{ 5, 6, 7, 8 };

        f.DeviceContext->ClearMethod.ExpectAtLeastOneCall(
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyOtherColor), *color);
            });

        ThrowIfFailed(f.Control->put_ClearColor(anyOtherColor));

        f.RenderAnyNumberOfFrames();
    }

};

TEST_CLASS(CanvasSharedControlTests_InteractionWithRecreatableDeviceManager)
{
    TEST_SHARED_CONTROL_BEHAVIOR(WhenDpiChangedEventRaised_ForwardsToDeviceManager)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;
        f.Load();

        // DPI change event without an actual change to the value should be ignored.
        f.Adapter->RaiseDpiChangedEvent();

        // But if the value changes, this should be passed on to the device manager.
        f.Adapter->LogicalDpi = 100;

        f.DeviceManager->SetDpiChangedMethod.SetExpectedCalls(1);
        f.Adapter->RaiseDpiChangedEvent();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenControlNotLoaded_DpiChangedEventIsIgnored)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        f.DeviceManager->SetDpiChangedMethod.SetExpectedCalls(0);

        // DPI change events on a new, not yet loaded control should be ignored.
        f.Adapter->LogicalDpi = 100;
        f.Adapter->RaiseDpiChangedEvent();

        f.Adapter->LogicalDpi = 96;
        f.Adapter->RaiseDpiChangedEvent();

        // Ditto after loading and then unloading the control again.
        f.Load();
        f.RaiseUnloadedEvent();

        f.Adapter->LogicalDpi = 100;
        f.Adapter->RaiseDpiChangedEvent();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenDpiChangesWhileControlNotLoaded_LoadRaisesDpiChangedEvent)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        // DPI changes, but is ignored because the control is not yet loaded.
        f.Adapter->LogicalDpi = 100;
        f.Adapter->RaiseDpiChangedEvent();

        // When the control loads, the DPI change shoudl be picked up.
        f.DeviceManager->SetDpiChangedMethod.SetExpectedCalls(1);
        f.Load();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(add_CreateResources_ForwardsToDeviceManager)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        EventRegistrationToken anyToken{0x1234567890ABCDEF};
        auto anyHandler = Callback<ControlFixtureWithRecreatableDeviceManager<TRAITS>::createResourcesEventHandler_t>(
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
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

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
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

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
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(f.Control->get_Device(&device));

        Assert::IsTrue(IsSameInstance(anyDevice.Get(), device.Get()));
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenGetDeviceReturnsNull_get_Device_ReportsUnderstandableErrorMessage)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(E_INVALIDARG, f.Control->get_Device(&device));

        ValidateStoredErrorState(E_INVALIDARG, L"The control does not currently have a CanvasDevice associated with it. "
            L"Ensure that resources are created from a CreateResources or Draw event handler.");
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenDrawing_RunWithDrawIsPassedTheCorrectControl)
    {
        ControlFixtureWithRecreatableDeviceManager<TRAITS> f;

        f.DeviceManager->RunWithDeviceMethod.SetExpectedCalls(1,
            [&](typename TRAITS::control_t* control, RunWithDeviceFunction)
            {
                Assert::IsTrue(IsSameInstance(f.Control.Get(), control));
            });

        f.Load();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
    }

    template<typename TRAITS>
    struct DrawFixture : public ControlFixtureWithRecreatableDeviceManager<TRAITS>
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

        f.Load();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(WhenResourcesAreCreated_DrawHandlersAreCalled)
    {
        DrawFixture<TRAITS> f;

        f.OnDraw.SetExpectedCalls(1);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 1);

        f.Load();
        f.EnsureChangedCallback();
        f.RenderSingleFrame();
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

    TEST_SHARED_CONTROL_BEHAVIOR(WhenLoadedAndUnloaded_EventsAreRegisteredAndUnregistered)
    {
        BasicControlFixture<TRAITS> f;

        f.CreateAdapter();

        // Creating the control should not register events.
        f.Adapter->DpiChangedEventSource->AddMethod.SetExpectedCalls(0);
        f.Adapter->SuspendingEventSource->AddMethod.SetExpectedCalls(0);
        f.Adapter->ResumingEventSource->AddMethod.SetExpectedCalls(0);

        f.Adapter->DpiChangedEventSource->RemoveMethod.SetExpectedCalls(0);
        f.Adapter->SuspendingEventSource->RemoveMethod.SetExpectedCalls(0);
        f.Adapter->ResumingEventSource->RemoveMethod.SetExpectedCalls(0);

        f.CreateControl();

        // Loading the control should register events.
        f.Adapter->DpiChangedEventSource->AddMethod.SetExpectedCalls(1);
        f.Adapter->SuspendingEventSource->AddMethod.SetExpectedCalls(1);
        f.Adapter->ResumingEventSource->AddMethod.SetExpectedCalls(1);

        f.Load();

        Expectations::Instance()->Validate();

        // Unloading the control should unregister events.
        f.Adapter->DpiChangedEventSource->RemoveMethod.SetExpectedCalls(1);
        f.Adapter->SuspendingEventSource->RemoveMethod.SetExpectedCalls(1);
        f.Adapter->ResumingEventSource->RemoveMethod.SetExpectedCalls(1);

        f.RaiseUnloadedEvent();
    }

    TEST_SHARED_CONTROL_BEHAVIOR(SizeProperty)
    {
        BasicControlFixture<TRAITS> f;

        f.CreateAdapter();
        f.CreateControl();

        Size size{ -1, -1 };
        ThrowIfFailed(f.Control->get_Size(&size));
        Assert::AreEqual(Size{ 0, 0 }, size);

        Size newSize{ 123, 456 };
        f.UserControl->Resize(newSize);

        ThrowIfFailed(f.Control->get_Size(&size));
        Assert::AreEqual(newSize, size);
    }
};
