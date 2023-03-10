// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/images/CanvasCommandList.h>

#include "stubs/TestEffect.h"
#include "stubs/StubD2DEffect.h"

#if WINVER > _WIN32_WINNT_WINBLUE
#include <lib/effects/generated/AlphaMaskEffect.h>
#include <lib/effects/generated/CrossFadeEffect.h>
#include <lib/effects/generated/OpacityEffect.h>
#include <lib/effects/generated/TintEffect.h>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CanvasEffectUnitTest)
{
public:

    ComPtr<TestEffect> m_testEffect;
    unsigned int m_realPropertiesSize = 4;
    unsigned int m_realSourcesSize = 8;
    GUID m_blurGuid = CLSID_D2D1GaussianBlur;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_testEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realSourcesSize, false);
    }

    TEST_METHOD_EX(CanvasEffect_Implements_Expected_Interfaces)
    {
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IGraphicsEffect);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IGraphicsEffectSource);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ICanvasImageInterop);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, ICanvasImageInternal);
        ASSERT_IMPLEMENTS_INTERFACE(m_testEffect, IGaussianBlurEffect);
    }

    TEST_METHOD_EX(CanvasEffect_Properties)
    {
        unsigned int propertiesSize;
        ThrowIfFailed(m_testEffect->GetPropertyCount(&propertiesSize));
        Assert::AreEqual(m_realPropertiesSize, propertiesSize);
        for (unsigned int i = 0; i < m_realPropertiesSize; ++i)
        {
            ComPtr<IPropertyValue> propertyValue;
            ThrowIfFailed(m_testEffect->GetProperty(i, &propertyValue));
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

        auto canvasEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realSourcesSize, false);
        auto device = Make<StubCanvasDevice>();

        Assert::AreEqual(S_OK, canvasEffect->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasEffect->GetBounds(device.Get(), &bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasEffect->GetBoundsWithTransform(device.Get(), matrix, &bounds));

        ComPtr<ID2D1Effect> resource;
        Assert::AreEqual(RO_E_CLOSED, canvasEffect->GetNativeResource(nullptr, 0, IID_PPV_ARGS(&resource)));
    }

    TEST_METHOD_EX(CanvasEffect_Sources)
    {
        unsigned int sourceSize;
        ThrowIfFailed(m_testEffect->GetSourceCount(&sourceSize));
        Assert::AreEqual(m_realSourcesSize, sourceSize);
        for (unsigned int i = 0; i < m_realSourcesSize; ++i)
        {
            ComPtr<IGraphicsEffectSource> source;
            ThrowIfFailed(m_testEffect->GetSource(i, &source));
            Assert::IsNull(source.Get());
        }

        bool isSetSourceCalled = false;
        m_testEffect->MockSetSource =
            [&]
            {
                Assert::IsFalse(isSetSourceCalled);
                isSetSourceCalled = true;
            };

        bool isGetSourceCalled = false;
        m_testEffect->MockGetSource =
            [&]
            {
                Assert::IsFalse(isGetSourceCalled);
                isGetSourceCalled = true;
            };

        // set same effect as source
        ThrowIfFailed(m_testEffect->put_Source(m_testEffect.Get()));
        ComPtr<IGraphicsEffectSource> source;
        ThrowIfFailed(m_testEffect->get_Source(&source));

        Assert::IsTrue(isSetSourceCalled);
        Assert::IsTrue(isGetSourceCalled);

        Assert::AreEqual(As<IUnknown>(source).Get(), As<IUnknown>(m_testEffect).Get());

        // set null source
        isSetSourceCalled = isGetSourceCalled = false;

        ThrowIfFailed(m_testEffect->put_Source(nullptr));
        ThrowIfFailed(m_testEffect->get_Source(&source));

        Assert::IsTrue(isSetSourceCalled);
        Assert::IsTrue(isGetSourceCalled);

        Assert::IsNull(source.Get());
    }

    struct Fixture
    {
        ComPtr<StubD2DDevice> m_stubDevice;
        ComPtr<StubD2DDeviceContextWithGetFactory> m_deviceContext;
        ComPtr<CanvasDrawingSession> m_drawingSession;
        ComPtr<StubCanvasDevice> m_canvasDevice;
        float m_dpi;

        Fixture()
        {
            m_stubDevice = Make<StubD2DDevice>();
            m_deviceContext = MakeDeviceContext();
            m_canvasDevice = Make<StubCanvasDevice>(m_stubDevice);
            m_drawingSession = CanvasDrawingSession::CreateNew(m_deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), m_canvasDevice.Get());
            m_dpi = DEFAULT_DPI;
        }

        ComPtr<StubD2DDeviceContextWithGetFactory> MakeDeviceContext()
        {
            auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

            deviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(m_stubDevice);
            deviceContext->GetPrimitiveBlendMethod.AllowAnyCall();

            deviceContext->GetDpiMethod.AllowAnyCall(
                [&](float* dpiX, float* dpiY)
                {
                    *dpiX = m_dpi;
                    *dpiY = m_dpi;
                });

            deviceContext->GetTargetMethod.AllowAnyCall(
                [] (ID2D1Image** target)
                {
                    *target = nullptr;
                });

            return deviceContext;
        }
    };

    TEST_METHOD_EX(CanvasEffect_Rerealize)
    {
        // Use a test effect with one source and one property.
        auto testEffect = Make<TestEffect>(m_blurGuid, 1, 1, false);

        // This test realizes m_testEffect twice, each time with different 
        // devices. Verifies that the D2D effect is re-created, and 
        // its sources and properties are set.
        for (int pass = 0; pass < 2; pass++)
        {
            Fixture f;

            // Set a source and non-default value.
            auto stubBitmap = CreateStubCanvasBitmap(DEFAULT_DPI, f.m_canvasDevice.Get());

            ThrowIfFailed(testEffect->put_Source(As<IGraphicsEffectSource>(stubBitmap).Get()));
            ThrowIfFailed(testEffect->put_BlurAmount(99));

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

                    mockEffect->MockGetInput =
                        [&](UINT32, ID2D1Image** input)
                        {
                            ThrowIfFailed(stubBitmap->GetD2DImage(nullptr, nullptr, WIN2D_GET_D2D_IMAGE_FLAGS_NONE, 0, nullptr).CopyTo(input));
                        };

                    mockEffect->MockGetInputCount =
                        []
                        {
                            return 1;
                        };

                    mockEffect->MockSetInputCount =
                        [&](UINT32)
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

                    mockEffect->MockGetType =
                        [](UINT32)
                        {
                            return D2D1_PROPERTY_TYPE_FLOAT;
                        };

                    mockEffect->MockGetValue =
                        [](UINT32 index, D2D1_PROPERTY_TYPE type, BYTE *data, UINT32 dataSize)
                        {
                            ZeroMemory(data, dataSize);
                            return S_OK;
                        };

                    return S_OK;
                });

            f.m_drawingSession = nullptr;
            f.m_drawingSession = CanvasDrawingSession::CreateNew(f.m_deviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), f.m_canvasDevice.Get());

            ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

            Assert::IsTrue(setInputCalled);
            Assert::IsTrue(setInputCountCalled);
            Assert::IsTrue(setValueCalled);

            // Drawing on a second device context that shares the same device should NOT rerealize the effect.
            auto deviceContext2 = f.MakeDeviceContext();
        
            deviceContext2->DrawImageMethod.AllowAnyCall();
            deviceContext2->CreateEffectMethod.SetExpectedCalls(0);

            auto drawingSession2 = CanvasDrawingSession::CreateNew(deviceContext2.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), f.m_canvasDevice.Get());

            ThrowIfFailed(drawingSession2->DrawImageAtOrigin(testEffect.Get()));
        }
    };

    class InvalidEffectSourceType : public RuntimeClass<IGraphicsEffectSource>
    {
        InspectableClass(L"InvalidEffectSourceType", BaseTrust);
    };

    TEST_METHOD_EX(CanvasEffect_WrongSourceType)
    {
        Fixture f;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return Make<StubD2DEffect>(m_blurGuid).CopyTo(effect);
            });

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        // Validate drawing with a null source.
        Assert::AreEqual(E_INVALIDARG, f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));
        
        // Validate drawing with a source that is not the right type.
        auto invalidSource = Make<InvalidEffectSourceType>();

        testEffect->SetSource(0, invalidSource.Get());

        Assert::AreEqual(E_NOINTERFACE, f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        ValidateStoredErrorState(E_NOINTERFACE, L"Effect source #0 is an unsupported type. To draw an effect using Win2D, all its sources must be Win2D ICanvasImage objects.");
    }

    TEST_METHOD_EX(CanvasEffect_CyclicGraph)
    {
        Fixture f;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const&, ID2D1Effect** effect)
            {
                return Make<StubD2DEffect>(m_blurGuid).CopyTo(effect);
            });

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, false);

        testEffect->put_Source(testEffect.Get());

        Assert::AreEqual(D2DERR_CYCLIC_GRAPH, f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        // Break the cycle so we don't leak memory.
        testEffect->put_Source(nullptr);
    }

    TEST_METHOD_EX(CanvasEffect_GetBounds_NullArg)
    {
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = { 0 };

        auto canvasEffect = Make<TestEffect>(m_blurGuid, m_realPropertiesSize, m_realSourcesSize, false);
        auto device = Make<StubCanvasDevice>();

        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBounds(nullptr, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBounds(device.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBoundsWithTransform(nullptr, matrix, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasEffect->GetBoundsWithTransform(device.Get(), matrix, nullptr));
    }

    TEST_METHOD_EX(CanvasEffect_RealizationRecursion)
    {
        Fixture f;

        auto stubBitmap = CreateStubCanvasBitmap(DEFAULT_DPI, f.m_canvasDevice.Get());

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        auto createCountingEffect =
            [&](IID const&, ID2D1Effect** effect)
            {
                auto mockEffect = Make<MockD2DEffectThatCountsCalls>();

                mockEffect->MockGetType =
                    [](UINT32)
                    {
                        return D2D1_PROPERTY_TYPE_FLOAT;
                    };

                mockEffect->MockGetValue =
                    [](UINT32 index, D2D1_PROPERTY_TYPE type, BYTE *data, UINT32 dataSize)
                    {
                        ZeroMemory(data, dataSize);
                        return S_OK;
                    };

                mockEffects.push_back(mockEffect);

                return mockEffect.CopyTo(effect);
            };

        std::vector<ComPtr<TestEffect>> testEffects;

        f.m_deviceContext->CreateEffectMethod.AllowAnyCall(createCountingEffect);        
        f.m_deviceContext->DrawImageMethod.AllowAnyCall();

        f.m_canvasDevice->GetResourceCreationDeviceContextMethod.AllowAnyCall(
            [&]
            {
                return DeviceContextLease(As<ID2D1DeviceContext1>(f.m_deviceContext));
            });

        // Create three effects, connected as each other's sources.
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

        // Drawing the first time should set properties and sources on all three effects.
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Drawing again with no configuration changes should not re-set any state through to D2D.
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 1, 1, 1 }, { 1, 1, 1 });

        // Draw after changing a source of the root effect.
        testEffects[0]->put_Source(testEffects[1].Get());
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 1, 1 }, { 1, 1, 1 });

        // Draw after changing a source of the second level effect.
        testEffects[1]->put_Source(testEffects[2].Get());
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 1 }, { 1, 1, 1 });

        // Draw after changing a source of the third level effect.
        testEffects[2]->put_Source(stubBitmap.Get());
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 1, 1, 1 });

        // Draw after changing a property of the root effect.
        testEffects[0]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 1, 1 });

        // Draw after changing a property of the second level effect.
        testEffects[1]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 1 });

        // Draw after changing a property of the third level effect.
        testEffects[2]->put_BlurAmount(1);
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[0].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the second level of the graph should not re-set anything.
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[1].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });

        // Draw starting at the third level of the graph should not re-set anything.
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffects[2].Get()));
        CheckCallCount(mockEffects, 3, { 2, 2, 2 }, { 2, 2, 2 });
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
            wchar_t input_msg[256], value_msg[256];

            swprintf_s(input_msg, L"effect #%Id inputs", i);
            swprintf_s(value_msg, L"effect #%Id values", i);

            Assert::AreEqual(*expectedSetInputs, (*effect)->m_setInputCalls, input_msg);
            Assert::AreEqual(*expectedSetValues, (*effect)->m_setValueCalls, value_msg);

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

        f.m_canvasDevice->GetResourceCreationDeviceContextMethod.AllowAnyCall(
            [&]
            {
                return DeviceContextLease(As<ID2D1DeviceContext1>(f.m_deviceContext));
            });

        // First we draw a default DPI bitmap onto a default DPI context. This should not insert any DPI compensation.
        auto testBitmap = CreateStubCanvasBitmap(DEFAULT_DPI, f.m_canvasDevice.Get());
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);

        ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(1, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, testBitmap.Get(), f.m_deviceContext.Get());

        // Now draw a high DPI bitmap. This should insert a DPI compensation effect.
        const float highDpi = 144;
        auto highDpiBitmap = CreateStubCanvasBitmap(highDpi, f.m_canvasDevice.Get());

        ThrowIfFailed(testEffect->put_Source(highDpiBitmap.Get()));
        ThrowIfFailed(f.m_drawingSession->DrawImageAtOrigin(testEffect.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[1].Get());
        CheckEffectTypeAndInput(mockEffects[1].Get(), CLSID_D2D1DpiCompensation, highDpiBitmap.Get(), f.m_deviceContext.Get(), highDpi);

        // Drawing a different bitmap should reuse the existing DPI compensation effect.
        const float highDpi2 = 192;
        auto highDpiBitmap2 = CreateStubCanvasBitmap(highDpi2, f.m_canvasDevice.Get());

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
        Assert::IsTrue(IsSameInstance(mockEffects[3].Get(), As<ICanvasImageInternal>(dpiCompensationEffect)->GetD2DImage(f.m_canvasDevice.Get(), f.m_deviceContext.Get()).Get()));
    }

    struct CommandListFixture
    {
        ComPtr<StubCanvasDevice> CanvasDevice;
        ComPtr<MockD2DDeviceContext> DeviceContext;
        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> MockEffects;
        ComPtr<ID2D1Image> CurrentTarget;
        ComPtr<CanvasCommandListFactory> CommandListFactory;

        CommandListFixture()
            : CanvasDevice(Make<StubCanvasDevice>())
            , DeviceContext(Make<MockD2DDeviceContext>())
            , CommandListFactory(Make<CanvasCommandListFactory>())
        {
            DeviceContext->CreateEffectMethod.AllowAnyCall(
                [=](IID const& effectId, ID2D1Effect** effect)
                {
                    MockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                    return MockEffects.back().CopyTo(effect);
                });
            
            DeviceContext->BeginDrawMethod.AllowAnyCall();
            DeviceContext->EndDrawMethod.AllowAnyCall();
            DeviceContext->DrawImageMethod.AllowAnyCall();
            DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall();
            DeviceContext->SetTextAntialiasModeMethod.AllowAnyCall();

            DeviceContext->SetTargetMethod.SetExpectedCalls(1,
                [&] (ID2D1Image* newTarget)
                {
                    CurrentTarget = newTarget;
                });
            
            DeviceContext->GetTargetMethod.SetExpectedCalls(1,
                [&] (ID2D1Image** value)
                {
                    CurrentTarget.CopyTo(value);
                });

            DeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(CanvasDevice->GetD2DDevice());

            // CanvasEffect must not query the DPI when the device context is being
            // used with a command list.
            DeviceContext->GetDpiMethod.SetExpectedCalls(0);

            CanvasDevice->CreateCommandListMethod.AllowAnyCall(
                [] 
                { 
                    auto d2dCl = Make<MockD2DCommandList>();
                    d2dCl->CloseMethod.AllowAnyCall();
                    return d2dCl;
                });

            CanvasDevice->CreateDeviceContextForDrawingSessionMethod.SetExpectedCalls(1, [=] { return DeviceContext; });            
        }

        ComPtr<ICanvasCommandList> CreateCommandList()
        {
            ComPtr<ICanvasCommandList> cl;
            ThrowIfFailed(CommandListFactory->Create(CanvasDevice.Get(), &cl));
            return cl;
        }

        void DrawEffectToCommandList(ComPtr<TestEffect> effect)
        {
            auto cl = CreateCommandList();

            ComPtr<ICanvasDrawingSession> drawingSession;
            ThrowIfFailed(cl->CreateDrawingSession(&drawingSession));

            ThrowIfFailed(drawingSession->DrawImageAtOrigin(effect.Get()));
        }        
    };

    TEST_METHOD_EX(CanvasEffect_When_BitmapSource_DrawnIntoCommandList_DpiCompensationAlwaysAdded)
    {
        for (auto dpi : { DEFAULT_DPI / 2, DEFAULT_DPI, DEFAULT_DPI * 2 })
        {
            Check_BitmapSource_DrawnIntoCommandList_DpiCompensationAdded(dpi);
        }
    }

    void Check_BitmapSource_DrawnIntoCommandList_DpiCompensationAdded(float dpi)
    {
        CommandListFixture f;

        auto testBitmap = CreateStubCanvasBitmap(dpi, f.CanvasDevice.Get());
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);
        ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));

        f.DrawEffectToCommandList(testEffect);

        Assert::AreEqual<size_t>(2, f.MockEffects.size());
        CheckEffectTypeAndInput(f.MockEffects[0].Get(), m_blurGuid, f.MockEffects[1].Get());
        CheckEffectTypeAndInput(f.MockEffects[1].Get(), CLSID_D2D1DpiCompensation, testBitmap.Get(), f.DeviceContext.Get(), dpi);
    }

    TEST_METHOD_EX(CanvasEffect_When_DpiIndependentEffect_DrawingIntoCommandList_NoDpiCompensationIsAdded)
    {
        CommandListFixture f;

        auto commandListUsedAsEffectSource = f.CreateCommandList();
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);
        ThrowIfFailed(testEffect->put_Source(As<IGraphicsEffectSource>(commandListUsedAsEffectSource).Get()));

        f.DrawEffectToCommandList(testEffect);

        Assert::AreEqual<size_t>(1, f.MockEffects.size());
        Assert::IsTrue(!!IsEqualGUID(m_blurGuid, f.MockEffects[0]->m_effectId));
    }

    TEST_METHOD_EX(CanvasEffect_EffectAsImageBrushSource_DpiCompensation)
    {
        SwitchableTestBrushFixture f;

        // Feed a bitmap into an effect into the image brush.
        auto testBitmap = CreateStubCanvasBitmap(DEFAULT_DPI, f.m_canvasDevice.Get());
        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1, true);
        
        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_d2dDeviceContext->CreateEffectMethod.AllowAnyCall(
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                return mockEffects.back().CopyTo(effect);
            });

        ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));

        ThrowIfFailed(f.m_canvasImageBrush->put_Image(testEffect.Get()));

        auto sourceRectangle = Make<Nullable<Rect>>(Rect{ 0, 0, 1, 1 });
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(sourceRectangle.Get()));

        // Create a drawing session.
        auto drawingSession = CanvasDrawingSession::CreateNew(f.m_d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), f.m_canvasDevice.Get());

        f.m_d2dDeviceContext->FillRectangleMethod.AllowAnyCall();
        f.m_d2dDeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(f.m_canvasDevice->GetD2DDevice());
        f.m_d2dDeviceContext->GetTargetMethod.AllowAnyCallAlwaysCopyValueToParam<ID2D1Image>(nullptr);

        float currentDpi = DEFAULT_DPI;

        f.m_d2dDeviceContext->GetDpiMethod.AllowAnyCall(
            [&](float* dpiX, float* dpiY)
            {
                *dpiX = currentDpi;
                *dpiY = currentDpi;
            });

        // Drawing at default DPI should realize the effect graph, but not insert any DPI compensation.
        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get()));

        Assert::AreEqual<size_t>(1, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, testBitmap.Get(), f.m_d2dDeviceContext.Get());

        // Drawing with the image brush at a different DPI should insert a DPI compensation effect.
        currentDpi *= 2;

        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get()));

        Assert::AreEqual<size_t>(2, mockEffects.size());
        CheckEffectTypeAndInput(mockEffects[0].Get(), m_blurGuid, mockEffects[1].Get());
        CheckEffectTypeAndInput(mockEffects[1].Get(), CLSID_D2D1DpiCompensation, testBitmap.Get(), f.m_d2dDeviceContext.Get(), DEFAULT_DPI);
    }

    TEST_METHOD_EX(CanvasEffect_InvalidateSourceRectangle)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto mockD2DEffect = Make<MockD2DEffectThatCountsCalls>();

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(1, [&](IID const& effectId, ID2D1Effect** effect) { return mockD2DEffect.CopyTo(effect); });

        Rect rect = { 1, 2, 3, 4 };

        Assert::AreEqual(E_INVALIDARG, testEffect->InvalidateSourceRectangle(nullptr, 0, rect));
        Assert::AreEqual(E_INVALIDARG, testEffect->InvalidateSourceRectangle(f.m_drawingSession.Get(), 1, rect));

        f.m_deviceContext->InvalidateEffectInputRectangleMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, UINT32 input, D2D1_RECT_F const* invalidRect)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                Assert::AreEqual(0u, input);
                Assert::AreEqual(rect, FromD2DRect(*invalidRect));

                return S_OK;
            });

        ThrowIfFailed(testEffect->InvalidateSourceRectangle(f.m_drawingSession.Get(), 0, rect));
    }

    TEST_METHOD_EX(CanvasEffect_InvalidateSourceRectangle_ForICanvasImageInterop)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto mockD2DEffect = Make<MockD2DEffectThatCountsCalls>();

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(1, [&](IID const& effectId, ID2D1Effect** effect) { return mockD2DEffect.CopyTo(effect); });

        Rect rect = { 1, 2, 3, 4 };

        Assert::AreEqual(E_INVALIDARG, InvalidateSourceRectangleForICanvasImageInterop(nullptr, As<ICanvasImageInterop>(testEffect.Get()).Get(), 0, &rect));
        Assert::AreEqual(E_INVALIDARG, InvalidateSourceRectangleForICanvasImageInterop(f.m_drawingSession.Get(), nullptr, 0, &rect));
        Assert::AreEqual(E_INVALIDARG, InvalidateSourceRectangleForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), 1, &rect));

        f.m_deviceContext->InvalidateEffectInputRectangleMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, UINT32 input, D2D1_RECT_F const* invalidRect)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                Assert::AreEqual(0u, input);
                Assert::AreEqual(rect, FromD2DRect(*invalidRect));

                return S_OK;
            });

        ThrowIfFailed(InvalidateSourceRectangleForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), 0, &rect));
    }

    TEST_METHOD_EX(CanvasEffect_GetInvalidRectangles)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto mockD2DEffect = Make<MockD2DEffectThatCountsCalls>();

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(1, [&](IID const& effectId, ID2D1Effect** effect) { return mockD2DEffect.CopyTo(effect); });

        Rect rect1 = { 1, 2, 3, 4 };
        Rect rect2 = { 5, 6, 7, 8 };

        ComArray<Rect> result;

        Assert::AreEqual(E_INVALIDARG, testEffect->GetInvalidRectangles(nullptr, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetInvalidRectangles(f.m_drawingSession.Get(), nullptr, result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetInvalidRectangles(f.m_drawingSession.Get(), result.GetAddressOfSize(), nullptr));

        f.m_deviceContext->GetEffectInvalidRectangleCountMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, UINT32* count)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                *count = 2;

                return S_OK;
            });

        f.m_deviceContext->GetEffectInvalidRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F* rects, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                Assert::AreEqual(2u, count);

                rects[0] = ToD2DRect(rect1);
                rects[1] = ToD2DRect(rect2);

                return S_OK;
            });

        ThrowIfFailed(testEffect->GetInvalidRectangles(f.m_drawingSession.Get(), result.GetAddressOfSize(), result.GetAddressOfData()));

        Assert::AreEqual(2u, result.GetSize());
     
        Assert::AreEqual(rect1, result[0]);
        Assert::AreEqual(rect2, result[1]);
    }

    TEST_METHOD_EX(CanvasEffect_GetInvalidRectangles_ForICanvasImageInterop)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto mockD2DEffect = Make<MockD2DEffectThatCountsCalls>();

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(1, [&](IID const& effectId, ID2D1Effect** effect) { return mockD2DEffect.CopyTo(effect); });

        Rect rect1 = { 1, 2, 3, 4 };
        Rect rect2 = { 5, 6, 7, 8 };

        ComArray<Rect> result;

        Assert::AreEqual(E_INVALIDARG, GetInvalidRectanglesForICanvasImageInterop(nullptr, As<ICanvasImageInterop>(testEffect.Get()).Get(), result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, GetInvalidRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), nullptr, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, GetInvalidRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), nullptr, result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, GetInvalidRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), result.GetAddressOfSize(), nullptr));

        f.m_deviceContext->GetEffectInvalidRectangleCountMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, UINT32* count)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                *count = 2;

                return S_OK;
            });

        f.m_deviceContext->GetEffectInvalidRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F* rects, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(effect, mockD2DEffect.Get()));
                Assert::AreEqual(2u, count);

                rects[0] = ToD2DRect(rect1);
                rects[1] = ToD2DRect(rect2);

                return S_OK;
            });

        ThrowIfFailed(GetInvalidRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), result.GetAddressOfSize(), result.GetAddressOfData()));

        Assert::AreEqual(2u, result.GetSize());
     
        Assert::AreEqual(rect1, result[0]);
        Assert::AreEqual(rect2, result[1]);
    }

    TEST_METHOD_EX(CanvasEffect_GetRequiredSourceRectangle)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto inputEffect = Make<TestEffect>(m_blurGuid, 0, 1);

        testEffect->SetSource(0, inputEffect.Get());

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(2,
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                return mockEffects.back().CopyTo(effect);
            });

        Rect outputRect = { 1, 2, 3, 4 };
        Rect inputBounds = { 5, 6, 7, 8 };
        Rect expectedResult = { 9, 10, 11, 12 };
        Rect result;

        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangle(nullptr, outputRect, inputEffect.Get(), 0, inputBounds, &result));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangle(f.m_drawingSession.Get(), outputRect, nullptr, 0, inputBounds, &result));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangle(f.m_drawingSession.Get(), outputRect, inputEffect.Get(), 1, inputBounds, &result));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangle(f.m_drawingSession.Get(), outputRect, inputEffect.Get(), 0, inputBounds, nullptr));

        f.m_deviceContext->GetEffectRequiredInputRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F const* output, D2D1_EFFECT_INPUT_DESCRIPTION const* desc, D2D1_RECT_F* result, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(mockEffects[0].Get(), effect));
                Assert::AreEqual(outputRect, FromD2DRect(*output));
                Assert::AreEqual(1u, count);

                Assert::IsTrue(IsSameInstance(mockEffects[1].Get(), desc[0].effect));
                Assert::AreEqual(0u, desc[0].inputIndex);
                Assert::AreEqual(inputBounds, FromD2DRect(desc[0].inputRectangle));

                result[0] = ToD2DRect(expectedResult);

                return S_OK;
            });

        ThrowIfFailed(testEffect->GetRequiredSourceRectangle(f.m_drawingSession.Get(), outputRect, inputEffect.Get(), 0, inputBounds, &result));

        Assert::AreEqual(expectedResult, result);
    }

    TEST_METHOD_EX(CanvasEffect_GetRequiredSourceRectangle_ForICanvasImageInterop)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 1);
        auto inputEffect = Make<TestEffect>(m_blurGuid, 0, 1);

        testEffect->SetSource(0, inputEffect.Get());

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(2,
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
        return mockEffects.back().CopyTo(effect);
            });

        Rect outputRect = { 1, 2, 3, 4 };
        Rect inputBounds = { 5, 6, 7, 8 };
        Rect expectedResult = { 9, 10, 11, 12 };
        Rect result;
        uint32_t inputIndex = 0;
        ICanvasEffect* inputEffects = inputEffect.Get();
        ICanvasEffect* nullEffects = nullptr;
        uint32_t invalidIndex = 1;

        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(nullptr, As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &inputIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), nullptr, &outputRect, 1, &inputEffects, 1, &inputIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, nullptr, 1, &inputIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &nullEffects, 1, &inputIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &invalidIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 0, &inputIndex, 1, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &inputIndex, 0, &inputBounds, 1, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &inputIndex, 1, &inputBounds, 0, &result));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &inputIndex, 1, &inputBounds, 1, nullptr));

        f.m_deviceContext->GetEffectRequiredInputRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F const* output, D2D1_EFFECT_INPUT_DESCRIPTION const* desc, D2D1_RECT_F* result, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(mockEffects[0].Get(), effect));
                Assert::AreEqual(outputRect, FromD2DRect(*output));
                Assert::AreEqual(1u, count);

                Assert::IsTrue(IsSameInstance(mockEffects[1].Get(), desc[0].effect));
                Assert::AreEqual(0u, desc[0].inputIndex);
                Assert::AreEqual(inputBounds, FromD2DRect(desc[0].inputRectangle));

                result[0] = ToD2DRect(expectedResult);

                return S_OK;
            });

        ThrowIfFailed(GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, &inputEffects, 1, &inputIndex, 1, &inputBounds, 1, &result));

        Assert::AreEqual(expectedResult, result);
    }

    TEST_METHOD_EX(CanvasEffect_GetRequiredSourceRectangles)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 2);
        auto inputEffect1 = Make<TestEffect>(m_blurGuid, 0, 1);
        auto inputEffect2 = Make<TestEffect>(m_blurGuid, 0, 1);

        testEffect->SetSource(0, inputEffect1.Get());
        testEffect->SetSource(1, inputEffect2.Get());

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(3,
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                return mockEffects.back().CopyTo(effect);
            });

        Rect outputRect = { 1, 2, 3, 4 };
        Rect inputBounds1 = { 5, 6, 7, 8 };
        Rect inputBounds2 = { 9, 10, 11, 12 };
        Rect expectedResult1 = { 13, 14, 15, 16 };
        Rect expectedResult2 = { 17, 18, 19, 20 };
        
        ICanvasEffect* inputEffects[] = { inputEffect1.Get(), inputEffect2.Get() };
        uint32_t inputIndices[] = { 0, 0 };
        uint32_t badIndices1[] = { 1, 0 };
        uint32_t badIndices2[] = { 0, 1 };
        Rect inputBounds[] = { inputBounds1, inputBounds2 };

        ComArray<Rect> result;

        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(nullptr, outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, nullptr, 2, inputIndices, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, nullptr, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, inputIndices, 2, nullptr, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, nullptr, result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, result.GetAddressOfSize(), nullptr));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 1, inputEffects, 2, inputIndices, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 1, inputIndices, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, inputIndices, 1, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, badIndices1, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));
        Assert::AreEqual(E_INVALIDARG, testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, badIndices2, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));

        f.m_deviceContext->GetEffectRequiredInputRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F const* output, D2D1_EFFECT_INPUT_DESCRIPTION const* desc, D2D1_RECT_F* result, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(mockEffects[0].Get(), effect));
                Assert::AreEqual(outputRect, FromD2DRect(*output));
                Assert::AreEqual(2u, count);

                Assert::IsTrue(IsSameInstance(mockEffects[1].Get(), desc[0].effect));
                Assert::AreEqual(0u, desc[0].inputIndex);
                Assert::AreEqual(inputBounds1, FromD2DRect(desc[0].inputRectangle));

                Assert::IsTrue(IsSameInstance(mockEffects[2].Get(), desc[1].effect));
                Assert::AreEqual(0u, desc[1].inputIndex);
                Assert::AreEqual(inputBounds2, FromD2DRect(desc[1].inputRectangle));

                result[0] = ToD2DRect(expectedResult1);
                result[1] = ToD2DRect(expectedResult2);

                return S_OK;
            });

        ThrowIfFailed(testEffect->GetRequiredSourceRectangles(f.m_drawingSession.Get(), outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, result.GetAddressOfSize(), result.GetAddressOfData()));

        Assert::AreEqual(2u, result.GetSize());
     
        Assert::AreEqual(expectedResult1, result[0]);
        Assert::AreEqual(expectedResult2, result[1]);
    }

    TEST_METHOD_EX(CanvasEffect_GetRequiredSourceRectangles_ForICanvasImageInterop)
    {
        Fixture f;

        auto testEffect = Make<TestEffect>(m_blurGuid, 0, 2);
        auto inputEffect1 = Make<TestEffect>(m_blurGuid, 0, 1);
        auto inputEffect2 = Make<TestEffect>(m_blurGuid, 0, 1);

        testEffect->SetSource(0, inputEffect1.Get());
        testEffect->SetSource(1, inputEffect2.Get());

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> mockEffects;

        f.m_deviceContext->CreateEffectMethod.SetExpectedCalls(3,
            [&](IID const& effectId, ID2D1Effect** effect)
            {
                mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                return mockEffects.back().CopyTo(effect);
            });

        Rect outputRect = { 1, 2, 3, 4 };
        Rect inputBounds1 = { 5, 6, 7, 8 };
        Rect inputBounds2 = { 9, 10, 11, 12 };
        Rect expectedResult1 = { 13, 14, 15, 16 };
        Rect expectedResult2 = { 17, 18, 19, 20 };
        
        ICanvasEffect* inputEffects[] = { inputEffect1.Get(), inputEffect2.Get() };
        uint32_t inputIndices[] = { 0, 0 };
        uint32_t badIndices1[] = { 1, 0 };
        uint32_t badIndices2[] = { 0, 1 };
        Rect inputBounds[] = { inputBounds1, inputBounds2 };

        Rect results[2];

        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(nullptr, As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), nullptr, &outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, nullptr, 2, inputIndices, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, nullptr, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 2, nullptr, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, 0, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, 2, nullptr));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 1, inputEffects, 2, inputIndices, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 1, inputIndices, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 1, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, badIndices1, 2, inputBounds, 2, results));
        Assert::AreEqual(E_INVALIDARG, GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, badIndices2, 2, inputBounds, 2, results));

        f.m_deviceContext->GetEffectRequiredInputRectanglesMethod.SetExpectedCalls(1,
            [&](ID2D1Effect* effect, D2D1_RECT_F const* output, D2D1_EFFECT_INPUT_DESCRIPTION const* desc, D2D1_RECT_F* result, UINT32 count)
            {
                Assert::IsTrue(IsSameInstance(mockEffects[0].Get(), effect));
                Assert::AreEqual(outputRect, FromD2DRect(*output));
                Assert::AreEqual(2u, count);

                Assert::IsTrue(IsSameInstance(mockEffects[1].Get(), desc[0].effect));
                Assert::AreEqual(0u, desc[0].inputIndex);
                Assert::AreEqual(inputBounds1, FromD2DRect(desc[0].inputRectangle));

                Assert::IsTrue(IsSameInstance(mockEffects[2].Get(), desc[1].effect));
                Assert::AreEqual(0u, desc[1].inputIndex);
                Assert::AreEqual(inputBounds2, FromD2DRect(desc[1].inputRectangle));

                result[0] = ToD2DRect(expectedResult1);
                result[1] = ToD2DRect(expectedResult2);

                return S_OK;
            });

        ThrowIfFailed(GetRequiredSourceRectanglesForICanvasImageInterop(f.m_drawingSession.Get(), As<ICanvasImageInterop>(testEffect.Get()).Get(), &outputRect, 2, inputEffects, 2, inputIndices, 2, inputBounds, 2, results));
     
        Assert::AreEqual(expectedResult1, results[0]);
        Assert::AreEqual(expectedResult2, results[1]);
    }

    struct EffectRealizationContextFixture : public Fixture
    {
        ComPtr<TestEffect> m_testEffect;
        ComPtr<CanvasBitmap> m_stubBitmap;

        std::vector<ComPtr<MockD2DEffectThatCountsCalls>> m_mockEffects;

        EffectRealizationContextFixture(int expectedCreateEffectCalls)
        {
            m_testEffect = Make<TestEffect>(CLSID_D2D1GaussianBlur, 0, 1);
            m_stubBitmap = CreateStubCanvasBitmap(DEFAULT_DPI, m_canvasDevice.Get());
            m_testEffect->SetSource(0, m_stubBitmap.Get());

            m_deviceContext->CreateEffectMethod.SetExpectedCalls(expectedCreateEffectCalls,
                [=](IID const& effectId, ID2D1Effect** effect)
                {
                    m_mockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                    return m_mockEffects.back().CopyTo(effect);
                });

            m_deviceContext->InvalidateEffectInputRectangleMethod.SetExpectedCalls(1);
        }
    };

    TEST_METHOD_EX(CanvasEffect_EffectRealizationContext_ResourceCreatorIsDeviceContext)
    {
        EffectRealizationContextFixture f(1);

        ThrowIfFailed(f.m_testEffect->InvalidateSourceRectangle(f.m_drawingSession.Get(), 0, Rect{}));

        Assert::AreEqual<size_t>(1, f.m_mockEffects.size());
        Assert::AreEqual(CLSID_D2D1GaussianBlur, f.m_mockEffects[0]->m_effectId);
    }

    TEST_METHOD_EX(CanvasEffect_EffectRealizationContext_ResourceCreatorIsNotDeviceContext)
    {
        EffectRealizationContextFixture f(1);

        f.m_canvasDevice->GetResourceCreationDeviceContextMethod.SetExpectedCalls(1,
            [&]
            {
                return DeviceContextLease(f.m_deviceContext);
            });

        ThrowIfFailed(f.m_testEffect->InvalidateSourceRectangle(f.m_stubBitmap.Get(), 0, Rect{}));

        Assert::AreEqual<size_t>(1, f.m_mockEffects.size());
        Assert::AreEqual(CLSID_D2D1GaussianBlur, f.m_mockEffects[0]->m_effectId);
    }

    TEST_METHOD_EX(CanvasEffect_EffectRealizationContext_WhenContextIsCommandList_InsertsDpiCompensation)
    {
        EffectRealizationContextFixture f(2);

        auto commandList = Make<MockD2DCommandList>();
        
        f.m_deviceContext->GetTargetMethod.SetExpectedCalls(1,
            [&](ID2D1Image** value)
            {
                commandList.CopyTo(value);
            });

        ThrowIfFailed(f.m_testEffect->InvalidateSourceRectangle(f.m_drawingSession.Get(), 0, Rect{}));

        Assert::AreEqual<size_t>(2, f.m_mockEffects.size());
        Assert::AreEqual(CLSID_D2D1GaussianBlur, f.m_mockEffects[0]->m_effectId);
        Assert::AreEqual(CLSID_D2D1DpiCompensation, f.m_mockEffects[1]->m_effectId);
    }

