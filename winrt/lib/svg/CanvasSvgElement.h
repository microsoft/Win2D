// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgElement : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1SvgElement,
        CanvasSvgElement,
        ICanvasSvgElement,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgElement, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        static ComPtr<CanvasSvgElement> CreateNew(CanvasSvgDocument* parent, IStream* stream);

        CanvasSvgElement(
            ICanvasDevice* canvasDevice,
            ID2D1SvgElement* d2dSvgElement);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;

        IFACEMETHOD(AppendChild)(ICanvasSvgElement* child) override;

        // No exception boundary
        ComPtr<ICanvasDevice> GetDevice() { return m_canvasDevice.EnsureNotClosed(); }
    };

}}}}}

#endif