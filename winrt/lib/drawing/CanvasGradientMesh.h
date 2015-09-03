// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace Numerics;

    class CanvasGradientMesh : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1GradientMesh,
        CanvasGradientMesh,
        ICanvasGradientMesh,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasGradientMesh, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        static ComPtr<CanvasGradientMesh> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            uint32_t patchCount,
            CanvasGradientMeshPatch* patchElements);

        CanvasGradientMesh(
            ICanvasDevice* canvasDevice,
            ID2D1GradientMesh* d2dGradientMesh);
        
        IFACEMETHOD(get_Patches)(
            uint32_t* valueCount,
            CanvasGradientMeshPatch** valueElements);

        IFACEMETHOD(GetBounds)(
            ICanvasDrawingSession* drawingSession,
            Rect* bounds);

        IFACEMETHOD(Close)();

        IFACEMETHOD(get_Device)(ICanvasDevice** device);
    };

    class CanvasGradientMeshFactory
        : public ActivationFactory<ICanvasGradientMeshStatics, ICanvasGradientMeshFactory>
        , private LifespanTracker<CanvasGradientMeshFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasGradientMesh, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t patchCount,
            CanvasGradientMeshPatch* patchElements,
            ICanvasGradientMesh** canvasGradientMesh) override;

        IFACEMETHOD(CreateCoonsPatch)(
            uint32_t pointCount,
            Vector2* pointElements,
            uint32_t colorCount,
            Color* colorElements,
            uint32_t edgeCount,
            CanvasGradientMeshPatchEdge* edgeElements,
            CanvasGradientMeshPatch* gradientMeshPatch) override;

        IFACEMETHOD(CreateTensorPatch)(
            uint32_t pointCount,
            Vector2* pointElements,
            uint32_t colorCount,
            Color* colorElements,
            uint32_t edgeCount,
            CanvasGradientMeshPatchEdge* edgeElements,
            CanvasGradientMeshPatch* gradientMeshPatch) override;

        static D2D1_GRADIENT_MESH_PATCH PatchToD2DPatch(CanvasGradientMeshPatch const& patch);
        static CanvasGradientMeshPatch PatchFromD2DPatch(D2D1_GRADIENT_MESH_PATCH const& d2dPatch);
    };
}}}}}

#endif