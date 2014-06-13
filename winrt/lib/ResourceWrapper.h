// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
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
    class ResourceWrapper : public Implements<ABI::Windows::Foundation::IClosable>
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
            CheckInPointer(resource);
        }

        virtual ~ResourceWrapper()
        {
            Close();
        }

        const ComPtr<resource_t>& GetResource()
        {
            return m_resource.EnsureNotClosed();
        }

        std::shared_ptr<typename TRAITS::manager_t> Manager()
        {
            return m_manager;
        }

        IFACEMETHODIMP Close() override
        {
            if (m_resource)
            {
                auto& resource = m_resource.Close();
                m_manager->Remove(resource.Get());
            }

            return S_OK;
        }
    };
}
