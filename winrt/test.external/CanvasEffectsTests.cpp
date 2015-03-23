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

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Platform;

using AbiEffectNative = ABI::Microsoft::Graphics::Canvas::Effects::IGraphicsEffectD2D1Interop;
using AbiEffectSource = ABI::Microsoft::Graphics::Canvas::Effects::IGraphicsEffectSource;
using AbiPropertyValue = ABI::Windows::Foundation::IPropertyValue;

TEST_CLASS(CanvasBitmapTests)
{
    TEST_METHOD(CanvasEffect_IGraphicsEffectD2D1Interop)
    {
        // Use gaussian blur for testing IGraphicsEffectD2D1Interop interface
        auto blurEffect = ref new GaussianBlurEffect();
        auto blurNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(blurEffect));

        GUID id;
        ThrowIfFailed(blurNative->GetEffectId(&id));
        Assert::IsTrue(id == CLSID_D2D1GaussianBlur);
        
        // Check sources initial vector
        UINT count;
        ThrowIfFailed(blurNative->GetSourceCount(&count));
        Assert::AreEqual(1U, count);

        ComPtr<AbiEffectSource> source;
        ThrowIfFailed(blurNative->GetSource(0, &source));
        Assert::IsNull(source.Get());

        // Check initial properties vector
        ThrowIfFailed(blurNative->GetPropertyCount(&count));
        Assert::AreEqual(3U, count);

        ComPtr<AbiPropertyValue> prop;

        for (unsigned int i = 0; i < 3U; ++i)
        {
            ThrowIfFailed(blurNative->GetProperty(i, &prop));
            Assert::IsNotNull(prop.Get());
        }

        // Check null parameters.
        Assert::AreEqual(E_INVALIDARG, blurNative->GetEffectId(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurNative->GetSourceCount(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurNative->GetSource(0, nullptr));
        Assert::AreEqual(E_INVALIDARG, blurNative->GetPropertyCount(nullptr));
        Assert::AreEqual(E_INVALIDARG, blurNative->GetProperty(0, nullptr));

        // Check out of range accesses.
        Assert::AreEqual(E_BOUNDS, blurNative->GetSource((unsigned)-1, &source));
        Assert::AreEqual(E_BOUNDS, blurNative->GetSource(1, &source));

        Assert::AreEqual(E_BOUNDS, blurNative->GetProperty((unsigned)-1, &prop));
        Assert::AreEqual(E_BOUNDS, blurNative->GetProperty(3, &prop));
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
        auto blurNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(blurEffect));

        ComPtr<AbiPropertyValue> prop;
        ThrowIfFailed(blurNative->GetProperty(0, &prop));

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
        auto blurNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(blurEffect));

        ComPtr<AbiPropertyValue> prop;
        ThrowIfFailed(blurNative->GetProperty(1, &prop));

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
        auto transformNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(transformEffect));

        // Transformation matrix is propery #2 in property array
        ComPtr<AbiPropertyValue> prop;
        ThrowIfFailed(transformNative->GetProperty(2, &prop));

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
        auto blurNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(blurEffect));

        unsigned count;
        ThrowIfFailed(blurNative->GetSourceCount(&count));
        Assert::AreEqual(1U, count);

        ComPtr<AbiEffectSource> source;
        ThrowIfFailed(blurNative->GetSource(0, &source));
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
};
