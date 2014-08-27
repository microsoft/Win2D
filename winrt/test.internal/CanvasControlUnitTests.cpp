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

TEST_CLASS(CanvasControlTests_CommonAdapter)
{
    std::shared_ptr<CanvasControlTestAdapter> m_adapter;

    TEST_METHOD_INITIALIZE(Init)
    {
        m_adapter = std::make_shared<CanvasControlTestAdapter>();
        m_createResourcesCallbackCount = 0;
        m_drawCallbackCount = 0;
    }

    TEST_METHOD(CanvasControl_Implements_Expected_Interfaces)
    {
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);

        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ICanvasControl);
        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ABI::Windows::UI::Xaml::Controls::IUserControl);
        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ICanvasResourceCreator);
    }

    TEST_METHOD(CanvasControl_DeviceProperty_Null)
    {
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);

        Assert::AreEqual(E_INVALIDARG, canvasControl->get_Device(nullptr));
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

    HRESULT OnCreateResources(ICanvasControl* sender, IInspectable* args)
    {
        Assert::IsNotNull(sender);
        Assert::IsNull(args); // Args are never used.

        m_createResourcesCallbackCount++;

        return S_OK;
    }

    HRESULT OnDraw(ICanvasControl* sender, ICanvasDrawEventArgs* args)
    {
        Assert::IsNotNull(sender);
        Assert::IsNotNull(args);

        m_drawCallbackCount++;

        return S_OK;
    }

    HRESULT OnDraw_NoNullCheck(ICanvasControl* sender, ICanvasDrawEventArgs* args)
    {
        m_drawCallbackCount++;

        return S_OK;
    }

    TEST_METHOD(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;

        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);
        Assert::AreEqual(0, m_createResourcesCallbackCount);
        Assert::AreEqual(0, m_drawCallbackCount);

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreateResourcesFn = Callback<CreateResourcesEventHandlerType>(this, &CanvasControlTests_CommonAdapter::OnCreateResources);
        EventRegistrationToken createResourcesEventToken0;
        ThrowIfFailed(canvasControl->add_CreateResources(onCreateResourcesFn.Get(), &createResourcesEventToken0));
        Assert::AreEqual(0, m_createResourcesCallbackCount);

        // Issue a Loaded.
        // Should fire CreateResources.
        canvasControl->OnLoaded(nullptr, nullptr);
        Assert::AreEqual(1, m_createResourcesCallbackCount);

        // Register the CreateResources handler again.
        // Because the Loaded event has already occurred, add_CreateResources should immediately fire the event too.
        EventRegistrationToken createResourcesEventToken1;
        ThrowIfFailed(canvasControl->add_CreateResources(onCreateResourcesFn.Get(), &createResourcesEventToken1));
        Assert::AreEqual(2, m_createResourcesCallbackCount);

        // Register the Draw handler.
        auto onDrawFn = Callback<DrawEventHandlerType>(this, &CanvasControlTests_CommonAdapter::OnDraw);
        EventRegistrationToken drawEventToken;
        ThrowIfFailed(canvasControl->add_Draw(onDrawFn.Get(), &drawEventToken));

        // Invalidate and ensure the Draw callback is called.
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(1, m_drawCallbackCount);

        // Ensure a subsequent invalidation doesn't recreate resources.
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(2, m_createResourcesCallbackCount);
        Assert::AreEqual(2, m_drawCallbackCount);

        // Unregister the events. Call invalidate. Ensure the handler doesn't get called again (the event was correctly unregistered).
        ThrowIfFailed(canvasControl->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(canvasControl->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(canvasControl->remove_Draw(drawEventToken));
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(2, m_createResourcesCallbackCount);
        Assert::AreEqual(2, m_drawCallbackCount);

        // Unregistering the same event twice should do nothing.
        ThrowIfFailed(canvasControl->remove_CreateResources(createResourcesEventToken0));
        ThrowIfFailed(canvasControl->remove_CreateResources(createResourcesEventToken1));
        ThrowIfFailed(canvasControl->remove_Draw(drawEventToken));
    }

    int m_createResourcesCallbackCount;
    int m_drawCallbackCount;
};

