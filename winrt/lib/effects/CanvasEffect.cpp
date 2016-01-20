// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "effects/shader/PixelShaderEffect.h"
#include "effects/shader/PixelShaderEffectImpl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    CanvasEffect::CanvasEffect(IID const& effectId, unsigned int propertiesSize, unsigned int sourcesSize, bool isSourcesSizeFixed, ICanvasDevice* device, ID2D1Effect* effect, IInspectable* outerInspectable)
        : ResourceWrapper(effect, outerInspectable)
        , m_closed(false)
        , m_insideGetImage(false)
        , m_effectId(effectId)
        , m_properties(propertiesSize)
        , m_sources(sourcesSize)
        , m_cacheOutput(false)
        , m_bufferPrecision(D2D1_BUFFER_PRECISION_UNKNOWN)
    {
        // If this effect has a variable number of inputs, expose them as an IVector<>.
        if (!isSourcesSizeFixed)
        {
            m_sourcesVector = Make<SourcesVector>(false, this);
            CheckMakeResult(m_sourcesVector);
        }

        // Create propertyValueFactory
        Wrappers::HStringReference stringActivableClassId(RuntimeClass_Windows_Foundation_PropertyValue);
        ThrowIfFailed(GetActivationFactory(stringActivableClassId.Get(), &m_propertyValueFactory));

        // Wrapping around an existing D2D resource?
        if (effect)
        {
            auto d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();

            m_realizationDevice.Set(d2dDevice.Get(), device);
        }
    }


    CanvasEffect::~CanvasEffect()
    {
        // The sources vector could outlive us if a customer is holding onto a separate reference
        // to it. But with us gone, its parent link would be a stale pointer, so we null that out.
        if (m_sourcesVector)
            m_sourcesVector->InternalVector() = nullptr;
    }


    bool CanvasEffect::TryCreateEffect(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result)
    {
        UNREFERENCED_PARAMETER(dpi);

        // Is this resource an effect?
        auto d2dEffect = MaybeAs<ID2D1Effect>(resource);

        if (!d2dEffect)
            return false;

        if (!device)
            ThrowHR(E_INVALIDARG, Strings::ResourceManagerNoDevice);

        // Look up which strongly typed Win2D wrapper class matches the effect CLSID.
        IID effectId = d2dEffect->GetValue<IID>(D2D1_PROPERTY_CLSID);

        for (auto effectMaker = m_effectMakers; effectMaker->second; effectMaker++)
        {
            if (IsEqualGUID(effectMaker->first, effectId))
            {
                // Found it! Create the Win2D wrapper class.
                effectMaker->second(device, d2dEffect.Get(), result);
                return true;
            }
        }

        // If this isn't one of the codegenned effect wrappers, could it be our custom pixel shader effect?
        if (IsEqualGUID(effectId, CLSID_PixelShaderEffect))
        {
            MakeEffect<PixelShaderEffect>(device, d2dEffect.Get(), result);
            return true;
        }

        // Unrecognized effect CLSID.
        return false;
    }


    //
    // ICanvasImage
    //

    IFACEMETHODIMP CanvasEffect::GetBounds(
        ICanvasResourceCreator* resourceCreator,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, resourceCreator, nullptr, bounds);
    }


    IFACEMETHODIMP CanvasEffect::GetBoundsWithTransform(
        ICanvasResourceCreator* resourceCreator,
        Numerics::Matrix3x2 transform,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, resourceCreator, &transform, bounds);
    }


    //
    // ICanvasImageInternal
    //

    ComPtr<ID2D1Image> CanvasEffect::GetD2DImage(ICanvasDevice* device, ID2D1DeviceContext* deviceContext, GetImageFlags flags, float targetDpi, float* realizedDpi)
    {
        ThrowIfClosed();

        // Check for graph cycles
        if (m_insideGetImage)
            ThrowHR(D2DERR_CYCLIC_GRAPH);

        m_insideGetImage = true;
        auto clearFlagWarden = MakeScopeWarden([&] { m_insideGetImage = false; });

        // Lock after the cycle detection, because m_mutex is not recursive.
        // Cycle checks don't need to be threadsafe because that's just a developer error.
        auto lock = Lock(m_mutex);

        // Process the ReadDpiFromDeviceContext flag.
        if ((flags & GetImageFlags::ReadDpiFromDeviceContext) != GetImageFlags::None)
        {
            if (TargetIsCommandList(deviceContext))
            {
                // Command lists are DPI independent, so we always
                // need to insert DPI compensation when drawing to them.
                flags |= GetImageFlags::AlwaysInsertDpiCompensation;
            }
            else
            {
                // If not drawing to a command list, read DPI from the destination device context.
                targetDpi = GetDpi(deviceContext);
            }

            flags &= ~GetImageFlags::ReadDpiFromDeviceContext;
        }

        // If this is a DPI compensation effect, we no longer need to insert any further compensation.
        if (IsEqualGUID(m_effectId, CLSID_D2D1DpiCompensation))
        {
            flags |= GetImageFlags::NeverInsertDpiCompensation;
        }

        // Check if device is the same as previous device.
        ComPtr<ID2D1Device> d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();

        if (!IsSameInstance(d2dDevice.Get(), m_realizationDevice.GetResource()))
        {
            Unrealize();

            m_realizationDevice.Set(d2dDevice.Get(), device);
        }

        if (!HasResource())
        {
            // Create resource if not created yet.
            if (!Realize(flags, targetDpi, deviceContext))
            {
                return nullptr;
            }
        }
        else if ((flags & GetImageFlags::MinimalRealization) == GetImageFlags::None)
        {
            // Recurse through the effect graph to make sure child nodes are properly realized.
            RefreshInputs(flags, targetDpi, deviceContext);
        }

        if (realizedDpi)
            *realizedDpi = 0;

        return As<ID2D1Image>(GetResource());
    }


    //
    // ICanvasResourceWrapperNative
    //

    IFACEMETHODIMP CanvasEffect::GetNativeResource(ICanvasDevice* device, float dpi, REFIID iid, void** resource)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(resource);
                ThrowIfClosed();

                if (!device)
                    ThrowHR(E_INVALIDARG, Strings::GetResourceNoDevice);

                GetImageFlags flags = GetImageFlags::AllowNullEffectInputs;

                // If the caller did not specify target DPI, we always need to insert DPI compensation
                // (same as when using effects with DPI independent contexts such as command lists).
                if (dpi <= 0)
                {
                    flags |= GetImageFlags::AlwaysInsertDpiCompensation;
                }

                auto realizedEffect = GetD2DImage(device, nullptr, flags, dpi);
                
                ThrowIfFailed(realizedEffect.CopyTo(iid, resource));
            });
    }


    //
    // IClosable
    //

    IFACEMETHODIMP CanvasEffect::Close()
    {
        ReleaseResource();

        m_realizationDevice.Reset();
        m_workaround6146411.Reset();
        m_sources.assign(m_sources.size(), SourceReference());

        if (m_sourcesVector)
            m_sourcesVector->InternalVector() = nullptr;

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
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(count);
                *count = GetSourceCount();
            });
    }


    IFACEMETHODIMP CanvasEffect::GetSource(UINT index, IGraphicsEffectSource** source)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(source);
                GetSource(index).CopyTo(source);
            });
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

                ThrowIfFailed(GetProperty(index).CopyTo(value));
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


    //
    // ICanvasEffect
    //

    IFACEMETHODIMP CanvasEffect::get_CacheOutput(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = Lock(m_mutex);

                // If we are realized, read the latest value from the underlying D2D resource.
                if (auto& d2dEffect = MaybeGetResource())
                {
                    m_cacheOutput = !!d2dEffect->GetValue<BOOL>(D2D1_PROPERTY_CACHED);
                }
        
                *value = m_cacheOutput;
            });
    }


    IFACEMETHODIMP CanvasEffect::put_CacheOutput(boolean value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = Lock(m_mutex);

                m_cacheOutput = value;

                // If we are realized, set the new value through to the underlying D2D resource.
                if (auto& d2dEffect = MaybeGetResource())
                {
                    ThrowIfFailed(d2dEffect->SetValue(D2D1_PROPERTY_CACHED, static_cast<BOOL>(m_cacheOutput)));
                }
            });
    }


    IFACEMETHODIMP CanvasEffect::get_BufferPrecision(IReference<CanvasBufferPrecision>** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);

                auto lock = Lock(m_mutex);

                // If we are realized, read the latest value from the underlying D2D resource.
                if (auto& d2dEffect = MaybeGetResource())
                {
                    m_bufferPrecision = d2dEffect->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION);
                }

                // If the value is not unknown, box it as an IReference.
                // Unknown precision returns null.
                if (m_bufferPrecision != D2D1_BUFFER_PRECISION_UNKNOWN)
                {
                    auto nullable = Make<Nullable<CanvasBufferPrecision>>(FromD2DBufferPrecision(m_bufferPrecision));
                    CheckMakeResult(nullable);

                    ThrowIfFailed(nullable.CopyTo(value));
                }
            });
    }


    IFACEMETHODIMP CanvasEffect::put_BufferPrecision(IReference<CanvasBufferPrecision>* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = Lock(m_mutex);

                if (value)
                {
                    // Convert non-null values from Win2D to D2D format.
                    CanvasBufferPrecision bufferPrecision;
                    ThrowIfFailed(value->get_Value(&bufferPrecision));

                    m_bufferPrecision = ToD2DBufferPrecision(bufferPrecision);
                }
                else
                {
                    // Null references -> unknown.
                    m_bufferPrecision = D2D1_BUFFER_PRECISION_UNKNOWN;
                }

                // If we are realized, set the new value through to the underlying D2D resource.
                if (auto& d2dEffect = MaybeGetResource())
                {
                    ThrowIfFailed(d2dEffect->SetValue(D2D1_PROPERTY_PRECISION, m_bufferPrecision));
                }
            });
    }


    // Helper used by InvalidateInputRectangle, GetInvalidRectangles, and GetRequiredInputRectangles.
    // Given any ICanvasResourceCreatorWithDpi, looks up a device context that can be used to realize effects.
    class EffectRealizationContext
    {
        DeviceContextLease m_deviceContext;
        ComPtr<ICanvasDevice> m_device;
        GetImageFlags m_flags;
        float m_dpi;

    public:
        EffectRealizationContext(ICanvasResourceCreatorWithDpi* resourceCreator)
            : m_flags(GetImageFlags::AllowNullEffectInputs)
        {
            CheckInPointer(resourceCreator);

            // Look up device and DPI from the resource creator interface.
            ThrowIfFailed(As<ICanvasResourceCreator>(resourceCreator)->get_Device(&m_device));
            ThrowIfFailed(resourceCreator->get_Dpi(&m_dpi));

            if (auto drawingSession = MaybeAs<ICanvasDrawingSession>(resourceCreator))
            {
                // If the specified resource creator is a CanvasDrawingSession, we can use that directly.
                m_deviceContext = DeviceContextLease(GetWrappedResource<ID2D1DeviceContext1>(drawingSession));

                // Special case for command lists, which always require DPI compensation.
                if (TargetIsCommandList(m_deviceContext.Get()))
                {
                    m_flags |= GetImageFlags::AlwaysInsertDpiCompensation;
                }
            }
            else
            {
                // Get a resource creation context from the device.
                m_deviceContext = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();
            }
        }

        ComPtr<ID2D1Effect> RealizeEffect(ICanvasEffect* effect)
        {
            auto imageInternal = As<ICanvasImageInternal>(effect);
            auto realizedEffect = imageInternal->GetD2DImage(m_device.Get(), m_deviceContext.Get(), m_flags, m_dpi);
            return As<ID2D1Effect>(realizedEffect);
        }

        ID2D1DeviceContext1* operator->()
        {
            return m_deviceContext.Get();
        }
    };


    IFACEMETHODIMP CanvasEffect::InvalidateSourceRectangle(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t sourceIndex, Rect invalidRectangle)
    {
        return ExceptionBoundary(
            [&]
            {
                EffectRealizationContext realizationContext(resourceCreator);

                auto d2dEffect = realizationContext.RealizeEffect(this);

                if (sourceIndex >= d2dEffect->GetInputCount())
                    ThrowHR(E_INVALIDARG);

                auto d2dInvalidRectangle = ToD2DRect(invalidRectangle);

                ThrowIfFailed(realizationContext->InvalidateEffectInputRectangle(d2dEffect.Get(), sourceIndex, &d2dInvalidRectangle));
            });
    }


    IFACEMETHODIMP CanvasEffect::GetInvalidRectangles(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t* valueCount, Rect** valueElements)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                EffectRealizationContext realizationContext(resourceCreator);

                auto d2dEffect = realizationContext.RealizeEffect(this);

                // Query the count.
                UINT32 invalidRectangleCount;

                ThrowIfFailed(realizationContext->GetEffectInvalidRectangleCount(d2dEffect.Get(), &invalidRectangleCount));

                // Query the rectangles.
                std::vector<D2D1_RECT_F> result(invalidRectangleCount);

                ThrowIfFailed(realizationContext->GetEffectInvalidRectangles(d2dEffect.Get(), result.data(), invalidRectangleCount));

                // Return the array.
                auto resultArray = TransformToComArray<Rect>(result.begin(), result.end(), FromD2DRect);
                
                resultArray.Detach(valueCount, valueElements);
            });
    }


    static std::vector<D2D1_RECT_F> GetRequiredSourceRectanglesImpl(
        ICanvasEffect* effect,
        ICanvasResourceCreatorWithDpi* resourceCreator,
        Rect outputRectangle,
        uint32_t sourceCount,
        ICanvasEffect** sourceEffects,
        uint32_t* sourceIndices,
        Rect* sourceBounds)
    {
        EffectRealizationContext realizationContext(resourceCreator);

        auto d2dEffect = realizationContext.RealizeEffect(effect);

        auto d2dOutputRectangle = ToD2DRect(outputRectangle);

        // Convert parameter data to an array of D2D1_EFFECT_INPUT_DESCRIPTION structs.
        std::vector<D2D1_EFFECT_INPUT_DESCRIPTION> inputDescriptions;
        std::vector<ComPtr<ID2D1Effect>> keepAliveReferences;

        inputDescriptions.reserve(sourceCount);
        keepAliveReferences.reserve(sourceCount);

        for (uint32_t i = 0; i < sourceCount; i++)
        {
            CheckInPointer(sourceEffects[i]);

            auto d2dSourceEffect = realizationContext.RealizeEffect(sourceEffects[i]);

            if (sourceIndices[i] >= d2dSourceEffect->GetInputCount())
                ThrowHR(E_INVALIDARG);

            inputDescriptions.push_back(D2D1_EFFECT_INPUT_DESCRIPTION{ d2dSourceEffect.Get(), sourceIndices[i], ToD2DRect(sourceBounds[i]) });

            keepAliveReferences.push_back(std::move(d2dSourceEffect));
        }

        // Query the input rectangles.
        std::vector<D2D1_RECT_F> result(sourceCount);

        ThrowIfFailed(realizationContext->GetEffectRequiredInputRectangles(d2dEffect.Get(), &d2dOutputRectangle, inputDescriptions.data(), result.data(), sourceCount));

        return result;
    }


    IFACEMETHODIMP CanvasEffect::GetRequiredSourceRectangle(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        Rect outputRectangle,
        ICanvasEffect* sourceEffect,
        uint32_t sourceIndex,
        Rect sourceBounds,
        Rect* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto result = GetRequiredSourceRectanglesImpl(this, resourceCreator, outputRectangle, 1, &sourceEffect, &sourceIndex, &sourceBounds);

                assert(result.size() == 1);

                *value = FromD2DRect(result[0]);
            });
    }


    IFACEMETHODIMP CanvasEffect::GetRequiredSourceRectangles(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        Rect outputRectangle,
        uint32_t sourceEffectCount,
        ICanvasEffect** sourceEffects,
        uint32_t sourceIndexCount,
        uint32_t* sourceIndices,
        uint32_t sourceBoundsCount,
        Rect* sourceBounds,
        uint32_t* valueCount,
        Rect** valueElements)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(sourceEffects);
                CheckInPointer(sourceIndices);
                CheckInPointer(sourceBounds);
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                // All three source arrays must be the same size.
                if (sourceEffectCount != sourceIndexCount ||
                    sourceEffectCount != sourceBoundsCount)
                {
                    ThrowHR(E_INVALIDARG);
                }

                auto result = GetRequiredSourceRectanglesImpl(this, resourceCreator, outputRectangle, sourceEffectCount, sourceEffects, sourceIndices, sourceBounds);

                auto resultArray = TransformToComArray<Rect>(result.begin(), result.end(), FromD2DRect);

                resultArray.Detach(valueCount, valueElements);
            });
    }


    unsigned int CanvasEffect::GetSourceCount()
    {
        auto lock = Lock(m_mutex);

        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, read the count from the underlying D2D resource.
            auto sourceCount = d2dEffect->GetInputCount();

            return sourceCount;
        }
        else
        {
            // If not realized, use our local sources vector.
            return (unsigned)m_sources.size();
        }
    }


    ComPtr<IGraphicsEffectSource> CanvasEffect::GetSource(unsigned int index)
    {
        auto lock = Lock(m_mutex);

        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, read the source image from the underlying D2D resource.
            if (index >= d2dEffect->GetInputCount())
                ThrowHR(E_BOUNDS);

            return GetD2DInput(d2dEffect.Get(), index);
        }
        else
        {
            // If not realized, use our local sources vector.
            if (index >= m_sources.size())
                ThrowHR(E_BOUNDS);

            return m_sources[index].GetWrapper();
        }
    }


    // SetD2DInput options used by SetSource, InsertSource, and AppendSource.
    const GetImageFlags SetSourceFlags = GetImageFlags::MinimalRealization | 
                                         GetImageFlags::AllowNullEffectInputs |
                                         GetImageFlags::UnrealizeOnFailure;


    void CanvasEffect::SetSource(unsigned int index, IGraphicsEffectSource* source)
    {
        auto lock = Lock(m_mutex);

        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, set the source image through to the underlying D2D resource.
            if (index >= d2dEffect->GetInputCount())
                ThrowHR(E_BOUNDS);

            SetD2DInput(d2dEffect.Get(), index, source, SetSourceFlags);
        }
        else
        {
            // If not realized, use our local sources vector.
            if (index >= m_sources.size())
                ThrowHR(E_BOUNDS);

            m_sources[index] = source;
        }
    }


    void CanvasEffect::InsertSource(unsigned int index, IGraphicsEffectSource* source)
    {
        auto lock = Lock(m_mutex);
        
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, insert the new value into the underlying D2D resource.
            auto inputCount = d2dEffect->GetInputCount();

            if (index > inputCount)
                ThrowHR(E_BOUNDS);

            ThrowIfFailed(d2dEffect->SetInputCount(inputCount + 1));

            for (unsigned i = inputCount; i > index; i--)
            {
                ComPtr<ID2D1Image> input;
                d2dEffect->GetInput(i - 1, &input);
                d2dEffect->SetInput(i, input.Get());
            }

            // This vector is not authoritative while realized, but we still do our best to keep it in sync.
            m_sources.resize(inputCount);
            m_sources.insert(m_sources.begin() + index, SourceReference());
    
            SetD2DInput(d2dEffect.Get(), index, source, SetSourceFlags);
        }
        else
        {
            // If not realized, use our local sources vector.
            if (index > m_sources.size())
                ThrowHR(E_BOUNDS);

            m_sources.insert(m_sources.begin() + index, source);
        }
    }


    void CanvasEffect::RemoveSource(unsigned int index)
    {
        auto lock = Lock(m_mutex);
        
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, remove the value from the underlying D2D resource.
            auto inputCount = d2dEffect->GetInputCount();

            if (index >= inputCount)
                ThrowHR(E_BOUNDS);

            if (inputCount == 1)
            {
                // Effects with variable number of inputs don't allow zero of them,
                // so we must unrealize before removing the last element.
                Unrealize(0);
            }
            else
            {
                for (unsigned i = index + 1; i < inputCount; i++)
                {
                    ComPtr<ID2D1Image> input;
                    d2dEffect->GetInput(i, &input);
                    d2dEffect->SetInput(i - 1, input.Get());
                }

                ThrowIfFailed(d2dEffect->SetInputCount(inputCount - 1));
            }

            // This vector is not authoritative while realized, but we still do our best to keep it in sync.
            m_sources.resize(inputCount);
        }
        else
        {
            // If not realized, our local sources vector holds the authoritative size.
            if (index >= m_sources.size())
                ThrowHR(E_BOUNDS);
        }

        // Common to both realized and unrealized paths.
        m_sources.erase(m_sources.begin() + index);
    }


    void CanvasEffect::AppendSource(IGraphicsEffectSource* source)
    {
        auto lock = Lock(m_mutex);
        
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, append the value to the underlying D2D resource.
            auto inputCount = d2dEffect->GetInputCount();

            ThrowIfFailed(d2dEffect->SetInputCount(inputCount + 1));

            SetD2DInput(d2dEffect.Get(), inputCount, source, SetSourceFlags);
        }
        else
        {
            // If not realized, use our local sources vector.
            m_sources.push_back(source);
        }
    }


    void CanvasEffect::ClearSources()
    {
        auto lock = Lock(m_mutex);
        
        // Effects with variable number of inputs don't allow zero of them,
        // so we must unrealize before we can clear the collection.
        Unrealize(0, true);

        m_sources.clear();
    }


    static void ThrowFormattedMessage(HRESULT hr, wchar_t const* message, int i)
    {
        WinStringBuilder formatted;
        formatted.Format(message, i);
        ThrowHR(hr, formatted.Get());
    }


    static void SetEffectInput(ID2D1Effect* effect, unsigned int index, ID2D1Image* image)
    {
        effect->SetInput(index, image);

#ifdef _DEBUG
        // ID2D1Effect::SetInput can fail (eg. if the provided image is on the wrong device) but it returns
        // void, silently putting the effect into an error state that is not detected until later. Win2D should
        // never hit this case, but in case we ever do, it's easier to debug if we fail fast during development.
        ComPtr<ID2D1Image> input;
        effect->GetInput(index, &input);
        assert(IsSameInstance(input.Get(), image));
#endif
    }


    ComPtr<ID2D1Effect> CanvasEffect::CreateD2DEffect(ID2D1DeviceContext* deviceContext, IID const& effectId)
    {
        // If the caller provided a device context, we can use that.
        // Otherwise, we must look up a resource creation device context.
        DeviceContextLease contextLease;

        if (!deviceContext)
        {
            contextLease = As<ICanvasDeviceInternal>(RealizationDevice())->GetResourceCreationDeviceContext();
            deviceContext = contextLease.Get();
        }

        // Create the effect.
        ComPtr<ID2D1Effect> effect;
        ThrowIfFailed(deviceContext->CreateEffect(effectId, &effect));
        return effect;
    }


    bool CanvasEffect::ApplyDpiCompensation(unsigned int index, ComPtr<ID2D1Image>& inputImage, float inputDpi, GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        auto& dpiCompensator = m_sources[index].DpiCompensator;

        bool hasDpiCompensation = (dpiCompensator != nullptr);
        bool needsDpiCompensation;

        if ((flags & GetImageFlags::MinimalRealization) != GetImageFlags::None)
        {
            // In minimal mode, we don't yet know the target DPI. For instance this occurs when setting an
            // effect as the source of an image brush. We'll fix up later when the brush is drawn to a device
            // context, at which point the real DPI is known. Since we don't yet know whether to include
            // compensation, we just preserve the existing state - this avoids repeated recreation during
            // set-draw-set-draw sequences.
            needsDpiCompensation = hasDpiCompensation && (inputDpi != 0);
        }
        else
        {
            // We need DPI compensation if:
            //  - the input has a fixed DPI
            //  - we were not told never to include it
            //  - either we were told to always include it, or the input DPI is different from the target

            bool neverCompensate  = (flags & GetImageFlags::NeverInsertDpiCompensation)  != GetImageFlags::None;
            bool alwaysCompensate = (flags & GetImageFlags::AlwaysInsertDpiCompensation) != GetImageFlags::None;

            needsDpiCompensation = (inputDpi != 0) &&
                                   !neverCompensate &&
                                   (alwaysCompensate || (inputDpi != targetDpi));

            // Early out if we are already in the right state.
            if (needsDpiCompensation == hasDpiCompensation)
                return false;
        }

        if (needsDpiCompensation)
        {
            // Create the D2D1DpiCompensation effect, if we don't have one already.
            if (!dpiCompensator)
            {
                dpiCompensator = CreateD2DEffect(deviceContext, CLSID_D2D1DpiCompensation);

                ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD));
                ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, D2D1_DPICOMPENSATION_INTERPOLATION_MODE_LINEAR));
            }

            // Set our input image as source for the DPI compensation.
            SetEffectInput(dpiCompensator.Get(), 0, inputImage.Get());

            // Set the DPI.
            ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INPUT_DPI, D2D1_VECTOR_2F{ inputDpi, inputDpi }));

            // Substitute our DPI compensation wrapper for the original input image.
            inputImage = As<ID2D1Image>(dpiCompensator);
        }
        else
        {
            dpiCompensator.Reset();
        }

        return true;
    }

    
    void CanvasEffect::RefreshInputs(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        auto& d2dEffect = GetResource();

        m_sources.resize(d2dEffect->GetInputCount());
        
        for (unsigned int i = 0; i < m_sources.size(); ++i)
        {
            auto& sourceInfo = m_sources[i];

            // Look up the WinRT interface representing this input.
            auto source = GetD2DInput(d2dEffect.Get(), i);

            if (!source)
            {
                if ((flags & GetImageFlags::AllowNullEffectInputs) == GetImageFlags::None)
                    ThrowFormattedMessage(E_INVALIDARG, Strings::EffectNullSource, i);
            }
            else
            {
                // Get the underlying D2D interface. This call recurses through the effect graph.
                float realizedDpi;
                auto realizedSource = As<ICanvasImageInternal>(source)->GetD2DImage(RealizationDevice(), deviceContext, flags, targetDpi, &realizedDpi);

                bool resourceChanged = sourceInfo.UpdateResource(realizedSource.Get());

                bool dpiChanged = ApplyDpiCompensation(i, realizedSource, realizedDpi, flags, targetDpi, deviceContext);

                // If the source value has changed, update the D2D effect graph.
                if (resourceChanged || dpiChanged)
                {
                    SetEffectInput(d2dEffect.Get(), i, realizedSource.Get());
                }
            }
        }
    }


    bool CanvasEffect::SetD2DInput(ID2D1Effect* d2dEffect, unsigned int index, IGraphicsEffectSource* source, GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        ComPtr<ID2D1Image> realizedSource;
        float realizedDpi = 0;

        if (source)
        {
            // Make sure the specified source is an ICanvasImage.
            ComPtr<ICanvasImageInternal> internalSource;
            HRESULT hr = source->QueryInterface(IID_PPV_ARGS(&internalSource));

            if (FAILED(hr))
            {
                if (hr != E_NOINTERFACE)
                    ThrowHR(hr);

                if ((flags & GetImageFlags::UnrealizeOnFailure) == GetImageFlags::None)
                    ThrowFormattedMessage(E_NOINTERFACE, Strings::EffectWrongSourceType, index);

                // If the source is not an ICanvasImage (eg. setting a Windows.UI.Composition resource), we must unrealize.
                Unrealize(index);
                m_sources[index] = source;
                return false;
            }

            // If the specified source has an associated device, validate that this matches the one we are realized on.
            auto sourceWithDevice = MaybeAs<ICanvasResourceWrapperWithDevice>(source);

            if (sourceWithDevice)
            {
                ComPtr<ICanvasDevice> sourceDevice;
                ThrowIfFailed(sourceWithDevice->get_Device(&sourceDevice));

                if (!IsSameInstance(RealizationDevice(), sourceDevice.Get()))
                {
                    if ((flags & GetImageFlags::UnrealizeOnFailure) == GetImageFlags::None)
                        ThrowFormattedMessage(E_INVALIDARG, Strings::EffectWrongDevice, index);
                    
                    // If the source is on a different device, we must unrealize.
                    Unrealize(index);
                    m_sources[index] = source;
                    return false;
                }
            }

            // Get the underlying D2D interface. This call recurses through the effect graph.
            realizedSource = internalSource->GetD2DImage(RealizationDevice(), deviceContext, flags, targetDpi, &realizedDpi);

            if (!realizedSource)
            {
                Unrealize(index);
                m_sources[index] = source;
                return false;
            }
        }
        else
        {
            // Source is null.
            if ((flags & GetImageFlags::AllowNullEffectInputs) == GetImageFlags::None)
                ThrowFormattedMessage(E_INVALIDARG, Strings::EffectNullSource, index);
        }

        // Update our local state vector.
        if (m_sources.size() <= index)
            m_sources.resize(index + 1);

        m_sources[index].Set(realizedSource.Get(), source);

        // Update the underlying D2D effect state.
        ApplyDpiCompensation(index, realizedSource, realizedDpi, flags, targetDpi, deviceContext);

        SetEffectInput(d2dEffect, index, realizedSource.Get());

        return true;
    }


    ComPtr<IGraphicsEffectSource> CanvasEffect::GetD2DInput(ID2D1Effect* d2dEffect, unsigned int index)
    {
        // Read the current input from D2D.
        ComPtr<ID2D1Image> input;
        d2dEffect->GetInput(index, &input);

        // Demand-grow our local state vector.
        if (m_sources.size() <= index)
            m_sources.resize(index + 1);

        // If this input had DPI compensation added, skip past that to report the real input image.
        if (m_sources[index].DpiCompensator)
        {
            if (IsSameInstance(input.Get(), m_sources[index].DpiCompensator.Get()))
            {
                m_sources[index].DpiCompensator->GetInput(0, &input);
            }
            else
            {
                m_sources[index].DpiCompensator.Reset();
            }
        }

        // Look up a Win2D wrapper for the D2D input image.
        return m_sources[index].GetOrCreateWrapper(RealizationDevice(), input.Get());
    }


    void CanvasEffect::SetProperty(unsigned int index, IPropertyValue* propertyValue)
    {
        auto lock = Lock(m_mutex);

        assert(index < m_properties.size());

        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, set the property value through to the underlying D2D resource.
            SetD2DProperty(d2dEffect.Get(), index, propertyValue);
        }
        else
        {
            // If we are not realized, directly store the property value.
            m_properties[index] = propertyValue;
        }
    }


    void CanvasEffect::SetD2DProperty(ID2D1Effect* d2dEffect, unsigned int index, IPropertyValue* propertyValue)
    {
        PropertyType propertyType;
        ThrowIfFailed(propertyValue->get_Type(&propertyType));

        switch (propertyType)
        {
        case PropertyType_Boolean:
            {
                boolean value;
                ThrowIfFailed(propertyValue->GetBoolean(&value));
                ThrowIfFailed(d2dEffect->SetValue(index, static_cast<BOOL>(value)));
            }
            break;

        case PropertyType_Int32:
            {
                INT32 value;
                ThrowIfFailed(propertyValue->GetInt32(&value));
                ThrowIfFailed(d2dEffect->SetValue(index, value));
            }
            break;

        case PropertyType_UInt32:
            {
                UINT32 value;
                ThrowIfFailed(propertyValue->GetUInt32(&value));
                ThrowIfFailed(d2dEffect->SetValue(index, value));
            }
            break;

        case PropertyType_Single:
            {
                float value;
                ThrowIfFailed(propertyValue->GetSingle(&value));
                ThrowIfFailed(d2dEffect->SetValue(index, value));
            }
            break;

        case PropertyType_SingleArray:
            {
                ComArray<float> value;
                ThrowIfFailed(propertyValue->GetSingleArray(value.GetAddressOfSize(), value.GetAddressOfData()));
                ThrowIfFailed(d2dEffect->SetValue(index, reinterpret_cast<BYTE*>(value.GetData()), value.GetSize() * sizeof(float)));
            }
            break;

        case PropertyType_InspectableArray:
            {
                ComPtr<IInspectable> wrapper;
                GetValueOfProperty(propertyValue, wrapper.GetAddressOf());

                auto d2dResource = wrapper ? GetWrappedResource<IUnknown>(wrapper, m_realizationDevice.GetWrapper()) : nullptr;

                ThrowIfFailed(d2dEffect->SetValue(index, d2dResource.Get()));

                // Windows has a bug that prevents reading back DESTINATION_COLOR_CONTEXT from a CLSID_D2D1ColorManagement effect.
                // As a partial workaround, we cache this property value in the Win2D wrapper.
                if (IsEqualGUID(m_effectId, CLSID_D2D1ColorManagement) && index == D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT)
                {
                    m_workaround6146411 = d2dResource;
                }
            }
            break;

        default:
            ThrowHR(E_NOTIMPL);
        }
    }


    ComPtr<IPropertyValue> CanvasEffect::GetProperty(unsigned int index)
    {
        auto lock = Lock(m_mutex);

        assert(index < m_properties.size());

        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // If we are realized, read the property value from the underlying D2D resource.
            return GetD2DProperty(d2dEffect.Get(), index);
        }
        else
        {
            // If we are not realized, directly return the property value.
            return m_properties[index];
        }
    }


    ComPtr<IPropertyValue> CanvasEffect::GetD2DProperty(ID2D1Effect* d2dEffect, unsigned int index)
    {
        switch (d2dEffect->GetType(index))
        {
        case D2D1_PROPERTY_TYPE_BOOL:
            {
                BOOL value = d2dEffect->GetValue<BOOL>(index);
                return CreateProperty(m_propertyValueFactory.Get(), static_cast<boolean>(value));
            }
            break;

        case D2D1_PROPERTY_TYPE_INT32:
        case D2D1_PROPERTY_TYPE_UINT32:     // Not a mistake: unsigned DImage properties are exposed in WinRT as signed.
            {
                INT32 value = d2dEffect->GetValue<INT32>(index);
                return CreateProperty(m_propertyValueFactory.Get(), value);
            }
            break;

        case D2D1_PROPERTY_TYPE_ENUM:
            {
                UINT32 value = d2dEffect->GetValue<UINT32>(index);
                return CreateProperty(m_propertyValueFactory.Get(), value);
            }
            break;

        case D2D1_PROPERTY_TYPE_FLOAT:
            {
                float value = d2dEffect->GetValue<float>(index);
                return CreateProperty(m_propertyValueFactory.Get(), value);
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
                unsigned sizeInBytes = d2dEffect->GetValueSize(index);
                unsigned sizeInFloats = sizeInBytes / sizeof(float);

                std::vector<BYTE> value(sizeInBytes);
                ThrowIfFailed(d2dEffect->GetValue(index, value.data(), sizeInBytes));

                return CreateProperty(m_propertyValueFactory.Get(), sizeInFloats, reinterpret_cast<float*>(value.data()));
            }
            break;

        case D2D1_PROPERTY_TYPE_IUNKNOWN:
        case D2D1_PROPERTY_TYPE_COLOR_CONTEXT:
            {
                ComPtr<IUnknown> d2dResource;

                if (IsEqualGUID(m_effectId, CLSID_D2D1ColorManagement) && index == D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT)
                {
                    // Windows has a bug that prevents reading back DESTINATION_COLOR_CONTEXT from a CLSID_D2D1ColorManagement effect.
                    // As a partial workaround, we cache this property value in the Win2D wrapper and return that instead.
                    // This is correct as long as the Win2D wrapper is kept alive, and the property is not changed via D2D interop.
                    d2dResource = m_workaround6146411;
                }
                else
                {
                    d2dResource.Attach(d2dEffect->GetValue<IUnknown*>(index));
                }

                auto wrapper = d2dResource ? ResourceManager::GetOrCreate(m_realizationDevice.GetWrapper(), d2dResource.Get(), 0) : nullptr;

                return CreateProperty(m_propertyValueFactory.Get(), wrapper.Get());
            }
            break;

        default:
            ThrowHR(E_NOTIMPL);
        }
    }


    bool CanvasEffect::Realize(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        assert(!HasResource());

        // Effects with variable number of inputs don't allow zero of them.
        if (m_sourcesVector && m_sources.size() == 0)
        {
            ThrowHR(E_INVALIDARG, Strings::EffectNoSources);
        }

        // Create a new D2D effect instance.
        auto d2dEffect = CreateD2DEffect(deviceContext, m_effectId);

        // Transfer property values from our resource independent m_properties store to the D2D effect.
        for (unsigned i = 0; i < m_properties.size(); ++i)
        {
            SetD2DProperty(d2dEffect.Get(), i, m_properties[i].Get());
        }

        // Also transfer the special properties that are common to all effects (CacheOutput and BufferPrecision).
        if (m_cacheOutput)
            ThrowIfFailed(d2dEffect->SetValue(D2D1_PROPERTY_CACHED, static_cast<BOOL>(true)));

        if (m_bufferPrecision != D2D1_BUFFER_PRECISION_UNKNOWN)
            ThrowIfFailed(d2dEffect->SetValue(D2D1_PROPERTY_PRECISION, m_bufferPrecision));

        // Transfer input images across to the D2D effect.
        ThrowIfFailed(d2dEffect->SetInputCount((unsigned)m_sources.size()));

        for (unsigned i = 0; i < m_sources.size(); ++i)
        {
            if (!SetD2DInput(d2dEffect.Get(), i, m_sources[i].GetWrapper(), flags, targetDpi, deviceContext))
                return false;
        }

        // Wipe m_properties, as the D2D effect is now the One True Source Of Authoritativeness.
        m_properties.assign(m_properties.size(), nullptr);

        // Store the new effect.
        SetResource(d2dEffect.Get());

        return true;
    }


    void CanvasEffect::Unrealize(unsigned int skipSourceIndex, bool skipAllSources)
    {
        auto& d2dEffect = MaybeGetResource();

        if (d2dEffect)
        {
            // Transfer property values from the D2D effect to our resource independent m_properties store.
            for (unsigned i = 0; i < m_properties.size(); ++i)
            {
                m_properties[i] = GetD2DProperty(d2dEffect.Get(), i);
            }

            // Also transfer the special properties that are common to all effects (CacheOutput and BufferPrecision).
            m_cacheOutput = !!d2dEffect->GetValue<BOOL>(D2D1_PROPERTY_CACHED);
            m_bufferPrecision = d2dEffect->GetValue<D2D1_BUFFER_PRECISION>(D2D1_PROPERTY_PRECISION);

            // Read back the list of source images from the D2D effect.
            if (!skipAllSources)
            {
                m_sources.resize(d2dEffect->GetInputCount());

                for (unsigned i = 0; i < m_sources.size(); ++i)
                {
                    if (i != skipSourceIndex)
                    {
                        m_sources[i] = GetD2DInput(d2dEffect.Get(), i).Get();
                    }
                    else
                    {
                        m_sources[i] = SourceReference();
                    }
                }
            }

            // Clear the effect resource.
            ReleaseResource();

            m_workaround6146411.Reset();
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
