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

#include <Canvas.abi.h>

#include "ClosablePtr.h"
#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;

    class ICanvasImageBrushAdapter
    {
    public:
        virtual ComPtr<ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>> CreateRectReference(D2D1_RECT_F const& d2dRect) = 0;
    };

    class ICanvasDeviceInternal;

    [uuid(DAA42776-D012-4A3D-A7A3-2A061B00CE4D)]
    class ICanvasImageBrushInternal : public ICanvasBrushInternal
    {
    public:
        virtual ComPtr<ID2D1Brush> GetD2DBrushNoValidation() = 0;
    };

    class CanvasImageBrushFactory
        : public ActivationFactory<
            ICanvasImageBrushFactory,
            CloakedIid<ICanvasFactoryNative>>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageBrush, BaseTrust);

        std::shared_ptr<ICanvasImageBrushAdapter> m_adapter;

    public:

        CanvasImageBrushFactory();

        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasImageBrush** canvasImageBrush) override;

        IFACEMETHOD(CreateWithImage)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasImage* image,
            ICanvasImageBrush** canvasImageBrush) override;
            
        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;
    };

    class CanvasImageBrush : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasImageBrush,
        ABI::Windows::Foundation::IClosable,
        ChainInterfaces<CloakedIid<ICanvasImageBrushInternal>, CloakedIid<ICanvasBrushInternal>>,
        MixIn<CanvasImageBrush, CanvasBrush>>,
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageBrush, BaseTrust);

        // This class wraps both an image brush and a bitmap brush, and uses one at a time.
        // It uses the bitmap brush whenever possible, because it is more performant.
        // Otherwise, it uses the image brush.
        // Bitmap brush is eligible when the source image is a bitmap and the source rect
        // is NULL.

        ComPtr<ID2D1BitmapBrush1> m_d2dBitmapBrush;

        ComPtr<ID2D1ImageBrush> m_d2dImageBrush;

        ComPtr<ICanvasDeviceInternal> m_deviceInternal;

        std::shared_ptr<ICanvasImageBrushAdapter> m_adapter;

        bool m_useBitmapBrush;

        bool m_isClosed;

        bool m_isSourceRectSet;

    public:
        CanvasImageBrush(
            ICanvasDevice* device, 
            ICanvasImage* image,
            std::shared_ptr<ICanvasImageBrushAdapter> adapter);

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
        virtual ComPtr<ID2D1Brush> GetD2DBrush() override;

        // ICanvasImageBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrushNoValidation() override;

    private:
        void ThrowIfClosed();
        void SwitchFromBitmapBrushToImageBrush();
        void SwitchFromImageBrushToBitmapBrush();
        void SetImage(ICanvasImage* image);
        static D2D1_RECT_F GetD2DRectFromRectReference(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value);
    };

}}}}
