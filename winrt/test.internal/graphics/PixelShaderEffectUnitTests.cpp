// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/effects/shader/PixelShaderEffect.h>
#include <lib/effects/shader/PixelShaderEffectImpl.h>
#include <lib/effects/shader/PixelShaderTransform.h>
#include <lib/effects/shader/SharedShaderState.h>

#include "mocks/MockD2DDrawInfo.h"
#include "mocks/MockD2DEffectContext.h"
#include "mocks/MockD2DTransformGraph.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


TEST_CLASS(PixelShaderEffectUnitTests)
{
    TEST_METHOD_EX(PixelShaderEffect_UsePropertyMapAfterClose)
    {
        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        auto effect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());

        ComPtr<IMap<HSTRING, IInspectable*>> properties;
        ThrowIfFailed(effect->get_Properties(&properties));

        effect->Close();

        ValidatePropertyMapAccessorsFail(properties.Get());
    }


    TEST_METHOD_EX(PixelShaderEffect_UsePropertyMapAfterFinalRelease)
    {
        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        auto effect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());

        ComPtr<IMap<HSTRING, IInspectable*>> properties;
        ThrowIfFailed(effect->get_Properties(&properties));

        effect.Reset();

        ValidatePropertyMapAccessorsFail(properties.Get());
    }


    static void ValidatePropertyMapAccessorsFail(IMap<HSTRING, IInspectable*>* properties)
    {
        unsigned size;
        boolean b;
        ComPtr<IInspectable> item;
        ComPtr<IIterator<IKeyValuePair<HSTRING, IInspectable*>*>> iterator;

        Assert::AreEqual(RO_E_CLOSED, properties->get_Size(&size));
        Assert::AreEqual(RO_E_CLOSED, properties->HasKey(nullptr, &b));
        Assert::AreEqual(RO_E_CLOSED, properties->Lookup(nullptr, &item));
        Assert::AreEqual(RO_E_CLOSED, properties->Insert(nullptr, nullptr, &b));
        Assert::AreEqual(RO_E_CLOSED, As<IIterable<IKeyValuePair<HSTRING, IInspectable*>*>>(properties)->First(&iterator));
    }


    struct Fixture
    {
        ComPtr<MockD2DFactory> Factory;
        ComPtr<StubD2DDevice> StubDevice;
        ComPtr<StubD2DDeviceContextWithGetFactory> DeviceContext;
        ComPtr<StubCanvasDevice> CanvasDevice;
        ComPtr<MockD2DEffect> MockEffect;

        std::vector<std::vector<BYTE>> EffectPropertyValues;


        Fixture()
        {
            Factory = Make<MockD2DFactory>();
            StubDevice = Make<StubD2DDevice>(Factory.Get());
            DeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
            CanvasDevice = Make<StubCanvasDevice>(StubDevice);
            MockEffect = Make<MockD2DEffect>();

            Factory->MockGetRegisteredEffects = [&](CLSID *effects, UINT32 effectsCount, UINT32 *effectsReturned, UINT32 *effectsRegistered)
            {
                *effectsReturned = *effectsRegistered = 0;
                return S_OK;
            };

            Factory->MockRegisterEffectFromString = [&](REFCLSID classId, PCWSTR propertyXml, CONST D2D1_PROPERTY_BINDING *bindings, UINT32 bindingsCount, CONST PD2D1_EFFECT_FACTORY effectFactory)
            {
                return S_OK;
            };

            MockEffect->MockSetInputCount = [](UINT32 count)
            {
                Assert::AreEqual(0u, count);
                return S_OK;
            };

            MockEffect->MockSetValue = [&](UINT32 index, D2D1_PROPERTY_TYPE type, CONST BYTE* data, UINT32 dataSize)
            {
                if (EffectPropertyValues.size() <= index)
                    EffectPropertyValues.resize(index + 1);

                EffectPropertyValues[index].assign(data, data + dataSize);
                return S_OK;
            };

            DeviceContext->CreateEffectMethod.AllowAnyCall([&](IID const&, ID2D1Effect** effect)
            {
                MockEffect.CopyTo(effect);
                return S_OK;
            });
        }


        template<typename T>
        T const& GetEffectPropertyValue(PixelShaderEffectProperty prop)
        {
            Assert::AreEqual(sizeof(T), EffectPropertyValues[(int)prop].size());

            return *reinterpret_cast<T*>(EffectPropertyValues[(int)prop].data());
        }
    };


    TEST_METHOD_EX(PixelShaderEffect_RealizeSetsD2DProperties)
    {
        Fixture f;

        std::vector<BYTE> constants{ 1, 3, 7 };

        CoordinateMappingState coordinateMapping;
        coordinateMapping.MaxOffset = 123;

        auto sharedState = Make<SharedShaderState>(ShaderDescription(), constants, coordinateMapping);
        auto effect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());

        // Realize the effect.
        effect->GetD2DImage(f.CanvasDevice.Get(), f.DeviceContext.Get(), GetImageFlags::None, 0, nullptr);

        // This should have passed the constant buffer through to D2D.
        Assert::AreEqual(constants, f.EffectPropertyValues[(int)PixelShaderEffectProperty::Constants]);

        // Should have also passed the shared state interface.
        Assert::IsTrue(IsSameInstance(sharedState.Get(), f.GetEffectPropertyValue<ISharedShaderState*>(PixelShaderEffectProperty::SharedState)));

        // And it should have passed the coordinate mapping state.
        Assert::AreEqual(coordinateMapping.MaxOffset, f.GetEffectPropertyValue<CoordinateMappingState>(PixelShaderEffectProperty::CoordinateMapping).MaxOffset);
    }


    TEST_METHOD_EX(PixelShaderEffect_InteropFromD2D)
    {
        Fixture f;

        // Construct a shader description containing one variable named "Hello".
        D3D11_SHADER_VARIABLE_DESC variableDesc = { "Hello" };
        D3D11_SHADER_TYPE_DESC variableType = { D3D_SVC_SCALAR };

        ShaderVariable variable(variableDesc, variableType);

        ShaderDescription desc;
        desc.Variables.push_back(variable);

        auto sharedState = Make<SharedShaderState>(desc, std::vector<BYTE>(), CoordinateMappingState());

        // Teach the D2D effect how to report its CLSID and shared state.
        f.MockEffect->MockGetValue = [&](UINT32 index, D2D1_PROPERTY_TYPE type, BYTE *data, UINT32 dataSize)
        {
            switch (index)
            {
            case D2D1_PROPERTY_CLSID:
                Assert::AreEqual<size_t>(sizeof(IID), dataSize);
                *reinterpret_cast<IID*>(data) = CLSID_PixelShaderEffect;
                break;

            case PixelShaderEffectProperty::SharedState:
                Assert::AreEqual<size_t>(sizeof(IUnknown*), dataSize);
                *reinterpret_cast<IUnknown**>(data) = As<IUnknown>(sharedState).Detach();
                break;

            default:
                Assert::Fail();
            }

            return S_OK;
        };

        // Wrap a Win2D PixelShaderEffect around a D2D custom pixel shader effect instance.
        auto wrapper = ResourceManager::GetOrCreate(f.CanvasDevice.Get(), As<IUnknown>(f.MockEffect).Get(), 0);

        auto effect = As<IPixelShaderEffect>(wrapper);

        // If this worked, the Win2D effect should have picked up shared state ifrom the D2D effect,
        // so it should know about things like what variables the shader constant buffer contains.
        ComPtr<IMap<HSTRING, IInspectable*>> properties;
        ThrowIfFailed(effect->get_Properties(&properties));

        boolean hasKey;

        ThrowIfFailed(properties->HasKey(HStringReference(L"Hello").Get(), &hasKey));
        Assert::IsTrue(!!hasKey);

        ThrowIfFailed(properties->HasKey(HStringReference(L"world").Get(), &hasKey));
        Assert::IsFalse(!!hasKey);
    }


    TEST_METHOD_EX(PixelShaderEffect_PropertyChangesArePassedThroughToD2D)
    {
        Fixture f;

        // Construct a shader description containing one integer variable.
        D3D11_SHADER_VARIABLE_DESC variableDesc = { "foo", 0, sizeof(int) };
        D3D11_SHADER_TYPE_DESC variableType = { D3D_SVC_SCALAR, D3D_SVT_INT, 1, 1 };

        ShaderVariable variable(variableDesc, variableType);

        ShaderDescription desc;
        desc.Variables.push_back(variable);

        auto sharedState = Make<SharedShaderState>(desc, std::vector<BYTE>(sizeof(int)), CoordinateMappingState());
        auto effect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());

        // Change the property value to 3.
        ComPtr<IMap<HSTRING, IInspectable*>> properties;
        ThrowIfFailed(effect->get_Properties(&properties));

        boolean replaced;
        ThrowIfFailed(properties->Insert(HStringReference(L"foo").Get(), Make<Nullable<int>>(3).Get(), &replaced));

        // Realize the effect.
        effect->GetD2DImage(f.CanvasDevice.Get(), f.DeviceContext.Get(), GetImageFlags::None, 0, nullptr);

        // This should have passed the constant buffer containing 3 through to D2D.
        auto& d2dConstants = f.GetEffectPropertyValue<int>(PixelShaderEffectProperty::Constants);
        Assert::AreEqual(3, d2dConstants);

        // Change the property value to 5.
        ThrowIfFailed(properties->Insert(HStringReference(L"foo").Get(), Make<Nullable<int>>(5).Get(), &replaced));

        // This change should immediately be passed along to D2D.
        Assert::AreEqual(5, d2dConstants);
    }


    TEST_METHOD_EX(PixelShaderEffect_CoordinateMappingChangesArePassedThroughToD2D)
    {
        Fixture f;

        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        auto effect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());

        // Change some coordinate mapping settings.
        ThrowIfFailed(effect->put_Source1Mapping(SamplerCoordinateMapping::OneToOne));
        ThrowIfFailed(effect->put_Source2Mapping(SamplerCoordinateMapping::Offset));
        ThrowIfFailed(effect->put_MaxSamplerOffset(23));

        // Realize the effect.
        effect->GetD2DImage(f.CanvasDevice.Get(), f.DeviceContext.Get(), GetImageFlags::None, 0, nullptr);

        // This should have passed the coordinate mapping state through to D2D.
        auto& d2dMapping = f.GetEffectPropertyValue<CoordinateMappingState>(PixelShaderEffectProperty::CoordinateMapping);
        
        Assert::AreEqual(SamplerCoordinateMapping::OneToOne, d2dMapping.Mapping[0]);
        Assert::AreEqual(SamplerCoordinateMapping::Offset, d2dMapping.Mapping[1]);
        Assert::AreEqual(23, d2dMapping.MaxOffset);

        // Change the state.
        ThrowIfFailed(effect->put_Source1Mapping(SamplerCoordinateMapping::Unknown));
        ThrowIfFailed(effect->put_Source2Mapping(SamplerCoordinateMapping::OneToOne));
        ThrowIfFailed(effect->put_MaxSamplerOffset(42));

        // Changes should immediately be passed along to D2D.
        Assert::AreEqual(SamplerCoordinateMapping::Unknown, d2dMapping.Mapping[0]);
        Assert::AreEqual(SamplerCoordinateMapping::OneToOne, d2dMapping.Mapping[1]);
        Assert::AreEqual(42, d2dMapping.MaxOffset);
    }
};


