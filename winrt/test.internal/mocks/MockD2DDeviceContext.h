// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "MockD2DBitmap.h"
#include "MockD2DEffect.h"

namespace canvas
{
    class MockD2DSolidColorBrush;

    class MockD2DDeviceContext : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
            ID2D1DeviceContext5,
            ID2D1DeviceContext4,
            ID2D1DeviceContext3,
            ID2D1DeviceContext2,
#endif
            ID2D1DeviceContext1, 
            ID2D1DeviceContext, 
            ID2D1RenderTarget, 
            ID2D1Resource>>
    {
    public:
        // ID2D1DeviceContext / ID2D1RenderTarget
        
        MOCK_METHOD1(Clear                            , void(D2D1_COLOR_F const*));
        MOCK_METHOD2(Flush                            , HRESULT(D2D1_TAG*, D2D1_TAG*));
        MOCK_METHOD1_CONST(GetTransform               , void(D2D1_MATRIX_3X2_F*));
        MOCK_METHOD1(SetTransform                     , void(D2D1_MATRIX_3X2_F const*));
        MOCK_METHOD0_CONST(GetAntialiasMode           , D2D1_ANTIALIAS_MODE());
        MOCK_METHOD1(SetAntialiasMode                 , void(D2D1_ANTIALIAS_MODE));
        MOCK_METHOD0_CONST(GetPrimitiveBlend          , D2D1_PRIMITIVE_BLEND());
        MOCK_METHOD1(SetPrimitiveBlend                , void(D2D1_PRIMITIVE_BLEND));
        MOCK_METHOD0_CONST(GetTextAntialiasMode       , D2D1_TEXT_ANTIALIAS_MODE());
        MOCK_METHOD1(SetTextAntialiasMode             , void(D2D1_TEXT_ANTIALIAS_MODE));
        MOCK_METHOD0_CONST(GetUnitMode                , D2D1_UNIT_MODE());
        MOCK_METHOD1(SetUnitMode                      , void(D2D1_UNIT_MODE));
        MOCK_METHOD2(SetDpi                           , void(float dpiX, float dpiY));
        MOCK_METHOD2_CONST(GetDpi                     , void(float* dpiX, float* dpiY));
        MOCK_METHOD5(DrawLine                         , void(D2D1_POINT_2F,D2D1_POINT_2F,ID2D1Brush*,float,ID2D1StrokeStyle*));
        MOCK_METHOD4(DrawRectangle                    , void(D2D1_RECT_F const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        MOCK_METHOD2(FillRectangle                    , void(D2D1_RECT_F const*,ID2D1Brush*));
        MOCK_METHOD4(DrawRoundedRectangle             , void(D2D1_ROUNDED_RECT const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        MOCK_METHOD2(FillRoundedRectangle             , void(D2D1_ROUNDED_RECT const*,ID2D1Brush*));
        MOCK_METHOD4(DrawEllipse                      , void(D2D1_ELLIPSE const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        MOCK_METHOD2(FillEllipse                      , void(D2D1_ELLIPSE const*,ID2D1Brush*));
        MOCK_METHOD7(DrawText                         , void(wchar_t const*,uint32_t,IDWriteTextFormat*,D2D1_RECT_F const*,ID2D1Brush*,D2D1_DRAW_TEXT_OPTIONS,DWRITE_MEASURING_MODE));
        MOCK_METHOD5(DrawImage                        , void(ID2D1Image*, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE));
        MOCK_METHOD6(DrawBitmap                       , void(ID2D1Bitmap*, D2D1_RECT_F const*, FLOAT, D2D1_INTERPOLATION_MODE, D2D1_RECT_F const*, D2D1_MATRIX_4X4_F const*));
        MOCK_METHOD1_CONST(GetDevice                  , void(ID2D1Device**));
        MOCK_METHOD2(CreateEffect                     , HRESULT(IID const&, ID2D1Effect **));
        MOCK_METHOD1(CreateCommandList                , HRESULT(ID2D1CommandList**));
        MOCK_METHOD3(CreateSolidColorBrush            , HRESULT(D2D1_COLOR_F const*, D2D1_BRUSH_PROPERTIES const*, ID2D1SolidColorBrush**));
        MOCK_METHOD2_CONST(GetImageLocalBounds        , HRESULT(ID2D1Image*, D2D1_RECT_F*));
        MOCK_METHOD2_CONST(GetImageWorldBounds        , HRESULT(ID2D1Image*, D2D1_RECT_F*));
        MOCK_METHOD0_CONST(GetMaximumBitmapSize       , UINT32());
        MOCK_METHOD5(CreateBitmap                     , HRESULT(D2D1_SIZE_U, void const*, UINT32, D2D1_BITMAP_PROPERTIES1 const*, ID2D1Bitmap1**));
        MOCK_METHOD3(CreateBitmapFromDxgiSurface      , HRESULT(IDXGISurface*, const D2D1_BITMAP_PROPERTIES1*, ID2D1Bitmap1**));
        MOCK_METHOD1(SetTarget                        , void(ID2D1Image*));
        MOCK_METHOD1_CONST(GetTarget                  , void(ID2D1Image**));
        MOCK_METHOD0(BeginDraw                        , void());
        MOCK_METHOD2(EndDraw                          , HRESULT(D2D1_TAG*, D2D1_TAG*));
        MOCK_METHOD4(DrawGeometry                     , void(ID2D1Geometry*, ID2D1Brush*,float,ID2D1StrokeStyle*));
        MOCK_METHOD3(FillGeometry                     , void(ID2D1Geometry*,ID2D1Brush*,ID2D1Brush*));
        MOCK_METHOD4(DrawTextLayout                   , void(D2D1_POINT_2F, IDWriteTextLayout*, ID2D1Brush*, D2D1_DRAW_TEXT_OPTIONS));
        MOCK_METHOD2(PushLayer                        , void(const D2D1_LAYER_PARAMETERS1*, ID2D1Layer*));
        MOCK_METHOD0(PopLayer                         , void());
        MOCK_METHOD2(PushAxisAlignedClip              , void(D2D1_RECT_F const*, D2D1_ANTIALIAS_MODE));
        MOCK_METHOD0(PopAxisAlignedClip               , void());
        MOCK_METHOD4(FillOpacityMask                  , void(ID2D1Bitmap*, ID2D1Brush*, D2D1_RECT_F const*, D2D1_RECT_F const*));
        MOCK_METHOD1(SetTextRenderingParams           , void(IDWriteRenderingParams*));
        MOCK_METHOD1_CONST(GetTextRenderingParams     , void(IDWriteRenderingParams**));
        MOCK_METHOD4(CreateLinearGradientBrush        , HRESULT(D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES const*, D2D1_BRUSH_PROPERTIES const*, ID2D1GradientStopCollection*, ID2D1LinearGradientBrush**));
        MOCK_METHOD4(CreateRadialGradientBrush        , HRESULT(D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES const*, D2D1_BRUSH_PROPERTIES const*, ID2D1GradientStopCollection*, ID2D1RadialGradientBrush**));
        MOCK_METHOD1_CONST(IsDxgiFormatSupported      , BOOL(DXGI_FORMAT));
        MOCK_METHOD1_CONST(IsBufferPrecisionSupported , BOOL(D2D1_BUFFER_PRECISION));
        MOCK_METHOD1(SetRenderingControls             , void(D2D1_RENDERING_CONTROLS const*));
        MOCK_METHOD1_CONST(GetRenderingControls       , void(D2D1_RENDERING_CONTROLS*));
        MOCK_METHOD4_CONST(GetGlyphRunWorldBounds     , HRESULT(D2D1_POINT_2F, const DWRITE_GLYPH_RUN*, DWRITE_MEASURING_MODE, D2D1_RECT_F*));
        MOCK_METHOD3(InvalidateEffectInputRectangle   , HRESULT(ID2D1Effect*, UINT32, D2D1_RECT_F const*));
        MOCK_METHOD2(GetEffectInvalidRectangleCount   , HRESULT(ID2D1Effect*, UINT32*));
        MOCK_METHOD3(GetEffectInvalidRectangles       , HRESULT(ID2D1Effect*, D2D1_RECT_F*, UINT32));
        MOCK_METHOD5(GetEffectRequiredInputRectangles , HRESULT(ID2D1Effect*, D2D1_RECT_F const*, D2D1_EFFECT_INPUT_DESCRIPTION const*, D2D1_RECT_F*, UINT32));
        MOCK_METHOD1_CONST(SaveDrawingState           , void(ID2D1DrawingStateBlock*));
        MOCK_METHOD1(RestoreDrawingState              , void(ID2D1DrawingStateBlock*));


        // ID2D1DeviceContext1
        
        MOCK_METHOD3(CreateFilledGeometryRealization  , HRESULT(ID2D1Geometry*, FLOAT, ID2D1GeometryRealization**));
        MOCK_METHOD5(CreateStrokedGeometryRealization , HRESULT(ID2D1Geometry*, FLOAT, FLOAT, ID2D1StrokeStyle *, ID2D1GeometryRealization**));
        MOCK_METHOD2(DrawGeometryRealization          , void(ID2D1GeometryRealization*, ID2D1Brush*));
        MOCK_METHOD8(CreateGradientStopCollection     , HRESULT(D2D1_GRADIENT_STOP const*, uint32_t, D2D1_COLOR_SPACE, D2D1_COLOR_SPACE, D2D1_BUFFER_PRECISION, D2D1_EXTEND_MODE, D2D1_COLOR_INTERPOLATION_MODE, ID2D1GradientStopCollection1**));


#if WINVER > _WIN32_WINNT_WINBLUE
        // ID2D1DeviceContext2

        MOCK_METHOD2(CreateInk                        , HRESULT(CONST D2D1_INK_POINT*, ID2D1Ink**));
        MOCK_METHOD2(CreateInkStyle                   , HRESULT(CONST D2D1_INK_STYLE_PROPERTIES*, ID2D1InkStyle**));
        MOCK_METHOD3(CreateGradientMesh               , HRESULT(CONST D2D1_GRADIENT_MESH_PATCH*, UINT32, ID2D1GradientMesh**));
        MOCK_METHOD4(CreateImageSourceFromWic         , HRESULT(IWICBitmapSource*, D2D1_IMAGE_SOURCE_LOADING_OPTIONS, D2D1_ALPHA_MODE, ID2D1ImageSourceFromWic**)); 
        MOCK_METHOD6(CreateLookupTable3D              , HRESULT(D2D1_BUFFER_PRECISION, CONST UINT32*, CONST BYTE*, UINT32, CONST UINT32*, ID2D1LookupTable3D**));
        MOCK_METHOD5(CreateImageSourceFromDxgi        , HRESULT(IDXGISurface**, UINT32, DXGI_COLOR_SPACE_TYPE, D2D1_IMAGE_SOURCE_FROM_DXGI_OPTIONS, ID2D1ImageSource**));
        MOCK_METHOD2_CONST(GetGradientMeshWorldBounds , HRESULT(ID2D1GradientMesh*, D2D1_RECT_F*));
        MOCK_METHOD3(DrawInk                          , void(ID2D1Ink*, ID2D1Brush*, ID2D1InkStyle*));
        MOCK_METHOD1(DrawGradientMesh                 , void(ID2D1GradientMesh*)); 
        MOCK_METHOD3(DrawGdiMetafile                  , void(ID2D1GdiMetafile*, CONST D2D1_RECT_F*, CONST D2D1_RECT_F*));
        MOCK_METHOD3(CreateTransformedImageSource     , HRESULT(ID2D1ImageSource*, CONST D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES*, ID2D1TransformedImageSource**));

        // ID2D1DeviceContext3
        
        MOCK_METHOD1(CreateSpriteBatch , HRESULT(ID2D1SpriteBatch**));
        MOCK_METHOD6(DrawSpriteBatch   , void(ID2D1SpriteBatch*, UINT32, UINT32, ID2D1Bitmap*, D2D1_BITMAP_INTERPOLATION_MODE, D2D1_SPRITE_OPTIONS));

        // ID2D1DeviceContext5
        MOCK_METHOD1(DrawSvgDocument, void(ID2D1SvgDocument*));
        MOCK_METHOD3(CreateSvgDocument, HRESULT(IStream*, D2D1_SIZE_F, ID2D1SvgDocument**));
        
#endif

        MockD2DDeviceContext()
        {
            CreateSolidColorBrushMethod.AllowAnyCall(
                [](D2D1_COLOR_F const*, D2D1_BRUSH_PROPERTIES const*, ID2D1SolidColorBrush** theValue)
                {
                    ComPtr<MockD2DSolidColorBrush> brush = Make<MockD2DSolidColorBrush>();
                    return brush.CopyTo(theValue);
                });
        }

        // ID2D1Resource

        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **) const override
        {
            Assert::Fail(L"Unexpected call to GetFactory");
        }

        // ID2D1RenderTarget

        IFACEMETHODIMP CreateBitmap(D2D1_SIZE_U,const void *,UINT32,const D2D1_BITMAP_PROPERTIES *,ID2D1Bitmap **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmap");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateBitmapFromWicBitmap(IWICBitmapSource *,const D2D1_BITMAP_PROPERTIES *,ID2D1Bitmap ** bitmap) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmapFromWicBitmap");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateSharedBitmap(IID const&,void *,const D2D1_BITMAP_PROPERTIES *,ID2D1Bitmap **) override
        {
            Assert::Fail(L"Unexpected call to CreateSharedBitmap");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateBitmapBrush(ID2D1Bitmap *,const D2D1_BITMAP_BRUSH_PROPERTIES *,const D2D1_BRUSH_PROPERTIES *,ID2D1BitmapBrush **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmapBrush");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateGradientStopCollection(const D2D1_GRADIENT_STOP *, uint32_t, D2D1_GAMMA, D2D1_EXTEND_MODE, ID2D1GradientStopCollection **) override
        {
            // This method is a deprecated version and should not be called. 
            // The version which returns ID2D1GradientStopCollection1 should be called instead.
            Assert::Fail(L"Unexpected call to CreateGradientStopCollection");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCompatibleRenderTarget(const D2D1_SIZE_F *,const D2D1_SIZE_U *,const D2D1_PIXEL_FORMAT *,D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS,ID2D1BitmapRenderTarget **) override
        {
            Assert::Fail(L"Unexpected call to CreateCompatibleRenderTarget");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateLayer(const D2D1_SIZE_F *,ID2D1Layer **) override
        {
            Assert::Fail(L"Unexpected call to CreateLayer");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateMesh(ID2D1Mesh **) override
        {
            Assert::Fail(L"Unexpected call to CreateMesh");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) FillMesh(ID2D1Mesh *,ID2D1Brush *) override
        {
            Assert::Fail(L"Unexpected call to FillMesh");
        }

        IFACEMETHODIMP_(void) FillOpacityMask(ID2D1Bitmap *,ID2D1Brush *,D2D1_OPACITY_MASK_CONTENT,const D2D1_RECT_F *,const D2D1_RECT_F *) override
        {
            Assert::Fail(L"Unexpected call to FillOpacityMask");
        }

        IFACEMETHODIMP_(void) DrawBitmap(ID2D1Bitmap *,const D2D1_RECT_F *,FLOAT,D2D1_BITMAP_INTERPOLATION_MODE,const D2D1_RECT_F *) override
        {
            // This is the v1 overload. Prefer the other DrawBitmap overload because it supports richer interpolation mode options.
            Assert::Fail(L"Unexpected call to DrawBitmap");
        }

        IFACEMETHODIMP_(void) DrawGlyphRun(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,ID2D1Brush *,DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawGlyphRun");
        }


        IFACEMETHODIMP_(void) SetTags(D2D1_TAG,D2D1_TAG) override
        {
            Assert::Fail(L"Unexpected call to SetTags");
        }

        IFACEMETHODIMP_(void) GetTags(D2D1_TAG *,D2D1_TAG *) const override
        {
            Assert::Fail(L"Unexpected call to GetTags");
        }

        IFACEMETHODIMP_(void) PushLayer(const D2D1_LAYER_PARAMETERS *,ID2D1Layer *) override
        {
            Assert::Fail(L"Unexpected call to PushLayer");
        }

        IFACEMETHODIMP_(D2D1_PIXEL_FORMAT) GetPixelFormat() const override
        {
            Assert::Fail(L"Unexpected call to GetPixelFormat");
            return D2D1::PixelFormat();
        }

        IFACEMETHODIMP_(D2D1_SIZE_F) GetSize() const override
        {
            Assert::Fail(L"Unexpected call to GetSize");
            return D2D1_SIZE_F{ 0.0f, 0.0f };
        }

        IFACEMETHODIMP_(D2D1_SIZE_U) GetPixelSize() const override
        {
            Assert::Fail(L"Unexpected call to GetPixelSize");
            return D2D1_SIZE_U{ 0U, 0U };
        }

        IFACEMETHODIMP_(BOOL) IsSupported(const D2D1_RENDER_TARGET_PROPERTIES *) const override
        {
            Assert::Fail(L"Unexpected call to IsSupported");
            return FALSE;
        }

        // ID2D1DeviceContext

        IFACEMETHODIMP CreateBitmapFromWicBitmap(IWICBitmapSource *,const D2D1_BITMAP_PROPERTIES1 *,ID2D1Bitmap1 **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmapFromWicBitmap");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateColorContext(D2D1_COLOR_SPACE,const BYTE *,UINT32,ID2D1ColorContext **) override
        {
            Assert::Fail(L"Unexpected call to CreateColorContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateColorContextFromFilename(PCWSTR,ID2D1ColorContext **) override
        {
            Assert::Fail(L"Unexpected call to CreateColorContextFromFilename");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateColorContextFromWicColorContext(IWICColorContext *,ID2D1ColorContext **) override
        {
            Assert::Fail(L"Unexpected call to CreateColorContextFromWicColorContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateImageBrush(ID2D1Image *,const D2D1_IMAGE_BRUSH_PROPERTIES *,const D2D1_BRUSH_PROPERTIES *,ID2D1ImageBrush **) override
        {
            Assert::Fail(L"Unexpected call to CreateImageBrush");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateBitmapBrush(ID2D1Bitmap *,const D2D1_BITMAP_BRUSH_PROPERTIES1 *,const D2D1_BRUSH_PROPERTIES *,ID2D1BitmapBrush1 **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmapBrush");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) DrawGlyphRun(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,const DWRITE_GLYPH_RUN_DESCRIPTION *,ID2D1Brush *,DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawGlyphRun");
        }

        IFACEMETHODIMP_(void) DrawGdiMetafile(ID2D1GdiMetafile *,const D2D1_POINT_2F *) override
        {
            Assert::Fail(L"Unexpected call to DrawGdiMetafile");
        }

#if WINVER > _WIN32_WINNT_WINBLUE

        // ID2D1DeviceContext4

        IFACEMETHODIMP CreateSvgGlyphStyle(ID2D1SvgGlyphStyle**) override
        {
            Assert::Fail(L"Unexpected call to CreateSvgGlyphStyle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) DrawText(
            CONST WCHAR*,
            UINT32,
            _In_ IDWriteTextFormat *,
            _In_ CONST D2D1_RECT_F *,
            _In_opt_ ID2D1Brush *,
            _In_opt_ ID2D1SvgGlyphStyle *,
            UINT32,
            D2D1_DRAW_TEXT_OPTIONS,
            DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawText");
        }

        IFACEMETHODIMP_(void) DrawTextLayout(
            D2D1_POINT_2F origin,
            _In_ IDWriteTextLayout *,
            _In_opt_ ID2D1Brush *,
            _In_opt_ ID2D1SvgGlyphStyle *,
            UINT32,
            D2D1_DRAW_TEXT_OPTIONS) override
        {
            Assert::Fail(L"Unexpected call to DrawTextLayout");
        }

        IFACEMETHODIMP_(void) DrawColorBitmapGlyphRun(
            DWRITE_GLYPH_IMAGE_FORMATS,
            D2D1_POINT_2F,
            CONST DWRITE_GLYPH_RUN*,
            DWRITE_MEASURING_MODE,
            D2D1_COLOR_BITMAP_GLYPH_SNAP_OPTION) override
        {
            Assert::Fail(L"Unexpected call to DrawColorBitmapGlyphRun");
        }

        IFACEMETHODIMP_(void) DrawSvgGlyphRun(
            D2D1_POINT_2F,
            _In_ CONST DWRITE_GLYPH_RUN *,
            _In_opt_ ID2D1Brush*,
            _In_opt_ ID2D1SvgGlyphStyle *,
            UINT32,
            DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawSvgGlyphRun");
        }

        IFACEMETHODIMP GetColorBitmapGlyphImage(
            DWRITE_GLYPH_IMAGE_FORMATS,
            D2D1_POINT_2F,
            IDWriteFontFace *,
            FLOAT,
            UINT16,
            BOOL,
            CONST D2D1_MATRIX_3X2_F *,
            FLOAT,
            FLOAT,
            D2D1_MATRIX_3X2_F *,
            ID2D1Image **) override
        {
            Assert::Fail(L"Unexpected call to GetColorBitmapGlyphImage");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetSvgGlyphImage(
            D2D1_POINT_2F,
            IDWriteFontFace *,
            FLOAT,
            UINT16,
            BOOL,
            CONST D2D1_MATRIX_3X2_F *,
            ID2D1Brush *,
            ID2D1SvgGlyphStyle *,
            UINT32,
            D2D1_MATRIX_3X2_F *,
            ID2D1CommandList**) override
        {
            Assert::Fail(L"Unexpected call to GetSvgGlyphImage");
            return E_NOTIMPL;
        }
        
        // ID2D1DeviceContext5
        
        IFACEMETHODIMP CreateColorContextFromDxgiColorSpace(
            DXGI_COLOR_SPACE_TYPE,
            ID2D1ColorContext1**) override
        {
            Assert::Fail(L"Unexpected call to CreateColorContextFromDxgiColorSpace");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateColorContextFromSimpleColorProfile(
            CONST D2D1_SIMPLE_COLOR_PROFILE*,
            ID2D1ColorContext1**) override
        {
            Assert::Fail(L"Unexpected call to CreateColorContextFromSimpleColorProfile");
            return E_NOTIMPL;
        }
#endif
    };
}
