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

#include <type_traits>

#include "ResourceTracker.h"
#include "ResourceWrapper.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // Manages idempotent resource wrapping.  This is intended to be used in
    // conjunction with ResourceWrapper.
    //
    // See ResourceTrackerUnitTests.cpp for simple example usage.
    //
    // TRAITS is expected to have following typedef members:
    //
    //     resource_t - the type of resource being wrapped (eg ID2D1Device1)
    //     wrapper_t  - the concrete type of wrapper (eg CanvasDevice)
    //     wrapper_interface_t - the default interface of the wrapper (eg ICanvasDevice)
    //     manager_t - the manager type
    //
    // manager_t is expected to derive from ResourceManager.
    //
    // It can provide however many overloaded CreateNew() methods to create
    // instances of wrapper_t is requires.  It must also provide a
    // CreateWrapper() method that wraps a resource_t.
    //
    // ResourceManager exposes Create() and GetOrCreate() methods that
    // internally call CreateNew() or CreateWrapper().
    //
    // class MyManager : public ResourceManager<MyTraits>
    // {
    // public:
    //     ComPtr<MyWrapper> CreateNew(int param1, int param2);
    //     ComPtr<IMyWrapper> CreateWrapper(MyResource* r);
    // };
    //
    // auto w = MyManager.Create(1,2); // will call CreateNew
    // auto w = MyManager.GetOrCreate(r); // calls CreateWrapper if wrapper needs to be created
    //
    template<typename TRAITS>
    class ResourceManager : public std::enable_shared_from_this<typename TRAITS::manager_t>
    {
        ResourceTracker<typename TRAITS::wrapper_t> m_tracker;
    public:
        typedef typename TRAITS::resource_t resource_t;
        typedef typename TRAITS::wrapper_t wrapper_t;
        typedef typename TRAITS::wrapper_interface_t wrapper_interface_t;

        template<typename... Arguments>
        ComPtr<wrapper_t> Create(Arguments&&... args)
        {
            auto wrapper = GetManager()->CreateNew(args...);
            m_tracker.Add(wrapper->GetResource().Get(), wrapper.Get());
            return wrapper;
        }

        ComPtr<wrapper_t> GetOrCreate(resource_t* resource)
        {
            return m_tracker.GetOrCreate(
                resource,
                [&] 
                {
                    return GetManager()->CreateWrapper(resource); 
                });
        }

        ComPtr<wrapper_t> GetOrCreate(ICanvasDevice* device, resource_t* resource)
        {
            return m_tracker.GetOrCreate(
                device,
                resource,
                [&]
                {
                    return GetManager()->CreateWrapper(device, resource);
                });
        }

    protected:
        //
        // This class is intended to only be used when derived from.  Making the
        // constructor protected helps to enforce this.
        //
        ResourceManager() = default;

    private:
        //
        // Cast to the manager type.  Remember that manager_t needs to derive
        // from ResourceManager.  This allows ResourceManager to call functions
        // in manager_t.
        //
        typename TRAITS::manager_t* GetManager()
        {
            return static_cast<TRAITS::manager_t*>(this);
        }

        //
        // Indicates that 'resource' is no longer being wrapped by anything.
        // The is called by ResourceWrapper::Close().
        //
        void Remove(resource_t* resource)
        {
            m_tracker.Remove(resource);
        }

        // ResourceWrapper needs to be able to call remove
        friend class ResourceWrapper<TRAITS>;
    };


    //
    // Helper for activation factories to manage their manager object.
    //
    // ActivationFactories may come and go, but as long as something is holding
    // a reference to a ResourceManager somewhere we want the factory to
    // continue to use that one.
    //
    // FACTORY is expected to derive from PerApplicationManager.
    //
    template<typename FACTORY, typename MANAGER>
    class PerApplicationManager
    {
        std::shared_ptr<MANAGER> m_manager;
    public:
        typedef MANAGER manager_t;

        //
        // Default constructor uses the real CoreApplication statics.
        //
        PerApplicationManager()
            : m_manager(GetOrCreateManager())
        {
        }

        //
        // Constructor that uses a specific ICoreApplication; allows tests to
        // provide their own instance.
        //
        PerApplicationManager(ABI::Windows::ApplicationModel::Core::ICoreApplication* coreApplication)
            : m_manager(GetOrCreateManager(coreApplication))
        {
        }

        std::shared_ptr<MANAGER> const& GetManager()
        {
            return m_manager;
        }

        static std::shared_ptr<MANAGER> GetOrCreateManager()
        {
            return GetOrCreateManager(GetCoreApplication().Get());
        }

        static std::shared_ptr<MANAGER> GetOrCreateManager(ABI::Windows::ApplicationModel::Core::ICoreApplication* coreApplication)
        {
            using ::Microsoft::WRL::Wrappers::HStringReference;
            using namespace ABI::Windows::Foundation::Collections;

            //
            // Look up the manager holder.  This is stored in
            // CoreApplication.Properties since we can't store any global
            // references to COM objects (since C++ doesn't have guaranteed
            // destruction order for global references).
            //

            ComPtr<IPropertySet> properties;
            ThrowIfFailed(coreApplication->get_Properties(
                &properties));

            ComPtr<IMap<HSTRING, IInspectable*>> propertyMap;
            ThrowIfFailed(properties.As(&propertyMap));

            HStringReference keyName(TEXT(__FUNCDNAME__));
            ComPtr<ManagerHolder> managerHolder;

            boolean hasKey = false;
            ThrowIfFailed(propertyMap->HasKey(keyName.Get(), &hasKey));

            if (hasKey)
            {
                ComPtr<IInspectable> inspectableManagerHolder;
                ThrowIfFailed(propertyMap->Lookup(keyName.Get(), &inspectableManagerHolder));

                managerHolder = static_cast<ManagerHolder*>(inspectableManagerHolder.Get());
            }

            //
            // Use the existing manager if possible
            //

            if (managerHolder)
            {
                auto manager = managerHolder->Manager.lock();
                if (manager)
                    return manager;
            }

            //
            // Create, and remember, a new manager if there wasn't one
            // previously
            //

            auto manager = FACTORY::CreateManager();

            managerHolder = Make<ManagerHolder>();
            managerHolder->Manager = manager;
            
            boolean inserted = false;
            ThrowIfFailed(propertyMap->Insert(keyName.Get(), managerHolder.Get(), &inserted)); // Insert will replace an existing entry

            return manager;
        }

    private:
        static ComPtr<ABI::Windows::ApplicationModel::Core::ICoreApplication> GetCoreApplication()
        {
            using namespace ABI::Windows::ApplicationModel::Core;
            using namespace ABI::Windows::Foundation;
            using namespace ::Microsoft::WRL::Wrappers;

            ComPtr<ICoreApplication> coreApplication;
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(),
                &coreApplication));

            return coreApplication;
        }

        //
        // Wrapper about a weak_ptr<MANAGER> that can be stored in a PropertySet
        // (and therefore on CoreApplication.Properties).
        //
        struct ManagerHolder : public RuntimeClass<IInspectable>
        {
            std::weak_ptr<MANAGER> Manager;
        };


        //
        // FACTORY can provide it's own implementation of this if it needs more
        // explicit control over manager creation.
        //
        static std::shared_ptr<MANAGER> CreateManager()
        {
            return std::make_shared<MANAGER>();
        }
    };

//
// This macro can be placed inside an activation factory class definition to
// provide a default implementation of ICanvasFactoryNative
//
#define IMPLEMENT_DEFAULT_ICANVASFACTORYNATIVE()                        \
    IFACEMETHODIMP GetOrCreate(IUnknown* resource, IInspectable** wrapper) override \
    {                                                                   \
        return ExceptionBoundary(                                       \
            [&]                                                         \
            {                                                           \
                CheckInPointer(resource);                               \
                CheckAndClearOutPointer(wrapper);                       \
                                                                        \
                ComPtr<typename manager_t::resource_t> typedResource; \
                ThrowIfFailed(resource->QueryInterface(typedResource.GetAddressOf())); \
                                                                        \
                auto newWrapper = GetManager()->GetOrCreate(typedResource.Get()); \
                                                                        \
                ThrowIfFailed(newWrapper.CopyTo(wrapper));              \
            });                                                         \
    }

}}}}
