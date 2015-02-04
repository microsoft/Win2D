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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
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

} } } }