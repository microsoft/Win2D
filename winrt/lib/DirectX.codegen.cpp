//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This file was automatically generated. Please do not edit it manually.
//

#include "pch.h"
#include "d2dwinrt.h"
#include <wrl.h>
using namespace Microsoft::WRL;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Math;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;

namespace canvas
{
    class LayerParameters : public Microsoft::WRL::RuntimeClass<ILayerParameters>
    {
        InspectableClass(L"Microsoft.DirectX.LayerParameters", BaseTrust);

    public:
        IFACEMETHOD(get_ContentBounds)(_Out_ Rect* value) override
        {
            if (!value)
                return E_INVALIDARG;

            *value = m_contentBounds;
            return S_OK;
        }

        IFACEMETHOD(put_ContentBounds)(Rect value) override
        {
            m_contentBounds = value;
            return S_OK;
        }

        IFACEMETHOD(get_GeometricMask)(_Out_ ICanvasGeometry** value) override
        {
            if (!value)
                return E_INVALIDARG;

            m_geometricMask.CopyTo(value);
            return S_OK;
        }

        IFACEMETHOD(put_GeometricMask)(ICanvasGeometry* value) override
        {
            m_geometricMask = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskAntialiasMode)(_Out_ CanvasAntialiasMode* value) override
        {
            if (!value)
                return E_INVALIDARG;

            *value = m_maskAntialiasMode;
            return S_OK;
        }

        IFACEMETHOD(put_MaskAntialiasMode)(CanvasAntialiasMode value) override
        {
            m_maskAntialiasMode = value;
            return S_OK;
        }

        IFACEMETHOD(get_MaskTransform)(_Out_ Matrix3x2* value) override
        {
            if (!value)
                return E_INVALIDARG;

            *value = m_maskTransform;
            return S_OK;
        }

        IFACEMETHOD(put_MaskTransform)(Matrix3x2 value) override
        {
            m_maskTransform = value;
            return S_OK;
        }

        IFACEMETHOD(get_Opacity)(_Out_ FLOAT* value) override
        {
            if (!value)
                return E_INVALIDARG;

            *value = m_opacity;
            return S_OK;
        }

        IFACEMETHOD(put_Opacity)(FLOAT value) override
        {
            m_opacity = value;
            return S_OK;
        }

        IFACEMETHOD(get_OpacityBrush)(_Out_ ICanvasBrush** value) override
        {
            if (!value)
                return E_INVALIDARG;

            m_opacityBrush.CopyTo(value);
            return S_OK;
        }

        IFACEMETHOD(put_OpacityBrush)(ICanvasBrush* value) override
        {
            m_opacityBrush = value;
            return S_OK;
        }

        IFACEMETHOD(get_LayerOptions)(_Out_ CanvasLayerOptions* value) override
        {
            if (!value)
                return E_INVALIDARG;

            *value = m_layerOptions;
            return S_OK;
        }

        IFACEMETHOD(put_LayerOptions)(CanvasLayerOptions value) override
        {
            m_layerOptions = value;
            return S_OK;
        }

    private:
        Rect m_contentBounds;
        ComPtr<ICanvasGeometry> m_geometricMask;
        CanvasAntialiasMode m_maskAntialiasMode;
        Matrix3x2 m_maskTransform;
        FLOAT m_opacity;
        ComPtr<ICanvasBrush> m_opacityBrush;
        CanvasLayerOptions m_layerOptions;
    };

}