TEST_CLASS(PixelShaderEffectImplUnitTests)
{
public:
    struct Fixture
    {
        ComPtr<MockD2DFactory> Factory;
        ComPtr<MockD2DEffectContext> MockEffectContext;
        ComPtr<MockD2DTransformGraph> MockTransformGraph;

        std::vector<IID> RegisteredEffects;
        std::vector<D2D1_PROPERTY_BINDING> Bindings;


        Fixture()
        {
            Factory = Make<MockD2DFactory>();
            MockEffectContext = Make<MockD2DEffectContext>();
            MockTransformGraph = Make<MockD2DTransformGraph>();

            Factory->MockGetRegisteredEffects = [&](CLSID *effects, UINT32 effectsCount, UINT32 *effectsReturned, UINT32 *effectsRegistered)
            {
                *effectsRegistered = static_cast<unsigned>(RegisteredEffects.size());
                *effectsReturned = std::min(*effectsRegistered, effectsCount);

                for (unsigned i = 0; i < *effectsReturned; i++)
                {
                    effects[i] = RegisteredEffects[i];
                }

                return S_OK;
            };

            Factory->MockRegisterEffectFromString = [&](REFCLSID classId, PCWSTR propertyXml, CONST D2D1_PROPERTY_BINDING *bindings, UINT32 bindingsCount, CONST PD2D1_EFFECT_FACTORY effectFactory)
            {
                RegisteredEffects.push_back(classId);
                Bindings.assign(bindings, bindings + bindingsCount);
                return S_OK;
            };
        }


        D2D1_PROPERTY_BINDING const& FindBinding(wchar_t const* name)
        {
            auto it = std::find_if(Bindings.begin(), Bindings.end(), [=](D2D1_PROPERTY_BINDING const& binding)
            {
                return !wcscmp(binding.propertyName, name);
            });

            Assert::IsTrue(it != Bindings.end());

            return *it;
        }
    };


    TEST_METHOD_EX(PixelShaderEffectImpl_Register)
    {
        Fixture f;

        // Registering our effect type should add it to the registered effects list.
        PixelShaderEffectImpl::Register(f.Factory.Get());

        Assert::AreEqual<size_t>(1, f.RegisteredEffects.size());
        Assert::AreEqual(CLSID_PixelShaderEffect, f.RegisteredEffects[0]);

        // Registering a second time should not re-add the same effect.
        PixelShaderEffectImpl::Register(f.Factory.Get());

        Assert::AreEqual<size_t>(1, f.RegisteredEffects.size());
    }


    TEST_METHOD_EX(PixelShaderEffectImpl_PrepareForRender_TransformGraphPassedToInitialize)
    {
        Fixture f;
        PixelShaderEffectImpl::Register(f.Factory.Get());

        auto impl = Make<PixelShaderEffectImpl>();

        ThrowIfFailed(impl->Initialize(f.MockEffectContext.Get(), f.MockTransformGraph.Get()));

        TestPrepareForRender(f, impl.Get());
    }


    TEST_METHOD_EX(PixelShaderEffectImpl_PrepareForRender_TransformGraphChangedViaSetGraph)
    {
        Fixture f;
        PixelShaderEffectImpl::Register(f.Factory.Get());

        auto impl = Make<PixelShaderEffectImpl>();

        auto someOtherTransformGraph = Make<MockD2DTransformGraph>();

        ThrowIfFailed(impl->Initialize(f.MockEffectContext.Get(), someOtherTransformGraph.Get()));
        ThrowIfFailed(impl->SetGraph(f.MockTransformGraph.Get()));

        TestPrepareForRender(f, impl.Get());
     
        // Can't change the graph after the transform node is created.
        Assert::AreEqual(E_FAIL, impl->SetGraph(someOtherTransformGraph.Get()));
    }


    static void TestPrepareForRender(Fixture& f, PixelShaderEffectImpl* impl)
    {
        // PrepareForRender without setting the shared state first should fail.
        Assert::AreEqual(E_FAIL, impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));

        // Set the shared state property.
        ShaderDescription desc;
        desc.Code = { 2, 5, 7, 9, 4 };
        desc.Hash = IID{ 0x8495c8be, 0xd63e, 0x40a0, 0x9f, 0xa5, 0x9, 0x72, 0x4b, 0xaf, 0xf7, 0x15 };
        desc.InputCount = 7;

        auto sharedState = Make<SharedShaderState>(desc, std::vector<BYTE>(), CoordinateMappingState());

        ThrowIfFailed(f.FindBinding(L"SharedState").setFunction(impl, reinterpret_cast<BYTE*>(sharedState.GetAddressOf()), sizeof(ISharedShaderState*)));

        // The first PrepareForRender should call LoadPixelShader and SetSingleTransformNode.
        f.MockEffectContext->LoadPixelShaderMethod.SetExpectedCalls(1, [&](IID const& shaderId, BYTE const* shaderBuffer, unsigned shaderBufferCount)
        {
            Assert::AreEqual(desc.Hash, shaderId);
            Assert::AreEqual<size_t>(desc.Code.size(), shaderBufferCount);

            for (unsigned i = 0; i < shaderBufferCount; i++)
            {
                Assert::AreEqual(desc.Code[i], shaderBuffer[i]);
            }

            return S_OK;
        });

        f.MockTransformGraph->SetSingleTransformNodeMethod.SetExpectedCalls(1, [&](ID2D1TransformNode* node)
        {
            Assert::AreEqual(desc.InputCount, node->GetInputCount());
            return S_OK;
        });

        ThrowIfFailed(impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));

        Expectations::Instance()->Validate();

        // Subsequent PrepareForRender calls should not reset the pixel shader.
        ThrowIfFailed(impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));
    }


    TEST_METHOD_EX(PixelShaderEffectImpl_SharedState)
    {
        Fixture f;
        PixelShaderEffectImpl::Register(f.Factory.Get());
        auto& binding = f.FindBinding(L"SharedState");

        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        auto impl = Make<PixelShaderEffectImpl>();

        // SharedState property is initially null.
        ComPtr<IUnknown> value;
        unsigned valueSize;
        ThrowIfFailed(binding.getFunction(impl.Get(), reinterpret_cast<BYTE*>(value.GetAddressOf()), sizeof(IUnknown*), &valueSize));
        Assert::IsNull(value.Get());
        Assert::AreEqual<size_t>(sizeof(IUnknown*), valueSize);

        // Can't set it to null.
        void* nullPointer = nullptr;
        Assert::AreEqual(E_INVALIDARG, binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(&nullPointer), sizeof(nullPointer)));

        // Can't set it to the wrong interface.
        Assert::AreEqual(E_NOINTERFACE, binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(impl.GetAddressOf()), sizeof(PixelShaderEffectImpl*)));

        // Set the property.
        ThrowIfFailed(binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(sharedState.GetAddressOf()), sizeof(ISharedShaderState*)));

        // Read it back.
        ThrowIfFailed(binding.getFunction(impl.Get(), reinterpret_cast<BYTE*>(value.GetAddressOf()), sizeof(IUnknown*), &valueSize));
        Assert::IsTrue(IsSameInstance(value.Get(), sharedState.Get()));
        Assert::AreEqual<size_t>(sizeof(IUnknown*), valueSize);

        // Can't set it a second time.
        Assert::AreEqual(E_FAIL, binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(sharedState.GetAddressOf()), sizeof(PixelShaderEffectImpl*)));
    }


    TEST_METHOD_EX(PixelShaderEffectImpl_Constants)
    {
        Fixture f;
        PixelShaderEffectImpl::Register(f.Factory.Get());
        auto& binding = f.FindBinding(L"Constants");

        auto impl = Make<PixelShaderEffectImpl>();

        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        ThrowIfFailed(f.FindBinding(L"SharedState").setFunction(impl.Get(), reinterpret_cast<BYTE*>(sharedState.GetAddressOf()), sizeof(ISharedShaderState*)));

        ThrowIfFailed(impl->Initialize(f.MockEffectContext.Get(), f.MockTransformGraph.Get()));

        // Constants property is initially empty.
        unsigned valueSize;
        ThrowIfFailed(binding.getFunction(impl.Get(), nullptr, 0, &valueSize));
        Assert::AreEqual<size_t>(0, valueSize);

        // Set some data.
        std::vector<BYTE> constants = { 3, 6, 8 };
        ThrowIfFailed(binding.setFunction(impl.Get(), constants.data(), static_cast<unsigned>(constants.size())));

        // Read it back.
        std::vector<BYTE> result(3);
        ThrowIfFailed(binding.getFunction(impl.Get(), result.data(), static_cast<unsigned>(result.size()), &valueSize));
        Assert::AreEqual(3u, valueSize);
        Assert::AreEqual(constants, result);

        // Read with too small a buffer.
        result.assign(1, 0);
        ThrowIfFailed(binding.getFunction(impl.Get(), result.data(), static_cast<unsigned>(result.size()), &valueSize));
        Assert::AreEqual(3u, valueSize);
        Assert::AreEqual(std::vector<byte>({ constants[0] }), result);

        // Read with too large a buffer.
        result.assign(4, 0);
        ThrowIfFailed(binding.getFunction(impl.Get(), result.data(), static_cast<unsigned>(result.size()), &valueSize));
        Assert::AreEqual(3u, valueSize);
        Assert::AreEqual(std::vector<byte>({ constants[0], constants[1], constants[2], 0 }), result);

        // PrepareForRender should pass the constant buffer through to SetPixelShaderConstantBuffer.
        auto mockDrawInfo = Make<MockD2DDrawInfo>();

        f.MockEffectContext->LoadPixelShaderMethod.SetExpectedCalls(1);

        f.MockTransformGraph->SetSingleTransformNodeMethod.SetExpectedCalls(1, [&](ID2D1TransformNode* node)
        {
            ThrowIfFailed(As<ID2D1DrawTransform>(node)->SetDrawInfo(mockDrawInfo.Get()));
            return S_OK;
        });

        mockDrawInfo->SetPixelShaderMethod.SetExpectedCalls(1);
        mockDrawInfo->SetInstructionCountHintMethod.SetExpectedCalls(1);

        auto validateConstants = [&](BYTE const* buffer, unsigned bufferCount)
        {
            Assert::AreEqual<size_t>(constants.size(), bufferCount);

            for (unsigned i = 0; i < bufferCount; i++)
            {
                Assert::AreEqual(constants[i], buffer[i]);
            }

            return S_OK;
        };

        mockDrawInfo->SetPixelShaderConstantBufferMethod.SetExpectedCalls(1, validateConstants);

        ThrowIfFailed(impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));

        Expectations::Instance()->Validate();

        // Draw a second time without changing constants should not set anything.
        ThrowIfFailed(impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));

        // Change constants, then draw again, should set the new value.
        constants = { 7, 2 };
        ThrowIfFailed(binding.setFunction(impl.Get(), constants.data(), static_cast<unsigned>(constants.size())));

        mockDrawInfo->SetPixelShaderConstantBufferMethod.SetExpectedCalls(1, validateConstants);

        ThrowIfFailed(impl->PrepareForRender(D2D1_CHANGE_TYPE_NONE));
    }


    TEST_METHOD_EX(PixelShaderEffectImpl_CoordinateMapping)
    {
        Fixture f;
        PixelShaderEffectImpl::Register(f.Factory.Get());
        auto& binding = f.FindBinding(L"CoordinateMapping");

        auto sharedState = Make<SharedShaderState>(ShaderDescription(), std::vector<BYTE>(), CoordinateMappingState());
        auto impl = Make<PixelShaderEffectImpl>();

        // Can query the size of the property.
        unsigned valueSize;
        ThrowIfFailed(binding.getFunction(impl.Get(), nullptr, 0, &valueSize));
        Assert::AreEqual<size_t>(sizeof(CoordinateMappingState), valueSize);

        // Can read the value of the property.
        CoordinateMappingState value;
        ThrowIfFailed(binding.getFunction(impl.Get(), reinterpret_cast<BYTE*>(&value), sizeof(CoordinateMappingState), &valueSize));
        Assert::AreEqual<size_t>(sizeof(CoordinateMappingState), valueSize);

        for (int i = 0; i < MaxShaderInputs; i++)
        {
            Assert::AreEqual(SamplerCoordinateMapping::Unknown, value.Mapping[i]);
        }

        Assert::AreEqual(0, value.MaxOffset);

        // Can't read if we specify the wrong size.
        Assert::AreEqual(E_NOT_SUFFICIENT_BUFFER, binding.getFunction(impl.Get(), reinterpret_cast<BYTE*>(&value), sizeof(CoordinateMappingState) - 1, &valueSize));

        // Can't set it to the wrong size.
        Assert::AreEqual(E_NOT_SUFFICIENT_BUFFER, binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(&value), sizeof(CoordinateMappingState) - 1));

        // Set the property.
        value.Mapping[0] = SamplerCoordinateMapping::Offset;
        value.Mapping[1] = SamplerCoordinateMapping::OneToOne;
        value.MaxOffset = 123;

        ThrowIfFailed(binding.setFunction(impl.Get(), reinterpret_cast<BYTE*>(&value), sizeof(CoordinateMappingState)));

        // Read it back.
        CoordinateMappingState value2;
        ThrowIfFailed(binding.getFunction(impl.Get(), reinterpret_cast<BYTE*>(&value2), sizeof(CoordinateMappingState), &valueSize));
        Assert::AreEqual<size_t>(sizeof(CoordinateMappingState), valueSize);

        for (int i = 0; i < MaxShaderInputs; i++)
        {
            Assert::AreEqual(value.Mapping[i], value2.Mapping[i]);
        }

        Assert::AreEqual(value.MaxOffset, value2.MaxOffset);
    }
};