#if WINVER > _WIN32_WINNT_WINBLUE 

    class D2DFactoryWithOptional5Support : public MockD2DFactory
    {
        bool m_supportFactory5;

    public:
        D2DFactoryWithOptional5Support(bool supportFactory5)
            : m_supportFactory5(supportFactory5)
        { }

        STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject) override
        {
            if (IsEqualGUID(riid, __uuidof(ID2D1Factory5)))
            {
                // Major cheat here! We don't actually bother implementing all of ID2D1Factory5, but
                // instead QI for the base ID2D1Factory interface. This is not even remotely correct
                // COM, but good enough for these tests because we happen to know that the
                // implementation only checks the return pointer for being non-null, then releases it.
                if (m_supportFactory5)
                    return MockD2DFactory::QueryInterface(__uuidof(ID2D1Factory), ppvObject);
                else
                    return E_NOINTERFACE;
            }

            return MockD2DFactory::QueryInterface(riid, ppvObject);
        }
    };

    struct NewEffectsAreSupportedFixture
    {
        std::shared_ptr<TestDeviceAdapter> m_deviceAdapter;

        NewEffectsAreSupportedFixture(bool supportFactory5)
            : m_deviceAdapter(std::make_shared<TestDeviceAdapter>())
        {
            m_deviceAdapter->m_overrideD2DFactory = Make<D2DFactoryWithOptional5Support>(supportFactory5);

            CanvasDeviceAdapter::SetInstance(m_deviceAdapter);
        }
    };

    TEST_METHOD_EX(CanvasEffect_NewEffectConstructorsFailWhenID2D1Factory5NotSupported)
    {
        NewEffectsAreSupportedFixture fixture(false);

        ExpectHResultException(E_NOTIMPL, [] { Make<AlphaMaskEffect>(); });
        ValidateStoredErrorState(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        ExpectHResultException(E_NOTIMPL, [] { Make<CrossFadeEffect>(); });
        ValidateStoredErrorState(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        ExpectHResultException(E_NOTIMPL, [] { Make<OpacityEffect>(); });
        ValidateStoredErrorState(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        ExpectHResultException(E_NOTIMPL, [] { Make<TintEffect>(); });
        ValidateStoredErrorState(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);
    }

    TEST_METHOD_EX(CanvasEffect_NewEffectConstructorsSucceedWhenID2D1Factory5Supported)
    {
        NewEffectsAreSupportedFixture fixture(true);

        CheckMakeResult(Make<AlphaMaskEffect>());
        CheckMakeResult(Make<CrossFadeEffect>());
        CheckMakeResult(Make<OpacityEffect>());
        CheckMakeResult(Make<TintEffect>());
    }

    template<typename TFactory>
    static void TestIsSupported(boolean expected)
    {
        auto factory = Make<TFactory>();

        boolean result;
        ThrowIfFailed(factory->get_IsSupported(&result));

        Assert::AreEqual(expected, result);

        Assert::AreEqual(E_INVALIDARG, factory->get_IsSupported(nullptr));
    }

    TEST_METHOD_EX(CanvasEffect_NewEffectIsSupportedReturnsFalseWhenID2D1Factory5NotSupported)
    {
        NewEffectsAreSupportedFixture fixture(false);

        TestIsSupported<AlphaMaskEffectFactory>(false);
        TestIsSupported<CrossFadeEffectFactory>(false);
        TestIsSupported<OpacityEffectFactory>(false);
        TestIsSupported<TintEffectFactory>(false);
    }

    TEST_METHOD_EX(CanvasEffect_NewEffectIsSupportedReturnsTrueWhenID2D1Factory5Supported)
    {
        NewEffectsAreSupportedFixture fixture(true);

        TestIsSupported<AlphaMaskEffectFactory>(true);
        TestIsSupported<CrossFadeEffectFactory>(true);
        TestIsSupported<OpacityEffectFactory>(true);
        TestIsSupported<TintEffectFactory>(true);
    }

#endif

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
