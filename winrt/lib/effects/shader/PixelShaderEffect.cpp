// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "PixelShaderEffect.h"
#include "PixelShaderEffectImpl.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ActivatableClassWithFactory(PixelShaderEffect, PixelShaderEffectFactory);


    IFACEMETHODIMP PixelShaderEffectFactory::Create(uint32_t shaderCodeCount, BYTE* shaderCode, IPixelShaderEffect** effect)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(shaderCode);
            CheckAndClearOutPointer(effect);

            // Create a shared state object using the specified shader code.
            auto sharedState = Make<SharedShaderState>(shaderCode, shaderCodeCount);
            CheckMakeResult(sharedState);

            // Create the WinRT effect instance.
            auto newEffect = Make<PixelShaderEffect>(nullptr, nullptr, sharedState.Get());
            CheckMakeResult(newEffect);

            ThrowIfFailed(newEffect.CopyTo(effect));
        });
    }


    // Describe how to implement WinRT IMap<> methods in terms of our shader constant buffer.
    template<typename TKey, typename TValue>
    struct PixelShaderEffectPropertyMapTraits
    {
        typedef ElementTraits<TKey> KeyTraits;
        typedef ElementTraits<TValue> ValueTraits;

        typedef PixelShaderEffect* InternalMapType;

        // Forward these accessors to the SharedShaderState.
        static unsigned GetSize(PixelShaderEffect* effect)                                      { return EnsureNotClosed(effect)->m_sharedState->GetPropertyCount(); };
        static bool HasKey(PixelShaderEffect* effect, HSTRING key)                              { return EnsureNotClosed(effect)->m_sharedState->HasProperty(key); }
        static ComPtr<IInspectable> Lookup(PixelShaderEffect* effect, HSTRING key)              { return EnsureNotClosed(effect)->m_sharedState->GetProperty(key); }
        static bool Insert(PixelShaderEffect* effect, HSTRING key, IInspectable* value, bool)   { EnsureNotClosed(effect)->SetProperty(key, value); return true; }
        static std::vector<StringObjectPair> GetKeyValuePairs(PixelShaderEffect* effect)        { return EnsureNotClosed(effect)->m_sharedState->EnumerateProperties(); }

        // Our map is created with IsFixedSize=true, so these should never be called.
        static void Remove(PixelShaderEffect*, TKey const&) { assert(false); }
        static void Clear(PixelShaderEffect*)               { assert(false); }

    private:
        static PixelShaderEffect* EnsureNotClosed(PixelShaderEffect* effect)
        {
            if (!effect)
                ThrowHR(RO_E_CLOSED);

            return effect;
        }
    };


    static ComPtr<ISharedShaderState> GetSharedStateFromD2DEffect(ID2D1Effect* effect)
    {
        ComPtr<IUnknown> sharedState;
        ThrowIfFailed(effect->GetValue(PixelShaderEffectProperty::SharedState, sharedState.GetAddressOf()));

        if (!sharedState)
            ThrowHR(E_INVALIDARG);

        return As<ISharedShaderState>(sharedState);
    }


    PixelShaderEffect::PixelShaderEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : PixelShaderEffect(device, effect, GetSharedStateFromD2DEffect(effect).Get())
    { }


    PixelShaderEffect::PixelShaderEffect(ICanvasDevice* device, ID2D1Effect* effect, ISharedShaderState* sharedState)
        : CanvasEffect(CLSID_PixelShaderEffect, 0, sharedState->Shader().InputCount, true, device, effect, static_cast<IPixelShaderEffect*>(this))
        , m_sharedState(sharedState)
    {
        m_propertyMap = Make<PropertyMap>(true, this);
        CheckMakeResult(m_propertyMap);
    }


    PixelShaderEffect::~PixelShaderEffect()
    {
        m_propertyMap->InternalMap() = nullptr;
    }


    IFACEMETHODIMP PixelShaderEffect::Close()
    {
        m_propertyMap->InternalMap() = nullptr;

        return CanvasEffect::Close();
    }


    IFACEMETHODIMP PixelShaderEffect::get_Properties(IMap<HSTRING, IInspectable*>** value)
    {
        return ExceptionBoundary([&]
        {
            CheckAndClearOutPointer(value);
            ThrowIfFailed(m_propertyMap.CopyTo(value));
        });
    }


    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source1, 0)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source2, 1)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source3, 2)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source4, 3)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source5, 4)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source6, 5)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source7, 6)
    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PixelShaderEffect, Source8, 7)

    
