// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI;

#if WINVER > _WIN32_WINNT_WINBLUE
    using namespace ABI::Windows::UI::Input::Inking;
#endif

    class MockCanvasDrawingSession : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasDrawingSession,
        IClosable,
        CloakedIid<ICanvasResourceWrapperNative>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(CloseMethod, HRESULT());

        MockCanvasDrawingSession()
        {
            CloseMethod.AllowAnyCall();
        }

        IFACEMETHODIMP Close() override
        {
            return CloseMethod.WasCalled();
        }

#define DONT_EXPECT(name, ...)                                  \
        IFACEMETHODIMP name(__VA_ARGS__) override               \
        {                                                       \
            Assert::Fail(L"Unexpected call to " WIDEN(#name));  \
            return E_NOTIMPL;                                   \
        }

        DONT_EXPECT(Clear , Color);

        DONT_EXPECT(DrawImageAtOrigin                                                       , ICanvasImage*);
        DONT_EXPECT(DrawImageAtOffset                                                       , ICanvasImage*, Vector2);
        DONT_EXPECT(DrawImageAtCoords                                                       , ICanvasImage*, float, float);
        DONT_EXPECT(DrawImageToRect                                                         , ICanvasBitmap*, Rect);
        DONT_EXPECT(DrawImageAtOffsetWithSourceRect                                         , ICanvasImage*, Vector2, Rect);
        DONT_EXPECT(DrawImageAtCoordsWithSourceRect                                         , ICanvasImage*, float, float, Rect);
        DONT_EXPECT(DrawImageToRectWithSourceRect                                           , ICanvasImage*, Rect, Rect);
        DONT_EXPECT(DrawImageAtOffsetWithSourceRectAndOpacity                               , ICanvasImage*, Vector2, Rect, float);
        DONT_EXPECT(DrawImageAtCoordsWithSourceRectAndOpacity                               , ICanvasImage*, float, float, Rect, float);
        DONT_EXPECT(DrawImageToRectWithSourceRectAndOpacity                                 , ICanvasImage*, Rect, Rect, float);
        DONT_EXPECT(DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolation               , ICanvasImage*, Vector2, Rect, float, CanvasImageInterpolation);
        DONT_EXPECT(DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolation               , ICanvasImage*, float, float, Rect, float, CanvasImageInterpolation);
        DONT_EXPECT(DrawImageToRectWithSourceRectAndOpacityAndInterpolation                 , ICanvasImage*, Rect, Rect, float, CanvasImageInterpolation);
        DONT_EXPECT(DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolationAndComposite   , ICanvasImage*, Vector2, Rect, float, CanvasImageInterpolation, CanvasComposite);
        DONT_EXPECT(DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolationAndComposite   , ICanvasImage*, float, float, Rect, float, CanvasImageInterpolation, CanvasComposite);
        DONT_EXPECT(DrawImageToRectWithSourceRectAndOpacityAndInterpolationAndComposite     , ICanvasImage*, Rect, Rect, float, CanvasImageInterpolation, CanvasComposite);
        DONT_EXPECT(DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolationAndPerspective , ICanvasBitmap*, Vector2, Rect, float, CanvasImageInterpolation, Matrix4x4);
        DONT_EXPECT(DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolationAndPerspective , ICanvasBitmap*, float, float, Rect, float, CanvasImageInterpolation, Matrix4x4);
        DONT_EXPECT(DrawImageToRectWithSourceRectAndOpacityAndInterpolationAndPerspective   , ICanvasBitmap*, Rect, Rect, float, CanvasImageInterpolation, Matrix4x4);

        DONT_EXPECT(DrawLineWithBrush                                     , Vector2, Vector2, ICanvasBrush*);
        DONT_EXPECT(DrawLineAtCoordsWithBrush                             , float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawLineWithColor                                     , Vector2, Vector2, Color);
        DONT_EXPECT(DrawLineAtCoordsWithColor                             , float, float, float, float, Color);
        DONT_EXPECT(DrawLineWithBrushAndStrokeWidth                       , Vector2, Vector2, ICanvasBrush*, float);
        DONT_EXPECT(DrawLineAtCoordsWithBrushAndStrokeWidth               , float, float, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawLineWithColorAndStrokeWidth                       , Vector2, Vector2, Color, float);
        DONT_EXPECT(DrawLineAtCoordsWithColorAndStrokeWidth               , float, float, float, float, Color, float);
        DONT_EXPECT(DrawLineWithBrushAndStrokeWidthAndStrokeStyle         , Vector2, Vector2, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle , float, float, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawLineWithColorAndStrokeWidthAndStrokeStyle         , Vector2, Vector2, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle , float, float, float, float, Color, float, ICanvasStrokeStyle*);

        DONT_EXPECT(DrawRectangleWithBrush                                     , Rect, ICanvasBrush*);
        DONT_EXPECT(DrawRectangleAtCoordsWithBrush                             , float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawRectangleWithColor                                     , Rect, Color);
        DONT_EXPECT(DrawRectangleAtCoordsWithColor                             , float, float, float, float, Color);
        DONT_EXPECT(DrawRectangleWithBrushAndStrokeWidth                       , Rect, ICanvasBrush*, float);
        DONT_EXPECT(DrawRectangleAtCoordsWithBrushAndStrokeWidth               , float, float, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawRectangleWithColorAndStrokeWidth                       , Rect, Color, float);
        DONT_EXPECT(DrawRectangleAtCoordsWithColorAndStrokeWidth               , float, float, float, float, Color, float);
        DONT_EXPECT(DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle         , Rect, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle , float, float, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRectangleWithColorAndStrokeWidthAndStrokeStyle         , Rect, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle , float, float, float, float, Color, float, ICanvasStrokeStyle*);

        DONT_EXPECT(FillRectangleWithBrush                       , Rect, ICanvasBrush*);
        DONT_EXPECT(FillRectangleAtCoordsWithBrush               , float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(FillRectangleWithColor                       , Rect, Color);
        DONT_EXPECT(FillRectangleAtCoordsWithColor               , float, float, float, float, Color);
        DONT_EXPECT(FillRectangleWithBrushAndOpacityBrush        , Rect, ICanvasBrush*, ICanvasBrush*);
        DONT_EXPECT(FillRectangleAtCoordsWithBrushAndOpacityBrush, float, float, float, float, ICanvasBrush*, ICanvasBrush*);

        DONT_EXPECT(DrawRoundedRectangleWithBrush                                     , Rect, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithBrush                             , float, float, float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawRoundedRectangleWithColor                                     , Rect, float, float, Color);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithColor                             , float, float, float, float, float, float, Color);
        DONT_EXPECT(DrawRoundedRectangleWithBrushAndStrokeWidth                       , Rect, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth               , float, float, float, float, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawRoundedRectangleWithColorAndStrokeWidth                       , Rect, float, float, Color, float);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth               , float, float, float, float, float, float, Color, float);
        DONT_EXPECT(DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle         , Rect, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle , float, float, float, float, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle         , Rect, float, float, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle , float, float, float, float, float, float, Color, float, ICanvasStrokeStyle*);
        
        DONT_EXPECT(FillRoundedRectangleWithBrush         , Rect, float, float, ICanvasBrush*);
        DONT_EXPECT(FillRoundedRectangleAtCoordsWithBrush , float, float, float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(FillRoundedRectangleWithColor         , Rect, float, float, Color);
        DONT_EXPECT(FillRoundedRectangleAtCoordsWithColor , float, float, float, float, float, float, Color);
        
        DONT_EXPECT(DrawEllipseWithBrush                                     , Vector2, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawEllipseAtCoordsWithBrush                             , float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawEllipseWithColor                                     , Vector2, float, float, Color);
        DONT_EXPECT(DrawEllipseAtCoordsWithColor                             , float, float, float, float, Color);
        DONT_EXPECT(DrawEllipseWithBrushAndStrokeWidth                       , Vector2, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawEllipseAtCoordsWithBrushAndStrokeWidth               , float, float, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawEllipseWithColorAndStrokeWidth                       , Vector2, float, float, Color, float);
        DONT_EXPECT(DrawEllipseAtCoordsWithColorAndStrokeWidth               , float, float, float, float, Color, float);
        DONT_EXPECT(DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle         , Vector2, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle , float, float, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawEllipseWithColorAndStrokeWidthAndStrokeStyle         , Vector2, float, float, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle , float, float, float, float, Color, float, ICanvasStrokeStyle*);

        DONT_EXPECT(FillEllipseWithBrush         , Vector2, float, float, ICanvasBrush*);
        DONT_EXPECT(FillEllipseAtCoordsWithBrush , float, float, float, float, ICanvasBrush*);
        DONT_EXPECT(FillEllipseWithColor         , Vector2, float, float, Color);
        DONT_EXPECT(FillEllipseAtCoordsWithColor , float, float, float, float, Color);
        
        DONT_EXPECT(DrawCircleWithBrush                                     , Vector2, float, ICanvasBrush*);
        DONT_EXPECT(DrawCircleAtCoordsWithBrush                             , float, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawCircleWithColor                                     , Vector2, float, Color);
        DONT_EXPECT(DrawCircleAtCoordsWithColor                             , float, float, float, Color);
        DONT_EXPECT(DrawCircleWithBrushAndStrokeWidth                       , Vector2, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawCircleAtCoordsWithBrushAndStrokeWidth               , float, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawCircleWithColorAndStrokeWidth                       , Vector2, float, Color, float);
        DONT_EXPECT(DrawCircleAtCoordsWithColorAndStrokeWidth               , float, float, float, Color, float);
        DONT_EXPECT(DrawCircleWithBrushAndStrokeWidthAndStrokeStyle         , Vector2, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle , float, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawCircleWithColorAndStrokeWidthAndStrokeStyle         , Vector2, float, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle , float, float, float, Color, float, ICanvasStrokeStyle*);

        DONT_EXPECT(FillCircleWithBrush         , Vector2, float, ICanvasBrush*);
        DONT_EXPECT(FillCircleAtCoordsWithBrush , float, float, float, ICanvasBrush*);
        DONT_EXPECT(FillCircleWithColor         , Vector2, float, Color);
        DONT_EXPECT(FillCircleAtCoordsWithColor , float, float, float, Color);

        DONT_EXPECT(DrawTextAtPointWithColor                , HSTRING, Vector2, Color);
        DONT_EXPECT(DrawTextAtPointCoordsWithColor          , HSTRING, float, float, Color);
        DONT_EXPECT(DrawTextAtPointWithBrushAndFormat       , HSTRING, Vector2, ICanvasBrush*, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtRectWithBrushAndFormat        , HSTRING, Rect, ICanvasBrush*, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtPointCoordsWithBrushAndFormat , HSTRING, float, float, ICanvasBrush*, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtRectCoordsWithBrushAndFormat  , HSTRING, float, float, float, float, ICanvasBrush*, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtPointWithColorAndFormat       , HSTRING, Vector2, Color, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtRectWithColorAndFormat        , HSTRING, Rect, Color, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtPointCoordsWithColorAndFormat , HSTRING, float, float, Color, ICanvasTextFormat*);
        DONT_EXPECT(DrawTextAtRectCoordsWithColorAndFormat  , HSTRING, float, float, float, float, Color, ICanvasTextFormat*);

        DONT_EXPECT(DrawGeometryWithBrush                                    , ICanvasGeometry*, Vector2, ICanvasBrush*);
        DONT_EXPECT(DrawGeometryWithColor                                    , ICanvasGeometry*, Vector2, Color);
        DONT_EXPECT(DrawGeometryAtCoordsWithBrush                            , ICanvasGeometry*, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawGeometryAtCoordsWithColor                            , ICanvasGeometry*, float, float, Color);
        DONT_EXPECT(DrawGeometryAtOriginWithBrush                            , ICanvasGeometry*, ICanvasBrush*);
        DONT_EXPECT(DrawGeometryAtOriginWithColor                            , ICanvasGeometry*, Color);
        DONT_EXPECT(DrawGeometryWithBrushAndStrokeWidth                      , ICanvasGeometry*, Vector2, ICanvasBrush*, float);
        DONT_EXPECT(DrawGeometryWithColorAndStrokeWidth                      , ICanvasGeometry*, Vector2, Color, float);
        DONT_EXPECT(DrawGeometryAtCoordsWithBrushAndStrokeWidth              , ICanvasGeometry*, float, float, ICanvasBrush*, float);
        DONT_EXPECT(DrawGeometryAtCoordsWithColorAndStrokeWidth              , ICanvasGeometry*, float, float, Color, float);
        DONT_EXPECT(DrawGeometryAtOriginWithBrushAndStrokeWidth              , ICanvasGeometry*, ICanvasBrush*, float);
        DONT_EXPECT(DrawGeometryAtOriginWithColorAndStrokeWidth              , ICanvasGeometry*, Color, float);
        DONT_EXPECT(DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle        , ICanvasGeometry*, Vector2, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawGeometryWithColorAndStrokeWidthAndStrokeStyle        , ICanvasGeometry*, Vector2, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawGeometryAtCoordsWithBrushAndStrokeWidthAndStrokeStyle, ICanvasGeometry*, float, float, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawGeometryAtCoordsWithColorAndStrokeWidthAndStrokeStyle, ICanvasGeometry*, float, float, Color, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawGeometryAtOriginWithBrushAndStrokeWidthAndStrokeStyle, ICanvasGeometry*, ICanvasBrush*, float, ICanvasStrokeStyle*);
        DONT_EXPECT(DrawGeometryAtOriginWithColorAndStrokeWidthAndStrokeStyle, ICanvasGeometry*, Color, float, ICanvasStrokeStyle*);

        DONT_EXPECT(FillGeometryWithBrush                       , ICanvasGeometry*, Vector2, ICanvasBrush*);
        DONT_EXPECT(FillGeometryWithBrushAndOpacityBrush        , ICanvasGeometry*, Vector2, ICanvasBrush*, ICanvasBrush*);
        DONT_EXPECT(FillGeometryWithColor                       , ICanvasGeometry*, Vector2, Color);
        DONT_EXPECT(FillGeometryAtCoordsWithBrush               , ICanvasGeometry*, float, float, ICanvasBrush*);
        DONT_EXPECT(FillGeometryAtCoordsWithBrushAndOpacityBrush, ICanvasGeometry*, float, float, ICanvasBrush*, ICanvasBrush*);
        DONT_EXPECT(FillGeometryAtCoordsWithColor               , ICanvasGeometry*, float, float, Color);
        DONT_EXPECT(FillGeometryAtOriginWithBrush               , ICanvasGeometry*, ICanvasBrush*);
        DONT_EXPECT(FillGeometryAtOriginWithBrushAndOpacityBrush, ICanvasGeometry*, ICanvasBrush*, ICanvasBrush*);
        DONT_EXPECT(FillGeometryAtOriginWithColor               , ICanvasGeometry*, Color);

        DONT_EXPECT(DrawCachedGeometryWithBrush        , ICanvasCachedGeometry*, Vector2, ICanvasBrush*);
        DONT_EXPECT(DrawCachedGeometryWithColor        , ICanvasCachedGeometry*, Vector2, Color);
        DONT_EXPECT(DrawCachedGeometryAtCoordsWithBrush, ICanvasCachedGeometry*, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawCachedGeometryAtCoordsWithColor, ICanvasCachedGeometry*, float, float, Color);
        DONT_EXPECT(DrawCachedGeometryAtOriginWithBrush, ICanvasCachedGeometry*, ICanvasBrush*);
        DONT_EXPECT(DrawCachedGeometryAtOriginWithColor, ICanvasCachedGeometry*, Color);

        DONT_EXPECT(DrawTextLayoutWithBrush, ICanvasTextLayout*, Vector2, ICanvasBrush*);
        DONT_EXPECT(DrawTextLayoutAtCoordsWithBrush, ICanvasTextLayout*, float, float, ICanvasBrush*);
        DONT_EXPECT(DrawTextLayoutWithColor, ICanvasTextLayout*, Vector2, Color);
        DONT_EXPECT(DrawTextLayoutAtCoordsWithColor, ICanvasTextLayout*, float, float, Color);

#if WINVER > _WIN32_WINNT_WINBLUE
        DONT_EXPECT(DrawInk, IIterable<InkStroke*>*);
        DONT_EXPECT(DrawInkWithHighContrast, IIterable<InkStroke*>*, boolean);

        DONT_EXPECT(DrawGradientMeshAtOrigin, ICanvasGradientMesh*);
        DONT_EXPECT(DrawGradientMesh, ICanvasGradientMesh*, Vector2);
        DONT_EXPECT(DrawGradientMeshAtCoords, ICanvasGradientMesh*, float, float);
#endif

		DONT_EXPECT(DrawGlyphRun, Vector2, ICanvasFontFace*, float, uint32_t, CanvasGlyph*, boolean, uint32_t, ICanvasBrush*);
		DONT_EXPECT(DrawGlyphRunWithMeasuringMode, Vector2, ICanvasFontFace*, float, uint32_t, CanvasGlyph*, boolean, uint32_t, ICanvasBrush*, CanvasTextMeasuringMode);
		DONT_EXPECT(DrawGlyphRunWithMeasuringModeAndDescription, Vector2, ICanvasFontFace*, float, uint32_t, CanvasGlyph*, boolean, uint32_t, ICanvasBrush*, CanvasTextMeasuringMode, HSTRING, HSTRING, uint32_t, int*, uint32_t);
	
        DONT_EXPECT(get_Antialiasing            , CanvasAntialiasing*);
        DONT_EXPECT(put_Antialiasing            , CanvasAntialiasing);
        DONT_EXPECT(get_Blend                   , CanvasBlend*);
        DONT_EXPECT(put_Blend                   , CanvasBlend);
        DONT_EXPECT(get_TextAntialiasing        , CanvasTextAntialiasing*);
        DONT_EXPECT(put_TextAntialiasing        , CanvasTextAntialiasing);
        DONT_EXPECT(get_TextRenderingParameters , ICanvasTextRenderingParameters**);
        DONT_EXPECT(put_TextRenderingParameters , ICanvasTextRenderingParameters*);
        DONT_EXPECT(get_Transform               , ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2*);
        DONT_EXPECT(put_Transform               , ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2);
        DONT_EXPECT(get_Units                   , CanvasUnits*);
        DONT_EXPECT(put_Units                   , CanvasUnits);

        DONT_EXPECT(CreateLayerWithOpacity                                , float, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityBrush                           , ICanvasBrush*, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityAndClipRectangle                , float, Rect, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityBrushAndClipRectangle           , ICanvasBrush*, Rect, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityAndClipGeometry                 , float, ICanvasGeometry*, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityBrushAndClipGeometry            , ICanvasBrush*, ICanvasGeometry*, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityAndClipGeometryAndTransform     , float, ICanvasGeometry*, Matrix3x2, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithOpacityBrushAndClipGeometryAndTransform, ICanvasBrush*, ICanvasGeometry*, Matrix3x2, ICanvasActiveLayer**);
        DONT_EXPECT(CreateLayerWithAllOptions                             , float, ICanvasBrush*, Rect, ICanvasGeometry*, Matrix3x2, CanvasLayerOptions, ICanvasActiveLayer**);

        // ICanvasResourceWrapperNative
        DONT_EXPECT(GetNativeResource, ICanvasDevice* device, float dpi, REFIID iid, void**);

#undef DONT_EXPECT

    };
}
