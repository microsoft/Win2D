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

#include "MockD2DBitmap.h"
#include "MockD2DEffect.h"

namespace canvas
{
    class MockD2DSolidColorBrush;

    class MockD2DDeviceContext : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1DeviceContext1, ID2D1DeviceContext, ID2D1RenderTarget, ID2D1Resource>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(ClearMethod                 , void(D2D1_COLOR_F const*));
        CALL_COUNTER_WITH_MOCK(GetTransformMethod          , void(D2D1_MATRIX_3X2_F*));
        CALL_COUNTER_WITH_MOCK(SetTransformMethod          , void(D2D1_MATRIX_3X2_F const*));
        CALL_COUNTER_WITH_MOCK(GetAntialiasModeMethod      , D2D1_ANTIALIAS_MODE());
        CALL_COUNTER_WITH_MOCK(SetAntialiasModeMethod      , void(D2D1_ANTIALIAS_MODE));
        CALL_COUNTER_WITH_MOCK(GetPrimitiveBlendMethod     , D2D1_PRIMITIVE_BLEND());
        CALL_COUNTER_WITH_MOCK(SetPrimitiveBlendMethod     , void(D2D1_PRIMITIVE_BLEND));
        CALL_COUNTER_WITH_MOCK(GetTextAntialiasModeMethod  , D2D1_TEXT_ANTIALIAS_MODE());
        CALL_COUNTER_WITH_MOCK(SetTextAntialiasModeMethod  , void(D2D1_TEXT_ANTIALIAS_MODE));
        CALL_COUNTER_WITH_MOCK(GetUnitModeMethod           , D2D1_UNIT_MODE());
        CALL_COUNTER_WITH_MOCK(SetUnitModeMethod           , void(D2D1_UNIT_MODE));
        CALL_COUNTER_WITH_MOCK(SetDpiMethod                , void(float dpiX, float dpiY));
        CALL_COUNTER_WITH_MOCK(GetDpiMethod                , void(float* dpiX, float* dpiY));
        CALL_COUNTER_WITH_MOCK(DrawLineMethod              , void(D2D1_POINT_2F,D2D1_POINT_2F,ID2D1Brush*,float,ID2D1StrokeStyle*));
        CALL_COUNTER_WITH_MOCK(DrawRectangleMethod         , void(D2D1_RECT_F const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        CALL_COUNTER_WITH_MOCK(FillRectangleMethod         , void(D2D1_RECT_F const*,ID2D1Brush*));
        CALL_COUNTER_WITH_MOCK(DrawRoundedRectangleMethod  , void(D2D1_ROUNDED_RECT const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        CALL_COUNTER_WITH_MOCK(FillRoundedRectangleMethod  , void(D2D1_ROUNDED_RECT const*,ID2D1Brush*));
        CALL_COUNTER_WITH_MOCK(DrawEllipseMethod           , void(D2D1_ELLIPSE const*,ID2D1Brush*,float,ID2D1StrokeStyle*));
        CALL_COUNTER_WITH_MOCK(FillEllipseMethod           , void(D2D1_ELLIPSE const*,ID2D1Brush*));
        CALL_COUNTER_WITH_MOCK(DrawTextMethod              , void(wchar_t const*,uint32_t,IDWriteTextFormat*,D2D1_RECT_F const*,ID2D1Brush*,D2D1_DRAW_TEXT_OPTIONS,DWRITE_MEASURING_MODE));
        CALL_COUNTER_WITH_MOCK(DrawImageMethod             , void(ID2D1Image*, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE));
        CALL_COUNTER_WITH_MOCK(DrawBitmapMethod            , void(ID2D1Bitmap*, D2D1_RECT_F const*, FLOAT, D2D1_INTERPOLATION_MODE, D2D1_RECT_F const*, D2D1_MATRIX_4X4_F const*));
        CALL_COUNTER_WITH_MOCK(GetDeviceMethod             , void(ID2D1Device**));
        CALL_COUNTER_WITH_MOCK(CreateEffectMethod          , HRESULT(IID const&, ID2D1Effect **));
        CALL_COUNTER_WITH_MOCK(CreateCommandListMethod     , HRESULT(ID2D1CommandList**));
        CALL_COUNTER_WITH_MOCK(CreateSolidColorBrushMethod , HRESULT(D2D1_COLOR_F const*, D2D1_BRUSH_PROPERTIES const*, ID2D1SolidColorBrush**));
        CALL_COUNTER_WITH_MOCK(GetImageWorldBoundsMethod   , HRESULT(ID2D1Image*, D2D1_RECT_F*));
        CALL_COUNTER_WITH_MOCK(GetMaximumBitmapSizeMethod  , UINT32());
        CALL_COUNTER_WITH_MOCK(CreateBitmapMethod          , HRESULT(D2D1_SIZE_U, void const*, UINT32, D2D1_BITMAP_PROPERTIES1 const*, ID2D1Bitmap1**));
        CALL_COUNTER_WITH_MOCK(CreateBitmapFromDxgiSurfaceMethod, HRESULT(IDXGISurface*, const D2D1_BITMAP_PROPERTIES1*, ID2D1Bitmap1**));
        CALL_COUNTER_WITH_MOCK(SetTargetMethod             , void(ID2D1Image*));
        CALL_COUNTER_WITH_MOCK(GetTargetMethod             , void(ID2D1Image**));
        CALL_COUNTER_WITH_MOCK(BeginDrawMethod             , void());
        CALL_COUNTER_WITH_MOCK(EndDrawMethod               , HRESULT(D2D1_TAG*, D2D1_TAG*));
        CALL_COUNTER_WITH_MOCK(DrawGeometryMethod          , void(ID2D1Geometry*, ID2D1Brush*,float,ID2D1StrokeStyle*));
        CALL_COUNTER_WITH_MOCK(FillGeometryMethod          , void(ID2D1Geometry*,ID2D1Brush*,ID2D1Brush*));

        MockD2DDeviceContext()
        {
            CreateSolidColorBrushMethod.AllowAnyCall(
                [](D2D1_COLOR_F const*, D2D1_BRUSH_PROPERTIES const*, ID2D1SolidColorBrush** value)
                {
                    ComPtr<MockD2DSolidColorBrush> brush = Make<MockD2DSolidColorBrush>();
                    return brush.CopyTo(value);
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

        IFACEMETHODIMP CreateSolidColorBrush(
            D2D1_COLOR_F const* color,
            D2D1_BRUSH_PROPERTIES const* brushProperties,
            ID2D1SolidColorBrush** solidColorBrush
            ) override
        {
            return CreateSolidColorBrushMethod.WasCalled(color, brushProperties, solidColorBrush);
        }

        IFACEMETHODIMP CreateGradientStopCollection(const D2D1_GRADIENT_STOP *, uint32_t, D2D1_GAMMA, D2D1_EXTEND_MODE, ID2D1GradientStopCollection **) override
        {
            // This method is a deprecated version and should not be called. 
            // The version which returns ID2D1GradientStopCollection1 should be called instead.
            Assert::Fail(L"Unexpected call to CreateGradientStopCollection");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateLinearGradientBrush(const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES *,const D2D1_BRUSH_PROPERTIES *,ID2D1GradientStopCollection *,ID2D1LinearGradientBrush **) override
        {
            Assert::Fail(L"Unexpected call to CreateLinearGradientBrush");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateRadialGradientBrush(const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES *,const D2D1_BRUSH_PROPERTIES *,ID2D1GradientStopCollection *,ID2D1RadialGradientBrush **) override
        {
            Assert::Fail(L"Unexpected call to CreateRadialGradientBrush");
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

        IFACEMETHODIMP_(void) DrawLine(D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            DrawLineMethod.WasCalled(p0, p1, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) DrawRectangle(const D2D1_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            DrawRectangleMethod.WasCalled(rect, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillRectangle(const D2D1_RECT_F* rect, ID2D1Brush* brush) override
        {
            FillRectangleMethod.WasCalled(rect, brush);
        }

        IFACEMETHODIMP_(void) DrawRoundedRectangle(const D2D1_ROUNDED_RECT* roundedRect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            DrawRoundedRectangleMethod.WasCalled(roundedRect, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillRoundedRectangle(const D2D1_ROUNDED_RECT* roundedRect, ID2D1Brush* brush) override
        {
            FillRoundedRectangleMethod.WasCalled(roundedRect, brush);
        }

        IFACEMETHODIMP_(void) DrawEllipse(const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            DrawEllipseMethod.WasCalled(ellipse, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillEllipse(const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush) override
        {
            FillEllipseMethod.WasCalled(ellipse, brush);
        }

        IFACEMETHODIMP_(void) DrawGeometry(ID2D1Geometry* geometry, ID2D1Brush* brush, FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            DrawGeometryMethod.WasCalled(geometry, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillGeometry(ID2D1Geometry* geometry, ID2D1Brush* brush, ID2D1Brush* opacityBrush) override
        {
            FillGeometryMethod.WasCalled(geometry, brush, opacityBrush);
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

        IFACEMETHODIMP_(void) DrawText(const wchar_t* text, uint32_t textLength, IDWriteTextFormat* format, const D2D1_RECT_F* rect, ID2D1Brush* brush, D2D1_DRAW_TEXT_OPTIONS options, DWRITE_MEASURING_MODE measuringMode) override
        {
            DrawTextMethod.WasCalled(text, textLength, format, rect, brush, options, measuringMode);
        }

        IFACEMETHODIMP_(void) DrawTextLayout(D2D1_POINT_2F,IDWriteTextLayout *,ID2D1Brush *,D2D1_DRAW_TEXT_OPTIONS) override
        {
            Assert::Fail(L"Unexpected call to DrawTextLayout");
        }

        IFACEMETHODIMP_(void) DrawGlyphRun(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,ID2D1Brush *,DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawGlyphRun");
        }

        IFACEMETHODIMP_(void) DrawImage(_In_ ID2D1Image *image, _In_opt_ CONST D2D1_POINT_2F *targetOffset, _In_opt_ CONST D2D1_RECT_F *imageRectangle,
            D2D1_INTERPOLATION_MODE interpolationMode, D2D1_COMPOSITE_MODE compositeMode) override
        {
            DrawImageMethod.WasCalled(image, targetOffset, imageRectangle, interpolationMode, compositeMode);
        }

        IFACEMETHODIMP_(void) SetTransform(const D2D1_MATRIX_3X2_F* m) override
        {
            SetTransformMethod.WasCalled(m);
        }

        IFACEMETHODIMP_(void) GetTransform(D2D1_MATRIX_3X2_F *m) const override
        {
            GetTransformMethod.WasCalled(m);
        }

        IFACEMETHODIMP_(void) SetAntialiasMode(D2D1_ANTIALIAS_MODE m) override
        {
            SetAntialiasModeMethod.WasCalled(m);
        }

        IFACEMETHODIMP_(D2D1_ANTIALIAS_MODE) GetAntialiasMode() const override
        {
            return GetAntialiasModeMethod.WasCalled();
        }

        IFACEMETHODIMP_(void) SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE m) override
        {
            SetTextAntialiasModeMethod.WasCalled(m);
        }

        IFACEMETHODIMP_(D2D1_TEXT_ANTIALIAS_MODE) GetTextAntialiasMode() const override
        {
            return GetTextAntialiasModeMethod.WasCalled();
        }

        IFACEMETHODIMP_(void) SetTextRenderingParams(IDWriteRenderingParams *) override
        {
            Assert::Fail(L"Unexpected call to SetTextRenderingParams");
        }

        IFACEMETHODIMP_(void) GetTextRenderingParams(IDWriteRenderingParams **) const override
        {
            Assert::Fail(L"Unexpected call to GetTextRenderingParams");
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

        IFACEMETHODIMP_(void) PopLayer() override
        {
            Assert::Fail(L"Unexpected call to PopLayer");
        }

        IFACEMETHODIMP Flush(D2D1_TAG *,D2D1_TAG *) override
        {
            Assert::Fail(L"Unexpected call to Flush");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) SaveDrawingState(ID2D1DrawingStateBlock *) const override
        {
            Assert::Fail(L"Unexpected call to SaveDrawingState");
        }

        IFACEMETHODIMP_(void) RestoreDrawingState(ID2D1DrawingStateBlock *) override
        {
            Assert::Fail(L"Unexpected call to RestoreDrawingState");
        }

        IFACEMETHODIMP_(void) PushAxisAlignedClip(const D2D1_RECT_F *,D2D1_ANTIALIAS_MODE) override
        {
            Assert::Fail(L"Unexpected call to PushAxisAlignedClip");
        }

        IFACEMETHODIMP_(void) PopAxisAlignedClip() override
        {
            Assert::Fail(L"Unexpected call to PopAxisAlignedClip");
        }

        IFACEMETHODIMP_(void) Clear(const D2D1_COLOR_F* color) override
        {
            ClearMethod.WasCalled(color);
        }

        IFACEMETHODIMP_(void) BeginDraw() override
        {
            BeginDrawMethod.WasCalled();
        }

        IFACEMETHODIMP EndDraw(D2D1_TAG* tag0, D2D1_TAG* tag1) override
        {
            return EndDrawMethod.WasCalled(tag0, tag1);
        }

        IFACEMETHODIMP_(D2D1_PIXEL_FORMAT) GetPixelFormat() const override
        {
            Assert::Fail(L"Unexpected call to GetPixelFormat");
            return D2D1::PixelFormat();
        }

        IFACEMETHODIMP_(void) SetDpi(FLOAT dpiX, FLOAT dpiY) override
        {
            SetDpiMethod.WasCalled(dpiX, dpiY);
        }

        IFACEMETHODIMP_(void) GetDpi(FLOAT* dpiX, FLOAT* dpiY) const override
        {
            GetDpiMethod.WasCalled(dpiX, dpiY);
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

        IFACEMETHODIMP_(UINT32) GetMaximumBitmapSize() const override
        {
            return GetMaximumBitmapSizeMethod.WasCalled();
        }

        IFACEMETHODIMP_(BOOL) IsSupported(const D2D1_RENDER_TARGET_PROPERTIES *) const override
        {
            Assert::Fail(L"Unexpected call to IsSupported");
            return FALSE;
        }

        // ID2D1DeviceContext

        IFACEMETHODIMP CreateBitmap(D2D1_SIZE_U size, void const* sourceData, UINT32 pitch, D2D1_BITMAP_PROPERTIES1 const* bitmapProperties, ID2D1Bitmap1** bitmap) override
        {
            return CreateBitmapMethod.WasCalled(
                size,
                sourceData,
                pitch,
                bitmapProperties,
                bitmap);
        }

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

        IFACEMETHODIMP CreateBitmapFromDxgiSurface(IDXGISurface* surface, const D2D1_BITMAP_PROPERTIES1* properties, ID2D1Bitmap1** bitmap) override
        {
            return CreateBitmapFromDxgiSurfaceMethod.WasCalled(surface, properties, bitmap);
        }

        IFACEMETHODIMP CreateEffect(IID const& iid, ID2D1Effect** effect) override
        {
            return CreateEffectMethod.WasCalled(iid, effect);
        }

        IFACEMETHODIMP CreateGradientStopCollection(const D2D1_GRADIENT_STOP *, uint32_t, D2D1_COLOR_SPACE, D2D1_COLOR_SPACE, D2D1_BUFFER_PRECISION, D2D1_EXTEND_MODE, D2D1_COLOR_INTERPOLATION_MODE, ID2D1GradientStopCollection1 **) override
        {
            Assert::Fail(L"Unexpected call to CreateGradientStopCollection");
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

        IFACEMETHODIMP CreateCommandList(ID2D1CommandList** value) override
        {
            return CreateCommandListMethod.WasCalled(value);
        }

        IFACEMETHODIMP_(BOOL) IsDxgiFormatSupported(DXGI_FORMAT) const override
        {
            Assert::Fail(L"Unexpected call to IsDxgiFormatSupported");
            return FALSE;
        }

        IFACEMETHODIMP_(BOOL) IsBufferPrecisionSupported(D2D1_BUFFER_PRECISION) const override
        {
            Assert::Fail(L"Unexpected call to IsBufferPrecisionSupported");
            return FALSE;
        }

        IFACEMETHODIMP GetImageLocalBounds(ID2D1Image *,D2D1_RECT_F *) const override
        {
            Assert::Fail(L"Unexpected call to GetImageLocalBounds");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetImageWorldBounds(ID2D1Image* image, D2D1_RECT_F* bounds) const override
        {
            return GetImageWorldBoundsMethod.WasCalled(image, bounds);
        }

        IFACEMETHODIMP GetGlyphRunWorldBounds(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,DWRITE_MEASURING_MODE,D2D1_RECT_F *) const override
        {
            Assert::Fail(L"Unexpected call to GetGlyphRunWorldBounds");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) GetDevice(ID2D1Device** device) const override
        {
            GetDeviceMethod.WasCalled(device);
        }

        IFACEMETHODIMP_(void) SetTarget(ID2D1Image* target) override
        {
            return SetTargetMethod.WasCalled(target);
        }

        IFACEMETHODIMP_(void) GetTarget(ID2D1Image ** target) const override
        {
            return GetTargetMethod.WasCalled(target);
        }

        IFACEMETHODIMP_(void) SetRenderingControls(const D2D1_RENDERING_CONTROLS *) override
        {
            Assert::Fail(L"Unexpected call to SetRenderingControls");
        }

        IFACEMETHODIMP_(void) GetRenderingControls(D2D1_RENDERING_CONTROLS *) const override
        {
            Assert::Fail(L"Unexpected call to GetRenderingControls");
        }

        IFACEMETHODIMP_(void) SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND b) override
        {
            SetPrimitiveBlendMethod.WasCalled(b);
        }

        IFACEMETHODIMP_(D2D1_PRIMITIVE_BLEND) GetPrimitiveBlend() const override
        {
            return GetPrimitiveBlendMethod.WasCalled();
        }

        IFACEMETHODIMP_(void) SetUnitMode(D2D1_UNIT_MODE m) override
        {
            SetUnitModeMethod.WasCalled(m);
        }

        IFACEMETHODIMP_(D2D1_UNIT_MODE) GetUnitMode() const override
        {
            return GetUnitModeMethod.WasCalled();
        }

        IFACEMETHODIMP_(void) DrawGlyphRun(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,const DWRITE_GLYPH_RUN_DESCRIPTION *,ID2D1Brush *,DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawGlyphRun");
        }

        IFACEMETHODIMP_(void) DrawGdiMetafile(ID2D1GdiMetafile *,const D2D1_POINT_2F *) override
        {
            Assert::Fail(L"Unexpected call to DrawGdiMetafile");
        }

        IFACEMETHODIMP_(void) DrawBitmap(ID2D1Bitmap* bitmap, const D2D1_RECT_F* destRect, FLOAT opacity, D2D1_INTERPOLATION_MODE interpolationMode, const D2D1_RECT_F* sourceRect, const D2D1_MATRIX_4X4_F* perspective) override
        {
            DrawBitmapMethod.WasCalled(bitmap, destRect, opacity, interpolationMode, sourceRect, perspective);
        }

        IFACEMETHODIMP_(void) PushLayer(const D2D1_LAYER_PARAMETERS1 *,ID2D1Layer *) override
        {
            Assert::Fail(L"Unexpected call to PushLayer");
        }

        IFACEMETHODIMP InvalidateEffectInputRectangle(ID2D1Effect *,UINT32,const D2D1_RECT_F *) override
        {
            Assert::Fail(L"Unexpected call to InvalidateEffectInputRectangle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetEffectInvalidRectangleCount(ID2D1Effect *,UINT32 *) override
        {
            Assert::Fail(L"Unexpected call to GetEffectInvalidRectangleCount");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetEffectInvalidRectangles(ID2D1Effect *,D2D1_RECT_F *,UINT32) override
        {
            Assert::Fail(L"Unexpected call to GetEffectInvalidRectangles");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetEffectRequiredInputRectangles(ID2D1Effect *,const D2D1_RECT_F *,const D2D1_EFFECT_INPUT_DESCRIPTION *,D2D1_RECT_F *,UINT32) override
        {
            Assert::Fail(L"Unexpected call to GetEffectRequiredInputRectangles");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) FillOpacityMask(ID2D1Bitmap *,ID2D1Brush *,const D2D1_RECT_F *,const D2D1_RECT_F *) override
        {
            Assert::Fail(L"Unexpected call to FillOpacityMask");
        }

        // ID2D1DeviceContext1

        IFACEMETHODIMP CreateFilledGeometryRealization(ID2D1Geometry *,FLOAT,ID2D1GeometryRealization **) override
        {
            Assert::Fail(L"Unexpected call to CreateFilledGeometryRealization");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateStrokedGeometryRealization(ID2D1Geometry *,FLOAT,FLOAT,ID2D1StrokeStyle *,ID2D1GeometryRealization **) override
        {
            Assert::Fail(L"Unexpected call to CreateStrokedGeometryRealization");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) DrawGeometryRealization(ID2D1GeometryRealization *,ID2D1Brush *) override
        {
            Assert::Fail(L"Unexpected call to DrawGeometryRealization");
        }
    };
}
