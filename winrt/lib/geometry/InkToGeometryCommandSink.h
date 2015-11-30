// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    class InkToGeometryCommandSink : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1CommandSink2>,
                                     private LifespanTracker<InkToGeometryCommandSink>
    {
        HRESULT m_result;

        D2D1_MATRIX_3X2_F m_transform;
        float m_flatteningTolerance;

        ComPtr<ID2D1GeometrySink> m_geometrySink;

    public:
        InkToGeometryCommandSink(Matrix3x2 const& transform, float flatteningTolerance, ID2D1GeometrySink* geometrySink)
            : m_result(S_OK)
            , m_transform(*ReinterpretAs<D2D1_MATRIX_3X2_F const*>(&transform))
            , m_flatteningTolerance(flatteningTolerance)
            , m_geometrySink(geometrySink)
        { }

        HRESULT GetResult()
        {
            return m_result;
        }

        IFACEMETHODIMP DrawInk(ID2D1Ink* ink, ID2D1Brush*, ID2D1InkStyle* inkStyle) override
        {
            if (SUCCEEDED(m_result))
            {
                m_result = ink->StreamAsGeometry(inkStyle, m_transform, m_flatteningTolerance, m_geometrySink.Get());
            }

            return m_result;
        }

        IFACEMETHODIMP BeginDraw() override { return S_OK; }
        IFACEMETHODIMP EndDraw() override { return S_OK; }
        IFACEMETHODIMP SetAntialiasMode(D2D1_ANTIALIAS_MODE) override { return S_OK; }
        IFACEMETHODIMP SetTags(D2D1_TAG, D2D1_TAG) override { return S_OK; }
        IFACEMETHODIMP SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE) override { return S_OK; }
        IFACEMETHODIMP SetTextRenderingParams(IDWriteRenderingParams*) override { return S_OK; }
        IFACEMETHODIMP SetTransform(CONST D2D1_MATRIX_3X2_F*) override { return S_OK; }
        IFACEMETHODIMP SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND) override { return S_OK; }
        IFACEMETHODIMP SetUnitMode(D2D1_UNIT_MODE) override { return S_OK; }
        IFACEMETHODIMP Clear(CONST D2D1_COLOR_F*) override { return S_OK; }
        IFACEMETHODIMP DrawGlyphRun(D2D1_POINT_2F, DWRITE_GLYPH_RUN CONST*, DWRITE_GLYPH_RUN_DESCRIPTION CONST*, ID2D1Brush*, DWRITE_MEASURING_MODE) override { return S_OK; }
        IFACEMETHODIMP DrawLine(D2D1_POINT_2F, D2D1_POINT_2F, ID2D1Brush*, FLOAT, ID2D1StrokeStyle*) override { return S_OK; }
        IFACEMETHODIMP DrawGeometry(ID2D1Geometry*, ID2D1Brush*, FLOAT, ID2D1StrokeStyle*) override { return S_OK; }
        IFACEMETHODIMP DrawRectangle(D2D1_RECT_F CONST*, ID2D1Brush*, FLOAT, ID2D1StrokeStyle*) override { return S_OK; }
        IFACEMETHODIMP DrawBitmap(ID2D1Bitmap*, D2D1_RECT_F CONST*, FLOAT, D2D1_INTERPOLATION_MODE, D2D1_RECT_F CONST*, D2D1_MATRIX_4X4_F CONST*) override { return S_OK; }
        IFACEMETHODIMP DrawImage(ID2D1Image*, D2D1_POINT_2F CONST*, D2D1_RECT_F CONST*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE) override { return S_OK; }
        IFACEMETHODIMP DrawGdiMetafile(ID2D1GdiMetafile*, D2D1_POINT_2F CONST*) override { return S_OK; }
        IFACEMETHODIMP FillMesh(ID2D1Mesh*, ID2D1Brush*) override { return S_OK; }
        IFACEMETHODIMP FillOpacityMask(ID2D1Bitmap*, ID2D1Brush*, D2D1_RECT_F CONST*, D2D1_RECT_F CONST*) override { return S_OK; }
        IFACEMETHODIMP FillGeometry(ID2D1Geometry*, ID2D1Brush*, ID2D1Brush*) override { return S_OK; }
        IFACEMETHODIMP FillRectangle(D2D1_RECT_F CONST*, ID2D1Brush*) override { return S_OK; }
        IFACEMETHODIMP PushAxisAlignedClip(D2D1_RECT_F CONST*, D2D1_ANTIALIAS_MODE) override { return S_OK; }
        IFACEMETHODIMP PushLayer(D2D1_LAYER_PARAMETERS1 CONST*, ID2D1Layer*) override { return S_OK; }
        IFACEMETHODIMP PopAxisAlignedClip() override { return S_OK; }
        IFACEMETHODIMP PopLayer() override { return S_OK; }
        IFACEMETHODIMP SetPrimitiveBlend1(D2D1_PRIMITIVE_BLEND) override { return S_OK; }
        IFACEMETHODIMP DrawGradientMesh(ID2D1GradientMesh*) override { return S_OK; }
        IFACEMETHODIMP DrawGdiMetafile(ID2D1GdiMetafile*, D2D1_RECT_F CONST*, D2D1_RECT_F CONST*) override { return S_OK; }
    };
}}}}}
