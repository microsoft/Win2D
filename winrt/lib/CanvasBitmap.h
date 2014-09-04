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

#include "ClosablePtr.h"
#include "ResourceManager.h"
#include "AsyncUtility.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::Effects;

    class ICanvasBitmapResourceCreationAdapter
    {
    public:
        virtual ComPtr<IWICFormatConverter> CreateWICFormatConverter(HSTRING fileName) = 0;
    };

    class CanvasBitmapFactory :
        public ActivationFactory<
        ICanvasBitmapFactory, 
        ICanvasBitmapStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> m_adapter;

    public:
        CanvasBitmapFactory();

        //
        // ICanvasBitmapStatics
        //
        IFACEMETHOD(LoadAsync)(
            ICanvasResourceCreator* resourceAllocator,
            HSTRING fileName,
            ABI::Windows::Foundation::IAsyncOperation<ICanvasBitmap*>** canvasBitmap) override;
    };

    class CanvasBitmap : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasBitmap,
        ICanvasImage,
        IEffectInput,
        ABI::Windows::Foundation::IClosable,
        CloakedIid<ICanvasImageInternal>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

        ClosablePtr<ID2D1Bitmap1> m_resource;

        ComPtr<IWICBitmap> m_wicBitmap;

        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> m_resourceAdapter;

    public:

        CanvasBitmap(
            ICanvasDevice* canvasDevice, 
            HSTRING fileName, 
            std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter);

        IFACEMETHOD(get_SizeInPixels)(_Out_ ABI::Windows::Foundation::Size* size) override;

        IFACEMETHOD(get_SizeInDips)(_Out_ ABI::Windows::Foundation::Size* size) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasImageInternal
        virtual ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext) override;
    };
}}}}
