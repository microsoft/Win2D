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
        static ComPtr<CanvasSolidColorBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color);

        static ComPtr<CanvasSolidColorBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            Numerics::Vector4 colorHdr);

        CanvasSolidColorBrush(
            ICanvasDevice* device,
            ID2D1SolidColorBrush* brush);

        IFACEMETHOD(get_Color)(ABI::Windows::UI::Color* value) override;
        IFACEMETHOD(put_Color)(ABI::Windows::UI::Color value) override;

        IFACEMETHOD(get_ColorHdr)(Numerics::Vector4* value) override;
        IFACEMETHOD(put_ColorHdr)(Numerics::Vector4 value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override;
    };

    class CanvasSolidColorBrushFactory 
        : public ActivationFactory<ICanvasSolidColorBrushFactory, ICanvasSolidColorBrushStatics>
        , private LifespanTracker<CanvasSolidColorBrushFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasSolidColorBrush, BaseTrust);

    public:

        //
        // ICanvasSolidColorBrushFactory
        //

        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;

        //
        // ICanvasSolidColorBrushStatics
        //

        IFACEMETHOD(CreateHdr)(
            ICanvasResourceCreator* resourceCreator,
            Numerics::Vector4 colorHdr,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;
    };
}}}}}
