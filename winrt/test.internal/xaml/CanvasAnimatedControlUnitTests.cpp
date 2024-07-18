// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "stubs/StubDxgiSwapChain.h"

#include "MockXamlSolidColorBrush.h"

#ifndef ARCH_X86

static Color const AnyColor                 {   1,   2,   3,   4 };
static Color const AnyOtherColor            {   5,   6,   7,   8 };
static Color const AnyOpaqueColor           { 255,   2,   3,   4 };
static Color const AnyOtherOpaqueColor      { 255,   5,   6,   7 };
static Color const AnyTranslucentColor      { 254, 253, 252, 251 };
static Color const AnyOtherTranslucentColor { 250, 249, 248, 247 };

static auto const TicksPerFrame = StepTimer::TicksPerSecond / 60;

static float dpiTestCases[] = {
    50,
    DEFAULT_DPI,
    200 };

class FixtureWithSwapChainAccess : public CanvasAnimatedControlFixture
{
protected:
    ComPtr<StubDxgiSwapChain> m_dxgiSwapChain;

public:
    FixtureWithSwapChainAccess()
    {
        ResetSwapChain();
    }

    void ResetSwapChain()
    {
        m_dxgiSwapChain = Make<StubDxgiSwapChain>();

        m_dxgiSwapChain->GetHwndMethod.AllowAnyCall(
            [=](HWND* pHwnd)
            {
                *pHwnd = 0;
                return E_FAIL;
            });

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

                stubDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
                {
                    return m_dxgiSwapChain;
                });

                auto canvasSwapChain = ResourceManager::GetOrCreate(device, m_dxgiSwapChain.Get(), dpi);

                ComPtr<CanvasSwapChain> typedPtr = static_cast<CanvasSwapChain*>(As<ICanvasSwapChain>(canvasSwapChain).Get());

                return typedPtr;
            });
    }
};

class MockDispatcherQueueHandler
{
    ComPtr<IDispatcherQueueHandler> m_handler;
    ComPtr<IAsyncAction> m_action;

public:
    CALL_COUNTER_WITH_MOCK(OnInvoke, HRESULT());
    CALL_COUNTER_WITH_MOCK(OnCompleted, HRESULT(IAsyncAction*, AsyncStatus));

    MockDispatcherQueueHandler()
    {
        m_handler = Callback<AddFtmBase<IDispatcherQueueHandler>::Type>(
            [=]
            {
                return OnInvoke.WasCalled();
            });
        OnCompleted.AllowAnyCall();
    }

    void RunOnGameLoopThreadAsync(ComPtr<ICanvasAnimatedControl> const& control)
    {
        ThrowIfFailed(control->RunOnGameLoopThreadAsync(m_handler.Get(), &m_action));

        auto completedCallback = Callback<IAsyncActionCompletedHandler>(
            [=] (IAsyncAction* action, AsyncStatus status)
            {
                return OnCompleted.WasCalled(action, status);
            });
        ThrowIfFailed(m_action->put_Completed(completedCallback.Get()));
    }

    void VerifyActionStatus(AsyncStatus expectedStatus, HRESULT expectedHr)
    {
        auto asyncInfo = As<IAsyncInfo>(m_action);

        AsyncStatus status;
        ThrowIfFailed(asyncInfo->get_Status(&status));
        Assert::AreEqual(expectedStatus, status);

        HRESULT errorCode;
        ThrowIfFailed(asyncInfo->get_ErrorCode(&errorCode));
        Assert::AreEqual(expectedHr, errorCode);
    }

