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

namespace dxrt
{
    class DxArcSegment : public Microsoft::WRL::RuntimeClass<IDxArcSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxArcSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point)(int value) override
        {
            m_point = value;
            return S_OK;
        }

        IFACEMETHOD(get_Size)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Size)(int value) override
        {
            m_size = value;
            return S_OK;
        }

        IFACEMETHOD(get_RotationAngle)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RotationAngle)(int value) override
        {
            m_rotationAngle = value;
            return S_OK;
        }

        IFACEMETHOD(get_SweepDirection)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_SweepDirection)(int value) override
        {
            m_sweepDirection = value;
            return S_OK;
        }

        IFACEMETHOD(get_ArcSize)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_ArcSize)(int value) override
        {
            m_arcSize = value;
            return S_OK;
        }

    private:
        int m_point;
        int m_size;
        int m_rotationAngle;
        int m_sweepDirection;
        int m_arcSize;
    };

    class DxBezierSegment : public Microsoft::WRL::RuntimeClass<IDxBezierSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxBezierSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point1)(int value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point2)(int value) override
        {
            m_point2 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point3)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point3)(int value) override
        {
            m_point3 = value;
            return S_OK;
        }

    private:
        int m_point1;
        int m_point2;
        int m_point3;
    };

    class DxTriangle : public Microsoft::WRL::RuntimeClass<IDxTriangle>
    {
        InspectableClass(L"Microsoft.DirectX.DxTriangle", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point1)(int value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point2)(int value) override
        {
            m_point2 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point3)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point3)(int value) override
        {
            m_point3 = value;
            return S_OK;
        }

    private:
        int m_point1;
        int m_point2;
        int m_point3;
    };

    class DxEllipse : public Microsoft::WRL::RuntimeClass<IDxEllipse>
    {
        InspectableClass(L"Microsoft.DirectX.DxEllipse", BaseTrust);

    public:
        IFACEMETHOD(get_Point)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point)(int value) override
        {
            m_point = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusX)(int value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusY)(int value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        int m_point;
        int m_radiusX;
        int m_radiusY;
    };

    class DxGradientStop : public Microsoft::WRL::RuntimeClass<IDxGradientStop>
    {
        InspectableClass(L"Microsoft.DirectX.DxGradientStop", BaseTrust);

    public:
        IFACEMETHOD(get_Position)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Position)(int value) override
        {
            m_position = value;
            return S_OK;
        }

        IFACEMETHOD(get_Color)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Color)(int value) override
        {
            m_color = value;
            return S_OK;
        }

    private:
        int m_position;
        int m_color;
    };

    class DxQuadraticBezierSegment : public Microsoft::WRL::RuntimeClass<IDxQuadraticBezierSegment>
    {
        InspectableClass(L"Microsoft.DirectX.DxQuadraticBezierSegment", BaseTrust);

    public:
        IFACEMETHOD(get_Point1)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point1)(int value) override
        {
            m_point1 = value;
            return S_OK;
        }

        IFACEMETHOD(get_Point2)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Point2)(int value) override
        {
            m_point2 = value;
            return S_OK;
        }

    private:
        int m_point1;
        int m_point2;
    };

    class RoundedRectangle : public Microsoft::WRL::RuntimeClass<IRoundedRectangle>
    {
        InspectableClass(L"Microsoft.DirectX.RoundedRectangle", BaseTrust);

    public:
        IFACEMETHOD(get_Rect)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Rect)(int value) override
        {
            m_rect = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusX)(int value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusY)(int value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        int m_rect;
        int m_radiusX;
        int m_radiusY;
    };

    class DxLinearGradientBrushProperties : public Microsoft::WRL::RuntimeClass<IDxLinearGradientBrushProperties>
    {
        InspectableClass(L"Microsoft.DirectX.DxLinearGradientBrushProperties", BaseTrust);

    public:
        IFACEMETHOD(get_StartPoint)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_StartPoint)(int value) override
        {
            m_startPoint = value;
            return S_OK;
        }

        IFACEMETHOD(get_EndPoint)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_EndPoint)(int value) override
        {
            m_endPoint = value;
            return S_OK;
        }

    private:
        int m_startPoint;
        int m_endPoint;
    };

    class DxRadialGradientBrushProperties : public Microsoft::WRL::RuntimeClass<IDxRadialGradientBrushProperties>
    {
        InspectableClass(L"Microsoft.DirectX.DxRadialGradientBrushProperties", BaseTrust);

    public:
        IFACEMETHOD(get_Center)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Center)(int value) override
        {
            m_center = value;
            return S_OK;
        }

        IFACEMETHOD(get_GradientOriginOffset)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_GradientOriginOffset)(int value) override
        {
            m_gradientOriginOffset = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusX)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusX)(int value) override
        {
            m_radiusX = value;
            return S_OK;
        }

        IFACEMETHOD(get_RadiusY)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_RadiusY)(int value) override
        {
            m_radiusY = value;
            return S_OK;
        }

    private:
        int m_center;
        int m_gradientOriginOffset;
        int m_radiusX;
        int m_radiusY;
    };

    class LayerParameters : public Microsoft::WRL::RuntimeClass<ILayerParameters>
    {
        InspectableClass(L"Microsoft.DirectX.LayerParameters", BaseTrust);

    public:
        IFACEMETHOD(get_ContentBounds)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_ContentBounds)(int value) override
        {
            m_contentBounds = value;
            return S_OK;
        }

        IFACEMETHOD(get_GeometricMask)(_Out_ int *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_geometricMask;
                return S_OK;
            }
        }

        IFACEMETHOD(put_GeometricMask)(int value) override
        {
            m_geometricMask = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskAntialiasMode)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_MaskAntialiasMode)(int value) override
        {
            m_maskAntialiasMode = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskTransform)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_MaskTransform)(int value) override
        {
            m_maskTransform = value;
            return S_OK;
        }

        IFACEMETHOD(get_Opacity)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_Opacity)(int value) override
        {
            m_opacity = value;
            return S_OK;
        }

        IFACEMETHOD(get_OpacityBrush)(_Out_ int *pValue) override
        {
            if (pValue == nullptr)
            {
                return E_POINTER;
            }
            else
            {
                *pValue = m_opacityBrush;
                return S_OK;
            }
        }

        IFACEMETHOD(put_OpacityBrush)(int value) override
        {
            m_opacityBrush = value;
            return S_OK;
        }

        IFACEMETHOD(get_LayerOptions)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_LayerOptions)(int value) override
        {
            m_layerOptions = value;
            return S_OK;
        }

    private:
        int m_contentBounds;
        int m_geometricMask;
        int m_maskAntialiasMode;
        int m_maskTransform;
        int m_opacity;
        int m_opacityBrush;
        int m_layerOptions;
    };

    class StrokeStyleProperties : public Microsoft::WRL::RuntimeClass<IStrokeStyleProperties>
    {
        InspectableClass(L"Microsoft.DirectX.StrokeStyleProperties", BaseTrust);

    public:
        IFACEMETHOD(get_StartCap)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_StartCap)(int value) override
        {
            m_startCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_EndCap)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_EndCap)(int value) override
        {
            m_endCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashCap)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_DashCap)(int value) override
        {
            m_dashCap = value;
            return S_OK;
        }

        IFACEMETHOD(get_LineJoin)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_LineJoin)(int value) override
        {
            m_lineJoin = value;
            return S_OK;
        }

        IFACEMETHOD(get_MiterLimit)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_MiterLimit)(int value) override
        {
            m_miterLimit = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashStyle)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_DashStyle)(int value) override
        {
            m_dashStyle = value;
            return S_OK;
        }

        IFACEMETHOD(get_DashOffset)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_DashOffset)(int value) override
        {
            m_dashOffset = value;
            return S_OK;
        }

        IFACEMETHOD(get_TransformType)(_Out_ int *pValue) override
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

        IFACEMETHOD(put_TransformType)(int value) override
        {
            m_transformType = value;
            return S_OK;
        }

    private:
        int m_startCap;
        int m_endCap;
        int m_dashCap;
        int m_lineJoin;
        int m_miterLimit;
        int m_dashStyle;
        int m_dashOffset;
        int m_transformType;
    };

}
