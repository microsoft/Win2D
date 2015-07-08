// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL;    

    class CanvasLinearGradientBrushManager;

    struct CanvasLinearGradientBrushTraits
    {
        typedef ID2D1LinearGradientBrush resource_t;
        typedef CanvasLinearGradientBrush wrapper_t;
        typedef ICanvasLinearGradientBrush wrapper_interface_t;
        typedef CanvasLinearGradientBrushManager manager_t;
    };

    class CanvasLinearGradientBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasLinearGradientBrushTraits,
        MixIn<CanvasLinearGradientBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasLinearGradientBrush, BaseTrust);

    public:
        CanvasLinearGradientBrush(
            std::shared_ptr<CanvasLinearGradientBrushManager> manager,
            ID2D1LinearGradientBrush* brush,
            ICanvasDevice* device);

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

    class CanvasLinearGradientBrushManager : public ResourceManager<CanvasLinearGradientBrushTraits>
    {
    public:
        ComPtr<CanvasLinearGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaMode alphaMode,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision);

        ComPtr<CanvasLinearGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ID2D1GradientStopCollection1* stopCollection);

        ComPtr<CanvasLinearGradientBrush> CreateWrapper(
            ICanvasDevice* device,
            ID2D1LinearGradientBrush* resource);
    };

    class CanvasLinearGradientBrushFactory
        : public ActivationFactory<
        ICanvasLinearGradientBrushFactory,
        CloakedIid<ICanvasDeviceResourceFactoryNative>,
        ICanvasLinearGradientBrushStatics>,
        public PerApplicationManager<CanvasLinearGradientBrushFactory, CanvasLinearGradientBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Brushes_CanvasLinearGradientBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASDEVICERESOURCEFACTORYNATIVE();
        
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
