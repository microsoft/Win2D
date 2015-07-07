// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Brushes
{
    using namespace ABI::Windows::UI;

    ComPtr<ID2D1GradientStopCollection1> CreateSimpleGradientStopCollection(
        ICanvasDevice* canvasDevice,
        Color startColor,
        Color endColor,
        CanvasEdgeBehavior edgeBehavior);

    Color Desaturate(Color const& color, float amount);

    ComPtr<ID2D1GradientStopCollection1> CreateRainbowGradientStopCollection(
        ICanvasDevice* canvasDevice,
        float eldritchness);

    void CopyStops(
        ComPtr<ID2D1GradientStopCollection> const& stopCollection,
        UINT32* valueCount,
        CanvasGradientStop** valueElements);

    uint8_t DesaturateChannel(uint8_t channel, float amount);

} } } } }
