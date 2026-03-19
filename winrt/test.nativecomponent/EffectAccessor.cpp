// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// Exposes features of IGraphicsEffectD2D1Interop and the Microsoft.Graphics.Canvas.native.h
// interop mechanism for use by test.managed.EffectTests. This cross language testing approach
// is used because:
//
//  1)  C# reflection is the easiest way to apply automated test logic across the entire set of
//      strongly typed effect variants, validating the marshalling of all source parameters and
//      effect properties without having to manually write separate tests for each effect type.
//
//  2)  Some aspects of this validation require access to interop functionality that is not
//      exposed in .NET.
//
// Because of 1), the reflection based effect tests are written in C# (test.managed.EffectTests).
// To satisfy 2), this module exposes the neccessary interop features for use by the C# tests.

#include "pch.h"
#include <Microsoft.Graphics.Canvas.native.h>

using namespace Windows::Graphics::Effects;
namespace EffectsAbi = ABI::Windows::Graphics::Effects;

using EffectsAbi::IGraphicsEffectD2D1Interop;

using namespace Microsoft::WRL;
using namespace Microsoft::Graphics::Canvas;

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

        static void RealizeEffect(CanvasDevice^ device, IGraphicsEffect^ effect)
        {
            int originalSourceCount = GetSourceCount(effect);

            auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect);

            // Before and after, Win2D + D2D should all agree on how many inputs this effect has.
            int newSourceCount = GetSourceCount(effect);
            int d2dInputCount = d2dEffect->GetInputCount();

            if (originalSourceCount != newSourceCount ||
                originalSourceCount != d2dInputCount)
            {
                ThrowHR(E_FAIL);
            }
        }

        static IGraphicsEffect^ CreateThenWrapNewEffectOfSameType(CanvasDevice^ device, IGraphicsEffect^ originalEffect)
        {
            // What type of effect is this?
            IID effectId;
            ThrowIfFailed(As<IGraphicsEffectD2D1Interop>(originalEffect)->GetEffectId(&effectId));

            // Create a new D2D effect of the same type.
            auto commandList = ref new CanvasCommandList(device);
            auto drawingSession = commandList->CreateDrawingSession();
            auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

            ComPtr<ID2D1Effect> d2dEffect;
            ThrowIfFailed(d2dContext->CreateEffect(effectId, &d2dEffect));

            // Wrap a Win2D effect around the D2D effect.
            return GetOrCreate<IGraphicsEffect>(device, d2dEffect.Get());
        }
    };
}