    void CancelAction()
    {
        ThrowIfFailed(As<IAsyncInfo>(m_action)->Cancel());
    }
};

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
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ABI::Microsoft::UI::Xaml::Controls::IUserControl);
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
        f.Load();
        f.Adapter->DoChanged();
        
        f.Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1, 
            [](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasAlphaMode::Ignore, alphaMode);
                return CanvasAnimatedControlFixture::CreateTestSwapChain(device);
            });

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyOpaqueColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();

        f.Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1,
            [](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasAlphaMode::Premultiplied, alphaMode);
                return CanvasAnimatedControlFixture::CreateTestSwapChain(device);
            });

        Assert::AreEqual(S_OK, f.Control->put_ClearColor(AnyTranslucentColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RedundantClearColorChangesDoNotCauseRecreation)
    {
        CanvasAnimatedControlFixture f;
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

        f.VerifyTickLoopIsStillRunning();

        ThrowIfFailed(f.Control->put_ClearColor(AnyColor));
        f.Adapter->Tick();
        f.Adapter->DoChanged();
        f.VerifyTickLoopIsStillRunning();
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

        static const int TickCountForExecute = 5;

        void Execute(Size size)
        {
            UserControl->Resize(size);

            for (int i = 0; i < TickCountForExecute; ++i)
            {
                Adapter->Tick();
                Assert::IsTrue(Adapter->GameThreadHasPendingWork());
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
        f.Execute(Size{ static_cast<float>(ResizeFixture::InitialWidth), static_cast<float>(ResizeFixture::InitialHeight) });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResizedToZero_ThenSwapChainIsSetToNull)
    {
        ResizeFixture f;

        // Resizing to zero should stop the update/render loop.
        f.Adapter->GetSwapChainPanel()->SetSwapChainMethod.SetExpectedCalls(0);
        f.UserControl->Resize(Size{ 0, 0 });
        f.Adapter->Tick();

        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

        // The UI thread change handler should then restart the update/render loop.
        f.Adapter->GetSwapChainPanel()->SetSwapChainMethod.SetExpectedCalls(1, [](IDXGISwapChain* swapChain)
            {
                Assert::IsNull(swapChain);
                return S_OK;
            });

        f.Adapter->DoChanged();
        
        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());

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

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsResizedToZero_SleepOccurs)
    {
        // 
        // A swap chain doesn't exist in this scenario. Sleep is used to threshold
        // the progression of the control, instead of synch to vertical blank.
        //

        for (int i = 0; i < 2; ++i)
        {
            ResizeFixture f;

            ThrowIfFailed(f.Control->put_IsFixedTimeStep(i==0));

            f.UserControl->Resize(Size{ 0, 0 });
            f.Adapter->Tick();
            f.Adapter->DoChanged();

            MockEventHandler<Animated_UpdateEventHandler> onUpdate(L"OnUpdate");
            f.AddUpdateHandler(onUpdate.Get());
            onUpdate.ExpectAtLeastOneCall();

            int sleepCount = 0;
            f.Adapter->m_sleepFn =
                [&](DWORD timeInMs)
                {
                    Assert::AreEqual(static_cast<DWORD>(16), timeInMs);
                    sleepCount++;
                };

            f.Adapter->ProgressTime(TicksPerFrame);
            f.Execute(Size{ 0, 0 });

            Assert::AreEqual(ResizeFixture::TickCountForExecute, sleepCount);
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_SeesConsistentSizeInUpdateRenderHandlers)
    {
        ResizeFixture f;
        
        Size initialSize{ 1, 1 };
        Size otherSize{ 2, 2 };
        
        MockEventHandler<Animated_UpdateEventHandler> onUpdate(L"OnUpdate");
        MockEventHandler<Animated_DrawEventHandler> onDraw(L"OnDraw");
        f.AddUpdateHandler(onUpdate.Get());
        f.AddDrawHandler(onDraw.Get());
        
        Size sizeSeenByUpdate, sizeSeenByDraw;

        onUpdate.SetExpectedCalls(2,
            [&](ICanvasAnimatedControl* c, ICanvasAnimatedUpdateEventArgs*)
            {
                // Calling Resize here simulates the UI thread receiving a
                // resize event while a game loop tick is running.
                //
                // We want to ensure that Update/Draw see the same size for the
                // control within a single tick.
                f.UserControl->Resize(otherSize);
                
                ThrowIfFailed(c->get_Size(&sizeSeenByUpdate));
                return S_OK;
            });

        onDraw.SetExpectedCalls(2,
            [&](ICanvasAnimatedControl* c, ICanvasAnimatedDrawEventArgs*)
            {
                ThrowIfFailed(c->get_Size(&sizeSeenByDraw));
                return S_OK;
            });

        ThrowIfFailed(f.Control->put_IsFixedTimeStep(FALSE));

        f.UserControl->Resize(initialSize);
        f.Adapter->Tick();

        Assert::AreEqual(initialSize, sizeSeenByUpdate);
        Assert::AreEqual(initialSize, sizeSeenByDraw);
        
        f.Adapter->Tick();

        Assert::AreEqual(otherSize, sizeSeenByUpdate);
        Assert::AreEqual(otherSize, sizeSeenByDraw);
    }

// Test removed, as DPI now comes from the XamlRoot instead of the adapter
#ifndef WINUI3
    class DpiFixture : public ResizeFixture
    {
    public:
        float m_controlSize;
        MockEventHandler<Animated_CreateResourcesEventHandler> m_createResourcesEventHandler;

        DpiFixture(float controlSize = 1000)
            : m_controlSize(controlSize)
            , m_createResourcesEventHandler(L"CreateResources", ExpectedEventParams::Both)
        {
            // Set an initial size. Commit it using a resize event.
            UserControl->Resize(Size{ m_controlSize, m_controlSize });

            // Process the resize event.
            m_dxgiSwapChain->ResizeBuffersMethod.AllowAnyCall();
            Adapter->Tick();
            Adapter->ProgressTime(TicksPerFrame);

            // An event handler needs to be registered for a drawing session to be constructed.
            auto onDrawFn =
                Callback<Animated_DrawEventHandler>([](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*) { return S_OK; });
            EventRegistrationToken drawEventToken;
            ThrowIfFailed(Control->add_Draw(onDrawFn.Get(), &drawEventToken));

            // CR is raised once immediately after adding, because this control's been Loaded already.
            m_createResourcesEventHandler.AllowAnyCall();
            AddCreateResourcesHandler(m_createResourcesEventHandler.Get());
        }

        void SetDpiAndFireEvent(float dpi)
        {
            Adapter->LogicalDpi = dpi;
            Adapter->RaiseDpiChangedEvent();
        }

        void DoNotExpectResizeBuffers()
        {
            m_dxgiSwapChain->ResizeBuffersMethod.SetExpectedCalls(0);
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_AfterDpiChange_NoSizeChange_DoNotExpectResize_UntilTargetRecreation)
    {
        for (auto dpiCase : dpiTestCases)
        {
            DpiFixture f;

            f.DoNotExpectResizeBuffers();

            f.SetDpiAndFireEvent(dpiCase);

            f.Adapter->Tick();

            if (dpiCase == DEFAULT_DPI)
                continue;

            float expectedPixelSize = static_cast<float>(SizeDipsToPixels(f.m_controlSize, dpiCase));
            f.ExpectOneResizeBuffers(Size{ expectedPixelSize, expectedPixelSize });
            f.Adapter->DoChanged();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_AfterDpiChange_DeviceContextHasDpiSet)
    {
        for (auto dpiCase : dpiTestCases)
        {
            DpiFixture f;

            auto deviceContext = Make<StubD2DDeviceContext>(nullptr);
            f.Adapter->InitialDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
                [&]
                {
                    return deviceContext;
                });

            f.SetDpiAndFireEvent(dpiCase);

            f.Adapter->Tick(); // Allow game loop thread to stop
            f.Adapter->DoChanged(); // Schedule game loop thread to start again

            deviceContext->SetDpiMethod.SetExpectedCalls(1,
                [&](float dpiX, float dpiY)
                {
                    Assert::AreEqual(dpiCase, dpiX);
                    Assert::AreEqual(dpiCase, dpiY);
                });
            f.Adapter->ProgressTime(TicksPerFrame);

            // Target will get re-created (or resized) with correct DPI.
            f.Adapter->Tick(); 
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ZeroSizedControl_DpiChange_DoesNotResize)
    {
        DpiFixture f(0);

        f.DoNotExpectResizeBuffers();

        float someDpi = 123.0f;
        f.SetDpiAndFireEvent(someDpi);

        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_AfterDpiChange_ExpectCreateResources)
    {
        for (auto dpiCase : dpiTestCases)
        {
            DpiFixture f;

            f.SetDpiAndFireEvent(dpiCase);

            if (dpiCase != DEFAULT_DPI)
                f.m_createResourcesEventHandler.SetExpectedCalls(1);

            f.Adapter->Tick();
            f.Adapter->DoChanged();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_AfterDpiChange_CreateResourcesHasCorrectParameter)
    {
        DpiFixture f;

        f.SetDpiAndFireEvent(200.0f);

        f.m_createResourcesEventHandler.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasCreateResourcesEventArgs* eventArgs)
            {
                CanvasCreateResourcesReason reason = (CanvasCreateResourcesReason)-1;
                Assert::AreEqual(S_OK, eventArgs->get_Reason(&reason));
                Assert::AreEqual(CanvasCreateResourcesReason::DpiChanged, reason);

                return S_OK;
            });

        f.Adapter->Tick();
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_ChangeToNonDefaultDpi_ThenResize_ResizeUsesCorrectDpi)
    {
        DpiFixture f;

        f.SetDpiAndFireEvent(DEFAULT_DPI * 2);

        // Reach steady state
        f.Adapter->Tick(); 
        f.Adapter->DoChanged(); 
        f.Adapter->Tick();

        // Issue a resize
        f.UserControl->Resize(Size{ 123, 456 });

        // Process the resize event.
        f.ExpectOneResizeBuffers(Size{ 123 * 2, 456 * 2 });
        f.Adapter->Tick();
    }
#endif

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

        void PresentSucceeds()
        {
            m_dxgiSwapChain->Present1Method.AllowAnyCall();
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

        void VerifyDeviceRecovered(bool controlAlreadyNoticed = false)
        {
            if (!controlAlreadyNoticed)
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


    TEST_METHOD_EX(CanvasAnimatedControl_WhenCustomDeviceIsLost_CustomDevicePropertyStillWorks)
    {
        DeviceLostFixture f;

        auto customDevice = Make<StubCanvasDevice>();
        Assert::AreEqual(S_OK, f.Control->put_CustomDevice(customDevice.Get()));
        customDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
            [=]
            {
                return Make<StubD2DDeviceContext>(nullptr);
            });

        f.DoChangedAndTick(); // Allows for custom device to be properly set.

        f.OnDraw.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*)
            {
                customDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });
        
        f.DoChangedAndTick(); // Allow for device lost to be noticed.
        f.Adapter->DoChanged(); // Control's device lost handler is called.

        ComPtr<ICanvasDevice> retrievedDevice;
        Assert::AreEqual(S_OK, f.Control->get_CustomDevice(&retrievedDevice));
        Assert::AreEqual(static_cast<ICanvasDevice*>(customDevice.Get()), retrievedDevice.Get());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenCustomDeviceIsLost_CustomDeviceCanBeSetAgain)
    {
        DeviceLostFixture f;

        auto customDevice = Make<StubCanvasDevice>();
        Assert::AreEqual(S_OK, f.Control->put_CustomDevice(customDevice.Get()));
        customDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
            [=]
            {
                return Make<StubD2DDeviceContext>(nullptr);
            });

        f.DoChangedAndTick(); // Allows for custom device to be properly set.

        f.OnDraw.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*)
            {
                customDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.DoChangedAndTick(); // Allow for device lost to be noticed.
        f.Adapter->DoChanged(); // Control's device lost handler is called.

        auto otherCustomDevice = Make<StubCanvasDevice>();
        Assert::AreEqual(S_OK, f.Control->put_CustomDevice(otherCustomDevice.Get()));

        ComPtr<ICanvasDevice> retrievedDevice;
        Assert::AreEqual(S_OK, f.Control->get_CustomDevice(&retrievedDevice));
        Assert::AreEqual(static_cast<ICanvasDevice*>(otherCustomDevice.Get()), retrievedDevice.Get());
    }


    TEST_METHOD_EX(CanvasAnimatedControl_WhenCustomDeviceIsLostAndSetAgain_ControlIsRecovered)
    {
        DeviceLostFixture f;

        auto customDevice = Make<StubCanvasDevice>();
        customDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
            [=]
            {
                return Make<StubD2DDeviceContext>(nullptr);
            });
        Assert::AreEqual(S_OK, f.Control->put_CustomDevice(customDevice.Get()));

        f.DoChangedAndTick(); // Allows for custom device to be properly set.

        f.OnDraw.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasAnimatedDrawEventArgs*)
            {
                customDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f.DoChangedAndTick(); // Allow for device lost to be noticed.
        f.Adapter->DoChanged(); // Control's device lost handler is called.

        auto otherCustomDevice = Make<StubCanvasDevice>();
        otherCustomDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
            [&]
            {
                return Make<StubD2DDeviceContext>(nullptr);
            });
        Assert::AreEqual(S_OK, f.Control->put_CustomDevice(otherCustomDevice.Get()));

        //
        // Necessary- otherwise, when the test infrastructure uses interop to
        // create a new CanvasSwapChain, it will get confused as it's already
        // seen its dxgi swap chain with a different canvas device.
        //
        f.ResetSwapChain();

        //
        // We want to do the validation, but we've already seen/processed
        // device lost (and reacted to it by resetting the custom device).
        //
        f.VerifyDeviceRecovered(true);
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

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlIsPaused_AndDeviceLostEventIsRaisedExternally_DeviceIsRecreated)
    {
        //
        // In this scenario, the control is not attempting to use the device.
        // However, some external code realizes that the device is lost and so
        // raises the device lost event.  In this case the control needs to
        // recreate the device.
        //

        DeviceLostFixture f;

        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.OnCreateResources.SetExpectedCalls(1);

        f.DoChangedAndTick();

        f.MarkDeviceAsLost();
        f.LoseDeviceDuringPresent(); // this ensures that animated control sees a device lost error

        ThrowIfFailed(f.Adapter->InitialDevice->RaiseDeviceLost());

        // In this tick, the Present() call will fail with device lost...
        f.DoChangedAndTick();

        // The tests don't actually create a new device, so we shim this one to
        // look like it is working again.
        f.PresentSucceeds();

        // Now the UI thread picks up the device lost exception
        f.DoChangedAndTick();

        // And so now the UI thread "creates" a new device, and runs through the
        // CreateResources process
        f.OnCreateResources.SetExpectedCalls(1);
        f.DoChangedAndTick();
    }


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
                    auto dxgiSwapChain = static_cast<MockDxgiSwapChain*>(swapChain);
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

        void ExpectUpdateWithElapsedTime(int64_t expected)
        {
            OnUpdate.SetExpectedCalls(1,
                [=] (ICanvasAnimatedControl*, ICanvasAnimatedUpdateEventArgs* args)
                {
                    CanvasTimingInformation timingInformation;
                    ThrowIfFailed(args->get_Timing(&timingInformation));
                    Assert::AreEqual(expected, timingInformation.ElapsedTime.Duration);
                    return S_OK;
                });
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

        Expectations::Instance()->Validate();
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

    TEST_METHOD_EX(CanvasAnimatedControl_ElapsedTime_DoesNotInclude_TimeWhilePaused)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(false));

        f.GetIntoSteadyState();
        f.OnDraw.AllowAnyCall();

        int64_t timeWhileNotPaused = 0;

        for (int i = 0; i < 50; ++i)
        {
            int64_t t = (i+1) * 10;

            f.Adapter->ProgressTime(t);
            timeWhileNotPaused += t;

            ThrowIfFailed(f.Control->put_Paused(TRUE));
            f.Adapter->ProgressTime(t * 50);
            ThrowIfFailed(f.Control->put_Paused(FALSE));
        }

        f.ExpectUpdateWithElapsedTime(timeWhileNotPaused);
        f.Adapter->DoChanged();
        f.Adapter->Tick();
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

                auto stubDevice = static_cast<StubCanvasDevice*>(device.Get());
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
        
        f.Adapter->ProgressTime(TicksPerFrame);
        ThrowIfFailed(f.Control->put_Paused(TRUE)); // we pause it after a
                                                    // single frame's worth time
                                                    // has passed

        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);
        f.Adapter->DoChanged();
        f.Adapter->Tick();

        f.Adapter->ProgressTime(TicksPerFrame * 1000);
        ThrowIfFailed(f.Control->put_Paused(FALSE)); // unpause after much time
                                                     // has passed

        // But only see one update - so the time while paused is not counted
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        f.Adapter->Tick();
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
        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

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

                bool isRunningSlowly = numUpdates > 1;

                for (INT64 i = 0; i < numUpdates; ++i)
                {
                    m_updateCount++;

                    INT64 timeSinceLastUpdate;
                    if (i < numUpdates - 1)
                    {
                        timeSinceLastUpdate = TicksPerFrame;
                        m_totalTime += TicksPerFrame;
                    }
                    else
                    {
                        timeSinceLastUpdate = targetTime - m_totalTime;
                        m_totalTime = targetTime;
                    }

                    ExpectedTiming expected{ m_updateCount, m_totalTime, timeSinceLastUpdate, static_cast<boolean>(isRunningSlowly) };

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

    TEST_METHOD_EX(CanvasAnimatedControl_FixedTimeStep_RenderThreadWaitsForVBlank_IfNoDraw)
    {
        CanvasAnimatedControlFixture f;
        f.Load();
        f.Adapter->DoChanged();

        auto mockDxgiOutput = Make<MockDxgiOutput>();

        // First ticks always updates/draws/presents, so we don't expect a
        // vblank.
        f.Adapter->Tick();


        // Second tick, because we didn't progress time, will no
        // update/draw/present, and so instead will wait on a vblank.
        f.Device->GetPrimaryDisplayOutputMethod.SetExpectedCalls(1,
            [&]
            {
                return mockDxgiOutput;
            });

        mockDxgiOutput->WaitForVBlankMethod.SetExpectedCalls(1);

        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_VariableTimeStep_AlwaysWaitsForVBlank)
    {
        CanvasAnimatedControlFixture f;
        f.Load();
        f.Adapter->DoChanged();
        ThrowIfFailed(f.Control->put_IsFixedTimeStep(false));

        auto mockDxgiOutput = Make<MockDxgiOutput>();

        f.Device->GetPrimaryDisplayOutputMethod.SetExpectedCalls(2,
            [&]
            {
                return mockDxgiOutput;
            });

        mockDxgiOutput->WaitForVBlankMethod.SetExpectedCalls(2);

        f.Adapter->Tick();
        f.Adapter->Tick();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_InvalidateCalled_AnyNumberOfTimes_WhileNotPaused_ThenSingleDrawCalled)
    {
        UpdateRenderFixture f;        
        f.GetIntoSteadyState();
        ThrowIfFailed(f.Control->put_Paused(FALSE));

        // Now, we don't progress time, so we don't expect any more updates.  We
        // call Invalidate many times, but only get a single Draw as a result.
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 20; ++i)
            ThrowIfFailed(f.Control->Invalidate());

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_InvalidateCalled_AnyNumberOfTimes_WhileNotPaused_AndTimeProgresses_ThenSingleUpdateAndDrawCalled)
    {
        UpdateRenderFixture f;        
        f.GetIntoSteadyState();
        ThrowIfFailed(f.Control->put_Paused(FALSE));

        // When time progresses and we call Invalidate() we still only see a
        // single Draw.
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 20; ++i)
            ThrowIfFailed(f.Control->Invalidate());

        f.Adapter->ProgressTime(TicksPerFrame);

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_InvalidateCalled_AnyNumberOfTimes_WhilePaused_ThenOnlySingleDrawIsCalled)
    {
        UpdateRenderFixture f;        
        f.GetIntoSteadyState();
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 20; ++i)
            ThrowIfFailed(f.Control->Invalidate());

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_ControlStartsPaused_AndInvalidateIsCalled_ThenDrawIsCalledBeforeFirstUpdate)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.OnCreateResources.SetExpectedCalls(1);
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(1);

        f.Load();

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        for (int i = 0; i < 20; ++i)
            ThrowIfFailed(f.Control->Invalidate());

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_ControlStartsPaused_AndCreateResourcesHasAnAsyncAction_AndInvalidateIsCalled_ThenDrawIsCalledAfterCreateResourcesCompletes)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        auto action = Make<MockAsyncAction>();
        f.OnCreateResources.SetExpectedCalls(1,
            [=] (IInspectable*, ICanvasCreateResourcesEventArgs* args)
            {
                return args->TrackAsyncAction(action.Get());
            });
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);

        f.Load();

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        for (int i = 0; i < 20; ++i)
            ThrowIfFailed(f.Control->Invalidate());

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        // The draw only happens after the async action completes
        action->SetResult(S_OK);
        f.OnDraw.SetExpectedCalls(1);

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->ProgressTime(TicksPerFrame);
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_When_ControlIsPaused_AndRunOnGameLoopAsyncActionCallsInvalidate_Then_DrawHandlerIsCalledOnTheSameTick)
    {
        UpdateRenderFixture f;
        ThrowIfFailed(f.Control->put_Paused(TRUE));

        f.OnCreateResources.SetExpectedCalls(1);
        f.OnUpdate.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);

        f.Load();

        for (int i = 0; i < 20; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        auto handler = Callback<IDispatcherQueueHandler>(
            [&]
            {
                ThrowIfFailed(f.Control->Invalidate());
                return S_OK;
            });
        ComPtr<IAsyncAction> action;
        ThrowIfFailed(f.Control->RunOnGameLoopThreadAsync(handler.Get(), &action));

        f.OnDraw.SetExpectedCalls(1);
        f.Adapter->DoChanged();
        f.Adapter->Tick();        
    }

    TEST_METHOD_EX(CanvasAnimatedControl_CreateCoreIndependentInputSource_CallsThroughToSwapChainPanel)
    {
        CanvasAnimatedControlFixture f;

        auto anyDeviceTypes = static_cast<InputPointerSourceDeviceKinds>(123);
        auto anyReturnValue = reinterpret_cast<IInputPointerSource**>(456);
        HRESULT anyResultCode = -789;

        f.Adapter->GetSwapChainPanel()->CreateCoreIndependentInputSourceMethod.SetExpectedCalls(1,
            [=] (InputPointerSourceDeviceKinds actualDeviceTypes, IInputPointerSource** actualReturnValue)
            {
                Assert::IsTrue(anyDeviceTypes == actualDeviceTypes, L"deviceTypes parameter is passed through");
                Assert::IsTrue(anyReturnValue == actualReturnValue, L"returnValue paremeter is passed through");
                return anyResultCode;
            });
        
        auto actualResultCode = f.Control->CreateCoreIndependentInputSource(
            anyDeviceTypes,
            anyReturnValue);

        Assert::AreEqual(anyResultCode, actualResultCode, L"result code is passed back");
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
            
            Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
                [=] (ICanvasDevice*, float, float, float, CanvasAlphaMode)
                {
                    return Make<MockCanvasSwapChain>();
                });
        }

        void GetIntoSteadyState()
        {
            Load();
            Adapter->DoChanged();
        }

        bool IsChangedActionRunning()
        {
            return Adapter->HasPendingActionsOnUiThread();
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

    TEST_METHOD_EX(CanvasAnimatedControl_UseSharedDevice_Default_False)
    {
        CanvasAnimatedControlFixture f;

        boolean useSharedDevice;
        Assert::AreEqual(S_OK, f.Control->get_UseSharedDevice(&useSharedDevice));
        Assert::IsFalse(!!useSharedDevice);
    }
};

