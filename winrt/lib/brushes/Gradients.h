// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ABI::Windows::UI;

    uint8_t DesaturateChannel(uint8_t channel, float amount);
    Color Desaturate(Color const& color, float amount);
    
    ComPtr<ID2D1GradientStopCollection1> CreateRainbowGradientStopCollection(
        ICanvasDevice* canvasDevice,
        float eldritchness);
    
    template<typename STOP>
    inline ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
        ICanvasDevice* canvasDevice,
        uint32_t stopCount,
        STOP const* stops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision,
        CanvasAlphaMode alphaMode)
    {
        if (stopCount > 0)
            CheckInPointer(stops);
        
        std::vector<D2D1_GRADIENT_STOP> d2dStops;
        d2dStops.reserve(stopCount);
        for (uint32_t i = 0; i < stopCount; ++i)
        {            
            d2dStops.push_back(D2D1_GRADIENT_STOP{ stops[i].Position, ToD2DColor(stops[i].Color) });
        }

        return As<ICanvasDeviceInternal>(canvasDevice)->CreateGradientStopCollection(
            std::move(d2dStops),
            static_cast<D2D1_COLOR_SPACE>(preInterpolationSpace),
            static_cast<D2D1_COLOR_SPACE>(postInterpolationSpace),
            ToD2DBufferPrecision(bufferPrecision),
            static_cast<D2D1_EXTEND_MODE>(edgeBehavior),
            ToD2DColorInterpolation(alphaMode));
    }

    template<typename COLOR>
    struct ToGradientStopType{};

    template<>
    struct ToGradientStopType<Color>
    {
        typedef CanvasGradientStop stop_t;
    };

    template<>
    struct ToGradientStopType<Vector4>
    {
        typedef CanvasGradientStopHdr stop_t;
    };

    template<typename COLOR>
    inline ComPtr<ID2D1GradientStopCollection1> CreateSimpleGradientStopCollection(
        ICanvasDevice* canvasDevice,
        COLOR startColor,
        COLOR endColor,
        CanvasEdgeBehavior edgeBehavior)
    {
        ToGradientStopType<COLOR>::stop_t stops[2] =
            { { 0.0f, startColor },
              { 1.0f, endColor   } };

        auto stopCollection = CreateGradientStopCollection(
            canvasDevice,
            _countof(stops),
            stops,
            edgeBehavior,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            CanvasAlphaMode::Premultiplied);

        return stopCollection;
    }

    template<typename BRUSH, typename COLOR, typename IBRUSH>
    inline HRESULT CreateSimpleGradientBrush(
        ICanvasResourceCreator* resourceCreator,
        COLOR startColor,
        COLOR endColor,
        IBRUSH** brush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(brush);

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));

                auto stopCollection = CreateSimpleGradientStopCollection(device.Get(), startColor, endColor, CanvasEdgeBehavior::Clamp);
                auto newBrush = BRUSH::CreateNew(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newBrush.CopyTo(brush));
            });
    }


    template<typename BRUSH, typename STOP, typename IBRUSH>
    inline HRESULT CreateGradientBrush(
        ICanvasResourceCreator* resourceCreator,
        uint32_t stopCount,
        STOP* stops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaMode alphaMode,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision,
        IBRUSH** brush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(brush);

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));

                auto stopCollection = CreateGradientStopCollection(
                    device.Get(),
                    stopCount,
                    stops,
                    edgeBehavior,
                    preInterpolationSpace,
                    postInterpolationSpace,
                    bufferPrecision,
                    alphaMode);

                auto newBrush = BRUSH::CreateNew(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newBrush.CopyTo(brush));
            });
    }

    template<typename BRUSH, typename STOP, typename IBRUSH>
    inline HRESULT CreateGradientBrush(
        ICanvasResourceCreator* resourceCreator,
        uint32_t stopCount,
        STOP* stops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaMode alphaMode,
        IBRUSH** brush)
    {
        return CreateGradientBrush<BRUSH>(
            resourceCreator,
            stopCount,
            stops,
            edgeBehavior,
            alphaMode,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            brush);
    }


    template<typename BRUSH, typename STOP, typename IBRUSH>
    inline HRESULT CreateGradientBrush(
        ICanvasResourceCreator* resourceCreator,
        uint32_t stopCount,
        STOP* stops,
        IBRUSH** brush)
    {
        return CreateGradientBrush<BRUSH>(
            resourceCreator,
            stopCount,
            stops,
            CanvasEdgeBehavior::Clamp,
            CanvasAlphaMode::Premultiplied,
            brush);
    }


    template<typename STOP> struct ToWin2DColor{};

    template<> struct ToWin2DColor<CanvasGradientStop>
    {
        static Color Convert(D2D1_COLOR_F color)
        {
            return ToWindowsColor(color);
        }
    };

    template<> struct ToWin2DColor<CanvasGradientStopHdr>
    {
        static Vector4 Convert(D2D1_COLOR_F color)
        {
            return *ReinterpretAs<Vector4*>(&color);
        }
    };

        
    template<typename D2DBRUSH, typename STOP>
    inline void CopyStops(
        ComPtr<D2DBRUSH> const& d2dBrush,
        UINT32* valueCount,
        STOP** valueElements)
    {
        ComPtr<ID2D1GradientStopCollection> stopCollection;
        d2dBrush->GetGradientStopCollection(&stopCollection);

        auto stopCollection1 = As<ID2D1GradientStopCollection1>(stopCollection);

        auto stopCount = stopCollection1->GetGradientStopCount();
        assert(stopCount != 0); // Enforced by D2D

        std::vector<D2D1_GRADIENT_STOP> d2dStops(stopCount);

        stopCollection1->GetGradientStops1(d2dStops.data(), stopCount);

        auto stops = TransformToComArray<STOP>(d2dStops.begin(), d2dStops.end(),
            [](D2D1_GRADIENT_STOP const& d2dStop)
            {
                return STOP{ d2dStop.position, ToWin2DColor<STOP>::Convert(d2dStop.color) };
            });

        stops.Detach(valueCount, valueElements);
    }



} } } } }
