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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // ResourceWrapper is a helper for implementing WinRT types that are
    // conceptually wrappers around a native resource.  It works in conjunction
    // with ResourceManager to allow looking up wrappers by resource.
    //
    // See ResourceManager.h for more information.
    //

#define RESOURCE_WRAPPER_RUNTIME_CLASS(TRAITS, ...)         \
    public RuntimeClass<                                    \
        RuntimeClassFlags<WinRtClassicComMix>,              \
        MixIn<TRAITS::wrapper_t, ResourceWrapper<TRAITS>>,  \
        TRAITS::wrapper_interface_t,                        \
        __VA_ARGS__>,                                       \
    public ResourceWrapper<TRAITS>


    template<typename TRAITS>
    class ResourceWrapper : public Implements<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Windows::Foundation::IClosable, 
        CloakedIid<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>>
    {
        std::shared_ptr<typename TRAITS::manager_t> m_manager;
        ClosablePtr<typename TRAITS::resource_t> m_resource;

    public:
        typedef typename TRAITS::resource_t resource_t;
        typedef typename TRAITS::wrapper_t wrapper_t;
        typedef typename TRAITS::wrapper_interface_t wrapper_interface_t;
        typedef std::function<void(resource_t*)> remover_t;

        ResourceWrapper(std::shared_ptr<typename TRAITS::manager_t> manager, resource_t* resource)
            : m_manager(manager)
            , m_resource(resource)
        {
            CheckInPointer(manager.get());
            CheckInPointer(resource);
        }

        virtual ~ResourceWrapper()
        {
            Close();
        }

        ComPtr<resource_t> const& GetResource()
        {
            return m_resource.EnsureNotClosed();
        }

        std::shared_ptr<typename TRAITS::manager_t> Manager()
        {
            return m_manager;
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
                        auto& resource = m_resource.Close();
                        m_manager->Remove(resource.Get());
                    }
                });
        }

        //
        // ICanvasResourceWrapperNative
        //

        IFACEMETHODIMP GetResource(IUnknown** outResource) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(outResource);

                    auto resource = GetResource();
                    ThrowIfFailed(resource.CopyTo(outResource));
                });
        }
    };
}}}}
