// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL;    

    class CanvasLinearGradientBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1LinearGradientBrush,
        CanvasLinearGradientBrush,
        ICanvasLinearGradientBrush,
        MixIn<CanvasLinearGradientBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasLinearGradientBrush, BaseTrust);

    public:
        static ComPtr<CanvasLinearGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaMode alphaMode,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision);

        static ComPtr<CanvasLinearGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ID2D1GradientStopCollection1* stopCollection);

        CanvasLinearGradientBrush(
            ICanvasDevice* device,
            ID2D1LinearGradientBrush* brush);

        IFACEMETHOD(get_StartPoint)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_StartPoint)(_In_ Vector2 value) override;

        IFACEMETHOD(get_EndPoint)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_EndPoint)(_In_ Vector2 value) override;

        IFACEMETHOD(get_Stops)(UINT32* valueCount, CanvasGradientStop** valueElements) override;

        IFACEMETHOD(get_EdgeBehavior)(CanvasEdgeBehavior* value) override;

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaMode* value) override;

        IFACEMETHOD(get_PreInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_PostInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_BufferPrecision)(CanvasBufferPrecision* value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush(ID2D1DeviceContext* deviceContext, GetBrushFlags flags) override;

    private:
        ComPtr<ID2D1GradientStopCollection1> GetGradientStopCollection();
    };

    class CanvasLinearGradientBrushFactory
        : public ActivationFactory<ICanvasLinearGradientBrushFactory, ICanvasLinearGradientBrushStatics>
        , private LifespanTracker<CanvasLinearGradientBrushFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasLinearGradientBrush, BaseTrust);

    public:
        IFACEMETHOD(CreateSimple)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color startColor,
            ABI::Windows::UI::Color endColor,
            ICanvasLinearGradientBrush** canvasLinearGradientBrush) override;

        IFACEMETHOD(CreateWithStops)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            ICanvasLinearGradientBrush** linearGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndAlphaMode)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior extend,
            CanvasAlphaMode alphaMode,
            ICanvasLinearGradientBrush** linearGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndInterpolationOptions)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaMode alphaMode,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            ICanvasLinearGradientBrush** linearGradientBrush) override;

        //
        // ICanvasLinearGradientBrushStatics
        //
        IFACEMETHOD(CreateRainbow)(
            ICanvasResourceCreator* resourceCreator,
            float eldritchness,
            ICanvasLinearGradientBrush** canvasLinearGradientBrush);
    };

}}}}}
