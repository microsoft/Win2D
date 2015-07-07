// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasCommandListManager;

    struct CanvasCommandListTraits
    {
        typedef ID2D1CommandList resource_t;
        typedef CanvasCommandList wrapper_t;
        typedef ICanvasCommandList wrapper_interface_t;
        typedef CanvasCommandListManager manager_t;
    };


    class CanvasCommandList : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasCommandListTraits,
        ICanvasImage,
        CloakedIid<ICanvasImageInternal>,
        Effects::IGraphicsEffectSource)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        bool m_d2dCommandListIsClosed;

    public:
        CanvasCommandList(
            std::shared_ptr<CanvasCommandListManager> manager,
            ICanvasDevice* device,
            ID2D1CommandList* d2dCommandList);

        // ICanvasCommandList

        IFACEMETHOD(CreateDrawingSession)(
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(ICanvasDevice** value) override;

        // IClosable

        IFACEMETHOD(Close)() override;


        // ICanvasImage

        IFACEMETHOD(GetBounds)(
            ICanvasDrawingSession* drawingSession,
            Rect* bounds) override;

        IFACEMETHOD(GetBoundsWithTransform)(
            ICanvasDrawingSession* drawingSession,
            Numerics::Matrix3x2 transform,
            Rect* bounds) override;


        // ICanvasImageInternal

        virtual ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext*) override;
        virtual RealizedEffectNode GetRealizedEffectNode(ID2D1DeviceContext*, float) override;
    };


    class CanvasCommandListManager : public ResourceManager<CanvasCommandListTraits>
    {
    public:
        ComPtr<CanvasCommandList> CreateNew(
            ICanvasResourceCreator* resourceCreator);

        ComPtr<CanvasCommandList> CreateWrapper(
            ICanvasDevice* device,
            ID2D1CommandList* resource);
    };


    class CanvasCommandListFactory : public ActivationFactory<
        ICanvasCommandListFactory,
        CloakedIid<ICanvasDeviceResourceFactoryNative>>,
        public PerApplicationManager<CanvasCommandListFactory, CanvasCommandListManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ICanvasCommandList** commandList) override;

        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;
    };
}}}}
