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

#include "CanvasControlTestAdapter.h"
#include "MockAsyncAction.h"

struct BasicControlFixture
{
    std::shared_ptr<CanvasControlTestAdapter> Adapter;
    ComPtr<CanvasControl> Control;
    ComPtr<StubUserControl> UserControl;

    BasicControlFixture()
        : Adapter(std::make_shared<CanvasControlTestAdapter>())
    {
    }

    void CreateControl()
    {
        Control = Make<CanvasControl>(Adapter);
        UserControl = dynamic_cast<StubUserControl*>(As<IUserControl>(Control).Get());
    }

    void RaiseLoadedEvent()
    {
        ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
    }

    void RaiseCompositionRenderingEvent()
    {
        Adapter->RaiseCompositionRenderingEvent();
    }

    void RaiseAnyNumberOfSurfaceContentsLostEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            Adapter->RaiseSurfaceContentsLostEvent();
    }
    
    void RaiseAnyNumberOfCompositionRenderingEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            Adapter->RaiseCompositionRenderingEvent();
    }

    EventRegistrationToken AddCreateResourcesHandler(CreateResourcesEventHandler* handler)
    {
        return AddEventHandler(&CanvasControl::add_CreateResources, handler);
    }

    EventRegistrationToken AddDrawHandler(DrawEventHandler* handler)
    {
        return AddEventHandler(&CanvasControl::add_Draw, handler);
    }

private:
    template<typename T, typename HANDLER>
    EventRegistrationToken AddEventHandler(HRESULT (STDMETHODCALLTYPE T::* addMethod)(HANDLER*, EventRegistrationToken*), HANDLER* handler)
    {
        EventRegistrationToken token;
        ThrowIfFailed((Control.Get()->*addMethod)(handler, &token));
        return token;
    }
};

struct CanvasControlFixture : public BasicControlFixture
{
    CanvasControlFixture()
    {
        CreateControl();
    }

    CanvasControlFixture(CanvasControlFixture const&) = delete;
    CanvasControlFixture& operator=(CanvasControlFixture const&) = delete;
};


TEST_CLASS(CanvasControlTests_CommonAdapter)
{
    TEST_METHOD_EX(CanvasControl_Implements_Expected_Interfaces)
    {
        CanvasControlFixture f;
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ICanvasControl);
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ABI::Windows::UI::Xaml::Controls::IUserControl);
        ASSERT_IMPLEMENTS_INTERFACE(f.Control, ICanvasResourceCreator);
    }

    TEST_METHOD_EX(CanvasControl_DeviceProperty_Null)
    {
        CanvasControlFixture f;
        Assert::AreEqual(E_INVALIDARG, f.Control->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasControl_DrawEventArgs_Getter)
    {
        ComPtr<ICanvasDrawingSession> drawingSession = Make<MockCanvasDrawingSession>();

        auto drawEventArgs = Make<CanvasDrawEventArgs>(drawingSession.Get());

        // Verify that an exception is returned for nullptr on getter
        Assert::AreEqual(E_INVALIDARG, drawEventArgs->get_DrawingSession(nullptr));

        // Verify that the getter in a typical case works
        ComPtr<ICanvasDrawingSession> drawingSessionRetrieved;
        drawEventArgs->get_DrawingSession(&drawingSessionRetrieved);

        Assert::AreEqual(drawingSession.Get(), drawingSessionRetrieved.Get());
    }

    TEST_METHOD_EX(CanvasControl_WhenInvalidateIsCalledBeforeLoadedEvent_ThenNothingBadHappens)
    {
        CanvasControlFixture f;

        ThrowIfFailed(f.Control->Invalidate());
    }

    TEST_METHOD_EX(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;

        CanvasControlFixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreateResources = MockEventHandler<CreateResourcesEventHandler>(L"CreateResources", ExpectedEventParams::Both);
        auto createResourcesEventToken0 = f.AddCreateResourcesHandler(onCreateResources.Get());

        // Before the control received the Loaded event,
        // CompositionRenderingEvents should essentially be ignored.
        f.RaiseCompositionRenderingEvent();

        // Issue a Loaded.  Now, the next CompositionRenderingEvent will cause
        // CreateResources to be raised.
        onCreateResources.SetExpectedCalls(1);
        f.RaiseLoadedEvent();
        f.RaiseCompositionRenderingEvent();

        onCreateResources.Validate();
 
        // Register the CreateResources handler again.  Because the device has
        // already been created, add_CreateResources should immediately fire the
        // event too.
        onCreateResources.SetExpectedCalls(1);

        auto createResourcesEventToken1 = f.AddCreateResourcesHandler(onCreateResources.Get());

        onCreateResources.Validate();

        // Register the Draw handler.
        auto onDraw = MockEventHandler<DrawEventHandler>(L"Draw", ExpectedEventParams::Both);

        auto drawEventToken = f.AddDrawHandler(onDraw.Get());

        // Invalidate and ensure the Draw callback is called.
        onDraw.SetExpectedCalls(1);
        f.Control->Invalidate();
        f.RaiseCompositionRenderingEvent();
        onDraw.Validate();

        // Ensure a subsequent invalidation doesn't recreate resources.
        onDraw.SetExpectedCalls(1);
        f.Control->Invalidate();
        f.RaiseCompositionRenderingEvent();

        // Unregister the events. Call invalidate. Ensure the handler doesn't get called again (the event was correctly unregistered).
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(f.Control->remove_Draw(drawEventToken));
        f.Control->Invalidate();
        f.RaiseCompositionRenderingEvent();

        // Unregistering the same event twice should do nothing.
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(f.Control->remove_Draw(drawEventToken));
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAdded_RedrawIsTriggered)
    {
        CanvasControlFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        f.RaiseLoadedEvent();
        f.RaiseCompositionRenderingEvent();

        auto onDraw1 = MockEventHandler<DrawEventHandler>(L"Draw1");
        f.AddDrawHandler(onDraw1.Get());

        onDraw1.SetExpectedCalls(1);

        f.RaiseCompositionRenderingEvent();

        auto onDraw2 = MockEventHandler<DrawEventHandler>(L"Draw2");
        f.AddDrawHandler(onDraw2.Get());

        onDraw1.SetExpectedCalls(1);
        onDraw2.SetExpectedCalls(1);

        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAddedBeforeLoadedEvent_RedrawIsTriggeredAfterLoadedEvent)
    {
        CanvasControlFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        auto onDraw = MockEventHandler<DrawEventHandler>(L"Draw");
        f.AddDrawHandler(onDraw.Get());

        f.RaiseCompositionRenderingEvent();
        f.RaiseLoadedEvent();

        onDraw.SetExpectedCalls(1);
        f.RaiseCompositionRenderingEvent();
    }
};


