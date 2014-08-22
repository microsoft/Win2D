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

namespace canvas
{
    class MockD2DSolidColorBrush;

    class MockD2DDeviceContext : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1DeviceContext1, ID2D1DeviceContext, ID2D1RenderTarget, ID2D1Resource>>
    {
    public:
        std::function<void(const D2D1_COLOR_F*)> MockClear;
        std::function<void(D2D1_MATRIX_3X2_F*)> MockGetTransform;
        std::function<void(const D2D1_MATRIX_3X2_F*)> MockSetTransform;
        std::function<D2D1_ANTIALIAS_MODE()> MockGetAntialiasMode;
        std::function<void(const D2D1_ANTIALIAS_MODE)> MockSetAntialiasMode;
        std::function<D2D1_PRIMITIVE_BLEND()> MockGetPrimitiveBlend;
        std::function<void(const D2D1_PRIMITIVE_BLEND)> MockSetPrimitiveBlend;
        std::function<D2D1_TEXT_ANTIALIAS_MODE()> MockGetTextAntialiasMode;
        std::function<void(const D2D1_TEXT_ANTIALIAS_MODE)> MockSetTextAntialiasMode;
        std::function<D2D1_UNIT_MODE()> MockGetUnitMode;
        std::function<void(const D2D1_UNIT_MODE)> MockSetUnitMode;
        std::function<void(float dpiX, float dpiY)> MockSetDpi;
        std::function<void(D2D1_POINT_2F,D2D1_POINT_2F,ID2D1Brush*,float,ID2D1StrokeStyle*)> MockDrawLine;
        std::function<void(const D2D1_RECT_F*,ID2D1Brush*,float,ID2D1StrokeStyle*)> MockDrawRectangle;
        std::function<void(const D2D1_RECT_F*,ID2D1Brush*)> MockFillRectangle;
        std::function<void(const D2D1_ROUNDED_RECT*,ID2D1Brush*,float,ID2D1StrokeStyle*)> MockDrawRoundedRectangle;
        std::function<void(const D2D1_ROUNDED_RECT*,ID2D1Brush*)> MockFillRoundedRectangle;
        std::function<void(const D2D1_ELLIPSE*,ID2D1Brush*,float,ID2D1StrokeStyle*)> MockDrawEllipse;
        std::function<void(const D2D1_ELLIPSE*,ID2D1Brush*)> MockFillEllipse;
        std::function<void(const wchar_t*,uint32_t,IDWriteTextFormat*,D2D1_RECT_F,ID2D1Brush*,D2D1_DRAW_TEXT_OPTIONS,DWRITE_MEASURING_MODE)> MockDrawText;
        std::function<void(ID2D1Image*)> MockDrawImage;
        std::function<void(ID2D1Device**)> MockGetDevice;

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

        IFACEMETHODIMP CreateSharedBitmap(const IID &,void *,const D2D1_BITMAP_PROPERTIES *,ID2D1Bitmap **) override
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
            const D2D1_COLOR_F* color,
            const D2D1_BRUSH_PROPERTIES* brushProperties,
            ID2D1SolidColorBrush** solidColorBrush
            ) override
        {
            ComPtr<MockD2DSolidColorBrush> mockD2DSolidColorBrush = Make<MockD2DSolidColorBrush>();
            return mockD2DSolidColorBrush.CopyTo(solidColorBrush);
        }

