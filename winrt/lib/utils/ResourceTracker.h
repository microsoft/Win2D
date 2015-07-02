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
    using namespace ::Microsoft::WRL;
    using namespace ::Microsoft::WRL::Wrappers;

    template<typename RESOURCE>
    class ResourceTracker
    {
        typedef typename RESOURCE::resource_t KEY;
        typedef typename RESOURCE::wrapper_t VALUE;
        typedef typename RESOURCE::wrapper_interface_t IVALUE;

        std::mutex m_mutex;
        std::map<IUnknown*, WeakRef> m_resources;

    public:
        void Add(KEY* key, VALUE* value)
        {
            ComPtr<IUnknown> keyIdentity;
            ThrowIfFailed(key->QueryInterface(IID_PPV_ARGS(keyIdentity.GetAddressOf())));

            std::lock_guard<std::mutex> lock(m_mutex);

            WeakRef weakValue;
            ThrowIfFailed(AsWeak(value, &weakValue));

            auto it = m_resources.lower_bound(keyIdentity.Get());

            if (it != m_resources.end() && it->first == keyIdentity.Get())
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
                std::make_pair(keyIdentity.Get(), weakValue));
        }

        template<typename CONSTRUCT_FN>
        ComPtr<VALUE> GetOrCreate(KEY* key, CONSTRUCT_FN&& constructFn)
        {
            return GetOrCreateWorker(key, constructFn).second;
        }

        template<typename CONSTRUCT_FN>
        ComPtr<VALUE> GetOrCreate(ICanvasDevice* device, KEY* key, CONSTRUCT_FN&& constructFn)
        {
            auto result = GetOrCreateWorker(key, constructFn);

            auto wasNewlyConstructed = result.first;
            auto const& value = result.second;

            ValidateDevice(value, wasNewlyConstructed, device);

            return value;
        }

        template<typename CONSTRUCT_FN>
        ComPtr<VALUE> GetOrCreate(ICanvasDevice* device, KEY* key, float dpi, CONSTRUCT_FN&& constructFn)
        {
            auto result = GetOrCreateWorker(key, constructFn);

            auto wasNewlyConstructed = result.first;
            auto const& value = result.second;

            ValidateDevice(value, wasNewlyConstructed, device);
            ValidateDpi(value, wasNewlyConstructed, dpi);

            return value;
        }

        void Remove(KEY* key)
        {
            ComPtr<IUnknown> keyIdentity;
            ThrowIfFailed(key->QueryInterface(IID_PPV_ARGS(keyIdentity.GetAddressOf())));

            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = m_resources.find(keyIdentity.Get());

            if (it != m_resources.end())
            {
                m_resources.erase(it);
            }
        }

    private:
        template<typename CONSTRUCT_FN>
        std::pair<bool, ComPtr<VALUE>> GetOrCreateWorker(KEY* key, CONSTRUCT_FN&& constructFn)
        {
            ComPtr<IUnknown> keyIdentity;
            ThrowIfFailed(key->QueryInterface(IID_PPV_ARGS(keyIdentity.GetAddressOf())));

            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = m_resources.lower_bound(keyIdentity.Get());

            if (it != m_resources.end() && it->first == keyIdentity.Get())
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
                    return std::make_pair(false, ComPtr<VALUE>(static_cast<VALUE*>(ivalue.Get())));
                }
            }

            // Make a new resource
            auto value = constructFn();

            WeakRef weakValue;
            ThrowIfFailed(AsWeak(value.Get(), &weakValue));
        
            m_resources.insert(
                it,
                std::make_pair(keyIdentity.Get(), weakValue));

            return std::make_pair(true, value);
        }

        void ValidateDevice(ComPtr<VALUE> const& value, bool wasNewlyConstructed, ICanvasDevice* expectedDevice)
        {
            //
            // Validate that the object we got back reports that it is
            // associated with the requested device.
            //
            ComPtr<ICanvasDevice> existingDevice;
            ThrowIfFailed(value->get_Device(&existingDevice));

            if (existingDevice.Get() != expectedDevice)
            {
                if (wasNewlyConstructed)
                {
                    // A device mismatch on a newly created device indicates an
                    // internal error
                    assert(false && L"constructFn didn't create a wrapper associated with the expected device");
                    ThrowHR(E_UNEXPECTED);
                }
                else
                {
                    // A device mismatch on a fetched device indicates a caller
                    // error
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::ResourceTrackerWrongDevice).Get());
                }
            }
        }

        void ValidateDpi(ComPtr<VALUE> const& value, bool wasNewlyConstructed, float expectedDpi)
        {
            //
            // Validate that the object we got back reports that it has
            // the requested DPI.
            //
            float existingDpi;
            ThrowIfFailed(value->get_Dpi(&existingDpi));

            if (existingDpi != expectedDpi)
            {
                if (wasNewlyConstructed)
                {
                    // DPI mismatch on a newly created resource indicates an internal error
                    assert(false && L"constructFn didn't create a wrapper associated with the expected DPI");
                    ThrowHR(E_UNEXPECTED);
                }
                else
                {
                    // A device mismatch on a fetched resource indicates a caller error
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::ResourceTrackerWrongDpi).Get());
                }
            }
        }
    };
}}}}
