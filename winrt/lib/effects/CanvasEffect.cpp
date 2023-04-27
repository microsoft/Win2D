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

            ComPtr<ID2D1Factory> effectFactory;
            As<ID2D1Image>(effect)->GetFactory(&effectFactory);

            ComPtr<ID2D1Factory> d2dDeviceFactory;
            d2dDevice->GetFactory(&d2dDeviceFactory);

            // Note: there is a small chance of constructing a CanvasEffect object that is in an invalid state
            // when using this constructor indirectly from the interop APIs exposed in the public headers. For
            // a more detailed explanation and a full example, see https://github.com/microsoft/Win2D/issues/913.
            // Here we're validating the D2D factories from the wrapped effect and the device to try to minimize
            // the chances of this happening. We can't be 100% sure since there is no way with the existing D2D
            // APIs to fully determine whether two resources are "compatible", short of just trying to draw with
            // both of them to see if that fails. But, comparing factories is at least a nice additional check
            // to perform, and due to the fact that Win2D creates a new D2D factory per CanvasDevice, this should
            // already cover most scenarios where this might potentially happen, so that's good enough.
            if (!IsSameInstance(effectFactory.Get(), d2dDeviceFactory.Get()))
            {
                ThrowHR(D2DERR_WRONG_FACTORY, Strings::ResourceManagerMismatchedFactoryForEffectAndDevice);
            }

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

        // As a last resort, let's try to see if there is an external factory for this effect.
        if (auto effectFactory = ResourceManager::TryGetEffectFactory(effectId))
        {
            ComPtr<IInspectable> externalResult;

            // First create the wrapper in a local target. We need to validate it first
            // before returning it, and we don't want invalid objects to bubble up in
            // case they fail validation after this call for whatever reason.
            ThrowIfFailed(effectFactory->CreateWrapper(device, d2dEffect.Get(), dpi, externalResult.GetAddressOf()));

            // If the returned object is null, clearly the factory isn't working correctly.
            // Realistically speaking this should never happen, but the extra check is here
            // mostly for correctness given that As<T> doesn't check the input for null.
            if (!externalResult)
            {
                ThrowHR(E_UNEXPECTED);
            }

            // Additional validation step for external factories: just like with RegisterWrapper,
            // the returned wrapper has to be an ICanvasImage, it can't just be some random object.
            As<ICanvasImage>(externalResult);

            // The object retrieved from the external factory is valid: copy it to the result.
            ThrowIfFailed(externalResult.As(result));

            return true;
        }

        // Unrecognized effect CLSID.
        return false;
    }

    bool CanvasEffect::IsWin2DEffectId(REFIID effectId)
    {
        // Check the generated effect wrappers first
        for (auto effectMaker = m_effectMakers; effectMaker->second; effectMaker++)
        {
            if (IsEqualGUID(effectMaker->first, effectId))
            {
                return true;
            }
        }

        // Check PixelShaderEffect specifically next
        return IsEqualGUID(effectId, CLSID_PixelShaderEffect);
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
    // ICanvasImageInterop
    //

    IFACEMETHODIMP CanvasEffect::GetDevice(ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type)
    {
        return ExceptionBoundary([&]
            {
                CheckAndClearOutPointer(device);
                CheckInPointer(type);

                *type = WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED;

                ThrowIfClosed();

                auto realizationDevice = RealizationDevice();

                // RealizationDevice() returns an ICanvasDevice* (not a ComPtr<ICanvasDevice>), so we can
                // just check if it's not null and forward to QueryInterface if that's the case. If instead
                // the device is null, CheckAndClearOutPointer will have already set the argument to null.
                if (realizationDevice)
                {
                    ThrowIfFailed(realizationDevice->QueryInterface(IID_PPV_ARGS(device)));
                }

                // The returned device is the current realization device, and can change
                *type = WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE;
            });
    }

    //
    // ICanvasImageInternal
    //

    ComPtr<ID2D1Image> CanvasEffect::GetD2DImage(ICanvasDevice* device, ID2D1DeviceContext* deviceContext, WIN2D_GET_D2D_IMAGE_FLAGS flags, float targetDpi, float* realizedDpi)
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
        if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT) != WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
        {
            if (TargetIsCommandList(deviceContext))
            {
                // Command lists are DPI independent, so we always
                // need to insert DPI compensation when drawing to them.
                flags |= WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION;
            }
            else
            {
                // If not drawing to a command list, read DPI from the destination device context.
                targetDpi = GetDpi(deviceContext);
            }

            flags &= ~WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT;
        }

        // If this is a DPI compensation effect, we no longer need to insert any further compensation.
        if (IsEqualGUID(m_effectId, CLSID_D2D1DpiCompensation))
        {
            flags |= WIN2D_GET_D2D_IMAGE_FLAGS_NEVER_INSERT_DPI_COMPENSATION;
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
        else if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_MINIMAL_REALIZATION) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
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

                WIN2D_GET_D2D_IMAGE_FLAGS flags = WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS;

                // If the caller did not specify target DPI, we always need to insert DPI compensation
                // (same as when using effects with DPI independent contexts such as command lists).
                if (dpi <= 0)
                {
                    flags |= WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION;
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
        WIN2D_GET_D2D_IMAGE_FLAGS m_flags;
        float m_dpi;

    public:
        EffectRealizationContext(ICanvasResourceCreatorWithDpi* resourceCreator)
            : m_flags(WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS)
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
                    m_flags |= WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION;
                }
            }
            else
            {
                // Get a resource creation context from the device.
                m_deviceContext = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();
            }
        }

        static HRESULT InvalidateSourceRectangle(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            IUnknown* image,
            uint32_t sourceIndex,
            Rect const* invalidRectangle)
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(image);
                    CheckInPointer(invalidRectangle);

                    EffectRealizationContext realizationContext(resourceCreator);

                    auto d2dEffect = realizationContext.RealizeEffect(image);

                    if (sourceIndex >= d2dEffect->GetInputCount())
                        ThrowHR(E_INVALIDARG);

                    auto d2dInvalidRectangle = ToD2DRect(*invalidRectangle);

                    ThrowIfFailed(realizationContext->InvalidateEffectInputRectangle(d2dEffect.Get(), sourceIndex, &d2dInvalidRectangle));
                });
        }

        static HRESULT GetInvalidRectangles(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            IUnknown* image,
            uint32_t* valueCount,
            Rect** valueElements)
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(image);
                    CheckInPointer(valueCount);
                    CheckAndClearOutPointer(valueElements);

                    EffectRealizationContext realizationContext(resourceCreator);

                    auto d2dEffect = realizationContext.RealizeEffect(image);

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

        // This method explicitly does not return an HRESULT, but instead just writes into a target buffer and throws if
        // an error is encountered, requiring callers to use ExceptionBoundary themselves. This is different than the two
        // APIs above, and it is done to give callers extra flexibility. Specifically, allowing them to pass an existing
        // buffer (which can easily be done given the number of result is known, as it's just the number of source effects)
        // means they can avoid the round-trip to a CoTaskMem-allocated buffer when they only want to fetch a single rectangle.
        // Instead, they can just pass a buffer to a value on the stack and have this method write the result there. If instead
        // they do want to return a COM array to callers, they can just preallocate the temporary D2D1_RECT_F vector and then
        // convert that to the COM array to return on their own.
        static void GetRequiredSourceRectangles(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            IUnknown* image,
            Rect const* outputRectangle,
            uint32_t sourceEffectCount,
            ICanvasEffect* const* sourceEffects,
            uint32_t sourceIndexCount,
            uint32_t const* sourceIndices,
            uint32_t sourceBoundsCount,
            Rect const* sourceBounds,
            uint32_t valueCount,
            D2D1_RECT_F* valueElements)
        {
            CheckInPointer(image);
            CheckInPointer(outputRectangle);
            CheckInPointer(sourceEffects);
            CheckInPointer(sourceIndices);
            CheckInPointer(sourceBounds);
            CheckInPointer(valueElements);

            // All three source arrays must be the same size.
            if (sourceEffectCount != sourceIndexCount ||
                sourceEffectCount != sourceBoundsCount ||
                sourceEffectCount != valueCount)
            {
                ThrowHR(E_INVALIDARG);
            }

            EffectRealizationContext realizationContext(resourceCreator);

            auto d2dEffect = realizationContext.RealizeEffect(image);

            auto d2dOutputRectangle = ToD2DRect(*outputRectangle);

            // Convert parameter data to an array of D2D1_EFFECT_INPUT_DESCRIPTION structs.
            std::vector<D2D1_EFFECT_INPUT_DESCRIPTION> inputDescriptions;
            std::vector<ComPtr<ID2D1Effect>> keepAliveReferences;

            inputDescriptions.reserve(sourceEffectCount);
            keepAliveReferences.reserve(sourceEffectCount);

            for (uint32_t i = 0; i < sourceEffectCount; i++)
            {
                CheckInPointer(sourceEffects[i]);

                auto d2dSourceEffect = realizationContext.RealizeEffect(sourceEffects[i]);

                if (sourceIndices[i] >= d2dSourceEffect->GetInputCount())
                    ThrowHR(E_INVALIDARG);

                inputDescriptions.push_back(D2D1_EFFECT_INPUT_DESCRIPTION{ d2dSourceEffect.Get(), sourceIndices[i], ToD2DRect(sourceBounds[i]) });

                keepAliveReferences.push_back(std::move(d2dSourceEffect));
            }

            // Query the input rectangles.
            ThrowIfFailed(realizationContext->GetEffectRequiredInputRectangles(d2dEffect.Get(), &d2dOutputRectangle, inputDescriptions.data(), valueElements, sourceEffectCount));
        }

    private:
        ComPtr<ID2D1Effect> RealizeEffect(IUnknown* effect)
        {
            auto realizedEffect = ICanvasImageInternal::GetD2DImageFromInternalOrInteropSource(effect, m_device.Get(), m_deviceContext.Get(), m_flags, m_dpi);
            return As<ID2D1Effect>(realizedEffect);
        }

        ID2D1DeviceContext1* operator->()
        {
            return m_deviceContext.Get();
        }
    };


    IFACEMETHODIMP CanvasEffect::InvalidateSourceRectangle(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t sourceIndex, Rect invalidRectangle)
    {
        return EffectRealizationContext::InvalidateSourceRectangle(resourceCreator, reinterpret_cast<IUnknown*>(this), sourceIndex, &invalidRectangle);
    }


    IFACEMETHODIMP CanvasEffect::GetInvalidRectangles(ICanvasResourceCreatorWithDpi* resourceCreator, uint32_t* valueCount, Rect** valueElements)
    {
        return EffectRealizationContext::GetInvalidRectangles(resourceCreator, reinterpret_cast<IUnknown*>(this), valueCount, valueElements);
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
                // For this method and the ones below, we need to explicitly check the additional
                // parameters these WinRT APIs expose (namely, the returned value pointer here, and
                // the returned value array pointer in GetRequiredSourceRectangles below), as those
                // two results are not directly computed by the shared stub. We can check that the
                // input pointers are valid before invoking that stub, as all invalid arguments just
                // throw E_INVALIDARG in case of failure, so the exact order they're checked isn't
                // important. That is, checking the parameters "out of order" is not observable.
                CheckInPointer(value);

                D2D1_RECT_F result;

                EffectRealizationContext::GetRequiredSourceRectangles(
                    resourceCreator,
                    reinterpret_cast<IUnknown*>(this),
                    &outputRectangle,
                    1,
                    &sourceEffect,
                    1,
                    &sourceIndex,
                    1,
                    &sourceBounds,
                    1,
                    &result);

                *value = FromD2DRect(result);
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
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                std::vector<D2D1_RECT_F> result(sourceEffectCount);

                EffectRealizationContext::GetRequiredSourceRectangles(
                    resourceCreator,
                    reinterpret_cast<IUnknown*>(this),
                    &outputRectangle,
                    sourceEffectCount,
                    sourceEffects,
                    sourceIndexCount,
                    sourceIndices,
                    sourceBoundsCount,
                    sourceBounds,
                    sourceEffectCount,
                    result.data());

                auto resultArray = TransformToComArray<Rect>(result.begin(), result.end(), FromD2DRect);

                resultArray.Detach(valueCount, valueElements);
            });
    }

    //
    // Implementation of the public exports to support ICanvasEffect APIs for ICanvasImageInterop
    //

