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

#include "CanvasImage.h"
#include "PolymorphicBitmapmanager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11;
    using namespace ABI::Microsoft::Graphics::Canvas::Effects;
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Foundation;

    class CanvasBitmapManager;

    class ICanvasBitmapResourceCreationAdapter
    {
    public:
        virtual ComPtr<IWICFormatConverter> CreateWICFormatConverter(HSTRING fileName) = 0;
        virtual ComPtr<IWICFormatConverter> CreateWICFormatConverter(IStream* fileStream) = 0;
    };
    

    [uuid(4684FA78-C721-4531-8CCE-BEA927F95E5D)]
    class ICanvasBitmapInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Bitmap1> GetD2DBitmap() = 0;
    };

    class ICanvasBitmapAdapter
    {
    public:
        virtual ComPtr<IRandomAccessStreamReference> CreateRandomAccessStreamFromUri(ComPtr<IUriRuntimeClass> const& uri) = 0;
    };

    class CanvasBitmapFactory :
        public ActivationFactory<
            ICanvasBitmapFactory, 
            ICanvasBitmapStatics,
            CloakedIid<ICanvasDeviceResourceFactoryNative>>,
        public PerApplicationPolymorphicBitmapManager
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

        std::shared_ptr<ICanvasBitmapAdapter> m_adapter;

    public:
        CanvasBitmapFactory();

        //
        // ICanvasBitmapStatics
        //

        IFACEMETHOD(CreateFromDirect3D11Surface)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithAlpha)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            CanvasAlphaBehavior alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithAlphaAndDpi)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            CanvasAlphaBehavior alpha,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytes)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytesWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColors)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            float width,
            float height,
            CanvasAlphaBehavior alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColorsWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            float width,
            float height,
            CanvasAlphaBehavior alpha,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(LoadAsyncFromHstring)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromHstringWithAlpha)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            CanvasAlphaBehavior alpha,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromUri)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::IUriRuntimeClass* uri,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromUriWithAlpha)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::IUriRuntimeClass* uri,
            CanvasAlphaBehavior alpha,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        //
        // ICanvasDeviceResourceFactoryNative
        //
        
        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;
    };


    struct CanvasBitmapTraits
    {
        typedef ID2D1Bitmap1 resource_t;
        typedef CanvasBitmap wrapper_t;
        typedef ICanvasBitmap wrapper_interface_t;
        typedef CanvasBitmapManager manager_t;
    };


    template<typename TRAITS>
    class CanvasBitmapImpl 
        : public Implements<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasBitmap,
        ICanvasImage,
        IEffectInput,
        IDirect3DSurface,
        CloakedIid<ICanvasImageInternal>,
        CloakedIid<ICanvasBitmapInternal>,
        CloakedIid<IDXGIInterfaceAccess>,
        ChainInterfaces<MixIn<CanvasBitmapImpl<TRAITS>, ResourceWrapper<TRAITS>>, ABI::Windows::Foundation::IClosable, CloakedIid<ICanvasResourceWrapperNative>>>
        , public ResourceWrapper<TRAITS>
    {
    protected:
        CanvasBitmapImpl(std::shared_ptr<typename TRAITS::manager_t> manager, ID2D1Bitmap1* resource)
            : ResourceWrapper(manager, resource)
        {}

    public:
        IFACEMETHODIMP get_SizeInPixels(_Out_ ABI::Windows::Foundation::Size* size) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(size);
                    
                    auto& resource = GetResource();
                    D2D1_SIZE_U d2dSize = resource->GetPixelSize();
                    size->Height = static_cast<float>(d2dSize.height);
                    size->Width = static_cast<float>(d2dSize.width);
                });
        }
        
        IFACEMETHODIMP get_Size(_Out_ ABI::Windows::Foundation::Size* size) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(size);

                    auto& resource = GetResource();
                    D2D1_SIZE_F d2dSize = resource->GetSize();
                    size->Height = d2dSize.height;
                    size->Width = d2dSize.width;
                });
        }

        IFACEMETHODIMP get_Bounds(_Out_ ABI::Windows::Foundation::Rect* bounds) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(bounds);

                    auto& resource = GetResource();
                    D2D1_SIZE_F d2dSize = resource->GetSize();
                    bounds->X = 0;
                    bounds->Y = 0;
                    bounds->Width = d2dSize.width;
                    bounds->Height = d2dSize.height;
                });
        }

        // IDirect3DSurface
        IFACEMETHODIMP get_Description(Direct3DSurfaceDescription* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);

                    auto& d2dBitmap = GetResource();
                    ComPtr<IDXGISurface> dxgiSurface;
                    ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));                    
                    ThrowIfFailed(dxgiSurface->GetDesc(ReinterpretAs<DXGI_SURFACE_DESC*>(value)));
                });
        }

        // ICanvasImageInternal
        virtual ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext, uint64_t* realizationId) override
        {
            CheckInPointer(deviceContext);

            if (realizationId)
                *realizationId = 0;

            return GetResource();
        }

        // ICanvasBitmapInternal
        virtual ComPtr<ID2D1Bitmap1> GetD2DBitmap() override
        {
            return GetResource();
        }

        // IDXGIInterfaceAccess
        IFACEMETHODIMP GetDXGIInterface(REFIID iid, void** p)
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();
                    ComPtr<IDXGISurface> dxgiSurface;
                    ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));
                    ThrowIfFailed(dxgiSurface.CopyTo(iid, p));
                });
        }
    };


    class CanvasBitmap :
        public RuntimeClass<                                    
            RuntimeClassFlags<WinRtClassicComMix>,              
            MixIn<CanvasBitmap, CanvasBitmapImpl<CanvasBitmapTraits>>>
        , public CanvasBitmapImpl<CanvasBitmapTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

    public:
        CanvasBitmap(
            std::shared_ptr<CanvasBitmapManager> manager,
            ID2D1Bitmap1* bitmap);
    };


    class CanvasBitmapManager : public ResourceManager<CanvasBitmapTraits>
    {
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> m_adapter;

    public:
        CanvasBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* canvasDevice, 
            HSTRING fileName,
            CanvasAlphaBehavior alpha);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* canvasDevice,
            IStream* fileStream,
            CanvasAlphaBehavior alpha);

        ComPtr<CanvasBitmap> CreateWrapper(
            ID2D1Bitmap1* bitmap);
    };


    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(U* obj)
    {
        ComPtr<T> dxgiInterface;
        ThrowIfFailed(As<IDXGIInterfaceAccess>(obj)->GetDXGIInterface(IID_PPV_ARGS(&dxgiInterface)));
        return dxgiInterface;
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(ComPtr<U> obj)
    {
        return GetDXGIInterface<T, U>(obj.Get());
    }
}}}}
