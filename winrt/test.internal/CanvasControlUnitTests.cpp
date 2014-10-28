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

TEST_CLASS(CanvasControlTests_CommonAdapter)
{
    std::shared_ptr<CanvasControlTestAdapter> m_adapter;
    ComPtr<CanvasControl> m_control;
    ComPtr<StubUserControl> m_userControl;

    TEST_METHOD_INITIALIZE(Init)
    {
        m_adapter = std::make_shared<CanvasControlTestAdapter>();
        m_control = Make<CanvasControl>(m_adapter);
        m_userControl = dynamic_cast<StubUserControl*>(As<IUserControl>(m_control).Get());
    }

    TEST_METHOD(CanvasControl_Implements_Expected_Interfaces)
    {
        ASSERT_IMPLEMENTS_INTERFACE(m_control, ICanvasControl);
        ASSERT_IMPLEMENTS_INTERFACE(m_control, ABI::Windows::UI::Xaml::Controls::IUserControl);
        ASSERT_IMPLEMENTS_INTERFACE(m_control, ICanvasResourceCreator);
    }

    TEST_METHOD(CanvasControl_DeviceProperty_Null)
    {
        Assert::AreEqual(E_INVALIDARG, m_control->get_Device(nullptr));
    }

    TEST_METHOD(CanvasControl_DrawEventArgs_Getter)
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

    TEST_METHOD(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;

        m_adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreateResources = MockEventHandler<CreateResourcesEventHandlerType>(L"CreateResources", ExpectedEventParams::OnlySender);
        EventRegistrationToken createResourcesEventToken0;
        ThrowIfFailed(m_control->add_CreateResources(onCreateResources.Get(), &createResourcesEventToken0));

        // Issue a Loaded.
        // Should fire CreateResources.
        onCreateResources.SetExpectedCalls(1);
        m_userControl->LoadedEventSource->InvokeAll(nullptr, nullptr);

        onCreateResources.Validate();
 
        // Register the CreateResources handler again.
        // Because the Loaded event has already occurred, add_CreateResources should immediately fire the event too.
        onCreateResources.SetExpectedCalls(1);

        EventRegistrationToken createResourcesEventToken1;
        ThrowIfFailed(m_control->add_CreateResources(onCreateResources.Get(), &createResourcesEventToken1));

        onCreateResources.Validate();

        // Register the Draw handler.
        auto onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw", ExpectedEventParams::Both);

        EventRegistrationToken drawEventToken;
        ThrowIfFailed(m_control->add_Draw(onDraw.Get(), &drawEventToken));

        // Invalidate and ensure the Draw callback is called.
        onDraw.SetExpectedCalls(1);
        m_control->Invalidate();
        m_adapter->RaiseCompositionRenderingEvent();
        onDraw.Validate();

        // Ensure a subsequent invalidation doesn't recreate resources.
        onDraw.SetExpectedCalls(1);
        m_control->Invalidate();
        m_adapter->RaiseCompositionRenderingEvent();

        // Unregister the events. Call invalidate. Ensure the handler doesn't get called again (the event was correctly unregistered).
        ThrowIfFailed(m_control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(m_control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(m_control->remove_Draw(drawEventToken));
        m_control->Invalidate();
        m_adapter->RaiseCompositionRenderingEvent();

        // Unregistering the same event twice should do nothing.
        ThrowIfFailed(m_control->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(m_control->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(m_control->remove_Draw(drawEventToken));

        Expectations::Instance()->Validate();
    }
};

TEST_CLASS(CanvasControlTests_SizeTests)
{
    static const int anyWidth = 128;
    static const int anyHeight = 256;
    static const int zeroWidth = 0;
    static const int zeroHeight = 0;

    TEST_METHOD(CanvasControl_Resizing)
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
                f.ExpectOneCreateCanvasImageSource(testStep.ResizeWidth, testStep.ResizeHeight);

            if (testStep.ExpectDraw)
                f.ExpectOneDrawEvent();

            f.Execute(testStep.ResizeWidth, testStep.ResizeHeight);
        }

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_ZeroSizedControl_DoesNotCreateImageSource_DoesNotCallDrawHandler)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, zeroHeight);

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_ResizeToZeroSize_ClearsImageSource_DoesNotCallDrawHandler)
    {
        ResizeFixture f;

        f.ExpectOneDrawEvent();
        f.ExpectOneCreateCanvasImageSource(anyWidth, anyHeight);
        f.Execute(anyWidth, anyHeight);

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, zeroHeight);

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_ZeroWidth_DoesNotCreateImageSource)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(zeroWidth, anyHeight);

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_ZeroHeight_DoesNotCreateImageSource)
    {
        ResizeFixture f;

        f.ExpectImageSourceSetToNull();
        f.ExpectNoDrawEvent();
        f.Execute(anyWidth, zeroHeight);

        Expectations::Instance()->Validate();
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
        int m_expectedImageSourceWidth;
        int m_expectedImageSourceHeight;
        ComPtr<MockImageControl> m_imageControl;

    public:
        CanvasControlTestAdapter_VerifyCreateImageSource()
            : m_createCanvasImageSourceExpected(false)
            , m_createCanvasImageSourceSeen(false)
            , m_expectedImageSourceWidth(-1)
            , m_expectedImageSourceHeight(-1)
            , m_imageControl(Make<MockImageControl>())
        {}

        void ExpectOneCreateCanvasImageSource(int width, int height)
        {
            CreateCanvasImageSourceMethod.SetExpectedCalls(1);
            m_expectedImageSourceWidth = width;
            m_expectedImageSourceHeight = height;
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

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override
        {
            Assert::AreEqual(m_expectedImageSourceWidth, width, L"ExpectedImageSourceWidth");
            Assert::AreEqual(m_expectedImageSourceHeight, height, L"ExpectedImageSourceHeight");

            return __super::CreateCanvasImageSource(device, width, height);
        }
    };

    class ResizeFixture
    {
        std::shared_ptr<CanvasControlTestAdapter_VerifyCreateImageSource> m_adapter;
        ComPtr<CanvasControl> m_control;
        ComPtr<StubUserControl> m_userControl;
        MockEventHandler<DrawEventHandlerType> m_onDraw;

    public:
        ResizeFixture()
            : m_adapter(std::make_shared<CanvasControlTestAdapter_VerifyCreateImageSource>())
            , m_control(Make<CanvasControl>(m_adapter))
            , m_userControl(dynamic_cast<StubUserControl*>(As<IUserControl>(m_control).Get()))
            , m_onDraw(L"Draw")
        {
            EventRegistrationToken tok;
            ThrowIfFailed(m_control->add_Draw(m_onDraw.Get(), &tok));

            m_userControl->LoadedEventSource->InvokeAll(nullptr, nullptr);
        }

        void ExpectOneDrawEvent()
        {
            m_onDraw.SetExpectedCalls(1);
        }
        
        void ExpectNoDrawEvent()
        {
            m_onDraw.SetExpectedCalls(0);
        }

        void ExpectOneCreateCanvasImageSource(int width, int height)
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
    class CanvasControlTestAdapter_VerifyDpi : public CanvasControlTestAdapter
    {
    public:
        float m_dpi;
        int m_lastImageSourceWidth;
        int m_lastImageSourceHeight;
        int m_imageSourceCount;
        ComPtr<ID2D1DeviceContext> m_d2dDeviceContext; 

        CanvasControlTestAdapter_VerifyDpi(ID2D1DeviceContext* d2dDeviceContext)
            : m_dpi(DEFAULT_DPI)
            , m_lastImageSourceWidth(0)
            , m_lastImageSourceHeight(0)
            , m_d2dDeviceContext(d2dDeviceContext)
            , m_imageSourceCount(0) 
        {}

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override
        {
            m_lastImageSourceWidth = width;
            m_lastImageSourceHeight = height;
            m_imageSourceCount++;
            
            auto sisFactory = Make<MockSurfaceImageSourceFactory>();
            sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
                [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
                {
                    auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
                                        
                    mockSurfaceImageSource->BeginDrawMethod.AllowAnyCall(
                        [&](RECT const& updateRect, IID const& iid, void** updateObject, POINT* offset)
                        {
                            return m_d2dDeviceContext.CopyTo(iid, updateObject);
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
                CanvasBackground::Transparent,
                sisFactory.Get(),
                dsFactory);
        }

        virtual float GetLogicalDpi() override
        {
            return m_dpi;
        }
    };

    TEST_METHOD(CanvasControl_Dpi)
    {
        auto deviceContext = Make<MockD2DDeviceContext>();
        deviceContext->SetTransformMethod.AllowAnyCall();

        float dpiCases[] = {
            50, 
            DEFAULT_DPI - (DEFAULT_DPI * FLT_EPSILON),
            DEFAULT_DPI,
            DEFAULT_DPI + (DEFAULT_DPI * FLT_EPSILON),
            200};

        for (auto dpiCase : dpiCases)
        {
            auto adapter = std::make_shared<CanvasControlTestAdapter_VerifyDpi>(deviceContext.Get());

            int expectedImageSourceCount = 0;

            adapter->m_dpi = dpiCase;

            ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(adapter);
            ComPtr<StubUserControl> userControl = dynamic_cast<StubUserControl*>(As<IUserControl>(canvasControl).Get());

            float const controlSize = 1000;
            userControl->Resize(Size{controlSize, controlSize});

            userControl->LoadedEventSource->InvokeAll(nullptr, nullptr);


            // An event handler needs to be registered for a drawing session to be constructed.
            auto onDrawFn = 
                Callback<DrawEventHandlerType>([](ICanvasControl*, ICanvasDrawEventArgs*) { return S_OK; });
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
            const float dpiScale = dpiCase / DEFAULT_DPI;
            float expectedBackingStoreDim = controlSize * dpiScale;
            float truncatedDimF = ceil(expectedBackingStoreDim);
            assert(truncatedDimF <= INT_MAX);
            int truncatedDimI = static_cast<int>(truncatedDimF);

            Assert::AreEqual(truncatedDimI, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(truncatedDimI, adapter->m_lastImageSourceHeight);

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
            Assert::AreEqual(static_cast<int>(controlSize), adapter->m_lastImageSourceWidth);
            Assert::AreEqual(static_cast<int>(controlSize), adapter->m_lastImageSourceHeight);

            // Verify the size of the control again.
            ThrowIfFailed(controlAsFrameworkElement->get_ActualWidth(&verifyWidth));
            ThrowIfFailed(controlAsFrameworkElement->get_ActualHeight(&verifyHeight));
            Assert::AreEqual<double>(verifyWidth, controlSize);
            Assert::AreEqual<double>(verifyHeight, controlSize);

            Expectations::Instance()->Validate();
        }
    }
};


TEST_CLASS(CanvasControl_ExternalEvents)
{
    std::shared_ptr<CanvasControlTestAdapter> m_adapter;
    ComPtr<MockWindow> m_window;
    ComPtr<CanvasControl> m_control;
    ComPtr<StubUserControl> m_userControl;
    EventRegistrationToken m_ignoredToken;
    MockEventHandler<DrawEventHandlerType> m_onDraw;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
        m_adapter = std::make_shared<CanvasControlTestAdapter>();
        m_window = m_adapter->GetCurrentMockWindow();
        m_onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw");
    }

    void CreateControl()
    {
        m_control = Make<CanvasControl>(m_adapter);
        m_userControl = dynamic_cast<StubUserControl*>(As<IUserControl>(m_control).Get());

        ThrowIfFailed(m_control->add_Draw(m_onDraw.Get(), &m_ignoredToken));

        m_userControl->LoadedEventSource->InvokeAll(nullptr, nullptr);
    }

    TEST_METHOD(CanvasControl_AfterSurfaceContentsLostEvent_RecreatesSurfaceImageSource)
    {
        m_adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(1);
        m_onDraw.SetExpectedCalls(1);

        CreateControl();

        RaiseAnyNumberOfSurfaceContentsLostEvents();
        m_adapter->RaiseCompositionRenderingEvent();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenDestroyed_UnregistersSurfaceContentsLostEvent)
    {
        m_adapter->SurfaceContentsLostEventSource->AddMethod.SetExpectedCalls(1);
        CreateControl();

        m_adapter->SurfaceContentsLostEventSource->RemoveMethod.SetExpectedCalls(1);

        m_control.Reset();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenWindowIsNotVisible_DrawEventIsNotInvoked)
    {
        m_window->SetVisible(false);
        m_adapter->CreateCanvasImageSourceMethod.SetExpectedCalls(0);
        m_onDraw.SetExpectedCalls(0);

        CreateControl();
        RaiseAnyNumberOfCompositionRenderingEvents();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenWindowBecomesVisible_DrawEventIsInvoked)
    {
        m_adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        m_window->SetVisible(false);

        CreateControl();
        RaiseAnyNumberOfCompositionRenderingEvents();

        m_window->SetVisible(true);
        m_onDraw.SetExpectedCalls(1);

        RaiseAnyNumberOfCompositionRenderingEvents();        

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenWindowBecomesVisibleWithValidContents_DrawEventIsNotInvoked)
    {
        m_adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        m_onDraw.SetExpectedCalls(1);

        CreateControl();
        RaiseAnyNumberOfCompositionRenderingEvents();

        m_onDraw.SetExpectedCalls(0);

        RaiseAnyNumberOfCompositionRenderingEvents();
        m_window->SetVisible(false);
        RaiseAnyNumberOfCompositionRenderingEvents();
        m_window->SetVisible(true);
        RaiseAnyNumberOfCompositionRenderingEvents();
    }

    TEST_METHOD(CanvasControl_WhenWindowIsNotVisible_NoRenderingEventHandlerIsAdded)
    {
        m_window->SetVisible(false);
        m_adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(0);

        CreateControl();
        RaiseAnyNumberOfCompositionRenderingEvents();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WindowWindowBecomesNotVisible_ExistingRenderingEventHandlerIsRemoved)
    {
        m_adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(1);
        CreateControl();

        m_adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(0);
        m_adapter->CompositionRenderingEventSource->RemoveMethod.SetExpectedCalls(1);

        m_window->SetVisible(false);

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenDestroyed_UnregistersRenderingEvent)
    {
        m_adapter->CompositionRenderingEventSource->AddMethod.SetExpectedCalls(1);
        m_adapter->CompositionRenderingEventSource->RemoveMethod.SetExpectedCalls(1);

        CreateControl();
        m_control.Reset();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenDestroyed_UnregistersVisibilityChangedEvent)
    {
        m_window->VisibilityChangedEventSource->AddMethod.SetExpectedCalls(1);
        m_window->VisibilityChangedEventSource->RemoveMethod.SetExpectedCalls(1);

        CreateControl();
        m_control.Reset();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenDestroyed_UnregisterDpiChangedEvent)
    {
        m_adapter->DpiChangedEventSource->AddMethod.SetExpectedCalls(1);
        m_adapter->DpiChangedEventSource->RemoveMethod.SetExpectedCalls(1);

        CreateControl();
        m_control.Reset();

        Expectations::Instance()->Validate();
    }

    TEST_METHOD(CanvasControl_WhenAppSuspended_TrimCalledOnDevice)
    {
        CreateControl();
        
        ComPtr<ICanvasDevice> icanvasDevice;
        ThrowIfFailed(m_control->get_Device(&icanvasDevice));
        StubCanvasDevice* canvasDevice = dynamic_cast<StubCanvasDevice*>(icanvasDevice.Get());

        canvasDevice->TrimMethod.SetExpectedCalls(1);

        m_adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr);

        Expectations::Instance()->Validate();
    }

private:
    void RaiseAnyNumberOfSurfaceContentsLostEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            m_adapter->RaiseSurfaceContentsLostEvent();
    }

    void RaiseAnyNumberOfCompositionRenderingEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            m_adapter->RaiseCompositionRenderingEvent();
    }
};

