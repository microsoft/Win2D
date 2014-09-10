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
#include <effects\CanvasEffect.h>
#include "TestBitmapResourceCreationAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define IMPLEMENT_MOCK_PROPERTY(PROPERTY_NAME, TYPE)                                \
        IFACEMETHODIMP TestEffect::get_##PROPERTY_NAME(_Out_ TYPE* value)           \
        {                                                                           \
            Assert::Fail(L"Unexpected call to getter in TestEffect");               \
            return E_NOTIMPL;                                                       \
        }                                                                           \
                                                                                    \
        IFACEMETHODIMP TestEffect::put_##PROPERTY_NAME(_In_ TYPE value)             \
        {                                                                           \
            Assert::Fail(L"Unexpected call to setter in TestEffect");               \
            return E_NOTIMPL;                                                       \
        }

TEST_CLASS(CanvasEffectUnitTest)
{
public:
	
    class TestEffect : public RuntimeClass <
        IGaussianBlurEffect,
        MixIn<TestEffect, CanvasEffect> >,
        public CanvasEffect
    {
        float m_deviation;
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_GaussianBlurEffect, BaseTrust);

    public:
        TestEffect(GUID effectId, int propertiesSize, int inputsSize, bool isInputSizeChange)
            : CanvasEffect(effectId, propertiesSize, inputsSize, isInputSizeChange)
        {
        }

        IMPLEMENT_PROPERTY(TestEffect,
                           StandardDeviation,
                           float,
                           0)

        IMPLEMENT_MOCK_PROPERTY(Optimization, EffectOptimization)
        IMPLEMENT_MOCK_PROPERTY(BorderMode, EffectBorderMode)
        
        IMPLEMENT_INPUT_PROPERTY(TestEffect, Source, 0)

        std::function<void()> MockGetInput;
        std::function<void()> MockSetInput;

        std::function<void()> MockGetProperty;
        std::function<void()> MockSetProperty;

        //
        // Forwarding base protected methods in order to inspect calls
        //

        void GetInput(unsigned int index, IEffectInput** input)
        {
            if (MockGetInput)
                MockGetInput();
            CanvasEffect::GetInput(index, input);
        }

        void SetInput(unsigned int index, IEffectInput* input)
        {
            if (MockSetInput)
                MockSetInput();
            CanvasEffect::SetInput(index, input);
        }

        template<typename T>
        void GetProperty(unsigned int index, T value)
        {
            if (MockGetProperty)
                MockGetProperty();
            CanvasEffect::GetProperty<T>(index, value);
        }

        template<typename T>
        void SetProperty(unsigned int index, T value, bool isDefault = false)
        {
            if (MockSetProperty)
                MockSetProperty();
            CanvasEffect::SetProperty<T>(index, value, isDefault);
        }
    };

    ComPtr<TestEffect> m_testEffect;
    unsigned int m_realPropertiesSize = 4;
    unsigned int m_realInputSize = 8;
    GUID m_blurGuid = CLSID_D2D1GaussianBlur;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_testEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realInputSize, false);
    }

    TEST_METHOD(CanvasEffect_Implements_Expected_Interfaces)
    {
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IEffect);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IEffectInput);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ICanvasImageInternal);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IGaussianBlurEffect);
    }

    TEST_METHOD(CanvasEffect_Properties)
    {
        ComPtr<IVector<IPropertyValue*>> properties;
        ThrowIfFailed(m_testEffect->get_Properties(&properties));

        unsigned int propertiesSize;
        ThrowIfFailed(properties->get_Size(&propertiesSize));
        Assert::AreEqual(m_realPropertiesSize, propertiesSize);
        for (unsigned int i = 0; i < m_realInputSize; ++i)
        {
            ComPtr<IPropertyValue> propertyValue;
            properties->GetAt(i, &propertyValue);
            Assert::IsNull(propertyValue.Get());
        }

        bool isSetPropertyCalled = false;
        m_testEffect->MockSetProperty =
            [&]
            {
            Assert::IsFalse(isSetPropertyCalled);
                isSetPropertyCalled = true;
            };

        bool isGetPropertyCalled = false;
        m_testEffect->MockGetProperty =
            [&]
            {
            Assert::IsFalse(isGetPropertyCalled);
                isGetPropertyCalled = true;
            };

        float value = 5.0f;
        ThrowIfFailed(m_testEffect->put_StandardDeviation(value));
        ThrowIfFailed(m_testEffect->get_StandardDeviation(&value));

        Assert::IsTrue(isSetPropertyCalled);
        Assert::IsTrue(isGetPropertyCalled);
    }

    TEST_METHOD(CanvasEffect_Inputs)
    {
        ComPtr<IVector<IEffectInput*>> inputs;
        ThrowIfFailed(m_testEffect->get_Inputs(&inputs));

        unsigned int inputSize;
        ThrowIfFailed(inputs->get_Size(&inputSize));
        Assert::AreEqual(m_realInputSize, inputSize);
        for (unsigned int i = 0; i < m_realInputSize; ++i)
        {
            ComPtr<IEffectInput> input;
            inputs->GetAt(i, &input);
            Assert::IsNull(input.Get());
        }

        bool isSetInputCalled = false;
        m_testEffect->MockSetInput =
            [&]
            {
                Assert::IsFalse(isSetInputCalled);
                isSetInputCalled = true;
            };

        bool isGetInputCalled = false;
        m_testEffect->MockGetInput =
            [&]
            {
                Assert::IsFalse(isGetInputCalled);
                isGetInputCalled = true;
            };

        // set same effect as input
        ThrowIfFailed(m_testEffect->put_Source(m_testEffect.Get()));
        ComPtr<IEffectInput> sourceInput;
        ThrowIfFailed(m_testEffect->get_Source(&sourceInput));

        Assert::IsTrue(isSetInputCalled);
        Assert::IsTrue(isGetInputCalled);
    }

    void VerifyEffectRealizationInputs(
        std::shared_ptr<CanvasDrawingSessionManager> const& drawingSessionManager,
        TestEffect* testEffect)
    {
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        deviceContext->MockDrawImage =
            [&](ID2D1Image* image)
        {
        };

        bool setInputCalled = false;
        bool setValueCalled = false;
        bool createEffectCalled = false;
        deviceContext->MockCreateEffect =
            [&](ID2D1Effect** effect)
        {
            Assert::IsFalse(createEffectCalled);
            createEffectCalled = true;

            ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>();
            mockEffect.CopyTo(effect);

            mockEffect->MockSetInput =
                [&]
            {
                Assert::IsFalse(setInputCalled);
                setInputCalled = true;
            };

            mockEffect->MockSetValue =
                [&]
            {
                Assert::IsFalse(setValueCalled);
                setValueCalled = true;
                return S_OK;
            };

            return S_OK;
        };

        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        Vector2 position = { 0, 0 };
        drawingSession->DrawImage(testEffect, position);

        Assert::IsTrue(setInputCalled);
        Assert::IsTrue(setValueCalled);
        Assert::IsTrue(createEffectCalled);
    }

    TEST_METHOD(CanvasEffect_Rerealize)
    {
        // Use a test effect with one source and one property.
        auto testEffect = Make<TestEffect>(m_blurGuid, 1, 1, false);

        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        
        // This test realizes m_testEffect twice, each time with different 
        // device contexts. Verifies that the D2D effect is re-created, and 
        // its inputs and properties are set.
        //
        // TODO #2386: Update this test to react to a change in device instead
        // of device context. 
        //
        
        auto canvasDevice = Make<StubCanvasDevice>();
            canvasDevice->MockCreateBitmap =
                [&]()
                {
                    return Make<StubD2DBitmap>();
                };

        // Set a source and non-default value.
        auto bitmap = CreateTestCanvasBitmap(canvasDevice.Get());
        ComPtr<IEffectInput> effectInput;
        ThrowIfFailed(bitmap.As(&effectInput));
        ThrowIfFailed(testEffect->put_Source(effectInput.Get()));
        ThrowIfFailed(testEffect->put_StandardDeviation(99));

        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
    }

};
