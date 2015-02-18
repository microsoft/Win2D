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

#include "MockAsyncAction.h"
#include "BasicControlFixture.h"
#include "CanvasControlTestAdapter.h"
#include "ControlFixtures.h"

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

    TEST_METHOD_EX(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;

        CanvasControlFixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreateResources = MockEventHandler<Static_CreateResourcesEventHandler>(L"CreateResources", ExpectedEventParams::Both);
        auto createResourcesEventToken0 = f.AddCreateResourcesHandler(onCreateResources.Get());

        // Before the control received the Loaded event,
        // CompositionRenderingEvents should essentially be ignored.
        f.RenderSingleFrame();

        // Issue a Loaded.  Now, the next CompositionRenderingEvent will cause
        // CreateResources to be raised.
        onCreateResources.SetExpectedCalls(1);
        f.Load();
        f.RenderSingleFrame();

        onCreateResources.Validate();
 
        // Register the CreateResources handler again.  Because the device has
        // already been created, add_CreateResources should immediately fire the
        // event too.
        onCreateResources.SetExpectedCalls(1);

        auto createResourcesEventToken1 = f.AddCreateResourcesHandler(onCreateResources.Get());

        onCreateResources.Validate();

        // Register the Draw handler.
        auto onDraw = MockEventHandler<Static_DrawEventHandler>(L"Draw", ExpectedEventParams::Both);

        auto drawEventToken = f.AddDrawHandler(onDraw.Get());

        // Invalidate and ensure the Draw callback is called.
        onDraw.SetExpectedCalls(1);
        f.Control->Invalidate();
        f.RenderSingleFrame();
        onDraw.Validate();

        // Ensure a subsequent invalidation doesn't recreate resources.
        onDraw.SetExpectedCalls(1);
        f.Control->Invalidate();
        f.RenderSingleFrame();

        // Unregister the events. Call invalidate. Ensure the handler doesn't get called again (the event was correctly unregistered).
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(f.Control->remove_Draw(drawEventToken));
        f.Control->Invalidate();
        f.RenderSingleFrame();

        // Unregistering the same event twice should do nothing.
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(f.Control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(f.Control->remove_Draw(drawEventToken));
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAdded_RedrawIsTriggered)
    {
        CanvasControlFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        f.Load();
        f.RenderSingleFrame();

        auto onDraw1 = MockEventHandler<Static_DrawEventHandler>(L"Draw1");
        f.AddDrawHandler(onDraw1.Get());

        onDraw1.SetExpectedCalls(1);

        f.RenderSingleFrame();

        auto onDraw2 = MockEventHandler<Static_DrawEventHandler>(L"Draw2");
        f.AddDrawHandler(onDraw2.Get());

        onDraw1.SetExpectedCalls(1);
        onDraw2.SetExpectedCalls(1);

        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAddedBeforeLoadedEvent_RedrawIsTriggeredAfterLoadedEvent)
    {
        CanvasControlFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        auto onDraw = MockEventHandler<Static_DrawEventHandler>(L"Draw");
        f.AddDrawHandler(onDraw.Get());

        f.RenderSingleFrame();
        f.Load();

        onDraw.SetExpectedCalls(1);
        f.RenderSingleFrame();
    }

    TEST_METHOD_EX(CanvasControl_ThreadingRestrictions)
    {
        CanvasControlFixture f;
        boolean b = false;
        Color color = {};
        MockEventHandler<Static_DrawEventHandler> onDraw;
        MockEventHandler<Static_CreateResourcesEventHandler> onCreateResources;
        EventRegistrationToken token;

        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->add_CreateResources(onCreateResources.Get(), &token));
        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->remove_CreateResources(token));

        VERIFY_THREADING_RESTRICTION(RPC_E_WRONG_THREAD, f.Control->get_ReadyToDraw(&b));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->add_Draw(onDraw.Get(), &token));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->remove_Draw(token));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->put_ClearColor(color));
        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->get_ClearColor(&color));

        VERIFY_THREADING_RESTRICTION(S_OK, f.Control->Invalidate());
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
                Assert::IsFalse(m_sourceSeen, L"put_Source was called, but we didn't expect it to be.");
                break;

            default:
                Assert::IsTrue(m_sourceSeen, L"Expected put_Source to be called, but it wasn't.");
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
        MockEventHandler<Static_DrawEventHandler> m_onDraw;

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
                Callback<Static_DrawEventHandler>([](ICanvasControl*, ICanvasDrawEventArgs*) { return S_OK; });
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
            bool expectResize = controlSize != ceil(controlSize * dpiCase / DEFAULT_DPI);

            deviceContext->SetDpiMethod.SetExpectedCalls(expectResize ? 1 : 0,
                [](float dpiX, float dpiY)
                {
                    Assert::AreEqual(DEFAULT_DPI, dpiX);
                    Assert::AreEqual(DEFAULT_DPI, dpiY);
                });            

            adapter->m_dpi = DEFAULT_DPI;
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

    TEST_METHOD_EX(CanvasControl_WhenInvalidateIsCalledBeforeLoadedEvent_ThenNothingBadHappens)
    {
        CanvasControlFixture f;
        ThrowIfFailed(f.Control->Invalidate());
    }
};


