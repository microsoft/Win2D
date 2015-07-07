// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
