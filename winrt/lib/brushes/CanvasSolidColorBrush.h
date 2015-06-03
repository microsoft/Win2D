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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ::Microsoft::WRL;

    class CanvasSolidColorBrush;
    class CanvasSolidColorBrushManager;

    struct CanvasSolidColorBrushTraits
    {
        typedef ID2D1SolidColorBrush resource_t;
        typedef CanvasSolidColorBrush wrapper_t;
        typedef ICanvasSolidColorBrush wrapper_interface_t;
        typedef CanvasSolidColorBrushManager manager_t;
    };

    class CanvasSolidColorBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasSolidColorBrushTraits, 
        MixIn<CanvasSolidColorBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasSolidColorBrush, BaseTrust);

    public:
        CanvasSolidColorBrush(
            std::shared_ptr<CanvasSolidColorBrushManager> manager,
            ID2D1SolidColorBrush* brush,
            ICanvasDevice* device);

        IFACEMETHOD(get_Color)(_Out_ ABI::Windows::UI::Color *value) override;

        IFACEMETHOD(put_Color)(_In_ ABI::Windows::UI::Color value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override;
    };

    class CanvasSolidColorBrushManager : public ResourceManager<CanvasSolidColorBrushTraits>
    {
    public:
        ComPtr<CanvasSolidColorBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color);

        ComPtr<CanvasSolidColorBrush> CreateWrapper(
            ICanvasDevice* device,
            ID2D1SolidColorBrush* resource);
    };

    class CanvasSolidColorBrushFactory 
        : public ActivationFactory<
            ICanvasSolidColorBrushFactory,
            CloakedIid<ICanvasDeviceResourceFactoryNative>>,
          public PerApplicationManager<CanvasSolidColorBrushFactory, CanvasSolidColorBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasSolidColorBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASDEVICERESOURCEFACTORYNATIVE();

        //
        // ICanvasSolidColorBrushFactory
        //

        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;
    };
}}}}}
