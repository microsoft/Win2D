// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    CanvasEffect::CanvasEffect(IID const& effectId, unsigned int propertiesSize, unsigned int sourcesSize, bool isSourcesSizeFixed, ICanvasDevice* device, ID2D1Effect* effect, IInspectable* outerInspectable)
        : ResourceWrapper(effect, outerInspectable)
        , m_effectId(effectId)
        , m_propertiesChanged(false)
        , m_realizationId(0)
        , m_insideGetImage(false)
        , m_closed(false)
    {
        m_properties.resize(propertiesSize);

        m_sources = Make<Vector<IGraphicsEffectSource*>>(sourcesSize, isSourcesSizeFixed);
        CheckMakeResult(m_sources);
        m_sources->SetChanged(true);

        // Create propertyValueFactory
        Wrappers::HStringReference stringActivableClassId(RuntimeClass_Windows_Foundation_PropertyValue);
        ThrowIfFailed(GetActivationFactory(stringActivableClassId.Get(), &m_propertyValueFactory));

        if (effect)
        {
            InitializeInputsFromD2D(device);
            InitializePropertiesFromD2D();

            m_previousDeviceIdentity = As<IUnknown>(As<ICanvasDeviceInternal>(device)->GetD2DDevice());
        }
    }


    bool CanvasEffect::TryCreateEffect(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result)
    {
        UNREFERENCED_PARAMETER(dpi);

        // Is this resource an effect?
        auto d2dEffect = MaybeAs<ID2D1Effect>(resource);

        if (!d2dEffect)
            return false;

        // Look up which strongly typed Win2D wrapper class matches the effect CLSID.
        IID effectId = d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID);

        for (auto effectMaker = m_effectMakers; effectMaker->second; effectMaker++)
        {
            if (IsEqualGUID(effectMaker->first, effectId))
            {
                // Found it! Create the Win2D wrapper class.
                if (!device)
                    ThrowHR(E_INVALIDARG, Strings::ResourceManagerNoDevice);

                effectMaker->second(device, d2dEffect.Get(), result);
                return true;
            }
        }

        // Unrecognized effect CLSID.
        return false;
    }


    //
    // ICanvasImageInternal
    //

    IFACEMETHODIMP CanvasEffect::GetBounds(
        ICanvasDrawingSession* drawingSession,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, nullptr, bounds);
    }


    IFACEMETHODIMP CanvasEffect::GetBoundsWithTransform(
        ICanvasDrawingSession* drawingSession,
        Numerics::Matrix3x2 transform,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, &transform, bounds);
    }


    //
    // ICanvasImageInternal
    //

    static float GetTargetDpi(ID2D1DeviceContext* deviceContext)
    {
        ComPtr<ID2D1Image> target;
        deviceContext->GetTarget(&target);

        if (MaybeAs<ID2D1CommandList>(target))
        {
            // Command lists are DPI independent, so we always need to insert
            // DPI compensation.
            return MAGIC_FORCE_DPI_COMPENSATION_VALUE;
        }
        else
        {
            return GetDpi(deviceContext);
        }        
    }


    ComPtr<ID2D1Image> CanvasEffect::GetD2DImage(ID2D1DeviceContext* deviceContext)
    {
        ThrowIfClosed();

        float targetDpi = GetTargetDpi(deviceContext);

        return GetRealizedEffectNode(deviceContext, targetDpi).Image;
    }


    ICanvasImageInternal::RealizedEffectNode CanvasEffect::GetRealizedEffectNode(ID2D1DeviceContext* deviceContext, float targetDpi)
    {
        ThrowIfClosed();

        // Check if device is the same as previous device
        ComPtr<ID2D1Device> device;
        deviceContext->GetDevice(&device);
        auto deviceIdentity = As<IUnknown>(device);

        if (deviceIdentity != m_previousDeviceIdentity)
        {
            m_previousDeviceIdentity = deviceIdentity;
            ReleaseResource();
            m_dpiCompensators.clear();
        }

        // Create resource if not created yet
        // TODO #802: make sure this lazy create (and the following cycle detection) is made properly threadsafe
        bool wasRecreated = false;

        if (!HasResource())
        {
            ComPtr<ID2D1Effect> newEffect;
            ThrowIfFailed(deviceContext->CreateEffect(m_effectId, &newEffect));
            SetResource(newEffect.Get());
            wasRecreated = true;
            m_realizationId++;
        }

        // If this is a DPI compensation effect, we no longer need to insert
        // any further compensation, so stop tracking the target DPI
        if (IsEqualGUID(m_effectId, CLSID_D2D1DpiCompensation))
        {
            targetDpi = 0;
        }

        // Check for graph cycles
        if (m_insideGetImage)
            ThrowHR(D2DERR_CYCLIC_GRAPH);

        m_insideGetImage = true;
        auto clearFlagWarden = MakeScopeWarden([&] { m_insideGetImage = false; });

        // Update ID2D1Image with the latest property values if a change is detected
        if (wasRecreated || m_propertiesChanged)
        {
            SetD2DProperties();
        }

        // Update ID2D1Image with the latest inputs, and recurse through 
        // the effect graph to make sure child nodes are properly realized
        SetD2DInputs(deviceContext, targetDpi, wasRecreated);

        return RealizedEffectNode{ As<ID2D1Image>(ResourceWrapper::GetResource().Get()), 0, m_realizationId };
    }


    //
    // ICanvasResourceWrapperNative
    //

    IFACEMETHODIMP CanvasEffect::GetResource(ICanvasDevice* device, float dpi, REFIID iid, void** resource)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(resource);
                ThrowIfClosed();

                if (!device)
                    ThrowHR(E_INVALIDARG, Strings::GetResourceNoDevice);

                // If the caller did not specify target DPI, we always need to insert DPI compensation
                // (same as when using effects with DPI independent contexts such as command lists).
                if (dpi <= 0)
                    dpi = MAGIC_FORCE_DPI_COMPENSATION_VALUE;

                auto deviceContext = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();

                auto realizedEffect = GetRealizedEffectNode(deviceContext.Get(), dpi);

                ThrowIfFailed(realizedEffect.Image.CopyTo(iid, resource));
            });
    }


    //
    // IClosable
    //

    IFACEMETHODIMP CanvasEffect::Close()
    {
        ReleaseResource();
        m_previousDeviceIdentity.Reset();
        m_dpiCompensators.clear();
        
        auto& sources = m_sources->InternalVector();

        if (m_sources->IsFixedSize())
        {
            sources.assign(sources.size(), nullptr);
        }
        else
        {
            sources.clear();
        }

        m_closed = true;

        return S_OK;
    }


    //
    // IGraphicsEffect
    //

    IFACEMETHODIMP CanvasEffect::get_Name(HSTRING* name)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(name);
                m_name.CopyTo(name);
            });
    }


    IFACEMETHODIMP CanvasEffect::put_Name(HSTRING name)
    {
        return ExceptionBoundary(
            [&]
            {
                m_name = name;
            });
    }


    //
    // IGraphicsEffectD2D1Interop
    //

    IFACEMETHODIMP CanvasEffect::GetEffectId(GUID* id)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(id);
                *id = m_effectId;
            });
    }


    IFACEMETHODIMP CanvasEffect::GetSourceCount(UINT* count)
    {
        return m_sources->get_Size(count);
    }


    IFACEMETHODIMP CanvasEffect::GetSource(UINT index, IGraphicsEffectSource** source)
    {
        return m_sources->GetAt(index, source);
    }


    IFACEMETHODIMP CanvasEffect::GetPropertyCount(UINT* count)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(count);
                *count = static_cast<UINT>(m_properties.size());
            });
    }


    IFACEMETHODIMP CanvasEffect::GetProperty(UINT index, IPropertyValue** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
        
                if (index >= m_properties.size())
                    ThrowHR(E_BOUNDS);

                ThrowIfFailed(m_properties[index].CopyTo(value));
            });
    }


    static bool FindNamedProperty(EffectPropertyMappingTable const& table, LPCWSTR name, UINT* index, GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping)
    {
        for (size_t i = 0; i < table.Count; i++)
        {
            if (CompareStringOrdinal(name, -1, table.Mappings[i].Name, -1, true) == CSTR_EQUAL)
            {
                *index = table.Mappings[i].Index;
                *mapping = table.Mappings[i].Mapping;

                return true;
            }
        }

        return false;
    }


    IFACEMETHODIMP CanvasEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(name);
                CheckInPointer(index);
                CheckInPointer(mapping);

                // Search the codegened property mapping table.
                auto table = GetPropertyMapping();

                if (!FindNamedProperty(table, name, index, mapping))
                {
                    // If not in the main table, could this be one of the special cases defined in a separate hand-coded table?
                    table = GetPropertyMappingHandCoded();

                    if (!FindNamedProperty(table, name, index, mapping))
                    {
                        ThrowHR(E_INVALIDARG);
                    }
                }
            });
    }


    STDMETHODIMP CanvasEffect::SetSource(UINT index, IGraphicsEffectSource* source)
    {
        return m_sources->SetAt(index, source);
    }


    static ComPtr<ID2D1Effect> InsertDpiCompensationEffect(ID2D1DeviceContext* deviceContext, ID2D1Image* inputImage, float inputDpi, ID2D1Effect* reuseExistingCompensator)
    {
        ComPtr<ID2D1Effect> dpiCompensator = reuseExistingCompensator;

        // Create the D2D1DpiCompensation effect, if we don't have one already.
        if (!dpiCompensator)
        {
            ThrowIfFailed(deviceContext->CreateEffect(CLSID_D2D1DpiCompensation, &dpiCompensator));

            ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD));
            ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, D2D1_DPICOMPENSATION_INTERPOLATION_MODE_LINEAR));
        }

        // Set our input image as source for the DPI compensation.
        dpiCompensator->SetInput(0, inputImage);
        
        // Set the DPI.
        ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INPUT_DPI, D2D1_VECTOR_2F{ inputDpi, inputDpi }));

        return dpiCompensator;
    }


    void CanvasEffect::SetD2DInputs(ID2D1DeviceContext* deviceContext, float targetDpi, bool wasRecreated)
    {
        auto& d2dEffect = ResourceWrapper::GetResource();

        auto& sources = m_sources->InternalVector();
        auto sourcesSize = (unsigned int)sources.size();

        bool sourcesChanged = wasRecreated || m_sources->IsChanged();

        // Resize sources array?
        if (sourcesChanged)
        {
            if (!m_sources->IsFixedSize())
            {
                // This test might need to be made more customizable if we ever add effects
                // that take variable numbers of inputs and do allow zero of them.
                if (sourcesSize == 0)
                    ThrowHR(E_INVALIDARG, Strings::EffectNoSources);

                ThrowIfFailed(d2dEffect->SetInputCount(sourcesSize));
            }

            m_previousSourceRealizationIds.resize(sourcesSize);
            m_dpiCompensators.resize(sourcesSize);
        }

        for (unsigned int i = 0; i < sourcesSize; ++i)
        {
            // Look up the WinRT interface representing this input
            if (!sources[i])
            {
                WinStringBuilder message;
                message.Format(Strings::EffectNullSource, i);
                ThrowHR(E_INVALIDARG, message.Get());
            }

            ComPtr<ICanvasImageInternal> internalSource;
            HRESULT hr = sources[i].As(&internalSource);

            if (FAILED(hr))
            {
                if (hr == E_NOINTERFACE)
                {
                    WinStringBuilder message;
                    message.Format(Strings::EffectWrongSourceType, i);
                    ThrowHR(hr, message.Get());
                }
                else
                {
                    ThrowHR(hr);
                }
            }

            // Get the underlying D2D interface (this call recurses through the effect graph)
            auto realizedSource = internalSource->GetRealizedEffectNode(deviceContext, targetDpi);

            bool needsDpiCompensation = (realizedSource.Dpi != targetDpi) && (realizedSource.Dpi != 0) && (targetDpi != 0);
            bool hasDpiCompensation = m_dpiCompensators[i] != nullptr;

            // If the source value has changed, update the D2D effect graph
            if (sourcesChanged || 
                realizedSource.RealizationId != m_previousSourceRealizationIds[i] ||
                needsDpiCompensation != hasDpiCompensation)
            {
                if (needsDpiCompensation)
                {
                    m_dpiCompensators[i] = InsertDpiCompensationEffect(deviceContext, realizedSource.Image.Get(), realizedSource.Dpi, m_dpiCompensators[i].Get());
                    realizedSource.Image = As<ID2D1Image>(m_dpiCompensators[i]);
                }
                else
                {
                    m_dpiCompensators[i].Reset();
                }

                d2dEffect->SetInput(i, realizedSource.Image.Get());
                m_previousSourceRealizationIds[i] = realizedSource.RealizationId;
            }
        }

        m_sources->SetChanged(false);
    }


    void CanvasEffect::SetD2DProperties()
    {
        auto& d2dEffect = ResourceWrapper::GetResource();

        for (unsigned i = 0; i < m_properties.size(); ++i)
        {
            auto& propertyValue = m_properties[i];

            PropertyType propertyType;
            ThrowIfFailed(propertyValue->get_Type(&propertyType));

            switch (propertyType)
            {
            case PropertyType_Boolean:
                {
                    boolean value;
                    ThrowIfFailed(propertyValue->GetBoolean(&value));
                    ThrowIfFailed(d2dEffect->SetValue(i, static_cast<BOOL>(value)));
                }
                break;

            case PropertyType_Int32:
                {
                    INT32 value;
                    ThrowIfFailed(propertyValue->GetInt32(&value));
                    ThrowIfFailed(d2dEffect->SetValue(i, value));
                }
                break;

            case PropertyType_UInt32:
                {
                    UINT32 value;
                    ThrowIfFailed(propertyValue->GetUInt32(&value));
                    ThrowIfFailed(d2dEffect->SetValue(i, value));
                }
                break;

            case PropertyType_Single:
                {
                    float value;
                    ThrowIfFailed(propertyValue->GetSingle(&value));
                    ThrowIfFailed(d2dEffect->SetValue(i, value));
                }
                break;

            case PropertyType_SingleArray:
                {
                    ComArray<float> value;
                    ThrowIfFailed(propertyValue->GetSingleArray(value.GetAddressOfSize(), value.GetAddressOfData()));
                    ThrowIfFailed(d2dEffect->SetValue(i, reinterpret_cast<BYTE*>(value.GetData()), value.GetSize() * sizeof(float)));
                }
                break;

            default:
                ThrowHR(E_NOTIMPL);
            }
        }

        m_propertiesChanged = false;
    }


    void CanvasEffect::InitializeInputsFromD2D(ICanvasDevice* device)
    {
        auto& d2dEffect = ResourceWrapper::GetResource();

        auto& sources = m_sources->InternalVector();

        if (!m_sources->IsFixedSize())
        {
            sources.resize(d2dEffect->GetInputCount());
        }

        for (unsigned i = 0; i < sources.size(); i++)
        {
            ComPtr<ID2D1Image> d2dInput;
            d2dEffect->GetInput(i, &d2dInput);

            if (d2dInput)
                sources[i] = ResourceManager::GetOrCreate<IGraphicsEffectSource>(device, d2dInput.Get());
            else
                sources[i].Reset();
        }
    }


    void CanvasEffect::InitializePropertiesFromD2D()
    {
        auto& d2dEffect = ResourceWrapper::GetResource();

        for (unsigned i = 0; i < m_properties.size(); ++i)
        {
            switch (d2dEffect->GetType(i))
            {
                case D2D1_PROPERTY_TYPE_BOOL:
                {
                    BOOL value = d2dEffect->GetValue<BOOL>(i);
                    m_properties[i] = CreateProperty(m_propertyValueFactory.Get(), static_cast<boolean>(value));
                }
                break;

            case D2D1_PROPERTY_TYPE_INT32:
                {
                    INT32 value = d2dEffect->GetValue<INT32>(i);
                    m_properties[i] = CreateProperty(m_propertyValueFactory.Get(), value);
                }
                break;

            case D2D1_PROPERTY_TYPE_UINT32:
            case D2D1_PROPERTY_TYPE_ENUM:
                {
                    UINT32 value = d2dEffect->GetValue<UINT32>(i);
                    m_properties[i] = CreateProperty(m_propertyValueFactory.Get(), value);
                }
                break;

            case D2D1_PROPERTY_TYPE_FLOAT:
                {
                    float value = d2dEffect->GetValue<float>(i);
                    m_properties[i] = CreateProperty(m_propertyValueFactory.Get(), value);
                }
                break;

            case D2D1_PROPERTY_TYPE_VECTOR2:
            case D2D1_PROPERTY_TYPE_VECTOR3:
            case D2D1_PROPERTY_TYPE_VECTOR4:
            case D2D1_PROPERTY_TYPE_MATRIX_3X2:
            case D2D1_PROPERTY_TYPE_MATRIX_4X4:
            case D2D1_PROPERTY_TYPE_MATRIX_5X4:
            case D2D1_PROPERTY_TYPE_BLOB:
                {
                    unsigned sizeInBytes = d2dEffect->GetValueSize(i);
                    unsigned sizeInFloats = sizeInBytes / sizeof(float);

                    std::vector<BYTE> value(sizeInBytes);
                    d2dEffect->GetValue(i, value.data(), sizeInBytes);

                    m_properties[i] = CreateProperty(m_propertyValueFactory.Get(), sizeInFloats, reinterpret_cast<float*>(value.data()));
                }
                break;

            default:
                ThrowHR(E_NOTIMPL);
            }
        }
    }


    void CanvasEffect::ThrowIfClosed()
    {
        if (m_closed)
        {
            ThrowHR(RO_E_CLOSED);
        }
    }

}}}}}
