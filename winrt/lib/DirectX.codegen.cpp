//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This file was automatically generated. Please do not edit it manually.
//

#include "pch.h"
#include "d2dwinrt.h"
#include <wrl.h>
using namespace Microsoft::WRL;
using namespace ABI::Microsoft::DirectX;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;

// This is placeholder until the inbox math types are ready to use.
typedef int Matrix3x2;

namespace dxrt
{
    class DxArcSegment : public Microsoft::WRL::RuntimeClass<IDxArcSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxArcSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point)(Point value) override
        {
            m_point = value;
            return S_OK;
        }

        IFACEMETHOD(get_Size)(_Out_ Size *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_size;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Size)(Size value) override
        {
            m_size = value;
            return S_OK;
        }

        IFACEMETHOD(get_RotationAngle)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_rotationAngle;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RotationAngle)(FLOAT value) override
        {
            m_rotationAngle = value;
            return S_OK;
        }

        IFACEMETHOD(get_SweepDirection)(_Out_ DxSweepDirection *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_sweepDirection;
                return S_OK;
            }
        }

        IFACEMETHOD(put_SweepDirection)(DxSweepDirection value) override
        {
            m_sweepDirection = value;
            return S_OK;
        }

        IFACEMETHOD(get_ArcSize)(_Out_ DxArcSize *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_arcSize;
                return S_OK;
            }
        }

        IFACEMETHOD(put_ArcSize)(DxArcSize value) override
        {
            m_arcSize = value;
            return S_OK;
        }

    private:
        Point m_point;
        Size m_size;
        FLOAT m_rotationAngle;
        DxSweepDirection m_sweepDirection;
        DxArcSize m_arcSize;
    };

    class DxBezierSegment : public Microsoft::WRL::RuntimeClass<IDxBezierSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxBezierSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point1;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point1)(Point value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point2;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point2)(Point value) override
        {
            m_point2 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point3)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point3;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point3)(Point value) override
        {
            m_point3 = value;
            return S_OK;
        }

    private:
        Point m_point1;
        Point m_point2;
        Point m_point3;
    };

    class DxTriangle : public Microsoft::WRL::RuntimeClass<IDxTriangle>
    {
        InspectableClass(L"Microsoft.DirectX.DxTriangle", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point1;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point1)(Point value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point2;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point2)(Point value) override
        {
            m_point2 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point3)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point3;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point3)(Point value) override
        {
            m_point3 = value;
            return S_OK;
        }

    private:
        Point m_point1;
        Point m_point2;
        Point m_point3;
    };

    class DxEllipse : public Microsoft::WRL::RuntimeClass<IDxEllipse>
    {
        InspectableClass(L"Microsoft.DirectX.DxEllipse", BaseTrust);

    public:
        IFACEMETHOD(get_Point)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point)(Point value) override
        {
            m_point = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusX;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusX)(FLOAT value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusY;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusY)(FLOAT value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        Point m_point;
        FLOAT m_radiusX;
        FLOAT m_radiusY;
    };

    class DxGradientStop : public Microsoft::WRL::RuntimeClass<IDxGradientStop>
    {
        InspectableClass(L"Microsoft.DirectX.DxGradientStop", BaseTrust);

    public:
        IFACEMETHOD(get_Position)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_position;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Position)(FLOAT value) override
        {
            m_position = value;
            return S_OK;
        }

        IFACEMETHOD(get_Color)(_Out_ Color *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_color;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Color)(Color value) override
        {
            m_color = value;
            return S_OK;
        }

    private:
        FLOAT m_position;
        Color m_color;
    };

    class DxQuadraticBezierSegment : public Microsoft::WRL::RuntimeClass<IDxQuadraticBezierSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxQuadraticBezierSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point1;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point1)(Point value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_point2;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Point2)(Point value) override
        {
            m_point2 = value;
            return S_OK;
        }

    private:
        Point m_point1;
        Point m_point2;
    };

    class RoundedRectangle : public Microsoft::WRL::RuntimeClass<IRoundedRectangle>
    {
        InspectableClass(L"Microsoft.DirectX.RoundedRectangle", BaseTrust);

    public:
        IFACEMETHOD(get_Rect)(_Out_ Rect *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_rect;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Rect)(Rect value) override
        {
            m_rect = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusX;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusX)(FLOAT value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusY;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusY)(FLOAT value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        Rect m_rect;
        FLOAT m_radiusX;
        FLOAT m_radiusY;
    };

    class DxLinearGradientBrushProperties : public Microsoft::WRL::RuntimeClass<IDxLinearGradientBrushProperties>
    {
        InspectableClass(L"Microsoft.DirectX.DxLinearGradientBrushProperties", BaseTrust);

    public:
        IFACEMETHOD(get_StartPoint)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_startPoint;
                return S_OK;
            }
        }

        IFACEMETHOD(put_StartPoint)(Point value) override
        {
            m_startPoint = value;
            return S_OK;
        }

        IFACEMETHOD(get_EndPoint)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_endPoint;
                return S_OK;
            }
        }

        IFACEMETHOD(put_EndPoint)(Point value) override
        {
            m_endPoint = value;
            return S_OK;
        }

    private:
        Point m_startPoint;
        Point m_endPoint;
    };

    class DxRadialGradientBrushProperties : public Microsoft::WRL::RuntimeClass<IDxRadialGradientBrushProperties>
    {
        InspectableClass(L"Microsoft.DirectX.DxRadialGradientBrushProperties", BaseTrust);

    public:
        IFACEMETHOD(get_Center)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_center;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Center)(Point value) override
        {
            m_center = value;
            return S_OK;
        }

        IFACEMETHOD(get_GradientOriginOffset)(_Out_ Point *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_gradientOriginOffset;
                return S_OK;
            }
        }

        IFACEMETHOD(put_GradientOriginOffset)(Point value) override
        {
            m_gradientOriginOffset = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusX;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusX)(FLOAT value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_radiusY;
                return S_OK;
            }
        }

        IFACEMETHOD(put_RadiusY)(FLOAT value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        Point m_center;
        Point m_gradientOriginOffset;
        FLOAT m_radiusX;
        FLOAT m_radiusY;
    };

    class LayerParameters : public Microsoft::WRL::RuntimeClass<ILayerParameters>
    {
        InspectableClass(L"Microsoft.DirectX.LayerParameters", BaseTrust);

    public:
        IFACEMETHOD(get_ContentBounds)(_Out_ Rect *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_contentBounds;
                return S_OK;
            }
        }

        IFACEMETHOD(put_ContentBounds)(Rect value) override
        {
            m_contentBounds = value;
            return S_OK;
        }

        IFACEMETHOD(get_GeometricMask)(_Out_ IDxGeometry* *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_geometricMask.Get();
                return S_OK;
            }
        }

        IFACEMETHOD(put_GeometricMask)(IDxGeometry* value) override
        {
            m_geometricMask = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskAntialiasMode)(_Out_ DxAntialiasMode *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_maskAntialiasMode;
                return S_OK;
            }
        }

        IFACEMETHOD(put_MaskAntialiasMode)(DxAntialiasMode value) override
        {
            m_maskAntialiasMode = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskTransform)(_Out_ Matrix3x2 *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_maskTransform;
                return S_OK;
            }
        }

        IFACEMETHOD(put_MaskTransform)(Matrix3x2 value) override
        {
            m_maskTransform = value;
            return S_OK;
        }

        IFACEMETHOD(get_Opacity)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_opacity;
                return S_OK;
            }
        }

        IFACEMETHOD(put_Opacity)(FLOAT value) override
        {
            m_opacity = value;
            return S_OK;
        }

        IFACEMETHOD(get_OpacityBrush)(_Out_ IDxBrush* *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_opacityBrush.Get();
                return S_OK;
            }
        }

        IFACEMETHOD(put_OpacityBrush)(IDxBrush* value) override
        {
            m_opacityBrush = value;
            return S_OK;
        }

        IFACEMETHOD(get_LayerOptions)(_Out_ LayerOptions *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_layerOptions;
                return S_OK;
            }
        }

        IFACEMETHOD(put_LayerOptions)(LayerOptions value) override
        {
            m_layerOptions = value;
            return S_OK;
        }

    private:
        Rect m_contentBounds;
        ComPtr<IDxGeometry> m_geometricMask;
        DxAntialiasMode m_maskAntialiasMode;
        Matrix3x2 m_maskTransform;
        FLOAT m_opacity;
        ComPtr<IDxBrush> m_opacityBrush;
        LayerOptions m_layerOptions;
    };

    class StrokeStyleProperties : public Microsoft::WRL::RuntimeClass<IStrokeStyleProperties>
    {
        InspectableClass(L"Microsoft.DirectX.StrokeStyleProperties", BaseTrust);

    public:
        IFACEMETHOD(get_StartCap)(_Out_ DxCapStyle *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_startCap;
                return S_OK;
            }
        }

        IFACEMETHOD(put_StartCap)(DxCapStyle value) override
        {
            m_startCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_EndCap)(_Out_ DxCapStyle *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_endCap;
                return S_OK;
            }
        }

        IFACEMETHOD(put_EndCap)(DxCapStyle value) override
        {
            m_endCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashCap)(_Out_ DxCapStyle *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_dashCap;
                return S_OK;
            }
        }

        IFACEMETHOD(put_DashCap)(DxCapStyle value) override
        {
            m_dashCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_LineJoin)(_Out_ DxLineJoin *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_lineJoin;
                return S_OK;
            }
        }

        IFACEMETHOD(put_LineJoin)(DxLineJoin value) override
        {
            m_lineJoin = value;
            return S_OK;
        }

        IFACEMETHOD(get_MiterLimit)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_miterLimit;
                return S_OK;
            }
        }

        IFACEMETHOD(put_MiterLimit)(FLOAT value) override
        {
            m_miterLimit = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashStyle)(_Out_ DxDashStyle *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_dashStyle;
                return S_OK;
            }
        }

        IFACEMETHOD(put_DashStyle)(DxDashStyle value) override
        {
            m_dashStyle = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashOffset)(_Out_ FLOAT *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_dashOffset;
                return S_OK;
            }
        }

        IFACEMETHOD(put_DashOffset)(FLOAT value) override
        {
            m_dashOffset = value;
            return S_OK;
        }

        IFACEMETHOD(get_TransformType)(_Out_ DxStrokeTransformType *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_transformType;
                return S_OK;
            }
        }

        IFACEMETHOD(put_TransformType)(DxStrokeTransformType value) override
        {
            m_transformType = value;
            return S_OK;
        }

    private:
        DxCapStyle m_startCap;
        DxCapStyle m_endCap;
        DxCapStyle m_dashCap;
        DxLineJoin m_lineJoin;
        FLOAT m_miterLimit;
        DxDashStyle m_dashStyle;
        FLOAT m_dashOffset;
        DxStrokeTransformType m_transformType;
    };

}
