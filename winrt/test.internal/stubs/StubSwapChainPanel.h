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

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::UI::Core;

    class StubSwapChainPanel : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>, 
        ISwapChainPanel, 
        IFrameworkElement, 
        IUIElement,
        ISwapChainPanelNative>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetSwapChainMethod, HRESULT(IDXGISwapChain*));
        CALL_COUNTER_WITH_MOCK(get_CompositionScaleXMethod, HRESULT(FLOAT*));
        CALL_COUNTER_WITH_MOCK(get_CompositionScaleYMethod, HRESULT(FLOAT*));
        CALL_COUNTER_WITH_MOCK(CreateCoreIndependentInputSourceMethod, HRESULT(CoreInputDeviceTypes, ICoreInputSourceBase**));

        ComPtr<MockEventSource<IRoutedEventHandler>> LoadedEventSource;

        StubSwapChainPanel()
            : LoadedEventSource(Make<MockEventSource<IRoutedEventHandler>>(L"Loaded"))
        {
        }

        // ISwapChainPanelNative
        IFACEMETHODIMP SetSwapChain(IDXGISwapChain* swapChain) override
        {
            return SetSwapChainMethod.WasCalled(swapChain);
        }

        // ISwapChainPanel

        IFACEMETHODIMP get_CompositionScaleX(FLOAT* value) override
        {
            return get_CompositionScaleXMethod.WasCalled(value);
        }

        IFACEMETHODIMP get_CompositionScaleY(FLOAT* value) override
        {
            return get_CompositionScaleYMethod.WasCalled(value);
        }

        IFACEMETHODIMP add_CompositionScaleChanged(ITypedEventHandler<SwapChainPanel*, IInspectable*>*, EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_CompositionScaleChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCoreIndependentInputSource(CoreInputDeviceTypes type, ICoreInputSourceBase** out) override
        {
            return CreateCoreIndependentInputSourceMethod.WasCalled(type, out);
        }

        IFACEMETHODIMP remove_CompositionScaleChanged(EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_CompositionScaleChanged");
            return E_NOTIMPL;
        }

        // IFrameworkElement

        IFACEMETHODIMP get_Triggers(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CTriggerBase_t **) override 
        {
            Assert::Fail(L"Unexpected call to get_Triggers");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Resources(IResourceDictionary **) override 
        {
            Assert::Fail(L"Unexpected call to get_Resources");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Resources(IResourceDictionary *) override 
        {
            Assert::Fail(L"Unexpected call to put_Resources");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Tag(IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to get_Tag");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Tag(IInspectable *) override 
        {
            Assert::Fail(L"Unexpected call to put_Tag");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Language(HSTRING *) override 
        {
            Assert::Fail(L"Unexpected call to get_Language");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Language(HSTRING) override 
        {
            Assert::Fail(L"Unexpected call to put_Language");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_ActualWidth(double* value) override
        {
            Assert::Fail(L"Unexpected call to get_ActualWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_ActualHeight(double* value) override 
        {
            Assert::Fail(L"Unexpected call to get_ActualHeight");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Width(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_Width");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Width(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_Width");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Height(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_Height");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Height(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_Height");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MinWidth(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MinWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MinWidth(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MinWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MaxWidth(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MaxWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MaxWidth(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MaxWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MinHeight(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MinHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MinHeight(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MinHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MaxHeight(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MaxHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MaxHeight(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MaxHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_HorizontalAlignment(HorizontalAlignment *) override 
        {
            Assert::Fail(L"Unexpected call to get_HorizontalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_HorizontalAlignment(HorizontalAlignment) override 
        {
            Assert::Fail(L"Unexpected call to put_HorizontalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_VerticalAlignment(VerticalAlignment *) override 
        {
            Assert::Fail(L"Unexpected call to get_VerticalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_VerticalAlignment(VerticalAlignment) override 
        {
            Assert::Fail(L"Unexpected call to put_VerticalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Margin(Thickness *) override 
        {
            Assert::Fail(L"Unexpected call to get_Margin");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Margin(Thickness) override 
        {
            Assert::Fail(L"Unexpected call to put_Margin");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Name(HSTRING *) override 
        {
            Assert::Fail(L"Unexpected call to get_Name");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Name(HSTRING) override 
        {
            Assert::Fail(L"Unexpected call to put_Name");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_BaseUri(ABI::Windows::Foundation::IUriRuntimeClass **) override 
        {
            Assert::Fail(L"Unexpected call to get_BaseUri");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_DataContext(IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to get_DataContext");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_DataContext(IInspectable *) override 
        {
            Assert::Fail(L"Unexpected call to put_DataContext");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Style(IStyle **) override 
        {
            Assert::Fail(L"Unexpected call to get_Style");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Style(IStyle *) override 
        {
            Assert::Fail(L"Unexpected call to put_Style");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Parent(IDependencyObject **) override 
        {
            Assert::Fail(L"Unexpected call to get_Parent");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_FlowDirection(FlowDirection *) override 
        {
            Assert::Fail(L"Unexpected call to get_FlowDirection");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_FlowDirection(FlowDirection) override 
        {
            Assert::Fail(L"Unexpected call to put_FlowDirection");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP add_Loaded(IRoutedEventHandler* handler, EventRegistrationToken* token) override 
        {
            return LoadedEventSource->add_Event(handler, token);
        }

        IFACEMETHODIMP remove_Loaded(EventRegistrationToken token) override 
        {
            Assert::Fail(L"Unexpected call to remove_Loaded");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_Unloaded(IRoutedEventHandler *,EventRegistrationToken *) override 
        {
            Assert::Fail(L"Unexpected call to add_Unloaded");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP remove_Unloaded(EventRegistrationToken) override 
        {
            Assert::Fail(L"Unexpected call to remove_Unloaded");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP add_SizeChanged(ISizeChangedEventHandler* handler, EventRegistrationToken* token) override 
        {
            Assert::Fail(L"Unexpected call to add_SizeChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_SizeChanged(EventRegistrationToken token) override 
        {
            Assert::Fail(L"Unexpected call to remove_SizeChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_LayoutUpdated(ABI::Windows::Foundation::__FIEventHandler_1_IInspectable_t *,EventRegistrationToken *) override 
        {
            Assert::Fail(L"Unexpected call to add_LayoutUpdated");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP remove_LayoutUpdated(EventRegistrationToken) override 
        {
            Assert::Fail(L"Unexpected call to remove_LayoutUpdated");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP FindName(HSTRING,IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to FindName");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP SetBinding(IDependencyProperty *,Data::IBindingBase *) override 
        {
            Assert::Fail(L"Unexpected call to SetBinding");
            return E_NOTIMPL; 
        }
        
        //
        // IUIElement
        //

        IFACEMETHODIMP get_DesiredSize(ABI::Windows::Foundation::Size *) override { return S_OK; }
        IFACEMETHODIMP get_AllowDrop(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_AllowDrop(boolean) override { return S_OK; }
        IFACEMETHODIMP get_Opacity(DOUBLE *) override { return S_OK; }
        IFACEMETHODIMP put_Opacity(DOUBLE) override { return S_OK; }
        IFACEMETHODIMP get_Clip(ABI::Windows::UI::Xaml::Media::IRectangleGeometry **) override { return S_OK; }
        IFACEMETHODIMP put_Clip(ABI::Windows::UI::Xaml::Media::IRectangleGeometry *) override { return S_OK; }
        IFACEMETHODIMP get_RenderTransform(ABI::Windows::UI::Xaml::Media::ITransform **) override { return S_OK; }
        IFACEMETHODIMP put_RenderTransform(ABI::Windows::UI::Xaml::Media::ITransform *) override { return S_OK; }
        IFACEMETHODIMP get_Projection(ABI::Windows::UI::Xaml::Media::IProjection **) override { return S_OK; }
        IFACEMETHODIMP put_Projection(ABI::Windows::UI::Xaml::Media::IProjection *) override { return S_OK; }
        IFACEMETHODIMP get_RenderTransformOrigin(ABI::Windows::Foundation::Point *) override { return S_OK; }
        IFACEMETHODIMP put_RenderTransformOrigin(ABI::Windows::Foundation::Point) override { return S_OK; }
        IFACEMETHODIMP get_IsHitTestVisible(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsHitTestVisible(boolean) override { return S_OK; }
        IFACEMETHODIMP get_Visibility(ABI::Windows::UI::Xaml::Visibility *) override { return S_OK; }
        IFACEMETHODIMP put_Visibility(ABI::Windows::UI::Xaml::Visibility) override { return S_OK; }
        IFACEMETHODIMP get_RenderSize(ABI::Windows::Foundation::Size *) override { return S_OK; }
        IFACEMETHODIMP get_UseLayoutRounding(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_UseLayoutRounding(boolean) override { return S_OK; }
        IFACEMETHODIMP get_Transitions(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t **) override { return S_OK; }
        IFACEMETHODIMP put_Transitions(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t *) override { return S_OK; }
        IFACEMETHODIMP get_CacheMode(ABI::Windows::UI::Xaml::Media::ICacheMode **) override { return S_OK; }
        IFACEMETHODIMP put_CacheMode(ABI::Windows::UI::Xaml::Media::ICacheMode *) override { return S_OK; }
        IFACEMETHODIMP get_IsTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsDoubleTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsDoubleTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsRightTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsRightTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsHoldingEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsHoldingEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_ManipulationMode(ABI::Windows::UI::Xaml::Input::ManipulationModes *) override { return S_OK; }
        IFACEMETHODIMP put_ManipulationMode(ABI::Windows::UI::Xaml::Input::ManipulationModes) override { return S_OK; }
        IFACEMETHODIMP get_PointerCaptures(ABI::Windows::Foundation::Collections::__FIVectorView_1_Windows__CUI__CXaml__CInput__CPointer_t **) override { return S_OK; }
        IFACEMETHODIMP add_KeyUp(ABI::Windows::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_KeyUp(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_KeyDown(ABI::Windows::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_KeyDown(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_GotFocus(ABI::Windows::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_GotFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_LostFocus(ABI::Windows::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_LostFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragEnter(ABI::Windows::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragEnter(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragLeave(ABI::Windows::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragLeave(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragOver(ABI::Windows::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragOver(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Drop(ABI::Windows::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Drop(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerPressed(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerPressed(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerMoved(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerMoved(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerReleased(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerReleased(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerEntered(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerEntered(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerExited(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerExited(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerCaptureLost(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerCaptureLost(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerCanceled(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerCanceled(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerWheelChanged(ABI::Windows::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerWheelChanged(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Tapped(ABI::Windows::UI::Xaml::Input::ITappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Tapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DoubleTapped(ABI::Windows::UI::Xaml::Input::IDoubleTappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DoubleTapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Holding(ABI::Windows::UI::Xaml::Input::IHoldingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Holding(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_RightTapped(ABI::Windows::UI::Xaml::Input::IRightTappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_RightTapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationStarting(ABI::Windows::UI::Xaml::Input::IManipulationStartingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationStarting(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationInertiaStarting(ABI::Windows::UI::Xaml::Input::IManipulationInertiaStartingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationInertiaStarting(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationStarted(ABI::Windows::UI::Xaml::Input::IManipulationStartedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationStarted(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationDelta(ABI::Windows::UI::Xaml::Input::IManipulationDeltaEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationDelta(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationCompleted(ABI::Windows::UI::Xaml::Input::IManipulationCompletedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationCompleted(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP Measure(ABI::Windows::Foundation::Size) override { return S_OK; }
        IFACEMETHODIMP Arrange(ABI::Windows::Foundation::Rect) override { return S_OK; }
        IFACEMETHODIMP CapturePointer(ABI::Windows::UI::Xaml::Input::IPointer *,boolean *) override { return S_OK; }
        IFACEMETHODIMP ReleasePointerCapture(ABI::Windows::UI::Xaml::Input::IPointer *) override { return S_OK; }
        IFACEMETHODIMP ReleasePointerCaptures(void) override { return S_OK; }
        IFACEMETHODIMP AddHandler(ABI::Windows::UI::Xaml::IRoutedEvent *,IInspectable *,boolean) override { return S_OK; }
        IFACEMETHODIMP RemoveHandler(ABI::Windows::UI::Xaml::IRoutedEvent *,IInspectable *) override { return S_OK; }
        IFACEMETHODIMP TransformToVisual(ABI::Windows::UI::Xaml::IUIElement *,ABI::Windows::UI::Xaml::Media::IGeneralTransform **) override { return S_OK; }
        IFACEMETHODIMP InvalidateMeasure(void) override { return S_OK; }
        IFACEMETHODIMP InvalidateArrange(void) override { return S_OK; }
        IFACEMETHODIMP UpdateLayout(void) override { return S_OK; }
    };
}

