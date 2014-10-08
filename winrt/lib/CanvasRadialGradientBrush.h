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

#include "CanvasBrush.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL;    

    class CanvasRadialGradientBrushManager;

    [uuid(4B62DFF9-6188-46EC-A4A1-C52099278E34)]
    class ICanvasRadialGradientBrushInternal : public ICanvasBrushInternal
    {
    public:
        virtual ComPtr<ID2D1RadialGradientBrush> GetD2DRadialGradientBrush() = 0;
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

    class CanvasRadialGradientBrushFactory
        : public ActivationFactory<
        ICanvasRadialGradientBrushFactory,
        CloakedIid<ICanvasFactoryNative>,
        ICanvasRadialGradientBrushStatics>,
        public PerApplicationManager<CanvasRadialGradientBrushFactory, CanvasRadialGradientBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRadialGradientBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASFACTORYNATIVE();

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

        //
        // ICanvasRadialGradientBrushStatics
        //

        IFACEMETHOD(CreateRainbow)(
            ICanvasResourceCreator* resourceCreator,
            float eldritchness,
            ICanvasRadialGradientBrush** canvasRadialGradientBrush);
    };

    uint8_t DesaturateChannel(uint8_t channel, float amount);

}}}}