class MockRecreatableDeviceManager : public ICanvasControlRecreatableDeviceManager
{
    ComPtr<ICanvasDevice> m_device;

public:
    CALL_COUNTER_WITH_MOCK(SetChangedCallbackMethod, void(std::function<void()>));
    CALL_COUNTER_WITH_MOCK(RunWithDeviceMethod, void(CanvasControl*, RunWithDeviceFunction));
    CALL_COUNTER_WITH_MOCK(IsReadyToDrawMethod, bool());
    CALL_COUNTER_WITH_MOCK(AddCreateResourcesMethod, EventRegistrationToken(CanvasControl*, CreateResourcesEventHandler*));
    CALL_COUNTER_WITH_MOCK(RemoveCreateResourcesMethod, void(EventRegistrationToken));

    virtual void SetChangedCallback(std::function<void()> fn) override
    {
        return SetChangedCallbackMethod.WasCalled(fn);
    }

    virtual void RunWithDevice(CanvasControl* sender, RunWithDeviceFunction fn) override
    {
        return RunWithDeviceMethod.WasCalled(sender, fn);
    }

    void SetRunWithDeviceFlags(RunWithDeviceFlags flags, int expectedCalls)
    {
        RunWithDeviceMethod.SetExpectedCalls(expectedCalls,
            [=](CanvasControl*, RunWithDeviceFunction fn)
            {
                fn(m_device.Get(), flags);
            });
    }

    void SetRunWithDeviceFlags(RunWithDeviceFlags flags)
    {
        RunWithDeviceMethod.AllowAnyCall(
            [=](CanvasControl*, RunWithDeviceFunction fn)
            {
                fn(m_device.Get(), flags);
            });
    }

    void SetDevice(ComPtr<ICanvasDevice> device)
    {
        m_device = device;
    }

    virtual ComPtr<ICanvasDevice> const& GetDevice() override
    {
        return m_device;
    }

    virtual bool IsReadyToDraw() override
    {
        return IsReadyToDrawMethod.WasCalled();
    }
    
    virtual EventRegistrationToken AddCreateResources(CanvasControl* sender, CreateResourcesEventHandler* value) override
    {
        return AddCreateResourcesMethod.WasCalled(sender, value);
    }

    virtual void RemoveCreateResources(EventRegistrationToken token) override
    {
        return RemoveCreateResourcesMethod.WasCalled(token);
    }
};


