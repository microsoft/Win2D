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

#include "CanvasBitmap.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;

    class CanvasRenderTargetManager;

    class CanvasRenderTargetFactory 
        : public ActivationFactory<ICanvasRenderTargetFactory, CloakedIid<ICanvasDeviceResourceFactoryNative>>
        , public FactoryWithResourceManager<CanvasRenderTargetFactory, CanvasRenderTargetManager>
    {
    public:
        CanvasRenderTargetFactory();
    
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Size size,
            ICanvasRenderTarget** renderTarget);

        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;

        static std::shared_ptr<CanvasRenderTargetManager> CreateManager();
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
            ICanvasResourceCreator,
            MixIn<CanvasRenderTarget, CanvasBitmapImpl<CanvasRenderTargetTraits>>>
        , public CanvasBitmapImpl<CanvasRenderTargetTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget, BaseTrust);

        ComPtr<ICanvasDevice> m_device;

    public:
        CanvasRenderTarget(
            std::shared_ptr<CanvasRenderTargetManager> manager,
            ID2D1Bitmap1* bitmap,
            ICanvasDevice* device);

        IFACEMETHOD(CreateDrawingSession)(
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;
    };


    class CanvasRenderTargetManager : public ResourceManager<CanvasRenderTargetTraits>
    {
    public:
        ComPtr<CanvasRenderTarget> CreateNew(
            ICanvasDevice* canvasDevice,
            ABI::Windows::Foundation::Size size);

        ComPtr<CanvasRenderTarget> CreateWrapper(
            ICanvasDevice* device,
            ID2D1Bitmap1* bitmap);
    };
}}}}
