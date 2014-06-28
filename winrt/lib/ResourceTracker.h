// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace Microsoft::WRL;

    template<typename RESOURCE>
    class ResourceTracker
    {
        typedef typename RESOURCE::resource_t KEY;
        typedef typename RESOURCE::wrapper_t VALUE;
        typedef typename RESOURCE::wrapper_interface_t IVALUE;

        std::mutex m_mutex;
        std::map<KEY*, WeakRef> m_resources;

    public:
        void Add(KEY* key, VALUE* value)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            WeakRef weakValue;
            ThrowIfFailed(AsWeak(value, &weakValue));

            auto it = m_resources.lower_bound(key);

            if (it != m_resources.end() && it->first == key)
            {
                //
                // We found an existing entry.  This should be impossible since
                // we expect resources to remove themselves.
                //
                assert(false);
                ThrowHR(E_UNEXPECTED);
            }

            m_resources.insert(
                it, 
                std::make_pair(key, weakValue));
        }

        template<typename CONSTRUCT_FN>
        ComPtr<VALUE> GetOrCreate(KEY* key, CONSTRUCT_FN&& constructFn)
        {            
            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = m_resources.lower_bound(key);

            if (it != m_resources.end() && it->first == key)
            {
                //
                // We found an existing entry.  It's a weak reference so check that
                // it is still valid.
                //
                auto weakRef = it->second;

                ComPtr<IVALUE> ivalue;
                (void)weakRef.As(&ivalue);
                if (ivalue)
                {
                    //
                    // WeakRef can only give us back an interface (it needs a
                    // UUID).  However, we want to deal with VALUE which is the
                    // implementation class (consider canvas::CanvasDevice
                    // versus ICanvasDevice).
                    //
                    // We know that we've only put VALUEs into m_resources, so
                    // we can safely cast from IVALUE to VALUE.
                    //
                    ComPtr<VALUE> value(static_cast<VALUE*>(ivalue.Get()));
                    return value;
                }
            }

            // Make a new resource
            auto value = constructFn();

            WeakRef weakValue;
            ThrowIfFailed(AsWeak(value.Get(), &weakValue));
        
            m_resources.insert(
                it,
                std::make_pair(key, weakValue));

            return value;
        }

        void Remove(KEY* key)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = m_resources.find(key);

            if (it == m_resources.end())
            {
                assert(false);
                ThrowHR(E_UNEXPECTED);
            }

            m_resources.erase(it);
        }
    };
}
