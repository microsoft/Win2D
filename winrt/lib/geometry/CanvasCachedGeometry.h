// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;

    class CanvasCachedGeometry;
    class CanvasCachedGeometryManager;

    struct CanvasCachedGeometryTraits
    {
        typedef ID2D1GeometryRealization resource_t;
        typedef CanvasCachedGeometry wrapper_t;
        typedef ICanvasCachedGeometry wrapper_interface_t;
        typedef CanvasCachedGeometryManager manager_t;
    };

    class CanvasCachedGeometry : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasCachedGeometryTraits,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasCachedGeometry, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasCachedGeometry(
            std::shared_ptr<CanvasCachedGeometryManager> manager,
            ID2D1GeometryRealization* d2dGeometryRealization,
            ComPtr<ICanvasDevice> const& device);

        IFACEMETHOD(Close)();

        IFACEMETHOD(get_Device)(ICanvasDevice** device);
    };


    class CanvasCachedGeometryManager : public ResourceManager<CanvasCachedGeometryTraits>
    {
    public:
        // Cached fills
        ComPtr<CanvasCachedGeometry> CreateNew(
            ICanvasDevice* device,
            ICanvasGeometry* geometry,
            float flatteningTolerance);

        // Cached strokes
        ComPtr<CanvasCachedGeometry> CreateNew(
            ICanvasDevice* device,
            ICanvasGeometry* geometry,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            float flatteningTolerance);

        ComPtr<CanvasCachedGeometry> CreateWrapper(
            ICanvasDevice* device,
            ID2D1GeometryRealization* resource);
    };

    class CanvasCachedGeometryFactory
        : public ActivationFactory<
            ICanvasCachedGeometryStatics,
            CloakedIid<ICanvasDeviceResourceFactoryNative>> ,
            public PerApplicationManager<CanvasCachedGeometryFactory, CanvasCachedGeometryManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasCachedGeometry, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASDEVICERESOURCEFACTORYNATIVE();

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