TEST_CLASS(CanvasAnimatedControlRenderLoop)
{
    struct Fixture : public Animated_BasicControlFixture
    {
        ComPtr<MockCanvasSwapChain> SwapChain;

        Fixture()
        {
            SwapChain = Make<MockCanvasSwapChain>();

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
    };

    TEST_METHOD_EX(CanvasAnimatedControl_OnConstruction_UpdateRenderLoopIsNotRunning)
    {
        Fixture f;

        f.Adapter->DoChanged();
        f.Adapter->Tick();

        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenLoaded_SingleFrameIsPresented)
    {
        Fixture f;
        
        f.Load();
        f.Adapter->DoChanged();

        f.ExpectRender();
        f.RenderSingleFrame();

        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());
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

        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

        // Confirm that the update/render loop doesn't get restarted
        for (int i = 0; i < 5; ++i)
        {
            f.Adapter->DoChanged();
            Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());
            f.RenderSingleFrame();
            Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DestroyedWhileRenderLoopIsPending)
    {
        Fixture f;

        f.Load();
        f.Adapter->DoChanged();

        f.AllowAnyRendering();
        f.RenderSingleFrame();

        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());

        f.RaiseUnloadedEvent();

        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

        f.Control.Reset();
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
        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());

        // Suspending event should stop the render thread.
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));

        f.Adapter->Tick();
        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

        // Once rendering is stopped, Trim should be called.
        f.GetDevice()->TrimMethod.SetExpectedCalls(1);
        f.Adapter->DoChanged();

        // Render thread should not be restarted by DoChanged while app is in the suspended state.
        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenSuspendingEventRaisedWhileControlIsPaused_TrimIsStillCalled)
    {
        Fixture f;

        ThrowIfFailed(f.Control->put_Paused(true));
        f.Adapter->Tick();
        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));
        f.Adapter->Tick();

        f.GetDevice()->TrimMethod.SetExpectedCalls(1);
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenControlUnloadedDuringSuspendProcessing_TrimIsStillCalled)
    {
        Fixture f;

        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());

        // Send the suspending notification.
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, f.SuspendingEventArgs.Get()));

        // Unload the control while it is holding a suspension deferral.
        f.RaiseUnloadedEvent();

        // Trim should still happen.
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
        Assert::IsFalse(f.Adapter->GameThreadHasPendingWork());

        // Resume the app.
        ThrowIfFailed(f.Adapter->ResumingEventSource->InvokeAll(nullptr, nullptr));
        f.Adapter->DoChanged();
        Assert::IsTrue(f.Adapter->GameThreadHasPendingWork());
    }
};


