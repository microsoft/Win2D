// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "AttributeHelpers.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgPathAttribute : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgPathAttribute,
            ICanvasSvgAttribute,
            CloakedIid<AttributeHelpers::IAttributeInternal>,
            ChainInterfaces<
                MixIn<CanvasSvgPathAttribute, ResourceWrapper<ID2D1SvgPathData, CanvasSvgPathAttribute, ICanvasSvgPathAttribute>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgPathData, CanvasSvgPathAttribute, ICanvasSvgPathAttribute>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgPathAttribute, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasSvgPathAttribute(
            ICanvasDevice* canvasDevice,
            ID2D1SvgPathData* d2dSvgPath);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(CreatePathGeometry)(ICanvasGeometry**);
        IFACEMETHOD(CreatePathGeometryWithFill)(CanvasFilledRegionDetermination, ICanvasGeometry**);

        IFACEMETHOD(get_Commands)(uint32_t*, CanvasSvgPathCommand**);
        IFACEMETHOD(GetCommands)(int32_t, int32_t, uint32_t*, CanvasSvgPathCommand**);

        IFACEMETHOD(get_SegmentData)(uint32_t*, float**);
        IFACEMETHOD(GetSegmentData)(int32_t, int32_t, uint32_t*, float**);

        IFACEMETHOD(RemoveCommandsAtEnd)(int32_t);
        IFACEMETHOD(RemoveSegmentDataAtEnd)(int32_t);

        IFACEMETHOD(SetCommands)(int32_t, uint32_t, CanvasSvgPathCommand*);
        IFACEMETHOD(SetSegmentData)(int32_t, uint32_t, float*);

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