#if defined(ARCH_X86)
#pragma comment(linker, "/export:InvalidateSourceRectangleForICanvasImageInterop=_InvalidateSourceRectangleForICanvasImageInterop@16")
#endif
    WIN2DAPI InvalidateSourceRectangleForICanvasImageInterop(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        ICanvasImageInterop* image,
        uint32_t sourceIndex,
        Rect const* invalidRectangle) noexcept
    {
        return EffectRealizationContext::InvalidateSourceRectangle(resourceCreator, image, sourceIndex, invalidRectangle);
    }

#if defined(ARCH_X86)
#pragma comment(linker, "/export:GetInvalidRectanglesForICanvasImageInterop=_GetInvalidRectanglesForICanvasImageInterop@16")
#endif
    WIN2DAPI GetInvalidRectanglesForICanvasImageInterop(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        ICanvasImageInterop* image,
        uint32_t* valueCount,
        Rect** valueElements) noexcept
    {
        return EffectRealizationContext::GetInvalidRectangles(resourceCreator, image, valueCount, valueElements);
    }

#if defined(ARCH_X86)
#pragma comment(linker, "/export:GetRequiredSourceRectanglesForICanvasImageInterop=_GetRequiredSourceRectanglesForICanvasImageInterop@44")
#endif
    WIN2DAPI GetRequiredSourceRectanglesForICanvasImageInterop(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        ICanvasImageInterop* image,
        Rect const* outputRectangle,
        uint32_t sourceEffectCount,
        ICanvasEffect* const* sourceEffects,
        uint32_t sourceIndexCount,
        uint32_t const* sourceIndices,
        uint32_t sourceBoundsCount,
        Rect const* sourceBounds,
        uint32_t valueCount,
        Rect* valueElements) noexcept
    {
        return ExceptionBoundary(
            [&]
            {
                // Same preemptive check for the output pointer, see notes in GetRequiredSourceRectangle above
                CheckInPointer(valueElements);

                std::vector<D2D1_RECT_F> result(valueCount);

                // This API explicitly does not return a COM array, but instead allows callers to have it
                // directly write the results into a buffer they own. See notes in EffectRealizationContext.
                EffectRealizationContext::GetRequiredSourceRectangles(
                    resourceCreator,
                    image,
                    outputRectangle,
                    sourceEffectCount,
                    sourceEffects,
                    sourceIndexCount,
                    sourceIndices,
                    sourceBoundsCount,
                    sourceBounds,
                    valueCount,
                    result.data());

                std::transform(result.begin(), result.end(), valueElements, FromD2DRect);
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
    const WIN2D_GET_D2D_IMAGE_FLAGS SetSourceFlags = WIN2D_GET_D2D_IMAGE_FLAGS_MINIMAL_REALIZATION |
                                                     WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS |
                                                     WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE;


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


    bool CanvasEffect::ApplyDpiCompensation(unsigned int index, ComPtr<ID2D1Image>& inputImage, float inputDpi, WIN2D_GET_D2D_IMAGE_FLAGS flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        auto& dpiCompensator = m_sources[index].DpiCompensator;

        bool hasDpiCompensation = (dpiCompensator != nullptr);
        bool needsDpiCompensation;

        if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_MINIMAL_REALIZATION) != WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
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

            bool neverCompensate  = (flags & WIN2D_GET_D2D_IMAGE_FLAGS_NEVER_INSERT_DPI_COMPENSATION)  != WIN2D_GET_D2D_IMAGE_FLAGS_NONE;
            bool alwaysCompensate = (flags & WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION) != WIN2D_GET_D2D_IMAGE_FLAGS_NONE;

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

    
    void CanvasEffect::RefreshInputs(WIN2D_GET_D2D_IMAGE_FLAGS flags, float targetDpi, ID2D1DeviceContext* deviceContext)
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
                if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
                    ThrowFormattedMessage(E_INVALIDARG, Strings::EffectNullSource, i);
            }
            else
            {
                // Get the underlying D2D interface. This call recurses through the effect graph.
                float realizedDpi;
                auto realizedSource = ICanvasImageInternal::GetD2DImageFromInternalOrInteropSource(source.Get(), RealizationDevice(), deviceContext, flags, targetDpi, &realizedDpi);

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


    bool CanvasEffect::SetD2DInput(ID2D1Effect* d2dEffect, unsigned int index, IGraphicsEffectSource* source, WIN2D_GET_D2D_IMAGE_FLAGS flags, float targetDpi, ID2D1DeviceContext* deviceContext)
    {
        ComPtr<ID2D1Image> realizedSource;
        float realizedDpi = 0;

        if (source)
        {
            // Check if the specified source is an ICanvasImage. There are two possible scenarios to
            // handle: ICanvasImageInternal (a Win2D effect) or ICanvasImageInterop (an external effect).
            ComPtr<ICanvasImageInternal> internalSource;
            ComPtr<ICanvasImageInterop> interopSource;
            HRESULT hr = source->QueryInterface(IID_PPV_ARGS(&internalSource));

            if (FAILED(hr))
            {
                // If QueryInterface failed in any way other than E_NOINTERFACE, we just rethrow.
                if (hr != E_NOINTERFACE)
                    ThrowHR(hr);

                // If the input source is not an ICanvasImage, we now check to see if it's an ICanvasImageInterop. This is
                // the only case other than ICanvasImage where setting the source is valid and we don't need to unrealize.
                hr = source->QueryInterface(IID_PPV_ARGS(&interopSource));

                if (FAILED(hr))
                {
                    if (hr != E_NOINTERFACE)
                        ThrowHR(hr);

                    if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
                        ThrowFormattedMessage(E_NOINTERFACE, Strings::EffectWrongSourceType, index);

                    // If the source is not an ICanvasImage (eg. setting a Windows.UI.Composition resource), we must unrealize.
                    Unrealize(index);
                    m_sources[index] = source;
                    return false;
                }
            }

            ComPtr<ICanvasDevice> sourceDevice;
            bool sourceDeviceMustBeExactMatch;

            if (internalSource)
            {
                // If the specified source is bound to a creation device, validate that this matches the one we are realized on.
                // A "creation device" is a canvas device that is used to create the native resources in a given canvas image
                // implementation, such that the canvas device and the canvas image are bound for the entire lifetime of the
                // canvas image itself. This applies to eg. CanvasBitmap, which directly wraps some native memory allocated on
                // a specific device. In these cases, if the associated device doesn't match the one this effect is currently
                // realized on, that is an error and the source is invalid.
                // 
                // Note: this path is only taken if the source is an ICanvasImageInternal, as the ICanvasResourceWrapperWithDevice
                // interface is internal. That is, if the source is an external effect, there is no way it could implement this.
                auto sourceWithDevice = MaybeAs<ICanvasResourceWrapperWithDevice>(source);

                if (sourceWithDevice)
                {
                    // If the input is an ICanvasResourceWrapperWithDevice, get the current device from there.
                    ThrowIfFailed(sourceWithDevice->get_Device(&sourceDevice));

                    // As mentioned above, if the source is bound to a creation device, it must match the realization device.
                    sourceDeviceMustBeExactMatch = true;
                }
                else
                {
                    // Otherwise, the source can be unrealized if it's currently using another device (eg. it's a CanvasEffect).
                    // As such, its underlying device could be null or a different one, and both cases are valid here.
                    sourceDeviceMustBeExactMatch = false;
                }
            }
            else
            {
                WIN2D_GET_DEVICE_ASSOCIATION_TYPE deviceInfo;

                // Otherwise (the source is an ICanvasImageInterop), get the device from there. The device returned here will
                // depend on the specific implementation of this external effect. That is, it could be null for an unrealized
                // effect, it could be null for a lazily initialized device-bound resource, or it could be an already instantiated
                // resource tied to a specific device. We can check that via the returned association type value.
                ThrowIfFailed(interopSource->GetDevice(&sourceDevice, &deviceInfo));

                // The only case where the device has to be an exact match is if the device is an owning device for the canvas image.
                // In all other cases (including when the device is null), we can proceed to resolve the effects graph normally.
                sourceDeviceMustBeExactMatch = deviceInfo == WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE;
            }

            // If required by the source, as described above, validate that the current realization device matches the source associated device.
            if (sourceDeviceMustBeExactMatch && !IsSameInstance(RealizationDevice(), sourceDevice.Get()))
            {
                if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
                    ThrowFormattedMessage(E_INVALIDARG, Strings::EffectWrongDevice, index);

                // If the source is on a different device, we must unrealize.
                Unrealize(index);
                m_sources[index] = source;
                return false;
            }

            // Get the underlying D2D interface. This call recurses through the effect graph.
            if (internalSource)
            {
                realizedSource = internalSource->GetD2DImage(RealizationDevice(), deviceContext, flags, targetDpi, &realizedDpi);
            }
            else
            {
                hr = interopSource->GetD2DImage(RealizationDevice(), deviceContext, flags, targetDpi, &realizedDpi, &realizedSource);

                if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
                    ThrowIfFailed(hr);
            }

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
            if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
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


    bool CanvasEffect::Realize(WIN2D_GET_D2D_IMAGE_FLAGS flags, float targetDpi, ID2D1DeviceContext* deviceContext)
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