        IFACEMETHODIMP CreateGradientStopCollection(const D2D1_GRADIENT_STOP *,UINT32,D2D1_GAMMA,D2D1_EXTEND_MODE,ID2D1GradientStopCollection **) override
        {
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
            if (!MockDrawLine)
            {
                Assert::Fail(L"Unexpected call to DrawLine");
                return;
            }

            MockDrawLine(p0, p1, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) DrawRectangle(const D2D1_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            if (!MockDrawRectangle)
            {
                Assert::Fail(L"Unexpected call to DrawRectangle");
                return;
            }

            MockDrawRectangle(rect, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillRectangle(const D2D1_RECT_F* rect, ID2D1Brush* brush) override
        {
            if (!MockFillRectangle)
            {
                Assert::Fail(L"Unexpected call to FillRectangle");
                return;
            }

            MockFillRectangle(rect, brush);
        }

        IFACEMETHODIMP_(void) DrawRoundedRectangle(const D2D1_ROUNDED_RECT* roundedRect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            if (!MockDrawRoundedRectangle)
            {
                Assert::Fail(L"Unexpected call to DrawRoundedRectangle");
                return;
            }

            MockDrawRoundedRectangle(roundedRect, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillRoundedRectangle(const D2D1_ROUNDED_RECT* roundedRect, ID2D1Brush* brush) override
        {
            if (!MockFillRoundedRectangle)
            {
                Assert::Fail(L"Unexpected call to FillRoundedRectangle");
                return;
            }

            MockFillRoundedRectangle(roundedRect, brush);
        }

        IFACEMETHODIMP_(void) DrawEllipse(const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle) override
        {
            if (!MockDrawEllipse)
            {
                Assert::Fail(L"Unexpected call to DrawEllipse");
                return;
            }

            MockDrawEllipse(ellipse, brush, strokeWidth, strokeStyle);
        }

        IFACEMETHODIMP_(void) FillEllipse(const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush) override
        {
            if (!MockFillEllipse)
            {
                Assert::Fail(L"Unexpected call to FillEllipse");
                return;
            }

            MockFillEllipse(ellipse, brush);
        }

        IFACEMETHODIMP_(void) DrawGeometry(ID2D1Geometry *,ID2D1Brush *,FLOAT,ID2D1StrokeStyle *) override
        {
            Assert::Fail(L"Unexpected call to DrawGeometry");
        }

        IFACEMETHODIMP_(void) FillGeometry(ID2D1Geometry *,ID2D1Brush *,ID2D1Brush *) override
        {
            Assert::Fail(L"Unexpected call to FillGeometry");
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
            Assert::Fail(L"Unexpected call to DrawBitmap");
        }

        IFACEMETHODIMP_(void) DrawText(const wchar_t* text, uint32_t textLength, IDWriteTextFormat* format, const D2D1_RECT_F* rect, ID2D1Brush* brush, D2D1_DRAW_TEXT_OPTIONS options, DWRITE_MEASURING_MODE measuringMode) override
        {
            if (!MockDrawText)
            {
                Assert::Fail(L"Unexpected call to MockDrawText");
                return;
            }

            MockDrawText(text, textLength, format, *rect, brush, options, measuringMode);
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
            if (!MockDrawImage)
            {
                Assert::Fail(L"Unexpected call to DrawImage");
                return;
            }

            MockDrawImage(image);
        }

        IFACEMETHODIMP_(void) SetTransform(const D2D1_MATRIX_3X2_F* m) override
        {
            if (!MockSetTransform)
            {
                Assert::Fail(L"Unexpected call to SetTransform");
                return;
            }

            MockSetTransform(m);
        }

        IFACEMETHODIMP_(void) GetTransform(D2D1_MATRIX_3X2_F *m) const override
        {
            if (!MockGetTransform)
            {
                Assert::Fail(L"Unexpected call to GetTransform");
                return;
            }

            MockGetTransform(m);
        }

        IFACEMETHODIMP_(void) SetAntialiasMode(D2D1_ANTIALIAS_MODE m) override
        {
            if (!MockSetAntialiasMode)
            {
                Assert::Fail(L"Unexpected call to SetAntialiasMode");
                return;
            }

            MockSetAntialiasMode(m);
        }

        IFACEMETHODIMP_(D2D1_ANTIALIAS_MODE) GetAntialiasMode() const override
        {
            if (!MockGetAntialiasMode)
            {
                Assert::Fail(L"Unexpected call to GetAntialiasMode");
                return D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
            }

            return MockGetAntialiasMode();
        }

        IFACEMETHODIMP_(void) SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE m) override
        {
            if (!MockSetTextAntialiasMode)
            {
                Assert::Fail(L"Unexpected call to SetTextAntialiasMode");
                return;
            }

            MockSetTextAntialiasMode(m);
        }

        IFACEMETHODIMP_(D2D1_TEXT_ANTIALIAS_MODE) GetTextAntialiasMode() const override
        {
            if (!MockGetTextAntialiasMode)
            {
                Assert::Fail(L"Unexpected call to GetTextAntialiasMode");
                return D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
            }

            return MockGetTextAntialiasMode();
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
            if (!MockClear)
            {
                Assert::Fail(L"Unexpected call to Clear");
                return;
            }

            MockClear(color);
        }

        IFACEMETHODIMP_(void) BeginDraw() override
        {
            Assert::Fail(L"Unexpected call to BeginDraw");
        }

        IFACEMETHODIMP EndDraw(D2D1_TAG *,D2D1_TAG *) override
        {
            Assert::Fail(L"Unexpected call to EndDraw");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(D2D1_PIXEL_FORMAT) GetPixelFormat() const override
        {
            Assert::Fail(L"Unexpected call to GetPixelFormat");
            return D2D1::PixelFormat();
        }

        IFACEMETHODIMP_(void) SetDpi(FLOAT dpiX, FLOAT dpiY) override
        {
            if (!MockSetDpi)
            {
                Assert::Fail(L"Unexpected call to SetDpi");
                return;
            }

            MockSetDpi(dpiX, dpiY);
        }

        IFACEMETHODIMP_(void) GetDpi(FLOAT *,FLOAT *) const override
        {
            Assert::Fail(L"Unexpected call to GetDpi");
        }

        IFACEMETHODIMP_(D2D1_SIZE_F) GetSize() const override
        {
            Assert::Fail(L"Unexpected call to GetSize");
            return D2D1_SIZE_F{0.0f,0.0f};
        }

        IFACEMETHODIMP_(D2D1_SIZE_U) GetPixelSize() const override
        {
            Assert::Fail(L"Unexpected call to GetPixelSize");
            return D2D1_SIZE_U{0U,0U};
        }

        IFACEMETHODIMP_(UINT32) GetMaximumBitmapSize() const override
        {
            Assert::Fail(L"Unexpected call to GetMaximumBitmapSize");
            return 0;
        }

        IFACEMETHODIMP_(BOOL) IsSupported(const D2D1_RENDER_TARGET_PROPERTIES *) const override
        {
            Assert::Fail(L"Unexpected call to IsSupported");
            return FALSE;
        }

        // ID2D1DeviceContext

        IFACEMETHODIMP CreateBitmap(D2D1_SIZE_U,const void *,UINT32,const D2D1_BITMAP_PROPERTIES1 *,ID2D1Bitmap1 **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmap");
            return E_NOTIMPL;
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

        IFACEMETHODIMP CreateBitmapFromDxgiSurface(IDXGISurface *,const D2D1_BITMAP_PROPERTIES1 *,ID2D1Bitmap1 **) override
        {
            Assert::Fail(L"Unexpected call to CreateBitmapFromDxgiSurface");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateEffect(const IID &,ID2D1Effect **) override
        {
            Assert::Fail(L"Unexpected call to CreateEffect");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateGradientStopCollection(const D2D1_GRADIENT_STOP *,UINT32,D2D1_COLOR_SPACE,D2D1_COLOR_SPACE,D2D1_BUFFER_PRECISION,D2D1_EXTEND_MODE,D2D1_COLOR_INTERPOLATION_MODE,ID2D1GradientStopCollection1 **) override
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

        IFACEMETHODIMP CreateCommandList(ID2D1CommandList **) override
        {
            Assert::Fail(L"Unexpected call to CreateCommandList");
            return E_NOTIMPL;
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

        IFACEMETHODIMP GetImageWorldBounds(ID2D1Image *,D2D1_RECT_F *) const override
        {
            Assert::Fail(L"Unexpected call to GetImageWorldBounds");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetGlyphRunWorldBounds(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,DWRITE_MEASURING_MODE,D2D1_RECT_F *) const override
        {
            Assert::Fail(L"Unexpected call to GetGlyphRunWorldBounds");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) GetDevice(ID2D1Device** device) const override
        {
            if (!MockGetDevice)
            {
                Assert::Fail(L"Unexpected call to GetDevice");
                return;
            }

            MockGetDevice(device);
        }

        IFACEMETHODIMP_(void) SetTarget(ID2D1Image *) override
        {
            Assert::Fail(L"Unexpected call to SetTarget");
        }

        IFACEMETHODIMP_(void) GetTarget(ID2D1Image **) const override
        {
            Assert::Fail(L"Unexpected call to GetTarget");
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
            if (!MockSetPrimitiveBlend)
            {
                Assert::Fail(L"Unexpected call to SetPrimitiveBlend");
                return;
            }

            MockSetPrimitiveBlend(b);
        }

        IFACEMETHODIMP_(D2D1_PRIMITIVE_BLEND) GetPrimitiveBlend() const override
        {
            if (!MockGetPrimitiveBlend)
            {
                Assert::Fail(L"Unexpected call to GetPrimitiveBlend");
                return D2D1_PRIMITIVE_BLEND_SOURCE_OVER;
            }

            return MockGetPrimitiveBlend();
        }

        IFACEMETHODIMP_(void) SetUnitMode(D2D1_UNIT_MODE m) override
        {
            if (!MockSetUnitMode)
            {
                Assert::Fail(L"Unexpected call to SetUnitMode");
                return;
            }

            MockSetUnitMode(m);
        }

        IFACEMETHODIMP_(D2D1_UNIT_MODE) GetUnitMode() const override
        {
            if (!MockGetUnitMode)
            {
                Assert::Fail(L"Unexpected call to GetUnitMode");
                return D2D1_UNIT_MODE_DIPS;
            }

            return MockGetUnitMode();
        }

        IFACEMETHODIMP_(void) DrawGlyphRun(D2D1_POINT_2F,const DWRITE_GLYPH_RUN *,const DWRITE_GLYPH_RUN_DESCRIPTION *,ID2D1Brush *,DWRITE_MEASURING_MODE) override
        {
            Assert::Fail(L"Unexpected call to DrawGlyphRun");
        }

        IFACEMETHODIMP_(void) DrawGdiMetafile(ID2D1GdiMetafile *,const D2D1_POINT_2F *) override
        {
            Assert::Fail(L"Unexpected call to DrawGdiMetafile");
        }

        IFACEMETHODIMP_(void) DrawBitmap(ID2D1Bitmap *,const D2D1_RECT_F *,FLOAT,D2D1_INTERPOLATION_MODE,const D2D1_RECT_F *,const D2D1_MATRIX_4X4_F *) override
        {
            Assert::Fail(L"Unexpected call to DrawBitmap");
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
