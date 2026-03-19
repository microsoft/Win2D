// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "AttributeHelpers.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgStrokeDashArrayAttribute : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgStrokeDashArrayAttribute,
            ICanvasSvgAttribute,
            CloakedIid<AttributeHelpers::IAttributeInternal>,
            ChainInterfaces<
                MixIn<CanvasSvgStrokeDashArrayAttribute, ResourceWrapper<ID2D1SvgStrokeDashArray, CanvasSvgStrokeDashArrayAttribute, ICanvasSvgStrokeDashArrayAttribute>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgStrokeDashArray, CanvasSvgStrokeDashArrayAttribute, ICanvasSvgStrokeDashArrayAttribute>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgStrokeDashArrayAttribute, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasSvgStrokeDashArrayAttribute(
            ICanvasDevice* canvasDevice,
            ID2D1SvgStrokeDashArray* d2dSvgStrokeDashArray);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(GetDashes)(uint32_t*, float**);
        IFACEMETHOD(GetDashesWithUnits)(int32_t, int32_t, uint32_t*, CanvasSvgLengthUnits**, uint32_t*, float**);

        IFACEMETHOD(RemoveDashesAtEnd)(int32_t);

        IFACEMETHOD(SetDashes)(int32_t, uint32_t, float*);
        IFACEMETHOD(SetDashesWithUnit)(int32_t, uint32_t, float*, CanvasSvgLengthUnits);
        IFACEMETHOD(SetDashesWithUnits)(int32_t, uint32_t, float*, uint32_t, CanvasSvgLengthUnits*);

        IFACEMETHOD(Clone)(ICanvasSvgAttribute**);

        IFACEMETHOD(GetElement)(ICanvasSvgNamedElement**);

        IFACEMETHOD(get_Device)(ICanvasDevice**);

        // IAttributeInternal
        ComPtr<ID2D1SvgAttribute> GetD2DBaseAttribute() override
        {
            return GetResource();
        }

        static
        std::vector<D2D1_SVG_LENGTH> GetD2DSvgLengths(uint32_t dashCount, float* dashValues, CanvasSvgLengthUnits* dashUnits);
    };
}}}}}