// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::UI::Xaml;

#pragma warning(push)
#pragma warning(disable: 4996)  // Disable warning about PlayToSource being deprecated (we must use it to implement IImage)

    class StubImageControl : public RuntimeClass<IImage, IUIElement>
    {
    public:
        //
        // IImage
        //

        IFACEMETHODIMP get_Source(ABI::Microsoft::UI::Xaml::Media::IImageSource **) override { return S_OK; }
        IFACEMETHODIMP put_Source(ABI::Microsoft::UI::Xaml::Media::IImageSource *) override { return S_OK; }
        IFACEMETHODIMP get_Stretch(ABI::Microsoft::UI::Xaml::Media::Stretch *) override { return S_OK; }
        IFACEMETHODIMP put_Stretch(ABI::Microsoft::UI::Xaml::Media::Stretch) override { return S_OK; }
        IFACEMETHODIMP get_NineGrid(ABI::Microsoft::UI::Xaml::Thickness *) override { return S_OK; }
        IFACEMETHODIMP put_NineGrid(ABI::Microsoft::UI::Xaml::Thickness) override { return S_OK; }
        IFACEMETHODIMP GetAsCastingSource(ABI::Windows::Media::Casting::ICastingSource** result) override { return S_OK; }
        IFACEMETHODIMP GetAlphaMask(ABI::Microsoft::UI::Composition::ICompositionBrush** result) override { return S_OK; }

#ifndef WINUI3
        IFACEMETHODIMP get_PlayToSource(ABI::Windows::Media::PlayTo::IPlayToSource **) override { return S_OK; }
#endif

        IFACEMETHODIMP add_ImageFailed(ABI::Microsoft::UI::Xaml::IExceptionRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ImageFailed(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ImageOpened(ABI::Microsoft::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ImageOpened(EventRegistrationToken) override { return S_OK; }

        //
        // IUIElement
        //

        IFACEMETHODIMP get_DesiredSize(ABI::Windows::Foundation::Size *) override { return S_OK; }
        IFACEMETHODIMP get_AllowDrop(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_AllowDrop(boolean) override { return S_OK; }
        IFACEMETHODIMP get_Opacity(DOUBLE *) override { return S_OK; }
        IFACEMETHODIMP put_Opacity(DOUBLE) override { return S_OK; }
        IFACEMETHODIMP get_Clip(ABI::Microsoft::UI::Xaml::Media::IRectangleGeometry **) override { return S_OK; }
        IFACEMETHODIMP put_Clip(ABI::Microsoft::UI::Xaml::Media::IRectangleGeometry *) override { return S_OK; }
        IFACEMETHODIMP get_RenderTransform(ABI::Microsoft::UI::Xaml::Media::ITransform **) override { return S_OK; }
        IFACEMETHODIMP put_RenderTransform(ABI::Microsoft::UI::Xaml::Media::ITransform *) override { return S_OK; }
        IFACEMETHODIMP get_Projection(ABI::Microsoft::UI::Xaml::Media::IProjection **) override { return S_OK; }
        IFACEMETHODIMP put_Projection(ABI::Microsoft::UI::Xaml::Media::IProjection *) override { return S_OK; }
        IFACEMETHODIMP get_Transform3D(ABI::Microsoft::UI::Xaml::Media::Media3D::ITransform3D** value) override { return S_OK; }
        IFACEMETHODIMP put_Transform3D(ABI::Microsoft::UI::Xaml::Media::Media3D::ITransform3D* value) override { return S_OK; }
        IFACEMETHODIMP get_RenderTransformOrigin(ABI::Windows::Foundation::Point *) override { return S_OK; }
        IFACEMETHODIMP put_RenderTransformOrigin(ABI::Windows::Foundation::Point) override { return S_OK; }
        IFACEMETHODIMP get_IsHitTestVisible(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsHitTestVisible(boolean) override { return S_OK; }
        IFACEMETHODIMP get_Visibility(ABI::Microsoft::UI::Xaml::Visibility *) override { return S_OK; }
        IFACEMETHODIMP put_Visibility(ABI::Microsoft::UI::Xaml::Visibility) override { return S_OK; }
        IFACEMETHODIMP get_RenderSize(ABI::Windows::Foundation::Size *) override { return S_OK; }
        IFACEMETHODIMP get_UseLayoutRounding(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_UseLayoutRounding(boolean) override { return S_OK; }

#ifdef WINUI3
        IFACEMETHODIMP get_Transitions(__FIVector_1_Microsoft__CUI__CXaml__CMedia__CAnimation__CTransition**) override { return S_OK; }
        IFACEMETHODIMP put_Transitions(__FIVector_1_Microsoft__CUI__CXaml__CMedia__CAnimation__CTransition*) override { return S_OK; }
#else
        IFACEMETHODIMP get_Transitions(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t **) override { return S_OK; }
        IFACEMETHODIMP put_Transitions(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t *) override { return S_OK; }
#endif

        IFACEMETHODIMP get_CacheMode(ABI::Microsoft::UI::Xaml::Media::ICacheMode **) override { return S_OK; }
        IFACEMETHODIMP put_CacheMode(ABI::Microsoft::UI::Xaml::Media::ICacheMode *) override { return S_OK; }
        IFACEMETHODIMP get_IsTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsDoubleTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsDoubleTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsRightTapEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsRightTapEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsHoldingEnabled(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsHoldingEnabled(boolean) override { return S_OK; }
        IFACEMETHODIMP get_ManipulationMode(ABI::Microsoft::UI::Xaml::Input::ManipulationModes *) override { return S_OK; }
        IFACEMETHODIMP put_ManipulationMode(ABI::Microsoft::UI::Xaml::Input::ManipulationModes) override { return S_OK; }

#ifdef WINUI3
        IFACEMETHODIMP get_CanDrag(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_CanDrag(boolean) override { return S_OK; }
        IFACEMETHODIMP get_ContextFlyout(ABI::Microsoft::UI::Xaml::Controls::Primitives::IFlyoutBase** value) override { return S_OK; }
        IFACEMETHODIMP put_ContextFlyout(ABI::Microsoft::UI::Xaml::Controls::Primitives::IFlyoutBase*) override { return S_OK; }
        IFACEMETHODIMP get_CompositeMode(ABI::Microsoft::UI::Xaml::Media::ElementCompositeMode*) override { return S_OK; }
        IFACEMETHODIMP put_CompositeMode(ABI::Microsoft::UI::Xaml::Media::ElementCompositeMode) override { return S_OK; }
        IFACEMETHODIMP get_Lights(__FIVector_1_Microsoft__CUI__CXaml__CMedia__CXamlLight**) override { return S_OK; }
        IFACEMETHODIMP get_CanBeScrollAnchor(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_CanBeScrollAnchor(boolean) override { return S_OK; }
        IFACEMETHODIMP get_ExitDisplayModeOnAccessKeyInvoked(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_ExitDisplayModeOnAccessKeyInvoked(boolean) override { return S_OK; }
        IFACEMETHODIMP get_IsAccessKeyScope(boolean *) override { return S_OK; }
        IFACEMETHODIMP put_IsAccessKeyScope(boolean) override { return S_OK; }
        IFACEMETHODIMP get_AccessKeyScopeOwner(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_AccessKeyScopeOwner(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_AccessKey(HSTRING*) override { return S_OK; }
        IFACEMETHODIMP put_AccessKey(HSTRING) override { return S_OK; }
        IFACEMETHODIMP get_KeyTipPlacementMode(ABI::Microsoft::UI::Xaml::Input::KeyTipPlacementMode*) override { return S_OK; }
        IFACEMETHODIMP put_KeyTipPlacementMode(ABI::Microsoft::UI::Xaml::Input::KeyTipPlacementMode) override { return S_OK; }
        IFACEMETHODIMP get_KeyTipHorizontalOffset(double *) override { return S_OK; }
        IFACEMETHODIMP put_KeyTipHorizontalOffset(double) override { return S_OK; }
        IFACEMETHODIMP get_KeyTipVerticalOffset(double *) override { return S_OK; }
        IFACEMETHODIMP put_KeyTipVerticalOffset(double) override { return S_OK; }
        IFACEMETHODIMP get_KeyTipTarget(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_KeyTipTarget(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusKeyboardNavigation(ABI::Microsoft::UI::Xaml::Input::XYFocusKeyboardNavigationMode*) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusKeyboardNavigation(ABI::Microsoft::UI::Xaml::Input::XYFocusKeyboardNavigationMode) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusUpNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy*) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusUpNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusDownNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy*) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusDownNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusLeftNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy*) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusLeftNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusRightNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy*) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusRightNavigationStrategy(ABI::Microsoft::UI::Xaml::Input::XYFocusNavigationStrategy) override { return S_OK; }
        IFACEMETHODIMP get_KeyboardAccelerators(__FIVector_1_Microsoft__CUI__CXaml__CInput__CKeyboardAccelerator**) override { return S_OK; }
        IFACEMETHODIMP get_KeyboardAcceleratorPlacementTarget(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_KeyboardAcceleratorPlacementTarget(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_KeyboardAcceleratorPlacementMode(ABI::Microsoft::UI::Xaml::Input::KeyboardAcceleratorPlacementMode*) override { return S_OK; }
        IFACEMETHODIMP put_KeyboardAcceleratorPlacementMode(ABI::Microsoft::UI::Xaml::Input::KeyboardAcceleratorPlacementMode) override { return S_OK; }
        IFACEMETHODIMP get_HighContrastAdjustment(ABI::Microsoft::UI::Xaml::ElementHighContrastAdjustment*) override { return S_OK; }
        IFACEMETHODIMP put_HighContrastAdjustment(ABI::Microsoft::UI::Xaml::ElementHighContrastAdjustment) override { return S_OK; }
        IFACEMETHODIMP get_TabFocusNavigation(ABI::Microsoft::UI::Xaml::Input::KeyboardNavigationMode*) override { return S_OK; }
        IFACEMETHODIMP put_TabFocusNavigation(ABI::Microsoft::UI::Xaml::Input::KeyboardNavigationMode) override { return S_OK; }
        IFACEMETHODIMP get_OpacityTransition(ABI::Microsoft::UI::Xaml::IScalarTransition**) override { return S_OK; }
        IFACEMETHODIMP put_OpacityTransition(ABI::Microsoft::UI::Xaml::IScalarTransition*) override { return S_OK; }
        IFACEMETHODIMP get_Translation(ABI::Windows::Foundation::Numerics::Vector3*) override { return S_OK; }
        IFACEMETHODIMP put_Translation(ABI::Windows::Foundation::Numerics::Vector3) override { return S_OK; }
        IFACEMETHODIMP get_TranslationTransition(ABI::Microsoft::UI::Xaml::IVector3Transition**) override { return S_OK; }
        IFACEMETHODIMP put_TranslationTransition(ABI::Microsoft::UI::Xaml::IVector3Transition*) override { return S_OK; }
        IFACEMETHODIMP get_Rotation(FLOAT*) override { return S_OK; }
        IFACEMETHODIMP put_Rotation(FLOAT) override { return S_OK; }
        IFACEMETHODIMP get_RotationTransition(ABI::Microsoft::UI::Xaml::IScalarTransition**) override { return S_OK; }
        IFACEMETHODIMP put_RotationTransition(ABI::Microsoft::UI::Xaml::IScalarTransition*) override { return S_OK; }
        IFACEMETHODIMP get_Scale(ABI::Windows::Foundation::Numerics::Vector3*) override { return S_OK; }
        IFACEMETHODIMP put_Scale(ABI::Windows::Foundation::Numerics::Vector3) override { return S_OK; }
        IFACEMETHODIMP get_ScaleTransition(ABI::Microsoft::UI::Xaml::IVector3Transition**) override { return S_OK; }
        IFACEMETHODIMP put_ScaleTransition(ABI::Microsoft::UI::Xaml::IVector3Transition*) override { return S_OK; }
        IFACEMETHODIMP get_TransformMatrix(ABI::Windows::Foundation::Numerics::Matrix4x4*) override { return S_OK; }
        IFACEMETHODIMP put_TransformMatrix(ABI::Windows::Foundation::Numerics::Matrix4x4) override { return S_OK; }
        IFACEMETHODIMP get_CenterPoint(ABI::Windows::Foundation::Numerics::Vector3*) override { return S_OK; }
        IFACEMETHODIMP put_CenterPoint(ABI::Windows::Foundation::Numerics::Vector3) override { return S_OK; }
        IFACEMETHODIMP get_RotationAxis(ABI::Windows::Foundation::Numerics::Vector3*) override { return S_OK; }
        IFACEMETHODIMP put_RotationAxis(ABI::Windows::Foundation::Numerics::Vector3) override { return S_OK; }
        IFACEMETHODIMP get_ActualOffset(ABI::Windows::Foundation::Numerics::Vector3*) override { return S_OK; }
        IFACEMETHODIMP get_ActualSize(ABI::Windows::Foundation::Numerics::Vector2*) override { return S_OK; }
        IFACEMETHODIMP get_XamlRoot(ABI::Microsoft::UI::Xaml::IXamlRoot**) override { return S_OK; }
        IFACEMETHODIMP put_XamlRoot(ABI::Microsoft::UI::Xaml::IXamlRoot*) override { return S_OK; }
        IFACEMETHODIMP get_Shadow(ABI::Microsoft::UI::Xaml::Media::IShadow**) override { return S_OK; }
        IFACEMETHODIMP put_Shadow(ABI::Microsoft::UI::Xaml::Media::IShadow*) override { return S_OK; }
        IFACEMETHODIMP get_RasterizationScale(DOUBLE*) override { return S_OK; }
        IFACEMETHODIMP put_RasterizationScale(DOUBLE) override { return S_OK; }
        IFACEMETHODIMP get_FocusState(ABI::Microsoft::UI::Xaml::FocusState*) override { return S_OK; }
        IFACEMETHODIMP get_UseSystemFocusVisuals(boolean*) override { return S_OK; }
        IFACEMETHODIMP put_UseSystemFocusVisuals(boolean) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusLeft(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusLeft(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusRight(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusRight(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusUp(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusUp(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_XYFocusDown(ABI::Microsoft::UI::Xaml::IDependencyObject**) override { return S_OK; }
        IFACEMETHODIMP put_XYFocusDown(ABI::Microsoft::UI::Xaml::IDependencyObject*) override { return S_OK; }
        IFACEMETHODIMP get_IsTabStop(boolean*) override { return S_OK; }
        IFACEMETHODIMP put_IsTabStop(boolean) override { return S_OK; }
        IFACEMETHODIMP get_TabIndex(INT32*) override { return S_OK; }
        IFACEMETHODIMP put_TabIndex(INT32) override { return S_OK; }
        IFACEMETHODIMP add_DragStarting(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CDragStartingEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_DragStarting(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DropCompleted(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CDropCompletedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_DropCompleted(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_CharacterReceived(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CCharacterReceivedRoutedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_CharacterReceived(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ContextRequested(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CContextRequestedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_ContextRequested(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ContextCanceled(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CRoutedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_ContextCanceled(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_AccessKeyDisplayRequested(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CAccessKeyDisplayRequestedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_AccessKeyDisplayRequested(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_AccessKeyDisplayDismissed(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CAccessKeyDisplayDismissedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_AccessKeyDisplayDismissed(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_AccessKeyInvoked(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CAccessKeyInvokedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_AccessKeyInvoked(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ProcessKeyboardAccelerators(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CProcessKeyboardAcceleratorEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_ProcessKeyboardAccelerators(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_GettingFocus(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CGettingFocusEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_GettingFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_LosingFocus(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CLosingFocusEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_LosingFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_NoFocusCandidateFound(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CInput__CNoFocusCandidateFoundEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_NoFocusCandidateFound(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_BringIntoViewRequested(__FITypedEventHandler_2_Microsoft__CUI__CXaml__CUIElement_Microsoft__CUI__CXaml__CBringIntoViewRequestedEventArgs*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_BringIntoViewRequested(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP CancelDirectManipulations(boolean*) override { return S_OK; }
        IFACEMETHODIMP StartDragAsync(ABI::Microsoft::UI::Input::Experimental::IExpPointerPoint*, __FIAsyncOperation_1_Windows__CApplicationModel__CDataTransfer__CDataPackageOperation**) override { return S_OK; }
        IFACEMETHODIMP TryInvokeKeyboardAccelerator(ABI::Microsoft::UI::Xaml::Input::IProcessKeyboardAcceleratorEventArgs*) override { return S_OK; }
        IFACEMETHODIMP StartBringIntoView(void) override { return S_OK; }
        IFACEMETHODIMP StartBringIntoViewWithOptions(ABI::Microsoft::UI::Xaml::IBringIntoViewOptions*) override { return S_OK; }
        IFACEMETHODIMP Focus(ABI::Microsoft::UI::Xaml::FocusState, boolean*) override { return S_OK; }
        IFACEMETHODIMP StartAnimation(ABI::Microsoft::UI::Composition::ICompositionAnimationBase*) override { return S_OK; }
        IFACEMETHODIMP StopAnimation(ABI::Microsoft::UI::Composition::ICompositionAnimationBase*) override { return S_OK; }
        IFACEMETHODIMP add_PreviewKeyUp(__x_ABI_CMicrosoft_CUI_CXaml_CInput_CIKeyEventHandler*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_PreviewKeyUp(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PreviewKeyDown(__x_ABI_CMicrosoft_CUI_CXaml_CInput_CIKeyEventHandler*, EventRegistrationToken*) override { return S_OK; }
        IFACEMETHODIMP remove_PreviewKeyDown(EventRegistrationToken) override { return S_OK; }
#endif

#ifdef WINUI3
        IFACEMETHODIMP get_PointerCaptures(__FIVectorView_1_Microsoft__CUI__CXaml__CInput__CPointer**) override { return S_OK; }
#else
        IFACEMETHODIMP get_PointerCaptures(ABI::Windows::Foundation::Collections::__FIVectorView_1_Windows__CUI__CXaml__CInput__CPointer_t **) override { return S_OK; }
#endif

        IFACEMETHODIMP add_KeyUp(ABI::Microsoft::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_KeyUp(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_KeyDown(ABI::Microsoft::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_KeyDown(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_GotFocus(ABI::Microsoft::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_GotFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_LostFocus(ABI::Microsoft::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_LostFocus(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragEnter(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragEnter(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragLeave(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragLeave(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DragOver(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DragOver(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Drop(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Drop(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerPressed(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerPressed(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerMoved(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerMoved(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerReleased(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerReleased(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerEntered(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerEntered(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerExited(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerExited(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerCaptureLost(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerCaptureLost(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerCanceled(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerCanceled(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_PointerWheelChanged(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_PointerWheelChanged(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Tapped(ABI::Microsoft::UI::Xaml::Input::ITappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Tapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_DoubleTapped(ABI::Microsoft::UI::Xaml::Input::IDoubleTappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_DoubleTapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_Holding(ABI::Microsoft::UI::Xaml::Input::IHoldingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_Holding(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_RightTapped(ABI::Microsoft::UI::Xaml::Input::IRightTappedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_RightTapped(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationStarting(ABI::Microsoft::UI::Xaml::Input::IManipulationStartingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationStarting(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationInertiaStarting(ABI::Microsoft::UI::Xaml::Input::IManipulationInertiaStartingEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationInertiaStarting(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationStarted(ABI::Microsoft::UI::Xaml::Input::IManipulationStartedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationStarted(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationDelta(ABI::Microsoft::UI::Xaml::Input::IManipulationDeltaEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationDelta(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP add_ManipulationCompleted(ABI::Microsoft::UI::Xaml::Input::IManipulationCompletedEventHandler *,EventRegistrationToken *) override { return S_OK; }
        IFACEMETHODIMP remove_ManipulationCompleted(EventRegistrationToken) override { return S_OK; }
        IFACEMETHODIMP Measure(ABI::Windows::Foundation::Size) override { return S_OK; }
        IFACEMETHODIMP Arrange(ABI::Windows::Foundation::Rect) override { return S_OK; }
        IFACEMETHODIMP CapturePointer(ABI::Microsoft::UI::Xaml::Input::IPointer *,boolean *) override { return S_OK; }
        IFACEMETHODIMP ReleasePointerCapture(ABI::Microsoft::UI::Xaml::Input::IPointer *) override { return S_OK; }
        IFACEMETHODIMP ReleasePointerCaptures(void) override { return S_OK; }
        IFACEMETHODIMP AddHandler(ABI::Microsoft::UI::Xaml::IRoutedEvent *,IInspectable *,boolean) override { return S_OK; }
        IFACEMETHODIMP RemoveHandler(ABI::Microsoft::UI::Xaml::IRoutedEvent *,IInspectable *) override { return S_OK; }
        IFACEMETHODIMP TransformToVisual(ABI::Microsoft::UI::Xaml::IUIElement *,ABI::Microsoft::UI::Xaml::Media::IGeneralTransform **) override { return S_OK; }
        IFACEMETHODIMP InvalidateMeasure(void) override { return S_OK; }
        IFACEMETHODIMP InvalidateArrange(void) override { return S_OK; }
        IFACEMETHODIMP UpdateLayout(void) override { return S_OK; }
    };

#pragma warning(pop)
}
