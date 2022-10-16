// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Windows::UI;
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ::collections;


    // Metadata created by codegen, used to implement IGraphicsEffectD2D1Interop::GetNamedPropertyMapping.
    struct EffectPropertyMapping
    {
        LPCWSTR Name;
        UINT Index;
        GRAPHICS_EFFECT_PROPERTY_MAPPING Mapping;
    };
    
    struct EffectPropertyMappingTable
    {
        EffectPropertyMapping const* Mappings;
        size_t Count;
    };


    class CanvasEffect
        : public Implements<
            RuntimeClassFlags<WinRtClassicComMix>,
            IGraphicsEffect,
            IGraphicsEffectSource,
            IGraphicsEffectD2D1Interop,
            ICanvasEffect,
            ICanvasImage,
            CloakedIid<ICanvasImageInternal>,
            ChainInterfaces<
                MixIn<CanvasEffect, ResourceWrapper<ID2D1Effect, CanvasEffect, IGraphicsEffect>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>
        , public ResourceWrapper<ID2D1Effect, CanvasEffect, IGraphicsEffect>
    {
        // Unlike other objects, a null ID2D1Effect does not necessarily indicate the object was closed.
        bool m_closed; 
        bool m_insideGetImage;

        IID m_effectId;
        WinString m_name;

        ComPtr<IPropertyValueStatics> m_propertyValueFactory;

        // What device are we currently realized on?
        CachedResourceReference<ID2D1Device, ICanvasDevice> m_realizationDevice;

        // Effect property values (only used when the effect is not realized).
        std::vector<ComPtr<IPropertyValue>> m_properties;

        boolean m_cacheOutput;
        D2D1_BUFFER_PRECISION m_bufferPrecision;

        // Workaround Windows bug 6146411 (crash when reading back DESTINATION_COLOR_CONTEXT from a CLSID_D2D1ColorManagement effect).
        ComPtr<IUnknown> m_workaround6146411;


        // State tracking the effect source images. This data is authoritative when
        // the effect is not realized - otherwise just a cache to speed reverse lookups.
        //
        // For realized effects, CachedResourceReference tracks both the ID2D1Image
        // resource and its IGraphicsEffectSource wrapper. For unrealized effects,
        // the resource is null and only the wrapper part is used.

        struct SourceReference : public CachedResourceReference<ID2D1Image, IGraphicsEffectSource>
        {
            SourceReference()
            { }

            SourceReference(IGraphicsEffectSource* source)
            {
                Set(nullptr, source);
            }

            ComPtr<ID2D1Effect> DpiCompensator;
        };

        std::vector<SourceReference> m_sources;


        // Optionally expose a view of our effect sources as an IVector<> collection.
        template<typename T>
        struct SourcesVectorTraits : public collections::ElementTraits<T>
        {
            typedef CanvasEffect* InternalVectorType;

            static unsigned GetSize(CanvasEffect* effect)                       { return EnsureNotClosed(effect)->GetSourceCount(); };
            static ElementType GetAt(CanvasEffect* effect, unsigned index)      { return EnsureNotClosed(effect)->GetSource(index); }
            static void SetAt(CanvasEffect* effect, unsigned index, T item)     { EnsureNotClosed(effect)->SetSource(index, item); }
            static void InsertAt(CanvasEffect* effect, unsigned index, T item)  { EnsureNotClosed(effect)->InsertSource(index, item); }
            static void RemoveAt(CanvasEffect* effect, unsigned index)          { EnsureNotClosed(effect)->RemoveSource(index); }
            static void Append(CanvasEffect* effect, T item)                    { EnsureNotClosed(effect)->AppendSource(item); }
            static void Clear(CanvasEffect* effect)                             { EnsureNotClosed(effect)->ClearSources(); }

        private:
            static CanvasEffect* EnsureNotClosed(CanvasEffect* effect)
            {
                if (!effect)
                    ThrowHR(RO_E_CLOSED);

                return effect;
            }
        };

        typedef Vector<IGraphicsEffectSource*, SourcesVectorTraits> SourcesVector;

        ComPtr<SourcesVector> m_sourcesVector;


        // Generated table of effect factory functions, used by interop to create strongly
        // typed wrapper classes. m_effectMakers is terminated by a null MakeEffectFunction.
        typedef void(*MakeEffectFunction)(ICanvasDevice* device, ID2D1Effect* d2dEffect, ComPtr<IInspectable>* result);

        static std::pair<IID, MakeEffectFunction> m_effectMakers[];


    protected:
        // Constructor.
        CanvasEffect(IID const& m_effectId, unsigned int propertiesSize, unsigned int sourcesSize, bool isSourcesSizeFixed, ICanvasDevice* device, ID2D1Effect* effect, IInspectable* outerInspectable);

        virtual ~CanvasEffect();

        virtual EffectPropertyMappingTable GetPropertyMapping()          { return EffectPropertyMappingTable{ nullptr, 0 }; }
        virtual EffectPropertyMappingTable GetPropertyMappingHandCoded() { return EffectPropertyMappingTable{ nullptr, 0 }; }

        ComPtr<SourcesVector> const& Sources() { return m_sourcesVector; }
        
        ICanvasDevice* RealizationDevice() { return m_realizationDevice.GetWrapper(); }

        std::mutex m_mutex;

    public:
        // Used by ResourceManager::GetOrCreate.
        static bool TryCreateEffect(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result);
            
        //
        // ICanvasImage
        //

        IFACEMETHOD(GetBounds)(ICanvasResourceCreator* resourceCreator, Rect *bounds) override;
        IFACEMETHOD(GetBoundsWithTransform)(ICanvasResourceCreator* resourceCreator, Numerics::Matrix3x2 transform, Rect *bounds) override;

        //
        // ICanvasImageInterop
        //

        IFACEMETHOD(GetDevice)(ICanvasDevice** device) override;

        //
        // ICanvasImageInternal
        //

        virtual ComPtr<ID2D1Image> GetD2DImage(ICanvasDevice* device, ID2D1DeviceContext* deviceContext, GetImageFlags flags, float targetDpi, float* realizedDpi = nullptr) override;

        //
        // ICanvasResourceWrapperNative
        //

        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** resource) override;

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // IGraphicsEffect
        //

        IFACEMETHOD(get_Name)(HSTRING* name) override;
        IFACEMETHOD(put_Name)(HSTRING name) override;

        //
        // IGraphicsEffectD2D1Interop
        //

        IFACEMETHOD(GetEffectId)(GUID* id) override;
        IFACEMETHOD(GetSourceCount)(UINT* count) override;
        IFACEMETHOD(GetSource)(UINT index, IGraphicsEffectSource** source) override;
        IFACEMETHOD(GetPropertyCount)(UINT* count) override;
        IFACEMETHOD(GetProperty)(UINT index, IPropertyValue** value) override;
        IFACEMETHOD(GetNamedPropertyMapping)(LPCWSTR name, UINT* index, GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) override;

        //
        // ICanvasEffect
        //

        IFACEMETHOD(get_CacheOutput)(boolean* value) override;
        IFACEMETHOD(put_CacheOutput)(boolean value) override;
        IFACEMETHOD(get_BufferPrecision)(IReference<CanvasBufferPrecision>** value) override;
        IFACEMETHOD(put_BufferPrecision)(IReference<CanvasBufferPrecision>* value) override;
        IFACEMETHOD(InvalidateSourceRectangle)(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t sourceIndex, Rect invalidRectangle) override;
        IFACEMETHOD(GetInvalidRectangles)(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t* valueCount, Rect** valueElements) override;
        IFACEMETHOD(GetRequiredSourceRectangle)(ICanvasResourceCreatorWithDpi* resourceCreator, Rect outputRectangle, ICanvasEffect* sourceEffect, uint32_t sourceIndex, Rect sourceBounds, Rect* value) override;
        IFACEMETHOD(GetRequiredSourceRectangles)(ICanvasResourceCreatorWithDpi* resourceCreator, Rect outputRectangle, uint32_t sourceEffectCount, ICanvasEffect** sourceEffects, uint32_t sourceIndexCount, uint32_t* sourceIndices, uint32_t sourceBoundsCount, Rect* sourceBounds, uint32_t* valueCount, Rect** valueElements) override;


    protected:
        //
        // The main property set/get methods. TBoxed is how we represent the data internally,
        // while TPublic is how it is exposed by strongly typed effect subclasses. For instance
        // enums are stored as unsigned integers, vectors and matrices as float arrays, and
        // colors as float[3] or float[4] depending on whether they include alpha.
        //

        template<typename TBoxed, typename TPublic>
        void SetBoxedProperty(unsigned int index, TPublic const& value)
        {
            auto boxedValue = PropertyTypeConverter<TBoxed, TPublic>::Box(m_propertyValueFactory.Get(), value);

            SetProperty(index, boxedValue.Get());
        }

        template<typename TBoxed, typename TPublic>
        void GetBoxedProperty(unsigned int index, TPublic* value)
        {
            CheckInPointer(value);

            auto boxedValue = GetProperty(index);

            PropertyTypeConverter<TBoxed, TPublic>::Unbox(boxedValue.Get(), value);
        }

        template<typename T>
        void SetArrayProperty(unsigned int index, uint32_t valueCount, T const* value)
        {
            auto boxedValue = CreateProperty(m_propertyValueFactory.Get(), valueCount, value);

            SetProperty(index, boxedValue.Get());
        }

        template<typename T>
        void SetArrayProperty(unsigned int index, std::initializer_list<T> const& value)
        {
            SetArrayProperty<T>(index, static_cast<uint32_t>(value.size()), value.begin());
        }

        template<typename T>
        void GetArrayProperty(unsigned int index, uint32_t* valueCount, T** value)
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(value);

            auto boxedValue = GetProperty(index);

            GetValueOfProperty(boxedValue.Get(), valueCount, value);
        }


        // Marker types, used as TBoxed for values that need special conversion.
        struct ConvertRadiansToDegrees { };
        struct ConvertAlphaMode { };
        struct ConvertColorHdrToVector3 { };


        // Methods for manipulating the collection of source images, used by SourcesVector::Traits.
        unsigned int GetSourceCount();
        ComPtr<IGraphicsEffectSource> GetSource(unsigned int index);
        void SetSource(unsigned int index, IGraphicsEffectSource* source);
        void InsertSource(unsigned int index, IGraphicsEffectSource* source);
        void RemoveSource(unsigned int index);
        void AppendSource(IGraphicsEffectSource* source);
        void ClearSources();


        // On-demand creation of the underlying D2D image effect.
        virtual bool Realize(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext);
        virtual void Unrealize(unsigned int skipSourceIndex = UINT_MAX, bool skipAllSources = false);

    private:
        ComPtr<ID2D1Effect> CreateD2DEffect(ID2D1DeviceContext* deviceContext, IID const& effectId);
        bool ApplyDpiCompensation(unsigned int index, ComPtr<ID2D1Image>& inputImage, float inputDpi, GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext);
        void RefreshInputs(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext);
        
        bool SetD2DInput(ID2D1Effect* d2dEffect, unsigned int index, IGraphicsEffectSource* source, GetImageFlags flags, float targetDpi = 0, ID2D1DeviceContext* deviceContext = nullptr);
        ComPtr<IGraphicsEffectSource> GetD2DInput(ID2D1Effect* d2dEffect, unsigned int index);

        void SetProperty(unsigned int index, IPropertyValue* propertyValue);
        void SetD2DProperty(ID2D1Effect* d2dEffect, unsigned int index, IPropertyValue* propertyValue);

        ComPtr<IPropertyValue> GetProperty(unsigned int index);
        ComPtr<IPropertyValue> GetD2DProperty(ID2D1Effect* d2dEffect, unsigned int index);

        void ThrowIfClosed();


        // Used by EffectMakers.cpp to populate the m_effectMakers table.
        template<typename T>
        static void MakeEffect(ICanvasDevice* device, ID2D1Effect* d2dEffect, ComPtr<IInspectable>* result)
        {
            auto wrapper = Make<T>(device, d2dEffect);
            CheckMakeResult(wrapper);
            ThrowIfFailed(wrapper.As(result));
        }


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
                GetValueOfProperty(propertyValue, result);
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
                GetValueOfProperty(propertyValue, &value);
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
                          std::is_same<TPublic, Numerics::Matrix4x4>::value ||
                          std::is_same<TPublic, Matrix5x4>::value,
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
                ComArray<float> value;

                ThrowIfFailed(propertyValue->GetSingleArray(value.GetAddressOfSize(), value.GetAddressOfData()));

                if (value.GetSize() != N)
                    ThrowHR(E_BOUNDS);

                *result = *reinterpret_cast<TPublic*>(value.GetData());
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


        // HDR color (Vector4) can be boxed as a float3 (for properties that only use rgb).
        template<>
        struct PropertyTypeConverter<ConvertColorHdrToVector3, Numerics::Vector4>
        {
            typedef PropertyTypeConverter<float[3], Numerics::Vector3> VectorConverter;

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, Numerics::Vector4 const& value)
            {
                return VectorConverter::Box(factory, Numerics::Vector3{ value.X, value.Y, value.Z });
            }

            static void  Unbox(IPropertyValue* propertyValue, Numerics::Vector4* result)
            {
                Numerics::Vector3 value;
                VectorConverter::Unbox(propertyValue, &value);
                *result = Numerics::Vector4{ value.X, value.Y, value.Z, 1.0f };
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


        // Handle the conversion for angles that are exposed as radians in WinRT, while internally D2D uses degrees.
        template<>
        struct PropertyTypeConverter<ConvertRadiansToDegrees, float>
        {
            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, float value)
            {
                return CreateProperty(factory, ::DirectX::XMConvertToDegrees(value));
            }

            static void Unbox(IPropertyValue* propertyValue, float* result)
            {
                float degrees;
                GetValueOfProperty(propertyValue, &degrees);
                *result = ::DirectX::XMConvertToRadians(degrees);
            }
        };


        // Handle the conversion between CanvasAlphaMode and D2D1_ALPHA_MODE/D2D1_COLORMATRIX_ALPHA_MODE enum values.
        template<>
        struct PropertyTypeConverter<ConvertAlphaMode, CanvasAlphaMode>
        {
            static_assert(D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED == D2D1_ALPHA_MODE_PREMULTIPLIED, "Enum values should match");
            static_assert(D2D1_COLORMATRIX_ALPHA_MODE_STRAIGHT == D2D1_ALPHA_MODE_STRAIGHT, "Enum values should match");

            static ComPtr<IPropertyValue> Box(IPropertyValueStatics* factory, CanvasAlphaMode value)
            {
                if (value == CanvasAlphaMode::Ignore)
                    ThrowHR(E_INVALIDARG);

                return CreateProperty(factory, static_cast<uint32_t>(ToD2DAlphaMode(value)));
            }

            static void Unbox(IPropertyValue* propertyValue, CanvasAlphaMode* result)
            {
                uint32_t value;
                GetValueOfProperty(propertyValue, &value);
                *result = FromD2DAlphaMode(static_cast<D2D1_ALPHA_MODE>(value));
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
        static void GetValueOfProperty(IPropertyValue* propertyValue, TYPE* result)                     \
        {                                                                                               \
            ThrowIfFailed(propertyValue->Get##WINRT_NAME(result));                                      \
        }

#define ARRAY_PROPERTY_TYPE_ACCESSOR(TYPE, WINRT_NAME)                                                                          \
        static ComPtr<IPropertyValue> CreateProperty(IPropertyValueStatics* factory, uint32_t valueCount, TYPE const* value)    \
        {                                                                                                                       \
            ComPtr<IPropertyValue> propertyValue;                                                                               \
            ThrowIfFailed(factory->Create##WINRT_NAME##Array(valueCount, const_cast<TYPE*>(value), &propertyValue));            \
            return propertyValue;                                                                                               \
        }                                                                                                                       \
                                                                                                                                \
        static void GetValueOfProperty(IPropertyValue* propertyValue, uint32_t* valueCount, TYPE** value)                       \
        {                                                                                                                       \
            ThrowIfFailed(propertyValue->Get##WINRT_NAME##Array(valueCount, value));                                            \
        }

        PROPERTY_TYPE_ACCESSOR(float,    Single)
        PROPERTY_TYPE_ACCESSOR(int32_t,  Int32)
        PROPERTY_TYPE_ACCESSOR(uint32_t, UInt32)
        PROPERTY_TYPE_ACCESSOR(boolean,  Boolean)
        
        ARRAY_PROPERTY_TYPE_ACCESSOR(float, Single)

#undef PROPERTY_TYPE_ACCESSOR
#undef ARRAY_PROPERTY_TYPE_ACCESSOR


        // Interface types are stored as a PropertyType_InspectableArray containing a single IInspectable.
        // This is because IPropertyValue provides CreateInspectableArray, but not CreateInspectable.
        static ComPtr<IPropertyValue> CreateProperty(IPropertyValueStatics* factory, IInspectable* value)
        {
            ComPtr<IPropertyValue> propertyValue;
            ThrowIfFailed(factory->CreateInspectableArray(1, &value, &propertyValue));
            return propertyValue;
        }

        template<typename T>
        static void GetValueOfProperty(IPropertyValue* propertyValue, T** result)
        {
            static_assert(std::is_base_of<IInspectable, T>::value, "Interface types must be IInspectable");
            
            ComArray<ComPtr<IInspectable>> value;
            ThrowIfFailed(propertyValue->GetInspectableArray(value.GetAddressOfSize(), value.GetAddressOfData()));

            if (value.GetSize() != 1)
                ThrowHR(E_NOTIMPL);

            if (value[0])
                ThrowIfFailed(value[0].CopyTo(result));
            else
                *result = nullptr;
        }


        //
        // Macros used by the generated strongly typed effect subclasses
        // 

#define EFFECT_PROPERTY(NAME, TYPE)                                                     \
        IFACEMETHOD(get_##NAME)(TYPE* value) override;                                  \
        IFACEMETHOD(put_##NAME)(TYPE value) override

#define EFFECT_ARRAY_PROPERTY(NAME, TYPE)                                               \
        IFACEMETHOD(get_##NAME)(UINT32 *valueCount, TYPE **valueElements) override;     \
        IFACEMETHOD(put_##NAME)(UINT32 valueCount, TYPE *valueElements) override

#define EFFECT_SOURCES_PROPERTY()                                                       \
        IFACEMETHOD(get_Sources)(IVector<IGraphicsEffectSource*>** value) override


#define IMPLEMENT_EFFECT_PROPERTY(CLASS, PROPERTY, BOXED_TYPE, PUBLIC_TYPE, INDEX)      \
                                                                                        \
        IFACEMETHODIMP CLASS::get_##PROPERTY(_Out_ PUBLIC_TYPE* value)                  \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                GetBoxedProperty<BOXED_TYPE, PUBLIC_TYPE>(INDEX, value);                \
            });                                                                         \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS::put_##PROPERTY(_In_ PUBLIC_TYPE value)                    \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                SetBoxedProperty<BOXED_TYPE, PUBLIC_TYPE>(INDEX, value);                \
            });                                                                         \
        }


#define IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(CLASS, PROPERTY, BOXED_TYPE,          \
                                                  PUBLIC_TYPE, INDEX, VALIDATOR)        \
                                                                                        \
        IFACEMETHODIMP CLASS::get_##PROPERTY(_Out_ PUBLIC_TYPE* value)                  \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                GetBoxedProperty<BOXED_TYPE, PUBLIC_TYPE>(INDEX, value);                \
            });                                                                         \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS::put_##PROPERTY(_In_ PUBLIC_TYPE value)                    \
        {                                                                               \
            if (!(VALIDATOR))                                                           \
            {                                                                           \
                return E_INVALIDARG;                                                    \
            }                                                                           \
                                                                                        \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                SetBoxedProperty<BOXED_TYPE, PUBLIC_TYPE>(INDEX, value);                \
            });                                                                         \
        }
    };


