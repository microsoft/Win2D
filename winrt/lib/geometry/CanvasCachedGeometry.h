// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
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
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCachedGeometry, BaseTrust);

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
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCachedGeometry, BaseTrust);

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
}}}}
