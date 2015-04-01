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
#include "generated\ArithmeticCompositeEffect.h"
#include "generated\ColorMatrixEffect.h"


// Macro defines effect property get and set methods that pack a floating point 
// value into just one component of a Vector4. This allows us to expand properties 
// which D2D defines as a single Vector4, exposing them to WinRT as 4 separate floats.

#define IMPLEMENT_PACKED_VECTOR_PROPERTY(CLASS_NAME, PROPERTY_NAME,                         \
                                         PROPERTY_INDEX, VECTOR_COMPONENT)                  \
                                                                                            \
    IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ float* value)                      \
    {                                                                                       \
        return ExceptionBoundary([&]                                                        \
        {                                                                                   \
            CheckInPointer(value);                                                          \
            Numerics::Vector4 packedValue;                                                  \
            GetBoxedProperty<float[4], Numerics::Vector4>(PROPERTY_INDEX, &packedValue);    \
            *value = packedValue.VECTOR_COMPONENT;                                          \
        });                                                                                 \
    }                                                                                       \
                                                                                            \
    IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ float value)                        \
    {                                                                                       \
        return ExceptionBoundary([&]                                                        \
        {                                                                                   \
            Numerics::Vector4 packedValue;                                                  \
            GetBoxedProperty<float[4], Numerics::Vector4>(PROPERTY_INDEX, &packedValue);    \
            packedValue.VECTOR_COMPONENT = value;                                           \
            SetBoxedProperty<float[4], Numerics::Vector4>(PROPERTY_INDEX, packedValue);     \
        });                                                                                 \
    }


// Customized effect property implementations.
// Unlike most of the effect method bodies, these are not codegenned.
namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    // Expose the ArithmeticCompositeEffect Coefficients property (type Vector4) as 4 separate floats.

    IMPLEMENT_PACKED_VECTOR_PROPERTY(ArithmeticCompositeEffect,
        MultiplyAmount, 
        D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, 
        X)

    IMPLEMENT_PACKED_VECTOR_PROPERTY(ArithmeticCompositeEffect, 
        Source1Amount, 
        D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, 
        Y)

    IMPLEMENT_PACKED_VECTOR_PROPERTY(ArithmeticCompositeEffect, 
        Source2Amount, 
        D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, 
        Z)

    IMPLEMENT_PACKED_VECTOR_PROPERTY(ArithmeticCompositeEffect, 
        Offset, 
        D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, 
        W)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING_HANDCODED(ArithmeticCompositeEffect,
        { L"MultiplyAmount", D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX },
        { L"Source1Amount",  D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY },
        { L"Source2Amount",  D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ },
        { L"Offset",         D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW })


    // ColorMatrixEffect.AlphaMode property needs special enum value conversion, because our
    // CanvasAlphaMode enum doesn't match the numeric values of D2D1_COLORMATRIX_ALPHA_MODE.

    IFACEMETHODIMP ColorMatrixEffect::get_AlphaMode(_Out_ CanvasAlphaMode* value)
    {
        return ExceptionBoundary([&]
        {
            D2D1_COLORMATRIX_ALPHA_MODE d2dValue;
            GetBoxedProperty<uint32_t>(D2D1_COLORMATRIX_PROP_ALPHA_MODE, &d2dValue);
            *value = FromD2DColorMatrixAlphaMode(d2dValue);
        });
    }

    IFACEMETHODIMP ColorMatrixEffect::put_AlphaMode(_In_ CanvasAlphaMode value)
    {
        if (value == CanvasAlphaMode::Ignore)
        {
            return E_INVALIDARG;
        }

        return ExceptionBoundary([&]
        {
            SetBoxedProperty<uint32_t>(D2D1_COLORMATRIX_PROP_ALPHA_MODE, ToD2DColorMatrixAlphaMode(value));
        });
    }

    IMPLEMENT_EFFECT_PROPERTY_MAPPING_HANDCODED(ColorMatrixEffect,
        { L"AlphaMode", D2D1_COLORMATRIX_PROP_ALPHA_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE })

}}}}}
