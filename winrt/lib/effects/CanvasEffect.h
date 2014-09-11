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

#pragma once

#include <CanvasImage.h>

#include "ClosablePtr.h"
#include "ResourceManager.h"
#include "Vector.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Microsoft::Graphics::Canvas::Effects;
    using namespace ::collections;

    class CanvasEffect 
        : public Implements<
            RuntimeClassFlags<WinRtClassicComMix>,
            IEffect,
            IEffectInput,
            ICanvasImageInternal,
            ICanvasImage,
            ABI::Windows::Foundation::IClosable>
    {

    private:
        ComPtr<ID2D1Effect> m_resource;

        IID m_effectId;

        ComPtr<Vector<IPropertyValue*>> m_properties;
        ComPtr<Vector<IEffectInput*>> m_inputs;

        ComPtr<IPropertyValueStatics> m_propertyValueFactory;

        ComPtr<ID2D1DeviceContext> m_previousDeviceContext;

    public:
        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // IEffect
        //

        IFACEMETHOD(get_EffectId)(GUID* effectId) override;

        IFACEMETHOD(get_Inputs)(_Out_ IVector<IEffectInput*>** inputs) override;

        IFACEMETHOD(get_Properties)(_Out_ IVector<IPropertyValue*>** properties) override;

        //
        // ICanvasImageInternal
        //

        ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext);

    protected:
        // for effects with unknown number of inputs, inputs Size have to be set as zero
        CanvasEffect(IID m_effectId, unsigned int propertiesSize, unsigned int inputSize, bool isInputSizeFixed);

        void GetInput(unsigned int index, IEffectInput** input);

        void SetInput(unsigned int index, IEffectInput* input);

        template<typename T>
        void SetProperty(unsigned int index, T value, bool isDefault = false)
        {
            ComPtr<IPropertyValue> propertyValue = CreateProperty(value);
            ThrowIfFailed(m_properties->SetAt(index, propertyValue.Get()));

            if (isDefault)
                m_properties->SetChanged(false);
        }

        template<typename T>
        void GetProperty(unsigned int index, T value)
        {
            CheckInPointer(value);

            ComPtr<IPropertyValue> propertyValue;
            ThrowIfFailed(m_properties->GetAt(index, &propertyValue));
            GetFromPropertyValue(propertyValue.Get(), value);
        }

    private:
        void SetProperties();

        //
        // CreateProperty templates
        // 

        template<typename T>
        ComPtr<IPropertyValue> CreateProperty(T value)
        {
            static_assert(false);
            return nullptr;
        }

        template<>
        ComPtr<IPropertyValue> CreateProperty(float value)
        {
            ComPtr<IPropertyValue> propertyValue;
            ThrowIfFailed(m_propertyValueFactory->CreateSingle(value, &propertyValue));
            return propertyValue;
        }

        template<>
        ComPtr<IPropertyValue> CreateProperty(uint32_t value)
        {
            ComPtr<IPropertyValue> propertyValue;
            ThrowIfFailed(m_propertyValueFactory->CreateUInt32(value, &propertyValue));
            return propertyValue;
        }

        template<>
        ComPtr<IPropertyValue> CreateProperty(Numerics::Matrix4x4 value)
        {
            ComPtr<IPropertyValue> propertyValue;

            ThrowIfFailed(m_propertyValueFactory->CreateSingleArray(16, (float*)&value, &propertyValue));
            return propertyValue;
        }

        //
        // GetFromPropertyValue templates
        // 

        template<typename T>
        void GetFromPropertyValue(IPropertyValue* propertyValue, T output)
        {
            static_assert(false);
        }

        template<>
        void GetFromPropertyValue(IPropertyValue* propertyValue, float* output)
        {
            ThrowIfFailed(propertyValue->GetSingle(output));
        }

        template<>
        void GetFromPropertyValue(IPropertyValue* propertyValue, uint32_t* output)
        {
            ThrowIfFailed(propertyValue->GetUInt32(output));
        }

        template<>
        void GetFromPropertyValue(IPropertyValue* propertyValue, Numerics::Matrix4x4* output)
        {
            float* arrayValue = nullptr;
            unsigned int size;

            ThrowIfFailed(propertyValue->GetSingleArray(&size, &arrayValue));

            auto freeArrayWarden = MakeScopeWarden([&] { CoTaskMemFree(arrayValue); });

            if (size!=16)
                ThrowHR(E_BOUNDS);
            *output = *(Numerics::Matrix4x4*)arrayValue;
        }
    };

#define PROPERTY(NAME, TYPE)                             \
        IFACEMETHOD(get_##NAME)(TYPE* value) override;   \
        IFACEMETHOD(put_##NAME)(TYPE value) override

#define IMPLEMENT_ENUM_PROPERTY(CLASS_NAME, PROPERTY_NAME, TYPE, PROPERTY_INDEX)        \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ TYPE* value)               \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    uint32_t uintValue;                                                 \
                    GetProperty(PROPERTY_INDEX, &uintValue);                            \
                    *value = static_cast<TYPE>(uintValue);                              \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ TYPE value)                 \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    uint32_t uintValue = static_cast<uint32_t>(value);                  \
                    SetProperty(PROPERTY_INDEX, uintValue);                             \
                });                                                                     \
        }

// Last parameter accept indexes list of enums which are not supported by this effect
#define IMPLEMENT_ENUM_PROPERTY_WITH_UNSUPPORTED(                                       \
        CLASS_NAME, PROPERTY_NAME, TYPE, PROPERTY_INDEX, ...)                           \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ TYPE* value)               \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    uint32_t uintValue;                                                 \
                    GetProperty(PROPERTY_INDEX, &uintValue);                            \
                    *value = static_cast<TYPE>(uintValue);                              \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ TYPE value)                 \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    uint32_t uintValue = static_cast<uint32_t>(value);                  \
                    std::initializer_list<uint32_t> list = {__VA_ARGS__};               \
                    if (std::find(list.begin(), list.end(), uintValue) != list.end())   \
                        ThrowHR(E_NOTIMPL);                                             \
                    SetProperty(PROPERTY_INDEX, uintValue);                             \
                });                                                                     \
        }

#define IMPLEMENT_INPUT_PROPERTY(CLASS_NAME, INPUT_NAME, INPUT_INDEX)                   \
        IFACEMETHODIMP CLASS_NAME::get_##INPUT_NAME(_Out_ IEffectInput** input)         \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    GetInput(INPUT_INDEX, input);                                       \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##INPUT_NAME(_In_ IEffectInput* input)           \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    SetInput(INPUT_INDEX, input);                                       \
                });                                                                     \
        }

#define IMPLEMENT_PROPERTY_WITH_VALIDATION(CLASS_NAME, PROPERTY_NAME,                   \
                                           TYPE, PROPERTY_INDEX, MIN, MAX)              \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ TYPE* value)               \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    GetProperty(PROPERTY_INDEX, value);                                 \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ TYPE value)                 \
        {                                                                               \
            if (value < MIN || value>MAX)                                               \
            {                                                                           \
                return E_INVALIDARG;                                                    \
            }                                                                           \
                                                                                        \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    SetProperty(PROPERTY_INDEX, value);                                 \
                });                                                                     \
        }

#define IMPLEMENT_PROPERTY(CLASS_NAME, PROPERTY_NAME, TYPE, PROPERTY_INDEX)             \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ TYPE* value)               \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    GetProperty(PROPERTY_INDEX, value);                                 \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ TYPE value)                 \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    SetProperty(PROPERTY_INDEX, value);                                 \
                });                                                                     \
        }
}}}}}