TEST_CLASS(CanvasControlTests_InteractionWithRecreatableDeviceManager)
{
    struct Fixture : public BasicControlFixture
    {
        MockRecreatableDeviceManager* DeviceManager;
        std::function<void()> ChangedCallback;

        Fixture()
            : DeviceManager(nullptr)
        {
            Adapter->CreateRecreatableDeviceManagerMethod.SetExpectedCalls(1,
                [=]
                {
                    Assert::IsNull(DeviceManager);
                    auto manager = std::make_unique<MockRecreatableDeviceManager>();
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
    };

    TEST_METHOD_EX(CanvasControl_WhenSuspendingEventRaised_TrimCalledOnDevice)
    {
        Fixture f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        anyDevice->TrimMethod.SetExpectedCalls(1);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }

    TEST_METHOD_EX(CanvasControl_WhenSuspendingEventRaisedAndThereIsNoDevice_NothingBadHappens)
    {
        Fixture f;

        ComPtr<ICanvasDevice> nullDevice;
        f.DeviceManager->SetDevice(nullDevice);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }

    TEST_METHOD_EX(CanvasControl_add_CreateResources_ForwardsToDeviceManager)
    {
        Fixture f;

        EventRegistrationToken anyToken{0x1234567890ABCDEF};
        auto anyHandler = Callback<CreateResourcesEventHandler>(
            [](ICanvasControl*, IInspectable*) { return S_OK; } );

        f.DeviceManager->AddCreateResourcesMethod.SetExpectedCalls(1,
            [&](CanvasControl* control, CreateResourcesEventHandler* handler)
            {
                Assert::IsTrue(IsSameInstance(f.Control.Get(), control));
                Assert::IsTrue(IsSameInstance(anyHandler.Get(), handler));
                return anyToken;
            });

        EventRegistrationToken actualToken;
        ThrowIfFailed(f.Control->add_CreateResources(anyHandler.Get(), &actualToken));

        Assert::AreEqual(anyToken.value, actualToken.value);
    }

    TEST_METHOD_EX(CanvasControl_remove_CreateResources_ForwardsToDeviceManager)
    {
        Fixture f;

        EventRegistrationToken anyToken{0x1234567890ABCDEF};
        f.DeviceManager->RemoveCreateResourcesMethod.SetExpectedCalls(1,
            [&](EventRegistrationToken token)
            {
                Assert::AreEqual(anyToken.value, token.value);
            });

        ThrowIfFailed(f.Control->remove_CreateResources(anyToken));
    }

    TEST_METHOD_EX(CanvasControl_get_ReadyToDraw_ForwardsToDeviceManager)
    {
        Fixture f;

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

    TEST_METHOD_EX(CanvasControl_get_Device_ForwardsToDeviceManager)
    {
        Fixture f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(f.Control->get_Device(&device));

        Assert::IsTrue(IsSameInstance(anyDevice.Get(), device.Get()));
    }

    TEST_METHOD_EX(CanvasControl_WhenGetDeviceReturnsNull_get_Device_ReportsUnderstandableErrorMessage)
    {
        Fixture f;

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(E_INVALIDARG, f.Control->get_Device(&device));

        ValidateStoredErrorState(E_INVALIDARG, L"The CanvasControl does not currently have a CanvasDevice associated with it. "
            L"Ensure that resources are created from a CreateResources or Draw event handler.");
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawing_RunWithDrawIsPassedTheCorrectControl)
    {
        Fixture f;

        f.DeviceManager->RunWithDeviceMethod.SetExpectedCalls(1,
            [&](CanvasControl* control, RunWithDeviceFunction)
            {
                Assert::IsTrue(IsSameInstance(f.Control.Get(), control));
            });

        f.RaiseLoadedEvent();
        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDeviceIsNewlyCreated_ANewImageSourceIsCreated)
    {
        Fixture f;
        f.RaiseLoadedEvent();

        auto anyDevice = Make<StubCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::NewlyCreatedDevice, 2);

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(2,
            [&](ICanvasDevice* device, float, float, float, CanvasBackground)
            {
                Assert::IsTrue(IsSameInstance(anyDevice.Get(), device));
                return nullptr;
            });

        f.RaiseCompositionRenderingEvent();

        // Second render, image source is created again
        anyDevice = Make<StubCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        ThrowIfFailed(f.Control->Invalidate());
        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDeviceIsNotNewlyCreated_NoImageSourceIsCreated)
    {
        Fixture f;
        f.RaiseLoadedEvent();

        auto anyDevice = Make<StubCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 2);

        // First render, the image source is created
        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1);
        f.RaiseCompositionRenderingEvent();

        // Second render, no new image source is created
        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(0);
        ThrowIfFailed(f.Control->Invalidate());
        f.RaiseCompositionRenderingEvent();
    }

    struct LoadedFixture : public Fixture
    {
        ComPtr<StubCanvasDevice> Device;

        LoadedFixture()
            : Device(Make<StubCanvasDevice>())
        {
            RaiseLoadedEvent();
            DeviceManager->SetDevice(Device);
        }
    };

    TEST_METHOD_EX(CanvasControl_WhenRendering_DrawingSessionIsCreatedOnImageSource_RegardlessOfFlags)
    {
        LoadedFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        int maxFlag = static_cast<int>(RunWithDeviceFlags::NewlyCreatedDevice | RunWithDeviceFlags::ResourcesNotCreated);

        for (int i = 0; i <= maxFlag; ++i)
        {
            VerifyDrawingSessionCreated(&f, static_cast<RunWithDeviceFlags>(i));
        }
    }

    void VerifyDrawingSessionCreated(LoadedFixture* f, RunWithDeviceFlags flags)
    {
        CallCounter<> createDrawingSessionMethod(L"CanvasImageSourceDrawingSessionFactory::Create");
        createDrawingSessionMethod.SetExpectedCalls(1);

        f->Adapter->OnCanvasImageSourceDrawingSessionFactory_Create =
            [&] 
            { 
                createDrawingSessionMethod.WasCalled(); 
                return nullptr;
            };

        f->DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 1);
        ThrowIfFailed(f->Control->Invalidate());
        f->RaiseCompositionRenderingEvent();
    }

    struct DrawFixture : public LoadedFixture
    {
        MockEventHandler<DrawEventHandler> OnDraw;

        DrawFixture()
            : OnDraw(L"OnDraw")
        {
            Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

            AddDrawHandler(OnDraw.Get());
            RaiseLoadedEvent();
        }
    };

    TEST_METHOD_EX(CanvasControl_WhenResourcesAreNotCreated_DrawHandlersAreNotCalled)
    {
        DrawFixture f;

        f.OnDraw.SetExpectedCalls(0);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::ResourcesNotCreated, 1);

        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenResourcesAreCreated_DrawHandlersAreCalled)
    {
        DrawFixture f;

        f.OnDraw.SetExpectedCalls(1);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 1);

        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenChangedCallbackIsCalled_RedrawIsTriggered)
    {
        DrawFixture f;
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None);

        f.OnDraw.SetExpectedCalls(1);
        f.RaiseAnyNumberOfCompositionRenderingEvents();

        f.ChangedCallback();

        f.OnDraw.SetExpectedCalls(1);
        f.RaiseAnyNumberOfCompositionRenderingEvents();        
    }
};