TEST_CLASS(CanvasControlTests_AdapterWithResizing)
{
    class SizableTestControl : public StubUserControl
    {
    public:
        double m_width;
        double m_height;

        SizableTestControl()
            : m_width(128)
            , m_height(128) {}

        IFACEMETHODIMP get_ActualWidth(double* value) override
        {
            *value = m_width;
            return S_OK;
        }

        IFACEMETHODIMP get_ActualHeight(double* value) override
        {
            *value = m_height;
            return S_OK;
        }
    };

    class CanvasControlTestAdapter_VerifyCreateImageSource : public CanvasControlTestAdapter
    {
    public:

        int m_imageSourceCount;
        int m_lastImageSourceWidth;
        int m_lastImageSourceHeight;
        ComPtr<SizableTestControl> m_userControl;

        CanvasControlTestAdapter_VerifyCreateImageSource()
            : m_imageSourceCount(0)
            , m_lastImageSourceWidth(0)
            , m_lastImageSourceHeight(0)
            , m_userControl(Make<SizableTestControl>())
        {}


        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override
        {
            ComPtr<IInspectable> inspectableControl;
            ThrowIfFailed(m_userControl.As(&inspectableControl));

            return std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>>(inspectableControl, m_userControl);
        }

        virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override
        {
            m_imageSourceCount++;
            m_lastImageSourceWidth = width;
            m_lastImageSourceHeight = height;

            return __super::CreateCanvasImageSource(device, width, height);
        }
    };

    TEST_METHOD(CanvasControl_Resizing)
    {
        std::shared_ptr<CanvasControlTestAdapter_VerifyCreateImageSource> adapter =
            std::make_shared<CanvasControlTestAdapter_VerifyCreateImageSource>();

        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(adapter);
        canvasControl->OnLoaded(nullptr, nullptr);
        Assert::AreEqual(0, adapter->m_imageSourceCount);
        Assert::AreEqual(0, adapter->m_lastImageSourceWidth);
        Assert::AreEqual(0, adapter->m_lastImageSourceHeight);

        struct TestCase
        {
            int ResizeWidth;
            int ResizeHeight;
            bool ExpectRecreation;
        } testSteps[]
        {
            { 100, 100, true }, // Initial sizing; resource always re-created
            { 123, 456, true }, // Change width and height
            { 50, 456, true }, // Change width only
            { 50, 51, true }, // Change height only
            { 50, 51, false }, // Change nothing
        };

        int expectedImageSourceCount = 0;
        for (size_t i = 0; i < _countof(testSteps); ++i)
        {
            if (testSteps[i].ExpectRecreation) expectedImageSourceCount++;

            adapter->m_userControl->m_width = testSteps[i].ResizeWidth;
            adapter->m_userControl->m_height = testSteps[i].ResizeHeight;
            canvasControl->Invalidate();
            adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);
            Assert::AreEqual(testSteps[i].ResizeWidth, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(testSteps[i].ResizeHeight, adapter->m_lastImageSourceHeight);
        }
    }

    class CanvasControlTestAdapter_VerifyDpi : public CanvasControlTestAdapter
    {
    public:

        float m_dpi;
        int m_lastImageSourceWidth;
        int m_lastImageSourceHeight;
        int m_imageSourceCount;
        float m_lastDpiX;
        float m_lastDpiY;
        int m_numSetDpiCalls;

        CanvasControlTestAdapter_VerifyDpi()
            : m_dpi(DEFAULT_DPI)
            , m_lastImageSourceWidth(0)
            , m_lastImageSourceHeight(0)
            , m_lastDpiX(0)
            , m_lastDpiY(0)
            , m_numSetDpiCalls(0)
            , m_mockD2DDeviceContext(Make<MockD2DDeviceContext>())
            , m_imageSourceCount(0) {}

        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override
        {
            ComPtr<SizableTestControl> sizedControl = Make<SizableTestControl>();
            sizedControl->m_width = 1000;
            sizedControl->m_height = 1000;

            ComPtr<IInspectable> inspectableControl;
            ThrowIfFailed(sizedControl.As(&inspectableControl));

            return std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>>(inspectableControl, sizedControl);
        }

        virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override
        {
            m_lastImageSourceWidth = width;
            m_lastImageSourceHeight = height;
            m_imageSourceCount++;
            
            m_mockD2DDeviceContext->MockSetDpi =
                [&](float dpiX, float dpiY)
                {
                    m_lastDpiX = dpiX;
                    m_lastDpiY = dpiY;
                    m_numSetDpiCalls++;
                };     

            m_mockD2DDeviceContext->MockSetTransform =
                [&](const D2D1_MATRIX_3X2_F* m)
                {
                };

            auto sisFactory = Make<MockSurfaceImageSourceFactory>();
            sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
                [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
                {
                    auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
                                        
                    mockSurfaceImageSource->MockBeginDraw =
                        [&](const RECT& updateRect, const IID& iid, void** updateObject, POINT* offset)
                        {
                            ThrowIfFailed(m_mockD2DDeviceContext.CopyTo(iid, updateObject));
                        };

                    mockSurfaceImageSource->MockSetDevice =
                        [&](IUnknown*)
                        {
                        };
                                        
                    mockSurfaceImageSource->MockEndDraw =
                        [&]()
                        {
                        };

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

    private:

        ComPtr<MockD2DDeviceContext> m_mockD2DDeviceContext; 
    };

    HRESULT OnDraw(ICanvasControl* sender, ICanvasDrawEventArgs* args)
    {
        return S_OK;
    }

    TEST_METHOD(CanvasControl_Dpi)
    {
        float dpiCases[] = {
            50, 
            DEFAULT_DPI - (DEFAULT_DPI * FLT_EPSILON),
            DEFAULT_DPI,
            DEFAULT_DPI + (DEFAULT_DPI * FLT_EPSILON),
            200};

        for (size_t i = 0; i < _countof(dpiCases); ++i)
        {
            std::shared_ptr<CanvasControlTestAdapter_VerifyDpi> adapter =
                std::make_shared<CanvasControlTestAdapter_VerifyDpi>();

            int expectedNumSetDpiCalls = 0;
            int expectedImageSourceCount = 0;

            adapter->m_dpi = dpiCases[i];

            ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(adapter);
            canvasControl->OnLoaded(nullptr, nullptr);

            // An event handler needs to be registered for a drawing session to be contructed.
            auto onDrawFn = 
                Callback<DrawEventHandlerType>(this, &CanvasControlTests_AdapterWithResizing::OnDraw);
            EventRegistrationToken drawEventToken;
            ThrowIfFailed(canvasControl->add_Draw(onDrawFn.Get(), &drawEventToken));

            canvasControl->Invalidate();
            adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

            expectedImageSourceCount++;
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);

            // Verify the backing store size is correct
            const float dpiScale = dpiCases[i] / DEFAULT_DPI;
            float expectedBackingStoreDim = 1000 * dpiScale;
            float truncatedDimF = ceil(expectedBackingStoreDim);
            assert(truncatedDimF <= INT_MAX);
            int truncatedDimI = static_cast<int>(truncatedDimF);

            Assert::AreEqual(truncatedDimI, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(truncatedDimI, adapter->m_lastImageSourceHeight);

            expectedNumSetDpiCalls++;
            Assert::AreEqual(expectedNumSetDpiCalls, adapter->m_numSetDpiCalls);
            Assert::AreEqual(dpiCases[i], adapter->m_lastDpiX);
            Assert::AreEqual(dpiCases[i], adapter->m_lastDpiY);

            // Verify the public, device-independent size of the control.
            ComPtr<IFrameworkElement> controlAsFrameworkElement;
            ThrowIfFailed(canvasControl.As(&controlAsFrameworkElement));
            DOUBLE verifyWidth, verifyHeight;
            ThrowIfFailed(controlAsFrameworkElement->get_ActualWidth(&verifyWidth));
            ThrowIfFailed(controlAsFrameworkElement->get_ActualHeight(&verifyHeight));
            Assert::AreEqual(verifyWidth, 1000.0);
            Assert::AreEqual(verifyHeight, 1000.0);

            // Fire a DpiChanged.
            adapter->m_dpi = DEFAULT_DPI;
            bool expectResize = 1000 != ceil(1000 * dpiCases[i] / DEFAULT_DPI);
            adapter->FireDpiChangedEvent();

            // Here, verify that no recreation occurred yet. The backing store recreation
            // should occur at the next rendering event.
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);

            adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

            // Verify the backing store got resized as appropriate.
            if (expectResize) expectedImageSourceCount++;
            Assert::AreEqual(expectedImageSourceCount, adapter->m_imageSourceCount);
            Assert::AreEqual(1000, adapter->m_lastImageSourceWidth);
            Assert::AreEqual(1000, adapter->m_lastImageSourceHeight);

            expectedNumSetDpiCalls++;
            Assert::AreEqual(expectedNumSetDpiCalls, adapter->m_numSetDpiCalls);
            Assert::AreEqual(DEFAULT_DPI, adapter->m_lastDpiX);
            Assert::AreEqual(DEFAULT_DPI, adapter->m_lastDpiY);

            // Verify the size of the control again.
            ThrowIfFailed(controlAsFrameworkElement->get_ActualWidth(&verifyWidth));
            ThrowIfFailed(controlAsFrameworkElement->get_ActualHeight(&verifyHeight));
            Assert::AreEqual(verifyWidth, 1000.0);
            Assert::AreEqual(verifyHeight, 1000.0);
        }


    }
};