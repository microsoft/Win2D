// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::UI;
    using namespace WinRTDirectX;

    class CanvasSwapChainManager;

    class CanvasSwapChainFactory
        : public ActivationFactory<
        ICanvasSwapChainFactory,
        ICanvasSwapChainStatics,
        CloakedIid<ICanvasDeviceResourceWithDpiFactoryNative>> ,
        public PerApplicationManager<CanvasSwapChainFactory, CanvasSwapChainManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

    public:
        CanvasSwapChainFactory();

        //
        // ICanvasSwapChainFactory
        //

        IFACEMETHOD(CreateWithSize)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            Size size,
            ICanvasSwapChain** swapChain) override;

        IFACEMETHOD(CreateWithWidthAndHeight)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            float width,
            float height,
            ICanvasSwapChain** swapChain) override;

        IFACEMETHOD(CreateWithWidthAndHeightAndDpi)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            ICanvasSwapChain** swapChain) override;

        IFACEMETHOD(CreateWithAllOptions)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode,
            ICanvasSwapChain** swapChain) override;

        //
        // ICanvasSwapChainStatics
        //

        IFACEMETHOD(CreateForCoreWindowWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            ICoreWindow* coreWindow,
            float dpi,
            ICanvasSwapChain** swapChain);

        IFACEMETHOD(CreateForCoreWindowWithAllOptions)(
            ICanvasResourceCreator* resourceCreator,
            ICoreWindow* coreWindow,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount,
            ICanvasSwapChain** swapChain);

        //
        // ICanvasDeviceResourceWithDpiFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            float dpi,
            IInspectable** wrapper) override;
    };

    struct CanvasSwapChainTraits
    {
        typedef IDXGISwapChain1 resource_t;
        typedef CanvasSwapChain wrapper_t;
        typedef ICanvasSwapChain wrapper_interface_t;
        typedef CanvasSwapChainManager manager_t;

        // IDXGISwapChain1 is used here, rather than IDXGISwapChain2, since
        // IDXGISwapChain2 is not supported on all platforms and swap chains (ie
        // a CoreWindow swap chain on Phone doesn't implement IDXGISwapChain2).
    };

    class ICanvasSwapChainAdapter
    {
    public:
        virtual ~ICanvasSwapChainAdapter() = default;

        virtual void Sleep(DWORD timeInMs) = 0;
    };

    class CanvasSwapChain : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasSwapChainTraits,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        float m_dpi;
        std::shared_ptr<ICanvasSwapChainAdapter> m_adapter;

    public:
        static DirectXPixelFormat const DefaultPixelFormat = PIXEL_FORMAT(B8G8R8A8UIntNormalized);
        static int32_t const DefaultBufferCount = 2;
        static CanvasAlphaMode const DefaultCompositionAlphaMode = CanvasAlphaMode::Premultiplied;
        static CanvasAlphaMode const DefaultCoreWindowAlphaMode = CanvasAlphaMode::Ignore;

        CanvasSwapChain(
            ICanvasDevice* device,
            std::shared_ptr<CanvasSwapChainManager> swapChainManager,
            std::shared_ptr<ICanvasSwapChainAdapter> swapChainAdapter,
            IDXGISwapChain1* dxgiSwapChain,
            float dpi);

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override;

        // ICanvasSwapChain
        IFACEMETHOD(get_Device)(ICanvasDevice** value) override;
        IFACEMETHOD(get_Size)(Size* value) override;
        IFACEMETHOD(get_SizeInPixels)(BitmapSize* value) override;
        IFACEMETHOD(get_Dpi)(float* value) override;
        IFACEMETHOD(get_Format)(DirectXPixelFormat* value) override;
        IFACEMETHOD(get_BufferCount)(int32_t* value) override;
        IFACEMETHOD(get_AlphaMode)(CanvasAlphaMode* value) override;
        
        IFACEMETHOD(get_Rotation)(CanvasSwapChainRotation* value) override;
        IFACEMETHOD(put_Rotation)(CanvasSwapChainRotation value) override;

        IFACEMETHOD(get_SourceSize)(Size* value) override;
        IFACEMETHOD(put_SourceSize)(Size value) override;

        IFACEMETHOD(get_TransformMatrix)(Matrix3x2* value) override;
        IFACEMETHOD(put_TransformMatrix)(Matrix3x2 value) override;

        IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips) override;
        IFACEMETHODIMP ConvertDipsToPixels(float dips, CanvasDpiRounding dpiRounding, int* pixels) override;

        IFACEMETHOD(Present)() override;
        IFACEMETHOD(PresentWithSyncInterval)(int32_t syncInterval) override;

        IFACEMETHOD(ResizeBuffersWithSize)(
            Size newSize) override;

        IFACEMETHOD(ResizeBuffersWithWidthAndHeight)(
            float newWidth,
            float newHeight) override;

        IFACEMETHOD(ResizeBuffersWithWidthAndHeightAndDpi)(
            float newWidth,
            float newHeight,
            float newDpi) override;

        IFACEMETHOD(ResizeBuffersWithAllOptions)(
            float newWidth,
            float newHeight,
            float newDpi,
            DirectXPixelFormat newFormat,
            int32_t bufferCount) override;

        IFACEMETHOD(WaitForVerticalBlank)() override;

        // IClosable
        IFACEMETHOD(Close)() override;

    private:
        DXGI_SWAP_CHAIN_DESC1 GetResourceDescription(); // Expected to be called from exception boundary.

    };
    
    class CanvasSwapChainManager : public ResourceManager<CanvasSwapChainTraits>
    {
    protected:
        std::shared_ptr<ICanvasSwapChainAdapter> m_adapter;

    public:
        CanvasSwapChainManager();

        virtual ~CanvasSwapChainManager() = default;

        ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode);

        ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            ICoreWindow* coreWindow,
            float dpi);

        ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            ICoreWindow* coreWindow,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount);

        virtual ComPtr<CanvasSwapChain> CreateWrapper(
            ICanvasDevice* device,
            IDXGISwapChain1* resource,
            float dpi);
    };

}}}}
