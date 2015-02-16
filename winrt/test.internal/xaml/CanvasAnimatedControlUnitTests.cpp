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

#include <CanvasAnimatedControl.h>

#include "CanvasControlTestAdapter.h"
#include "BasicControlFixture.h"
#include "MockCanvasSwapChain.h"

static Color const AnyColor                 {   1,   2,   3,   4 };
static Color const AnyOtherColor            {   5,   6,   7,   8 };
static Color const AnyOpaqueColor           { 255,   2,   3,   4 };
static Color const AnyOtherOpaqueColor      { 255,   5,   6,   7 };
static Color const AnyTranslucentColor      { 254, 253, 252, 251 };
static Color const AnyOtherTranslucentColor { 250, 249, 248, 247 };

TEST_CLASS(CanvasAnimatedControl_DrawArgs)
{
    struct Fixture
    {
        ComPtr<ICanvasAnimatedDrawEventArgs> DrawArgs;
        ComPtr<MockCanvasDrawingSession> DrawingSession;
        CanvasTimingInformation TimingInformation;

        Fixture()
            : TimingInformation(CanvasTimingInformation{ 1, 2, 3, true })
            , DrawingSession(Make<MockCanvasDrawingSession>())
        {
            auto drawArgsFactory = Make<CanvasAnimatedDrawEventArgsFactory>();

            ThrowIfFailed(drawArgsFactory->Create(DrawingSession.Get(), TimingInformation, &DrawArgs));
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_DrawEventArgs_Null)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.DrawArgs->get_Timing(nullptr));

        Assert::AreEqual(E_INVALIDARG, f.DrawArgs->get_DrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DrawEventArgs_CorrectValuesReturned)
    {
        Fixture f;

        CanvasTimingInformation timingInformation;
        ThrowIfFailed(f.DrawArgs->get_Timing(&timingInformation));
        Assert::AreEqual(f.TimingInformation, timingInformation);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.DrawArgs->get_DrawingSession(&drawingSession));
        Assert::AreEqual(static_cast<ICanvasDrawingSession*>(f.DrawingSession.Get()), drawingSession.Get());
    }
};

