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
        Color opaqueColor{ 255, 255, 255, 255 };
        Color translucentColor{ 254, 254, 254, 254 };

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

        //
        // The put_ClearColor causes recreation in the following way:
        //
        // A flag is set in the control to exit out of the worker thread.
        // Given that the worker thread is exited, the UI thread is free to 
        // start it again. Before doing this, i.e. before RunWithRenderTarget's
        // payload, there's a check to verify whether the target needs updating.
        // Re-creation happens there.
        //

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(opaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        f.Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1,
            [swapChainManager](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasAlphaMode::Premultiplied, alphaMode);
                return CanvasAnimatedControlFixture::CreateTestSwapChain(swapChainManager, device);
            });

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(translucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RedundantClearColorChangesDoNotCauseRecreation)
    {
        Color opaqueColor{ 255, 255, 255, 255 };
        Color otherOpaqueColor{ 255, 12, 34, 56 };
        Color translucentColor{ 254, 254, 254, 254 };
        Color otherTranslucentColor{ 200, 200, 200, 200 };

        CanvasAnimatedControlFixture f;
        auto swapChainManager = f.Adapter->SwapChainManager;
        f.Load();
        f.Adapter->DoChanged();
        
        f.ExpectOneCreateSwapChain();

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(opaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
        Assert::AreEqual(S_OK, f.Control->put_ClearColor(otherOpaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        f.ExpectOneCreateSwapChain();

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(translucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
        Assert::AreEqual(S_OK, f.Control->put_ClearColor(otherTranslucentColor));
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

        ThrowIfFailed(f.Control->put_ClearColor(Color{ 1, 2, 3, 4 }));
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



    class DeviceLostFixture : public FixtureWithSwapChainAccess
    {
        bool m_throwDeviceRemovedDuringNextCreateResources;

    public:
        MockEventHandler<Animated_DrawEventHandler> OnDraw;
        MockEventHandler<Animated_CreateResourcesEventHandler> OnCreateResources;
        MockEventHandler<Animated_UpdateEventHandler> OnUpdate;

        DeviceLostFixture()
            : m_throwDeviceRemovedDuringNextCreateResources(false)
        {
            OnCreateResources.AllowAnyCall();
            AddCreateResourcesHandler(OnCreateResources.Get());

            OnDraw.AllowAnyCall();
            AddDrawHandler(OnDraw.Get());

            OnUpdate.AllowAnyCall();
            AddUpdateHandler(OnUpdate.Get());
        }

        // Simulates a recoverable device.
        void ThrowDeviceRemovedDuringNextCreateResources()
        {
            m_throwDeviceRemovedDuringNextCreateResources = true;

            OnCreateResources.AllowAnyCall(
                [=](ICanvasAnimatedControl*, IInspectable*) -> HRESULT
                {
                    bool shouldThrow = m_throwDeviceRemovedDuringNextCreateResources;

                    m_throwDeviceRemovedDuringNextCreateResources = false;

                    if (shouldThrow)
                        return DXGI_ERROR_DEVICE_REMOVED;

                    return S_OK;
                });
        }
        
        // Simulates a permanently 'broken' device.
        void ThrowDeviceRemovedDuringCreateResources()
        {
            OnCreateResources.AllowAnyCall(
                [=](ICanvasAnimatedControl*, IInspectable*) -> HRESULT
                {
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }

        void ThrowDeviceRemovedDuringDraw()
        {        
            OnDraw.AllowAnyCall( 
                [=](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*) -> HRESULT
                {
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }

        void MakePresentFail()
        {
            m_dxgiSwapChain->Present1Method.AllowAnyCall(
                [=](UINT, UINT, const DXGI_PRESENT_PARAMETERS*)
                {
                    return DXGI_ERROR_DEVICE_REMOVED;
                });

        }

        void ForceRedraw()
        {
            Adapter->ProgressTime(TicksPerFrame);
        }

        void MarkAsLost()
        {
            Adapter->InitialDevice->MarkAsLost();
        }

        void TickAndVerifyDeviceRemovedErrorCode()
        {
            auto renderAction = Adapter->m_outstandingWorkItemAsyncAction;

            Adapter->Tick();

            auto renderAsyncInfo = As<IAsyncInfo>(renderAction);

            HRESULT errorCode;
            ThrowIfFailed(renderAsyncInfo->get_ErrorCode(&errorCode));

            Assert::AreEqual(DXGI_ERROR_DEVICE_REMOVED, errorCode);
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_FirstPresentFails)
    {
        DeviceLostFixture f;
        f.MakePresentFail();
        f.RaiseLoadedAndVerify();

        auto renderAction = f.Adapter->m_outstandingWorkItemAsyncAction;

        //
        // The Loaded event will begin the worker thread and issue
        // the first draw, but the exception doesn't get propagated 
        // until the worker thread completes, at the next tick.
        //
        f.TickAndVerifyDeviceRemovedErrorCode();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_TypicalPresentFail)
    {
        DeviceLostFixture f;
        f.RaiseLoadedAndVerify();

        f.Adapter->Tick();

        f.MakePresentFail();
        f.ForceRedraw();        

        f.TickAndVerifyDeviceRemovedErrorCode();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_LostDeviceDuringCreateResources)
    {
        //
        // Device is lost on the UI thread, not the render thread, so it
        // is recovered here without being seen at Tick(). Verify no
        // exception gets propagated up.
        //
        DeviceLostFixture f;
        f.ThrowDeviceRemovedDuringNextCreateResources();
        f.MarkAsLost();
        f.Load();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_UnrecoverableDeviceDuringCreateResources)
    {
        //
        // Similar to above, device is lost on the UI thread.
        //
        DeviceLostFixture f;
        f.ThrowDeviceRemovedDuringCreateResources();
        f.MarkAsLost();
        f.Load();

        //
        // If we attempt to re-create the device, and fail, control should 
        // never go into RunWithRenderTarget. No exceptions should 
        // be propagated up.
        //
        for (int i = 0; i < 3; i++)
        {
            f.ForceRedraw();
            f.Adapter->Tick();
        }

        //
        // Worker thread should be gone.
        //
        Assert::IsFalse(f.IsRenderActionRunning());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_LostDeviceDuringDraw)
    {
        DeviceLostFixture f;
        f.RaiseLoadedAndVerify();
        f.ThrowDeviceRemovedDuringDraw();
        f.MarkAsLost();

        f.TickAndVerifyDeviceRemovedErrorCode();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_LostDeviceDuringDraw_ExceptionOnly)
    {
        DeviceLostFixture f;
        f.RaiseLoadedAndVerify();
        
        f.ThrowDeviceRemovedDuringDraw();

        f.TickAndVerifyDeviceRemovedErrorCode();
    }

    static auto const TicksPerFrame = StepTimer::TicksPerSecond / 60;

    struct UpdateRenderFixture : public CanvasAnimatedControlFixture
    {
        MockEventHandler<Animated_DrawEventHandler> OnDraw;
        MockEventHandler<Animated_UpdateEventHandler> OnUpdate;

        UpdateRenderFixture()
            : OnDraw(L"OnDraw")
            , OnUpdate(L"OnUpdate")
        {
            AddDrawHandler(OnDraw.Get());
            AddUpdateHandler(OnUpdate.Get());
        }

        void GetIntoSteadyState()
        {
            Load();
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

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_WhenPaused_UpdatesAndDrawsAreNotCalled)
    {
        UpdateRenderFixture f;
        f.GetIntoSteadyState();
        
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.RenderSingleFrame();
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
            , m_updateCount(0)
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

            //
            // Both fixed and variable step have initial updates. However, fixed has a 'force'
            // update called by the control, while variable has an initial update actually
            // fired by the step timer.
            //
            if (!m_isFixedTimestep) m_updateCount = 1;

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

            Assert::AreEqual(expected.UpdateCount, timingInformation.UpdateCount);
            Assert::AreEqual(expected.TotalTime, timingInformation.TotalTime.Duration);
            Assert::AreEqual(expected.ElapsedTime, timingInformation.ElapsedTime.Duration);
            Assert::AreEqual(expected.IsRunningSlowly, timingInformation.IsRunningSlowly);
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
            return static_cast<bool>(Adapter->m_changedAsyncAction);
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

        Color opaque = { 255, 255, 255, 255 };
        ThrowIfFailed(f.Control->put_ClearColor(opaque));

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
            if (Adapter->m_changedAsyncAction)
            {
                AsyncStatus status = (AsyncStatus)-1;
                Adapter->m_changedAsyncAction->get_Status(&status);
                if (status == AsyncStatus::Canceled)
                {
                    Adapter->m_changedAsyncAction.Reset();
                    Adapter->m_changedFn = nullptr;
                }
            }
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

        auto returnedAction = f.Adapter->StartChangedAction(f.Window, fakeChangedFn);

        Assert::IsTrue(IsSameInstance(f.Action.Get(), returnedAction.Get()));
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

        Color opaqueColor{ 255, 255, 255, 255 };
        f.Control->put_ClearColor(opaqueColor);
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