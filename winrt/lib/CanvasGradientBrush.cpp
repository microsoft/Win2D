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

#include "pch.h"
#include "CanvasGradientBrush.h"
#include "CanvasDevice.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using ABI::Windows::UI::Color;
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ::Microsoft::WRL::Wrappers;

    ComPtr<ID2D1GradientStopCollection1> CreateSimpleGradientStopCollection(
        ICanvasDevice* canvasDevice,
        Color startColor,
        Color endColor,
        CanvasEdgeBehavior edgeBehavior)
    {
        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(deviceInternal.GetAddressOf()));

        CanvasGradientStop stops[2];
        stops[0].Color = startColor;
        stops[0].Position = 0.0f;
        stops[1].Color = endColor;
        stops[1].Position = 1.0f;

        ComPtr<ID2D1GradientStopCollection1> stopCollection = deviceInternal->CreateGradientStopCollection(
            2,
            stops,
            edgeBehavior,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied);

        return stopCollection;
    }

    Color Desaturate(Color const& color, float amount)
    {
        Color result;
        result.R = DesaturateChannel(color.R, amount);
        result.G = DesaturateChannel(color.G, amount);
        result.B = DesaturateChannel(color.B, amount);
        result.A = color.A;
        return result;
    }

    ComPtr<ID2D1GradientStopCollection1> CreateRainbowGradientStopCollection(
        ICanvasDevice* canvasDevice,
        float eldritchness)
    {
        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(deviceInternal.GetAddressOf()));

        CanvasGradientStop stops[] =
        {
            { 0.0f, { 255, 233, 1, 1 } }, // Red
            { 0.16f, { 255, 224, 166, 58 } }, // Orange
            { 0.33f, { 255, 253, 247, 121 } }, // Yellow
            { 0.50f, { 255, 18, 203, 22 } }, // Green
            { 0.66f, { 255, 33, 38, 235 } }, // Blue
            { 0.82f, { 255, 157, 69, 237 } }, // Indigo
            { 1.0f, { 255, 171, 47, 213 } }, // Violet
        };

        for (size_t i = 0; i < _countof(stops); i++)
        {
            stops[i].Color = Desaturate(stops[i].Color, eldritchness);
        }

        ComPtr<ID2D1GradientStopCollection1> stopCollection = deviceInternal->CreateGradientStopCollection(
            _countof(stops),
            stops,
            CanvasEdgeBehavior::Wrap,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied);

        return stopCollection;
    }

    void CopyStops(
        ComPtr<ID2D1GradientStopCollection> const stopCollection, 
        UINT32* valueCount, 
        CanvasGradientStop** valueElements)
    {
        ComPtr<ID2D1GradientStopCollection1> stopCollection1;
        ThrowIfFailed(stopCollection.As(&stopCollection1));

        std::vector<D2D1_GRADIENT_STOP> d2dStops;
        UINT stopCount = stopCollection1->GetGradientStopCount();

        assert(stopCount != 0); // Enforced by D2D

        d2dStops.resize(stopCount);
        stopCollection1->GetGradientStops1(&(d2dStops[0]), stopCount);

        std::vector<CanvasGradientStop> stops;
        stops.resize(stopCount);
        for (UINT i = 0; i < stopCount; ++i)
        {
            stops[i].Position = d2dStops[i].position;
            stops[i].Color = ToWindowsColor(d2dStops[i].color);
            // TODO #837: Decide what to do about high-color gradient stops.
        }

        assert(stops.size() <= UINT_MAX);

        // TODO #2508: Use RAII wrapper for these types of allocations.
        auto sizeInBytes = stops.size() * sizeof(CanvasGradientStop);
        (*valueCount) = static_cast<UINT32>(stops.size());
        (*valueElements) = static_cast<CanvasGradientStop*>(CoTaskMemAlloc(sizeInBytes));
        ThrowIfNullPointer(*valueElements, E_OUTOFMEMORY);

        if (stops.size() > 0)
        {
            memcpy_s(*valueElements, sizeInBytes, &stops[0], sizeInBytes);
        }
    }

    uint8_t DesaturateChannel(uint8_t channel, float amount)
    {
        amount = Saturate(amount); // performs clamping
        const float precColor = static_cast<float>(channel);
        const float gray = 127.0f;
        float result = (amount * gray) + ((1.0f - amount) * precColor);
        result = std::min<float>(result, 255.0f);
        result = std::max<float>(result, 0.0f);
        return static_cast<uint8_t>(result);
    }

    ComPtr<CanvasLinearGradientBrush> CanvasLinearGradientBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device.As(&deviceInternal));

        ComPtr<ID2D1GradientStopCollection1> stopCollection = deviceInternal->CreateGradientStopCollection(
            gradientStopCount,
            gradientStops,
            edgeBehavior,
            preInterpolationSpace,
            postInterpolationSpace,
            bufferPrecision,
            alphaBehavior);

        auto d2dBrush = deviceInternal->CreateLinearGradientBrush(stopCollection.Get());

        auto canvasLinearGradientBrush = Make<CanvasLinearGradientBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasLinearGradientBrush);

        return canvasLinearGradientBrush;
    }

    ComPtr<CanvasLinearGradientBrush> CanvasLinearGradientBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        ID2D1GradientStopCollection1* stopCollection)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device.As(&deviceInternal));

        auto d2dBrush = deviceInternal->CreateLinearGradientBrush(stopCollection);

        auto canvasLinearGradientBrush = Make<CanvasLinearGradientBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasLinearGradientBrush);

        return canvasLinearGradientBrush;
    }

    ComPtr<CanvasLinearGradientBrush> CanvasLinearGradientBrushManager::CreateWrapper(
        ID2D1LinearGradientBrush* resource)
    {
        auto canvasLinearGradientBrush = Make<CanvasLinearGradientBrush>(
            shared_from_this(),
            resource);
        CheckMakeResult(canvasLinearGradientBrush);

        return canvasLinearGradientBrush;
    }

    IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateSimple(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::UI::Color startColor,
        ABI::Windows::UI::Color endColor,
        ICanvasLinearGradientBrush** linearGradientBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(linearGradientBrush);

                ComPtr<ICanvasDevice> canvasDevice;
                resourceCreator->get_Device(&canvasDevice);

                ComPtr<ID2D1GradientStopCollection1> stopCollection =
                    CreateSimpleGradientStopCollection(canvasDevice.Get(), startColor, endColor, CanvasEdgeBehavior::Clamp);

                auto newLinearBrush = GetManager()->Create(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newLinearBrush.CopyTo(linearGradientBrush));
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateWithEdgeBehaviorAndAlphaBehavior(
        ICanvasResourceCreator* resourceAllocator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        ICanvasLinearGradientBrush** linearGradientBrush)
    {
        return CreateWithEdgeBehaviorAndInterpolationOptions(
            resourceAllocator,
            gradientStopCount,
            gradientStops,
            edgeBehavior,
            alphaBehavior,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            linearGradientBrush);
    }

    IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateWithEdgeBehaviorAndInterpolationOptions(
        ICanvasResourceCreator* resourceCreator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision,
        ICanvasLinearGradientBrush** linearGradientBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(linearGradientBrush);

                auto newLinearBrush = GetManager()->Create(
                    resourceCreator,
                    gradientStopCount,
                    gradientStops,
                    edgeBehavior,
                    alphaBehavior,
                    preInterpolationSpace,
                    postInterpolationSpace,
                    bufferPrecision);

                ThrowIfFailed(newLinearBrush.CopyTo(linearGradientBrush));
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateRainbow(
        ICanvasResourceCreator* resourceCreator,
        float eldritchness,
        ICanvasLinearGradientBrush** canvasLinearGradientBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(canvasLinearGradientBrush);

                ComPtr<ICanvasDevice> canvasDevice;
                resourceCreator->get_Device(&canvasDevice);

                ComPtr<ID2D1GradientStopCollection1> stopCollection =
                    CreateRainbowGradientStopCollection(canvasDevice.Get(), eldritchness);

                auto newLinearBrush = GetManager()->Create(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newLinearBrush.CopyTo(canvasLinearGradientBrush));
            });
    }

    CanvasLinearGradientBrush::CanvasLinearGradientBrush(
        std::shared_ptr<CanvasLinearGradientBrushManager> manager,
        ID2D1LinearGradientBrush* brush)
        : ResourceWrapper(manager, brush)
    {
        assert(brush != nullptr);
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_StartPoint(_Out_ Vector2* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DPoint(GetD2DLinearGradientBrush()->GetStartPoint());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::put_StartPoint(_In_ Vector2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DLinearGradientBrush()->SetStartPoint(ToD2DPoint(value));
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_EndPoint(_Out_ Vector2* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DPoint(GetD2DLinearGradientBrush()->GetEndPoint());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::put_EndPoint(_In_ Vector2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DLinearGradientBrush()->SetEndPoint(ToD2DPoint(value));
            });
    }
    
    IFACEMETHODIMP CanvasLinearGradientBrush::get_Stops(UINT32* valueCount, CanvasGradientStop** valueElements)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                auto resource = GetResource();

                ComPtr<ID2D1GradientStopCollection> stopCollection;
                resource->GetGradientStopCollection(&stopCollection);
                CopyStops(stopCollection, valueCount, valueElements);
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_EdgeBehavior(CanvasEdgeBehavior* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = static_cast<CanvasEdgeBehavior>(GetGradientStopCollection()->GetExtendMode());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_AlphaMode(CanvasAlphaBehavior* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DColorInterpolation(GetGradientStopCollection()->GetColorInterpolationMode());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_PreInterpolationSpace(CanvasColorSpace* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = static_cast<CanvasColorSpace>(GetGradientStopCollection()->GetPreInterpolationSpace());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_PostInterpolationSpace(CanvasColorSpace* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = static_cast<CanvasColorSpace>(GetGradientStopCollection()->GetPostInterpolationSpace());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::get_BufferPrecision(CanvasBufferPrecision* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DBufferPrecision(GetGradientStopCollection()->GetBufferPrecision());
            });
    }

    IFACEMETHODIMP CanvasLinearGradientBrush::Close()
    {
        return ResourceWrapper::Close();
    }

    ComPtr<ID2D1Brush> CanvasLinearGradientBrush::GetD2DBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1LinearGradientBrush> CanvasLinearGradientBrush::GetD2DLinearGradientBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1GradientStopCollection1> CanvasLinearGradientBrush::GetGradientStopCollection()
    {
        auto resource = GetResource();
        ComPtr<ID2D1GradientStopCollection> stopCollection;
        resource->GetGradientStopCollection(stopCollection.GetAddressOf());
        ComPtr<ID2D1GradientStopCollection1> stopCollection1;
        ThrowIfFailed(stopCollection.As(&stopCollection1));
        return stopCollection1;
    }

    ComPtr<CanvasRadialGradientBrush> CanvasRadialGradientBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device.As(&deviceInternal));

        ComPtr<ID2D1GradientStopCollection1> stopCollection = deviceInternal->CreateGradientStopCollection(
            gradientStopCount,
            gradientStops,
            edgeBehavior,
            preInterpolationSpace,
            postInterpolationSpace,
            bufferPrecision,
            alphaBehavior);

        auto d2dBrush = deviceInternal->CreateRadialGradientBrush(stopCollection.Get());

        auto canvasRadialGradientBrush = Make<CanvasRadialGradientBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasRadialGradientBrush);

        return canvasRadialGradientBrush;
    }

    ComPtr<CanvasRadialGradientBrush> CanvasRadialGradientBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        ID2D1GradientStopCollection1* stopCollection)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device.As(&deviceInternal));

        auto d2dBrush = deviceInternal->CreateRadialGradientBrush(stopCollection);

        auto canvasRadialGradientBrush = Make<CanvasRadialGradientBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasRadialGradientBrush);

        return canvasRadialGradientBrush;
    }

    ComPtr<CanvasRadialGradientBrush> CanvasRadialGradientBrushManager::CreateWrapper(
        ID2D1RadialGradientBrush* resource)
    {
        auto canvasRadialGradientBrush = Make<CanvasRadialGradientBrush>(
            shared_from_this(),
            resource);
        CheckMakeResult(canvasRadialGradientBrush);

        return canvasRadialGradientBrush;
    }


    IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateSimple(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::UI::Color startColor,
        ABI::Windows::UI::Color endColor,
        ICanvasRadialGradientBrush** radialGradientBrush)
    {

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(radialGradientBrush);

                ComPtr<ICanvasDevice> canvasDevice;
                resourceCreator->get_Device(&canvasDevice);

                ComPtr<ID2D1GradientStopCollection1> stopCollection =
                    CreateSimpleGradientStopCollection(canvasDevice.Get(), startColor, endColor, CanvasEdgeBehavior::Clamp);

                auto newRadialBrush = GetManager()->Create(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newRadialBrush.CopyTo(radialGradientBrush));
            });    
     }

    IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateWithEdgeBehaviorAndAlphaBehavior(
        ICanvasResourceCreator* resourceAllocator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        ICanvasRadialGradientBrush** radialGradientBrush)
    {
        return CreateWithEdgeBehaviorAndInterpolationOptions(
            resourceAllocator,
            gradientStopCount,
            gradientStops,
            edgeBehavior,
            alphaBehavior,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            radialGradientBrush);
    }

    IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateWithEdgeBehaviorAndInterpolationOptions(
        ICanvasResourceCreator* resourceCreator,
        UINT32 gradientStopCount,
        CanvasGradientStop* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasAlphaBehavior alphaBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision,
        ICanvasRadialGradientBrush** radialGradientBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(radialGradientBrush);

                auto newRadialBrush = GetManager()->Create(
                    resourceCreator,
                    gradientStopCount,
                    gradientStops,
                    edgeBehavior,
                    alphaBehavior,
                    preInterpolationSpace,
                    postInterpolationSpace,
                    bufferPrecision);

                ThrowIfFailed(newRadialBrush.CopyTo(radialGradientBrush));
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateRainbow(
        ICanvasResourceCreator* resourceCreator,
        float eldritchness,
        ICanvasRadialGradientBrush** canvasRadialGradientBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(canvasRadialGradientBrush);

                ComPtr<ICanvasDevice> canvasDevice;
                resourceCreator->get_Device(&canvasDevice);

                ComPtr<ID2D1GradientStopCollection1> stopCollection =
                    CreateRainbowGradientStopCollection(canvasDevice.Get(), eldritchness);

                auto newRadialBrush = GetManager()->Create(resourceCreator, stopCollection.Get());

                ThrowIfFailed(newRadialBrush.CopyTo(canvasRadialGradientBrush));
            });
    }

    CanvasRadialGradientBrush::CanvasRadialGradientBrush(
        std::shared_ptr<CanvasRadialGradientBrushManager> manager,
        ID2D1RadialGradientBrush* brush)
        : ResourceWrapper(manager, brush)
    {
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_Center(_Out_ Vector2* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DPoint(GetD2DRadialGradientBrush()->GetCenter());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::put_Center(_In_ Vector2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DRadialGradientBrush()->SetCenter(ToD2DPoint(value));
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_OriginOffset(_Out_ Vector2* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DPoint(GetD2DRadialGradientBrush()->GetGradientOriginOffset());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::put_OriginOffset(_In_ Vector2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DRadialGradientBrush()->SetGradientOriginOffset(ToD2DPoint(value));
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_RadiusX(_Out_ float* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = GetD2DRadialGradientBrush()->GetRadiusX();
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::put_RadiusX(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DRadialGradientBrush()->SetRadiusX(value);
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_RadiusY(_Out_ float* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = GetD2DRadialGradientBrush()->GetRadiusY();
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::put_RadiusY(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DRadialGradientBrush()->SetRadiusY(value);
            });
    }
    IFACEMETHODIMP CanvasRadialGradientBrush::get_Stops(UINT32* valueCount, CanvasGradientStop** valueElements)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                auto resource = GetResource();

                ComPtr<ID2D1GradientStopCollection> stopCollection;
                resource->GetGradientStopCollection(&stopCollection);
                CopyStops(stopCollection, valueCount, valueElements);
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_EdgeBehavior(CanvasEdgeBehavior* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = StaticCastAs<CanvasEdgeBehavior>(GetGradientStopCollection()->GetExtendMode());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_AlphaMode(CanvasAlphaBehavior* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DColorInterpolation(GetGradientStopCollection()->GetColorInterpolationMode());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_PreInterpolationSpace(CanvasColorSpace* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = StaticCastAs<CanvasColorSpace>(GetGradientStopCollection()->GetPreInterpolationSpace());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_PostInterpolationSpace(CanvasColorSpace* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = StaticCastAs<CanvasColorSpace>(GetGradientStopCollection()->GetPostInterpolationSpace());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::get_BufferPrecision(CanvasBufferPrecision* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = FromD2DBufferPrecision(GetGradientStopCollection()->GetBufferPrecision());
            });
    }

    IFACEMETHODIMP CanvasRadialGradientBrush::Close()
    {
        return ResourceWrapper::Close();
    }

    ComPtr<ID2D1Brush> CanvasRadialGradientBrush::GetD2DBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1RadialGradientBrush> CanvasRadialGradientBrush::GetD2DRadialGradientBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1GradientStopCollection1> CanvasRadialGradientBrush::GetGradientStopCollection()
    {
        auto resource = GetResource();
        ComPtr<ID2D1GradientStopCollection> stopCollection;
        resource->GetGradientStopCollection(stopCollection.GetAddressOf());
        ComPtr<ID2D1GradientStopCollection1> stopCollection1;
        ThrowIfFailed(stopCollection.As(&stopCollection1));
        return stopCollection1;
    }

    ActivatableClassWithFactory(CanvasLinearGradientBrush, CanvasLinearGradientBrushFactory);
    ActivatableClassWithFactory(CanvasRadialGradientBrush, CanvasRadialGradientBrushFactory);
} } } }