TEST_CLASS(CanvasControlTests_SizeTests)
{
    static const int anyWidth = 128;
    static const int anyHeight = 256;
    static const int zeroWidth = 0;
    static const int zeroHeight = 0;

    TEST_METHOD_EX(CanvasControl_Resizing)
    {
        struct TestCase
        {
            int ResizeWidth;
            int ResizeHeight;
            bool ExpectDraw;
            bool ExpectRecreation;
        } testSteps[]
        {
            { 100, 100,  true,  true }, // Initial sizing; resource always re-created
            { 123, 456,  true,  true }, // Change width and height
            {  50, 456,  true,  true }, // Change width only
            {  50,  51,  true,  true }, // Change height only
            {  50,  51, false, false }, // Change nothing
        };

        ResizeFixture f;

        for (auto const& testStep : testSteps)
        {
            if (testStep.ExpectRecreation) 
                f.ExpectOneCreateCanvasImageSource((float)testStep.ResizeWidth, (float)testStep.ResizeHeight);

            if (testStep.ExpectDraw)
                f.ExpectOneDrawEvent();

            f.Execute(testStep.ResizeWidth, testStep.ResizeHeight);
        }
    }

    TEST_METHOD_EX(CanvasControl_ZeroSizedControl_DoesNotCreateImageSource_DoesNotCallDrawHandler)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, zeroHeight);
    }

    TEST_METHOD_EX(CanvasControl_ResizeToZeroSize_ClearsImageSource_DoesNotCallDrawHandler)
    {
        ResizeFixture f;

        f.ExpectOneDrawEvent();
        f.ExpectOneCreateCanvasImageSource((float)anyWidth, (float)anyHeight);
        f.Execute(anyWidth, anyHeight);

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, zeroHeight);
    }

    TEST_METHOD_EX(CanvasControl_ZeroWidth_DoesNotCreateImageSource)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, anyHeight);
    }

    TEST_METHOD_EX(CanvasControl_ZeroHeight_DoesNotCreateImageSource)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(anyWidth, zeroHeight);
    }


    class MockImageControl : public StubImageControl
    {
    public:
        enum ExpectedSource { None, NullSource, NonNullSource };

    private:
        ExpectedSource m_expectedSource;
        bool m_sourceSeen;

    public:
        MockImageControl()
            : m_expectedSource(None)
            , m_sourceSeen(false)
        {
        }

        void ExpectOnePutSource(ExpectedSource s)
        {
            m_expectedSource = s;
        }

        void Validate()
        {
            switch (m_expectedSource)
            {
            case None:
                Assert::IsFalse(m_sourceSeen, L"put_Source wasn't called");
                break;

            default:
                Assert::IsTrue(m_sourceSeen, L"put_Source was called");
                break;
            }

            m_expectedSource = None;
            m_sourceSeen = false;
        }
        
    private:
        IFACEMETHODIMP put_Source(ABI::Windows::UI::Xaml::Media::IImageSource* value) override 
        {
            Assert::IsFalse(m_sourceSeen);

            if (value)
                Assert::IsTrue(m_expectedSource == NonNullSource, L"non-NULL source expected");
            else
                Assert::IsTrue(m_expectedSource == NullSource, L"NULL source expected");

            m_sourceSeen = true;

            return S_OK;
        }        
    };

    class CanvasControlTestAdapter_VerifyCreateImageSource : public CanvasControlTestAdapter
    {
        bool m_createCanvasImageSourceExpected;
        bool m_createCanvasImageSourceSeen;
        float m_expectedImageSourceWidth;
        float m_expectedImageSourceHeight;
        float m_expectedImageSourceDpi;
        ComPtr<MockImageControl> m_imageControl;

    public:
        CanvasControlTestAdapter_VerifyCreateImageSource()
            : m_createCanvasImageSourceExpected(false)
            , m_createCanvasImageSourceSeen(false)
            , m_expectedImageSourceWidth(-1)
            , m_expectedImageSourceHeight(-1)
            , m_expectedImageSourceDpi(-1)
            , m_imageControl(Make<MockImageControl>())
        {}

        void ExpectOneCreateCanvasImageSource(float width, float height, float dpi = DEFAULT_DPI)
        {
            CreateCanvasImageSourceMethod.SetExpectedCalls(1);
            m_expectedImageSourceWidth = width;
            m_expectedImageSourceHeight = height;
            m_expectedImageSourceDpi = dpi;
            m_imageControl->ExpectOnePutSource(MockImageControl::NonNullSource);
        }

        void ExpectImageSourceSetToNull()
        {
            m_imageControl->ExpectOnePutSource(MockImageControl::NullSource);
        }

        void Validate()
        {
            Assert::AreEqual(m_createCanvasImageSourceExpected, m_createCanvasImageSourceSeen, L"CreateCanvasImageSource");

            m_imageControl->Validate();

            m_createCanvasImageSourceExpected = false;
            m_createCanvasImageSourceSeen = false;
        }

    private:
        virtual ComPtr<IImage> CreateImageControl() override
        {
            return m_imageControl;
        }

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasBackground backgroundMode) override
        {
            Assert::AreEqual(m_expectedImageSourceWidth, width, L"ExpectedImageSourceWidth");
            Assert::AreEqual(m_expectedImageSourceHeight, height, L"ExpectedImageSourceHeight");
            Assert::AreEqual(m_expectedImageSourceDpi, dpi, L"ExpectedImageSourceDpi");

            return __super::CreateCanvasImageSource(device, width, height, dpi, backgroundMode);
        }
    };

    class ResizeFixture
    {
        std::shared_ptr<CanvasControlTestAdapter_VerifyCreateImageSource> m_adapter;
        ComPtr<CanvasControl> m_control;
        ComPtr<StubUserControl> m_userControl;
        MockEventHandler<DrawEventHandler> m_onDraw;

    public:
        ResizeFixture()
            : m_adapter(std::make_shared<CanvasControlTestAdapter_VerifyCreateImageSource>())
            , m_control(Make<CanvasControl>(m_adapter))
            , m_userControl(dynamic_cast<StubUserControl*>(As<IUserControl>(m_control).Get()))
            , m_onDraw(L"Draw")
        {
            EventRegistrationToken tok;
            ThrowIfFailed(m_control->add_Draw(m_onDraw.Get(), &tok));

            ThrowIfFailed(m_userControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
        }

        void ExpectOneDrawEvent()
        {
            m_onDraw.SetExpectedCalls(1);
        }
        
        void ExpectNoDrawEvent()
        {
            m_onDraw.SetExpectedCalls(0);
        }

        void ExpectOneCreateCanvasImageSource(float width, float height)
        {
            m_adapter->ExpectOneCreateCanvasImageSource(width, height);
        }

        void ExpectImageSourceSetToNull()
        {
            m_adapter->ExpectImageSourceSetToNull();
        }

        void Execute(int width, int height)
        {
            Resize(width, height);
            Validate();
        }

    private:
        void Resize(int width, int height)
        {
            m_userControl->Resize(Size{static_cast<float>(width), static_cast<float>(height)});
            m_adapter->RaiseCompositionRenderingEvent();
        }

        void Validate()
        {
            m_adapter->Validate();
            m_onDraw.Validate();
        }
    };
};

