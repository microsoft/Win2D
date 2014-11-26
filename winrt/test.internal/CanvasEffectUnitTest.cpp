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
#include "SwitchableTestBrushFixture.h"
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

    TEST_METHOD_EX(CanvasEffect_Closed)
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

    struct Fixture
    {
        std::shared_ptr<CanvasDrawingSessionManager> m_drawingSessionManager;
        ComPtr<StubD2DDevice> m_stubDevice;
        ComPtr<StubD2DDeviceContextWithGetFactory> m_deviceContext;
        ComPtr<CanvasDrawingSession> m_drawingSession;
        ComPtr<MockD2DEffect> m_mockEffect;
        float m_dpi;

        Fixture()
        {
            m_drawingSessionManager = std::make_shared<CanvasDrawingSessionManager>();
            m_stubDevice = Make<StubD2DDevice>();
            m_deviceContext = MakeDeviceContext();
            m_drawingSession = m_drawingSessionManager->Create(m_deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
            m_mockEffect = Make<MockD2DEffect>();
            m_dpi = DEFAULT_DPI;
        }

        ComPtr<StubD2DDeviceContextWithGetFactory> MakeDeviceContext()
        {
            auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

            deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(m_stubDevice);

            deviceContext->GetDpiMethod.AllowAnyCall(
                [&](float* dpiX, float* dpiY)
            {
                *dpiX = m_dpi;
                *dpiY = m_dpi;
            });

            return deviceContext;
        }
    };

    void VerifyEffectRealizationInputs(
        std::shared_ptr<CanvasDrawingSessionManager> const& drawingSessionManager,
        TestEffect* testEffect)
    {
        Fixture f;

        f.m_deviceContext->DrawImageMethod.AllowAnyCall();

        bool setInputCalled = false;
        bool setInputCountCalled = false;
        bool setValueCalled = false;

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(1,
            [&](IID const&, ID2D1Effect** effect)
            {
                ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>();
                mockEffect.CopyTo(effect);

                mockEffect->MockSetInput =
                    [&](UINT32, ID2D1Image*)
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
                    [&](UINT32, D2D1_PROPERTY_TYPE, CONST BYTE*, UINT32)
                    {
                        Assert::IsFalse(setValueCalled);
                        setValueCalled = true;
                        return S_OK;
                    };

                return S_OK;
            });

        f.m_drawingSession = drawingSessionManager->Create(f.m_deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        Vector2 position = { 0, 0 };
        f.m_drawingSession->DrawImage(testEffect, position);

        Assert::IsTrue(setInputCalled);
        Assert::IsTrue(setInputCountCalled);
        Assert::IsTrue(setValueCalled);

        // Drawing on a second device context that shares the same device should NOT rerealize the effect.
        auto deviceContext2 = f.MakeDeviceContext();
        
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
        Fixture f;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return f.m_mockEffect.CopyTo(effect);
            });

        f.m_mockEffect->MockSetInputCount = [&]
        {
            return S_OK;
        };

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        // Validate drawing with a null input.
        Assert::AreEqual(E_POINTER, f.m_drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));
        
        // Validate drawing with an input that is not the right type.
        auto invalidInput = Make<InvalidEffectInputType>();

        testEffect->SetInput(0, invalidInput.Get());

        Assert::AreEqual(E_NOINTERFACE, f.m_drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));

        ValidateStoredErrorState(E_NOINTERFACE, L"Effect input #0 is an unsupported type. To draw an effect using Win2D, all its inputs must be Win2D ICanvasImage objects.");
    }

    TEST_METHOD_EX(CanvasEffect_CyclicGraph)
    {
        Fixture f;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return f.m_mockEffect.CopyTo(effect);
            });

        bool setInputCountCalled = false;

        f.m_mockEffect->MockSetInputCount = [&]
        {
            Assert::IsFalse(setInputCountCalled);
            setInputCountCalled = true;
            return S_OK;
        };

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        testEffect->put_Source(testEffect.Get());

        Assert::AreEqual(D2DERR_CYCLIC_GRAPH, f.m_drawingSession->DrawImage(testEffect.Get(), Vector2{ 0, 0 }));
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

    TEST_METHOD_EX(CanvasEffect_RealizationRecursion)
    {
        Fixture f;

        auto stubBitmap = CreateStubCanvasBitmap();

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;
        auto createCountingEffect =
            [&](IID const&, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>());
                return mockEffects.back().CopyTo(effect);
            };

        std::vector<ComPtr<TestEffect>> testEffects;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(createCountingEffect);        
        f.m_deviceContext->DrawImageMethod.AllowAnyCall();

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
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Drawing again with no configuration changes should not re-set any state through to D2D.
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the root effect.
        testEffects[0]->put_Source(testEffects[1].Get());
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 1, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the second level effect.
        testEffects[1]->put_Source(testEffects[2].Get());
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 1 }, { 1, 1, 1 });

        // Draw after changing an input of the third level effect.
        testEffects[2]->put_Source(stubBitmap.Get());
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 1, 1, 1 });

        // Draw after changing a property of the root effect.
        testEffects[0]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 1, 1 });

        // Draw after changing a property of the second level effect.
        testEffects[1]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 1 });

        // Draw after changing a property of the third level effect.
        testEffects[2]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the second level of the graph should not re-set anything.
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[1].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the third level of the graph should not re-set anything.
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[2].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Drawing the third level effect on a second device should re-realize just that effect.
        auto deviceContext2 = f.MakeDeviceContext();
        auto drawingSession2 = f.m_drawingSessionManager->Create(deviceContext2.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubDevice2 = Make<StubD2DDevice>();

        deviceContext2->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(stubDevice2);
        deviceContext2->CreateEffectMethod.AllowAnyCall(createCountingEffect);
        deviceContext2->DrawImageMethod.AllowAnyCall();

        ThrowIfFailed(drawingSession2->DrawImage(testEffects[2].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 4, { 2, 2, 2, 1 }, { 2, 2, 2, 1 });

        // Drawing the root effect back on the original device should notice that the third level effect needs to be re-realized.
        ThrowIfFailed(f.m_drawingSession->DrawImage(testEffects[0].Get(), Vector2{ 0, 0 }));
        CheckCallCount(mockEffects, 5, { 2, 3, 2, 1, 1 }, { 2, 2, 2, 1, 1 });
    }

    static void CheckCallCount(std::vector<ComPtr<MockD2DEffectThatCountsCalls>> const& mockEffects,
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

    TEST_METHOD_EX(CanvasEffect_DpiCompensation)
    {
        Fixture f;

        f.m_deviceContext->DrawImageMethod.AllowAnyCall();

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                return mockEffects.back().CopyTo(effect);
            });

        // First we draw a default DPI bitmap onto a default DPI context. This should not insert any DPI compensation.
        auto testBitmap = CreateStubCanvasBitmap(DEFAULT_DPI);
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);

        ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(1, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, testBitmap.Get(), f.m_deviceContext.Get());

        // Now draw a high DPI bitmap. This should insert a DPI compensation effect.
        const float highDpi = 144;
        auto highDpiBitmap = CreateStubCanvasBitmap(highDpi);

        ThrowIfFailed(testEffect->put_Source(highDpiBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[1].Get());
        CheckEffectTypeAndInput(mockEffects[1].Get(), CLSID_D2D1DpiCompensation, highDpiBitmap.Get(), f.m_deviceContext.Get(), highDpi);

        // Drawing a different bitmap should reuse the existing DPI compensation effect.
        const float highDpi2 = 192;
        auto highDpiBitmap2 = CreateStubCanvasBitmap(highDpi2);

        ThrowIfFailed(testEffect->put_Source(highDpiBitmap2.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[1].Get());
        CheckEffectTypeAndInput(mockEffects[1].Get(), CLSID_D2D1DpiCompensation, highDpiBitmap2.Get(), f.m_deviceContext.Get(), highDpi2);

        // Drawing a high DPI bitmap that matches a high DPI device context should remove the DPI compensation effect.
        f.m_dpi = highDpi;

        ThrowIfFailed(testEffect->put_Source(highDpiBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, highDpiBitmap.Get(), f.m_deviceContext.Get());

        // Drawing a high DPI bitmap that doesn't match a different high DPI device context should insert a new DPI compensation effect.
        f.m_dpi = highDpi2;

        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(3, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[2].Get());
        CheckEffectTypeAndInput(mockEffects[2].Get(), CLSID_D2D1DpiCompensation, highDpiBitmap.Get(), f.m_deviceContext.Get(), highDpi);

        // If we insert our own DPI compensation effect in the chain, Win2D should not automatically add a new one.
        auto dpiCompensationEffect = Make<TestEffect>(CLSID_D2D1DpiCompensation, 0, 1, true);

        ThrowIfFailed(testEffect->put_Source(dpiCompensationEffect.Get()));
        ThrowIfFailed(dpiCompensationEffect->put_Source(highDpiBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(4, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[3].Get());
        CheckEffectTypeAndInput(mockEffects[3].Get(), CLSID_D2D1DpiCompensation, highDpiBitmap.Get(), f.m_deviceContext.Get());
        Assert::IsTrue(IsSameInstance(mockEffects[3].Get(), As<ICanvasImageInternal>(dpiCompensationEffect)->GetD2DImage(f.m_deviceContext.Get()).Get()));
    }

    TEST_METHOD_EX(CanvasEffect_EffectAsImageBrushSource_DpiCompensation)
    {
        SwitchableTestBrushFixture f;

        // Feed a bitmap into an effect into the image brush.
        auto testBitmap = CreateStubCanvasBitmap(DEFAULT_DPI);
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);

        ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));

        f.m_canvasImageBrush->SetImage(testEffect.Get());

        auto sourceRectangle = Make<Nullable<Rect>>(Rect{ 0, 0, 1, 1 });
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(sourceRectangle.Get()));

        // Create a drawing session.
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto drawingSession = manager->Create(f.m_canvasDevice.Get(), d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());

        d2dDeviceContext->FillRectangleMethod.AllowAnyCall();
        d2dDeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(Make<StubD2DDevice>());

        float currentDpi = DEFAULT_DPI;

        d2dDeviceContext->GetDpiMethod.AllowAnyCall(
            [&](float* dpiX, float* dpiY)
        {
            *dpiX = currentDpi;
            *dpiY = currentDpi;
        });

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        d2dDeviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const& effectId, ID2D1Effect** effect)
        {
            mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
            return mockEffects.back().CopyTo(effect);
        });

        // Drawing at default DPI should realize the effect graph, but not insert any DPI compensation.
        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get()));

        Assert::AreEqual<size_t>(1, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, testBitmap.Get(), d2dDeviceContext.Get());

        // Drawing with the image brush at a different DPI should insert a DPI compensation effect.
        currentDpi *= 2;

        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[1].Get());
        CheckEffectTypeAndInput(mockEffects[1].Get(), CLSID_D2D1DpiCompensation, testBitmap.Get(), d2dDeviceContext.Get(), DEFAULT_DPI);
    }

    static void CheckEffectTypeAndInput(MockD2DEffectThatCountsCalls* mockEffect, IID const& expectedId, ICanvasImage* expectedInput, ID2D1DeviceContext* deviceContext, float expectedDpi = 0)
    {
        CheckEffectTypeAndInput(mockEffect, expectedId, As<ICanvasImageInternal>(expectedInput)->GetD2DImage(deviceContext).Get(), expectedDpi);
    }

    static void CheckEffectTypeAndInput(MockD2DEffectThatCountsCalls* mockEffect, IID const& expectedId, ID2D1Image* expectedInput, float expectedDpi = 0)
    {
        Assert::IsTrue(!!IsEqualGUID(expectedId, mockEffect->m_effectId));

        Assert::IsTrue(IsSameInstance(expectedInput, mockEffect->m_inputs[0].Get()));

        // If this is a DPI compensation effect, also validate that its DPI got set to what we expect.
        if (expectedDpi != 0)
        {
            Assert::IsTrue(mockEffect->m_properties.size() > D2D1_DPICOMPENSATION_PROP_INPUT_DPI);
            Assert::AreEqual(sizeof(D2D1_VECTOR_2F), mockEffect->m_properties[D2D1_DPICOMPENSATION_PROP_INPUT_DPI].size());

            D2D1_VECTOR_2F dpi = *reinterpret_cast<D2D1_VECTOR_2F*>(&mockEffect->m_properties[D2D1_DPICOMPENSATION_PROP_INPUT_DPI].front());

            Assert::AreEqual(expectedDpi, dpi.x);
            Assert::AreEqual(expectedDpi, dpi.y);
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