TEST_CLASS(PixelShaderTransformUnitTests)
{
public:
    TEST_METHOD_EX(PixelShaderTransform_GetInputCount)
    {
        ShaderDescription desc;
        desc.InputCount = 5;

        auto sharedState = Make<SharedShaderState>(desc, std::vector<BYTE>(), CoordinateMappingState());
        auto transform = Make<PixelShaderTransform>(sharedState.Get(), std::make_shared<CoordinateMappingState>());

        Assert::AreEqual(5u, transform->GetInputCount());
    }


    TEST_METHOD_EX(PixelShaderTransform_SetDrawInfo)
    {
        ShaderDescription desc;
        desc.InstructionCount = 123;
        desc.Hash = IID{ 0x8495c8be, 0xd63e, 0x40a0, 0x9f, 0xa5, 0x9, 0x72, 0x4b, 0xaf, 0xf7, 0x15 };

        auto sharedState = Make<SharedShaderState>(desc, std::vector<BYTE>(), CoordinateMappingState());
        auto transform = Make<PixelShaderTransform>(sharedState.Get(), std::make_shared<CoordinateMappingState>());

        auto mockDrawInfo = Make<MockD2DDrawInfo>();

        // SetDrawInfo should call SetPixelShader and SetInstructionCountHint.
        mockDrawInfo->SetPixelShaderMethod.SetExpectedCalls(1, [&](IID const& iid, D2D1_PIXEL_OPTIONS options)
        {
            Assert::AreEqual(desc.Hash, iid);
            Assert::AreEqual<int>(D2D1_PIXEL_OPTIONS_NONE, options);
            return S_OK;
        });

        mockDrawInfo->SetInstructionCountHintMethod.SetExpectedCalls(1, [&](UINT32 instructionCount)
        {
            Assert::AreEqual(desc.InstructionCount, instructionCount);
        });

        ThrowIfFailed(transform->SetDrawInfo(mockDrawInfo.Get()));

        Expectations::Instance()->Validate();

        // Set Constants should call SetPixelShaderConstantBuffer.
        std::vector<BYTE> constants = { 1, 23, 42 };

        mockDrawInfo->SetPixelShaderConstantBufferMethod.SetExpectedCalls(1, [&](BYTE const* buffer, UINT32 bufferCount)
        {
            Assert::AreEqual<size_t>(constants.size(), bufferCount);

            for (unsigned i = 0; i < bufferCount; i++)
            {
                Assert::AreEqual(constants[i], buffer[i]);
            }

            return S_OK;
        });

        transform->SetConstants(constants);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenMappingIsOneToOne_ReturnPassThrough)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::OneToOne;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        D2D1_RECT_L input = { 1, 2, 3, 4 };
        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(&input, nullptr, 1, &output, &outputOpaqueSubRect));

        Assert::AreEqual(input, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenMappingIsOffset_ReturnsExpandedRect)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::Offset;
        mapping->MaxOffset = 5;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        D2D1_RECT_L input = { 1, 2, 3, 4 };
        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(&input, nullptr, 1, &output, &outputOpaqueSubRect));

        Assert::AreEqual(D2D1_RECT_L{ -4, -3, 8, 9 }, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenMappingIsUnknown_ReturnsInfinite)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::Unknown;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        D2D1_RECT_L input = { 1, 2, 3, 4 };
        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(&input, nullptr, 1, &output, &outputOpaqueSubRect));

        Assert::AreEqual(D2D1_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX }, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenMultipleInputs_ReturnsUnion)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::OneToOne;
        mapping->Mapping[1] = SamplerCoordinateMapping::Offset;
        mapping->MaxOffset = 1;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        D2D1_RECT_L inputs[] =
        {
            { 50, 200, 79, 210 },
            { -3, 210, 50, 220 },
        };

        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(inputs, nullptr, _countof(inputs), &output, &outputOpaqueSubRect));

        Assert::AreEqual(D2D1_RECT_L{ -4, 200, 79, 221 }, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenMultipleInputs_IgnoresUnknownMappings)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::OneToOne;
        mapping->Mapping[1] = SamplerCoordinateMapping::Unknown;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        D2D1_RECT_L inputs[] =
        {
            { 50, 200, 79, 210 },
            { -3, 210, 50, 220 },
        };

        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(inputs, nullptr, _countof(inputs), &output, &outputOpaqueSubRect));

        Assert::AreEqual(inputs[0], output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInputRectsToOutputRect_WhenNoInputs_ReturnsInfinite)
    {
        auto transform = Make<PixelShaderTransform>(nullptr, std::make_shared<CoordinateMappingState>());

        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(nullptr, nullptr, 0, &output, &outputOpaqueSubRect));

        Assert::AreEqual(D2D1_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX }, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapOutputRectToInputRects)
    {
        auto mapping = std::make_shared<CoordinateMappingState>();

        mapping->Mapping[0] = SamplerCoordinateMapping::Unknown;
        mapping->Mapping[1] = SamplerCoordinateMapping::OneToOne;
        mapping->Mapping[2] = SamplerCoordinateMapping::Offset;
        mapping->MaxOffset = 2;

        auto transform = Make<PixelShaderTransform>(nullptr, mapping);

        // Store input image bounds.
        D2D1_RECT_L initialInputs[] =
        {
            { 1, 2, 3, 4 },
            { 5, 6, 7, 8 },
            { 9, 10, 11, 12 },
        };

        D2D1_RECT_L output;
        D2D1_RECT_L outputOpaqueSubRect;

        ThrowIfFailed(transform->MapInputRectsToOutputRect(initialInputs, nullptr, _countof(initialInputs), &output, &outputOpaqueSubRect));

        Assert::AreEqual(D2D1_RECT_L{ 5, 6, 13, 14 }, output);
        Assert::AreEqual(D2D1_RECT_L{ 0, 0, 0, 0 }, outputOpaqueSubRect);

        // Map backward from an output region being drawn to parts of our input images.
        output = { 10, 11, 23, 42 };
        D2D1_RECT_L inputs[3];

        ThrowIfFailed(transform->MapOutputRectToInputRects(&output, inputs, 3));

        // Unknown mapping = return the entire input image bounds.
        Assert::AreEqual(D2D1_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX }, inputs[0]);

        // OneToOne mapping.
        Assert::AreEqual(output, inputs[1]);

        // Offset mapping.
        Assert::AreEqual(D2D1_RECT_L{ 8, 9, 25, 44 }, inputs[2]);
    }


    TEST_METHOD_EX(PixelShaderTransform_MapInvalidRect_ReturnsInfinity)
    {
        auto transform = Make<PixelShaderTransform>(nullptr, std::make_shared<CoordinateMappingState>());

        D2D1_RECT_L input = { 1, 2, 3, 4 };
        D2D1_RECT_L output;
        D2D1_RECT_L expected = { INT_MIN, INT_MIN, INT_MAX, INT_MAX };

        ThrowIfFailed(transform->MapInvalidRect(1234, input, &output));

        Assert::AreEqual(expected, output);
    }
};