TEST_CLASS(CanvasAnimatedControl_VisibilityTests)
{
    class Fixture : public FixtureWithSwapChainAccess
    {
    public:
        MockEventHandler<Animated_DrawEventHandler> OnDraw;
        MockEventHandler<Animated_UpdateEventHandler> OnUpdate;
        ComPtr<MockWindow> Window;

        Fixture()
        {
            Load();
            Adapter->DoChanged();

            Window = Adapter->GetCurrentMockWindow();

            OnDraw = MockEventHandler<Animated_DrawEventHandler>(L"Draw", ExpectedEventParams::Both);
            AddDrawHandler(OnDraw.Get());

            OnUpdate = MockEventHandler<Animated_UpdateEventHandler>(L"Update", ExpectedEventParams::Both);
            AddUpdateHandler(OnUpdate.Get());

            m_dxgiSwapChain->ResizeBuffersMethod.AllowAnyCall();
        }

        void DoNotExpectResizeBuffers()
        {
            m_dxgiSwapChain->ResizeBuffersMethod.SetExpectedCalls(0);
        }

        void ExpectOneResizeBuffers()
        {
            m_dxgiSwapChain->ResizeBuffersMethod.SetExpectedCalls(1);
        }

        void LoseDeviceDuringPresent()
        {
            m_dxgiSwapChain->Present1Method.SetExpectedCalls(1,
                [=](UINT, UINT, const DXGI_PRESENT_PARAMETERS*)
                {
                    Adapter->InitialDevice->MarkAsLost();
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisible_NoDrawOccurs)
    {
        Fixture f;

        f.Window->SetVisible(false);

        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(0);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisible_SwapchainNotResized)
    {
        Fixture f;

        f.Window->SetVisible(false);

        f.UserControl->Resize(Size{ 3, 4 });

        f.DoNotExpectResizeBuffers();
        f.OnUpdate.SetExpectedCalls(1);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisibleThenVisible_SwapchainResizedOnNextDrawWithUpdate)
    {
        Fixture f;

        f.Window->SetVisible(false);

        f.UserControl->Resize(Size{ 3, 4 });

        f.OnUpdate.SetExpectedCalls(1);
        f.RenderSingleFrame();

        f.Window->SetVisible(true);
        f.Adapter->ProgressTime(TicksPerFrame);
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);
        f.ExpectOneResizeBuffers();
        f.RenderSingleFrame();

    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisible_ClearColorChanged_NoDrawOccurs)
    {
        Fixture f;

        f.Window->SetVisible(false);

        f.Control->put_ClearColor(AnyColor);

        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(0);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisible_SizeChanged_NoDrawOccurs)
    {
        Fixture f;

        f.Window->SetVisible(false);

        f.UserControl->Resize(Size{ 3, 4 });

        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(0);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenDeviceLost_ThenInvisible_NoDrawOccurs)
    {
        Fixture f;

        f.LoseDeviceDuringPresent();
        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);
        f.RenderSingleFrame();

        f.Window->SetVisible(false);

        f.Adapter->DoChanged();

        f.OnUpdate.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(0);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_WhenInvisible_SwapChainStillExists_NoSleep)
    {
        //
        // Sleep doesn't occur because although the control is invisible,
        // the swap chain still exists, and can still be used to synchronize with
        // vertical blank.
        //

        Fixture f;

        f.Window->SetVisible(false);

        f.OnUpdate.SetExpectedCalls(1);

        f.Adapter->m_sleepFn = 
            [&](DWORD)
            {
                Assert::Fail(L"Unexpected");
            };

        f.RenderSingleFrame();
    }
};

TEST_CLASS(CanvasAnimatedControl_AppAccessingWorkerThreadTests)
{
    struct RunOnGameLoopFixture : public CanvasAnimatedControlFixture
    {
        RunOnGameLoopFixture()
        {
            Load();
            Adapter->DoChanged();
        }

        ~RunOnGameLoopFixture()
        {
            RaiseUnloadedEvent();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_HasGameLoopThreadAccess_NullArgs)
    {
        CanvasAnimatedControlFixture f;

        Assert::AreEqual(E_INVALIDARG, f.Control->get_HasGameLoopThreadAccess(nullptr));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_HasGameLoopThreadAccess_BeforeLoad)
    {
        CanvasAnimatedControlFixture f;

        boolean b;
        Assert::AreEqual(S_OK, f.Control->get_HasGameLoopThreadAccess(&b));
        Assert::IsFalse(!!b);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_HasGameLoopThreadAccess_CallsTheGameLoopsDispatcher)
    {
        CanvasAnimatedControlFixture f;

        f.Load();

        for (int i = 0; i < 2; ++i)
        {
            boolean expectedResult = (i == 0);

            f.Adapter->GetGameThreadDispatcher()->get_HasThreadAccessMethod.SetExpectedCalls(1,
                [=] (boolean* value)
                {
                    *value = expectedResult;
                    return S_OK;
                });

            boolean result;
            ThrowIfFailed(f.Control->get_HasGameLoopThreadAccess(&result));

            Assert::AreEqual(expectedResult, result, L"get_HasGameLoopThreadAccess returned the expected result");
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_NullArgs)
    {
        CanvasAnimatedControlFixture f;

        auto dispatchedHandler = Callback<IDispatcherQueueHandler>([] { return S_OK; });
        ComPtr<IAsyncAction> asyncAction;

        Assert::AreEqual(E_INVALIDARG, f.Control->RunOnGameLoopThreadAsync(dispatchedHandler.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Control->RunOnGameLoopThreadAsync(nullptr, &asyncAction));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_AsyncActionInvokedOnNextTick)
    {
        RunOnGameLoopFixture f;

        MockDispatcherQueueHandler dispatchedHandler;
        dispatchedHandler.OnInvoke.SetExpectedCalls(0);

        dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

        dispatchedHandler.OnInvoke.SetExpectedCalls(1);

        f.TickUntil([&] { return dispatchedHandler.OnInvoke.GetCurrentCallCount() == 1; });

        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_AsyncActionInvokedOnNextTick_WhenPaused)
    {
        RunOnGameLoopFixture f;

        ThrowIfFailed(f.Control->put_Paused(TRUE));

        // Wait until any outstanding ticks have completed
        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });


        MockDispatcherQueueHandler dispatchedHandler;

        dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

        dispatchedHandler.OnInvoke.SetExpectedCalls(1);

        f.Adapter->DoChanged();
        f.TickUntil([&] { return dispatchedHandler.OnInvoke.GetCurrentCallCount() == 1; });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_MultipleAsyncActionsInvokedInCorrectOrder)
    {
        static const int actionCount = 3;

        RunOnGameLoopFixture f;

        int callbackIndex = 0;
        MockDispatcherQueueHandler dispatchedHandlers[actionCount];
        for (int i = 0; i < actionCount; ++i)
        {
            dispatchedHandlers[i].OnInvoke.SetExpectedCalls(1,
                [i, &callbackIndex] 
                {
                    Assert::AreEqual(i, callbackIndex); 
                    callbackIndex++; 
                    return S_OK;
                });
            dispatchedHandlers[i].RunOnGameLoopThreadAsync(f.Control);
        }

        f.TickUntil([&] { return callbackIndex == actionCount; });
        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfHandlerThrowsDeviceRemoved_DeviceIsRecreated)
    {
        RunOnGameLoopFixture f;
                
        MockDispatcherQueueHandler dispatchedHandler;
        dispatchedHandler.OnInvoke.SetExpectedCalls(1,
            [&]
            {
                f.Adapter->InitialDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });
        dispatchedHandler.OnCompleted.SetExpectedCalls(1);

        dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

        // Tick until the tick loop stops
        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

        // It takes two rounds of DoChanged() for a device to get recreated
        f.Adapter->DoChanged();
        f.Adapter->DoChanged();

        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfHandlerThrowsDeviceRemoved_OtherAsyncActionsNotRunUntilDeviceRecreated)
    {
        static const int actionCount = 3;

        RunOnGameLoopFixture f;
        
        MockDispatcherQueueHandler dispatchedHandlers[actionCount];

        dispatchedHandlers[0].OnInvoke.SetExpectedCalls(1,
            [&]
            {
                f.Adapter->InitialDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        for (int i = 1; i < actionCount; ++i)
        {
            dispatchedHandlers[i].OnInvoke.SetExpectedCalls(0);
        }

        for (auto& h : dispatchedHandlers)
        {
            h.RunOnGameLoopThreadAsync(f.Control);
        }

        // Tick until the tick loop stops
        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

        // It takes two rounds of DoChanged() for a device to get recreated
        f.Adapter->DoChanged();
        f.Adapter->DoChanged();

        // Now the remaining handlers should get invoked
        for (int i = 1; i < actionCount; ++i)
        {
            dispatchedHandlers[i].OnInvoke.SetExpectedCalls(1);
        }

        f.TickUntil([&] { return dispatchedHandlers[actionCount-1].OnInvoke.GetCurrentCallCount() == 1; });

        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfHandlerThrowsSomeWeirdError_ActionStateIsSetCorrectly)
    {
        RunOnGameLoopFixture f;
        
        MockDispatcherQueueHandler dispatchedHandler;
        dispatchedHandler.OnInvoke.SetExpectedCalls(1, [] { return E_INVALIDARG; });
        dispatchedHandler.OnCompleted.SetExpectedCalls(1);

        dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

        f.TickUntil([&] { return dispatchedHandler.OnCompleted.GetCurrentCallCount() == 1; });
        f.VerifyTickLoopIsStillRunning();

        dispatchedHandler.VerifyActionStatus(AsyncStatus::Error, E_INVALIDARG);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfHandlerThrowsSomeWeirdError_OtherActionsAreStillRun)
    {
        static const int actionCount = 3;

        RunOnGameLoopFixture f;
        
        MockDispatcherQueueHandler dispatchedHandlers[actionCount];
        dispatchedHandlers[0].OnInvoke.SetExpectedCalls(1, [] { return E_INVALIDARG; });
        for (int i = 1; i < actionCount; ++i)
            dispatchedHandlers[i].OnInvoke.SetExpectedCalls(1);

        for (auto& h: dispatchedHandlers)
        {
            h.RunOnGameLoopThreadAsync(f.Control);
        }

        f.TickUntil([&] { return dispatchedHandlers[actionCount-1].OnInvoke.GetCurrentCallCount() == 1; });
        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfCompletedHandlerThrowsAnyError_ErrorIsRethrownByChangedHandler)
    {
        HRESULT errors[] = { E_INVALIDARG, DXGI_ERROR_DEVICE_REMOVED };

        for (HRESULT error : errors)
        {
            RunOnGameLoopFixture f;
            
            MockDispatcherQueueHandler dispatchedHandler;
            dispatchedHandler.OnInvoke.SetExpectedCalls(1);
            dispatchedHandler.OnCompleted.SetExpectedCalls(1, [=] (IAsyncAction*, AsyncStatus) { return error; });

            dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

            // Tick until the tick loop stops
            f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

            ExpectHResultException(error, [&] { f.Adapter->DoChanged(); });
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_IfCompletedHandlerThrowsAnError_OtherActionsNotRunThatTick)
    {
        static const int actionCount = 3;

        RunOnGameLoopFixture f;
        
        MockDispatcherQueueHandler dispatchedHandlers[actionCount];

        dispatchedHandlers[0].OnInvoke.SetExpectedCalls(1);
        dispatchedHandlers[0].OnCompleted.SetExpectedCalls(1, [] (IAsyncAction*, AsyncStatus) { return E_INVALIDARG; });

        for (int i = 1; i < actionCount; ++i)
            dispatchedHandlers[i].OnInvoke.SetExpectedCalls(0);

        for (auto& h : dispatchedHandlers)
            h.RunOnGameLoopThreadAsync(f.Control);

        f.TickUntil([&] { return dispatchedHandlers[0].OnInvoke.GetCurrentCallCount() == 1; });

        ExpectHResultException(E_INVALIDARG, [&] { f.Adapter->DoChanged(); });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_HandlerThrowsDeviceRemoved_AndCompletedHandlerThrowsSomeError_CompleteHandlerErrorIsIgnored)
    {
        // The design is for the 'device removed' to take precedence.
        RunOnGameLoopFixture f;
        
        MockDispatcherQueueHandler dispatchedHandler;
        dispatchedHandler.OnInvoke.SetExpectedCalls(1,
            [&]
            {
                f.Adapter->InitialDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });
        dispatchedHandler.OnCompleted.SetExpectedCalls(1, [] (IAsyncAction*, AsyncStatus) { return E_INVALIDARG; });

        dispatchedHandler.RunOnGameLoopThreadAsync(f.Control);

        // Tick until the tick loop stops
        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

        // if the completed handler's error had priority then this call to
        // DoChanged() would throw
        f.Adapter->DoChanged();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_AfterAnError_EventsAreRequeuedCorrectly)
    {
        // 
        // This test does the following:
        // RunOnGameLoopThreadAsync(#0);
        // RunOnGameLoopThreadAsync(#1); - throw here
        // RunOnGameLoopThreadAsync(#2);
        // RunOnGameLoopThreadAsync(#3);
        //
        // Tick();
        // 
        // RunOnGameLoopThreadAsync(#4);
        // RunOnGameLoopThreadAsync(#5);
        //
        // Tick();
        //
        //
        // Expected result:
        // First tick issues  #0, #1
        // Second tick issues #2, #3, #4, #5
        //
        // because #2 and #3 were requeued.
        //

        static const int actionCount = 6;
        MockDispatcherQueueHandler dispatchedHandlers[actionCount];

        RunOnGameLoopFixture f;
        
        dispatchedHandlers[0].OnInvoke.SetExpectedCalls(1);
        dispatchedHandlers[1].OnInvoke.SetExpectedCalls(1,
            [&]
            {
                f.Adapter->InitialDevice->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        dispatchedHandlers[0].RunOnGameLoopThreadAsync(f.Control);
        dispatchedHandlers[1].RunOnGameLoopThreadAsync(f.Control);
        dispatchedHandlers[2].RunOnGameLoopThreadAsync(f.Control);
        dispatchedHandlers[3].RunOnGameLoopThreadAsync(f.Control);

        // Tick until the tick loop stops
        f.TickUntil([&] { return !f.Adapter->GameThreadHasPendingWork(); });

        // This puts us down the device recreation path.
        f.Adapter->DoChanged();
        f.Adapter->DoChanged();
        
        dispatchedHandlers[2].OnInvoke.SetExpectedCalls(1);
        dispatchedHandlers[3].OnInvoke.SetExpectedCalls(1);
        dispatchedHandlers[4].OnInvoke.SetExpectedCalls(1);
        dispatchedHandlers[5].OnInvoke.SetExpectedCalls(1);

        dispatchedHandlers[4].RunOnGameLoopThreadAsync(f.Control);
        dispatchedHandlers[5].RunOnGameLoopThreadAsync(f.Control);

        f.TickUntil([&] { return dispatchedHandlers[5].OnInvoke.GetCurrentCallCount() == 1; });

        f.VerifyTickLoopIsStillRunning();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_ActionIsNotExecutedUntilCreateResourcesHasCompleted)
    {
        CanvasAnimatedControlFixture f;

        f.Load();

        MockDispatcherQueueHandler handler;

        handler.RunOnGameLoopThreadAsync(f.Control);

        auto onCreateResources = MockEventHandler<Animated_CreateResourcesEventHandler>(L"CreateResources", ExpectedEventParams::Both);
        f.AddCreateResourcesHandler(onCreateResources.Get());

        ComPtr<MockAsyncAction> asyncCreateResources;

        onCreateResources.SetExpectedCalls(1,
            [&](ICanvasAnimatedControl*, ICanvasCreateResourcesEventArgs* args)
            {
                asyncCreateResources = Make<MockAsyncAction>();
                return args->TrackAsyncAction(asyncCreateResources.Get());
            });

        f.Adapter->DoChanged();
        f.Adapter->Tick();

        Assert::IsTrue(asyncCreateResources, L"CreateResources started tracking async action");

        for (int i = 0; i < 10; ++i)
        {
            f.Adapter->DoChanged();
            f.Adapter->Tick();
        }

        // Only after the CreateResources has completed do we expect to see the handler we passed
        // to RunOnGameLoopThreadAsync get called
        asyncCreateResources->SetResult(S_OK);
        handler.OnInvoke.SetExpectedCalls(1);

        f.Adapter->DoChanged();
        f.TickUntil([&] { return handler.OnInvoke.GetCurrentCallCount() == 1; });
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_WhenCalledAndControlIsNotLoaded_ActionIsCanceled)
    {
        CanvasAnimatedControlFixture f;

        MockDispatcherQueueHandler handler;

        handler.OnInvoke.SetExpectedCalls(0);
        handler.OnCompleted.SetExpectedCalls(1,
            [=] (IAsyncAction*, AsyncStatus status)
            {
                Assert::AreEqual(AsyncStatus::Canceled, status);
                return S_OK;
            });

        handler.RunOnGameLoopThreadAsync(f.Control);
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_WhenUnloaded_AnyOutstandingAsyncActionsAreCanceled)
    {
        CanvasAnimatedControlFixture f;

        std::vector<MockDispatcherQueueHandler> handlers;
        for (int i = 0; i < 10; ++i)
        {
            MockDispatcherQueueHandler handler;
            handler.OnInvoke.SetExpectedCalls(0);
            handler.OnCompleted.SetExpectedCalls(1,
                [] (IAsyncAction*, AsyncStatus status)
                {
                    Assert::AreEqual(AsyncStatus::Canceled, status);
                    return S_OK;
                });
            handlers.push_back(std::move(handler));
        }

        f.Load();

        for (auto& h : handlers)
        {
            h.RunOnGameLoopThreadAsync(f.Control);
        }

        f.RaiseUnloadedEvent();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_RunOnGameLoopThreadAsync_WhenActionIsCanceled_HandlerIsNotCalled)
    {
        CanvasAnimatedControlFixture f;

        f.Load();

        MockDispatcherQueueHandler handler;
        handler.OnInvoke.SetExpectedCalls(0);
        handler.OnCompleted.SetExpectedCalls(1,
            [] (IAsyncAction*, AsyncStatus status)
            {
                Assert::AreEqual(AsyncStatus::Canceled, status);
                return S_OK;
            });

        handler.RunOnGameLoopThreadAsync(f.Control);
        handler.CancelAction();

        f.Adapter->DoChanged();
        f.Adapter->Tick();
    }


    struct DispatcherFixture : public CanvasAnimatedControlFixture
    {
        MockEventHandler<Animated_CreateResourcesEventHandler> OnCreateResources;

        DispatcherFixture()
            : OnCreateResources(L"OnCreateResources")
        {
            AddCreateResourcesHandler(OnCreateResources.Get());
            OnCreateResources.AllowAnyCall();
        }

        ComPtr<MockAsyncAction> SetupAsyncCreateResources()
        {
            auto action = Make<MockAsyncAction>();
            OnCreateResources.SetExpectedCalls(1,
                [=] (IInspectable*, ICanvasCreateResourcesEventArgs* args)
                {
                    return args->TrackAsyncAction(action.Get());
                });
            return action;
        }

        void Run()
        {
            for (int i = 0; i < 10; ++i)
            {
                Adapter->DoChanged();
                Adapter->Tick();
            }
        }

        typedef CallCounterWithMock<DefaultCallCounterTraits, HRESULT()> HResultCallCounter;

        std::shared_ptr<HResultCallCounter> RunOnDispatcher()
        {
            auto mock = std::make_shared<HResultCallCounter>(L"RunAsync handler");
            
            auto handler = Callback<IDispatcherQueueHandler>([=] { return mock->WasCalled(); });
            
            boolean ignoredResult;
            
            ThrowIfFailed(Adapter->GetGameThreadDispatcher()->TryEnqueueWithPriority(DispatcherQueuePriority_Normal, handler.Get(), &ignoredResult));
            
            return mock;
        }

        std::shared_ptr<HResultCallCounter> RunOnGameLoop()
        {
            auto mock = std::make_shared<HResultCallCounter>(L"RunOnGameLoopThreadAsync handler");

            auto handler = Callback<IDispatcherQueueHandler>([=] { return mock->WasCalled(); });
            
            ComPtr<IAsyncAction> ignoredAction;
            
            ThrowIfFailed(Control->RunOnGameLoopThreadAsync(handler.Get(), &ignoredAction));
            
            return mock;
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_DispatcherDoesNotProcessEventsUntilCreateResourcesCompleted)
    {
        DispatcherFixture f;

        auto createResourcesAsyncAction = f.SetupAsyncCreateResources();

        f.Load();

        auto asyncHandler = f.RunOnDispatcher();
        asyncHandler->SetExpectedCalls(0);

        f.Run();

        createResourcesAsyncAction->SetResult(S_OK);
        asyncHandler->SetExpectedCalls(1);

        f.Run();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DispatcherStopsProcessingEventsAfterDeviceLost)
    {
        DispatcherFixture f;

        f.Load();
        f.Run();

        // Lose the device
        f.Adapter->InitialDevice->MarkAsLost();
        auto gameLoopAction = f.RunOnGameLoop();
        gameLoopAction->SetExpectedCalls(1, [] { return DXGI_ERROR_DEVICE_REMOVED; });

        f.Adapter->Tick();      // this'll cause gameLoopAction to run and start the lost-device handling

        // This will prevent anything queued on the dispatcher from running
        auto createResourcesAsyncAction = f.SetupAsyncCreateResources();

        auto asyncHandler = f.RunOnDispatcher();
        asyncHandler->SetExpectedCalls(0);
        
        f.Run();

        // The async handler won't get called until create resources has
        // completed
        createResourcesAsyncAction->SetResult(S_OK);
        asyncHandler->SetExpectedCalls(1);

        f.Run();
    }
    
    //
    // These test the interaction between CanvasAnimatedControl and
    // IGameLoopThread; another test verifies that the production implementation
    // of IGameLoopThread raises the event at an appropriate time.
    //

    TEST_METHOD_EX(CanvasAnimatedControl_GameLoopStartingEventIsRaised)
    {
        CanvasAnimatedControlFixture f;

        auto startingHandler = MockEventHandler<ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>>(L"GameLoopStarting");
        startingHandler.SetExpectedCalls(1,
            [&] (ICanvasAnimatedControl* control, IInspectable* args)
            {
                Assert::IsTrue(IsSameInstance(control, f.Control.Get()));
                Assert::IsNull(args);
                return S_OK;
            });
        
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_GameLoopStarting(startingHandler.Get(), &token));

        f.Load();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_GameLoopStoppedEventIsRaised)
    {
        CanvasAnimatedControlFixture f;

        auto stoppedHandler = MockEventHandler<ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>>(L"GameLoopStopped");
        stoppedHandler.SetExpectedCalls(1,
            [&] (ICanvasAnimatedControl* control, IInspectable* args)
            {
                Assert::IsTrue(IsSameInstance(control, f.Control.Get()));
                Assert::IsNull(args);
                return S_OK;
            });
        
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_GameLoopStopped(stoppedHandler.Get(), &token));

        f.Load();
        f.RaiseUnloadedEvent();
    }
};

TEST_CLASS(CanvasAnimatedControl_DpiScaling)
{
    class DpiScalingFixture : public FixtureWithSwapChainAccess
    {
    public:
        DpiScalingFixture(float logicalDpi = DEFAULT_DPI)
        {
            Adapter->LogicalDpi = logicalDpi;
        }

        void ExpectCreateSwapChainWithDpi(float expectedDpi)
        {
            Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1, 
                [=](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
                {
                    StubCanvasDevice* stubDevice = static_cast<StubCanvasDevice*>(device); // Ensured by test construction
                    stubDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
                    {
                        return m_dxgiSwapChain;
                    });

                    Assert::AreEqual(dpi, expectedDpi);

                    return Make<CanvasSwapChain>(device, m_dxgiSwapChain.Get(), dpi, /* isTransformMatrixSupported */ true);
                });
        }

        void ExpectResizeBuffersWithDpi(float expectedDpi)
        {
            const float dpiScale = expectedDpi / DEFAULT_DPI;
            const uint32_t expectedWidthInPixels = static_cast<uint32_t>(FixtureWithSwapChainAccess::InitialWidth * dpiScale);
            const uint32_t expectedHeightInPixels = static_cast<uint32_t>(FixtureWithSwapChainAccess::InitialHeight * dpiScale);

            m_dxgiSwapChain->ResizeBuffersMethod.SetExpectedCalls(1,
                [=](UINT bufferCount,
                    UINT width,
                    UINT height,
                    DXGI_FORMAT newFormat,
                    UINT swapChainFlags)
                {
                    Assert::AreEqual(expectedWidthInPixels, width);
                    Assert::AreEqual(expectedHeightInPixels, height);
                    return S_OK;
                });
        }

        void EnsureSwapChain()
        {
            Load();
            Adapter->DoChanged();
        }
    };

// Test removed, as DPI now comes from the XamlRoot instead of the adapter
#ifndef WINUI3
    TEST_METHOD_EX(CanvasAnimatedControl_DpiScaling_Affects_SwapChainDpi_InitialCreate)
    {
        for (auto testCase : dpiScalingTestCases)
        {
            DpiScalingFixture f(testCase.Dpi);

            f.Control->put_DpiScale(testCase.DpiScale);
            f.ExpectCreateSwapChainWithDpi(testCase.DpiScale * testCase.Dpi);

            f.EnsureSwapChain();
        }
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DpiScaling_Affects_SwapChainDpi_ResizeBuffers)
    {
        for (auto testCase : dpiScalingTestCases)
        {
            DpiScalingFixture f(testCase.Dpi);
            f.EnsureSwapChain();

            f.Adapter->ProgressTime(TicksPerFrame);

            f.Control->put_DpiScale(testCase.DpiScale);

            f.Adapter->Tick(); // Render thread notices the dpi scale change, and stops

            f.ExpectResizeBuffersWithDpi(testCase.DpiScale * testCase.Dpi);
            f.Adapter->DoChanged();
        }
    }
#endif
};

//
// In the XAML designer, background threads, dispatchers and swapchains are not
// supported.  Since CanvasAnimatedControl relies on these it doesn't work well
// in the designer.  At the very least we want the clear color to be displayed.
//
TEST_CLASS(CanvasAnimatedControl_DesignMode)
{
    struct Fixture : public CanvasAnimatedControlFixture
    {
        Fixture()
        {
            Adapter->DesignModeEnabled = true;

            CreateControl();
        }
    };

    TEST_METHOD_EX(CanvasAnimatedControl_DesignMode_ContentIsARectangle)
    {
        Fixture f;

        ComPtr<IUIElement> actualContent;
        ThrowIfFailed(f.UserControl->get_Content(&actualContent));

        Assert::IsTrue(IsSameInstance(actualContent.Get(), f.Adapter->GetShape()));        
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DesignMode_MostOperationsAreNoOps)
    {
        Fixture f;

        f.Load();
        f.Adapter->DoChanged();

        // In test.internal, RemoveFromVisualTree will fail because we don't
        // actually have an underlying XAML control to remove.  However, this
        // method should not AV: it should fail with this specific error code.
        Assert::AreEqual(E_NOINTERFACE, f.Control->RemoveFromVisualTree());
        
        f.RaiseUnloadedEvent();
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DesignMode_CreateCoreIndependentInputSource_Fails)
    {
        Fixture f;

        auto anyDeviceTypes = static_cast<InputPointerSourceDeviceKinds>(123);
        auto anyReturnValue = reinterpret_cast<IInputPointerSource**>(456);

        Assert::AreEqual(E_NOTIMPL, f.Control->CreateCoreIndependentInputSource(anyDeviceTypes, anyReturnValue));
    }

    TEST_METHOD_EX(CanvasAnimatedControl_DesignMode_SettingClearColor_SetsTheRectanglesFillColor)
    {
        Fixture f;

        Color anyColor{ 1, 2, 3, 4 };

        auto brush = Make<MockXamlSolidColorBrush>();

        f.Adapter->GetShape()->get_FillMethod.SetExpectedCalls(2,
            [&] (IBrush** b)
            {
                return brush.CopyTo(b);
            });

        brush->put_ColorMethod.SetExpectedCalls(1,
            [&] (Color color)
            {
                Assert::AreEqual(anyColor, color);
                return S_OK;
            });
        
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        brush->get_ColorMethod.SetExpectedCalls(1,
            [&] (Color* color)
            {
                *color = anyColor;
                return S_OK;
            });

        Color retrievedColor;
        ThrowIfFailed(f.Control->get_ClearColor(&retrievedColor));
        Assert::AreEqual(anyColor, retrievedColor);
    }
};

#endif