// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class ResourceManager
    {
    public:
        // Used by ResourceWrapper to maintain its state in the interop mapping table.
        static void Add(IUnknown* resource, WeakRef const& wrapper);
        static void Remove(IUnknown* resource);


        // Used internally, and exposed to apps via CanvasDeviceFactory::GetOrCreate and Microsoft.Graphics.Canvas.native.h.
        static ComPtr<IInspectable> GetOrCreate(ICanvasDevice* device, IUnknown* resource, float dpi);


        // Convenience helpers.
        template<typename T>
        static ComPtr<T> GetOrCreate(IUnknown* resource)
        {
            return As<T>(GetOrCreate(nullptr, resource, 0));
        }

        template<typename T>
        static ComPtr<T> GetOrCreate(ICanvasDevice* device, IUnknown* resource)
        {
            return As<T>(GetOrCreate(device, resource, 0));
        }
        
        
        // A try-create function attempts to wrap a native resource with a WinRT wrapper of a specific type.
        // The result is an out pointer rather than return value because we are going to call these functions
        // a bunch of times in a loop probing for different types, and don't want the overhead of messing
        // with refcounts for the common case of probes that early out due to wrong resource type.

        typedef bool(*TryCreateFunction)(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result);


        // Allow unit tests to inject additional try-create functions.
        static void RegisterType(TryCreateFunction tryCreate);
        static void UnregisterType(TryCreateFunction tryCreate);


        // TODO interop: the following templates are far too repetetetetive - should be greatly simplified.
        // Won't need so many variants here once the manager intermediates are 100% refactored out.

        template<typename TResource>
        static bool DefaultTester(TResource*)
        {
            return true;
        }


        template<typename TResource, typename TWrapper, ComPtr<TWrapper> TMaker(ICanvasDevice*, TResource*, float), bool TTester(TResource*) = DefaultTester<TResource>>
        static bool TryCreate(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result)
        {
            // Is this the type we are looking for?
            auto myTypeOfResource = MaybeAs<TResource>(resource);

            if (!myTypeOfResource)
                return false;

            if (!TTester(myTypeOfResource.Get()))
                return false;

            // Create a new wrapper instance.
            auto wrapper = TMaker(device, myTypeOfResource.Get(), dpi);

            ThrowIfFailed(wrapper.As(result));
            return true;
        }


        template<typename TResource, typename TWrapper>
        static ComPtr<TWrapper> MakeWrapper(ICanvasDevice* device, TResource* resource, float dpi)
        {
            UNREFERENCED_PARAMETER(device);
            UNREFERENCED_PARAMETER(dpi);

            auto wrapper = Make<TWrapper>(resource);
            CheckMakeResult(wrapper);
            return wrapper;
        }


        template<typename TResource, typename TWrapper>
        static ComPtr<TWrapper> MakeWrapperWithDevice(ICanvasDevice* device, TResource* resource, float dpi)
        {
            UNREFERENCED_PARAMETER(dpi);

            auto wrapper = Make<TWrapper>(device, resource);
            CheckMakeResult(wrapper);
            return wrapper;
        }


        template<typename TResource, typename TWrapper>
        static ComPtr<TWrapper> MakeWrapperWithDeviceAndDpi(ICanvasDevice* device, TResource* resource, float dpi)
        {
            auto wrapper = Make<TWrapper>(device, resource, dpi);
            CheckMakeResult(wrapper);
            return wrapper;
        }


    private:
        // Native resource -> WinRT wrapper map, shared by all active resources.
        static std::unordered_map<IUnknown*, WeakRef> m_resources;
        static std::recursive_mutex m_mutex;

        // Table of try-create functions, one per type.
        static std::vector<TryCreateFunction> tryCreateFunctions;
    };
}}}}
