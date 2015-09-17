// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // Some Win2D classes (eg. image brushes and effects) often find themselves needing
    // to carry out operations of the form:
    //
    //  1) Look up a D2D resource from a property of some other D2D interface
    //  2) Use ResourceManager::GetOrCreate to find the corresponding Win2D wrapper
    //
    // This helper can optimize that work. If the Win2D implementation stores a
    // CachedResourceReference and replaces the ResourceManager::GetOrCreate call
    // with CachedResourceReference.GetOrCreateWrapper, it will see two benefits:
    //
    //  1) Improved performance for the common case of repeated calls using the same D2D instance
    //  2) Maintaining a reference to the wrapper avoids it being repeatedly released and recreated
    //

    template<typename TResource, typename TWrapper>
    class CachedResourceReference
    {
        ComPtr<IUnknown> m_resource;
        ComPtr<TWrapper> m_wrapper;

    public:
        IUnknown* GetResource() { return m_resource.Get(); }
        TWrapper* GetWrapper()  { return m_wrapper.Get();  }


        // It is not required to call Set, but doing so at resource creation time if you already
        // happen to know both the resource and wrapper will speed future GetOrCreateWrapper calls.
        void Set(TResource* resource, TWrapper* wrapper)
        {
            m_resource = resource ? As<IUnknown>(resource) : nullptr;
            m_wrapper = wrapper;
        }


        void Reset()
        {
            m_resource.Reset();
            m_wrapper.Reset();
        }


        ComPtr<TWrapper> const& GetOrCreateWrapper(ICanvasDevice* device, TResource* resource)
        {
            auto resourceIdentity = resource ? As<IUnknown>(resource) : nullptr;

            // If we don't already have a cached wrapper for this resource, look one up now.
            if (m_resource != resourceIdentity)
            {
                if (resource)
                    m_wrapper = ResourceManager::GetOrCreate<TWrapper>(device, resource);
                else
                    m_wrapper.Reset();

                m_resource = resourceIdentity;
            }

            return m_wrapper;
        }


        // Updates the resource for an existing wrapper, after potential re-realization
        // of a lazily bound type. Returns whether the resource had changed.
        bool UpdateResource(TResource* resource)
        {
            auto resourceIdentity = As<IUnknown>(resource);

            if (m_resource == resourceIdentity)
                return false;

            m_resource = resourceIdentity;
            return true;
        }
    };
}}}}
