// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasCommandList : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1CommandList,
        CanvasCommandList,
        ICanvasCommandList,
        ICanvasImage,
        ChainInterfaces<CloakedIid<ICanvasImageInternal>, CloakedIid<ICanvasImageInterop>>,
        CloakedIid<ICanvasResourceWrapperWithDevice>,
        Effects::IGraphicsEffectSource)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        bool m_d2dCommandListIsClosed;
        bool m_hasInteropBeenUsed;
        std::shared_ptr<bool> m_hasActiveDrawingSession;

    public:
        static ComPtr<CanvasCommandList> CreateNew(
            ICanvasDevice* device);

        CanvasCommandList(
            ICanvasDevice* device,
            ID2D1CommandList* d2dCommandList,
            bool hasInteropBeenUsed = true);

        // ICanvasCommandList

        IFACEMETHOD(CreateDrawingSession)(
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(ICanvasDevice** value) override;

        // IClosable

        IFACEMETHOD(Close)() override;


        // ICanvasImage

        IFACEMETHOD(GetBounds)(
            ICanvasResourceCreator* resourceCreator,
            Rect* bounds) override;

        IFACEMETHOD(GetBoundsWithTransform)(
            ICanvasResourceCreator* resourceCreator,
            Numerics::Matrix3x2 transform,
            Rect* bounds) override;

        //
        // ICanvasImageInterop
        //

        IFACEMETHOD(GetDevice)(ICanvasDevice** device)
        {
            return get_Device(device);
        }

        // ICanvasImageInternal

        virtual ComPtr<ID2D1Image> GetD2DImage(
            ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            GetD2DImageFlags flags,
            float targetDpi,
            float* realizedDpi) override;

        // ResourceWrapper

        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** outResource) override;
    };


    class CanvasCommandListFactory
        : public AgileActivationFactory<ICanvasCommandListFactory>
        , private LifespanTracker<CanvasCommandListFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ICanvasCommandList** commandList) override;
    };
}}}}
