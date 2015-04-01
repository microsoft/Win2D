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

#include <Microsoft.Graphics.Canvas.Effects.interop.h>
#include <Canvas.abi.h>

namespace NativeComponent
{
    using namespace Microsoft::WRL;
    using namespace Microsoft::Graphics::Canvas::Effects;

    namespace EffectsAbi = ABI::Microsoft::Graphics::Canvas::Effects;

    using EffectsAbi::IGraphicsEffectD2D1Interop;

    public enum class EffectPropertyMapping
    {
        Unknown              = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
        Direct               = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
        VectorX              = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
        VectorY              = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
        VectorZ              = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
        VectorW              = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
        RectToVector4        = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
        RadiansToDegrees     = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
        ColorMatrixAlphaMode = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
        ColorToVector3       = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
        ColorToVector4       = EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
    };

    // Exposes features of IGraphicsEffectD2D1Interop for use by test.managed.EffectTests.
    public ref class EffectAccessor sealed
    {
    public:
        static int GetSourceCount(IGraphicsEffect^ effect)
        {
            auto effectInterop = As<IGraphicsEffectD2D1Interop>(effect);

            unsigned count;
            ThrowIfFailed(effectInterop->GetSourceCount(&count));

            return static_cast<int>(count);
        }

        static IGraphicsEffectSource^ GetSource(IGraphicsEffect^ effect, int index)
        {
            auto effectInterop = As<IGraphicsEffectD2D1Interop>(effect);

            ComPtr<EffectsAbi::IGraphicsEffectSource> source;
            ThrowIfFailed(effectInterop->GetSource(index, &source));

            return reinterpret_cast<IGraphicsEffectSource^>(source.Get());
        }

        static int GetPropertyCount(IGraphicsEffect^ effect)
        {
            auto effectInterop = As<IGraphicsEffectD2D1Interop>(effect);

            unsigned count;
            ThrowIfFailed(effectInterop->GetPropertyCount(&count));

            return static_cast<int>(count);
        }

        static Object^ GetProperty(IGraphicsEffect^ effect, int index)
        {
            auto effectInterop = As<IGraphicsEffectD2D1Interop>(effect);

            ComPtr<ABI::Windows::Foundation::IPropertyValue> propertyValue;
            ThrowIfFailed(effectInterop->GetProperty(index, &propertyValue));

            return reinterpret_cast<Object^>(propertyValue.Get());
        }

        static void GetNamedPropertyMapping(IGraphicsEffect^ effect, Platform::String^ name, int* index, EffectPropertyMapping* mapping)
        {
            auto effectInterop = As<IGraphicsEffectD2D1Interop>(effect);

            UINT interopIndex;
            EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING interopMapping;

            ThrowIfFailed(effectInterop->GetNamedPropertyMapping(name->Data(), &interopIndex, &interopMapping));

            *index = static_cast<int>(interopIndex);
            *mapping = static_cast<EffectPropertyMapping>(interopMapping);
        }
    };
}
