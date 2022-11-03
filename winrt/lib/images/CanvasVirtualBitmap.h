// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasVirtualBitmapFactory :
        public AgileActivationFactory<ICanvasVirtualBitmapStatics>,
        private LifespanTracker<CanvasVirtualBitmapFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasVirtualBitmap, BaseTrust);

    public:
        IFACEMETHODIMP LoadAsyncFromFileName(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromFileNameWithOptions(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            CanvasVirtualBitmapOptions options,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromFileNameWithOptionsAndAlpha(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            CanvasVirtualBitmapOptions options,
            CanvasAlphaMode alphaMode,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromUri(
            ICanvasResourceCreator* resourceCreator,
            IUriRuntimeClass* uri,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromUriWithOptions(
            ICanvasResourceCreator* resourceCreator,
            IUriRuntimeClass* uri,
            CanvasVirtualBitmapOptions options,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromUriWithOptionsAndAlpha(
            ICanvasResourceCreator* resourceCreator,
            IUriRuntimeClass* uri,
            CanvasVirtualBitmapOptions options,
            CanvasAlphaMode alphaMode,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromStream(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromStreamWithOptions(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            CanvasVirtualBitmapOptions options,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;

        IFACEMETHODIMP LoadAsyncFromStreamWithOptionsAndAlpha(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            CanvasVirtualBitmapOptions options,
            CanvasAlphaMode alphaMode,
            IAsyncOperation<CanvasVirtualBitmap*>** result) override;
    };
    
    //
    // CanvasVirtualBitmap needs to wrap either ID2D1ImageSourceFromWic or
    // ID2D1TransformedImageSource.
    //
    // The highest common interface these have is ID2D1Image (since
    // ID2D1TransformedImageSource doesn't derive from ID2D1ImageSource, as you
    // might naively expect).
    //

    class CanvasVirtualBitmap : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1Image,
        CanvasVirtualBitmap,
        ICanvasVirtualBitmap,
        ICanvasImage,
        IGraphicsEffectSource,
        CloakedIid<ICanvasImageInternal>,
        CloakedIid<ICanvasResourceWrapperWithDevice>)        
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasVirtualBitmap, BaseTrust);

        ComPtr<ICanvasDevice> m_device;
        ComPtr<ID2D1ImageSourceFromWic> m_imageSourceFromWic;
        Rect m_localBounds;
        D2D1_ORIENTATION m_orientation;
        
    public:
        static ComPtr<CanvasVirtualBitmap> CreateNew(
            ComPtr<ICanvasResourceCreator> const& resourceCreator,
            WicBitmapSource const& source,
            CanvasVirtualBitmapOptions options,
            CanvasAlphaMode alphaMode);
        
        CanvasVirtualBitmap(
            ICanvasDevice* device,
            ID2D1Image* imageSource);

        CanvasVirtualBitmap(
            ICanvasDevice* device,
            ID2D1Image* imageSource,
            ID2D1ImageSourceFromWic* imageSourceFromWic,
            Rect localBounds,
            D2D1_ORIENTATION orientation);

        // IClosable
        IFACEMETHODIMP Close() override;
        
        // ICanvasVirtualBitmap
        IFACEMETHODIMP get_Device(ICanvasDevice** result) override;
        IFACEMETHODIMP get_IsCachedOnDemand(boolean* value) override;
        IFACEMETHODIMP get_SizeInPixels(BitmapSize* value) override;
        IFACEMETHODIMP get_Size(Size* value) override;
        IFACEMETHODIMP get_Bounds(Rect* value) override;

        // ICanvasImage
        IFACEMETHODIMP GetBounds(ICanvasResourceCreator*, Rect*) override;
        IFACEMETHODIMP GetBoundsWithTransform(ICanvasResourceCreator*, Matrix3x2, Rect*) override;

        //
        // ICanvasImageInterop
        //

        IFACEMETHOD(GetDevice)(ICanvasDevice** device)
        {
            return get_Device(device);
        }

        // ICanvasImageInternal
        ComPtr<ID2D1Image> GetD2DImage(ICanvasDevice* , ID2D1DeviceContext*, GetD2DImageFlags, float, float*) override;
    };

}}}}

#endif