class CanvasControlTestAdapter_InjectDeviceContext : public CanvasControlTestAdapter
{
    ComPtr<ID2D1DeviceContext> m_deviceContext;

public:
    CanvasControlTestAdapter_InjectDeviceContext(ID2D1DeviceContext* deviceContext)
        : m_deviceContext(deviceContext)
    {
    }

    virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(
        ICanvasDevice* device, 
        float width, 
        float height, 
        float dpi,
        CanvasBackground backgroundMode) override
    {
        auto result = CreateCanvasImageSourceMethod.WasCalled(device, width, height, dpi, backgroundMode);
        if (result)
            return result;

        auto sisFactory = Make<MockSurfaceImageSourceFactory>();
        sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
            [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
            {
                auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
                                        
                mockSurfaceImageSource->BeginDrawMethod.AllowAnyCall(
                    [&](RECT const&, IID const& iid, void** updateObject, POINT*)
                    {
                        return m_deviceContext.CopyTo(iid, updateObject);
                    });

                mockSurfaceImageSource->SetDeviceMethod.AllowAnyCall();
                mockSurfaceImageSource->EndDrawMethod.AllowAnyCall();

                return mockSurfaceImageSource;
            };

        auto dsFactory = std::make_shared<CanvasImageSourceDrawingSessionFactory>();

        ComPtr<ICanvasResourceCreator> resourceCreator;
        ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

        return Make<CanvasImageSource>(
            resourceCreator.Get(),
            width,
            height,
            dpi,
            backgroundMode,
            sisFactory.Get(),
            dsFactory);
    }
};

TEST_CLASS(CanvasControlTests_Dpi)
{
    class CanvasControlTestAdapter_VerifyDpi : public CanvasControlTestAdapter_InjectDeviceContext
    {
    public:
        float m_dpi;
        float m_lastImageSourceWidth;
        float m_lastImageSourceHeight;
        int m_imageSourceCount;

        CanvasControlTestAdapter_VerifyDpi(ID2D1DeviceContext* deviceContext)
            : CanvasControlTestAdapter_InjectDeviceContext(deviceContext)
            , m_dpi(DEFAULT_DPI)
            , m_lastImageSourceWidth(0)
            , m_lastImageSourceHeight(0)
            , m_imageSourceCount(0) 
        {}

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasBackground backgroundMode) override
        {
            m_lastImageSourceWidth = width;
            m_lastImageSourceHeight = height;
            m_imageSourceCount++;
            
            return __super::CreateCanvasImageSource(device, width, height, dpi, backgroundMode);
        }

        virtual float GetLogicalDpi() override
        {
            return m_dpi;
        }
    };

