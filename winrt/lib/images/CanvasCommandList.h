// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasCommandListManager;

    class CanvasCommandList : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1CommandList,
        CanvasCommandList,
        ICanvasCommandList,
        ICanvasImage,
        CloakedIid<ICanvasImageInternal>,
        CloakedIid<ICanvasResourceWrapperWithDevice>,
        Effects::IGraphicsEffectSource)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        bool m_d2dCommandListIsClosed;

    public:
        CanvasCommandList(
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


    class CanvasCommandListManager : private LifespanTracker<CanvasCommandListManager>
    {
    public:
        ComPtr<CanvasCommandList> CreateNew(
            ICanvasResourceCreator* resourceCreator);

        ComPtr<CanvasCommandList> CreateWrapper(
            ICanvasDevice* device,
            ID2D1CommandList* resource);
    };


    class CanvasCommandListFactory
        : public ActivationFactory<ICanvasCommandListFactory>
        , private LifespanTracker<CanvasCommandListFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasCommandList, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_GETMANAGER(CanvasCommandListManager)
            
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ICanvasCommandList** commandList) override;
    };
}}}}
