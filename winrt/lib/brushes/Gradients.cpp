// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "Gradients.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    
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

        return CreateGradientStopCollection(
            canvasDevice,
            _countof(stops),
            stops,
            CanvasEdgeBehavior::Clamp,
            CanvasColorSpace::Srgb,
            CanvasColorSpace::Srgb,
            CanvasBufferPrecision::Precision8UIntNormalized,
            CanvasAlphaMode::Premultiplied);
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


} } } } }
