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

#include "CanvasSolidColorBrush.h" // For ICanvasBrushInternal; TODO #2556 move ICanvasBrushInternal somewhere less surprising

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL;    

    class CanvasLinearGradientBrushManager;

    [uuid(B44BEE31-5341-4F2A-9F1D-D9F215B7FC32)]
    class ICanvasLinearGradientBrushInternal : public ICanvasBrushInternal
    {
    public:
        virtual ComPtr<ID2D1LinearGradientBrush> GetD2DLinearGradientBrush() = 0;
    };

    class CanvasLinearGradientBrushFactory
        : public ActivationFactory<
        ICanvasLinearGradientBrushFactory,
        CloakedIid<ICanvasFactoryNative>,
        ICanvasLinearGradientBrushStatics>,
        public PerApplicationManager<CanvasLinearGradientBrushFactory, CanvasLinearGradientBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasLinearGradientBrush, BaseTrust);

    public:

        CanvasLinearGradientBrushFactory();
        
        IFACEMETHOD(CreateSimple)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color startColor,
            ABI::Windows::UI::Color endColor,
            ICanvasLinearGradientBrush** canvasLinearGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndAlphaBehavior)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior extend,
            CanvasAlphaBehavior alphaBehavior,
            ICanvasLinearGradientBrush** linearGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndInterpolationOptions)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaBehavior alphaBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            ICanvasLinearGradientBrush** linearGradientBrush) override;

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;

        //
        // ICanvasLinearGradientBrushStatics
        //
        IFACEMETHOD(CreateRainbow)(
            ICanvasResourceCreator* resourceCreator,
            float eldritchness,
            ICanvasLinearGradientBrush** canvasLinearGradientBrush);
    };

    struct CanvasLinearGradientBrushTraits
    {
        typedef ID2D1LinearGradientBrush resource_t;
        typedef CanvasLinearGradientBrush wrapper_t;
        typedef ICanvasLinearGradientBrush wrapper_interface_t;
        typedef CanvasLinearGradientBrushManager manager_t;
    };

    class CanvasLinearGradientBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasLinearGradientBrushTraits,
        ChainInterfaces<CloakedIid<ICanvasLinearGradientBrushInternal>, CloakedIid<ICanvasBrushInternal>>,
        MixIn<CanvasLinearGradientBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasLinearGradientBrush, BaseTrust);

    public:
        CanvasLinearGradientBrush(
            std::shared_ptr<CanvasLinearGradientBrushManager> manager,
            ID2D1LinearGradientBrush* brush);

        IFACEMETHOD(get_StartPoint)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_StartPoint)(_In_ Vector2 value) override;

        IFACEMETHOD(get_EndPoint)(_Out_ Vector2* value) override;

        IFACEMETHOD(put_EndPoint)(_In_ Vector2 value) override;

        IFACEMETHOD(get_Stops)(UINT32* valueCount, CanvasGradientStop** valueElements) override;

        IFACEMETHOD(get_EdgeBehavior)(CanvasEdgeBehavior* value) override;

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaBehavior* value) override;

        IFACEMETHOD(get_PreInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_PostInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_BufferPrecision)(CanvasBufferPrecision* value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush() override;

        // ICanvasLinearGradientBrushInternal
        virtual ComPtr<ID2D1LinearGradientBrush> GetD2DLinearGradientBrush() override;

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
            CanvasAlphaBehavior alphaBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision);

        ComPtr<CanvasLinearGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ID2D1GradientStopCollection1* stopCollection);

        ComPtr<CanvasLinearGradientBrush> CreateWrapper(
            ID2D1LinearGradientBrush* resource);
    };

    class CanvasRadialGradientBrushManager;

    [uuid(4B62DFF9-6188-46EC-A4A1-C52099278E34)]
    class ICanvasRadialGradientBrushInternal : public ICanvasBrushInternal
    {
    public:
        virtual ComPtr<ID2D1RadialGradientBrush> GetD2DRadialGradientBrush() = 0;
    };

    class CanvasRadialGradientBrushFactory
        : public ActivationFactory<
        ICanvasRadialGradientBrushFactory,
        CloakedIid<ICanvasFactoryNative>,
        ICanvasRadialGradientBrushStatics>,
        public PerApplicationManager<CanvasRadialGradientBrushFactory, CanvasRadialGradientBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRadialGradientBrush, BaseTrust);

    public:

        CanvasRadialGradientBrushFactory();

        IFACEMETHOD(CreateSimple)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color startColor,
            ABI::Windows::UI::Color endColor,
            ICanvasRadialGradientBrush** canvasRadialGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndAlphaBehavior)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior extend,
            CanvasAlphaBehavior alphaBehavior,
            ICanvasRadialGradientBrush** RadialGradientBrush) override;

        IFACEMETHOD(CreateWithEdgeBehaviorAndInterpolationOptions)(
            ICanvasResourceCreator* resourceAllocator,
            UINT32 gradientStopCount,
            CanvasGradientStop* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasAlphaBehavior alphaBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            ICanvasRadialGradientBrush** radialGradientBrush) override;

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;

        //
        // ICanvasRadialGradientBrushStatics
        //

        IFACEMETHOD(CreateRainbow)(
            ICanvasResourceCreator* resourceCreator,
            float eldritchness,
            ICanvasRadialGradientBrush** canvasRadialGradientBrush);
    };

    struct CanvasRadialGradientBrushTraits
    {
        typedef ID2D1RadialGradientBrush resource_t;
        typedef CanvasRadialGradientBrush wrapper_t;
        typedef ICanvasRadialGradientBrush wrapper_interface_t;
        typedef CanvasRadialGradientBrushManager manager_t;
    };

    class CanvasRadialGradientBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasRadialGradientBrushTraits,
        ChainInterfaces<CloakedIid<ICanvasRadialGradientBrushInternal>, CloakedIid<ICanvasBrushInternal>>,
        MixIn<CanvasRadialGradientBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRadialGradientBrush, BaseTrust);

    public:
        CanvasRadialGradientBrush(
            std::shared_ptr<CanvasRadialGradientBrushManager> manager,
            ID2D1RadialGradientBrush* brush);

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

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaBehavior* value) override;

        IFACEMETHOD(get_PreInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_PostInterpolationSpace)(CanvasColorSpace* value) override;

        IFACEMETHOD(get_BufferPrecision)(CanvasBufferPrecision* value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush() override;

        // ICanvasRadialGradientBrushInternal
        virtual ComPtr<ID2D1RadialGradientBrush> GetD2DRadialGradientBrush() override;

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
            CanvasAlphaBehavior alphaBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision);

        ComPtr<CanvasRadialGradientBrush> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            ID2D1GradientStopCollection1* stopCollection);

        ComPtr<CanvasRadialGradientBrush> CreateWrapper(
            ID2D1RadialGradientBrush* resource);
    };

    uint8_t DesaturateChannel(uint8_t channel, float amount);

}}}}
