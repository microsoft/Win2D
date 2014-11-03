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
        ComPtr<IVector<IInspectable*>> properties;
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
        ThrowIfFailed(m_testEffect->put_BlurAmount(value));
        ThrowIfFailed(m_testEffect->get_BlurAmount(&value));

        Assert::IsTrue(isSetPropertyCalled);
        Assert::IsTrue(isGetPropertyCalled);
    }

    TEST_METHOD(CanvasEffect_Closed)
    {
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = { 0 };

        auto canvasEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realInputSize, false);
        auto drawingSession = CreateStubDrawingSession();

        Assert::AreEqual(S_OK, canvasEffect->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasEffect->GetBounds(drawingSession.Get(), &bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasEffect->GetBoundsWithTransform(drawingSession.Get(), matrix, &bounds));
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

        Assert::AreEqual(As<IUnknown>(sourceInput).Get(), As<IUnknown>(m_testEffect).Get());

        // set null input
        isSetInputCalled = isGetInputCalled = false;

        ThrowIfFailed(m_testEffect->put_Source(nullptr));
        ThrowIfFailed(m_testEffect->get_Source(&sourceInput));

        Assert::IsTrue(isSetInputCalled);
        Assert::IsTrue(isGetInputCalled);

        Assert::IsNull(sourceInput.Get());
    }

    void VerifyEffectRealizationInputs(
        std::shared_ptr<CanvasDrawingSessionManager> const& drawingSessionManager,
        TestEffect* testEffect)
    {
        ComPtr<StubD2DDevice> stubDevice = Make<StubD2DDevice>();
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

        deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice);
        deviceContext->DrawImageMethod.AllowAnyCall();

        bool setInputCalled = false;
        bool setInputCountCalled = false;
        bool setValueCalled = false;

        deviceContext->CreateEffectMethod.SetExpectedCalls(1,
            [&](IID const&, ID2D1Effect** effect)
            {
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
            });

        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        Vector2 position = { 0, 0 };
        drawingSession->DrawImage(testEffect, position);

        Assert::IsTrue(setInputCalled);
        Assert::IsTrue(setInputCountCalled);
        Assert::IsTrue(setValueCalled);

        // Drawing on a second device context that shares the same device should NOT rerealize the effect.
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext2 = Make<StubD2DDeviceContextWithGetFactory>();

        deviceContext2->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice);
        deviceContext2->DrawImageMethod.AllowAnyCall();
        deviceContext2->CreateEffectMethod.SetExpectedCalls(0);

        auto drawingSession2 = drawingSessionManager->Create(deviceContext2.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        drawingSession2->DrawImage(testEffect, position);
    }

    TEST_METHOD_EX(CanvasEffect_Rerealize)
    {
        // Use a test effect with one source and one property.
        auto testEffect = Make<TestEffect>(m_blurGuid, 1, 1, false);

        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        
        // This test realizes m_testEffect twice, each time with different 
        // devices. Verifies that the D2D effect is re-created, and 
        // its inputs and properties are set.
        
        // Set a source and non-default value.
        ThrowIfFailed(testEffect->put_Source(As<IEffectInput>(CreateStubCanvasBitmap()).Get()));
        ThrowIfFailed(testEffect->put_BlurAmount(99));

        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
        VerifyEffectRealizationInputs(drawingSessionManager, testEffect.Get());
    }

    class InvalidEffectInputType : public RuntimeClass<IEffectInput>
    {
        InspectableClass(L"InvalidEffectInputType", BaseTrust);
    };

    TEST_METHOD_EX(CanvasEffect_WrongInputType)
    {
        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto mockEffect = Make<MockD2DEffect>();
        auto stubDevice = Make<StubD2DDevice>();

        deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice);
        deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return mockEffect.CopyTo(effect);
            });

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

    TEST_METHOD_EX(CanvasEffect_CyclicGraph)
    {
        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto mockEffect = Make<MockD2DEffect>();
        auto stubDevice = Make<StubD2DDevice>();

        deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice);

        deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return mockEffect.CopyTo(effect);
            });

        bool setInputCountCalled = false;

        mockEffect->MockSetInputCount = [&]
        {
            Assert::IsFalse(setInputCountCalled);
            setInputCountCalled = true;
            return S_OK;
        };

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        testEffect->put_Source(testEffect.Get());

        Assert::AreEqual(D2DERR_CYCLIC_GRAPH, drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));
    }

    TEST_METHOD_EX(CanvasEffect_GetBounds_NullArg)
    {
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = { 0 };

        auto canvasEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realInputSize, false);
        auto drawingSession = CreateStubDrawingSession();

        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBounds(nullptr, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBounds(drawingSession.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBoundsWithTransform(nullptr, matrix, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBoundsWithTransform(drawingSession.Get(), matrix, nullptr));
    }

    class MockEffectThatCountsCalls : public MockD2DEffect
    {
    public:
        int m_setInputCalls;
        int m_setValueCalls;

        MockEffectThatCountsCalls()
          : m_setInputCalls(0),
            m_setValueCalls(0)
        {
            MockSetInputCount = []
            {
                return S_OK;
            };

            MockSetInput = [&]
            {
                m_setInputCalls++;
            };

            MockSetValue = [&]
            {
                m_setValueCalls++;
                return S_OK;
            };
        }
    };

    TEST_METHOD_EX(CanvasEffect_RealizationRecursion)
    {
        auto drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession = drawingSessionManager->Create(deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubDevice = Make<StubD2DDevice>();
        auto stubBitmap = CreateStubCanvasBitmap();

        std::vector<ComPtr<MockEffectThatCountsCalls>> mockEffects;
        auto createCountingEffect =
            [&](IID const&, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockEffectThatCountsCalls>());
                return mockEffects.back().CopyTo(effect);
            };

        std::vector<ComPtr<TestEffect>> testEffects;

        deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice);
        deviceContext->CreateEffectMethod.AllowAnyCall(createCountingEffect);        
        deviceContext->DrawImageMethod.AllowAnyCall();

        // Create three effects, connected as each other's inputs.
        for (int i = 0; i < 3; i++)
        {
            testEffects.push_back(Make<TestEffect>(m_blurGuid, 1, 1, false));
        }

        testEffects[0]->put_Source(testEffects[1].Get());
        testEffects[1]->put_Source(testEffects[2].Get());
        testEffects[2]->put_Source(stubBitmap.Get());

        testEffects[0]->put_BlurAmount(0);
        testEffects[1]->put_BlurAmount(0);
        testEffects[2]->put_BlurAmount(0);

        // Drawing the first time should set properties and inputs on all three effects.
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Drawing again with no configuration changes should not re-set any state through to D2D.
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the root effect.
        testEffects[0]->put_Source(testEffects[1].Get());
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 1, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the second level effect.
        testEffects[1]->put_Source(testEffects[2].Get());
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the third level effect.
        testEffects[2]->put_Source(stubBitmap.Get());
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 1, 1, 1 });

        // Draw after changing a property of the root effect.
        testEffects[0]->put_BlurAmount(1);
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 1, 1 });

        // Draw after changing a property of the second level effect.
        testEffects[1]->put_BlurAmount(1);
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 1 });

        // Draw after changing a property of the third level effect.
        testEffects[2]->put_BlurAmount(1);
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the second level of the graph should not re-set anything.
        ThrowIfFailed(drawingSession->DrawImage(testEffects[1].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the third level of the graph should not re-set anything.
        ThrowIfFailed(drawingSession->DrawImage(testEffects[2].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Drawing the third level effect on a second device should re-realize just that effect.
        auto deviceContext2 = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession2 = drawingSessionManager->Create(deviceContext2.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubDevice2 = Make<StubD2DDevice>();

        deviceContext2->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice2);
        deviceContext2->CreateEffectMethod.AllowAnyCall(createCountingEffect);
        deviceContext2->DrawImageMethod.AllowAnyCall();

        ThrowIfFailed(drawingSession2->DrawImage(testEffects[2].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 4, { 2, 2, 2, 1 }, { 2, 2, 2, 1 });

        // Drawing the root effect back on the original device should notice that the third level effect needs to be re-realized.
        ThrowIfFailed(drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 5, { 2, 3, 2, 1, 1 }, { 2, 2, 2, 1, 1 });
    }

    void CheckCallCount(std::vector<ComPtr<MockEffectThatCountsCalls>> const& mockEffects,
                        size_t expectedEffectCount,
                        std::initializer_list<int> const& expectedSetInputCalls,
                        std::initializer_list<int> const& expectedSetValueCalls)
    {
        Assert::AreEqual(expectedEffectCount, mockEffects.size());
        Assert::AreEqual(expectedEffectCount, expectedSetInputCalls.size());
        Assert::AreEqual(expectedEffectCount, expectedSetValueCalls.size());

        auto effect = mockEffects.begin();
        auto expectedSetInputs = expectedSetInputCalls.begin();
        auto expectedSetValues = expectedSetValueCalls.begin();

        for (size_t i = 0; i < expectedEffectCount; i++)
        {
            Assert::AreEqual(*expectedSetInputs, (*effect)->m_setInputCalls);
            Assert::AreEqual(*expectedSetValues, (*effect)->m_setValueCalls);

            ++effect;
            ++expectedSetInputs;
            ++expectedSetValues;
        }
    }

    // DImage defines separate (but identical) enum types for different effects.
    // Effects codegen tool collapses this duplication in the WinRT projection.
    // Let's validate that the native enums really are the same!

    static_assert(D2D1_3DTRANSFORM_INTERPOLATION_MODE_NEAREST_NEIGHBOR == (int)CanvasImageInterpolation::NearestNeighbor, "enums should match");
    static_assert(D2D1_3DTRANSFORM_INTERPOLATION_MODE_LINEAR == (int)CanvasImageInterpolation::Linear, "enums should match");
    static_assert(D2D1_3DTRANSFORM_INTERPOLATION_MODE_CUBIC == (int)CanvasImageInterpolation::Cubic, "enums should match");
    static_assert(D2D1_3DTRANSFORM_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR == (int)CanvasImageInterpolation::MultiSampleLinear, "enums should match");
    static_assert(D2D1_3DTRANSFORM_INTERPOLATION_MODE_ANISOTROPIC == (int)CanvasImageInterpolation::Anisotropic, "enums should match");

    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_NEAREST_NEIGHBOR == (int)CanvasImageInterpolation::NearestNeighbor, "enums should match");
    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR == (int)CanvasImageInterpolation::Linear, "enums should match");
    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_CUBIC == (int)CanvasImageInterpolation::Cubic, "enums should match");
    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR == (int)CanvasImageInterpolation::MultiSampleLinear, "enums should match");
    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_ANISOTROPIC == (int)CanvasImageInterpolation::Anisotropic, "enums should match");
    static_assert(D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC == (int)CanvasImageInterpolation::HighQualityCubic, "enums should match");

    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_NEAREST_NEIGHBOR == (int)CanvasImageInterpolation::NearestNeighbor, "enums should match");
    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_LINEAR == (int)CanvasImageInterpolation::Linear, "enums should match");
    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_CUBIC == (int)CanvasImageInterpolation::Cubic, "enums should match");
    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_MULTI_SAMPLE_LINEAR == (int)CanvasImageInterpolation::MultiSampleLinear, "enums should match");
    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_ANISOTROPIC == (int)CanvasImageInterpolation::Anisotropic, "enums should match");
    static_assert(D2D1_CONVOLVEMATRIX_SCALE_MODE_HIGH_QUALITY_CUBIC == (int)CanvasImageInterpolation::HighQualityCubic, "enums should match");

    static_assert(D2D1_BORDER_EDGE_MODE_CLAMP == (int)CanvasEdgeBehavior::Clamp, "enums should match");
    static_assert(D2D1_BORDER_EDGE_MODE_WRAP == (int)CanvasEdgeBehavior::Wrap, "enums should match");
    static_assert(D2D1_BORDER_EDGE_MODE_MIRROR == (int)CanvasEdgeBehavior::Mirror, "enums should match");
};