#define IMPLEMENT_EFFECT_ARRAY_PROPERTY(CLASS, PROPERTY, TYPE, INDEX)                   \
                                                                                        \
        IFACEMETHODIMP CLASS::get_##PROPERTY(UINT32 *valueCount, TYPE **valueElements)  \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                GetArrayProperty<TYPE>(INDEX, valueCount, valueElements);               \
            });                                                                         \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS::put_##PROPERTY(UINT32 valueCount, TYPE *valueElements)    \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                SetArrayProperty<TYPE>(INDEX, valueCount, valueElements);               \
            });                                                                         \
        }


#define IMPLEMENT_EFFECT_SOURCE_PROPERTY(CLASS, SOURCE_NAME, SOURCE_INDEX)              \
                                                                                        \
        IFACEMETHODIMP CLASS::get_##SOURCE_NAME(_Out_ IGraphicsEffectSource** source)   \
        {                                                                               \
            return GetSource(SOURCE_INDEX, source);                                     \
        }                                                                               \
                                                                                        \
        IFACEMETHODIMP CLASS::put_##SOURCE_NAME(_In_ IGraphicsEffectSource* source)     \
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                SetSource(SOURCE_INDEX, source);                                        \
            });                                                                         \
        }


#define IMPLEMENT_EFFECT_SOURCES_PROPERTY(CLASS)                                        \
                                                                                        \
        IFACEMETHODIMP CLASS::get_Sources(_Out_ IVector<IGraphicsEffectSource*>** value)\
        {                                                                               \
            return ExceptionBoundary([&]                                                \
            {                                                                           \
                CheckAndClearOutPointer(value);                                         \
                ThrowIfFailed(Sources().CopyTo(value));                                 \
            });                                                                         \
        }


