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
#include "Gradients.h"
#include "CanvasDevice.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
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
            CanvasEdgeBehavior::Clamp,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied);

        return stopCollection;
    }

    void CopyStops(
        ComPtr<ID2D1GradientStopCollection> const& stopCollection,
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

        auto stops = TransformToComArray<CanvasGradientStop>(d2dStops.begin(), d2dStops.end(),
            [](D2D1_GRADIENT_STOP const& d2dStop)
        {
            // TODO #837: Decide what to do about high-color gradient stops.
            return CanvasGradientStop{ d2dStop.position, ToWindowsColor(d2dStop.color) };
        });

        stops.Detach(valueCount, valueElements);
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


} } } }
