// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class __declspec(uuid("D8CF19FE-8064-423E-B649-8B458BA86116"))
    ICanvasResourceWrapperWithDevice : public IUnknown
    {
    public:
        IFACEMETHOD(get_Device)(ICanvasDevice** value) = 0;
    };


    class __declspec(uuid("D4142C4E-024D-45C3-85D1-B058314D9204"))
    ICanvasResourceWrapperWithDpi : public IUnknown
    {
    public:
        IFACEMETHOD(get_Dpi)(float* value) = 0;
    };


    class ResourceManager
    {
    public:
        // Used by ResourceWrapper to maintain its state in the interop mapping table.
        static void RegisterWrapper(IUnknown* resource, IInspectable* wrapper);
        static bool TryRegisterWrapper(IUnknown* resource, IInspectable* wrapper);
        static void UnregisterWrapper(IUnknown* resource);
        static bool TryUnregisterWrapper(IUnknown* resource);
        static bool RegisterEffectFactory(REFIID effectId, ICanvasEffectFactoryNative* factory);
        static bool UnregisterEffectFactory(REFIID effectId);


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
        
        
        // Validation helpers, also used by ResourceWrapper.
        static void ValidateDevice(IInspectable* wrapper, ICanvasDevice* device);
        static void ValidateDevice(ICanvasResourceWrapperWithDevice* wrapper, ICanvasDevice* device);
        static void ValidateDevice(ICanvasImageInterop* wrapper, ICanvasDevice* device);
        
        static void ValidateDpi(IInspectable* wrapper, float dpi);
        static void ValidateDpi(ICanvasResourceWrapperWithDpi* wrapper, float dpi);

        // Lookup function used by CanvasEffect::TryCreateEffect to try to resolve external effects
        static ComPtr<ICanvasEffectFactoryNative> TryGetEffectFactory(REFIID effectId);


        // A try-create function attempts to wrap a native resource with a WinRT wrapper of a specific type.
        // The result is an out pointer rather than return value because we are going to call these functions
        // a bunch of times in a loop probing for different types, and don't want the overhead of messing
        // with refcounts for the common case of probes that early out due to wrong resource type.

        typedef bool(*TryCreateFunction)(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result);


        // Allow unit tests to inject additional try-create functions.
        static void RegisterType(TryCreateFunction tryCreate);
        static void UnregisterType(TryCreateFunction tryCreate);


        // Custom tester functions can apply extra filtering rules (eg. calling IsRenderTargetBitmap on an ID2D1Bitmap1).
        template<typename TResource>
        static inline bool DefaultTester(TResource*)
        {
            return true;
        }


        template<typename TResource, typename TWrapper, typename TMaker, bool TTester(TResource*) = DefaultTester<TResource>>
        static bool TryCreate(ICanvasDevice* device, IUnknown* resource, float dpi, ComPtr<IInspectable>* result)
        {
            static_assert(std::is_base_of<ICanvasResourceWrapperNative, TWrapper>::value, "Types used with interop should implement ICanvasResourceWrapperNative");

            // Is this the type we are looking for?
            auto myTypeOfResource = MaybeAs<TResource>(resource);

            if (!myTypeOfResource)
                return false;

            if (!TTester(myTypeOfResource.Get()))
                return false;

            // Create a new wrapper instance.
            auto wrapper = TMaker::Make<TResource, TWrapper>(device, myTypeOfResource.Get(), dpi);
            
            CheckMakeResult(wrapper);
            ThrowIfFailed(wrapper.As(result));

            return true;
        }


        //  Calls Make<> on a type whose constructor wants only a resource parameter.
        struct MakeWrapper
        {
            template<typename TResource, typename TWrapper>
            static ComPtr<TWrapper> Make(ICanvasDevice* device, TResource* resource, float dpi)
            {
                UNREFERENCED_PARAMETER(device);
                UNREFERENCED_PARAMETER(dpi);

                return ::Microsoft::WRL::Make<TWrapper>(resource);
            }
        };


        //  Calls Make<> on a type whose constructor wants device and resource parameters.
        struct MakeWrapperWithDevice
        {
            template<typename TResource, typename TWrapper>
            static ComPtr<TWrapper> Make(ICanvasDevice* device, TResource* resource, float dpi)
            {
                UNREFERENCED_PARAMETER(dpi);

                static_assert(std::is_base_of<ICanvasResourceWrapperWithDevice, TWrapper>::value, "Types constructed from a device should implement ICanvasResourceWrapperWithDevice");

                if (!device)
                    ThrowHR(E_INVALIDARG, Strings::ResourceManagerNoDevice);

                return ::Microsoft::WRL::Make<TWrapper>(device, resource);
            }
        };


        //  Calls Make<> on a type whose constructor wants device, resource, and dpi parameters.
        struct MakeWrapperWithDeviceAndDpi
        {
            template<typename TResource, typename TWrapper>
            static ComPtr<TWrapper> Make(ICanvasDevice* device, TResource* resource, float dpi)
            {
                static_assert(std::is_base_of<ICanvasResourceWrapperWithDevice, TWrapper>::value, "Types constructed from a device should implement ICanvasResourceWrapperWithDevice");
                static_assert(std::is_base_of<ICanvasResourceWrapperWithDpi,    TWrapper>::value, "Types constructed with a dpi should implement ICanvasResourceWrapperWithDpi");
                
                if (!device)
                    ThrowHR(E_INVALIDARG, Strings::ResourceManagerNoDevice);

                if (dpi <= 0)
                    ThrowHR(E_INVALIDARG, Strings::ResourceManagerNoDpi);

                return ::Microsoft::WRL::Make<TWrapper>(device, resource, dpi);
            }
        };


    private:
        // Native resource -> WinRT wrapper map, shared by all active resources.
        static std::unordered_map<IUnknown*, WeakRef> m_resources;
        static std::unordered_map<IID, ComPtr<ICanvasEffectFactoryNative>> m_effectFactories;
        static std::recursive_mutex m_mutex;

        // Table of try-create functions, one per type.
        static std::vector<TryCreateFunction> tryCreateFunctions;
    };
}}}}
