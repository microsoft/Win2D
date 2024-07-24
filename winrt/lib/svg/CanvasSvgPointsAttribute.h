// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "AttributeHelpers.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgPointsAttribute : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgPointsAttribute,
            ICanvasSvgAttribute,
            CloakedIid<AttributeHelpers::IAttributeInternal>,
            ChainInterfaces<
                MixIn<CanvasSvgPointsAttribute, ResourceWrapper<ID2D1SvgPointCollection, CanvasSvgPointsAttribute, ICanvasSvgPointsAttribute>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgPointCollection, CanvasSvgPointsAttribute, ICanvasSvgPointsAttribute>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgPointsAttribute, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasSvgPointsAttribute(
            ICanvasDevice* canvasDevice,
            ID2D1SvgPointCollection* d2dSvgPoints);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Points)(uint32_t*, Vector2**);
        IFACEMETHOD(GetPoints)(int32_t, int32_t, uint32_t*, Vector2**);

        IFACEMETHOD(RemovePointsAtEnd)(int32_t);

        IFACEMETHOD(SetPoints)(int32_t, uint32_t, Vector2*);

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