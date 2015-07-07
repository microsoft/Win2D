// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Platform;

#ifdef USE_LOCALLY_EMULATED_UAP_APIS
namespace EffectsAbi = ABI::Microsoft::Graphics::Canvas::Effects;
#else
namespace EffectsAbi = ABI::Windows::Graphics::Effects;
#endif

using EffectsAbi::IGraphicsEffectD2D1Interop;

TEST_CLASS(CanvasBitmapTests)
{
    TEST_METHOD(CanvasEffect_IGraphicsEffectD2D1Interop)
    {
        // Use gaussian blur for testing IGraphicsEffectD2D1Interop interface
        auto blurEffect = ref new GaussianBlurEffect();
        auto blurInterop = As<IGraphicsEffectD2D1Interop>(blurEffect);

        GUID id;
        ThrowIfFailed(blurInterop->GetEffectId(&id));
        Assert::IsTrue(id == CLSID_D2D1GaussianBlur);
        
        // Check sources initial vector
        UINT count;
        ThrowIfFailed(blurInterop->GetSourceCount(&count));
        Assert::AreEqual(1U, count);

        ComPtr<EffectsAbi::IGraphicsEffectSource> source;
        ThrowIfFailed(blurInterop->GetSource(0, &source));
        Assert::IsNull(source.Get());

        // Check initial properties vector
        ThrowIfFailed(blurInterop->GetPropertyCount(&count));
        Assert::AreEqual(3U, count);

        ComPtr<ABI::Windows::Foundation::IPropertyValue> prop;

        for (unsigned int i = 0; i < 3U; ++i)
        {
            ThrowIfFailed(blurInterop->GetProperty(i, &prop));
            Assert::IsNotNull(prop.Get());
        }

        // Check null parameters.
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetEffectId(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetSourceCount(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetSource(0, nullptr));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetPropertyCount(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetProperty(0, nullptr));

        // Check out of range accesses.
        Assert::AreEqual(E_BOUNDS, blurInterop->GetSource((unsigned)-1, &source));
        Assert::AreEqual(E_BOUNDS, blurInterop->GetSource(1, &source));

        Assert::AreEqual(E_BOUNDS, blurInterop->GetProperty((unsigned)-1, &prop));
        Assert::AreEqual(E_BOUNDS, blurInterop->GetProperty(3, &prop));
    }

    TEST_METHOD(CanvasEffect_FloatProperty)
    {
        // Use gaussian blur StandardDeviation float property for testing
        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();

        // Check float property access through strongly typed interface
        Assert::AreEqual(3.0f, blurEffect->BlurAmount);
        float newValue = 5.0f;
        blurEffect->BlurAmount = newValue;
        Assert::AreEqual(newValue, blurEffect->BlurAmount);

        // Check that IGraphicsEffectD2D1Interop interface connects to the same data
        auto blurInterop = As<IGraphicsEffectD2D1Interop>(blurEffect);

        ComPtr<ABI::Windows::Foundation::IPropertyValue> prop;
        ThrowIfFailed(blurInterop->GetProperty(0, &prop));

        float propValue = 0;
        ThrowIfFailed(prop->GetSingle(&propValue));
        Assert::AreEqual(newValue, propValue);

        // Try setting not valid data. 
        // For StandardDeviation in GaussianBlur valid range is (0.0f;250.0f)
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                blurEffect->BlurAmount = -1.0f;
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                blurEffect->BlurAmount = 300.0f;
            });
    }

    TEST_METHOD(CanvasEffect_EnumProperty)
    {
        // Use gaussian blur Optimization enum property for testing
        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();

        // Check enum property access through strongly typed interface
        Assert::IsTrue(EffectOptimization::Balanced == blurEffect->Optimization);
        EffectOptimization newValue = EffectOptimization::Speed;
        blurEffect->Optimization = newValue;
        Assert::IsTrue(newValue == blurEffect->Optimization);

        // Check that IGraphicsEffectD2D1Interop interface connects to the same data
        auto blurInterop = As<IGraphicsEffectD2D1Interop>(blurEffect);

        ComPtr<ABI::Windows::Foundation::IPropertyValue> prop;
        ThrowIfFailed(blurInterop->GetProperty(1, &prop));

        unsigned propValue = 0;
        ThrowIfFailed(prop->GetUInt32(&propValue));
        Assert::AreEqual((unsigned)newValue, propValue);
    }

    TEST_METHOD(CanvasEffect_UnsupportedEnums)
    {
        // Use Transform3DEffect effect for testing unsupported enums
        Transform3DEffect^ transformEffect = ref new Transform3DEffect();

        transformEffect->InterpolationMode = CanvasImageInterpolation::Anisotropic;
        Assert::IsTrue(CanvasImageInterpolation::Anisotropic == transformEffect->InterpolationMode);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                transformEffect->InterpolationMode = CanvasImageInterpolation::HighQualityCubic;
            });

        // Check that transformEffect did not change it's interpolation mode
        Assert::IsTrue(CanvasImageInterpolation::Anisotropic == transformEffect->InterpolationMode);
    }

    TEST_METHOD(CanvasEffect_Matrix4x4Propery)
    {
        // Use Transform3D TransformMatrix property for testing
        Transform3DEffect^ transformEffect = ref new Transform3DEffect();

        // Check enum property access through strongly typed interface
        float4x4 matrix;
        float newValue = 5.0f;
        matrix.m22 = newValue;
        transformEffect->TransformMatrix = matrix;
        Assert::AreEqual(newValue, static_cast<float4x4>(transformEffect->TransformMatrix).m22);

        // Check that IGraphicsEffectD2D1Interop interface connects to the same data
        auto transformInterop = As<IGraphicsEffectD2D1Interop>(transformEffect);

        // Transformation matrix is propery #2 in property array
        ComPtr<ABI::Windows::Foundation::IPropertyValue> prop;
        ThrowIfFailed(transformInterop->GetProperty(2, &prop));

        ComArray<float> propertyArray;
        ThrowIfFailed(prop->GetSingleArray(propertyArray.GetAddressOfSize(), propertyArray.GetAddressOfData()));

        Assert::AreEqual(16u, propertyArray.GetSize());

        Assert::AreEqual(matrix.m11, propertyArray[0]);
        Assert::AreEqual(matrix.m12, propertyArray[1]);
        Assert::AreEqual(matrix.m13, propertyArray[2]);
        Assert::AreEqual(matrix.m14, propertyArray[3]);

        Assert::AreEqual(matrix.m21, propertyArray[4]);
        Assert::AreEqual(matrix.m22, propertyArray[5]);
        Assert::AreEqual(matrix.m23, propertyArray[6]);
        Assert::AreEqual(matrix.m24, propertyArray[7]);

        Assert::AreEqual(matrix.m31, propertyArray[8]);
        Assert::AreEqual(matrix.m32, propertyArray[9]);
        Assert::AreEqual(matrix.m33, propertyArray[10]);
        Assert::AreEqual(matrix.m34, propertyArray[11]);

        Assert::AreEqual(matrix.m41, propertyArray[12]);
        Assert::AreEqual(matrix.m42, propertyArray[13]);
        Assert::AreEqual(matrix.m43, propertyArray[14]);
        Assert::AreEqual(matrix.m44, propertyArray[15]);
    }

    TEST_METHOD(CanvasEffect_LimitedSources)
    {
        // Use gaussian for testing limited sources
        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();

        Assert::IsNull(blurEffect->Source);

        // Set same effect as source for reference testing
        blurEffect->Source = blurEffect;
        Assert::IsTrue(blurEffect == blurEffect->Source);

        // Check that IGraphicsEffectD2D1Interop interface connects to the same data
        auto blurInterop = As<IGraphicsEffectD2D1Interop>(blurEffect);

        unsigned count;
        ThrowIfFailed(blurInterop->GetSourceCount(&count));
        Assert::AreEqual(1U, count);

        ComPtr<EffectsAbi::IGraphicsEffectSource> source;
        ThrowIfFailed(blurInterop->GetSource(0, &source));
        Assert::IsTrue(IsSameInstance(reinterpret_cast<IInspectable*>(blurEffect), source.Get()));
    }

    TEST_METHOD(CanvasEffect_UnlimitedSources)
    {
        // Use Composite effect for testing unlimited sources
        CompositeEffect^ compositeEffect = ref new CompositeEffect();

        // All unlimited sources supposed to have 0 size sources vector by default
        Assert::AreEqual(0U, compositeEffect->Sources->Size);

        // Set same effect as source for reference testing
        unsigned int sourceNumber = 10;
        for (unsigned int i = 0; i < sourceNumber; i++)
        {
            compositeEffect->Sources->Append(compositeEffect);
        }
        Assert::AreEqual(sourceNumber, compositeEffect->Sources->Size);
        for (unsigned int i = 0; i < sourceNumber; i++)
        {
            Assert::IsTrue(compositeEffect == compositeEffect->Sources->GetAt(i));
        }
    }

    TEST_METHOD(CanvasEffect_GetNamedPropertyMapping)
    {
        using EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING;
        using EffectsAbi::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();
        auto blurInterop = As<IGraphicsEffectD2D1Interop>(blurEffect);

        UINT index = static_cast<UINT>(-1);
        GRAPHICS_EFFECT_PROPERTY_MAPPING mapping = static_cast<GRAPHICS_EFFECT_PROPERTY_MAPPING>(-1);

        // Look up properties that exist.
        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"BlurAmount", &index, &mapping));

        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, index);
        Assert::AreEqual<UINT>(GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT, mapping);

        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"Optimization", &index, &mapping));

        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, index);
        Assert::AreEqual<UINT>(GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT, mapping);

        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"BorderMode", &index, &mapping));

        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, index);
        Assert::AreEqual<UINT>(GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT, mapping);

        // Property look up should be case-insensitive.
        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"bluramount", &index, &mapping));
        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, index);

        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"OPTIMIZATION", &index, &mapping));
        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, index);

        ThrowIfFailed(blurInterop->GetNamedPropertyMapping(L"bORdERmODE", &index, &mapping));
        Assert::AreEqual<UINT>(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, index);

        // Look up properties that do not exist.
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"xBlurAmount", &index, &mapping));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"BlurAmountx", &index, &mapping));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"!@#$%^&*", &index, &mapping));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"", &index, &mapping));

        // Null parameters.
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(nullptr, &index, &mapping));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"BlurAmount", nullptr, &mapping));
        Assert::AreEqual(E_INVALIDARG, blurInterop->GetNamedPropertyMapping(L"BlurAmount", &index, nullptr));
    }
};
