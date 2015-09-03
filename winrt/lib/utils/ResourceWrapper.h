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

    public:
        ResourceWrapper(TResource* resource)
            : m_resource(resource)
        {
            CheckInPointer(resource);

            ResourceManager::Add(resource, AsWeak(static_cast<TWrapper*>(this)));
        }

        virtual ~ResourceWrapper()
        {
            Close();
        }

        ComPtr<TResource> const& GetResource()
        {
            return m_resource.EnsureNotClosed();
        }

        //
        // IClosable
        //

        IFACEMETHODIMP Close() override
        {
            return ExceptionBoundary(
                [&]
                {
                    if (m_resource)
                    {
                        auto resource = m_resource.Close();
                        
                        ResourceManager::Remove(resource.Get());
                    }
                });
        }

        //
        // ICanvasResourceWrapperNative
        //

        IFACEMETHODIMP GetResource(REFIID iid, void** outResource) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(outResource);

                    auto& resource = GetResource();
                    ThrowIfFailed(resource.CopyTo(iid, outResource));
                });
        }
    };
}}}}