TEST_CLASS(CanvasAnimatedControl_UpdateArgs)
{
    struct Fixture
    {
        ComPtr<ICanvasAnimatedUpdateEventArgs> UpdateArgs;
        CanvasTimingInformation TimingInformation;

        Fixture()
            : TimingInformation(CanvasTimingInformation{ 1, 2, 3, true })
        {
            auto updateArgsFactory = Make<CanvasAnimatedUpdateEventArgsFactory>();

            ThrowIfFailed(updateArgsFactory->Create(TimingInformation, &UpdateArgs));
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_UpdateEventArgs_Null)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.UpdateArgs->get_Timing(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_UpdateEventArgs_CorrectValueReturned)
    {
        Fixture f;

        CanvasTimingInformation timingInformation;
        ThrowIfFailed(f.UpdateArgs->get_Timing(&timingInformation));
        Assert::AreEqual(f.TimingInformation, timingInformation);
    }
};

TEST_CLASS(CanvasAnimatedControlTests)
{
    TEST_METHOD_EX(CanvasAnimatedControl_Implements_Expected_Interfaces)
    {
        CanvasAnimatedControlFixture f;
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ICanvasAnimatedControl);
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ABI::Windows::UI::Xaml::Controls::IUserControl);
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ICanvasResourceCreator);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DeviceProperty_Null)
    {
        CanvasAnimatedControlFixture f;
        Assert::AreEqual(E_INVALIDARG, f.Control->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_InitialState_NotPaused)
    {
        boolean paused;
        
        CanvasAnimatedControlFixture f;
        ThrowIfFailed(f.Control->get_Paused(&paused));
        Assert::IsFalse(!!paused);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RedundantChangedCallsDoNothing)
    {
        CanvasAnimatedControlFixture f;
        f.Load();
        f.Adapter->DoChanged();

        auto onDraw = MockEventHandler<Animated_DrawEventHandler>(L"Draw", ExpectedEventParams::Both);
        f.AddDrawHandler(onDraw.Get());
        onDraw.AllowAnyCall();
        f.Adapter->Tick(); // Puts adapter in steady state.

        onDraw.SetExpectedCalls(1); // Ensure the draw handler gets called one time, not three.

        for (int i = 0; i < 3; ++i)
        {
            ThrowIfFailed(f.Control->put_Paused(true));
            ThrowIfFailed(f.Control->put_Paused(false)); // Internally calls Changed().
        }

        //
        // Verifies that multiple calls to Changed() don't somehow queue anything up, or
        // corrupt anything, and only one draw handler gets called on Tick.
        //
        f.Adapter->DoChanged();
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_IsFixedTimeStep_DefaultsToTrue)
    {
        CanvasAnimatedControlFixture f;

        boolean initialFixedTimeStep;
        ThrowIfFailed(f.Control->get_IsFixedTimeStep(&initialFixedTimeStep));

        Assert::IsTrue(!!initialFixedTimeStep);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_IsFixedTimeStep_ValueIsPersisted)
    {
        CanvasAnimatedControlFixture f;

        ThrowIfFailed(f.Control->put_IsFixedTimeStep(FALSE));

        boolean timeStepVerify;
        ThrowIfFailed(f.Control->get_IsFixedTimeStep(&timeStepVerify));
        Assert::IsFalse(!!timeStepVerify);

        ThrowIfFailed(f.Control->put_IsFixedTimeStep(TRUE));

        ThrowIfFailed(f.Control->get_IsFixedTimeStep(&timeStepVerify));
        Assert::IsTrue(!!timeStepVerify);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_TargetElapsedTime_DefaultsTo60FPS)
    {
        CanvasAnimatedControlFixture f;

        TimeSpan t;
        ThrowIfFailed(f.Control->get_TargetElapsedTime(&t));

        double seconds = StepTimer::TicksToSeconds(t.Duration);
        Assert::AreEqual(1.0 / 60.0, seconds, 0.0001);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_TargetElapsedTime_ValueIsPersisted)
    {
        CanvasAnimatedControlFixture f;

        TimeSpan t{123};
        ThrowIfFailed(f.Control->put_TargetElapsedTime(t));

        TimeSpan retrieved{};
        ThrowIfFailed(f.Control->get_TargetElapsedTime(&retrieved));
        
        Assert::AreEqual(t.Duration, retrieved.Duration);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ResourcesCreatedOnLoad)
    {
        CanvasAnimatedControlFixture f;

        auto onCreateResources = MockEventHandler<Animated_CreateResourcesEventHandler>(L"CreateResources", ExpectedEventParams::Both);
        f.AddCreateResourcesHandler(onCreateResources.Get());
        onCreateResources.SetExpectedCalls(1);

        f.Load();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_NoCreateResources_IfChangedCompletes_BeforeLoaded)
    {
        CanvasAnimatedControlFixture f;

        auto onCreateResources = MockEventHandler<Animated_CreateResourcesEventHandler>(L"CreateResources", ExpectedEventParams::Both);
        f.AddCreateResourcesHandler(onCreateResources.Get());
        onCreateResources.SetExpectedCalls(0);

        f.Adapter->DoChanged();

        f.Load();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_put_TargetElapsedTime_MustBePositiveNonZero)
    {
        CanvasAnimatedControlFixture f;

        TimeSpan zero{ 0 };
        Assert::AreEqual(E_INVALIDARG, f.Control->put_TargetElapsedTime(zero));

        TimeSpan neg{ -1 };
        Assert::AreEqual(E_INVALIDARG, f.Control->put_TargetElapsedTime(neg));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RecreatedSwapChainHasCorrectAlphaMode)
    {
        CanvasAnimatedControlFixture f;
        auto swapChainManager = f.Adapter->SwapChainManager;
        f.Load();
        f.Adapter->DoChanged();
        
        f.Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1, 
            [swapChainManager](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasAlphaMode::Ignore, alphaMode);
                return CanvasAnimatedControlFixture::CreateTestSwapChain(swapChainManager, device);
            });

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyOpaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        f.Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1,
            [swapChainManager](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasAlphaMode::Premultiplied, alphaMode);
                return CanvasAnimatedControlFixture::CreateTestSwapChain(swapChainManager, device);
            });

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyTranslucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RedundantClearColorChangesDoNotCauseRecreation)
    {
        CanvasAnimatedControlFixture f;
        auto swapChainManager = f.Adapter->SwapChainManager;
        f.Load();
        f.Adapter->DoChanged();
        
        f.ExpectOneCreateSwapChain();

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyOpaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyOtherOpaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        f.ExpectOneCreateSwapChain();

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyTranslucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyOtherTranslucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ChangingClearColor_DoesNotRestartUpdateRenderLoop)
    {
        CanvasAnimatedControlFixture f;
        f.Load();
        f.Adapter->DoChanged();

        f.Adapter->Tick();
        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());
        f.Adapter->DoChanged();

        ThrowIfFailed(f.Control->put_ClearColor(AnyColor));
        f.Adapter->Tick();
        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());
        f.Adapter->DoChanged();       
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ThreadingRestrictions)
    {
        CanvasAnimatedControlFixture f;
        boolean b = false;
        Color color = {};
        TimeSpan t{ 123 };
        MockEventHandler<Animated_DrawEventHandler> onDraw;
        MockEventHandler<Animated_UpdateEventHandler> onUpdate;
        MockEventHandler<Animated_CreateResourcesEventHandler> onCreateResources;
        EventRegistrationToken token;

        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->add_CreateResources(onCreateResources.Get(), &token));
        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->remove_CreateResources(token));

        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->get_ReadyToDraw(&b));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->add_Draw(onDraw.Get(), &token));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->remove_Draw(token));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->add_Update(onUpdate.Get(), &token));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->remove_Update(token));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->put_TargetElapsedTime(t));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->get_TargetElapsedTime(&t));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->put_IsFixedTimeStep(b));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->get_IsFixedTimeStep(&b));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->put_Paused(b));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->get_Paused(&b));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->ResetElapsedTime());

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->put_ClearColor(color));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->get_ClearColor(&color));
    }

    class FixtureWithSwapChainAccess : public CanvasAnimatedControlFixture
    {
    protected:
        ComPtr<MockDxgiSwapChain> m_dxgiSwapChain;

    public:
        FixtureWithSwapChainAccess()
            : m_dxgiSwapChain(Make<MockDxgiSwapChain>())
        {
            m_dxgiSwapChain->Present1Method.AllowAnyCall();

            m_dxgiSwapChain->GetDesc1Method.AllowAnyCall(
                [=](DXGI_SWAP_CHAIN_DESC1* desc)
                {
                    desc->Width = 1;
                    desc->Height = 1;
                    desc->Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                    desc->BufferCount = 2;
                    desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                    return S_OK;
                });

            m_dxgiSwapChain->GetBufferMethod.AllowAnyCall(
                [=](UINT index, const IID& iid, void** out)
                {
                    Assert::AreEqual(__uuidof(IDXGISurface2), iid);
                    auto surface = Make<MockDxgiSurface>();

                    return surface.CopyTo(reinterpret_cast<IDXGISurface2**>(out));
                });

            Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
                [=](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
                {
                    StubCanvasDevice* stubDevice = static_cast<StubCanvasDevice*>(device); // Ensured by test construction

                    stubDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
                    {
                        return m_dxgiSwapChain;
                    });

                    auto canvasSwapChain = Adapter->SwapChainManager->GetOrCreate(device, m_dxgiSwapChain.Get(), DEFAULT_DPI);

                    return canvasSwapChain;
                });  
        }
    };

    class ResizeFixture : public FixtureWithSwapChainAccess
    {
    public:
        ResizeFixture()
        {
            Load();
            Adapter->DoChanged();
        }

        void ExpectOneResizeBuffers(Size size)
        {
            m_dxgiSwapChain->ResizeBuffersMethod.SetExpectedCalls(1,
                [=](UINT bufferCount,
                    UINT width,
                    UINT height,
                    DXGI_FORMAT newFormat,
                    UINT swapChainFlags)
                    {
                        Assert::AreEqual(2u, bufferCount);
                        Assert::AreEqual(static_cast<UINT>(size.Width), width);
                        Assert::AreEqual(static_cast<UINT>(size.Height), height);
                        Assert::AreEqual(DXGI_FORMAT_B8G8R8A8_UNORM, newFormat);
                        Assert::AreEqual(0u, swapChainFlags);
                        return S_OK;
                    });
        }

        void Execute(Size size)
        {
            UserControl->Resize(size);

            for (int i = 0; i < 5; ++i)
            {
                Adapter->Tick();
                Assert::IsTrue(Adapter->IsUpdateRenderLoopActive());
                Adapter->DoChanged();
            }

            Expectations::Instance()->Validate();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResized_ThenResizeBuffersIsCalled)
    {
        Size sizes[] 
        { 
            { ResizeFixture::InitialWidth + 1, ResizeFixture::InitialHeight     }, // width only
            { ResizeFixture::InitialWidth,     ResizeFixture::InitialHeight + 1 }, // height only
            { ResizeFixture::InitialWidth + 1, ResizeFixture::InitialHeight + 1 }, // both
        };

        for (auto size : sizes)
        {
            ResizeFixture f;
            f.ExpectOneResizeBuffers(size);
            f.Execute(size);
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResizedToCurrentSize_ThenResizeBuffersIsNotCalled)
    {
        ResizeFixture f;
        f.Execute(Size{ ResizeFixture::InitialWidth, ResizeFixture::InitialHeight });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResizedToZero_ThenSwapChainIsSetToNull)
    {
        ResizeFixture f;

        // Resizing to zero should stop the update/render loop.
        f.Adapter->GetSwapChainPanel()->SetSwapChainMethod.SetExpectedCalls(0);
        f.UserControl->Resize(Size{ 0, 0 });
        f.Adapter->Tick();
        Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());

        // The UI thread change handler should then restart the update/render loop.
        f.Adapter->GetSwapChainPanel()->SetSwapChainMethod.SetExpectedCalls(1, [](IDXGISwapChain* swapChain)
            {
                Assert::IsNull(swapChain);
                return S_OK;
            });

        f.Adapter->DoChanged();
        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());

        // Should now be in a steady state where there are no more calls to ResizeBuffers or SetSwapChain.
        f.Execute(Size{ 0, 0 });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResizedToZero_ThenDrawIsNotCalled)
    {
        ResizeFixture f;

        f.UserControl->Resize(Size{ 0, 0 });
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        // Should now be in a steady state where Update is called but not Draw.
        MockEventHandler<Animated_UpdateEventHandler> onUpdate(L"OnUpdate");
        MockEventHandler<Animated_DrawEventHandler> onDraw(L"OnDraw");

        f.AddUpdateHandler(onUpdate.Get());
        f.AddDrawHandler(onDraw.Get());

        onUpdate.ExpectAtLeastOneCall();
        onDraw.SetExpectedCalls(0);

        f.Adapter->ProgressTime(TicksPerFrame);
        f.Execute(Size{ 0, 0 });
    }

    class DeviceLostFixture : public FixtureWithSwapChainAccess
    {
    public:
        MockEventHandler<Animated_DrawEventHandler> OnDraw;
        MockEventHandler<Animated_CreateResourcesEventHandler> OnCreateResources;
        MockEventHandler<Animated_UpdateEventHandler> OnUpdate;

        DeviceLostFixture()
            : OnDraw(L"OnDraw")
            , OnCreateResources(L"OnCreateResources")
            , OnUpdate(L"OnUpdate")
        {
            // Use variable timestep so Update gets called on each tick
            ThrowIfFailed(Control->put_IsFixedTimeStep(FALSE));

            AddCreateResourcesHandler(OnCreateResources.Get());
            AddUpdateHandler(OnUpdate.Get());
            AddDrawHandler(OnDraw.Get());

            Load();

            OnCreateResources.AllowAnyCall();
            OnUpdate.AllowAnyCall();
            OnDraw.AllowAnyCall();
        }

        void LoseDeviceDuringPresent()
        {
            m_dxgiSwapChain->Present1Method.SetExpectedCalls(1,
                [=](UINT, UINT, const DXGI_PRESENT_PARAMETERS*)
                {
                    MarkDeviceAsLost();
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }

        void PresentReportsDeviceLost()
        {
            m_dxgiSwapChain->Present1Method.SetExpectedCalls(1,
                [=](UINT, UINT, const DXGI_PRESENT_PARAMETERS*)
                {
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }

        void ExpectAtLeastOnePresent()
        {
            m_dxgiSwapChain->Present1Method.ExpectAtLeastOneCall();
        }

        void MarkDeviceAsLost()
        {
            Adapter->InitialDevice->MarkAsLost();
        }

        void DoChangedAndTick()
        {
            Adapter->DoChanged();
            Adapter->Tick();
        }

        void VerifyDeviceRecovered()
        {
            DoChangedAndTick(); // Device lost first noticed

            OnCreateResources.SetExpectedCalls(1);
            OnUpdate.ExpectAtLeastOneCall();
            OnDraw.ExpectAtLeastOneCall();
            ExpectAtLeastOnePresent();

            // We need two changed/ticks to reliably have recreated the device
            // depending on whether the UI thread or the render thread spotted
            // the device lost.  In the render thread case this extra iteration
            // is required in order to marshal the exception to the UI thread.
            DoChangedAndTick();
            DoChangedAndTick();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceIsLostOnFirstPresent_DeviceIsRecreated)
    {
        DeviceLostFixture f;

        f.LoseDeviceDuringPresent();
        f.VerifyDeviceRecovered();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceIsLostDuringSubsequentPresent_DeviceIsRecreated)
    {
        DeviceLostFixture f;

        for (int i = 0; i < 5; ++i)
            f.DoChangedAndTick();

        f.LoseDeviceDuringPresent();
        f.VerifyDeviceRecovered();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenPresentReportsDeviceLost_ButDeviceIsntLost_ExceptionPropagates)
    {
        DeviceLostFixture f;

        f.PresentReportsDeviceLost();
        f.DoChangedAndTick();

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [&] { f.Adapter->DoChanged(); });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceIsLostDuringCreateResources_DeviceIsRecreated)
    {
        DeviceLostFixture f;
        
        f.OnCreateResources.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, IInspectable*)
            {
                f.MarkDeviceAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.VerifyDeviceRecovered();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenCreateResourcesReportsDeviceLost_ButDeviceIsntLost_ExceptionPropagates)
    {
        DeviceLostFixture f;

        f.OnCreateResources.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, IInspectable*)
            {
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [&] { f.Adapter->DoChanged(); });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceIsLostDuringDraw_DeviceIsRecreated)
    {
        DeviceLostFixture f;

        f.OnDraw.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*)
            {
                f.MarkDeviceAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.VerifyDeviceRecovered();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDrawReportsDeviceLost_ButDeviceIsntLost_ExceptionPropagates)
    {
        DeviceLostFixture f;

        f.OnDraw.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*)
            {
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.DoChangedAndTick();

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [&] { f.Adapter->DoChanged(); });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceIsLostDuringUpdate_DeviceIsRecreated)
    {
        DeviceLostFixture f;

        f.OnUpdate.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs*)
            {
                f.MarkDeviceAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.VerifyDeviceRecovered();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenUpdateReportsDeviceLost_ButDeviceIsntLost_ExceptionPropagates)
    {
        DeviceLostFixture f;

        f.OnUpdate.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs*)
            {
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.DoChangedAndTick();

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [&] { f.Adapter->DoChanged(); });
    }

    static auto const TicksPerFrame = StepTimer::TicksPerSecond / 60;

    struct UpdateRenderFixture : public CanvasAnimatedControlFixture
    {
        MockEventHandler<Animated_CreateResourcesEventHandler> OnCreateResources;
        MockEventHandler<Animated_DrawEventHandler> OnDraw;
        MockEventHandler<Animated_UpdateEventHandler> OnUpdate;

        UpdateRenderFixture()
            : OnCreateResources(L"OnCreateResources")
            , OnDraw(L"OnDraw")
            , OnUpdate(L"OnUpdate")
        {
            AddCreateResourcesHandler(OnCreateResources.Get());
            AddDrawHandler(OnDraw.Get());
            AddUpdateHandler(OnUpdate.Get());

            Adapter->GetSwapChainPanel()->SetSwapChainMethod.AllowAnyCall(
                [=] (IDXGISwapChain* swapChain)
                {
                    auto dxgiSwapChain = dynamic_cast<MockDxgiSwapChain*>(swapChain);
                    Assert::IsNotNull(dxgiSwapChain);
                    dxgiSwapChain->ResizeBuffersMethod.AllowAnyCall();
                    return S_OK;
                });
        }

        void GetIntoSteadyState()
        {
            Load();
            OnCreateResources.SetExpectedCalls(1);
            Adapter->DoChanged();
            OnUpdate.SetExpectedCalls(1);
            OnDraw.SetExpectedCalls(1);
            RenderSingleFrame();
            
            Expectations::Instance()->Validate();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_FirstRenderFrameAlwaysIssuesSingleUpdateAndDraw)
    {
        CheckFirstRender(TRUE);
        CheckFirstRender(FALSE);

        // (Cases here expanded out so that we can look at call stack to
        // see which case failed)
    }

    void CheckFirstRender(boolean fixedTimeStep)
    {
        CheckFirstRender(fixedTimeStep, 0);
        CheckFirstRender(fixedTimeStep, TicksPerFrame / 2);
        CheckFirstRender(fixedTimeStep, TicksPerFrame);
        CheckFirstRender(fixedTimeStep, TicksPerFrame * 100);
    }

    void CheckFirstRender(boolean fixedTimeStep, int64_t initialTime)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(fixedTimeStep));

        f.OnCreateResources.SetExpectedCalls(1);
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        f.Adapter->ProgressTime(initialTime);

        f.Load();
        f.Adapter->DoChanged();
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_VariableTimestepFiresInitialUpdateAtZeroTime)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(false));

        f.OnCreateResources.SetExpectedCalls(1);
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        f.Adapter->ProgressTime(0);

        f.Load();
        f.Adapter->DoChanged();
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenNotEnoughTimeHasElapsed_UpdateAndRenderNotCalled)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();
        
        f.Adapter->ProgressTime(TicksPerFrame / 2);
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenSingleFrameElapsed_UpdateAndRenderCalledOnce)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();

        f.Adapter->ProgressTime(TicksPerFrame);
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenMultipleFramesElapsed_UpdateCalledMultipleTimesAndRenderCalledOnce)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();

        int frames = 2;

        f.Adapter->ProgressTime(TicksPerFrame * frames);
        f.OnUpdate.SetExpectedCalls(frames);
        f.OnDraw.SetExpectedCalls(1);
        f.RenderSingleFrame();
    }

    //
    // We don't exhaustively test the update/draw behavior here since we're not
    // trying to test StepTimer. This is a more superficial test to validate
    // that CanvasAnimatedControl is hooked up to StepTimer.
    //

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenInitiallyPaused_UpdatesAndDrawsAreNotCalled)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_Paused(TRUE));
        f.Load();
        
        f.OnCreateResources.SetExpectedCalls(1); // even when paused we still create resources
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenPausedAfterUpdate_UpdatesAndDrawsAreNotCalled)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();
        
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenPausedAfterUpdateAndClearColorChanged_DrawIsCalled)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_ClearColor(AnyColor));
        f.GetIntoSteadyState();

        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.Adapter->ProgressTime(TicksPerFrame);
        f.Adapter->DoChanged();
        f.RenderSingleFrame();

        ThrowIfFailed(f.Control->put_ClearColor(AnyOtherColor));
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenPausedAfterUpdateAndSizeChanged_DrawIsCalled)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();

        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.Adapter->ProgressTime(TicksPerFrame);
        f.Adapter->DoChanged();
        f.RenderSingleFrame();

        f.UserControl->Resize(Size{123, 456});
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceLost_DrawIsNotCalledUntilUpdateHasCompleted)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(false)); // ensure that update handlers are called on each tick
        ThrowIfFailed(f.Control->put_ClearColor(AnyColor));

        f.GetIntoSteadyState();

        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.Adapter->ProgressTime(TicksPerFrame);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        // Trigger a draw by changing the clear color, but this will report a
        // lost device
        f.OnDraw.SetExpectedCalls(1,
            [] (ICanvasAnimatedControl* sender, ICanvasAnimatedDrawEventArgs*)
            {
                ComPtr<ICanvasDevice> device;
                Assert::AreEqual(S_OK, As<ICanvasResourceCreator>(sender)->get_Device(&device));

                auto stubDevice = dynamic_cast<StubCanvasDevice*>(device.Get());
                Assert::IsNotNull(stubDevice);

                stubDevice->MarkAsLost();

                return DXGI_ERROR_DEVICE_REMOVED;
            });

        ThrowIfFailed(f.Control->put_ClearColor(AnyOtherColor));
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        //
        // At this point the device has been lost.  We don't expect any draw
        // calls until the asynchronous create resources has completed.
        //
        f.OnDraw.SetExpectedCalls(0);

        auto action = Make<MockAsyncAction>();
        f.OnCreateResources.SetExpectedCalls(1,
            [=] (IInspectable*, ICanvasCreateResourcesEventArgs* args)
            {
                return args->TrackAsyncAction(action.Get());
            });

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        f.OnCreateResources.SetExpectedCalls(0);

        //
        // Event after resources have been created we still don't expect any
        // draw calls since the control is paused and the draw handlers mustn't
        // be called until there's been at least one update since the last
        // CreateResources.
        //
        action->SetResult(S_OK); // this completes the CreateResources

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        //
        // Changing the clear color would normally force the draw handlers to be
        // called.  However, since there still hasn't been an update the draw
        // handlers are not called.
        //
        ThrowIfFailed(f.Control->put_ClearColor(AnyColor));

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        //
        // Now when the control is unpaused we expect an update/render.
        //
        ThrowIfFailed(f.Control->put_Paused(FALSE));

        for (int i = 0; i < 10; ++i)
        {
            f.OnUpdate.SetExpectedCalls(1);
            f.OnDraw.SetExpectedCalls(1);
            
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenUnpausedAfterBeingPausedForMultipleFrames_UpdateAndDrawIsCalledOnceOnly)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();
        
        f.Adapter->ProgressTime(TicksPerFrame * 1000);
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));
        f.Adapter->DoChanged();
        f.RenderSingleFrame();

        f.Adapter->ProgressTime(TicksPerFrame * 1000);
        ThrowIfFailed(f.Control->put_Paused(FALSE));
        f.Adapter->DoChanged();

        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);
        f.RenderSingleFrame();
    }


    struct NoExtraUpdateFixture : public UpdateRenderFixture
    {
        void DontExpectAnUpdateOrDrawUntilTargetElapsedTimeHasBeenReached(int64_t ticks = TicksPerFrame)
        {
            OnUpdate.SetExpectedCalls(0);
            OnDraw.SetExpectedCalls(0);

            for (int i = 0; i < 10; ++i)
            {
                Adapter->DoChanged();
                Adapter->Tick();
            }

            Adapter->ProgressTime(ticks);

            OnUpdate.SetExpectedCalls(1);
            OnDraw.SetExpectedCalls(1);
        
            for (int i = 0; i < 10; ++i)
            {
                Adapter->DoChanged();
                Adapter->Tick();
            }
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenSizeChangedBeforeTargetElapsedTimeReached_DrawIsCalledOnce_AndThen_UpdateAndDrawAreNotCalled)
    {
        NoExtraUpdateFixture f;

        ThrowIfFailed(f.Control->put_Paused(FALSE));

        f.GetIntoSteadyState();

        f.UserControl->Resize(Size{123, 456});
        
        // After resizing we allow one extra draw without an update - consider
        // the case of a very long TargetElapsedTime, we want size changes to be
        // reflected asap. (Also this simplifies handling the case when we're
        // paused...)
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        f.DontExpectAnUpdateOrDrawUntilTargetElapsedTimeHasBeenReached();        
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenClearColorChangedBeforeTargetElapsedTimeReached_DrawIsCalledOnce_AndThen_UpdateAndDrawAreNotCalled)
    {
        NoExtraUpdateFixture f;

        ThrowIfFailed(f.Control->put_ClearColor(AnyOpaqueColor));
        ThrowIfFailed(f.Control->put_Paused(FALSE));
        
        f.GetIntoSteadyState();

        ThrowIfFailed(f.Control->put_ClearColor(AnyOtherOpaqueColor));

        // After the changing the clear color we allow one extra draw without an
        // update - consider the case of a very long TargetElapsedTime, we want
        // clear color changes to be reflected asap.
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        f.DontExpectAnUpdateOrDrawUntilTargetElapsedTimeHasBeenReached();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenClearColorChangedFromTransparentToOpaqueBeforeTargetElapsedTimeReached_TimerIsNotReset)
    {
        NoExtraUpdateFixture f;

        ThrowIfFailed(f.Control->put_ClearColor(AnyTranslucentColor));
        ThrowIfFailed(f.Control->put_Paused(FALSE));
        
        f.GetIntoSteadyState();

        ThrowIfFailed(f.Control->put_ClearColor(AnyOpaqueColor));

        // Move time forwards by only half a frame
        f.Adapter->ProgressTime(TicksPerFrame / 2);

        // Next changed/tick will cause the update/render loop to exit.
        f.Adapter->DoChanged();
        f.Adapter->Tick();
        Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());

        // After the changing the clear color we allow one extra draw without an
        // update - consider the case of a very long TargetElapsedTime, we want
        // clear color changes to be reflected asap.
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        // The update should happen after just another half frame has passed.
        // If the timer had been reset when the update/render loop was started
        // then the update wouldn't happen since it would think that there had
        // only been half a frame since the reset.
        f.DontExpectAnUpdateOrDrawUntilTargetElapsedTimeHasBeenReached(TicksPerFrame / 2);
    }

    class ElapsedTicksFixture : public CanvasAnimatedControlFixture
    {
        MockEventHandler<Animated_DrawEventHandler> m_onDraw;
        MockEventHandler<Animated_UpdateEventHandler> m_onUpdate;
        int64_t m_expectedElapsedTicks;

    public:

        ElapsedTicksFixture()
            : m_onDraw(L"OnDraw")
            , m_onUpdate(L"OnUpdate")
            , m_expectedElapsedTicks(0)
        {
            m_onDraw.AllowAnyCall(
                [=](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs* eventArgs)
                {
                    CanvasTimingInformation timingInformation;
                    ThrowIfFailed(eventArgs->get_Timing(&timingInformation));
                    Assert::AreEqual(m_expectedElapsedTicks, timingInformation.ElapsedTime.Duration);
                    return S_OK;
                });

            m_onUpdate.AllowAnyCall(
                [=](ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs* eventArgs)
                {
                    CanvasTimingInformation timingInformation;
                    ThrowIfFailed(eventArgs->get_Timing(&timingInformation));
                    Assert::AreEqual(m_expectedElapsedTicks, timingInformation.ElapsedTime.Duration);
                    return S_OK;
                });
        }

        void GetIntoSteadyState()
        {
            Load();
            Adapter->DoChanged();
            RenderSingleFrame();
        }

        void SetExpectedElapsedTime(int64_t t)
        {
            m_expectedElapsedTicks = t;
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_ResetElapsedTime_ResetsTheElapsedTime)
    {
        ElapsedTicksFixture f;
        f.GetIntoSteadyState();

        f.Adapter->ProgressTime(TicksPerFrame);

        f.SetExpectedElapsedTime(static_cast<int64_t>(TicksPerFrame));
        f.Adapter->Tick();

        ThrowIfFailed(f.Control->ResetElapsedTime());
        f.SetExpectedElapsedTime(static_cast<int64_t>(0));
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ResetElapsedTime_Statefulness)
    {
        ElapsedTicksFixture f;
        f.GetIntoSteadyState();

        f.Adapter->ProgressTime(TicksPerFrame);

        ThrowIfFailed(f.Control->ResetElapsedTime());
        f.SetExpectedElapsedTime(static_cast<int64_t>(0));
        f.Adapter->Tick();


        f.Adapter->ProgressTime(TicksPerFrame);
        f.SetExpectedElapsedTime(static_cast<int64_t>(TicksPerFrame));
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_OnFirstUpdate_UpdateCount_Is_1)
    {
        UpdateRenderFixture f;
        f.Load();

        f.OnCreateResources.SetExpectedCalls(1);
        f.OnUpdate.SetExpectedCalls(1,
            [&] (ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs* args)
            {
                CanvasTimingInformation timingInformation;
                Assert::AreEqual(S_OK, args->get_Timing(&timingInformation));
                Assert::AreEqual(1LL, timingInformation.UpdateCount);
                return S_OK;
            });
        f.OnDraw.SetExpectedCalls(1);

        f.Adapter->DoChanged();
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedStep_WhilePaused_UpdateCountIncrements)
    {
        WhilePaused_UpdateCountIncrements(true);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_VariableStep_WhilePaused_UpdateCountIncrements)
    {
        WhilePaused_UpdateCountIncrements(false);
    }

    void WhilePaused_UpdateCountIncrements(bool isFixedTimeStep)
    {
        UpdateRenderFixture f;
        f.OnCreateResources.SetExpectedCalls(1);
        f.OnDraw.AllowAnyCall();
        f.Load();

        ThrowIfFailed(f.Control->put_IsFixedTimeStep(isFixedTimeStep));
        

        int64_t expectedUpdateCount = 1;

        for (int i = 0; i < 10; ++i)
        {
            ThrowIfFailed(f.Control->put_Paused(FALSE));
            f.OnUpdate.SetExpectedCalls(1,
                [&] (ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs* args)
                {
                    CanvasTimingInformation timingInformation;
                    Assert::AreEqual(S_OK, args->get_Timing(&timingInformation));
                    Assert::AreEqual(expectedUpdateCount, timingInformation.UpdateCount);
                    ThrowIfFailed(f.Control->put_Paused(TRUE));
                    return S_OK;
                });
            f.Adapter->DoChanged();
            f.Adapter->Tick();
            ++expectedUpdateCount;
        }        
    }

    class TimingFixture : public CanvasAnimatedControlFixture
    {
        MockEventHandler<Animated_DrawEventHandler> m_onDraw;
        MockEventHandler<Animated_UpdateEventHandler> m_onUpdate;
        INT64 m_updateCount;
        INT64 m_totalTime;
        bool m_isFixedTimestep;

        struct ExpectedTiming
        {
            INT64 UpdateCount;
            INT64 TotalTime;
            INT64 ElapsedTime;
            boolean IsRunningSlowly;
        };

        std::queue<ExpectedTiming> m_expectedOnDraw;
        std::queue<ExpectedTiming> m_expectedOnUpdate;

    public:

        TimingFixture(bool isFixedTimestep)
            : m_onDraw(L"OnDraw")
            , m_onUpdate(L"OnUpdate")
            , m_updateCount(1)
            , m_totalTime(0)
            , m_isFixedTimestep(isFixedTimestep)
        {
            m_onDraw.AllowAnyCall(
                [=](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs* eventArgs)
                {
                    CanvasTimingInformation timingInformation;
                    ThrowIfFailed(eventArgs->get_Timing(&timingInformation));
                    AssertExpectedTiming(m_expectedOnDraw, timingInformation);
                    return S_OK;
                });

            m_onUpdate.AllowAnyCall(
                [=](ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs* eventArgs)
                {
                    CanvasTimingInformation timingInformation;
                    ThrowIfFailed(eventArgs->get_Timing(&timingInformation));
                    AssertExpectedTiming(m_expectedOnUpdate, timingInformation);
                    return S_OK;
                });

            AddDrawHandler(m_onDraw.Get());
            AddUpdateHandler(m_onUpdate.Get());

            ExpectedTiming initialUpdate{ m_updateCount, 0, 0, false };
            m_expectedOnDraw.push(initialUpdate);
            m_expectedOnUpdate.push(initialUpdate);

            ThrowIfFailed(Control->put_IsFixedTimeStep(m_isFixedTimestep));
        }

        void GetIntoSteadyState()
        {
            Load();
            Adapter->DoChanged();
            RenderSingleFrame();
        }

        void ProgressTime(int64_t ticks)
        {
            const INT64 targetTime = m_totalTime + ticks;

            if (m_isFixedTimestep)
            {
                const INT64 delta = targetTime - m_totalTime;
                const INT64 numUpdates = delta / TicksPerFrame;

                for (INT64 i = 0; i < numUpdates; ++i)
                {
                    m_updateCount++;

                    bool isRunningSlowly;
                    INT64 timeSinceLastUpdate;
                    if (i < numUpdates - 1)
                    {
                        timeSinceLastUpdate = TicksPerFrame;
                        m_totalTime += TicksPerFrame;
                        isRunningSlowly = true;
                    }
                    else
                    {
                        timeSinceLastUpdate = targetTime - m_totalTime;
                        m_totalTime = targetTime;
                        isRunningSlowly = false;
                    }

                    ExpectedTiming expected{ m_updateCount, m_totalTime, timeSinceLastUpdate, isRunningSlowly };

                    m_expectedOnUpdate.push(expected);

                    if (i == numUpdates - 1)
                        m_expectedOnDraw.push(expected);
                }
            }
            else
            {
                m_updateCount++;

                const INT64 timeSinceLastUpdate = targetTime - m_totalTime;
                m_totalTime = targetTime;

                ExpectedTiming expected{ m_updateCount, m_totalTime, timeSinceLastUpdate, false };
                m_expectedOnUpdate.push(expected);
                m_expectedOnDraw.push(expected);
            }

            // Necessary to always assign this, for the case where it's fixed timestep and no update was sent.
            m_totalTime = targetTime;

            Adapter->ProgressTime(ticks);
        }

    private:

        void AssertExpectedTiming(
            std::queue<ExpectedTiming>& notifications,
            CanvasTimingInformation const& timingInformation)
        {
            assert(notifications.size() > 0);
            const ExpectedTiming expected = notifications.front();
            notifications.pop();

            Assert::AreEqual(expected.UpdateCount, timingInformation.UpdateCount, L"UpdateCount");
            Assert::AreEqual(expected.TotalTime, timingInformation.TotalTime.Duration, L"TotalTime");
            Assert::AreEqual(expected.ElapsedTime, timingInformation.ElapsedTime.Duration, L"ElapsedTime");
            Assert::AreEqual(expected.IsRunningSlowly, timingInformation.IsRunningSlowly, L"IsRunningSlowly");
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_CallbacksNotifiedOfCorrectTimestamp)
    {
        TimingFixture f(true);
        f.GetIntoSteadyState();

        for (int i = 0; i < 3; ++i)
        {
            f.ProgressTime(TicksPerFrame);
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_RunningSlowly)
    {
        TimingFixture f(true);
        f.GetIntoSteadyState();

        f.ProgressTime(TicksPerFrame * 3);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_VariableTimeStep_CallbacksNotifiedOfCorrectTimestamp)
    {
        TimingFixture f(false);
        f.GetIntoSteadyState();

        for (int i = 0; i < 3; ++i)
        {
            f.ProgressTime(TicksPerFrame);
            f.RenderSingleFrame();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_VariableTimeStep_SlowFrame)
    {
        TimingFixture f(false);
        f.GetIntoSteadyState();

        f.ProgressTime(TicksPerFrame * 3);
        f.RenderSingleFrame();
    }
};

TEST_CLASS(CanvasAnimatedControlChangedAction)
{
    struct Fixture : public BasicControlFixture<CanvasAnimatedControlTraits>
    {
        Fixture()
        {
            CreateAdapter();
            CreateControl();
            
            auto swapChainManager = std::make_shared<MockCanvasSwapChainManager>();

            Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
                [=] (ICanvasDevice*, float, float, float, CanvasAlphaMode)
                {
                    return swapChainManager->CreateMock();;
                });
        }

        void GetIntoSteadyState()
        {
            Load();
            Adapter->DoChanged();
        }

        bool IsChangedActionRunning()
        {
            return Adapter->HasPendingChangedActions();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_OnConstruction_NoPendingChangedAction)
    {
        Fixture f;
        Assert::IsFalse(f.IsChangedActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_LoadedCausesChanged)
    {
        Fixture f;
        f.Load();
        Assert::IsTrue(f.IsChangedActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_UnpausedCausesChanged)
    {
        Fixture f;
        f.GetIntoSteadyState();

        ThrowIfFailed(f.Control->put_Paused(true));
        ThrowIfFailed(f.Control->put_Paused(false));
        Assert::IsTrue(f.IsChangedActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_AlphaModeCausesChanged)
    {
        Fixture f;
        f.GetIntoSteadyState();

        Assert::IsFalse(f.IsChangedActionRunning());

        ThrowIfFailed(f.Control->put_ClearColor(AnyOpaqueColor));

        Assert::IsTrue(f.IsChangedActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ResizeCausesChanged)
    {
        Fixture f;
        f.GetIntoSteadyState();

        Assert::IsFalse(f.IsChangedActionRunning());

        f.UserControl->Resize(Size{ static_cast<float>(22), static_cast<float>(33) });

        Assert::IsTrue(f.IsChangedActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DestroyedWhileChangedIsPending)
    {
        Fixture f;
        f.Load();
        Assert::IsTrue(f.IsChangedActionRunning());
        f.Control.Reset();

        Assert::IsTrue(f.IsChangedActionRunning());

        f.Adapter->DoChanged();
        Assert::IsFalse(f.IsChangedActionRunning());
    }
};

TEST_CLASS(CanvasAnimatedControlRenderLoop)
{
    struct Fixture : public BasicControlFixture<CanvasAnimatedControlTraits>
    {
        ComPtr<MockCanvasSwapChain> SwapChain;

        Fixture()
        {
            auto swapChainManager = std::make_shared<MockCanvasSwapChainManager>();
            SwapChain = swapChainManager->CreateMock();

            SwapChain->put_TransformMethod.AllowAnyCall();

            CreateAdapter();

            Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
                [=] (ICanvasDevice*, float, float, float, CanvasAlphaMode)
                {
                    return SwapChain;
                });

            CreateControl();
        }

        void ExpectRender()
        {
            SwapChain->PresentMethod.SetExpectedCalls(1);
            ExpectDrawingSession();
        }

        void AllowAnyRendering()
        {
            SwapChain->PresentMethod.AllowAnyCall();

            SwapChain->CreateDrawingSessionMethod.SetExpectedCalls(1,
                [=](Color clearColor, ICanvasDrawingSession** value)
                {
                    return Make<MockCanvasDrawingSession>().CopyTo(value);
                });
        }

        void ExpectDrawingSession()
        {
            Color expectedClearColor;
            ThrowIfFailed(Control->get_ClearColor(&expectedClearColor));

            SwapChain->CreateDrawingSessionMethod.SetExpectedCalls(1,
                [=] (Color clearColor, ICanvasDrawingSession** value)
                {
                    Assert::AreEqual(expectedClearColor, clearColor);
                    return Make<MockCanvasDrawingSession>().CopyTo(value);
                });
        }

        void ClearCanceledActions()
        {
            if (Adapter->m_outstandingWorkItemAsyncAction)
            {
                AsyncStatus status = (AsyncStatus)-1;
                Adapter->m_outstandingWorkItemAsyncAction->get_Status(&status);
                if (status == AsyncStatus::Canceled)
                {
                    Adapter->m_outstandingWorkItemAsyncAction.Reset();
                    Adapter->m_currentTickFn = nullptr;
                }
            }
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_OnConstruction_UpdateRenderLoopIsNotRunning)
    {
        Fixture f;
        Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenLoaded_SingleFrameIsPresented)
    {
        Fixture f;
        
        f.Load();
        f.Adapter->DoChanged();

        f.ExpectRender();
        f.RenderSingleFrame();

        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenLoadedWhilePaused_SingleFrameIsPresented_AndThen_UpdateRenderLoopIsNotRunning)
    {
        Fixture f;
        ThrowIfFailed(f.Control->put_Paused(TRUE));
        f.Adapter->DoChanged();
        
        f.Load();
        f.Adapter->DoChanged();

        f.ExpectRender();
        f.RenderSingleFrame();

        Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());        

        // Confirm that the update/render loop doesn't get restarted
        for (int i = 0; i < 5; ++i)
        {
            f.Adapter->DoChanged();
            Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());        
            f.RenderSingleFrame();
            Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());        
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DestroyedWhileRenderLoopIsPending)
    {
        Fixture f;

        f.Load();
        f.Adapter->DoChanged();

        f.AllowAnyRendering();
        f.RenderSingleFrame();

        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());

        f.Control.Reset();

        Assert::IsTrue(f.Adapter->IsUpdateRenderLoopActive());

        f.RaiseUnloadedEvent();

        f.ClearCanceledActions();

        Assert::IsFalse(f.Adapter->IsUpdateRenderLoopActive());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenBackgroundModeChanges_UpdateRenderLoopKeepsRunning_Race0)
    {
        WhenBackgroundModeChanges(0);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenBackgroundModeChanges_UpdateRenderLoopKeepsRunning_Race1)
    {
        WhenBackgroundModeChanges(1);
    }

    void WhenBackgroundModeChanges(int race)
    {
        Fixture f;
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(FALSE)); // ensure update/draw is always called
        ThrowIfFailed(f.Control->put_ClearColor(AnyTranslucentColor));
        f.Load();

        for (int i = 0; i < 5; ++i)
        {
            f.ExpectRender();
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        ThrowIfFailed(f.Control->put_ClearColor(AnyOpaqueColor));

        // After the put_ClearColor then either Tick or DoChanged could happen
        // first (since Tick runs on the update/render thread, while DoChanged
        // runs on the UI thread).
        switch (race)
        {
        case 0:
            f.Adapter->Tick();
            f.Adapter->DoChanged();
            break;

        case 1:
            f.Adapter->DoChanged();
            f.Adapter->Tick();
            break;

        default:
            Assert::Fail();
        }

        for (int i = 0; i < 5; ++i)
        {
            f.ExpectRender();
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    struct AsyncCreateResourcesFixture : public Fixture
    {
        ComPtr<MockAsyncAction> Action;

        AsyncCreateResourcesFixture()
            : Action(Make<MockAsyncAction>())
        {
            // ensure update/draw is always called
            ThrowIfFailed(Control->put_IsFixedTimeStep(FALSE));

            auto onCreateResources = Callback<Animated_CreateResourcesEventHandler>(
                [&] (IInspectable*, ICanvasCreateResourcesEventArgs* args)
                {
                    return ExceptionBoundary(
                        [=]
                        {
                            ThrowIfFailed(args->TrackAsyncAction(Action.Get()));
                        });
                });
            AddCreateResourcesHandler(onCreateResources.Get());
        
            Load();
        }

        void DoChangedAndTick()
        {
            Adapter->DoChanged();
            Adapter->Tick();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_WhileWaitingForAsyncCreateResources_OnlySingleFrameIsPresented)
    {
        AsyncCreateResourcesFixture f;

        // We expect there to only be one frame presented while the async action
        // is in progress
        f.ExpectRender();

        for (int i = 0; i < 5; ++i)
        {
            f.DoChangedAndTick();
        }

        // When the action completes we expect there to be a frame presented for
        // each changed/tick that happens.
        f.Action->SetResult(S_OK);

        for (int i = 0; i < 5; ++i)
        {
            f.ExpectRender();
            f.DoChangedAndTick();
        }
    }
    
    TEST_METHOD_EX(CanvasAnimatedControl_When_ClearColorIsChanged_WhileWaitingForAsyncCreateResource_ThenNewFrameIsPresented)
    {
        Color anyColors[] {
            { 1, 2, 3, 4 },
            { 2, 3, 4, 5 },
            { 3, 4, 5, 6 } };

        AsyncCreateResourcesFixture f;

        f.ExpectRender();
        f.DoChangedAndTick();

        for (auto anyColor : anyColors)
        {
            ThrowIfFailed(f.Control->put_ClearColor(anyColor));
            f.ExpectRender();
            f.DoChangedAndTick();
            f.DoChangedAndTick();
        }
    }
};


class MockThreadPoolStatics : public RuntimeClass<IThreadPoolStatics>
{
public:
    CALL_COUNTER_WITH_MOCK(RunAsyncMethod, HRESULT(IWorkItemHandler*, IAsyncAction**));
    CALL_COUNTER_WITH_MOCK(RunWithPriorityAsyncMethod, HRESULT(IWorkItemHandler*, WorkItemPriority, IAsyncAction**));
    CALL_COUNTER_WITH_MOCK(RunWithPriorityAndOptionsAsyncMethod, HRESULT(IWorkItemHandler*, WorkItemPriority, WorkItemOptions, IAsyncAction**));

    IFACEMETHODIMP RunAsync(IWorkItemHandler* handler, IAsyncAction** operation)
    {
        return RunAsyncMethod.WasCalled(handler, operation);
    }

    IFACEMETHODIMP RunWithPriorityAsync(IWorkItemHandler* handler, WorkItemPriority priority, IAsyncAction** operation)
    {
        return RunWithPriorityAsyncMethod.WasCalled(handler, priority, operation);
    }

    IFACEMETHODIMP RunWithPriorityAndOptionsAsync(IWorkItemHandler* handler, WorkItemPriority priority, WorkItemOptions options, IAsyncAction** operation)
    {
        return RunWithPriorityAndOptionsAsyncMethod.WasCalled(handler, priority, options, operation);
    }
};

TEST_CLASS(CanvasAnimatedControlAdapter_ChangedAction_UnitTests)
{
    struct Fixture
    {
        ComPtr<MockThreadPoolStatics> ThreadPoolStatics;
        std::shared_ptr<ICanvasAnimatedControlAdapter> Adapter;
        ComPtr<MockWindow> Window;

        ComPtr<IDispatchedHandler> Handler;
        ComPtr<IAsyncAction> Action;

        Fixture()
            : ThreadPoolStatics(Make<MockThreadPoolStatics>())
            , Adapter(CreateCanvasAnimatedControlAdapter(ThreadPoolStatics.Get()))
            , Window(Make<MockWindow>())
        {
            Window->get_DispatcherMethod.SetExpectedCalls(1,
                [=](ICoreDispatcher** dispatcher)
                {
                    auto mockDispatcher = Make<MockDispatcher>();

                    mockDispatcher->RunAsyncMethod.SetExpectedCalls(1,
                        [=](CoreDispatcherPriority priority, IDispatchedHandler* callbackFn, IAsyncAction** action)
                        {
                            Assert::AreEqual(CoreDispatcherPriority_Normal, priority);

                            Handler = callbackFn;

                            Action = Make<MockAsyncAction>();

                            return Action.CopyTo(action);
                        });

                    return mockDispatcher.CopyTo(dispatcher);
                });
        }
    };    

    TEST_METHOD_EX(CanvasAnimatedControlAdapter_StartChangedAction_SchedulesWorkItemThatCallsChanged)
    {
        Fixture f;

        bool changedCalled = false;
        auto fakeChangedFn =
            [&]()
            {
                changedCalled = true;
            };

        f.Adapter->StartChangedAction(f.Window, fakeChangedFn);
        Assert::IsNotNull(f.Action.Get());
        Assert::IsNotNull(f.Handler.Get());

        ThrowIfFailed(f.Handler->Invoke());
        Assert::IsTrue(changedCalled);
    }
};

TEST_CLASS(CanvasAnimatedControlAdapter_StartUpdateRenderLoop_UnitTests)
{
    struct Fixture 
    {
        ComPtr<MockThreadPoolStatics> ThreadPoolStatics;
        std::shared_ptr<ICanvasAnimatedControlAdapter> Adapter;

        ComPtr<IWorkItemHandler> Handler;
        ComPtr<IAsyncAction> Action;

        Fixture()
            : ThreadPoolStatics(Make<MockThreadPoolStatics>())
            , Adapter(CreateCanvasAnimatedControlAdapter(ThreadPoolStatics.Get()))
        {
            ThreadPoolStatics->RunWithPriorityAndOptionsAsyncMethod.SetExpectedCalls(1,
                [=] (IWorkItemHandler* theHandler, WorkItemPriority priority, WorkItemOptions options, IAsyncAction** operation)
                {
                    Assert::AreEqual<int>(WorkItemPriority_Normal, priority);
                    Assert::AreEqual<int>(WorkItemOptions_TimeSliced, options);
                    Handler = theHandler;
                
                    Action = Make<MockAsyncAction>();
                    return Action.CopyTo(operation);
                });
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControlAdapter_StartUpdateRenderLoop_SchedulesWorkItemThatCallsTickFnUntilItReturnsFalse)
    {
        Fixture f;

        int count = 0;
        auto tickFn = 
            [&]()
            {
                ++count;
                if (count == 10)
                    return false;
                return true;
            };

        auto beforeLoopFn = [&](){};

        auto afterLoopFn = [&](){};

        auto returnedAction = f.Adapter->StartUpdateRenderLoop(beforeLoopFn, tickFn, afterLoopFn);

        Assert::IsTrue(IsSameInstance(f.Action.Get(), returnedAction.Get()));
        Assert::IsNotNull(f.Handler.Get());

        ThrowIfFailed(f.Handler->Invoke(f.Action.Get()));
        Assert::AreEqual(10, count);
    }

    TEST_METHOD_EX(CanvasAnimatedControlAdapter_StartUpdateRenderLoop_SchedulesWorkItemThatStopsWhenAsyncActionIsCanceled)
    {
        Fixture f;

        int count = 0;
        auto tickFn = 
            [&]()
            {
                ++count;
                if (count == 10)
                    As<IAsyncInfo>(f.Action)->Cancel();

                return true;
            };

        auto beforeLoopFn = [&](){};

        auto afterLoopFn = [&](){};

        f.Adapter->StartUpdateRenderLoop(beforeLoopFn, tickFn, afterLoopFn);

        ThrowIfFailed(f.Handler->Invoke(f.Action.Get()));
        Assert::AreEqual(10, count);
    }
};


TEST_CLASS(CanvasAnimatedControl_Input)
{
    struct InputFixture : public CanvasAnimatedControlFixture
    {
        InputFixture()
        {
            Load();
        }

        ComPtr<ICorePointerInputSource> GetInputSource()
        {
            ComPtr<ICorePointerInputSource> inputSource;

            ThrowIfFailed(Control->get_Input(&inputSource));

            return inputSource;
        }

        void AllowWorkerThreadToStart()
        {
            Adapter->DoChanged();
            Adapter->Tick();
        }

        void VerifyPropertiesAreInaccessibleToThisThread()
        {
            auto inputSource = GetInputSource();

            boolean b;
            Point p;
            ComPtr<ICoreCursor> cursor;

            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->ReleasePointerCapture());
            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->SetPointerCapture());
            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->get_HasCapture(&b));
            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->get_PointerPosition(&p));
            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->get_PointerCursor(&cursor));
            Assert::AreEqual(RPC_E_WRONG_THREAD, inputSource->put_PointerCursor(nullptr));

        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_get_Input_NullArg)
    {
        CanvasAnimatedControlFixture f;

        Assert::AreEqual(E_INVALIDARG, f.Control->get_Input(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_Input_NoAccessToPropertiesBeforeRenderThreadIsStarted)
    {
        InputFixture f;
        Assert::IsFalse(f.IsRenderActionRunning());

        f.VerifyPropertiesAreInaccessibleToThisThread();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_Input_IfRenderThreadStops_PropertiesInaccessible)
    {
        InputFixture f;
        f.AllowWorkerThreadToStart();

        f.Control->put_ClearColor(AnyOpaqueColor);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        Assert::IsFalse(f.IsRenderActionRunning());

        f.VerifyPropertiesAreInaccessibleToThisThread();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_Input_PropertiesPassThruAfterRenderThreadIsStarted)
    {
        InputFixture f;
        f.AllowWorkerThreadToStart();

        auto coreIndependentInputSource = f.Adapter->GetCoreIndependentInputSource();

        ComPtr<ICorePointerInputSource> inputSource;
        ThrowIfFailed(f.Control->get_Input(&inputSource));

        coreIndependentInputSource->ReleasePointerCaptureMethod.SetExpectedCalls(1);
        ThrowIfFailed(inputSource->ReleasePointerCapture());

        coreIndependentInputSource->SetPointerCaptureMethod.SetExpectedCalls(1);
        ThrowIfFailed(inputSource->SetPointerCapture());

        coreIndependentInputSource->get_HasCaptureMethod.SetExpectedCalls(1,
            [=](boolean* value)
            {
                *value = true;
                return S_OK;
            });
        boolean b;
        ThrowIfFailed(inputSource->get_HasCapture(&b)),
        Assert::IsTrue(!!b);

        Point testPoint = { 11, 22 };
        Point retrievedPoint;
        coreIndependentInputSource->get_PointerPositionMethod.SetExpectedCalls(1,
            [=](Point* pt)
            {
                *pt = testPoint;
                return S_OK;
            });
        ThrowIfFailed(inputSource->get_PointerPosition(&retrievedPoint));
        Assert::AreEqual(testPoint, retrievedPoint);

        // Tests with some non-null cursor, not worth mocking up
        ICoreCursor* fakeCursor = reinterpret_cast<ICoreCursor*>(0x1234567);

        coreIndependentInputSource->get_PointerCursorMethod.SetExpectedCalls(1,
            [=](ICoreCursor** cursor)
            {
                *cursor = fakeCursor;
                return S_OK;
            });
        ICoreCursor* retrievedCursor;
        ThrowIfFailed(inputSource->get_PointerCursor(&retrievedCursor));
        Assert::AreEqual(fakeCursor, retrievedCursor);

        coreIndependentInputSource->put_PointerCursorMethod.SetExpectedCalls(1,
            [=](ICoreCursor* cursor)
            {
                Assert::AreEqual(fakeCursor, cursor);
                return S_OK;
            });
        ThrowIfFailed(inputSource->put_PointerCursor(fakeCursor));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_Input_put_PointerCursor_NullOk)
    {
        //
        // Verifies we don't accidentally sanitize or validate null values
        //
        InputFixture f;
        f.AllowWorkerThreadToStart();

        auto inputSource = f.GetInputSource();

        auto coreIndependentInputSource = f.Adapter->GetCoreIndependentInputSource();
        coreIndependentInputSource->put_PointerCursorMethod.SetExpectedCalls(1,
            [=](ICoreCursor* cursor)
            {
                Assert::IsNull(cursor);
                return S_OK;
            });
        ThrowIfFailed(inputSource->put_PointerCursor(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_Input_AddEventDelegateNullArgs)
    {
        InputFixture f;
        auto inputSource = f.GetInputSource();

        EventRegistrationToken token;

        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerCaptureLost(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerEntered(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerExited(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerMoved(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerPressed(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerReleased(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, inputSource->add_PointerWheelChanged(nullptr, &token));
    }

    class InputFixture_EventTest
    {
    public:
        ComPtr<StubCoreIndependentInputSource> CoreIndependentInputSource;

        InputFixture_EventTest()
            : CoreIndependentInputSource(Make<StubCoreIndependentInputSource>())
        {}

        template<typename MOCK_EVENT_ADD_FN, typename INPUT_SOURCE_FN>
        void TestEvent(
            wchar_t const* name,
            MOCK_EVENT_ADD_FN& mockEventAddMethod,
            INPUT_SOURCE_FN inputSourceMethod)
        {
            InputFixture f;

            CoreIndependentInputSource->AllowAnyCall();

            auto inputSource = f.GetInputSource();

            f.Adapter->SetCoreIndpendentInputSource(CoreIndependentInputSource);

            auto appEventHandler = MockEventHandler<EventHandlerWithPointerArgs>(name);

            ComPtr<EventHandlerWithPointerArgs> storedEventHandlerOfControl;
            mockEventAddMethod.SetExpectedCalls(1,
                [&storedEventHandlerOfControl](EventHandlerWithPointerArgs* eventHandlerOfControl, EventRegistrationToken*)
            {
                Assert::IsNotNull(eventHandlerOfControl);
                storedEventHandlerOfControl = eventHandlerOfControl;
                return S_OK;
            });

            EventRegistrationToken token;
            ThrowIfFailed((*(inputSource.Get()).*inputSourceMethod)(appEventHandler.Get(), &token));

            //
            // Once the render thread starts, the event handlers should be called as intended.
            //
            f.AllowWorkerThreadToStart();

            // When an input event happens, expect our event handler to be called.
            appEventHandler.SetExpectedCalls(1);

            // Issue a fake input event by raising the saved event handler.
            storedEventHandlerOfControl->Invoke(nullptr, nullptr);
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_Input_CanAddEventsBeforeWorkerThreadIsStarted)
    {
        //
        // Implementation detail: The control's input events are tracked using 
        // EventSource. This test, therefore, will not try to test all the features
        // of EventSource, but verifes that events are passed through as expected.
        //
        InputFixture_EventTest f;
        f.TestEvent(L"PointerCaptureLost", f.CoreIndependentInputSource->add_PointerCaptureLostMethod, &ICorePointerInputSource::add_PointerCaptureLost);

        f.TestEvent(L"PointerEntered", f.CoreIndependentInputSource->add_PointerEnteredMethod, &ICorePointerInputSource::add_PointerEntered);

        f.TestEvent(L"PointerExited", f.CoreIndependentInputSource->add_PointerExitedMethod, &ICorePointerInputSource::add_PointerExited);

        f.TestEvent(L"PointerMoved", f.CoreIndependentInputSource->add_PointerMovedMethod, &ICorePointerInputSource::add_PointerMoved);

        f.TestEvent(L"PointerPressed", f.CoreIndependentInputSource->add_PointerPressedMethod, &ICorePointerInputSource::add_PointerPressed);

        f.TestEvent(L"PointerReleased", f.CoreIndependentInputSource->add_PointerReleasedMethod, &ICorePointerInputSource::add_PointerReleased);

        f.TestEvent(L"PointerWheelChanged", f.CoreIndependentInputSource->add_PointerWheelChangedMethod, &ICorePointerInputSource::add_PointerWheelChanged);
    }
};

TEST_CLASS(CanvasAnimatedControl_SuspendingTests)
{
    class Fixture : public CanvasAnimatedControlFixture
    {
    public:
        ComPtr<MockSuspendingEventArgs> SuspendingEventArgs;
        ComPtr<MockSuspendingOperation> SuspendingOperation;
        ComPtr<MockSuspendingDeferral> SuspendingDeferral;

        Fixture()
        {
            Load();
            Adapter->DoChanged();

            SuspendingEventArgs = Make<MockSuspendingEventArgs>();
            SuspendingOperation = Make<MockSuspendingOperation>();
            SuspendingDeferral = Make<MockSuspendingDeferral>();

            SuspendingEventArgs->get_SuspendingOperationMethod.SetExpectedCalls(1, [=](ISuspendingOperation** value)
                {
                    return SuspendingOperation.CopyTo(value);
                });

            SuspendingOperation->GetDeferralMethod.SetExpectedCalls(1, [=](ISuspendingDeferral** value)
                {
                    return SuspendingDeferral.CopyTo(value);
                });

            SuspendingDeferral->CompleteMethod.SetExpectedCalls(1);
        }

        MockCanvasDevice* GetDevice()
        {
            ComPtr<ICanvasDevice> device;
            Control->get_Device(&device);

            return static_cast<MockCanvasDevice*>(device.Get());
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_WhenSuspendingEventRaised_TrimCalledOnDeviceAfterRenderThreadStops)
    {
        Fixture f;

        // Initial state should have the render thread active.
        Assert::IsTrue(f.IsRenderActionRunning());

        // Suspending event should stop the render thread.
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));

        f.Adapter->Tick();
        Assert::IsFalse(f.IsRenderActionRunning());

        // Once rendering is stopped, Trim should be called.
        f.GetDevice()->TrimMethod.SetExpectedCalls(1);
        f.Adapter->DoChanged();

        // Render thread should not be restarted by DoChanged while app is in the suspended state.
        Assert::IsFalse(f.IsRenderActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenSuspendingEventRaisedWhileControlIsPaused_TrimIsStillCalled)
    {
        Fixture f;

        ThrowIfFailed(f.Control->put_Paused(true));
        f.Adapter->Tick();
        Assert::IsFalse(f.IsRenderActionRunning());

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));
        f.Adapter->Tick();

        f.GetDevice()->TrimMethod.SetExpectedCalls(1);
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenResumingEventRaised_RenderThreadRestarts)
    {
        Fixture f;

        // Suspend the app.
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));
        f.Adapter->Tick();
        f.GetDevice()->TrimMethod.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        Assert::IsFalse(f.IsRenderActionRunning());

        // Resume the app.
        ThrowIfFailed(f.Adapter->ResumingEventSource->InvokeAll(nullptr, nullptr));
        f.Adapter->DoChanged();
        Assert::IsTrue(f.IsRenderActionRunning());
    }
};