    TEST_METHOD_EX(CanvasControl_Dpi)
    {
        auto deviceContext = Make<MockD2DDeviceContext>();
        deviceContext->ClearMethod.AllowAnyCall();
        deviceContext->SetTransformMethod.AllowAnyCall();

        float dpiCases[] = {
            50, 
            DEFAULT_DPI,
            200};

        for (auto dpiCase : dpiCases)
        {
            auto adapter = std::make_shared<CanvasControlTestAdapter_VerifyDpi>(deviceContext.Get());
            adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

            int expectedImageSourceCount = 0;

            adapter->m_dpi = dpiCase;

            ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(adapter);
            ComPtr<StubUserControl> userControl = dynamic_cast<StubUserControl*>(As<IUserControl>(canvasControl).Get());

            float const controlSize = 1000;
            userControl->Resize(Size{controlSize, controlSize});

            ThrowIfFailed(userControl->LoadedEventSource->InvokeAll(nullptr, nullptr));


            // An event handler needs to be registered for a drawing session to be constructed.
            auto onDrawFn = 
                Callback<DrawEventHandler>([](ICanvasControl*, ICanvasDrawEventArgs*) { return S_OK; });
            EventRegistrationToken drawEventToken;
            ThrowIfFailed(canvasControl->add_Draw(onDrawFn.Get(), &drawEventToken));

            deviceContext->SetDpiMethod.SetExpectedCalls(1,
                [&](float dpiX, float dpiY)
                {
                    Assert::AreEqual(dpiCase, dpiX);
                    Assert::AreEqual(dpiCase, dpiY);
                });

            canvasControl->Invalidate();
            adapter->RaiseCompositionRenderingEvent();

            expectedImageSourceCount++;
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);

            // Verify the backing store size is correct
            Assert::AreEqual(controlSize, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(controlSize, adapter->m_lastImageSourceHeight);

            // Verify the public, device-independent size of the control.
            ComPtr<IFrameworkElement> controlAsFrameworkElement;
            ThrowIfFailed(canvasControl.As(&controlAsFrameworkElement));
            double verifyWidth, verifyHeight;
            ThrowIfFailed(controlAsFrameworkElement->get_ActualWidth(&verifyWidth));
            ThrowIfFailed(controlAsFrameworkElement->get_ActualHeight(&verifyHeight));
            Assert::AreEqual<double>(verifyWidth, controlSize);
            Assert::AreEqual<double>(verifyHeight, controlSize);

            // Raise a DpiChanged.
            deviceContext->SetDpiMethod.SetExpectedCalls(1,
                [](float dpiX, float dpiY)
                {
                    Assert::AreEqual(DEFAULT_DPI, dpiX);
                    Assert::AreEqual(DEFAULT_DPI, dpiY);
                });            

            adapter->m_dpi = DEFAULT_DPI;
            bool expectResize = controlSize != ceil(controlSize * dpiCase / DEFAULT_DPI);
            adapter->RaiseDpiChangedEvent();

            // Here, verify that no recreation occurred yet. The backing store recreation
            // should occur at the next rendering event.
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);

            adapter->RaiseCompositionRenderingEvent();

            // Verify the backing store got resized as appropriate.
            if (expectResize) expectedImageSourceCount++;
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);
            Assert::AreEqual(controlSize, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(controlSize, adapter->m_lastImageSourceHeight);

            // Verify the size of the control again.
            ThrowIfFailed(controlAsFrameworkElement->get_ActualWidth(&verifyWidth));
            ThrowIfFailed(controlAsFrameworkElement->get_ActualHeight(&verifyHeight));
            Assert::AreEqual<double>(verifyWidth, controlSize);
            Assert::AreEqual<double>(verifyHeight, controlSize);
        }
    }

    TEST_METHOD_EX(CanvasControl_DpiProperties)
    {
        const float dpi = 144;

        auto deviceContext = Make<MockD2DDeviceContext>();
        auto adapter = std::make_shared<CanvasControlTestAdapter_VerifyDpi>(deviceContext.Get());
        adapter->m_dpi = dpi;
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(adapter);

        float actualDpi = 0;
        ThrowIfFailed(canvasControl->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(canvasControl->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(canvasControl->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }
};


TEST_CLASS(CanvasControl_ExternalEvents)
{
    struct Fixture : public BasicControlFixture
    {
        ComPtr<MockWindow> Window;
        MockEventHandler<DrawEventHandler> OnDraw;

        Fixture()
            : Window(Adapter->GetCurrentMockWindow())
            , OnDraw(MockEventHandler<DrawEventHandler>(L"Draw"))
        {
        }

        void CreateControl()
        {
            BasicControlFixture::CreateControl();
            AddDrawHandler(OnDraw.Get());
            RaiseLoadedEvent();
        }
    };
    
    TEST_METHOD_EX(CanvasControl_AfterSurfaceContentsLostEvent_RecreatesSurfaceImageSource)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        f.CreateControl();

        f.RaiseAnyNumberOfSurfaceContentsLostEvents();
        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDestroyed_UnregistersSurfaceContentsLostEvent)
    {
        Fixture f;

        f.Adapter->SurfaceContentsLostEventSource->AddMethod.SetExpectedCalls(1);
        f.CreateControl();

        f.Adapter->SurfaceContentsLostEventSource->RemoveMethod.SetExpectedCalls(1);

        f.Control.Reset();
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowIsNotVisible_DrawEventIsNotInvoked)
    {
        Fixture f;

        f.Window->SetVisible(false);
        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(0);
        f.OnDraw.SetExpectedCalls(0);

        f.CreateControl();
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowBecomesVisible_DrawEventIsInvoked)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        f.Window->SetVisible(false);

        f.CreateControl();
        f.RaiseAnyNumberOfCompositionRenderingEvents();

        f.Window->SetVisible(true);
        f.OnDraw.SetExpectedCalls(1);

        f.RaiseAnyNumberOfCompositionRenderingEvents();        
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowBecomesVisibleWithValidContents_DrawEventIsNotInvoked)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        f.OnDraw.SetExpectedCalls(1);

        f.CreateControl();
        f.RaiseAnyNumberOfCompositionRenderingEvents();

        f.OnDraw.SetExpectedCalls(0);

        f.RaiseAnyNumberOfCompositionRenderingEvents();
        f.Window->SetVisible(false);
        f.RaiseAnyNumberOfCompositionRenderingEvents();
        f.Window->SetVisible(true);
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowIsNotVisible_NoRenderingEventHandlerIsAdded)
    {
        Fixture f;

        f.Window->SetVisible(false);
        f.Adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(0);

        f.CreateControl();
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_WindowWindowBecomesNotVisible_ExistingRenderingEventHandlerIsRemoved)
    {
        Fixture f;

        f.Adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(1);
        f.CreateControl();

        f.Adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(0);
        f.Adapter->CompositionRenderingEventSource->RemoveMethod.SetExpectedCalls(1);

        f.Window->SetVisible(false);
    }

    TEST_METHOD_EX(CanvasControl_WhenDestroyed_UnregistersRenderingEvent)
    {
        Fixture f;

        f.Adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(1);
        f.Adapter->CompositionRenderingEventSource->RemoveMethod.SetExpectedCalls(1);

        f.CreateControl();
        f.Control.Reset();
    }

    TEST_METHOD_EX(CanvasControl_WhenDestroyed_UnregistersVisibilityChangedEvent)
    {
        Fixture f;

        f.Window->VisibilityChangedEventSource->AddMethod.SetExpectedCalls(1);
        f.Window->VisibilityChangedEventSource->RemoveMethod.SetExpectedCalls(1);

        f.CreateControl();
        f.Control.Reset();
    }

    TEST_METHOD_EX(CanvasControl_WhenDestroyed_UnregisterDpiChangedEvent)
    {
        Fixture f;

        f.Adapter->DpiChangedEventSource->AddMethod.SetExpectedCalls(1);
        f.Adapter->DpiChangedEventSource->RemoveMethod.SetExpectedCalls(1);

        f.CreateControl();
        f.Control.Reset();
    }
};


