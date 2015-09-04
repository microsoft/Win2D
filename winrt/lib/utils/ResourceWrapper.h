// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // ResourceWrapper is a helper for implementing WinRT types that are
    // conceptually wrappers around a native resource.
    //

#define RESOURCE_WRAPPER_RUNTIME_CLASS(TResource, TWrapper, TWrapperInterface, ...)         \
    public RuntimeClass<                                                                    \
        RuntimeClassFlags<WinRtClassicComMix>,                                              \
        TWrapperInterface,                                                                  \
        ChainInterfaces<                                                                    \
            MixIn<TWrapper, ResourceWrapper<TResource, TWrapper>>,                          \
            ABI::Windows::Foundation::IClosable,                                            \
            CloakedIid<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>>,    \
        __VA_ARGS__>,                                                                       \
    public ResourceWrapper<TResource, TWrapper>


    template<typename TResource, typename TWrapper>
    class ResourceWrapper : public ABI::Windows::Foundation::IClosable, 
                            public ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative,
                            private LifespanTracker<TWrapper>
    {
        ClosablePtr<TResource> m_resource;

    protected:
        ResourceWrapper(TResource* resource)
            : m_resource(resource)
        {
            if (resource)
            {
                ResourceManager::Add(resource, AsWeak(static_cast<TWrapper*>(this)));
            }
        }

        virtual ~ResourceWrapper()
        {
            Close();
        }

        void ReleaseResource()
        {
            if (m_resource)
            {
                auto resource = m_resource.Close();

                ResourceManager::Remove(resource.Get());
            }
        }

        void SetResource(TResource* resource)
        {
            ReleaseResource();

            if (resource)
            {
                m_resource = resource;

                ResourceManager::Add(resource, AsWeak(static_cast<TWrapper*>(this)));
            }
        }

    public:
        ComPtr<TResource> const& GetResource()
        {
            return m_resource.EnsureNotClosed();
        }

        bool HasResource()
        {
            return (bool)m_resource;
        }

        //
        // IClosable
        //

        IFACEMETHODIMP Close() override
        {
            return ExceptionBoundary(
                [&]
                {
                    ReleaseResource();
                });
        }

        //
        // ICanvasResourceWrapperNative
        //

        IFACEMETHODIMP GetResource(ICanvasDevice* device, float dpi, REFIID iid, void** outResource) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(outResource);

                    auto& resource = GetResource();

                    // Validation methods are overloaded to provide compile time dispatch, so they only actually
                    // bother to check anything if TWrapper implements ICanvasResourceWrapperWith(Device|Dpi).
                    auto wrapper = static_cast<TWrapper*>(this);

                    ValidateDevice(wrapper, device);
                    ValidateDpi(wrapper, dpi);

                    ThrowIfFailed(resource.CopyTo(iid, outResource));
                });
        }

    private:
        // Forward validation requests for types that need them to the ResourceManager.
        static void ValidateDevice(ICanvasResourceWrapperWithDevice* wrapper, ICanvasDevice* device)
        {
            ResourceManager::ValidateDevice(wrapper, device);
        }

        static void ValidateDpi(ICanvasResourceWrapperWithDpi* wrapper, float dpi)
        {
            ResourceManager::ValidateDpi(wrapper, dpi);
        }

        // No-op validation requests for other types.
        static void ValidateDevice(void*, ICanvasDevice*) { }
        static void ValidateDpi(void*, float) { }
    };
}}}}
