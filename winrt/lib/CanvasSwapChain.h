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
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL;

    class ICanvasSwapChainDrawingSessionFactory
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            IDXGISwapChain2* swapChainResource,
            Color const& clearColor) const = 0;
    };

    class CanvasSwapChainManager;

    class CanvasSwapChainFactory
        : public ActivationFactory<
        ICanvasSwapChainFactory,
        CloakedIid<ICanvasDeviceResourceFactoryNative>> ,
        public PerApplicationManager<CanvasSwapChainFactory, CanvasSwapChainManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

        std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasSwapChainFactory();

        IFACEMETHOD(CreateWithSize)(
            ICanvasResourceCreator* resourceCreator,
            int32_t widthInPixels,
            int32_t heightInPixels,
            ICanvasSwapChain** SwapChain) override;

        IFACEMETHOD(CreateWithSizeAndFormat)(
            ICanvasResourceCreator* resourceCreator,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            ICanvasSwapChain** SwapChain) override;

        IFACEMETHOD(CreateWithAllOptions)(
            ICanvasResourceCreator* resourceCreator,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaBehavior alphaBehavior,
            ICanvasSwapChain** SwapChain) override;

        //
        // ICanvasDeviceResourceFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;
    };

    struct CanvasSwapChainTraits
    {
        typedef IDXGISwapChain2 resource_t;
        typedef CanvasSwapChain wrapper_t;
        typedef ICanvasSwapChain wrapper_interface_t;
        typedef CanvasSwapChainManager manager_t;
    };

    class CanvasSwapChain : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasSwapChainTraits,
        ICanvasResourceCreator,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;
        std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasSwapChain(
            ICanvasResourceCreator* resourceCreator,
            std::shared_ptr<CanvasSwapChainManager> swapChainManager,
            std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> drawingSessionFactory,
            IDXGISwapChain2* dxgiSwapChain);

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override;

        // ICanvasSwapChain

        IFACEMETHOD(get_Width)(int32_t* value) override;

        IFACEMETHOD(get_Height)(int32_t* value) override;

        IFACEMETHOD(get_Format)(DirectXPixelFormat* value) override;

        IFACEMETHOD(get_BufferCount)(int32_t* value) override;

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaBehavior* value) override;

        IFACEMETHOD(Present)() override;

        IFACEMETHOD(ResizeBuffers)(
            int32_t bufferCount,
            int32_t newWidth,
            int32_t newHeight,
            DirectXPixelFormat newFormat) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasResourceCreator
        IFACEMETHOD(get_Device)(ICanvasDevice** value) override;

    private:
        DXGI_SWAP_CHAIN_DESC1 GetResourceDescription(); // Expected to be called from exception boundary.

    };
    
    class CanvasSwapChainManager : public ResourceManager<CanvasSwapChainTraits>
    {
    public:
        ComPtr<CanvasSwapChain> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaBehavior alphaBehavior,
            std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> const& drawingSessionFactory);

        ComPtr<CanvasSwapChain> CreateWrapper(
            ICanvasDevice* device,
            IDXGISwapChain2* resource);
    };

    //
    // Drawing session factory
    //    
    class CanvasDrawingSessionManager;

    class CanvasSwapChainDrawingSessionFactory : public ICanvasSwapChainDrawingSessionFactory
    {
        std::shared_ptr<CanvasDrawingSessionManager> m_drawingSessionManager;

    public:
        CanvasSwapChainDrawingSessionFactory();

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            IDXGISwapChain2* swapChainResource,
            Color const& clearColor) const override;
    };

}}}}
