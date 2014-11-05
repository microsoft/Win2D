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

    EventRegistrationToken AddCreateResourcesHandler(CreateResourcesEventHandlerType* handler)
    {
        return AddEventHandler(&CanvasControl::add_CreateResources, handler);
    }

    EventRegistrationToken AddDrawHandler(DrawEventHandlerType* handler)
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

    TEST_METHOD_EX(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;

        CanvasControlFixture f;

        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreateResources = MockEventHandler<CreateResourcesEventHandlerType>(L"CreateResources", ExpectedEventParams::OnlySender);
        auto createResourcesEventToken0 = f.AddCreateResourcesHandler(onCreateResources.Get());

        // Issue a Loaded.
        // Should fire CreateResources.
        onCreateResources.SetExpectedCalls(1);
        f.RaiseLoadedEvent();

        onCreateResources.Validate();
 
        // Register the CreateResources handler again.
        // Because the Loaded event has already occurred, add_CreateResources should immediately fire the event too.
        onCreateResources.SetExpectedCalls(1);

        auto createResourcesEventToken1 = f.AddCreateResourcesHandler(onCreateResources.Get());

        onCreateResources.Validate();

        // Register the Draw handler.
        auto onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw", ExpectedEventParams::Both);

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

        auto onDraw1 = MockEventHandler<DrawEventHandlerType>(L"Draw1");
        f.AddDrawHandler(onDraw1.Get());

        onDraw1.SetExpectedCalls(1);

        f.RaiseCompositionRenderingEvent();

        auto onDraw2 = MockEventHandler<DrawEventHandlerType>(L"Draw2");
        f.AddDrawHandler(onDraw2.Get());

        onDraw1.SetExpectedCalls(1);
        onDraw2.SetExpectedCalls(1);

        f.RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAddedBeforeLoadedEvent_RedrawIsTriggeredAfterLoadedEvent)
    {
        CanvasControlFixture f;
        f.Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();

        auto onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw");
        f.AddDrawHandler(onDraw.Get());

        f.RaiseCompositionRenderingEvent();
        f.RaiseLoadedEvent();

        onDraw.SetExpectedCalls(1);
        f.RaiseCompositionRenderingEvent();
    }

    //
    // Fixture for tests that have a canvas control with a CreateResources event
    // handler registered.
    //
    struct CreateResourcesFixture : public CanvasControlFixture
    {
        MockEventHandler<CreateResourcesEventHandlerType> OnCreateResources;

        CreateResourcesFixture()
            : OnCreateResources(MockEventHandler<CreateResourcesEventHandlerType>(L"onCreateResources"))
        {
            AddCreateResourcesHandler(OnCreateResources.Get());
        }

        HRESULT GetDeviceRemovedReason()
        {
            auto d3dDevice = GetDXGIInterfaceFromResourceCreator<ID3D11Device>(Control.Get());
            return d3dDevice->GetDeviceRemovedReason();
        }

        void MarkControlDeviceAsLost()
        {
            auto d3dDevice = GetDXGIInterfaceFromResourceCreator<ID3D11Device>(Control.Get());
            auto mockD3DDevice = dynamic_cast<MockD3D11Device*>(d3dDevice.Get());
            mockD3DDevice->GetDeviceRemovedReasonMethod.AllowAnyCall(
                []
                {
                    return DXGI_ERROR_DEVICE_REMOVED;
                });
        }
    };

    TEST_METHOD_EX(CanvasControl_WhenCreateResourcesHandlerCalledOnLoadAndItThrowsDeviceLost_ThenDeviceIsRecreatedAndAllHandlersCalledAgain)
    {
        auto f = std::make_shared<CreateResourcesFixture>();

        f->OnCreateResources.SetExpectedCalls(1,
            [=]
            {
                f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
                    [=]
                    {
                        f->OnCreateResources.SetExpectedCalls(1,
                            [=]
                            {
                                Assert::AreEqual(S_OK, f->GetDeviceRemovedReason());
                                return S_OK;
                            });

                        return nullptr;
                    });

                f->MarkControlDeviceAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });
        

        f->RaiseLoadedEvent();

        Expectations::Instance()->ValidateNotSatisfied();
        
        f->RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenCreateResourcesHandlerCalledOnLoadAndItThrowsDeviceLost_ThenDeviceIsRecreatedAndAllHandlersCalledAgain_EvenIfDeviceIsLostImmediatelyAfterRecreation)
    {
        auto f = std::make_shared<CreateResourcesFixture>();

        int devicesLostInARow = 10;

        f->OnCreateResources.SetExpectedCalls(devicesLostInARow,
            [=]
            {
                Assert::AreEqual(S_OK, f->GetDeviceRemovedReason());

                if (f->OnCreateResources.GetCurrentCallCount() != devicesLostInARow)
                {
                    f->MarkControlDeviceAsLost();
                    f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1);
                    return DXGI_ERROR_DEVICE_REMOVED;
                }
                else
                {
                    return S_OK;
                }
            });

        f->RaiseLoadedEvent();
        for (int i = 0; i < devicesLostInARow - 1; ++i)
        {
            Expectations::Instance()->ValidateNotSatisfied();
            f->Adapter->RaiseCompositionRenderingEvent();
        }
    }

    TEST_METHOD_EX(CanvasControl_WhenCreateResourceHandlerCalledOnLoadAndItAlwaysThrowsDeviceLost_ThenExceptionIsPropagated)
    {
        auto f = std::make_shared<CreateResourcesFixture>();

        // The CanvasControl's device hasn't actually been lost, so we don't
        // expect it to be recreated.
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(0);

        f->OnCreateResources.SetExpectedCalls(1, DXGI_ERROR_DEVICE_REMOVED);

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [=]
            { 
                f->RaiseLoadedEvent(); 
            });
    }


    TEST_METHOD_EX(CanvasControl_WhenCreateResourceHandlerCalledAsItIsAddedAndTheDeviceIsLost_ThenDeviceIsRecreatedAndAllHandlersCalledAgain)
    {
        auto f = std::make_shared<CreateResourcesFixture>();
        f->OnCreateResources.SetExpectedCalls(1);
        f->RaiseLoadedEvent();

        auto newlyAddedCreateResources = MockEventHandler<CreateResourcesEventHandlerType>(L"newlyAddedCreateResources");
        newlyAddedCreateResources.SetExpectedCalls(1,
            [=, &newlyAddedCreateResources]
            {
                f->MarkControlDeviceAsLost();

                f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
                    [=, &newlyAddedCreateResources]
                    {
                        f->OnCreateResources.SetExpectedCalls(1);
                        newlyAddedCreateResources.SetExpectedCalls(1);
                        return nullptr;
                    });

                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f->AddCreateResourcesHandler(newlyAddedCreateResources.Get());

        Expectations::Instance()->ValidateNotSatisfied();
        f->Adapter->RaiseCompositionRenderingEvent();
    }


    TEST_METHOD_EX(CanvasControl_WhenCreateResourceHandlerCalledAsItIsAddedAndTheDeviceIsLost_ThenDeviceIsRecreatedAndAllHandlersCalledAgain_EvenIfDeviceIsLostImmediatelyAfterRecreation)
    {
        auto f = std::make_shared<CreateResourcesFixture>();
        f->OnCreateResources.SetExpectedCalls(1);
        f->RaiseLoadedEvent();

        int devicesLostInARow = 10;
        auto newlyAddedCreateResources = MockEventHandler<CreateResourcesEventHandlerType>(L"newlyAddedCreateResources");

        newlyAddedCreateResources.SetExpectedCalls(devicesLostInARow,
            [=]
            {
                Assert::AreEqual(S_OK, f->GetDeviceRemovedReason());
                    
                if (newlyAddedCreateResources.GetCurrentCallCount() != devicesLostInARow)
                {
                    f->MarkControlDeviceAsLost();
                    f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1);
                    f->OnCreateResources.SetExpectedCalls(1);
                    return DXGI_ERROR_DEVICE_REMOVED;
                }
                else
                {
                    return S_OK;
                }
            });

        f->AddCreateResourcesHandler(newlyAddedCreateResources.Get());
        for (int i=0; i<devicesLostInARow - 1; ++i)
        {
            Expectations::Instance()->ValidateNotSatisfied();
            f->Adapter->RaiseCompositionRenderingEvent();
        }
    }


    TEST_METHOD_EX(CanvasControl_WhenCreateResourceHandlerCallsAsItIsAddedAndItAlwaysThrowsDeviceLost_ThenExceptionIsPropagated)
    {
        auto f = std::make_shared<CreateResourcesFixture>();
        f->OnCreateResources.SetExpectedCalls(1);
        f->RaiseLoadedEvent();

        auto newlyAddedCreateResources = MockEventHandler<CreateResourcesEventHandlerType>(L"newlyAddedCreateResources");

        // The CanvasControl's device hasn't actually been lost, so we don't
        // expect it to be recreated.
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(0);

        newlyAddedCreateResources.SetExpectedCalls(1, DXGI_ERROR_DEVICE_REMOVED);

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED,
            [&]
            {
                f->AddCreateResourcesHandler(newlyAddedCreateResources.Get());
            });
    }

    struct CreateResourcesAndDrawFixture : public CreateResourcesFixture
    {
        MockEventHandler<DrawEventHandlerType> OnDraw;

        CreateResourcesAndDrawFixture()
            : OnDraw(MockEventHandler<DrawEventHandlerType>(L"Draw"))
        {
            OnCreateResources.SetExpectedCalls(1);
            RaiseLoadedEvent(); 
            AddDrawHandler(OnDraw.Get());

            Adapter->CreateCanvasImageSourceMethod.AllowAnyCall(
                [](ICanvasDevice* device, int, int, CanvasBackground)
                {
                    auto d3dDevice = GetDXGIInterface<ID3D11Device>(device);
                    ThrowIfFailed(d3dDevice->GetDeviceRemovedReason());
                    return nullptr;
                });
        }   
    };

    TEST_METHOD_EX(CanvasControl_WhenDrawingAndCreateImageSourceReportsDeviceLost_CreateResourcesIsCalledAndDrawSucceeds)
    {
        auto f = std::make_shared<CreateResourcesAndDrawFixture>();

        f->MarkControlDeviceAsLost();

        // First Rendering we don't expect OnDraw to be called because
        // CreateCanvasImageSource fails before we get that far.
        f->RaiseCompositionRenderingEvent();

        // For the second Rendering, we expect CreateCanvasDevice to be called
        // to recreate the lost device...
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
            [=]
            {
                // ...after that we expect CreateResources to be called to
                // recreate resources
                f->OnCreateResources.SetExpectedCalls(1,
                    [=]
                    {
                        // ...and then we expect OnDraw.
                        f->OnDraw.SetExpectedCalls(1);
                        return S_OK;
                    });

                return nullptr;
            });

        f->RaiseCompositionRenderingEvent();        
    }

    TEST_METHOD_EX(CanvasControl_WhenCanvasImageSourceDrawingSessionFactoryCreateReportsDeviceLost_CreateResourcesIsCalledAndDrawSucceeds)
    {
        auto f = std::make_shared<CreateResourcesAndDrawFixture>();

        CallCounter<> canvasImageSourceDrawingSessionFactoryCreate(L"canvasImageSourceDrawingSessionFactoryCreate");
        canvasImageSourceDrawingSessionFactoryCreate.SetExpectedCalls(1);

        // The first time this is called we report device lost...
        f->Adapter->OnCanvasImageSourceDrawingSessionFactory_Create = 
            [=, &canvasImageSourceDrawingSessionFactoryCreate]() -> ComPtr<MockCanvasDrawingSession>
            {
                canvasImageSourceDrawingSessionFactoryCreate.WasCalled();

                // ...but only the first time
                f->Adapter->OnCanvasImageSourceDrawingSessionFactory_Create = nullptr;

                // We mark the control device as lost so that everything is
                // consistent
                f->MarkControlDeviceAsLost();

                // And return the failure.
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
            };
        
        f->RaiseCompositionRenderingEvent();

        // We expect CreateCanvasDevice to be called to recreate the lost
        // device...
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
            [=]
            {
                // ...after that we expect CreateResources to be called to
                // recreate resources
                f->OnCreateResources.SetExpectedCalls(1,
                    [=]
                    {
                        // ...and then we expect OnDraw.
                        f->OnDraw.SetExpectedCalls(1);
                        return S_OK;
                    });

                return nullptr;
            });

        f->RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerReportsDeviceLost_CreateResourcesIsCalledAndDrawIsCalledAgain)
    {
        auto f = std::make_shared<CreateResourcesAndDrawFixture>();

        // First OnDraw will report device lost
        f->OnDraw.SetExpectedCalls(1,
            [=]
            {
                f->MarkControlDeviceAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        f->RaiseCompositionRenderingEvent();

        // then we expect CreateCanvasDevice...
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
            [=]
            {
                // ...CreateResources
                f->OnCreateResources.SetExpectedCalls(1,
                    [=]
                    {
                        // ...finally OnDraw
                        f->OnDraw.SetExpectedCalls(1);
                        return S_OK;
                    });
                
                return nullptr;
            });

        f->Adapter->RaiseCompositionRenderingEvent();
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawHandlerAlwaysThrowsDeviceLost_ThenExceptionIsPropagated)
    {
        auto f = std::make_shared<CreateResourcesAndDrawFixture>();
        
        f->OnDraw.SetExpectedCalls(1, [=] { return DXGI_ERROR_DEVICE_REMOVED; });

        ExpectHResultException(DXGI_ERROR_DEVICE_REMOVED, 
            [=] 
            { 
                f->Adapter->RaiseCompositionRenderingEvent();
            });
    }

    TEST_METHOD_EX(CanvasControl_WhenDrawingSessionCloseThrowsDeviceLost_CreateResourcesIsCalledAndDrawIsCalledAgain)
    {
        auto f = std::make_shared<CreateResourcesAndDrawFixture>();

        f->OnDraw.SetExpectedCalls(1);

        // The first CanvasDrawingSession created will throw device lost when
        // closed.
        f->Adapter->OnCanvasImageSourceDrawingSessionFactory_Create =
            [=]
            {
                auto drawingSession = Make<MockCanvasDrawingSession>();
                drawingSession->CloseMethod.SetExpectedCalls(1, 
                    [=]
                    {
                        f->MarkControlDeviceAsLost();
                        return DXGI_ERROR_DEVICE_REMOVED; 
                    });                
                return drawingSession;
            };

        f->RaiseCompositionRenderingEvent();

        // After the first CloseMethod call we reset the drawing
        // session factory to generate drawing sessions that
        // don't fail.
        f->Adapter->OnCanvasImageSourceDrawingSessionFactory_Create = nullptr;
        
        // After this we expect the following to happen:
        f->Adapter->CreateCanvasDeviceMethod.SetExpectedCalls(1,
            [=]
            {
                f->OnCreateResources.SetExpectedCalls(1,
                    [=]
                    {
                        f->OnDraw.SetExpectedCalls(1);
                        return S_OK;
                    });
                return nullptr;
            });

        f->RaiseCompositionRenderingEvent();
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
                f.ExpectOneCreateCanvasImageSource(testStep.ResizeWidth, testStep.ResizeHeight);

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
        f.ExpectOneCreateCanvasImageSource(anyWidth, anyHeight);
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

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height, CanvasBackground backgroundMode) override
        {
            Assert::AreEqual(m_expectedImageSourceWidth, width, L"ExpectedImageSourceWidth");
            Assert::AreEqual(m_expectedImageSourceHeight, height, L"ExpectedImageSourceHeight");

            return __super::CreateCanvasImageSource(device, width, height, backgroundMode);
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
        int width, 
        int height, 
        CanvasBackground backgroundMode) override
    {
        auto result = CreateCanvasImageSourceMethod.WasCalled(device, width, height, backgroundMode);
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
        int m_lastImageSourceWidth;
        int m_lastImageSourceHeight;
        int m_imageSourceCount;

        CanvasControlTestAdapter_VerifyDpi(ID2D1DeviceContext* deviceContext)
            : CanvasControlTestAdapter_InjectDeviceContext(deviceContext)
            , m_dpi(DEFAULT_DPI)
            , m_lastImageSourceWidth(0)
            , m_lastImageSourceHeight(0)
            , m_imageSourceCount(0) 
        {}

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height, CanvasBackground backgroundMode) override
        {
            m_lastImageSourceWidth = width;
            m_lastImageSourceHeight = height;
            m_imageSourceCount++;
            
            return __super::CreateCanvasImageSource(device, width, height, backgroundMode);
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
            DEFAULT_DPI - (DEFAULT_DPI * FLT_EPSILON),
            DEFAULT_DPI,
            DEFAULT_DPI + (DEFAULT_DPI * FLT_EPSILON),
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
        }
    }
};


