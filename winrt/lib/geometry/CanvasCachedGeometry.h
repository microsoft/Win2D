// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;

    class CanvasCachedGeometry : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1GeometryRealization,
        CanvasCachedGeometry,
        ICanvasCachedGeometry,
        CloakedIid<ICanvasResourceWrapperWithDevice>,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasCachedGeometry, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        // Cached fills
        static ComPtr<CanvasCachedGeometry> CreateNew(
            ICanvasDevice* device,
            ICanvasGeometry* geometry,
            float flatteningTolerance);

        // Cached strokes
        static ComPtr<CanvasCachedGeometry> CreateNew(
            ICanvasDevice* device,
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            float flatteningTolerance);

        CanvasCachedGeometry(
            ICanvasDevice* device,
            ID2D1GeometryRealization* d2dGeometryRealization);

        IFACEMETHOD(Close)();

        IFACEMETHOD(get_Device)(ICanvasDevice** device);
    };


    class CanvasCachedGeometryFactory
        : public ActivationFactory<ICanvasCachedGeometryStatics>
        , private LifespanTracker<CanvasCachedGeometryFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasCachedGeometry, BaseTrust);

    public:
        IFACEMETHOD(CreateFill)(
            ICanvasGeometry* geometry,
            ICanvasCachedGeometry** cachedGeometry) override;

        IFACEMETHOD(CreateFillWithFlatteningTolerance)(
            ICanvasGeometry* geometry,
            float flatteningTolerance,
            ICanvasCachedGeometry** cachedGeometry) override;

        IFACEMETHOD(CreateStroke)(
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasCachedGeometry** cachedGeometry) override;

        IFACEMETHOD(CreateStrokeWithStrokeStyle)(
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            ICanvasCachedGeometry** cachedGeometry) override;

        IFACEMETHOD(CreateStrokeWithStrokeStyleAndFlatteningTolerance)(
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            float flatteningTolerance,
            ICanvasCachedGeometry** cachedGeometry) override;

    private:
        void CreateStrokeImpl(
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            float flatteningTolerance,
            ICanvasCachedGeometry** cachedGeometry);
    };
}}}}}
