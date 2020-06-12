// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockShape : public RuntimeClass<IShape, IUIElement>
{
public:
    // IShape

    MOCK_METHOD1(get_Fill               , HRESULT(ABI::Microsoft::UI::Xaml::Media::IBrush **));
    MOCK_METHOD1(put_Fill               , HRESULT(ABI::Microsoft::UI::Xaml::Media::IBrush *));
    MOCK_METHOD1(get_Stroke             , HRESULT(ABI::Microsoft::UI::Xaml::Media::IBrush **));
    MOCK_METHOD1(put_Stroke             , HRESULT(ABI::Microsoft::UI::Xaml::Media::IBrush *));
    MOCK_METHOD1(get_StrokeMiterLimit   , HRESULT(double *));
    MOCK_METHOD1(put_StrokeMiterLimit   , HRESULT(double));
    MOCK_METHOD1(get_StrokeThickness    , HRESULT(double *));
    MOCK_METHOD1(put_StrokeThickness    , HRESULT(double));
    MOCK_METHOD1(get_StrokeStartLineCap , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap *));
    MOCK_METHOD1(put_StrokeStartLineCap , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap));
    MOCK_METHOD1(get_StrokeEndLineCap   , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap *));
    MOCK_METHOD1(put_StrokeEndLineCap   , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap));
    MOCK_METHOD1(get_StrokeLineJoin     , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineJoin *));
    MOCK_METHOD1(put_StrokeLineJoin     , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineJoin));
    MOCK_METHOD1(get_StrokeDashOffset   , HRESULT(double *));
    MOCK_METHOD1(put_StrokeDashOffset   , HRESULT(double));
    MOCK_METHOD1(get_StrokeDashCap      , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap *));
    MOCK_METHOD1(put_StrokeDashCap      , HRESULT(ABI::Microsoft::UI::Xaml::Media::PenLineCap));
    MOCK_METHOD1(get_StrokeDashArray    , HRESULT(ABI::Windows::Foundation::Collections::__FIVector_1_double_t **));
    MOCK_METHOD1(put_StrokeDashArray    , HRESULT(ABI::Windows::Foundation::Collections::__FIVector_1_double_t *));
    MOCK_METHOD1(get_Stretch            , HRESULT(ABI::Microsoft::UI::Xaml::Media::Stretch *));
    MOCK_METHOD1(put_Stretch            , HRESULT(ABI::Microsoft::UI::Xaml::Media::Stretch));
    MOCK_METHOD1(get_GeometryTransform  , HRESULT(ABI::Microsoft::UI::Xaml::Media::ITransform **));

    // IUIElement

    MOCK_METHOD1(get_DesiredSize                    , HRESULT(ABI::Windows::Foundation::Size *));
    MOCK_METHOD1(get_AllowDrop                      , HRESULT(boolean *));
    MOCK_METHOD1(put_AllowDrop                      , HRESULT(boolean));
    MOCK_METHOD1(get_Opacity                        , HRESULT(double *));
    MOCK_METHOD1(put_Opacity                        , HRESULT(double));
    MOCK_METHOD1(get_Clip                           , HRESULT(ABI::Microsoft::UI::Xaml::Media::IRectangleGeometry **));
    MOCK_METHOD1(put_Clip                           , HRESULT(ABI::Microsoft::UI::Xaml::Media::IRectangleGeometry *));
    MOCK_METHOD1(get_RenderTransform                , HRESULT(ABI::Microsoft::UI::Xaml::Media::ITransform **));
    MOCK_METHOD1(put_RenderTransform                , HRESULT(ABI::Microsoft::UI::Xaml::Media::ITransform *));
    MOCK_METHOD1(get_Projection                     , HRESULT(ABI::Microsoft::UI::Xaml::Media::IProjection **));
    MOCK_METHOD1(put_Projection                     , HRESULT(ABI::Microsoft::UI::Xaml::Media::IProjection *));
    MOCK_METHOD1(get_RenderTransformOrigin          , HRESULT(ABI::Windows::Foundation::Point *));
    MOCK_METHOD1(put_RenderTransformOrigin          , HRESULT(ABI::Windows::Foundation::Point));
    MOCK_METHOD1(get_IsHitTestVisible               , HRESULT(boolean *));
    MOCK_METHOD1(put_IsHitTestVisible               , HRESULT(boolean));
    MOCK_METHOD1(get_Visibility                     , HRESULT(ABI::Microsoft::UI::Xaml::Visibility *));
    MOCK_METHOD1(put_Visibility                     , HRESULT(ABI::Microsoft::UI::Xaml::Visibility));
    MOCK_METHOD1(get_RenderSize                     , HRESULT(ABI::Windows::Foundation::Size *));
    MOCK_METHOD1(get_UseLayoutRounding              , HRESULT(boolean *));
    MOCK_METHOD1(put_UseLayoutRounding              , HRESULT(boolean));
    MOCK_METHOD1(get_Transitions                    , HRESULT(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t **));
    MOCK_METHOD1(put_Transitions                    , HRESULT(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition_t *));
    MOCK_METHOD1(get_CacheMode                      , HRESULT(ABI::Microsoft::UI::Xaml::Media::ICacheMode **));
    MOCK_METHOD1(put_CacheMode                      , HRESULT(ABI::Microsoft::UI::Xaml::Media::ICacheMode *));
    MOCK_METHOD1(get_IsTapEnabled                   , HRESULT(boolean *));
    MOCK_METHOD1(put_IsTapEnabled                   , HRESULT(boolean));
    MOCK_METHOD1(get_IsDoubleTapEnabled             , HRESULT(boolean *));
    MOCK_METHOD1(put_IsDoubleTapEnabled             , HRESULT(boolean));
    MOCK_METHOD1(get_IsRightTapEnabled              , HRESULT(boolean *));
    MOCK_METHOD1(put_IsRightTapEnabled              , HRESULT(boolean));
    MOCK_METHOD1(get_IsHoldingEnabled               , HRESULT(boolean *));
    MOCK_METHOD1(put_IsHoldingEnabled               , HRESULT(boolean));
    MOCK_METHOD1(get_ManipulationMode               , HRESULT(ABI::Microsoft::UI::Xaml::Input::ManipulationModes *));
    MOCK_METHOD1(put_ManipulationMode               , HRESULT(ABI::Microsoft::UI::Xaml::Input::ManipulationModes));
    MOCK_METHOD1(get_PointerCaptures                , HRESULT(ABI::Windows::Foundation::Collections::__FIVectorView_1_Windows__CUI__CXaml__CInput__CPointer_t **));
    MOCK_METHOD2(add_KeyUp                          , HRESULT(ABI::Microsoft::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_KeyUp                       , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_KeyDown                        , HRESULT(ABI::Microsoft::UI::Xaml::Input::IKeyEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_KeyDown                     , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_GotFocus                       , HRESULT(ABI::Microsoft::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_GotFocus                    , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_LostFocus                      , HRESULT(ABI::Microsoft::UI::Xaml::IRoutedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_LostFocus                   , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_DragEnter                      , HRESULT(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_DragEnter                   , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_DragLeave                      , HRESULT(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_DragLeave                   , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_DragOver                       , HRESULT(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_DragOver                    , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_Drop                           , HRESULT(ABI::Microsoft::UI::Xaml::IDragEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_Drop                        , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerPressed                 , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerPressed              , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerMoved                   , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerMoved                , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerReleased                , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerReleased             , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerEntered                 , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerEntered              , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerExited                  , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerExited               , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerCaptureLost             , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerCaptureLost          , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerCanceled                , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerCanceled             , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_PointerWheelChanged            , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointerEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_PointerWheelChanged         , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_Tapped                         , HRESULT(ABI::Microsoft::UI::Xaml::Input::ITappedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_Tapped                      , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_DoubleTapped                   , HRESULT(ABI::Microsoft::UI::Xaml::Input::IDoubleTappedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_DoubleTapped                , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_Holding                        , HRESULT(ABI::Microsoft::UI::Xaml::Input::IHoldingEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_Holding                     , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_RightTapped                    , HRESULT(ABI::Microsoft::UI::Xaml::Input::IRightTappedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_RightTapped                 , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_ManipulationStarting           , HRESULT(ABI::Microsoft::UI::Xaml::Input::IManipulationStartingEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_ManipulationStarting        , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_ManipulationInertiaStarting    , HRESULT(ABI::Microsoft::UI::Xaml::Input::IManipulationInertiaStartingEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_ManipulationInertiaStarting , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_ManipulationStarted            , HRESULT(ABI::Microsoft::UI::Xaml::Input::IManipulationStartedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_ManipulationStarted         , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_ManipulationDelta              , HRESULT(ABI::Microsoft::UI::Xaml::Input::IManipulationDeltaEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_ManipulationDelta           , HRESULT(EventRegistrationToken));
    MOCK_METHOD2(add_ManipulationCompleted          , HRESULT(ABI::Microsoft::UI::Xaml::Input::IManipulationCompletedEventHandler *,EventRegistrationToken *));
    MOCK_METHOD1(remove_ManipulationCompleted       , HRESULT(EventRegistrationToken));
    MOCK_METHOD1(Measure                            , HRESULT(ABI::Windows::Foundation::Size));
    MOCK_METHOD1(Arrange                            , HRESULT(ABI::Windows::Foundation::Rect));
    MOCK_METHOD2(CapturePointer                     , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointer *,boolean *));
    MOCK_METHOD1(ReleasePointerCapture              , HRESULT(ABI::Microsoft::UI::Xaml::Input::IPointer *));
    MOCK_METHOD0(ReleasePointerCaptures             , HRESULT());
    MOCK_METHOD3(AddHandler                         , HRESULT(ABI::Microsoft::UI::Xaml::IRoutedEvent *,IInspectable *,boolean));
    MOCK_METHOD2(RemoveHandler                      , HRESULT(ABI::Microsoft::UI::Xaml::IRoutedEvent *,IInspectable *));
    MOCK_METHOD2(TransformToVisual                  , HRESULT(ABI::Microsoft::UI::Xaml::IUIElement *,ABI::Microsoft::UI::Xaml::Media::IGeneralTransform **));
    MOCK_METHOD0(InvalidateMeasure                  , HRESULT());
    MOCK_METHOD0(InvalidateArrange                  , HRESULT());
    MOCK_METHOD0(UpdateLayout                       , HRESULT());

};
