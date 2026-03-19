// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "generated\ArithmeticCompositeEffect.h"
#include "generated\ColorManagementEffect.h"
#include "generated\HighlightsAndShadowsEffect.h"


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


    // ColorManagementEffect has a custom GPU caps query.
    IFACEMETHODIMP ColorManagementEffectFactory::IsBestQualitySupported(ICanvasDevice* device, boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(device);
            CheckInPointer(result);

            // Best quality mode requires floating point buffer precision.
            boolean isFloatPrecisionSupported;
            ThrowIfFailed(device->IsBufferPrecisionSupported(CanvasBufferPrecision::Precision32Float, &isFloatPrecisionSupported));

            // It also requires at least D3D feature level 10.
            ComPtr<ID3D11Device> d3dDevice;
            ThrowIfFailed(As<IDirect3DDxgiInterfaceAccess>(device)->GetInterface(IID_PPV_ARGS(&d3dDevice)));
            auto isFeatureLevel10 = d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0;

            *result = isFloatPrecisionSupported && isFeatureLevel10;
        });
    }


    // HighlightsAndShadowsEffect.SourceIsLinearGamma property needs special conversion,
    // because we project the D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA enum as a bool.

    IFACEMETHODIMP HighlightsAndShadowsEffect::get_SourceIsLinearGamma(_Out_ boolean* value)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(value);
            D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA d2dValue;
            GetBoxedProperty<uint32_t>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, &d2dValue);
            *value = (d2dValue == D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR);
        });
    }

    IFACEMETHODIMP HighlightsAndShadowsEffect::put_SourceIsLinearGamma(_In_ boolean value)
    {
        return ExceptionBoundary([&]
        {
            D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA d2dValue = value ? D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR : D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB;
            SetBoxedProperty<uint32_t>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, d2dValue);
        });
    }

    IMPLEMENT_EFFECT_PROPERTY_MAPPING_HANDCODED(HighlightsAndShadowsEffect,
        { L"SourceIsLinearGamma", D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN })

}}}}}
