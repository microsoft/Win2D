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
TEST_CLASS(CanvasBitmapTests)
{

    TEST_METHOD(CanvasEffect_IEffectInterface)
    {
        // Use gaussian blur for testing IEffect interface
        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();

        Guid realId(CLSID_D2D1GaussianBlur);
        Assert::IsTrue(realId == blurEffect->EffectId);
        
        // Check inputs initial vector
        IVector<IEffectInput^>^ inputs = blurEffect->Inputs;
        Assert::AreEqual(inputs->Size, 1U);
        Assert::IsNull(inputs->GetAt(0));

        // Set same effect as input for reference testing
        inputs->SetAt(0, blurEffect);
        Assert::IsTrue(blurEffect==inputs->GetAt(0));

        // Try to set more inputs in fixed size input effect
        Assert::ExpectException<NotImplementedException^>(
            [&]
            {
                inputs->Append(blurEffect);
            });

        // Try to remove input in fixed size input effect
        Assert::ExpectException<NotImplementedException^>(
            [&]
            {
                inputs->RemoveAt(0);
            });

        // Check initial properties vector
        IVector<Object^>^ properties = blurEffect->Properties;
        Assert::AreEqual(properties->Size, 3U);
        for (unsigned int i = 0; i < properties->Size; ++i)
        {
            Assert::IsNotNull(properties->GetAt(i));
        }

        // Try to set more properties in effect
        Assert::ExpectException<NotImplementedException^>(
            [&]
            {
                IPropertyValue^ value = nullptr;
                properties->Append(value);
            });

        // Try to remove property in effect
        Assert::ExpectException<NotImplementedException^>(
            [&]
            {
                properties->RemoveAt(0);
            });
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

        // Check that IEffect Interface connect to the same data vector
        Assert::AreEqual(newValue, safe_cast<IPropertyValue^>(blurEffect->Properties->GetAt(0))->GetSingle());
        newValue = 4.0f;
        IPropertyValue^ newPropertyValue = safe_cast<IPropertyValue^>(PropertyValue::CreateSingle(newValue));
        blurEffect->Properties->SetAt(0, newPropertyValue);
        Assert::AreEqual(newValue, blurEffect->BlurAmount);

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

        // Check that IEffect Interface connect to the same data vector
        Assert::IsTrue(newValue == safe_cast<EffectOptimization>(safe_cast<IPropertyValue^>(blurEffect->Properties->GetAt(1))->GetUInt32()));
        newValue = EffectOptimization::Speed;
        IPropertyValue^ newPropertyValue = safe_cast<IPropertyValue^>(PropertyValue::CreateUInt32(safe_cast<UINT32>(newValue)));
        blurEffect->Properties->SetAt(1, newPropertyValue);
        Assert::IsTrue(newValue == blurEffect->Optimization);
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

        // Check that IEffect Interface connect to the same data vector
        Platform::Array<float, 1U>^ propertyArray;
        // Transformation matrix is propery #2 in property array
        safe_cast<IPropertyValue^>(transformEffect->Properties->GetAt(2))->GetSingleArray(&propertyArray);
        // Cell (2,2) have index 5 in array representation of 4x4 matrix
        Assert::AreEqual(newValue, propertyArray[5]);
        // Set Cell (2, 3)
        propertyArray[6] = newValue;
        IPropertyValue^ newPropertyValue = safe_cast<IPropertyValue^>(PropertyValue::CreateSingleArray(propertyArray));
        transformEffect->Properties->SetAt(2, newPropertyValue);
        Assert::AreEqual(newValue, static_cast<float4x4>(transformEffect->TransformMatrix).m23);
    }

    TEST_METHOD(CanvasEffect_LimitedInputs)
    {
        // Use gaussian for testing limited inputs
        GaussianBlurEffect^ blurEffect = ref new GaussianBlurEffect();

        Assert::IsNull(blurEffect->Source);

        // Set same effect as input for reference testing
        blurEffect->Source = blurEffect;
        Assert::IsTrue(blurEffect == blurEffect->Source);

        // Check that IEffect Interface connect to the same inputs
        Assert::AreEqual(1U, blurEffect->Inputs->Size);
        Assert::IsTrue(blurEffect == blurEffect->Inputs->GetAt(0));
        GaussianBlurEffect^ secondBlurEffect = ref new GaussianBlurEffect();
        blurEffect->Inputs->SetAt(0, secondBlurEffect);
        Assert::IsTrue(secondBlurEffect == blurEffect->Source);
    }

    TEST_METHOD(CanvasEffect_UnlimitedInputs)
    {
        // Use Composite effect for testing unlimited inputs
        CompositeEffect^ compositeEffect = ref new CompositeEffect();

        // All unlimited inputs supposed to have 0 size inputs vector by default
        Assert::AreEqual(0U, compositeEffect->Inputs->Size);

        // Set same effect as input for reference testing
        unsigned int inputNumber = 10;
        for (unsigned int i = 0; i < inputNumber; i++)
        {
            compositeEffect->Inputs->Append(compositeEffect);
        }
        Assert::AreEqual(inputNumber, compositeEffect->Inputs->Size);
        for (unsigned int i = 0; i < inputNumber; i++)
        {
            Assert::IsTrue(compositeEffect == compositeEffect->Inputs->GetAt(i));
        }
    }
};
