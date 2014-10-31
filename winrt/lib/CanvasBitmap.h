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
#include "TextureUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11;
    using namespace ABI::Microsoft::Graphics::Canvas::Effects;
    using namespace ABI::Windows::Storage;
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Foundation;

    class CanvasBitmapManager;

    class ICanvasBitmapResourceCreationAdapter
    {
    public:
        virtual ComPtr<IWICFormatConverter> CreateWICFormatConverter(HSTRING fileName) = 0;
        virtual ComPtr<IWICFormatConverter> CreateWICFormatConverter(IStream* fileStream) = 0;

        virtual void SaveLockedMemoryToFile(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapLock* bitmapLock) = 0;

        virtual void SaveLockedMemoryToStream(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapLock* bitmapLock) = 0;
    };
    

    [uuid(4684FA78-C721-4531-8CCE-BEA927F95E5D)]
    class ICanvasBitmapInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Bitmap1> const& GetD2DBitmap() = 0;
    };

    class ICanvasBitmapAdapter
    {
    public:
        virtual ComPtr<IRandomAccessStreamReference> CreateRandomAccessStreamFromUri(IUriRuntimeClass* uri) = 0;
        virtual ComPtr<IAsyncOperation<StorageFile*>> GetFileFromPathAsync(HSTRING path) = 0;
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
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytesWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColors)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            CanvasAlphaBehavior alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColorsWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
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

        IFACEMETHOD(LoadAsyncFromStream)(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromStreamWithAlpha)(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
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

    void GetBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        uint8_t** valueElements);

    void GetColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        Color **valueElements);

    void SaveBitmapToFileImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        HSTRING rawfileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction);

    void SaveBitmapToStreamImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction);

    void SetBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        uint8_t* valueElements);

    void SetColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        Color *valueElements);

    HRESULT CopyFromBitmapImpl(
        ICanvasBitmap* to,
        ICanvasBitmap* from,
        int32_t* destX = nullptr,
        int32_t* destY = nullptr,
        int32_t* sourceRectLeft = nullptr,
        int32_t* sourceRectTop = nullptr,
        int32_t* sourceRectWidth = nullptr,
        int32_t* sourceRectHeight = nullptr);

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
        CanvasBitmapImpl(
            std::shared_ptr<typename TRAITS::manager_t> manager, 
            ID2D1Bitmap1* resource)
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
        virtual ComPtr<ID2D1Bitmap1> const& GetD2DBitmap() override
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

        HRESULT STDMETHODCALLTYPE SaveToFileAsync(
            HSTRING fileName,
            IAsyncAction **resultAsyncAction) override
        {
            return SaveToFileWithBitmapFileFormatAndQualityAsync(
                fileName,
                CanvasBitmapFileFormat::Auto,
                DEFAULT_CANVASBITMAP_QUALITY,
                resultAsyncAction);
        }        

        HRESULT STDMETHODCALLTYPE SaveToFileWithBitmapFileFormatAsync(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            IAsyncAction **resultAsyncAction) override
        {
            return SaveToFileWithBitmapFileFormatAndQualityAsync(
                fileName,
                fileFormat,
                DEFAULT_CANVASBITMAP_QUALITY,
                resultAsyncAction);
        }

        HRESULT STDMETHODCALLTYPE SaveToFileWithBitmapFileFormatAndQualityAsync(
            HSTRING rawfileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            IAsyncAction **resultAsyncAction) override
        {
            return ExceptionBoundary(
                [=]
                {
                    CheckInPointer(rawfileName);
                    CheckAndClearOutPointer(resultAsyncAction);

                    auto& d2dBitmap = GetResource();

                    SaveBitmapToFileImpl(
                        d2dBitmap.Get(), 
                        Manager()->GetAdapter(),
                        rawfileName, 
                        fileFormat,
                        quality,
                        resultAsyncAction);
                });
        }

        HRESULT STDMETHODCALLTYPE SaveToStreamAsync(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            IAsyncAction** asyncAction) override
        {
            return SaveToStreamWithQualityAsync(
                stream,
                fileFormat,
                DEFAULT_CANVASBITMAP_QUALITY,
                asyncAction);
        }

        HRESULT STDMETHODCALLTYPE SaveToStreamWithQualityAsync(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            IAsyncAction** asyncAction) override
        {
            return ExceptionBoundary(
                [=]
                {
                    CheckInPointer(stream);
                    CheckAndClearOutPointer(asyncAction);

                    auto& d2dBitmap = GetResource();

                    SaveBitmapToStreamImpl(
                        d2dBitmap.Get(), 
                        Manager()->GetAdapter(),
                        stream,
                        fileFormat,
                        quality,
                        asyncAction);
                });
        }

        HRESULT STDMETHODCALLTYPE GetBytes(
            uint32_t* valueCount,
            uint8_t** valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetBytesImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE GetBytesWithSubrectangle(
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height,
            uint32_t* valueCount,
            uint8_t** valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetBytesImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE GetColors(
            uint32_t* valueCount,
            ABI::Windows::UI::Color **valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetColorsImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE GetColorsWithSubrectangle(
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height,
            uint32_t* valueCount,
            ABI::Windows::UI::Color **valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetColorsImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE SetBytes(
            uint32_t valueCount,
            uint8_t* valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetBytesImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE SetBytesWithSubrectangle(
            uint32_t valueCount,
            uint8_t* valueElements,
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetBytesImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE SetColors(
            uint32_t valueCount,
            ABI::Windows::UI::Color* valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetColorsImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE SetColorsWithSubrectangle(
            uint32_t valueCount,
            ABI::Windows::UI::Color* valueElements,
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetColorsImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        HRESULT STDMETHODCALLTYPE GetBounds(
            ICanvasDrawingSession *drawingSession,
            Rect *bounds) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(drawingSession);
                    CheckInPointer(bounds);

                    Numerics::Matrix3x2 identity = { 1, 0, 0, 1, 0, 0 };
                    *bounds = GetImageBoundsImpl(this, drawingSession, identity);
                });
        }

        HRESULT STDMETHODCALLTYPE GetBoundsWithTransform(
            ICanvasDrawingSession *drawingSession,
            Numerics::Matrix3x2 transform,
            Rect *bounds) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(drawingSession);
                    CheckInPointer(bounds);

                    *bounds = GetImageBoundsImpl(this, drawingSession, transform);
                });
        }

        HRESULT STDMETHODCALLTYPE CopyFromBitmap(
            ICanvasBitmap* otherBitmap)
        {
            return CopyFromBitmapImpl(
                this,
                otherBitmap);
        }

        HRESULT STDMETHODCALLTYPE CopyFromBitmapWithDestPoint(
            ICanvasBitmap* otherBitmap,
            int32_t destX,
            int32_t destY)
        {
            return CopyFromBitmapImpl(
                this,
                otherBitmap,
                &destX,
                &destY);
        }

        HRESULT STDMETHODCALLTYPE CopyFromBitmapWithDestPointAndSourceRect(
            ICanvasBitmap* otherBitmap,
            int32_t destX,
            int32_t destY,
            int32_t sourceRectLeft,
            int32_t sourceRectTop,
            int32_t sourceRectWidth,
            int32_t sourceRectHeight)
        {      
            return CopyFromBitmapImpl(
                this, 
                otherBitmap, 
                &destX, 
                &destY, 
                &sourceRectLeft, 
                &sourceRectTop, 
                &sourceRectWidth, 
                &sourceRectHeight);
        }

    private:

        D2D1_RECT_U GetResourceBitmapExtents(ComPtr<ID2D1Bitmap1> const d2dBitmap)
        {
            const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
            return D2D1::RectU(0, 0, size.width, size.height);
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

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* device,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            float dpi);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* device,
            uint32_t colorCount,
            Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            CanvasAlphaBehavior alpha,
            float dpi);

        ComPtr<CanvasBitmap> CreateWrapper(
            ID2D1Bitmap1* bitmap);

        ICanvasBitmapResourceCreationAdapter* GetAdapter();
    };
}}}}
