// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::WRL;

#ifdef USE_LOCALLY_EMULATED_UAP_APIS

	#include <Microsoft.Graphics.Canvas.h>

	using namespace Microsoft::Graphics::Canvas::Effects;
	namespace EffectsAbi = ABI::Microsoft::Graphics::Canvas::Effects;

#else

	using namespace Windows::Graphics::Effects;
	namespace EffectsAbi = ABI::Windows::Graphics::Effects;

#endif

using EffectsAbi::IGraphicsEffectD2D1Interop;

namespace NativeComponent
{
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
	[Windows::Foundation::Metadata::WebHostHidden]
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
