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

using namespace canvas;
using namespace ABI::Microsoft::Graphics::Canvas;
namespace canvasABI = ABI::Microsoft::Graphics::Canvas;


TEST_CLASS(CanvasControlTests)
{
    std::shared_ptr<CanvasControlTestAdapter> m_adapter;

    TEST_METHOD_INITIALIZE(Init)
    {
        m_adapter = std::make_shared<CanvasControlTestAdapter>();
        m_creatingResourcesCallbackCount = 0;
        m_drawingCallbackCount = 0;
    }

    TEST_METHOD(CanvasControl_Implements_Expected_Interfaces)
    {
        using canvas::CanvasControl;
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);

        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ICanvasControl);
        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ABI::Windows::UI::Xaml::Controls::IUserControl);
        ASSERT_IMPLEMENTS_INTERFACE(canvasControl, ICanvasResourceCreator);
    }

    TEST_METHOD(CanvasControl_DeviceProperty_Null)
    {
        using canvas::CanvasControl;
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);

        Assert::AreEqual(E_INVALIDARG, canvasControl->get_Device(nullptr));
    }

    TEST_METHOD(CanvasControl_DrawingEventArgs_Getter)
    {
        using canvas::CanvasDrawingEventArgs;

        ComPtr<ICanvasDrawingSession> drawingSession = Make<MockCanvasDrawingSession>();

        auto drawingEventArgs = Make<CanvasDrawingEventArgs>(drawingSession.Get());

        // Verify that an exception is returned for nullptr on getter
        Assert::AreEqual(E_INVALIDARG, drawingEventArgs->get_DrawingSession(nullptr));

        // Verify that the getter in a typical case works
        ComPtr<ICanvasDrawingSession> drawingSessionRetrieved;
        drawingEventArgs->get_DrawingSession(&drawingSessionRetrieved);

        Assert::AreEqual(drawingSession.Get(), drawingSessionRetrieved.Get());
    }

    HRESULT OnCreatingResources(canvasABI::ICanvasControl* sender, IInspectable* args)
    {
        Assert::IsNotNull(sender);
        Assert::IsNull(args); // Args are never used.

        m_creatingResourcesCallbackCount++;

        return S_OK;
    }

    HRESULT OnDrawing(ICanvasControl* sender, ICanvasDrawingEventArgs* args)
    {
        Assert::IsNotNull(sender);
        Assert::IsNotNull(args);

        m_drawingCallbackCount++;

        return S_OK;
    }

    HRESULT OnDrawing_NoNullCheck(ICanvasControl* sender, ICanvasDrawingEventArgs* args)
    {
        m_drawingCallbackCount++;

        return S_OK;
    }

    TEST_METHOD(CanvasControl_Callbacks)
    {
        using namespace ABI::Windows::Foundation;
        using canvas::CanvasControl;

        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(m_adapter);
        Assert::AreEqual(0, m_creatingResourcesCallbackCount);
        Assert::AreEqual(0, m_drawingCallbackCount);

        // Register one CreateResources handler.
        // Note that Loaded hasn't occured yet, so it shouldn't actually be fired.
        auto onCreatingResourcesFn = Callback<CreateResourcesEventHandlerType>(this, &CanvasControlTests::OnCreatingResources);
        EventRegistrationToken creatingResourcesEventToken0;
        ThrowIfFailed(canvasControl->add_CreatingResources(onCreatingResourcesFn.Get(), &creatingResourcesEventToken0));
        Assert::AreEqual(0, m_creatingResourcesCallbackCount);

        // Issue a Loaded.
        // Should fire CreateResources.
        canvasControl->OnLoaded(nullptr, nullptr);
        Assert::AreEqual(1, m_creatingResourcesCallbackCount);

        // Register the CreateResources handler again.
        // Because the Loaded event has already occurred, add_CreatingResources should immediately fire the event too.
        EventRegistrationToken creatingResourcesEventToken1;
        ThrowIfFailed(canvasControl->add_CreatingResources(onCreatingResourcesFn.Get(), &creatingResourcesEventToken1));
        Assert::AreEqual(2, m_creatingResourcesCallbackCount);

        // Register the Drawing handler.
        auto onDrawingFn = Callback<DrawingEventHandlerType>(this, &CanvasControlTests::OnDrawing);
        EventRegistrationToken drawingEventToken;
        ThrowIfFailed(canvasControl->add_Drawing(onDrawingFn.Get(), &drawingEventToken));

        // Invalidate and ensure the drawing callback is called.
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(1, m_drawingCallbackCount);

        // Ensure a subsequent invalidation doesn't recreate resources.
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(2, m_creatingResourcesCallbackCount);
        Assert::AreEqual(2, m_drawingCallbackCount);

        // Unregister the events. Call invalidate. Ensure the handler doesn't get called again (the event was correctly unregistered).
        ThrowIfFailed(canvasControl->remove_CreatingResources(creatingResourcesEventToken0));
        ThrowIfFailed(canvasControl->remove_CreatingResources(creatingResourcesEventToken1));
        ThrowIfFailed(canvasControl->remove_Drawing(drawingEventToken));
        canvasControl->Invalidate();
        m_adapter->FireCompositionRenderingEvent(static_cast<ICanvasControl*>(canvasControl.Get()));

        Assert::AreEqual(2, m_creatingResourcesCallbackCount);
        Assert::AreEqual(2, m_drawingCallbackCount);

        // Unregistering the same event twice should do nothing.
        ThrowIfFailed(canvasControl->remove_CreatingResources(creatingResourcesEventToken0));
        ThrowIfFailed(canvasControl->remove_CreatingResources(creatingResourcesEventToken1));
        ThrowIfFailed(canvasControl->remove_Drawing(drawingEventToken));
    }

    int m_creatingResourcesCallbackCount;
    int m_drawingCallbackCount;
};
