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

    TEST_METHOD(CanvasEffect_IEffectInerface)
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
        IVector<IPropertyValue^>^ properties = blurEffect->Properties;
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
        Assert::AreEqual(3.0f, blurEffect->StandardDeviation);
        float newValue = 5.0f;
        blurEffect->StandardDeviation = newValue;
        Assert::AreEqual(newValue, blurEffect->StandardDeviation);

        // Check that IEffect Interface connect to the same data vector
        Assert::AreEqual(newValue, blurEffect->Properties->GetAt(0)->GetSingle());
        newValue = 4.0f;
        IPropertyValue^ newPropertyValue = safe_cast<IPropertyValue^>(PropertyValue::CreateSingle(newValue));
        blurEffect->Properties->SetAt(0, newPropertyValue);
        Assert::AreEqual(newValue, blurEffect->StandardDeviation);

        // Try setting not valid data. 
        // For StandardDeviation in GaussianBlur valid range is (0.0f;250.0f)
        Assert::ExpectException<InvalidArgException>(
            [&]
            {
                blurEffect->StandardDeviation = -1.0f;
            });
        Assert::ExpectException<InvalidArgException>(
            [&]
            {
                blurEffect->StandardDeviation = 300.0f;
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
        Assert::IsTrue(newValue == safe_cast<EffectOptimization>(blurEffect->Properties->GetAt(1)->GetUInt32()));
        newValue = EffectOptimization::Speed;
        IPropertyValue^ newPropertyValue = safe_cast<IPropertyValue^>(PropertyValue::CreateUInt32(safe_cast<UINT32>(newValue)));
        blurEffect->Properties->SetAt(1, newPropertyValue);
        Assert::IsTrue(newValue == blurEffect->Optimization);
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
};




