TEST_CLASS(CanvasControl_ClearColor)
{
    struct Fixture
    {
        enum Options
        {
            Default            = 0x0,
            DontRegisterOnDraw = 0x1
        };

        ComPtr<MockD2DDeviceContext> DeviceContext;
        std::shared_ptr<CanvasControlTestAdapter> Adapter;
        ComPtr<CanvasControl> Control;
        ComPtr<StubUserControl> UserControl;
        MockEventHandler<DrawEventHandler> OnDraw;

        Fixture(Options options = Default)
        {
            DeviceContext = Make<MockD2DDeviceContext>();
            DeviceContext->ClearMethod.AllowAnyCall();
            DeviceContext->SetTransformMethod.AllowAnyCall();
            DeviceContext->SetDpiMethod.AllowAnyCall();

            Adapter = std::make_shared<CanvasControlTestAdapter_InjectDeviceContext>(DeviceContext.Get());
            Control = Make<CanvasControl>(Adapter);
            UserControl = dynamic_cast<StubUserControl*>(As<IUserControl>(Control).Get());

            OnDraw = MockEventHandler<DrawEventHandler>(L"Draw");

            bool shouldRegisterOnDraw = !(options & DontRegisterOnDraw);

            if (shouldRegisterOnDraw)
            {
                EventRegistrationToken ignoredToken;
                ThrowIfFailed(Control->add_Draw(OnDraw.Get(), &ignoredToken));
            }

            Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
            ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));

            if (shouldRegisterOnDraw)
                OnDraw.SetExpectedCalls(1);

            RaiseAnyNumberOfCompositionRenderingEvents();
        }

        void RaiseAnyNumberOfCompositionRenderingEvents()
        {
            int anyNumberOfTimes = 5;
            for (auto i = 0; i < anyNumberOfTimes; ++i)
                Adapter->RaiseCompositionRenderingEvent();
        }
    };

    TEST_METHOD_EX(CanvasControl_DefaultClearColorIsTransparentBlack)
    {
        Fixture f;

        Color expectedColor{ 0, 0, 0, 0 };

        Color actualColor{ 0xFF, 0xFF, 0xFF, 0xFF };
        ThrowIfFailed(f.Control->get_ClearColor(&actualColor));

        Assert::AreEqual(expectedColor, actualColor);
    }

    TEST_METHOD_EX(CanvasControl_ClearColorValueIsPersisted)
    {
        Fixture f;

        Color anyColor{ 1, 2, 3, 4 };

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        Color actualColor{ 0xFF, 0xFF, 0xFF, 0xFF };
        ThrowIfFailed(f.Control->get_ClearColor(&actualColor));

        Assert::AreEqual(anyColor, actualColor);
    }

    TEST_METHOD_EX(CanvasControl_SettingDifferentClearColorTriggersRedraw)
    {
        Fixture f;

        Color currentColor;
        ThrowIfFailed(f.Control->get_ClearColor(&currentColor));

        Color differentColor = currentColor;
        differentColor.R = 255 - differentColor.R;

        ThrowIfFailed(f.Control->put_ClearColor(differentColor));

        f.OnDraw.SetExpectedCalls(1);
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_SettingClearColorToCurrentValueDoesNotTriggerRedraw)
    {
        Fixture f;

        Color currentColor;
        ThrowIfFailed(f.Control->get_ClearColor(&currentColor));

        ThrowIfFailed(f.Control->put_ClearColor(currentColor));

        f.OnDraw.SetExpectedCalls(0);
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_DeviceContextIsClearedToCorrectColorBeforeDrawHandlerIsCalled)
    {
        Fixture f;

        Color anyColor{ 1, 2, 3, 4 };

        f.OnDraw.SetExpectedCalls(0);

        f.DeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
                f.OnDraw.SetExpectedCalls(1);
            });

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_DeviceContextIsClearedToCorrectColorEvenIfNoDrawHandlersRegistered)
    {
        Fixture f(Fixture::DontRegisterOnDraw);

        Color anyColor{ 1, 2, 3, 4 };

        f.DeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(ToD2DColor(anyColor), *color);
            });

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_WhenClearColorBecomesOpaque_SurfaceImageSourceIsCreatedWithOpaqueBackgroundMode)
    {
        Fixture f;

        f.OnDraw.AllowAnyCall();

        // The default clear color is transparent
        Color defaultClearColor;
        ThrowIfFailed(f.Control->get_ClearColor(&defaultClearColor));
        Assert::AreNotEqual<uint8_t>(255, defaultClearColor.A);

        Color anyOpaqueColor{ 255, 1, 2, 3 };
        ThrowIfFailed(f.Control->put_ClearColor(anyOpaqueColor));

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1,
            [&](ICanvasDevice*, float, float, float, CanvasBackground backgroundMode)
            {
                Assert::AreEqual(CanvasBackground::Opaque, backgroundMode);
                return nullptr;
            });

        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD_EX(CanvasControl_WhenClearColorBecomesTransparent_SurfaceImageSourceIsCreatedWithTransparentBackgroundMode)
    {
        Fixture f;

        f.OnDraw.AllowAnyCall();

        // First ensure we have an opaque color set
        Color anyOpaqueColor{ 255, 1, 2, 3};
        ThrowIfFailed(f.Control->put_ClearColor(anyOpaqueColor));
        f.RaiseAnyNumberOfCompositionRenderingEvents();

        // Now set it to transparent
        Color anyTransparentColor{ 254, 1, 2, 3 };
        ThrowIfFailed(f.Control->put_ClearColor(anyTransparentColor));

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1,
            [&](ICanvasDevice*, float, float, float, CanvasBackground backgroundMode)
            {
                Assert::AreEqual(CanvasBackground::Transparent, backgroundMode);
                return nullptr;
            });

        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }
};