// The following tests only do minimal validation across the different shader property types,
// numbers of inputs, default value initialization, and property accessor methods.
// Full validation for these things occurs in test.managed: EffectTests.cs.
// This test only exists to check a handful of internal things the managed tests can't access.

static std::vector<BYTE> compiledShader1;
static std::vector<BYTE> compiledShader2;


TEST_CLASS(SharedShaderStateUnitTests)
{
    TEST_CLASS_INITIALIZE(Initialize)
    {
#define HLSL(quote) #quote

        static char const* shader1 = HLSL
        (
            cbuffer constants : register(b0)
            {
                float f                    : packoffset(c0.x);
                int i                      : packoffset(c0.y);
                bool b                     : packoffset(c0.z);
                row_major float4x4 rows    : packoffset(c1);
                column_major float4x4 cols : packoffset(c5);
                row_major int2x4 irows     : packoffset(c9);
                column_major int2x4 icols  : packoffset(c11);
            };

            float4 main() : SV_Target
            {
                return rows._11 * cols._11 * f * i * b;
            }
        );

        static char const* shader2 = HLSL
        (
            float4 main() : SV_Target
            {
                return 0;
            }
        );

        compiledShader1 = CompileShader(shader1);
        compiledShader2 = CompileShader(shader2);
    }


    static std::vector<BYTE> CompileShader(char const* shaderCode)
    {
        ComPtr<ID3DBlob> result;

        ThrowIfFailed(D3DCompile(shaderCode, strlen(shaderCode), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &result, nullptr));

        auto buffer = reinterpret_cast<BYTE*>(result->GetBufferPointer());

        return std::vector<BYTE>(buffer, buffer + result->GetBufferSize());
    }


    TEST_METHOD_EX(SharedShaderState_Clone)
    {
        ShaderDescription desc;
        desc.InputCount = 73;

        std::vector<BYTE> constants = { 29 };

        CoordinateMappingState coordinateMapping;
        coordinateMapping.MaxOffset = 23;

        auto originalState = Make<SharedShaderState>(desc, constants, coordinateMapping);

        Assert::AreEqual(desc.InputCount, originalState->Shader().InputCount);
        Assert::AreEqual<size_t>(1, originalState->Constants().size());
        Assert::AreEqual(constants[0], originalState->Constants()[0]);
        Assert::AreEqual(coordinateMapping.MaxOffset, originalState->CoordinateMapping().MaxOffset);

        auto clone = originalState->Clone();

        Assert::AreEqual(desc.InputCount, clone->Shader().InputCount);
        Assert::AreEqual<size_t>(1, clone->Constants().size());
        Assert::AreEqual(constants[0], clone->Constants()[0]);
        Assert::AreEqual(coordinateMapping.MaxOffset, clone->CoordinateMapping().MaxOffset);

        Assert::AreNotEqual<void const*>(&originalState->Shader(), &clone->Shader());
        Assert::AreNotEqual<void const*>(&originalState->Constants(), &clone->Constants());
        Assert::AreNotEqual<void const*>(&originalState->CoordinateMapping(), &clone->CoordinateMapping());
    };


    TEST_METHOD_EX(SharedShaderState_Hashing)
    {
        auto state1a = Make<SharedShaderState>(compiledShader1.data(), static_cast<unsigned>(compiledShader1.size()));
        auto state1b = Make<SharedShaderState>(compiledShader1.data(), static_cast<unsigned>(compiledShader1.size()));
        
        auto state2a = Make<SharedShaderState>(compiledShader2.data(), static_cast<unsigned>(compiledShader2.size()));
        auto state2b = Make<SharedShaderState>(compiledShader2.data(), static_cast<unsigned>(compiledShader2.size()));

        Assert::AreEqual(state1a->Shader().Hash, state1b->Shader().Hash);
        Assert::AreEqual(state2a->Shader().Hash, state2b->Shader().Hash);

        Assert::AreNotEqual(state1a->Shader().Hash, state2a->Shader().Hash);
    };


    TEST_METHOD_EX(SharedShaderState_ShaderReflection)
    {
        auto state = Make<SharedShaderState>(compiledShader1.data(), static_cast<unsigned>(compiledShader1.size()));

        Assert::AreEqual(compiledShader1, state->Shader().Code);
        Assert::AreEqual(0u, state->Shader().InputCount);
        Assert::IsTrue(state->Shader().InstructionCount > 3 && state->Shader().InstructionCount < 20);
        Assert::AreEqual<int>(D3D_FEATURE_LEVEL_10_0, state->Shader().MinFeatureLevel);

        auto& variables = state->Shader().Variables;

        Assert::AreEqual<size_t>(7, variables.size());

        // The variable list is sorted, so indices are alphabetical.
        ValidateVariable(variables[2], L"f",     D3D_SVC_SCALAR,         D3D_SVT_FLOAT, 1, 1, 0, 4,  0);
        ValidateVariable(variables[3], L"i",     D3D_SVC_SCALAR,         D3D_SVT_INT,   1, 1, 0, 4,  4);
        ValidateVariable(variables[0], L"b",     D3D_SVC_SCALAR,         D3D_SVT_BOOL,  1, 1, 0, 4,  8);
        ValidateVariable(variables[6], L"rows",  D3D_SVC_MATRIX_ROWS,    D3D_SVT_FLOAT, 4, 4, 0, 64, 16);
        ValidateVariable(variables[1], L"cols",  D3D_SVC_MATRIX_COLUMNS, D3D_SVT_FLOAT, 4, 4, 0, 64, 80);
        ValidateVariable(variables[5], L"irows", D3D_SVC_MATRIX_ROWS,    D3D_SVT_INT,   2, 4, 0, 32, 144);
        ValidateVariable(variables[4], L"icols", D3D_SVC_MATRIX_COLUMNS, D3D_SVT_INT,   2, 4, 0, 56, 176);
    };


    static void ValidateVariable(ShaderVariable const& variable, 
                                 std::wstring expectedName,
                                 D3D_SHADER_VARIABLE_CLASS expectedClass, 
                                 D3D_SHADER_VARIABLE_TYPE expectedType,
                                 unsigned expectedRows, 
                                 unsigned expectedColumns, 
                                 unsigned expectedElements,
                                 unsigned expectedSize, 
                                 unsigned expectedOffset)
    {
        Assert::AreEqual<std::wstring>(expectedName, static_cast<wchar_t const*>(variable.Name));
        Assert::AreEqual<int>(expectedClass, variable.Class);
        Assert::AreEqual<int>(expectedType, variable.Type);
        Assert::AreEqual(expectedRows, variable.Rows);
        Assert::AreEqual(expectedColumns, variable.Columns);
        Assert::AreEqual(expectedElements, variable.Elements);
        Assert::AreEqual(expectedSize, variable.Size);
        Assert::AreEqual(expectedOffset, variable.Offset);
    }


    TEST_METHOD_EX(SharedShaderState_ConstantBufferLayout)
    {
        // Layout of this struct matches the compiledShader1 constant buffer.
        struct ConstantBuffer
        {
            float f;
            int i;
            int b;
            int _pad;

            float rows[16];
            float cols[16];

            int irows[8];
            int icols[14];

            int _pad2[2];
        };

        auto state = Make<SharedShaderState>(compiledShader1.data(), static_cast<unsigned>(compiledShader1.size()));

        Assert::AreEqual(sizeof(ConstantBuffer), state->Constants().size());

        auto constants = reinterpret_cast<ConstantBuffer const*>(state->Constants().data());

        // Float property.
        state->SetProperty(HStringReference(L"f").Get(), Make<Nullable<float>>(1.0f).Get());
        Assert::AreEqual(1.0f, constants->f);

        state->SetProperty(HStringReference(L"f").Get(), Make<Nullable<float>>(2.0f).Get());
        Assert::AreEqual(2.0f, constants->f);

        // Int property.
        state->SetProperty(HStringReference(L"i").Get(), Make<Nullable<int>>(1).Get());
        Assert::AreEqual(1, constants->i);

        state->SetProperty(HStringReference(L"i").Get(), Make<Nullable<int>>(2).Get());
        Assert::AreEqual(2, constants->i);

        // Bool property.
        state->SetProperty(HStringReference(L"b").Get(), Make<Nullable<bool>>(true).Get());
        Assert::AreEqual(1, constants->b);

        state->SetProperty(HStringReference(L"b").Get(), Make<Nullable<bool>>(false).Get());
        Assert::AreEqual(0, constants->b);

        // Row matrix property.
        Matrix4x4 floatMatrix = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

        state->SetProperty(HStringReference(L"rows").Get(), Make<Nullable<Matrix4x4>>(floatMatrix).Get());
        
        Assert::AreEqual(1.0f, constants->rows[0]);
        Assert::AreEqual(2.0f, constants->rows[1]);
        Assert::AreEqual(3.0f, constants->rows[2]);
        Assert::AreEqual(4.0f, constants->rows[3]);

        Assert::AreEqual(5.0f, constants->rows[4]);
        Assert::AreEqual(6.0f, constants->rows[5]);
        Assert::AreEqual(7.0f, constants->rows[6]);
        Assert::AreEqual(8.0f, constants->rows[7]);

        Assert::AreEqual(9.0f, constants->rows[8]);
        Assert::AreEqual(10.0f, constants->rows[9]);
        Assert::AreEqual(11.0f, constants->rows[10]);
        Assert::AreEqual(12.0f, constants->rows[11]);

        Assert::AreEqual(13.0f, constants->rows[12]);
        Assert::AreEqual(14.0f, constants->rows[13]);
        Assert::AreEqual(15.0f, constants->rows[14]);
        Assert::AreEqual(16.0f, constants->rows[15]);

        // Column matrix property.
        state->SetProperty(HStringReference(L"cols").Get(), Make<Nullable<Matrix4x4>>(floatMatrix).Get());

        Assert::AreEqual(1.0f, constants->cols[0]);
        Assert::AreEqual(5.0f, constants->cols[1]);
        Assert::AreEqual(9.0f, constants->cols[2]);
        Assert::AreEqual(13.0f, constants->cols[3]);

        Assert::AreEqual(2.0f, constants->cols[4]);
        Assert::AreEqual(6.0f, constants->cols[5]);
        Assert::AreEqual(10.0f, constants->cols[6]);
        Assert::AreEqual(14.0f, constants->cols[7]);

        Assert::AreEqual(3.0f, constants->cols[8]);
        Assert::AreEqual(7.0f, constants->cols[9]);
        Assert::AreEqual(11.0f, constants->cols[10]);
        Assert::AreEqual(15.0f, constants->cols[11]);

        Assert::AreEqual(4.0f, constants->cols[12]);
        Assert::AreEqual(8.0f, constants->cols[13]);
        Assert::AreEqual(12.0f, constants->cols[14]);
        Assert::AreEqual(16.0f, constants->cols[15]);

        // Integer 2x4 row matrix.
        std::vector<int> intValues = { 1, 2, 3, 4, 5, 6, 7, 8 };

        state->SetProperty(HStringReference(L"irows").Get(), Make<ReferenceArray<int>>(intValues).Get());

        Assert::AreEqual(1, constants->irows[0]);
        Assert::AreEqual(2, constants->irows[1]);
        Assert::AreEqual(3, constants->irows[2]);
        Assert::AreEqual(4, constants->irows[3]);

        Assert::AreEqual(5, constants->irows[4]);
        Assert::AreEqual(6, constants->irows[5]);
        Assert::AreEqual(7, constants->irows[6]);
        Assert::AreEqual(8, constants->irows[7]);

        // Integer 2x4 column matrix.
        state->SetProperty(HStringReference(L"icols").Get(), Make<ReferenceArray<int>>(intValues).Get());

        Assert::AreEqual(1, constants->icols[0]);
        Assert::AreEqual(5, constants->icols[1]);
        Assert::AreEqual(0, constants->icols[2]);
        Assert::AreEqual(0, constants->icols[3]);

        Assert::AreEqual(2, constants->icols[4]);
        Assert::AreEqual(6, constants->icols[5]);
        Assert::AreEqual(0, constants->icols[6]);
        Assert::AreEqual(0, constants->icols[7]);

        Assert::AreEqual(3, constants->icols[8]);
        Assert::AreEqual(7, constants->icols[9]);
        Assert::AreEqual(0, constants->icols[10]);
        Assert::AreEqual(0, constants->icols[11]);

        Assert::AreEqual(4, constants->icols[12]);
        Assert::AreEqual(8, constants->icols[13]);
    };
};