TEST_CLASS(CanvasControl_ExternalEvents)
{
    struct Fixture : public Static_BasicControlFixture
    {
        ComPtr<MockWindow> Window;
        MockEventHandler<Static_DrawEventHandler> OnDraw;

        Fixture()
            : OnDraw(MockEventHandler<Static_DrawEventHandler>(L"Draw"))
        {
            CreateAdapter(); 
            Window = Adapter->GetCurrentMockWindow();
        }

        void CreateControl()
        {
            BasicControlFixture::CreateControl();
            AddDrawHandler(OnDraw.Get());
            Load();
        }
    };
    
    TEST_METHOD_EX(CanvasControl_AfterSurfaceContentsLostEvent_RecreatesSurfaceImageSource)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        f.CreateControl();

        f.RenderSingleFrame();

        // Now, after a surface contents lost event we should recreate the image
        // source on the next redraw.
        f.RaiseAnyNumberOfSurfaceContentsLostEvents();

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1);
        f.OnDraw.SetExpectedCalls(1);

        f.RenderSingleFrame();
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
        f.RenderAnyNumberOfFrames();
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowBecomesVisible_DrawEventIsInvoked)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        f.Window->SetVisible(false);

        f.CreateControl();
        f.RenderAnyNumberOfFrames();

        f.Window->SetVisible(true);
        f.OnDraw.SetExpectedCalls(1);

        f.RenderAnyNumberOfFrames();        
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowBecomesVisibleWithValidContents_DrawEventIsNotInvoked)
    {
        Fixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        f.OnDraw.SetExpectedCalls(1);

        f.CreateControl();
        f.RenderAnyNumberOfFrames();

        f.OnDraw.SetExpectedCalls(0);

        f.RenderAnyNumberOfFrames();
        f.Window->SetVisible(false);
        f.RenderAnyNumberOfFrames();
        f.Window->SetVisible(true);
        f.RenderAnyNumberOfFrames();
    }

    TEST_METHOD_EX(CanvasControl_WhenWindowIsNotVisible_NoRenderingEventHandlerIsAdded)
    {
        Fixture f;

        f.Window->SetVisible(false);
        f.Adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(0);

        f.CreateControl();
        f.RenderAnyNumberOfFrames();
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

    TEST_METHOD_EX(CanvasControl_WhenSuspendingEventRaised_TrimCalledOnDevice)
    {
        ControlFixtureWithRecreatableDeviceManager<CanvasControlTraits> f;

        auto anyDevice = Make<MockCanvasDevice>();
        f.DeviceManager->SetDevice(anyDevice);

        // When a new control has not yet been loaded, app suspend should be ignored.
        anyDevice->TrimMethod.SetExpectedCalls(0);
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
        ThrowIfFailed(f.Adapter->ResumingEventSource->InvokeAll(nullptr, nullptr));

        // After the control is loaded, app suspend should call Trim.
        f.Load();

        anyDevice->TrimMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));

        anyDevice->TrimMethod.SetExpectedCalls(0);
        ThrowIfFailed(f.Adapter->ResumingEventSource->InvokeAll(nullptr, nullptr));

        // If the control is unloaded, Trim should be ignored once more.
        f.RaiseUnloadedEvent();

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
        ThrowIfFailed(f.Adapter->ResumingEventSource->InvokeAll(nullptr, nullptr));
    }

    TEST_METHOD_EX(CanvasControl_WhenSuspendingEventRaisedAndThereIsNoDevice_NothingBadHappens)
    {
        ControlFixtureWithRecreatableDeviceManager<CanvasControlTraits> f;

        ComPtr<ICanvasDevice> nullDevice;
        f.DeviceManager->SetDevice(nullDevice);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }
};


TEST_CLASS(CanvasControl_ClearColor)
{
    TEST_METHOD_EX(CanvasControl_SettingDifferentClearColorTriggersRedraw)
    {
        ClearColorFixture<CanvasControlTraits> f;
        f.RegisterOnDraw();

        Color currentColor;
        ThrowIfFailed(f.Control->get_ClearColor(&currentColor));

        Color differentColor = currentColor;
        differentColor.R = 255 - differentColor.R;

        ThrowIfFailed(f.Control->put_ClearColor(differentColor));

        f.OnDraw.SetExpectedCalls(1);

        f.Load();
        f.RenderAnyNumberOfFrames();
    }

    TEST_METHOD_EX(CanvasControl_SettingClearColorToCurrentValueDoesNotTriggerRedraw)
    {
        ClearColorFixture<CanvasControlTraits> f;

        f.RegisterOnDraw();
        f.OnDraw.SetExpectedCalls(1);

        f.Load();
        f.RenderAnyNumberOfFrames();

        Color currentColor;
        ThrowIfFailed(f.Control->get_ClearColor(&currentColor));

        ThrowIfFailed(f.Control->put_ClearColor(currentColor));

        f.OnDraw.SetExpectedCalls(0);
        f.RenderAnyNumberOfFrames();
    }

    TEST_METHOD_EX(CanvasControl_WhenClearColorBecomesOpaque_SurfaceImageSourceIsCreatedWithOpaqueBackgroundMode)
    {
        ClearColorFixture<CanvasControlTraits> f;

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

        f.Load();
        f.RenderAnyNumberOfFrames();
    }

    TEST_METHOD_EX(CanvasControl_WhenClearColorBecomesTransparent_SurfaceImageSourceIsCreatedWithTransparentBackgroundMode)
    {
        ClearColorFixture<CanvasControlTraits> f;

        f.OnDraw.AllowAnyCall();

        // First ensure we have an opaque color set
        Color anyOpaqueColor{ 255, 1, 2, 3};
        ThrowIfFailed(f.Control->put_ClearColor(anyOpaqueColor));

        f.Load();
        f.RenderAnyNumberOfFrames();

        // Now set it to transparent
        Color anyTransparentColor{ 254, 1, 2, 3 };
        ThrowIfFailed(f.Control->put_ClearColor(anyTransparentColor));

        f.Adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1,
            [&](ICanvasDevice*, float, float, float, CanvasBackground backgroundMode)
            {
                Assert::AreEqual(CanvasBackground::Transparent, backgroundMode);
                return nullptr;
            });

        f.RenderAnyNumberOfFrames();
    }
};
