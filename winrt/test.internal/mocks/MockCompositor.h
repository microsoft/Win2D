// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

using namespace ABI::Microsoft::UI::Composition;

class MockCompositor : public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>,
    ICompositor,
    ICompositorInterop>
{
public:
    // ICompositor
    MOCK_METHOD1(CreateColorKeyFrameAnimation            , HRESULT(IColorKeyFrameAnimation** result));
    MOCK_METHOD1(CreateColorBrush                        , HRESULT(ICompositionColorBrush** result));
    MOCK_METHOD2(CreateColorBrushWithColor               , HRESULT(Color color, ICompositionColorBrush** result));
    MOCK_METHOD1(CreateContainerVisual                   , HRESULT(IContainerVisual** result));
    MOCK_METHOD3(CreateCubicBezierEasingFunction         , HRESULT(Vector2 controlPoint1, Vector2 controlPoint2, ICubicBezierEasingFunction** result));
    MOCK_METHOD2(CreateEffectFactory                     , HRESULT(IGraphicsEffect* graphicsEffect, ICompositionEffectFactory** result));
    MOCK_METHOD3(CreateEffectFactoryWithProperties       , HRESULT(IGraphicsEffect* graphicsEffect, IIterable<HSTRING>* animatableProperties, ICompositionEffectFactory* *result));
    MOCK_METHOD1(CreateExpressionAnimation               , HRESULT(IExpressionAnimation** result));
    MOCK_METHOD2(CreateExpressionAnimationWithExpression , HRESULT(HSTRING expression, IExpressionAnimation** result));
    MOCK_METHOD1(CreateInsetClip                         , HRESULT(IInsetClip** result));
    MOCK_METHOD5(CreateInsetClipWithInsets               , HRESULT(FLOAT leftInset, FLOAT topInset, FLOAT rightInset, FLOAT bottomInset, IInsetClip** result));
    MOCK_METHOD1(CreateLinearEasingFunction              , HRESULT(ILinearEasingFunction** result));
    MOCK_METHOD1(CreatePropertySet                       , HRESULT(ICompositionPropertySet** result));
    MOCK_METHOD1(CreateQuaternionKeyFrameAnimation       , HRESULT(IQuaternionKeyFrameAnimation** result));
    MOCK_METHOD1(CreateScalarKeyFrameAnimation           , HRESULT(IScalarKeyFrameAnimation** result));
    MOCK_METHOD2(CreateScopedBatch                       , HRESULT(CompositionBatchTypes batchType, ICompositionScopedBatch** result));
    MOCK_METHOD1(CreateSpriteVisual                      , HRESULT(ISpriteVisual** result));
    MOCK_METHOD1(CreateSurfaceBrush                      , HRESULT(ICompositionSurfaceBrush** result));
    MOCK_METHOD2(CreateSurfaceBrushWithSurface           , HRESULT(ICompositionSurface* surface, ICompositionSurfaceBrush** result));
#ifndef WINUI3
    MOCK_METHOD1(CreateTargetForCurrentView              , HRESULT(ABI::Microsoft::UI::Composition::ICompositionTarget** result));
#endif
    MOCK_METHOD1(CreateVector2KeyFrameAnimation          , HRESULT(IVector2KeyFrameAnimation** result));
    MOCK_METHOD1(CreateVector3KeyFrameAnimation          , HRESULT(IVector3KeyFrameAnimation** result));
    MOCK_METHOD1(CreateVector4KeyFrameAnimation          , HRESULT(IVector4KeyFrameAnimation** result));
    MOCK_METHOD2(GetCommitBatch                          , HRESULT(CompositionBatchTypes batchType, ICompositionCommitBatch** result));

    // ICompositorInterop
    MOCK_METHOD2(CreateCompositionSurfaceForHandle    , HRESULT(HANDLE hDxgiSwapChain, ICompositionSurface** value));
    MOCK_METHOD2(CreateCompositionSurfaceForSwapChain , HRESULT(IUnknown* pDxgiSwapChain, ICompositionSurface** value));
    MOCK_METHOD2(CreateGraphicsDevice                 , HRESULT(IUnknown* pRenderingDevice, ICompositionGraphicsDevice** value));
};

#endif