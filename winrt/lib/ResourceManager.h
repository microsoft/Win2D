// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <type_traits>

#include "ResourceTracker.h"
#include "ResourceWrapper.h"

namespace canvas
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
            auto wrapper = Manager()->CreateNew(args...);
            m_tracker.Add(wrapper->GetResource().Get(), wrapper.Get());
            return wrapper;
        }

        ComPtr<wrapper_t> GetOrCreate(resource_t* resource)
        {
            return m_tracker.GetOrCreate(
                resource,
                [&]() 
                {
                    return Manager()->CreateWrapper(resource); 
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
        typename TRAITS::manager_t* Manager()
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
}
