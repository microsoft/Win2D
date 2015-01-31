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
    using namespace ABI::Windows::UI::Xaml;

    class StubImageControl : public RuntimeClass<IImage, IUIElement>
    {
    public:
        //
        // IImage
        //

        IFACEMETHODIMP get_Source(ABI::Windows::UI::Xaml::Media::IImageSource **) override { return S_OK; }
        IFACEMETHODIMP put_Source(ABI::Windows::UI::Xaml::Media::IImageSource *) override { return S_OK; }
        IFACEMETHODIMP get_Stretch(ABI::Windows::UI::Xaml::Media::Stretch *) override { return S_OK; }
        IFACEMETHODIMP put_Stretch(ABI::Windows::UI::Xaml::Media::Stretch) override { return S_OK; }
        IFACEMETHODIMP get_NineGrid(ABI::Windows::UI::Xaml::Thickness *) override { return S_OK; }
        IFACEMETHODIMP put_NineGrid(ABI::Windows::UI::Xaml::Thickness) override { return S_OK; }
        IFACEMETHODIMP get_PlayToSource(ABI::Windows::Media::PlayTo::IPlayToSource **) override { return S_OK; }
        IFACEMETHODIMP add_ImageFailed(ABI::Windows::UI::Xaml::IExceptionRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ImageFailed(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ImageOpened(ABI::Windows::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ImageOpened(EventRegistrationToken) override { return S_OK; }

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