#define IMPLEMENT_COORDINATE_MAPPING_PROPERTY(PROPERTY, INDEX)                          \
                                                                                        \
    IFACEMETHODIMP PixelShaderEffect::get_##PROPERTY(SamplerCoordinateMapping* value)   \
    {                                                                                   \
        return GetCoordinateMapping(INDEX, value);                                      \
    }                                                                                   \
                                                                                        \
    IFACEMETHODIMP PixelShaderEffect::put_##PROPERTY(SamplerCoordinateMapping value)    \
    {                                                                                   \
        return SetCoordinateMapping(INDEX, value);                                      \
    }


    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source1Mapping, 0)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source2Mapping, 1)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source3Mapping, 2)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source4Mapping, 3)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source5Mapping, 4)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source6Mapping, 5)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source7Mapping, 6)
    IMPLEMENT_COORDINATE_MAPPING_PROPERTY(Source8Mapping, 7)


#define IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(PROPERTY, INDEX)                        \
                                                                                        \
    IFACEMETHODIMP PixelShaderEffect::get_##PROPERTY(CanvasImageInterpolation* value)   \
    {                                                                                   \
        return GetSourceInterpolation(INDEX, value);                                    \
    }                                                                                   \
                                                                                        \
    IFACEMETHODIMP PixelShaderEffect::put_##PROPERTY(CanvasImageInterpolation value)    \
    {                                                                                   \
        return SetSourceInterpolation(INDEX, value);                                    \
    }


    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source1Interpolation, 0)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source2Interpolation, 1)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source3Interpolation, 2)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source4Interpolation, 3)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source5Interpolation, 4)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source6Interpolation, 5)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source7Interpolation, 6)
    IMPLEMENT_SOURCE_INTERPOLATION_PROPERTY(Source8Interpolation, 7)


    IFACEMETHODIMP PixelShaderEffect::IsSupported(ICanvasDevice* device, boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(device);
            CheckInPointer(result);

            *result = IsSupported(device);
        });
    }


    bool PixelShaderEffect::IsSupported(ICanvasDevice* device)
    {
        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(As<IDirect3DDxgiInterfaceAccess>(device)->GetInterface(IID_PPV_ARGS(&d3dDevice)));

        auto deviceFeatureLevel = d3dDevice->GetFeatureLevel();
        auto shaderFeatureLevel = m_sharedState->Shader().MinFeatureLevel;

        return deviceFeatureLevel >= shaderFeatureLevel;
    }


    bool PixelShaderEffect::Realize(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        // Validate that this device supports the D3D feature level of the pixel shader.
        if (!IsSupported(RealizationDevice()))
        {
            ThrowHR(E_FAIL, Strings::CustomEffectBadFeatureLevel);
        }

        // Before trying to instantiate our custom effect type, we must register it with the D2D factory.
        ComPtr<ID2D1Factory> factory;
        As<ICanvasDeviceInternal>(RealizationDevice())->GetD2DDevice()->GetFactory(&factory);

        PixelShaderEffectImpl::Register(As<ID2D1Factory1>(factory).Get());

        // Chain to the base implementation.
        if (!CanvasEffect::Realize(flags, targetDpi, deviceContext))
        {
            return false;
        }

        // Set our shared state as properties on the newly realized D2D effect instance.
        ThrowIfFailed(GetResource()->SetValue(PixelShaderEffectProperty::SharedState, m_sharedState.Get()));

        SetD2DConstants();
        SetD2DCoordinateMapping();
        SetD2DSourceInterpolation();

        return true;
    }


    void PixelShaderEffect::Unrealize(unsigned int skipSourceIndex, bool skipAllSources)
    {
        // Break the link between ourselves and the old D2D effect instance
        // by making a separate copy of the shared state object.
        if (HasResource())
        {
            m_sharedState = m_sharedState->Clone();
        }

        // Chain to the base implementation.
        CanvasEffect::Unrealize(skipSourceIndex, skipAllSources);
    }


    IFACEMETHODIMP PixelShaderEffect::GetSource(unsigned int index, IGraphicsEffectSource** source)
    {
        if (index >= m_sharedState->Shader().InputCount && source)
        {
            // Getting an out of range source returns null, rather than throwing like most effects.
            *source = nullptr;
            return S_OK;
        }
        else
        {
            // Chain to the base implementation.
            return CanvasEffect::GetSource(index, source);
        }
    }
    
    
    void PixelShaderEffect::SetSource(unsigned int index, IGraphicsEffectSource* source)
    {
        if (index >= m_sharedState->Shader().InputCount)
        {
            // Setting unused sources to null is valid.
            // If the value is not null, we format a nice exception message.
            if (source)
            {
                WinStringBuilder message;
                message.Format(Strings::CustomEffectSourceOutOfRange, index + 1, m_sharedState->Shader().InputCount);
                ThrowHR(E_INVALIDARG, message.Get());
            }
        }
        else
        {
            // Chain to the base implementation.
            CanvasEffect::SetSource(index, source);
        }
    }


    void PixelShaderEffect::SetProperty(HSTRING name, IInspectable* boxedValue)
    {
        auto lock = Lock(m_mutex);

        // Store the new property value into our shared state object.
        m_sharedState->SetProperty(name, boxedValue);

        // If we are realized, pass the updated constant buffer on to Direct2D.
        SetD2DConstants();
    }


    HRESULT PixelShaderEffect::GetCoordinateMapping(unsigned index, SamplerCoordinateMapping* value)
    {
        assert(index < MaxShaderInputs);

        return ExceptionBoundary([&]
        {
            CheckInPointer(value);

            *value = m_sharedState->CoordinateMapping().Mapping[index];
        });
    }


    HRESULT PixelShaderEffect::SetCoordinateMapping(unsigned index, SamplerCoordinateMapping value)
    {
        assert(index < MaxShaderInputs);

        return ExceptionBoundary([&]
        {
            auto lock = Lock(m_mutex);

            // Store the new value into our shared state object.
            m_sharedState->CoordinateMapping().Mapping[index] = value;

            // If we are realized, pass the updated mapping state on to Direct2D.
            SetD2DCoordinateMapping();
        });
    }


    IFACEMETHODIMP PixelShaderEffect::get_MaxSamplerOffset(int* value)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(value);

            *value = m_sharedState->CoordinateMapping().MaxOffset;
        });
    }


    IFACEMETHODIMP PixelShaderEffect::put_MaxSamplerOffset(int value)
    {
        return ExceptionBoundary([&]
        {
            auto lock = Lock(m_mutex);

            // Store the new value into our shared state object.
            m_sharedState->CoordinateMapping().MaxOffset = value;

            // If we are realized, pass the updated mapping state on to Direct2D.
            SetD2DCoordinateMapping();
        });
    }


    HRESULT PixelShaderEffect::GetSourceInterpolation(unsigned index, CanvasImageInterpolation* value)
    {
        assert(index < MaxShaderInputs);

        return ExceptionBoundary([&]
        {
            CheckInPointer(value);

           *value = FromD2DFilter(m_sharedState->SourceInterpolation().Filter[index]);
        });
    }


    HRESULT PixelShaderEffect::SetSourceInterpolation(unsigned index, CanvasImageInterpolation value)
    {
        assert(index < MaxShaderInputs);

        return ExceptionBoundary([&]
        {
            auto lock = Lock(m_mutex);

            // Convert the enum from Win2D -> D2D format.
            auto d2dFilter = ToD2DFilter(value);

            if (d2dFilter == D2D1_FILTER_FORCE_DWORD)
                ThrowHR(E_INVALIDARG);

            // Store the new value into our shared state object.
            m_sharedState->SourceInterpolation().Filter[index] = d2dFilter;

            // If we are realized, pass the updated interpolation state on to Direct2D.
            SetD2DSourceInterpolation();
        });
    }


    void PixelShaderEffect::SetD2DConstants()
    {
        auto& d2dEffect = MaybeGetResource();
        auto& constants = m_sharedState->Constants();

        if (d2dEffect && !constants.empty())
        {
            ThrowIfFailed(d2dEffect->SetValue(PixelShaderEffectProperty::Constants,
                                              constants.data(),
                                              static_cast<UINT32>(constants.size())));
        }
    }


    void PixelShaderEffect::SetD2DCoordinateMapping()
    {
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            ThrowIfFailed(d2dEffect->SetValue(PixelShaderEffectProperty::CoordinateMapping,
                                              reinterpret_cast<BYTE*>(&m_sharedState->CoordinateMapping()),
                                              sizeof(CoordinateMappingState)));
        }
    }


    void PixelShaderEffect::SetD2DSourceInterpolation()
    {
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            ThrowIfFailed(d2dEffect->SetValue(PixelShaderEffectProperty::SourceInterpolation,
                                              reinterpret_cast<BYTE*>(&m_sharedState->SourceInterpolation()),
                                              sizeof(SourceInterpolationState)));
        }
    }

}}}}}
