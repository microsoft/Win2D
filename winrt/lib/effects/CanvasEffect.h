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

#include "..\CanvasImage.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Windows::UI;
    using namespace ABI::Microsoft::Graphics::Canvas;
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

        ComPtr<Vector<IInspectable*>> m_properties;
        ComPtr<Vector<IEffectInput*>> m_inputs;

        ComPtr<IPropertyValueStatics> m_propertyValueFactory;

        ComPtr<IUnknown> m_previousDeviceIdentity;
        std::vector<uint64_t> m_previousInputRealizationIds;
        int64_t m_realizationId;
        
        bool m_insideGetImage;

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
        IFACEMETHOD(get_Properties)(_Out_ IVector<IInspectable*>** properties) override;

        //
        // ICanvasImageInternal
        //

        ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext, uint64_t* realizationId);

    protected:
        // for effects with unknown number of inputs, inputs Size have to be set as zero
        CanvasEffect(IID m_effectId, unsigned int propertiesSize, unsigned int inputSize, bool isInputSizeFixed);

        void GetInput(unsigned int index, IEffectInput** input);
        void SetInput(unsigned int index, IEffectInput* input);


        //
        // The main property set/get methods. TBoxed is how we represent the data internally,
        // while TPublic is how it is exposed by strongly typed effect subclasses. For instance
        // enums are stored as unsigned integers, vectors and matrices as float arrays, and
        // colors as float[3] or float[4] depending on whether they include alpha.
        //

        template<typename TBoxed, typename TPublic>
        void SetProperty(unsigned int index, TPublic const& value)
        {
            auto propertyValue = PropertyTypeConverter<TBoxed, TPublic>::Box(m_propertyValueFactory.Get(), value);

            ThrowIfFailed(m_properties->SetAt(index, propertyValue.Get()));
        }

        template<typename TBoxed, typename TPublic>
        void GetProperty(unsigned int index, TPublic* value)
        {
            CheckInPointer(value);

            ComPtr<IInspectable> propertyValue;
            ThrowIfFailed(m_properties->GetAt(index, &propertyValue));

            PropertyTypeConverter<TBoxed, TPublic>::Unbox(As<IPropertyValue>(propertyValue).Get(), value);
        }


    private:
        void SetProperties();


        //
        // PropertyTypeConverter is responsible for converting values between TBoxed and TPublic forms.
        // This is designed to produce compile errors if incompatible types are specified.
        //

        template<typename TBoxed, typename TPublic, typename Enable = void>
        struct PropertyTypeConverter
        {
            static_assert(std::is_same<TBoxed, TPublic>::value, "Default PropertyTypeConverter should only be used when TBoxed = TPublic");

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, TPublic const& value)
            {
                return CreateProperty(factory, value);
            }

            static void Unbox(IPropertyValue* propertyValue, TPublic* result)
            {
                GetPropertyValue(propertyValue, result);
            }
        };


        // Enum values are boxed as unsigned integers.
        template<typename TPublic>
        struct PropertyTypeConverter<uint32_t, TPublic,
                                     typename std::enable_if<std::is_enum<TPublic>::value>::type>
        {
            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, TPublic value)
            {
                return CreateProperty(factory, static_cast<uint32_t>(value));
            }

            static void Unbox(IPropertyValue* propertyValue, TPublic* result)
            {
                uint32_t value;
                GetPropertyValue(propertyValue, &value);
                *result = static_cast<TPublic>(value);
            }
        };


        // Vectors and matrices are boxed as float arrays.
        template<int N, typename TPublic>
        struct PropertyTypeConverter<float[N], TPublic>
        {
            static_assert(std::is_same<TPublic, Numerics::Vector2>::value   ||
                          std::is_same<TPublic, Numerics::Vector3>::value   ||
                          std::is_same<TPublic, Numerics::Vector4>::value   ||
                          std::is_same<TPublic, Numerics::Matrix3x2>::value ||
                          std::is_same<TPublic, Numerics::Matrix4x4>::value,
                          "This type cannot be boxed as a float array");

            static_assert(sizeof(TPublic) == sizeof(float[N]), "Wrong array size");

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, TPublic const& value)
            {
                ComPtr<IPropertyValue> propertyValue;
                ThrowIfFailed(factory->CreateSingleArray(N, (float*)&value, &propertyValue));
                return propertyValue;
            }

            static void Unbox(IPropertyValue* propertyValue, TPublic* result)
            {
                float* arrayValue = nullptr;
                unsigned int size;

                ThrowIfFailed(propertyValue->GetSingleArray(&size, &arrayValue));

                auto freeArrayWarden = MakeScopeWarden([&] { CoTaskMemFree(arrayValue); });

                if (size != N)
                    ThrowHR(E_BOUNDS);

                *result = *reinterpret_cast<TPublic*>(arrayValue);
            }
        };


        // Color can be boxed as a float4 (for properties that include alpha).
        template<>
        struct PropertyTypeConverter<float[4], Color>
        {
            typedef PropertyTypeConverter<float[4], Numerics::Vector4> VectorConverter;

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, Color const& value)
            {
                return VectorConverter::Box(factory, ToVector4(value));
            }

            static void Unbox(IPropertyValue* propertyValue, Color* result)
            {
                Numerics::Vector4 value;
                VectorConverter::Unbox(propertyValue, &value);
                *result = ToWindowsColor(value);
            }
        };


        // Color can also be boxed as float3 (for properties that only use rgb).
        template<>
        struct PropertyTypeConverter<float[3], Color>
        {
            typedef PropertyTypeConverter<float[3], Numerics::Vector3> VectorConverter;

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, Color const& value)
            {
                return VectorConverter::Box(factory, ToVector3(value));
            }

            static void Unbox(IPropertyValue* propertyValue, Color* result)
            {
                Numerics::Vector3 value;
                VectorConverter::Unbox(propertyValue, &value);
                *result = ToWindowsColor(value);
            }
        };


        // Rect is boxed as a float4, after converting WinRT x/y/w/h format to D2D left/top/right/bottom.
        template<>
        struct PropertyTypeConverter<float[4], Rect>
        {
            typedef PropertyTypeConverter<float[4], Numerics::Vector4> VectorConverter;

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, Rect const& value)
            {
                auto d2dRect = ToD2DRect(value);
                return VectorConverter::Box(factory, *ReinterpretAs<Numerics::Vector4*>(&d2dRect));
            }

            static void Unbox(IPropertyValue* propertyValue, Rect* result)
            {
                Numerics::Vector4 value;
                VectorConverter::Unbox(propertyValue, &value);
                *result = FromD2DRect(*ReinterpretAs<D2D1_RECT_F*>(&value));
            }
        };


        //
        // Wrap the IPropertyValue accessors (which use different method names for each type) with
        // overloaded C++ versions that can be used by generic PropertyTypeConverter implementations.
        //

