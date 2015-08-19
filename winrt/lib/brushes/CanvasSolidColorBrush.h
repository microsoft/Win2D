// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ::Microsoft::WRL;

    class CanvasSolidColorBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1SolidColorBrush,
        CanvasSolidColorBrush,
        ICanvasSolidColorBrush,
        MixIn<CanvasSolidColorBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasSolidColorBrush, BaseTrust);

    public:
        CanvasSolidColorBrush(
            ID2D1SolidColorBrush* brush,
            ICanvasDevice* device);

        IFACEMETHOD(get_Color)(_Out_ ABI::Windows::UI::Color *value) override;

        IFACEMETHOD(put_Color)(_In_ ABI::Windows::UI::Color value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override;
    };

    class CanvasSolidColorBrushManager : private LifespanTracker<CanvasSolidColorBrushManager>
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
        : public ActivationFactory<ICanvasSolidColorBrushFactory>
        , private LifespanTracker<CanvasSolidColorBrushFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasSolidColorBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_GETMANAGER(CanvasSolidColorBrushManager);

        //
        // ICanvasSolidColorBrushFactory
        //

        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;
    };
}}}}}
