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
#include "TestEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CanvasEffectUnitTest)
{
public:

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
        ComPtr<StubD2DDevice> stubDevice = Make<StubD2DDevice>();
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

        deviceContext->MockGetDevice =
            [&](ID2D1Device** device)
        {
            ThrowIfFailed(stubDevice.CopyTo(device));
        };

        deviceContext->MockDrawImage =
            [&](ID2D1Image* image)
        {
        };

        bool setInputCalled = false;
        bool setInputCountCalled = false;
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

            mockEffect->MockSetInputCount =
                [&]
            {
                Assert::IsFalse(setInputCountCalled);
                setInputCountCalled = true;
                return S_OK;
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
        Assert::IsTrue(setInputCountCalled);
        Assert::IsTrue(setValueCalled);
        Assert::IsTrue(createEffectCalled);

        // Drawing on a second device context that shares the same device should NOT rerealize the effect.
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext2 = Make<StubD2DDeviceContextWithGetFactory>();

        deviceContext2->MockGetDevice = deviceContext->MockGetDevice;
        deviceContext2->MockDrawImage = deviceContext->MockDrawImage;

        deviceContext2->MockCreateEffect = [](ID2D1Effect**)
        {
            Assert::Fail(L"Effect is already realized - this should not get called.");
            return E_UNEXPECTED;
        };

        auto drawingSession2 = drawingSessionManager->Create(deviceContext2.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        drawingSession2->DrawImage(testEffect, position);
    }

    TEST_METHOD(CanvasEffect_Rerealize)
    {
        // Use a test effect with one source and one property.
        auto testEffect = Make<TestEffect>(m_blurGuid, 1, 1, false);

        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        
        // This test realizes m_testEffect twice, each time with different 
        // devices. Verifies that the D2D effect is re-created, and 
        // its inputs and properties are set.
        
        // Set a source and non-default value.
        ThrowIfFailed(testEffect->put_Source(CreateStubCanvasBitmap().Get()));
        ThrowIfFailed(testEffect->put_StandardDeviation(99));

        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
    }

    class InvalidEffectInputType : public RuntimeClass<IEffectInput>
    {
        InspectableClass(L"InvalidEffectInputType", BaseTrust);
    };

    TEST_METHOD(CanvasEffect_WrongInputType)
    {
        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto mockEffect = Make<MockD2DEffect>();
        auto stubDevice = Make<StubD2DDevice>();

        deviceContext->MockGetDevice = [&](ID2D1Device** device)
        {
            ThrowIfFailed(stubDevice.CopyTo(device));
        };

        deviceContext->MockCreateEffect = [&](ID2D1Effect** effect)
        { 
            return mockEffect.CopyTo(effect);
        };

        mockEffect->MockSetInputCount = [&]
        {
            return S_OK;
        };

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        // Validate drawing with a null input.
        Assert::AreEqual(E_POINTER, drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));
        
        // Validate drawing with an input that is not the right type.
        auto invalidInput = Make<InvalidEffectInputType>();

        testEffect->SetInput(0, invalidInput.Get());

        Assert::AreEqual(E_NOINTERFACE, drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));

        ValidateStoredErrorState(E_NOINTERFACE, L"Effect input #0 is an unsupported type. To draw an effect using Win2D, all its inputs must be Win2D ICanvasImage objects.");
    }
};
