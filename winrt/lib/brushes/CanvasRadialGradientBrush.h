// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL;    

    class CanvasRadialGradientBrushManager;

    struct CanvasRadialGradientBrushTraits
    {
        typedef ID2D1RadialGradientBrush resource_t;
        typedef CanvasRadialGradientBrush wrapper_t;
        typedef ICanvasRadialGradientBrush wrapper_interface_t;
        typedef CanvasRadialGradientBrushManager manager_t;
    };

    class CanvasRadialGradientBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasRadialGradientBrushTraits,
        MixIn<CanvasRadialGradientBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasRadialGradientBrush, BaseTrust);

    public:
        CanvasRadialGradientBrush(
            std::shared_ptr<CanvasRadialGradientBrushManager> manager,
            ID2D1RadialGradientBrush* brush,
            ICanvasDevice* device);

        IFACEMETHOD(get_Center)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_Center)(_In_ Vector2 value) override;

        IFACEMETHOD(get_OriginOffset)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_OriginOffset)(_In_ Vector2 value) override;

        IFACEMETHOD(get_RadiusX)(_Out_ float* value) override;

        IFACEMETHOD(put_RadiusX)(_In_ float value) override;

        IFACEMETHOD(get_RadiusY)(_Out_ float* value) override;

        IFACEMETHOD(put_RadiusY)(_In_ float value) override;

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

    class CanvasRadialGradientBrushManager : public ResourceManager<CanvasRadialGradientBrushTraits>
    {
    public:
        ComPtr<CanvasRadialGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaMode alphaMode,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision);

        ComPtr<CanvasRadialGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ID2D1GradientStopCollection1* stopCollection);

        ComPtr<CanvasRadialGradientBrush> CreateWrapper(
            ICanvasDevice* device,
            ID2D1RadialGradientBrush* resource);
    };

    class CanvasRadialGradientBrushFactory
        : public ActivationFactory<
        ICanvasRadialGradientBrushFactory,
        CloakedIid<ICanvasDeviceResourceFactoryNative>,
        ICanvasRadialGradientBrushStatics>,
        public PerApplicationManager<CanvasRadialGradientBrushFactory, CanvasRadialGradientBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasRadialGradientBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASDEVICERESOURCEFACTORYNATIVE();

        IFACEMETHOD(CreateSimple)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color startColor,
            ABI::Windows::UI::Color endColor,
            ICanvasRadialGradientBrush** canvasRadialGradientBrush) override;

        IFACEMETHOD(CreateWithStops)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            ICanvasRadialGradientBrush** RadialGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndAlphaMode)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior extend,
            CanvasAlphaMode alphaMode,
            ICanvasRadialGradientBrush** RadialGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndInterpolationOptions)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaMode alphaMode,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            ICanvasRadialGradientBrush** radialGradientBrush) override;

        //
        // ICanvasRadialGradientBrushStatics
        //

        IFACEMETHOD(CreateRainbow)(
            ICanvasResourceCreator* resourceCreator,
            float eldritchness,
            ICanvasRadialGradientBrush** canvasRadialGradientBrush);
    };

    uint8_t DesaturateChannel(uint8_t channel, float amount);

}}}}}