#define PROPERTY_TYPE_ACCESSOR(TYPE, WINRT_NAME)                                                        \
        static ComPtr<IPropertyValue> CreateProperty(IPropertyValueStatics* factory, TYPE const& value) \
        {                                                                                               \
            ComPtr<IPropertyValue> propertyValue;                                                       \
            ThrowIfFailed(factory->Create##WINRT_NAME(value, &propertyValue));                          \
            return propertyValue;                                                                       \
        }                                                                                               \
                                                                                                        \
        static void GetPropertyValue(IPropertyValue* propertyValue, TYPE* result)                       \
        {                                                                                               \
            ThrowIfFailed(propertyValue->Get##WINRT_NAME(result));                                      \
        }

        PROPERTY_TYPE_ACCESSOR(float,    Single)
        PROPERTY_TYPE_ACCESSOR(int32_t,  Int32)
        PROPERTY_TYPE_ACCESSOR(uint32_t, UInt32)
        PROPERTY_TYPE_ACCESSOR(boolean,  Boolean)

#undef PROPERTY_TYPE_ACCESSOR


        //
        // Macros used by the generated strongly typed effect subclasses
        // 

#define PROPERTY(NAME, TYPE)                             \
        IFACEMETHOD(get_##NAME)(TYPE* value) override;   \
        IFACEMETHOD(put_##NAME)(TYPE value) override


#define IMPLEMENT_INPUT_PROPERTY(CLASS_NAME, INPUT_NAME, INPUT_INDEX)                   \
                                                                                        \
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


#define IMPLEMENT_PROPERTY(CLASS_NAME, PROPERTY_NAME,                                   \
                           BOXED_TYPE, PUBLIC_TYPE,                                     \
                           PROPERTY_INDEX)                                              \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ PUBLIC_TYPE* value)        \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    GetProperty<BOXED_TYPE, PUBLIC_TYPE>(PROPERTY_INDEX, value);        \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ PUBLIC_TYPE value)          \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    SetProperty<BOXED_TYPE, PUBLIC_TYPE>(PROPERTY_INDEX, value);        \
                });                                                                     \
        }


#define IMPLEMENT_PROPERTY_WITH_VALIDATION(CLASS_NAME, PROPERTY_NAME,                   \
                                           BOXED_TYPE, PUBLIC_TYPE,                     \
                                           PROPERTY_INDEX, VALIDATOR)                   \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::get_##PROPERTY_NAME(_Out_ PUBLIC_TYPE* value)        \
        {                                                                               \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    GetProperty<BOXED_TYPE, PUBLIC_TYPE>(PROPERTY_INDEX, value);        \
                });                                                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS_NAME::put_##PROPERTY_NAME(_In_ PUBLIC_TYPE value)          \
        {                                                                               \
            if (!(VALIDATOR))                                                           \
            {                                                                           \
                return E_INVALIDARG;                                                    \
            }                                                                           \
                                                                                        \
            return ExceptionBoundary(                                                   \
                [&]                                                                     \
                {                                                                       \
                    SetProperty<BOXED_TYPE, PUBLIC_TYPE>(PROPERTY_INDEX, value);        \
                });                                                                     \
        }
    };
}}}}}
