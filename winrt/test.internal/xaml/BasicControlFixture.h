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

#pragma once

template<typename TRAITS>
struct BasicControlFixture
{
    typedef typename TestAdapter<TRAITS>::type             adapter_t;
    typedef typename TRAITS::control_t                     control_t;
    typedef typename TRAITS::createResourcesEventHandler_t createResourcesEventHandler_t;
    typedef typename TRAITS::drawEventHandler_t            drawEventHandler_t;

    static int const InitialWidth = 100;
    static int const InitialHeight = 200;

    std::shared_ptr<adapter_t> Adapter;
    ComPtr<control_t> Control;
    ComPtr<StubUserControl> UserControl;

    BasicControlFixture()
    {
    }

    BasicControlFixture(BasicControlFixture const&) = delete;
    BasicControlFixture& operator=(BasicControlFixture const&) = delete;

    virtual ~BasicControlFixture() = default;

    virtual void CreateAdapter()
    {
        Adapter = std::make_shared<adapter_t>();
    }

    void CreateControl()
    {
        Control = Make<control_t>(Adapter);
        UserControl = dynamic_cast<StubUserControl*>(As<IUserControl>(Control).Get());
    }

    void Load()
    {
        UserControl->Resize(Size{InitialWidth, InitialHeight});
        ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
    }

    void RaiseUnloadedEvent()
    {
        ThrowIfFailed(UserControl->UnloadedEventSource->InvokeAll(nullptr, nullptr));
    }

    void RenderSingleFrame();

    void RenderAnyNumberOfFrames()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            RenderSingleFrame();
    }

    EventRegistrationToken AddCreateResourcesHandler(createResourcesEventHandler_t* handler)
    {
        return AddEventHandler(&control_t::add_CreateResources, handler);
    }

    EventRegistrationToken AddDrawHandler(drawEventHandler_t* handler)
    {
        return AddEventHandler(&control_t::add_Draw, handler);
    }

protected:
    template<typename T, typename HANDLER>
    EventRegistrationToken AddEventHandler(HRESULT (STDMETHODCALLTYPE T::* addMethod)(HANDLER*, EventRegistrationToken*), HANDLER* handler)
    {
        EventRegistrationToken token;
        ThrowIfFailed((Control.Get()->*addMethod)(handler, &token));
        return token;
    }
};

inline void BasicControlFixture<CanvasControlTraits>::RenderSingleFrame()
{
    Adapter->RaiseCompositionRenderingEvent();
}


inline void BasicControlFixture<CanvasAnimatedControlTraits>::RenderSingleFrame()
{
    Adapter->Tick();
}

struct Static_BasicControlFixture : public BasicControlFixture<CanvasControlTraits>
{
    void RaiseAnyNumberOfSurfaceContentsLostEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            Adapter->RaiseSurfaceContentsLostEvent();
    }    
};

typedef BasicControlFixture<CanvasAnimatedControlTraits> Animated_BasicControlFixture;

template<typename T>
struct ControlFixture;

template<>
struct ControlFixture<CanvasControlTraits> : public Static_BasicControlFixture
{
    ControlFixture()
    {
        CreateAdapter();
        CreateControl();
    }
};

template<>
struct ControlFixture<CanvasAnimatedControlTraits> : public Animated_BasicControlFixture
{
    ComPtr<MockD2DDeviceContext> DeviceContext;

    ControlFixture()
        : DeviceContext(Make<MockD2DDeviceContext>())
    {
        CreateAdapter();
        CreateControl();
    }

    virtual void CreateAdapter() override
    {
        Adapter = CreateAnimatedControlTestAdapter(DeviceContext);
    }

    EventRegistrationToken AddUpdateHandler(Animated_UpdateEventHandler* handler)
    {
        return AddEventHandler(&CanvasAnimatedControl::add_Update, handler);
    }

    void ExpectOneCreateSwapChain()
    {
        auto swapChainManager = Adapter->SwapChainManager;

        Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1,
            [swapChainManager](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                return CreateTestSwapChain(swapChainManager, device);
            });
    }

    bool IsRenderActionRunning()
    {
        return static_cast<bool>(Adapter->m_outstandingWorkItemAsyncAction);
    }

    bool IsChangedActionRunning()
    {
        return Adapter->HasPendingChangedActions();
    }

    void RaiseLoadedAndVerify()
    {
        Assert::IsFalse(IsChangedActionRunning());
        Load();
        Assert::IsTrue(IsChangedActionRunning());
        Adapter->DoChanged();
        Assert::IsFalse(IsChangedActionRunning());
    }

    void ExpectStableState()
    {
        Assert::IsFalse(IsChangedActionRunning());
        Assert::IsTrue(IsRenderActionRunning());

        Adapter->Tick();

        Assert::IsFalse(IsChangedActionRunning());
        Assert::IsTrue(IsRenderActionRunning());
    }

    static
    ComPtr<CanvasSwapChain> CreateTestSwapChain(
        std::shared_ptr<CanvasSwapChainManager> const& swapChainManager,
        ICanvasDevice* device)
    {
        return swapChainManager->Create(
            device,
            1.0f,
            1.0f,
            DEFAULT_DPI,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            2,
            CanvasAlphaMode::Premultiplied);
    }
};


typedef ControlFixture<CanvasControlTraits> CanvasControlFixture;
typedef ControlFixture<CanvasAnimatedControlTraits> CanvasAnimatedControlFixture;


template<class TRAITS>
struct ControlFixtureWithRecreatableDeviceManager : public ControlFixture<TRAITS>
{
    MockRecreatableDeviceManager<TRAITS>* DeviceManager;
    std::function<void(ChangeReason)> ChangedCallback;

    ControlFixtureWithRecreatableDeviceManager()
        : DeviceManager(nullptr)
    {
        CreateAdapter();

        Adapter->CreateRecreatableDeviceManagerMethod.SetExpectedCalls(1,
            [=]
            {
                Assert::IsNull(DeviceManager);
                auto manager = std::make_unique<MockRecreatableDeviceManager<TRAITS>>();
                manager->SetChangedCallbackMethod.SetExpectedCalls(1,
                    [=](std::function<void(ChangeReason)> fn)
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
    
