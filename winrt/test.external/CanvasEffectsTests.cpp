// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::UI;
using namespace Windows::Devices::Enumeration;
using namespace Platform;

namespace EffectsAbi = ABI::Windows::Graphics::Effects;

using EffectsAbi::IGraphicsEffectD2D1Interop;

TEST_CLASS(CanvasEffectsTests)
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

    TEST_METHOD(CanvasEffect_WhenClosed_AccessingSourcesCollectionThrows)
    {
        auto effect = ref new CompositeEffect();
        auto sources = effect->Sources;

        delete effect;

        EnsureCollectionAccessorsThrowObjectDisposed(sources);
    }

    TEST_METHOD(CanvasEffect_WhenReleased_AccessingSourcesCollectionThrows)
    {
        auto effect = ref new CompositeEffect();
        auto sources = effect->Sources;

        effect = nullptr;

        EnsureCollectionAccessorsThrowObjectDisposed(sources);
    }

    static void EnsureCollectionAccessorsThrowObjectDisposed(IVector<IGraphicsEffectSource^>^ sources)
    {
        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->Size;
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->GetAt(0);
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->SetAt(0, nullptr);
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->InsertAt(0, nullptr);
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->RemoveAt(0);
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->Append(nullptr);
        });

        Assert::ExpectException<Platform::ObjectDisposedException^>([&]
        {
            sources->Clear();
        });
    }

    TEST_METHOD(CanvasEffect_SourcesCollection_GetAndSet)
    {
        auto device = ref new CanvasDevice();
        auto effect = ref new CompositeEffect();

        auto bitmap1 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap2 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap3 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);

        effect->Sources->Append(bitmap1);
        effect->Sources->Append(bitmap2);

        // Test using an unrealized effect.
        Assert::AreEqual(2u, effect->Sources->Size);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap1, effect->Sources->GetAt(0));
        Assert::AreEqual<IGraphicsEffectSource>(bitmap2, effect->Sources->GetAt(1));

        effect->Sources->SetAt(0, bitmap3);
        effect->Sources->SetAt(1, nullptr);

        Assert::AreEqual(2u, effect->Sources->Size);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap3, effect->Sources->GetAt(0));
        Assert::IsNull(effect->Sources->GetAt(1));

        Assert::ExpectException<Platform::OutOfBoundsException^>([&]
        {
            effect->Sources->GetAt(2);
        });

        Assert::ExpectException<Platform::OutOfBoundsException^>([&]
        {
            effect->Sources->SetAt(2, nullptr);
        });

        // Test using a realized effect.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI);

        effect->Sources->SetAt(0, nullptr);
        effect->Sources->SetAt(1, bitmap3);

        Assert::AreEqual(2u, effect->Sources->Size);
        Assert::IsNull(effect->Sources->GetAt(0));
        Assert::AreEqual<IGraphicsEffectSource>(bitmap3, effect->Sources->GetAt(1));

        Assert::ExpectException<Platform::OutOfBoundsException^>([&]
        {
            effect->Sources->GetAt(2);
        });

        Assert::ExpectException<Platform::OutOfBoundsException^>([&]
        {
            effect->Sources->SetAt(2, nullptr);
        });

        // Make sure we are still realized.
        Assert::AreEqual(effect, GetOrCreate<CompositeEffect>(d2dEffect.Get()));
    }

    TEST_METHOD(CanvasEffect_SourcesCollection_InsertSource)
    {
        auto device = ref new CanvasDevice();
        auto effect = ref new CompositeEffect();

        auto bitmap1 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap2 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap3 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);

        ComPtr<ID2D1Effect> d2dEffect;

        // First time test an unrealized effect, second time a realized one.
        for (int pass = 0; pass < 2; pass++)
        {
            effect->Sources->InsertAt(0, bitmap2);

            if (pass > 0)
            {
                // Realize the effect (not possible while it has zero sources, so this comes after the first InsertAt).
                d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI);
            }

            effect->Sources->InsertAt(0, nullptr);
            effect->Sources->InsertAt(2, bitmap3);
            effect->Sources->InsertAt(1, bitmap1);

            Assert::AreEqual(4u, effect->Sources->Size);
            Assert::IsNull(effect->Sources->GetAt(0));
            Assert::AreEqual<IGraphicsEffectSource>(bitmap1, effect->Sources->GetAt(1));
            Assert::AreEqual<IGraphicsEffectSource>(bitmap2, effect->Sources->GetAt(2));
            Assert::AreEqual<IGraphicsEffectSource>(bitmap3, effect->Sources->GetAt(3));

            Assert::ExpectException<Platform::OutOfBoundsException^>([&]
            {
                effect->Sources->InsertAt(5, nullptr);
            });

            if (pass > 0)
            {
                // Make sure we are still realized (before the Clear, which will unrealize).
                Assert::AreEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));
            }

            effect->Sources->Clear();

            Assert::AreEqual(0u, effect->Sources->Size);

            if (pass > 0)
            {
                // Becoming empty should have unrealized the effect.
                Assert::AreNotEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));
            }
        }
    }

    TEST_METHOD(CanvasEffect_SourcesCollection_RemoveSource)
    {
        auto device = ref new CanvasDevice();
        auto effect = ref new CompositeEffect();

        auto bitmap1 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap2 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto bitmap3 = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        
        ComPtr<ID2D1Effect> d2dEffect;

        // First time test an unrealized effect, second time a realized one.
        for (int pass = 0; pass < 2; pass++)
        {
            effect->Sources->Append(bitmap1);

            if (pass > 0)
            {
                // Realize the effect (not possible while it has zero sources, so this comes after the first Append).
                d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI);
            }

            effect->Sources->Append(bitmap2);
            effect->Sources->Append(bitmap3);

            Assert::ExpectException<Platform::OutOfBoundsException^>([&]
            {
                effect->Sources->RemoveAt(3);
            });

            effect->Sources->RemoveAt(2);

            Assert::AreEqual(2u, effect->Sources->Size);
            Assert::AreEqual<IGraphicsEffectSource>(bitmap1, effect->Sources->GetAt(0));
            Assert::AreEqual<IGraphicsEffectSource>(bitmap2, effect->Sources->GetAt(1));

            effect->Sources->RemoveAt(0);

            Assert::AreEqual(1u, effect->Sources->Size);
            Assert::AreEqual<IGraphicsEffectSource>(bitmap2, effect->Sources->GetAt(0));

            if (pass > 0)
            {
                // Make sure we are still realized (before the last RemoveAt, which will unrealize).
                Assert::AreEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));
            }

            effect->Sources->RemoveAt(0);

            Assert::AreEqual(0u, effect->Sources->Size);

            if (pass > 0)
            {
                // Becoming empty should have unrealized the effect.
                Assert::AreNotEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));
            }

            Assert::ExpectException<Platform::OutOfBoundsException^>([&]
            {
                effect->Sources->RemoveAt(0);
            });
        }
    }

    TEST_METHOD(CanvasEffect_SetSource_BitmapIsOnWrongDevice)
    {
        auto effect1 = ref new GaussianBlurEffect;
        auto effect2 = ref new GaussianBlurEffect;

        effect1->Source = effect2;

        // Realize the effects onto device1.
        auto device1 = ref new CanvasDevice();
        auto bitmap1 = ref new CanvasRenderTarget(device1, 1, 1, DEFAULT_DPI);

        effect2->Source = bitmap1;

        auto d2dEffect11 = GetWrappedResource<ID2D1Effect>(device1, effect1);
        auto d2dEffect21 = GetWrappedResource<ID2D1Effect>(device1, effect2);

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect11.Get()));
        Assert::AreEqual(effect2, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect21.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
        Assert::AreEqual<IGraphicsEffectSource>(effect2->Source, bitmap1);

        // Set a source bitmap that is on a different device.
        auto device2 = ref new CanvasDevice();
        auto bitmap2 = ref new CanvasRenderTarget(device2, 1, 1, DEFAULT_DPI);

        effect2->Source = bitmap2;

        // This should have unrealized effect2, but not effect1.
        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect11.Get()));
        Assert::AreNotEqual(effect2, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect21.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
        Assert::AreEqual<IGraphicsEffectSource>(effect2->Source, bitmap2);

        // Realize both effects on to device2.
        auto d2dEffect22 = GetWrappedResource<ID2D1Effect>(device2, effect2);
        auto d2dEffect12 = GetWrappedResource<ID2D1Effect>(device2, effect1);

        Assert::IsFalse(IsSameInstance(d2dEffect11.Get(), d2dEffect12.Get()));
        Assert::IsFalse(IsSameInstance(d2dEffect21.Get(), d2dEffect22.Get()));

        // It should not be possible to realize onto device1, as the bitmap does not match.
        ExpectCOMException(E_INVALIDARG, L"Effect source #0 is associated with a different device.",
            [&]
            {
                GetWrappedResource<ID2D1Effect>(device1, effect1);
            });

        ExpectCOMException(E_INVALIDARG, L"Effect source #0 is associated with a different device.",
            [&]
            {
                GetWrappedResource<ID2D1Effect>(device1, effect2);
            });

        // Failed realizations attempt should not have messed up the source links.
        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
        Assert::AreEqual<IGraphicsEffectSource>(effect2->Source, bitmap2);

        // But it should have unrealized the effects.
        Assert::AreNotEqual(effect1, GetOrCreate<GaussianBlurEffect>(device2, d2dEffect12.Get()));
        Assert::AreNotEqual(effect2, GetOrCreate<GaussianBlurEffect>(device2, d2dEffect22.Get()));

        // Rerealize both effects back on to device2 again.
        auto d2dEffect23 = GetWrappedResource<ID2D1Effect>(device2, effect2);
        auto d2dEffect13 = GetWrappedResource<ID2D1Effect>(device2, effect1);

        // What if we set an unrealized effect, whose input is a bitmap created from device 1?
        // This must unrealize the target effect, rather than trying to realize the source.
        auto effect3 = ref new GaussianBlurEffect;
        effect3->Source = bitmap1;

        effect2->Source = effect3;

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device2, d2dEffect13.Get()));
        Assert::AreNotEqual(effect2, GetOrCreate<GaussianBlurEffect>(device2, d2dEffect23.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
        Assert::AreEqual<IGraphicsEffectSource>(effect2->Source, effect3);
        Assert::AreEqual<IGraphicsEffectSource>(effect3->Source, bitmap1);

        // Now we can realize onto device1.
        auto d2dEffect14 = GetWrappedResource<ID2D1Effect>(device1, effect1);

        Assert::IsFalse(IsSameInstance(d2dEffect11.Get(), d2dEffect13.Get()));
        Assert::IsFalse(IsSameInstance(d2dEffect12.Get(), d2dEffect13.Get()));

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect14.Get()));

        // But we cannot realize onto device2.
        ExpectCOMException(E_INVALIDARG, L"Effect source #0 is associated with a different device.",
            [&]
            {
                GetWrappedResource<ID2D1Effect>(device2, effect1);
            });

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
        Assert::AreEqual<IGraphicsEffectSource>(effect2->Source, effect3);
        Assert::AreEqual<IGraphicsEffectSource>(effect3->Source, bitmap1);
    }

    TEST_METHOD(CanvasEffect_GetResource)
    {
        auto blurEffect = ref new GaussianBlurEffect();
        auto device = ref new CanvasDevice();
        auto source = ref new ColorSourceEffect();

        const float expectedBlurAmount = 23;
        blurEffect->BlurAmount = expectedBlurAmount;

        blurEffect->Source = source;

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, blurEffect, DEFAULT_DPI);

        // Make sure we got back a sensible looking effect.
        Assert::IsTrue(d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1GaussianBlur);
        Assert::AreEqual(expectedBlurAmount, d2dEffect->GetValue<float>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION));

        ComPtr<ID2D1Image> input;
        d2dEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), GetWrappedResource<ID2D1Image>(device, source).Get()));

        // Repeated queries should be idempotent.
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device, blurEffect);
        Assert::IsTrue(IsSameInstance(d2dEffect.Get(), d2dEffect2.Get()));

        // Reverse lookups should give back the object we started with.
        auto effect2 = GetOrCreate<GaussianBlurEffect>(d2dEffect.Get());
        Assert::AreEqual(blurEffect, effect2);

        // Realizing onto a different device should return a different D2D instance.
        auto device2 = ref new CanvasDevice();
        auto d2dEffect3 = GetWrappedResource<ID2D1Effect>(device2, blurEffect);
        Assert::IsFalse(IsSameInstance(d2dEffect.Get(), d2dEffect3.Get()));
    }

    TEST_METHOD(CanvasEffect_GetResource_NoDevice)
    {
        auto blurEffect = ref new GaussianBlurEffect();

        ExpectCOMException(E_INVALIDARG, L"To unwrap this resource type, a device parameter must be passed to GetWrappedResource.",
            [&]
            {
                GetWrappedResource<ID2D1Effect>(blurEffect);
            });
    }

    TEST_METHOD(CanvasEffect_GetResource_NoSources)
    {
        auto device = ref new CanvasDevice();
        auto compositeEffect = ref new CompositeEffect();

        ExpectCOMException(E_INVALIDARG, L"Effect Sources collection is empty.",
            [&]
            {
                GetWrappedResource<ID2D1Effect>(device, compositeEffect);
            });

        // But this is ok.
        compositeEffect->Sources->Append(ref new ColorSourceEffect());

        GetWrappedResource<ID2D1Effect>(device, compositeEffect);
    }

    TEST_METHOD(CanvasEffect_GetResource_NullSource)
    {
        auto device = ref new CanvasDevice();

        // Should be able to realize the effect even though its input is null.
        auto blurEffect = ref new GaussianBlurEffect();

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, blurEffect);

        Assert::IsTrue(d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1GaussianBlur);

        ComPtr<ID2D1Image> input;
        d2dEffect->GetInput(0, &input);
        Assert::IsNull(input.Get());

        // Ditto if the null source is 2 levels down the graph.
        auto linearTransferEffect = ref new LinearTransferEffect();
        auto morphologyEffect = ref new MorphologyEffect();

        linearTransferEffect->Source = morphologyEffect;

        d2dEffect = GetWrappedResource<ID2D1Effect>(device, linearTransferEffect);

        Assert::IsTrue(d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1LinearTransfer);

        d2dEffect->GetInput(0, &input);
        d2dEffect = As<ID2D1Effect>(input);
        Assert::IsTrue(d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1Morphology);

        d2dEffect->GetInput(0, &input);
        Assert::IsNull(input.Get());
    }

    TEST_METHOD(CanvasEffect_GetResource_DpiCompensation)
    {
        auto blurEffect = ref new GaussianBlurEffect();
        auto device = ref new CanvasDevice();
        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        blurEffect->Source = bitmap;

        // Requesting different DPI from the bitmap should insert DPI compenstation.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, blurEffect, DEFAULT_DPI + 1);

        ComPtr<ID2D1Image> input;
        d2dEffect->GetInput(0, &input);
        auto dpiEffect = As<ID2D1Effect>(input);
        Assert::IsTrue(dpiEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1DpiCompensation);

        dpiEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), GetWrappedResource<ID2D1Image>(bitmap).Get()));

        // Requesting the same DPI as the bitmap should not insert DPI compensation.
        d2dEffect = GetWrappedResource<ID2D1Effect>(device, blurEffect, DEFAULT_DPI);

        d2dEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), GetWrappedResource<ID2D1Image>(bitmap).Get()));

        // Not specifying DPI should insert DPI compenstation.
        d2dEffect = GetWrappedResource<ID2D1Effect>(device, blurEffect);

        d2dEffect->GetInput(0, &input);
        dpiEffect = As<ID2D1Effect>(input);
        Assert::IsTrue(dpiEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1DpiCompensation);

        dpiEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), GetWrappedResource<ID2D1Image>(bitmap).Get()));
    }

    TEST_METHOD(CanvasEffect_GetOrCreate)
    {
        auto device = ref new CanvasDevice();
        auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto drawingSession = renderTarget->CreateDrawingSession();
        auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

        // Create and configure a D2D effect instance.
        ComPtr<ID2D1Effect> d2dEffect;
        ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &d2dEffect));

        const float expectedBlurAmount = 23;
        d2dEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, expectedBlurAmount);

        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        d2dEffect->SetInput(0, d2dBitmap.Get());

        // Wrap a Win2D object around the D2D effect.
        auto effect = GetOrCreate<GaussianBlurEffect>(device, d2dEffect.Get());

        Assert::AreEqual(expectedBlurAmount, effect->BlurAmount);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap, effect->Source);

        // Repeated lookups should be idempotent.
        auto effect2 = GetOrCreate<ICanvasImage>(d2dEffect.Get());
        Assert::AreEqual<ICanvasImage>(effect, effect2);

        // Reverse lookups should give back the object we started with.
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device, effect);
        Assert::IsTrue(IsSameInstance(d2dEffect.Get(), d2dEffect2.Get()));
    }

    TEST_METHOD(CanvasEffect_GetOrCreate_SourceHasNoExistingWrapper)
    {
        auto device = ref new CanvasDevice();
        auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto drawingSession = renderTarget->CreateDrawingSession();
        auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

        // Create and configure a D2D effect instance.
        ComPtr<ID2D1Effect> d2dEffect;
        ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &d2dEffect));

        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        d2dEffect->SetInput(0, d2dBitmap.Get());

        // Release the Win2D wrapper, so the source bitmap only exists in the land of native D2D.
        bitmap = nullptr;

        // Trying to wrap the effect without specifying a device should throw,
        // because wrapping its source bitmap requires a device.
        ExpectCOMException(E_INVALIDARG, L"To wrap this resource type, a device parameter must be passed to GetOrCreate.",
            [&]
            {
                GetOrCreate<GaussianBlurEffect>(d2dEffect.Get());
            });

        // Wrap a Win2D object around the D2D effect, now specifying the device.
        auto effect = GetOrCreate<GaussianBlurEffect>(device, d2dEffect.Get());

        // Now that we have a wrapper, looking it up a second time without specifying the device is fine.
        auto effect2 = GetOrCreate<GaussianBlurEffect>(d2dEffect.Get());
        Assert::AreEqual(effect, effect2);

        // A new wrapper for the source bitmap should have been automatically created.
        Assert::IsNotNull(effect->Source);
        bitmap = GetOrCreate<CanvasRenderTarget>(d2dBitmap.Get());
        Assert::AreEqual<IGraphicsEffectSource>(bitmap, effect->Source);
    }

    TEST_METHOD(CanvasEffect_GetOrCreate_VariableNumberOfInputs)
    {
        auto device = ref new CanvasDevice();
        auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto drawingSession = renderTarget->CreateDrawingSession();
        auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

        for (unsigned inputCount = 1; inputCount <= 3; inputCount++)
        {
            // Create and configure a D2D effect instance.
            ComPtr<ID2D1Effect> d2dEffect;
            ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D1Composite, &d2dEffect));

            d2dEffect->SetInputCount(inputCount);

            std::vector<CanvasRenderTarget^> bitmaps;
            std::vector<ComPtr<ID2D1Bitmap>> d2dBitmaps;

            for (unsigned i = 0; i < inputCount; i++)
            {
                bitmaps.emplace_back(ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI));
                d2dBitmaps.emplace_back(GetWrappedResource<ID2D1Bitmap>(bitmaps.back()));
                d2dEffect->SetInput(i, d2dBitmaps.back().Get());
            }

            // Wrap a Win2D object around the D2D effect, and validate its inputs.
            auto effect = GetOrCreate<CompositeEffect>(device, d2dEffect.Get());

            Assert::AreEqual(inputCount, effect->Sources->Size);

            for (unsigned i = 0; i < inputCount; i++)
            {
                Assert::AreEqual<IGraphicsEffectSource>(bitmaps[i], effect->Sources->GetAt(i));
            }
        }
    }

    TEST_METHOD(CanvasEffect_GetOrCreate_UnknownEffectType)
    {
        auto device = ref new CanvasDevice();
        auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto drawingSession = renderTarget->CreateDrawingSession();
        auto d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);

        // Create and configure a D2D effect instance using an effect type that Win2D does not support.
        ComPtr<ID2D1Effect> d2dEffect;
        ThrowIfFailed(d2dContext->CreateEffect(CLSID_D2D13DPerspectiveTransform, &d2dEffect));

        // Try to wrap a Win2D object around the unsupported D2D effect.
        ExpectCOMException(E_NOINTERFACE, L"Unsupported type. Win2D is not able to wrap the specified resource.",
            [&]
            {
                GetOrCreate<ICanvasImage>(device, d2dEffect.Get());
            });
    }

    TEST_METHOD(CanvasEffect_InteropChangesProperty_IsBidirectional)
    {
        auto device = ref new CanvasDevice();
        auto image = ref new CanvasCommandList(device);
        auto effect = ref new PointSpecularEffect();
        effect->Source = image;

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect);

        // Change properties via Win2D.
        effect->SpecularExponent = 3;
        effect->SpecularAmount = 0.5f;
        effect->HeightMapInterpolationMode = CanvasImageInterpolation::NearestNeighbor;
        effect->LightColor = Colors::Yellow;
        effect->LightPosition = float3(1, 2, 3);

        // D2D should immediately see the changes.
        Assert::AreEqual(3.0f, d2dEffect->GetValue<float>(D2D1_POINTSPECULAR_PROP_SPECULAR_EXPONENT));
        Assert::AreEqual(0.5f, d2dEffect->GetValue<float>(D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT));
        Assert::AreEqual(static_cast<UINT32>(D2D1_POINTSPECULAR_SCALE_MODE_NEAREST_NEIGHBOR), d2dEffect->GetValue<UINT32>(D2D1_POINTSPECULAR_PROP_SCALE_MODE));
        Assert::AreEqual(float3(1, 1, 0), d2dEffect->GetValue<float3>(D2D1_POINTSPECULAR_PROP_COLOR));
        Assert::AreEqual(float3(1, 2, 3), d2dEffect->GetValue<float3>(D2D1_POINTSPECULAR_PROP_LIGHT_POSITION));

        // Changes properties via D2D.
        d2dEffect->SetValue(D2D1_POINTSPECULAR_PROP_SPECULAR_EXPONENT, 7.0f);
        d2dEffect->SetValue(D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT, 2.5f);
        d2dEffect->SetValue(D2D1_POINTSPECULAR_PROP_SCALE_MODE, D2D1_POINTSPECULAR_SCALE_MODE_ANISOTROPIC);
        d2dEffect->SetValue(D2D1_POINTSPECULAR_PROP_COLOR, float3(0, 1, 1));
        d2dEffect->SetValue(D2D1_POINTSPECULAR_PROP_LIGHT_POSITION, float3(4, 5, 6));

        // Win2D should immediately see the changes.
        Assert::AreEqual(7.0f, effect->SpecularExponent);
        Assert::AreEqual(2.5f, effect->SpecularAmount);
        Assert::AreEqual(CanvasImageInterpolation::Anisotropic, effect->HeightMapInterpolationMode);
        Assert::AreEqual(Colors::Cyan, effect->LightColor);
        Assert::AreEqual(float3(4, 5, 6), (float3)effect->LightPosition);
    }

    TEST_METHOD(CanvasEffect_InteropChangesInput_IsBidirectional)
    {
        auto device = ref new CanvasDevice();

        CanvasCommandList^ images[] =
        {
            ref new CanvasCommandList(device),
            ref new CanvasCommandList(device),
            ref new CanvasCommandList(device),
            ref new CanvasCommandList(device),
            ref new CanvasCommandList(device),
        };

        ComPtr<ID2D1CommandList> d2dImages[] =
        {
            GetWrappedResource<ID2D1CommandList>(images[0]),
            GetWrappedResource<ID2D1CommandList>(images[1]),
            GetWrappedResource<ID2D1CommandList>(images[2]),
            GetWrappedResource<ID2D1CommandList>(images[3]),
            GetWrappedResource<ID2D1CommandList>(images[4]),
        };

        for (auto& commandList : d2dImages)
        {
            ThrowIfFailed(commandList->Close());
        }

        auto effect = ref new BlendEffect();

        effect->Background = images[0];
        effect->Foreground = images[0];

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI);

        // Change properties via Win2D.
        effect->Background = images[1];
        effect->Foreground = images[2];

        // D2D should immediately see the changes.
        ComPtr<ID2D1Image> input;

        d2dEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), d2dImages[1].Get()));

        d2dEffect->GetInput(1, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), d2dImages[2].Get()));

        // Changes properties via D2D.
        d2dEffect->SetInput(0, d2dImages[3].Get());
        d2dEffect->SetInput(1, d2dImages[4].Get());

        // Win2D should immediately see the changes.
        Assert::AreEqual<IGraphicsEffectSource>(images[3], effect->Background);
        Assert::AreEqual<IGraphicsEffectSource>(images[4], effect->Foreground);
    }

    TEST_METHOD(CanvasEffect_InteropChangesSourcesCollection_IsBidirectional)
    {
        auto device = ref new CanvasDevice();

        CanvasBitmap^ images[] =
        {
            ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI),
            ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI),
            ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI),
            ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI),
        };

        auto effect = ref new CompositeEffect();

        effect->Sources->Append(images[0]);

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI);

        ComPtr<ID2D1Image> d2dImages[] =
        {
            GetWrappedResource<ID2D1Image>(images[0]),
            GetWrappedResource<ID2D1Image>(images[1]),
            GetWrappedResource<ID2D1Image>(images[2]),
            GetWrappedResource<ID2D1Image>(images[3]),
        };

        // Change properties via Win2D.
        effect->Sources->SetAt(0, images[1]);
        effect->Sources->Append(images[2]);
        effect->Sources->Append(images[3]);

        // D2D should immediately see the changes.
        Assert::AreEqual(3u, d2dEffect->GetInputCount());

        ComPtr<ID2D1Image> input;

        d2dEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), d2dImages[1].Get()));

        d2dEffect->GetInput(1, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), d2dImages[2].Get()));

        d2dEffect->GetInput(2, &input);
        Assert::IsTrue(IsSameInstance(input.Get(), d2dImages[3].Get()));

        // Changes properties via D2D.
        d2dEffect->SetInputCount(2);
        d2dEffect->SetInput(0, d2dImages[2].Get());
        d2dEffect->SetInput(1, d2dImages[1].Get());

        // Win2D should immediately see the changes.
        Assert::AreEqual(2u, effect->Sources->Size);

        Assert::AreEqual<IGraphicsEffectSource>(images[2], effect->Sources->GetAt(0));
        Assert::AreEqual<IGraphicsEffectSource>(images[1], effect->Sources->GetAt(1));
    }

    TEST_METHOD(CanvasEffect_WhenSourcesCollectionIsEmpty_EffectBecomesUnrealized)
    {
        auto device = ref new CanvasDevice();
        auto image = ref new CanvasCommandList(device);
        auto effect = ref new CompositeEffect();

        effect->Sources->Append(image);

        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect);

        // Reverse lookups should get us back to the Win2D effect (because it is currently realized).
        Assert::AreEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));

        Assert::AreEqual(1u, effect->Sources->Size);

        // Emptying the sources collection should unrealize the effect (because D2D doesn't support 0 sources).
        effect->Sources->Clear();

        Assert::AreEqual(0u, effect->Sources->Size);

        // We prove we are no longer realized by noting that GetOrCreate from
        // our previous D2D resource now creates a different Win2D wrapper.
        Assert::AreNotEqual(effect, GetOrCreate<CompositeEffect>(device, d2dEffect.Get()));
    }

    TEST_METHOD(CanvasEffect_WhenRerealizedOnDifferentDevice_PropertiesArePreserved)
    {
        auto effect = ref new PointSpecularEffect();
        effect->Source = ref new ColorSourceEffect();

        // Realize the effect onto device #1.
        auto device1 = ref new CanvasDevice();
        auto d2dEffect1 = GetWrappedResource<ID2D1Effect>(device1, effect);

        // Change properties, one via Win2D and the other via D2D.
        effect->SpecularExponent = 3;
        d2dEffect1->SetValue(D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT, 2.5f);

        // Re-realize onto a different device.
        auto device2 = ref new CanvasDevice();
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);
        Assert::IsFalse(IsSameInstance(d2dEffect1.Get(), d2dEffect2.Get()));

        // Modified property values should be preserved, whether viewed from Win2D or D2D.
        Assert::AreEqual(3.0f, effect->SpecularExponent);
        Assert::AreEqual(2.5f, effect->SpecularAmount);

        Assert::AreEqual(3.0f, d2dEffect2->GetValue<float>(D2D1_POINTSPECULAR_PROP_SPECULAR_EXPONENT));
        Assert::AreEqual(2.5f, d2dEffect2->GetValue<float>(D2D1_POINTSPECULAR_PROP_SPECULAR_CONSTANT));
    }

    TEST_METHOD(CanvasEffect_WhenRerealizedOnDifferentDevice_InputsArePreserved)
    {
        auto effect = ref new BlendEffect();

        auto image1 = ref new ColorSourceEffect();
        auto image2 = ref new ColorSourceEffect();
        auto image3 = ref new ColorSourceEffect();

        effect->Background = image1;
        effect->Foreground = image1;

        // Realize the effect onto device #1.
        auto device1 = ref new CanvasDevice();
        auto d2dEffect1 = GetWrappedResource<ID2D1Effect>(device1, effect);

        // Change inputs, one via Win2D and the other via D2D.
        effect->Background = image2;
        d2dEffect1->SetInput(1, GetWrappedResource<ID2D1Image>(device1, image3).Get());

        // Re-realize onto a different device.
        auto device2 = ref new CanvasDevice();
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);
        Assert::IsFalse(IsSameInstance(d2dEffect1.Get(), d2dEffect2.Get()));

        // Modified input values should be preserved, whether viewed from Win2D or D2D.
        Assert::AreEqual<IGraphicsEffectSource>(image2, effect->Background);
        Assert::AreEqual<IGraphicsEffectSource>(image3, effect->Foreground);

        ComPtr<ID2D1Image> input;

        d2dEffect2->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Effect>(device2, image2).Get(), input.Get()));

        d2dEffect2->GetInput(1, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Effect>(device2, image3).Get(), input.Get()));
    }

    TEST_METHOD(CanvasEffect_WhenRerealizedOnDifferentDevice_SourcesCollectionIsPreserved)
    {
        auto effect = ref new CompositeEffect();

        auto image1 = ref new ColorSourceEffect();
        auto image2 = ref new ColorSourceEffect();
        auto image3 = ref new ColorSourceEffect();

        effect->Sources->Append(image1);
        effect->Sources->Append(image1);

        // Realize the effect onto device #1.
        auto device1 = ref new CanvasDevice();
        auto d2dEffect1 = GetWrappedResource<ID2D1Effect>(device1, effect);

        // Change inputs, one via Win2D and the other via D2D.
        effect->Sources->SetAt(0, image2);
        d2dEffect1->SetInput(1, GetWrappedResource<ID2D1Image>(device1, image3).Get());

        // Re-realize onto a different device.
        auto device2 = ref new CanvasDevice();
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);
        Assert::IsFalse(IsSameInstance(d2dEffect1.Get(), d2dEffect2.Get()));

        // Modified input values should be preserved, whether viewed from Win2D or D2D.
        Assert::AreEqual(2u, effect->Sources->Size);
        Assert::AreEqual(2u, d2dEffect2->GetInputCount());

        Assert::AreEqual<IGraphicsEffectSource>(image2, effect->Sources->GetAt(0));
        Assert::AreEqual<IGraphicsEffectSource>(image3, effect->Sources->GetAt(1));

        ComPtr<ID2D1Image> input;

        d2dEffect2->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Effect>(device2, image2).Get(), input.Get()));

        d2dEffect2->GetInput(1, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Effect>(device2, image3).Get(), input.Get()));
    }

    TEST_METHOD(CanvasEffect_RerealizeChildEffectsOnDifferentDevice)
    {
        auto effect1 = ref new GaussianBlurEffect;
        auto effect2 = ref new ColorSourceEffect;

        effect1->Source = effect2;

        // Realize the effects onto device1.
        auto device1 = ref new CanvasDevice();

        auto d2dEffect11 = GetWrappedResource<ID2D1Effect>(device1, effect1);
        auto d2dEffect21 = GetWrappedResource<ID2D1Effect>(device1, effect2);

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect11.Get()));
        Assert::AreEqual(effect2, GetOrCreate<ColorSourceEffect>(device1, d2dEffect21.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);

        // Realize the child effect onto device2.
        auto device2 = ref new CanvasDevice();

        auto d2dEffect22 = GetWrappedResource<ID2D1Effect>(device2, effect2);

        Assert::IsFalse(IsSameInstance(d2dEffect21.Get(), d2dEffect22.Get()));

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect11.Get()));
        Assert::AreEqual(effect2, GetOrCreate<ColorSourceEffect>(device2, d2dEffect22.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);

        // Using the parent effect (still on device1) should move the child back to device1 as well.
        auto d2dEffect12 = GetWrappedResource<ID2D1Effect>(device1, effect1);

        Assert::IsTrue(IsSameInstance(d2dEffect11.Get(), d2dEffect12.Get()));

        Assert::AreEqual(effect1, GetOrCreate<GaussianBlurEffect>(device1, d2dEffect12.Get()));

        ComPtr<ID2D1Image> input;
        d2dEffect12->GetInput(0, &input);

        Assert::AreEqual(effect2, GetOrCreate<ColorSourceEffect>(device1, input.Get()));

        Assert::AreEqual<IGraphicsEffectSource>(effect1->Source, effect2);
    }

    TEST_METHOD(CanvasEffect_WhenInputSetToBitmapUsingDifferentDevice_EffectIsUnrealized)
    {
        auto effect = ref new BlendEffect();

        // Set the inputs to bitmaps created on device #1.
        auto device1 = ref new CanvasDevice();

        auto bitmap1a = ref new CanvasRenderTarget(device1, 1, 1, DEFAULT_DPI);
        auto bitmap1b = ref new CanvasRenderTarget(device1, 1, 1, DEFAULT_DPI);

        effect->Background = bitmap1a;
        effect->Foreground = bitmap1b;

        // Realize the effect onto device #1.
        auto d2dEffect1 = GetWrappedResource<ID2D1Effect>(device1, effect, DEFAULT_DPI);

        Assert::AreEqual(effect, GetOrCreate<BlendEffect>(device1, d2dEffect1.Get()));

        // Change the first input to a bitmap created on device #2.
        auto device2 = ref new CanvasDevice();

        auto bitmap2a = ref new CanvasRenderTarget(device2, 1, 1, DEFAULT_DPI);
        auto bitmap2b = ref new CanvasRenderTarget(device2, 1, 1, DEFAULT_DPI);

        effect->Background = bitmap2a;

        // This should have unrealized the effect.
        Assert::AreNotEqual(effect, GetOrCreate<BlendEffect>(device1, d2dEffect1.Get()));

        // At this transitional point Win2D should be able to report back the two input bitmaps, regardless of their mixed devices.
        Assert::AreEqual<IGraphicsEffectSource>(bitmap2a, effect->Background);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap1b, effect->Foreground);

        // Change the second input to match the device of the first.
        effect->Foreground = bitmap2b;

        // Realize the effect onto device #2.
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect, DEFAULT_DPI);

        Assert::IsFalse(IsSameInstance(d2dEffect1.Get(), d2dEffect2.Get()));

        // Should be able to read back the inputs via Win2D.
        Assert::AreEqual<IGraphicsEffectSource>(bitmap2a, effect->Background);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap2b, effect->Foreground);

        // Should also be able to read back the inputs via D2D.
        ComPtr<ID2D1Image> input;

        d2dEffect2->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Bitmap1>(bitmap2a).Get(), input.Get()));

        d2dEffect2->GetInput(1, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Bitmap1>(bitmap2b).Get(), input.Get()));
    }

    TEST_METHOD(CanvasEffect_DpiCompensationIsNotVisibleInWinRT)
    {
        auto device = ref new CanvasDevice();
        auto effect = ref new GaussianBlurEffect();
        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);

        effect->Source = bitmap;

        // Realize the effect.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect, DEFAULT_DPI + 123);

        // At the D2D level, a DPI compensation effect should have been inserted.
        ComPtr<ID2D1Image> input;
        d2dEffect->GetInput(0, &input);
        auto dpiEffect = As<ID2D1Effect>(input);
        Assert::IsTrue(dpiEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1DpiCompensation);

        dpiEffect->GetInput(0, &input);
        Assert::IsTrue(IsSameInstance(GetWrappedResource<ID2D1Bitmap1>(bitmap).Get(), input.Get()));

        // Win2D should not see the DPI compensation effect.
        Assert::AreEqual<IGraphicsEffectSource>(bitmap, effect->Source);

        // Discard our Win2D effect wrapper, then construct a new wrapper around the D2D effect.
        effect = nullptr;
        effect = GetOrCreate<GaussianBlurEffect>(device, d2dEffect.Get());

        // The DPI compensation effect should now be visible from Win2D.
        Assert::AreNotEqual<IGraphicsEffectSource>(bitmap, effect->Source);
        auto dpiWrapper = dynamic_cast<DpiCompensationEffect^>(effect->Source);
        Assert::IsNotNull(dpiWrapper);
        Assert::AreEqual<IGraphicsEffectSource>(bitmap, dpiWrapper->Source);
    }

    TEST_METHOD(CanvasEffect_BufferPrecision)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();
        
        auto effect = ref new ColorSourceEffect();

        // Check the default value.
        Assert::IsNull(effect->BufferPrecision);

        // Set and get the property while not realized.
        effect->BufferPrecision = CanvasBufferPrecision::Precision32Float;
        Assert::AreEqual(CanvasBufferPrecision::Precision32Float, effect->BufferPrecision->Value);

        effect->BufferPrecision = nullptr;
        Assert::IsNull(effect->BufferPrecision);

        // Realizing the effect should immediately set its value through to D2D.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device1, effect);
        Assert::AreEqual(D2D1_BUFFER_PRECISION_UNKNOWN, d2dEffect->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION));

        // Changing the Win2D property should now immediately affect D2D state.
        effect->BufferPrecision = CanvasBufferPrecision::Precision16Float;
        Assert::AreEqual(CanvasBufferPrecision::Precision16Float, effect->BufferPrecision->Value);
        Assert::AreEqual(D2D1_BUFFER_PRECISION_16BPC_FLOAT, d2dEffect->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION));

        effect->BufferPrecision = nullptr;
        Assert::IsNull(effect->BufferPrecision);
        Assert::AreEqual(D2D1_BUFFER_PRECISION_UNKNOWN, d2dEffect->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION));

        // Changing the D2D property should be immediately reflected to Win2D.
        d2dEffect->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB);
        Assert::AreEqual(CanvasBufferPrecision::Precision8UIntNormalizedSrgb, effect->BufferPrecision->Value);

        d2dEffect->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_UNKNOWN);
        Assert::IsNull(effect->BufferPrecision);

        // Changing the D2D property, then re-realizing onto a different device should read back and preserve the new value.
        d2dEffect->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);

        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);
        Assert::IsFalse(IsSameInstance(d2dEffect.Get(), d2dEffect2.Get()));

        Assert::AreEqual(CanvasBufferPrecision::Precision32Float, effect->BufferPrecision->Value);
        Assert::AreEqual(D2D1_BUFFER_PRECISION_32BPC_FLOAT, d2dEffect2->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION));
    }

    TEST_METHOD(CanvasEffect_CacheOutput)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();

        auto effect = ref new ColorSourceEffect();

        // Check the default value.
        Assert::IsFalse(effect->CacheOutput);

        // Set and get the property while not realized.
        effect->CacheOutput = true;
        Assert::IsTrue(effect->CacheOutput);

        // Realizing the effect should immediately set its value through to D2D.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device1, effect);
        Assert::IsTrue(!!d2dEffect->GetValue<BOOL>(D2D1_PROPERTY_CACHED));

        // Changing the Win2D property should now immediately affect D2D state.
        effect->CacheOutput = false;
        Assert::IsFalse(effect->CacheOutput);
        Assert::IsFalse(!!d2dEffect->GetValue<BOOL>(D2D1_PROPERTY_CACHED));

        // Changing the D2D property should be immediately reflected to Win2D.
        d2dEffect->SetValue(D2D1_PROPERTY_CACHED, (BOOL)true);
        Assert::IsTrue(effect->CacheOutput);

        // Changing the D2D property, then re-realizing onto a different device should read back and preserve the new value.
        d2dEffect->SetValue(D2D1_PROPERTY_CACHED, (BOOL)false);

        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);
        Assert::IsFalse(IsSameInstance(d2dEffect.Get(), d2dEffect2.Get()));

        Assert::IsFalse(effect->CacheOutput);
        Assert::IsFalse(!!d2dEffect2->GetValue<BOOL>(D2D1_PROPERTY_CACHED));
    }

    template<typename TValue, typename TGetter, typename TSetter>
    static void TestInterfaceProperty_NotRealized(TValue^ const& valueWrapper, TGetter&& getter, TSetter&& setter)
    {
        // Initial state.
        Assert::IsNull(getter());
        Assert::AreEqual(1, GetRefCount(valueWrapper), L"Initially, we are the only ones holding a refcount.");

        // Set property.
        setter(valueWrapper);
        Assert::AreEqual(2, GetRefCount(valueWrapper), L"Now the effect has a refcount as well.");

        // Get property.
        auto readBack = getter();
        Assert::AreEqual(valueWrapper, readBack);
        Assert::AreEqual(3, GetRefCount(valueWrapper), L"Reading back the interface pointer AddRefs it.");
        readBack = nullptr;
        Assert::AreEqual(2, GetRefCount(valueWrapper));

        // Clear property.
        setter(nullptr);
        Assert::IsNull(getter());
        Assert::AreEqual(1, GetRefCount(valueWrapper), L"Nulling the effect property releases its refcount.");
    }

    template<typename TValue, typename TGetter, typename TSetter>
    static void TestInterfaceProperty_Realized(CanvasDevice^ device, Object^ effect, TValue^ const& valueWrapper, TGetter&& getter, TSetter&& setter, bool expectWorkaround6146411 = false)
    {
        // Realize the effect.
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(device, effect);
        auto d2dResource = GetWrappedResource<IUnknown>(device, valueWrapper);

        // Initial state.
        Assert::IsNull(getter());
        Assert::AreEqual(1, GetRefCount(valueWrapper), L"Initially, we are the only ones holding a refcount on the Win2D wrapper.");
        Assert::AreEqual(2, GetRefCount(d2dResource.Get()), L"D2D resource has refcounts from the Win2D wrapper and directly from us.");

        // Set property.
        setter(valueWrapper);
        Assert::AreEqual(1, GetRefCount(valueWrapper), L"No refcount from the effect to Win2D wrapper, because D2D is directly referencing the D2D resource.");
        Assert::AreEqual(expectWorkaround6146411 ? 4 : 3, GetRefCount(d2dResource.Get()), L"Now the effect has a refcount on the D2D resource.");

        // Get property.
        auto readBack = getter();
        Assert::AreEqual(valueWrapper, readBack);
        Assert::AreEqual(2, GetRefCount(valueWrapper), L"Reading back the Win2D wrapper AddRefs it.");
        Assert::AreEqual(expectWorkaround6146411 ? 4 : 3, GetRefCount(d2dResource.Get()), L"No change to the D2D resource.");
        readBack = nullptr;
        Assert::AreEqual(1, GetRefCount(valueWrapper));
        Assert::AreEqual(expectWorkaround6146411 ? 4 : 3, GetRefCount(d2dResource.Get()));

        // Clear property.
        setter(nullptr);
        Assert::IsNull(getter());
        Assert::AreEqual(1, GetRefCount(valueWrapper), L"Nulling the effect property does not alter the Win2D wrapper refcount.");
        Assert::AreEqual(2, GetRefCount(d2dResource.Get()), L"Nulling the effect property releases the D2D resource refcount.");
    }

    TEST_METHOD(CanvasEffect_ColorManagementProfile_SourceProperty_NotRealized)
    {
        auto effect = ref new ColorManagementEffect();
        auto colorProfile = ref new ColorManagementProfile(CanvasColorSpace::Srgb);

        TestInterfaceProperty_NotRealized(
            colorProfile,
            [&] { return effect->SourceColorProfile; },
            [&](ColorManagementProfile^ value) { effect->SourceColorProfile = value; }
        );
    }

    TEST_METHOD(CanvasEffect_ColorManagementProfile_SourceProperty_Realized)
    {
        // The debug layer changes refcounting behavior, so run this test without it.
        DisableDebugLayer disableDebug;

        auto device = ref new CanvasDevice();
        auto effect = ref new ColorManagementEffect();
        auto colorProfile = ref new ColorManagementProfile(CanvasColorSpace::Srgb);

        TestInterfaceProperty_Realized(
            device,
            effect,
            colorProfile,
            [&] { return effect->SourceColorProfile; },
            [&](ColorManagementProfile^ value) { effect->SourceColorProfile = value; }
        );
    }

    TEST_METHOD(CanvasEffect_ColorManagementProfile_OutputProperty_NotRealized)
    {
        auto effect = ref new ColorManagementEffect();
        auto colorProfile = ref new ColorManagementProfile(CanvasColorSpace::Srgb);

        TestInterfaceProperty_NotRealized(
            colorProfile,
            [&] { return effect->OutputColorProfile; },
            [&](ColorManagementProfile^ value) { effect->OutputColorProfile = value; }
        );
    }

    TEST_METHOD(CanvasEffect_ColorManagementProfile_OutputProperty_Realized)
    {
        // The debug layer changes refcounting behavior, so run this test without it.
        DisableDebugLayer disableDebug;

        auto device = ref new CanvasDevice();
        auto effect = ref new ColorManagementEffect();
        auto colorProfile = ref new ColorManagementProfile(CanvasColorSpace::Srgb);

        TestInterfaceProperty_Realized(
            device,
            effect,
            colorProfile,
            [&] { return effect->OutputColorProfile; },
            [&](ColorManagementProfile^ value) { effect->OutputColorProfile = value; },
            true
        );
    }

    TEST_METHOD(CanvasEffect_TransferTable3D_TableProperty_NotRealized)
    {
        auto device = ref new CanvasDevice();
        auto effect = ref new TableTransfer3DEffect();
        auto transferTable = EffectTransferTable3D::CreateFromColors(device, ref new Array<Windows::UI::Color>(8), 2, 2, 2);

        TestInterfaceProperty_NotRealized(
            transferTable,
            [&] { return effect->Table; },
            [&](EffectTransferTable3D^ value) { effect->Table = value; }
        );
    }

    TEST_METHOD(CanvasEffect_TransferTable3D_TableProperty_Realized)
    {
        // The debug layer changes refcounting behavior, so run this test without it.
        DisableDebugLayer disableDebug;

        auto device = ref new CanvasDevice();
        auto effect = ref new TableTransfer3DEffect();
        auto transferTable = EffectTransferTable3D::CreateFromColors(device, ref new Array<Windows::UI::Color>(8), 2, 2, 2);

        TestInterfaceProperty_Realized(
            device,
            effect,
            transferTable,
            [&] { return effect->Table; },
            [&](EffectTransferTable3D^ value) { effect->Table = value; }
        );
    }

    // See https://github.com/microsoft/Win2D/issues/913
    TEST_METHOD(CanvasEffect_CreateWrapperOnInvalidDevice_DetectD2DFactoryMismatch)
    {
        // Create CanvasDevice #1 (this will internally create the D2D factory #1)
        auto canvasDevice1 = ref new CanvasDevice();

        // Create an effect (this will be in the unrealized state, not tied to any device)
        auto colorEffect1 = ref new ColorSourceEffect();

        ComPtr<ID2D1Image> imageAbi1;

        // Realize the effect on CanvasDevice #1. This will cause the effect to be realized, and to return the D2D effect
        // with the same D2D factory #1 as parent as the CanvasDevice (and hence D2D device) it was created from.
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(colorEffect1)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice1).Get(),
            0,
            IID_PPV_ARGS(&imageAbi1)));

        // Create CanvasDevice #2 (this will internally use a different D2D factory #2)
        auto canvasDevice2 = ref new CanvasDevice();

        ComPtr<ID2D1Image> imageAbi2;

        // Realize the effect on CanvasDevice #2. From GetD2DImage, this effect will see the realization device does not
        // match, so it will unrealize and re-realize on the new device. That is, the returned D2D image will now be tied
        // to the new D2D device retrieved from CanvasDevice #2, and as such it will be parented to D2D factory #2.
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(colorEffect1)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice2).Get(),
            0,
            IID_PPV_ARGS(&imageAbi2)));

        // Get the ICanvasFactoryNative activation factory to test the ABI methods
        ComPtr<ABI::Microsoft::Graphics::Canvas::ICanvasFactoryNative> factory;
        ThrowIfFailed(Windows::Foundation::GetActivationFactory(
            reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
            &factory));

        // At this point: the D2D effect retrieved from the color effect is "orphaned". That is, it no longer has an associated
        // wrapper, as its parent Win2D effect has unrealized (ie. discarded it) and re-realized on another device. We can now
        // call GetOrCreate to ask Win2D to create a new wrapper for this D2D effect then. Here's the issue:
        //   - We're passing CanvasDevice #2 as the realization device (tied to D2D factory #2)
        //   - We're wrapping the D2D effect from the effect created from CanvasDevice #1
        // Win2D does validation for this when constructing a wrapper from an external effect, by checking that the D2D factory of
        // the input device matches the factory of the effect being wrapped. This doesn't cover 100% of cases (as there might be
        // multiple devices from a single factory), but because there is no way to get a device from an effect, this is the best it
        // can do. And it's still enough to cover the majority of cases, such as this one. As such, we expect an exception here.
        ComPtr<IInspectable> inspectableResult;
        Assert::AreEqual(D2DERR_WRONG_FACTORY, factory->GetOrCreate(As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice2).Get(), imageAbi1.Get(), 0, &inspectableResult));
        Assert::IsNull(inspectableResult.Get());

        // Also test the C++/CX helper in this scenario (it should throw an exception)
        try
        {
            auto colorEffect2 = GetOrCreate<ColorSourceEffect>(canvasDevice2, imageAbi1.Get());
        }
        catch (Platform::COMException^ e)
        {
            Assert::AreEqual(D2DERR_WRONG_FACTORY, (HRESULT)e->HResult);
        }
    }
};
