// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;

    class CanvasRenderTargetManager;

    class CanvasRenderTargetFactory 
        : public ActivationFactory<ICanvasRenderTargetFactory, ICanvasRenderTargetStatics, CloakedIid<ICanvasDeviceResourceFactoryNative>>
        , public PerApplicationPolymorphicBitmapManager
    {
    public:
        CanvasRenderTargetFactory();
    
        IFACEMETHOD(CreateWithSize)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            Size size,
            ICanvasRenderTarget** renderTarget);

        IFACEMETHOD(CreateWithWidthAndHeight)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            float width,
            float height,
            ICanvasRenderTarget** renderTarget);

        IFACEMETHOD(CreateWithWidthAndHeightAndDpi)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            ICanvasRenderTarget** renderTarget);

        IFACEMETHOD(CreateWithWidthAndHeightAndDpiAndFormatAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha,
            ICanvasRenderTarget** renderTarget);

        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;

        //
        // ICanvasRenderTargetStatics
        //

        IFACEMETHOD(CreateFromDirect3D11Surface)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            ICanvasRenderTarget** canvasRenderTarget) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            float dpi,
            ICanvasRenderTarget** canvasRenderTarget) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            float dpi,
            CanvasAlphaMode alpha,
            ICanvasRenderTarget** canvasRenderTarget) override;
    };


    struct CanvasRenderTargetTraits
    {
        typedef ID2D1Bitmap1 resource_t;
        typedef CanvasRenderTarget wrapper_t;
        typedef ICanvasRenderTarget wrapper_interface_t;
        typedef CanvasRenderTargetManager manager_t;
    };

    class CanvasRenderTarget 
        : public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasRenderTarget,
            MixIn<CanvasRenderTarget, CanvasBitmapImpl<CanvasRenderTargetTraits>>>
        , public CanvasBitmapImpl<CanvasRenderTargetTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget, BaseTrust);

    public:
        CanvasRenderTarget(
            std::shared_ptr<CanvasRenderTargetManager> manager,
            ID2D1Bitmap1* bitmap,
            ICanvasDevice* device);

        IFACEMETHOD(CreateDrawingSession)(
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;
    };


    class CanvasRenderTargetManager : public ResourceManager<CanvasRenderTargetTraits>
    {
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> m_adapter;

    public:
        CanvasRenderTargetManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter);

        ComPtr<CanvasRenderTarget> CreateNew(
            ICanvasDevice* canvasDevice,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha);

        ComPtr<CanvasRenderTarget> CreateWrapper(
            ICanvasDevice* device,
            ID2D1Bitmap1* bitmap);

        ICanvasBitmapResourceCreationAdapter* GetAdapter();
    };
}}}}
