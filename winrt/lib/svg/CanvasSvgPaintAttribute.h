// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE 

#include "AttributeHelpers.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgPaintAttribute : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgPaintAttribute,
            ICanvasSvgAttribute,
            CloakedIid<AttributeHelpers::IAttributeInternal>,
            ChainInterfaces<
                MixIn<CanvasSvgPaintAttribute, ResourceWrapper<ID2D1SvgPaint, CanvasSvgPaintAttribute, ICanvasSvgPaintAttribute>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgPaint, CanvasSvgPaintAttribute, ICanvasSvgPaintAttribute>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgPaintAttribute, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasSvgPaintAttribute(
            ICanvasDevice* canvasDevice,
            ID2D1SvgPaint* d2dSvgPaint);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_PaintType)(CanvasSvgPaintType*);
        IFACEMETHOD(put_PaintType)(CanvasSvgPaintType);

        IFACEMETHOD(get_Color)(Color*);
        IFACEMETHOD(put_Color)(Color);

        IFACEMETHOD(get_Id)(HSTRING*);
        IFACEMETHOD(put_Id)(HSTRING);

        IFACEMETHOD(Clone)(ICanvasSvgAttribute**);

        IFACEMETHOD(GetElement)(ICanvasSvgNamedElement**);

        IFACEMETHOD(get_Device)(ICanvasDevice**);

        // IAttributeInternal
        ComPtr<ID2D1SvgAttribute> GetD2DBaseAttribute() override
        {
            return GetResource();
        }
    };
}}}}}

#endif