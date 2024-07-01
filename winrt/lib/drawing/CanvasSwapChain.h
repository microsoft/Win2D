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
    using namespace ABI::Microsoft::UI;
    using namespace WinRTDirectX;

    class CanvasSwapChainFactory
        : public AgileActivationFactory<ICanvasSwapChainFactory, ICanvasSwapChainStatics, CloakedIid<ICanvasSwapChainFactoryNative>>
        , private LifespanTracker<CanvasSwapChainFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

    public:
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
        // ICanvasSwapChainFactoryNative
        //
        IFACEMETHOD(CreateForHwnd)(
            ICanvasResourceCreator* resourceCreator,
            HWND hwnd,
            uint32_t width,
            uint32_t height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount,
            ICanvasSwapChain** canvasSwapChain) override;
    };


    class DefaultCanvasSwapChainAdapter;

    class CanvasSwapChainAdapter : public Singleton<CanvasSwapChainAdapter, DefaultCanvasSwapChainAdapter>
    {
    public:
        virtual ~CanvasSwapChainAdapter() = default;

        virtual void Sleep(DWORD timeInMs) = 0;
    };

    class DefaultCanvasSwapChainAdapter : public CanvasSwapChainAdapter
    {
    public:
        virtual void Sleep(DWORD timeInMs) override
        {
            ::Sleep(timeInMs);
        }
    };


    // IDXGISwapChain1 is used here, rather than IDXGISwapChain2, since
    // IDXGISwapChain2 is not supported on all platforms and swap chains (ie
    // a CoreWindow swap chain on Phone doesn't implement IDXGISwapChain2).

    class CanvasSwapChain : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDXGISwapChain1,
        CanvasSwapChain,
        ICanvasSwapChain,
        ICanvasResourceCreator,
        ICanvasResourceCreatorWithDpi,
        CloakedIid<ICanvasResourceWrapperWithDevice>,
        CloakedIid<ICanvasResourceWrapperWithDpi>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        bool m_isTransformMatrixSupported;
        float m_dpi;
        std::shared_ptr<CanvasSwapChainAdapter> m_adapter;
        std::shared_ptr<bool> m_hasActiveDrawingSession;

    public:
        static DirectXPixelFormat const DefaultPixelFormat = PIXEL_FORMAT(B8G8R8A8UIntNormalized);
        static int32_t const DefaultBufferCount = 2;
        static CanvasAlphaMode const DefaultCompositionAlphaMode = CanvasAlphaMode::Premultiplied;
        static CanvasAlphaMode const DefaultCoreWindowAlphaMode = CanvasAlphaMode::Ignore;

        static ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode);

        static ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            ICoreWindow* coreWindow,
            float dpi);

        static ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            ICoreWindow* coreWindow,
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount);

        static ComPtr<CanvasSwapChain> CreateNew(
            ICanvasDevice* device,
            HWND hwnd,
            uint32_t width,
            uint32_t height,
            float dpi,
            DirectXPixelFormat format,
            int32_t bufferCount);

        CanvasSwapChain(
            ICanvasDevice* device,
            IDXGISwapChain1* dxgiSwapChain,
            float dpi,
            bool isCoreWindowSwapChain);

        CanvasSwapChain(
            ICanvasDevice* device,
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
        D2DResourceLock GetResourceLock();

        DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc(D2DResourceLock const& lock);

        DXGI_MATRIX_3X2_F GetMatrixInternal(
            D2DResourceLock const& lock, 
            ComPtr<IDXGISwapChain2> const& swapChain);

        void SetMatrixInternal(
            D2DResourceLock const& lock, 
            ComPtr<IDXGISwapChain2> const& resource, 
            DXGI_MATRIX_3X2_F* transform);

        void ResizeBuffersImpl(
            D2DResourceLock const& lock,
            float newWidth,
            float newHeight,
            float newDpi,
            DirectXPixelFormat newFormat,
            int32_t bufferCount);
    };

}}}}