#define EFFECT_PROPERTY_MAPPING()                                                       \
    EffectPropertyMappingTable GetPropertyMapping() override

#define EFFECT_PROPERTY_MAPPING_HANDCODED()                                             \
    EffectPropertyMappingTable GetPropertyMappingHandCoded() override


#define IMPLEMENT_EFFECT_PROPERTY_MAPPING(CLASS, ...)                                   \
    IMPLEMENT_EFFECT_PROPERTY_MAPPING_WORKER(CLASS, PropertyMapping, __VA_ARGS__)

#define IMPLEMENT_EFFECT_PROPERTY_MAPPING_HANDCODED(CLASS, ...)                         \
    IMPLEMENT_EFFECT_PROPERTY_MAPPING_WORKER(CLASS, PropertyMappingHandCoded, __VA_ARGS__)


#define IMPLEMENT_EFFECT_PROPERTY_MAPPING_WORKER(CLASS, SUFFIX, ...)                    \
    static const EffectPropertyMapping CLASS##SUFFIX[] =                                \
    {                                                                                   \
        __VA_ARGS__                                                                     \
    };                                                                                  \
                                                                                        \
    EffectPropertyMappingTable CLASS::Get##SUFFIX()                                     \
    {                                                                                   \
        auto count = sizeof(CLASS##SUFFIX) / sizeof(EffectPropertyMapping);             \
        return EffectPropertyMappingTable{ CLASS##SUFFIX, count };                      \
    }

}}}}}