TEST_CLASS(CanvasControl_ExternalEvents)
{
    struct Fixture : public BasicControlFixture
    {
        ComPtr<MockWindow> Window;
        MockEventHandler<DrawEventHandlerType> OnDraw;

        Fixture()
            : Window(Adapter->GetCurrentMockWindow())
            , OnDraw(MockEventHandler<DrawEventHandlerType>(L"Draw"))
        {
        }

        void CreateControl()
        {
            BasicControlFixture::CreateControl();
            AddDrawHandler(OnDraw.Get());
            RaiseLoadedEvent();
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

    TEST_METHOD_EX(CanvasControl_WhenAppSuspended_TrimCalledOnDevice)
    {
        Fixture f;

        f.CreateControl();
        
        ComPtr<ICanvasDevice> icanvasDevice;
        ThrowIfFailed(f.Control->get_Device(&icanvasDevice));
        StubCanvasDevice* canvasDevice = dynamic_cast<StubCanvasDevice*>(icanvasDevice.Get());

        canvasDevice->TrimMethod.SetExpectedCalls(1);

        ThrowIfFailed(f.Adapter->SuspendingEventSource->InvokeAll(nullptr, nullptr));
    }
};


TEST_CLASS(CanvasControl_ClearColor)
{
    struct Fixture
    {
        ComPtr<MockD2DDeviceContext> DeviceContext;
        std::shared_ptr<CanvasControlTestAdapter> Adapter;
        ComPtr<CanvasControl> Control;
        ComPtr<StubUserControl> UserControl;
        MockEventHandler<DrawEventHandlerType> OnDraw;

        Fixture()
        {
            DeviceContext = Make<MockD2DDeviceContext>();
            DeviceContext->ClearMethod.AllowAnyCall();
            DeviceContext->SetTransformMethod.AllowAnyCall();
            DeviceContext->SetDpiMethod.AllowAnyCall();

            Adapter = std::make_shared<CanvasControlTestAdapter_InjectDeviceContext>(DeviceContext.Get());
            Control = Make<CanvasControl>(Adapter);
            UserControl = dynamic_cast<StubUserControl*>(As<IUserControl>(Control).Get());

            OnDraw = MockEventHandler<DrawEventHandlerType>(L"Draw");

            EventRegistrationToken ignoredToken;
            ThrowIfFailed(Control->add_Draw(OnDraw.Get(), &ignoredToken));

            Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
            ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));

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
            [&](ICanvasDevice*, int, int, CanvasBackground backgroundMode)
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
            [&](ICanvasDevice*, int, int, CanvasBackground backgroundMode)
            {
                Assert::AreEqual(CanvasBackground::Transparent, backgroundMode);
                return nullptr;
            });

        f.RaiseAnyNumberOfCompositionRenderingEvents();
    }
};
