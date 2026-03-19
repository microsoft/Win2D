// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes 
{
    using namespace ::Microsoft::WRL;

    class CanvasImageBrushFactory
        : public AgileActivationFactory<ICanvasImageBrushFactory>
        , private LifespanTracker<CanvasImageBrushFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasImageBrush, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasImageBrush** canvasImageBrush) override;

        IFACEMETHOD(CreateWithImage)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasImage* image,
            ICanvasImageBrush** canvasImageBrush) override;
    };

    class CanvasImageBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1Brush,
        CanvasImageBrush,
        ICanvasImageBrush,
        MixIn<CanvasImageBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasImageBrush, BaseTrust);

        // This class wraps both an image brush and a bitmap brush, and uses one at a time.
        // It uses the bitmap brush whenever possible, because it is more performant.
        // Otherwise, it uses the image brush.
        // Bitmap brush is eligible when the source image is a bitmap and the source rect
        // is NULL.

        mutable std::mutex m_mutex;

        ComPtr<ID2D1BitmapBrush1> m_d2dBitmapBrush;

        ComPtr<ID2D1ImageBrush> m_d2dImageBrush;

        CachedResourceReference<ID2D1Image, ICanvasImage> m_currentImageCache;

        bool m_isSourceRectSet;

    public:
        CanvasImageBrush(
            ICanvasDevice* device,
            ID2D1BitmapBrush1* bitmapBrush);

        CanvasImageBrush(
            ICanvasDevice* device,
            ID2D1ImageBrush* imageBrush);

        CanvasImageBrush(
            ICanvasDevice* device,
            ICanvasImage* image = nullptr);

        IFACEMETHOD(get_Image)(ICanvasImage** value) override;

        IFACEMETHOD(put_Image)(ICanvasImage* value) override;

        IFACEMETHOD(get_ExtendX)(CanvasEdgeBehavior* value) override;

        IFACEMETHOD(put_ExtendX)(CanvasEdgeBehavior value) override;

        IFACEMETHOD(get_ExtendY)(CanvasEdgeBehavior* value) override;

        IFACEMETHOD(put_ExtendY)(CanvasEdgeBehavior value) override;

        IFACEMETHOD(get_SourceRectangle)(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>** value) override;

        IFACEMETHOD(put_SourceRectangle)(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value) override;

        IFACEMETHOD(get_Interpolation)(CanvasImageInterpolation* value) override;

        IFACEMETHOD(put_Interpolation)(CanvasImageInterpolation value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override;

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** resource) override;

    private:
        void ThrowIfClosed();
        void SetImage(ICanvasImage* image);
        void SwitchToImageBrush(ID2D1Image* image);
        void SwitchToBitmapBrush(ID2D1Bitmap1* bitmap);
        void TrySwitchFromImageBrushToBitmapBrush();
        void RealizeSourceEffect(ID2D1DeviceContext* deviceContext, WIN2D_GET_D2D_IMAGE_FLAGS flags, float dpi);
    };

}}}}}
