// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <inspectable.h>
#include <wrl.h>

interface ID2D1Device1;

namespace ABI
{
    namespace Microsoft
    {
        namespace Graphics
        {
            namespace Canvas
            {
                interface ICanvasDevice;

                //
                // Interface provided by the CanvasDevice factory that is
                // able to get or create objects that wrap resources.
                //
                class __declspec(uuid("695C440D-04B3-4EDD-BFD9-63E51E9F7202"))
                ICanvasFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(ICanvasDevice* device, IUnknown* resource, float dpi, IInspectable** wrapper) = 0;
                };

                //
                // Interface provided by various Canvas objects that is able to
                // retrieve the wrapped resource.
                //
                class __declspec(uuid("5F10688D-EA55-4D55-A3B0-4DDB55C0C20A"))
                ICanvasResourceWrapperNative : public IUnknown
                {
                public:
                    IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** resource) = 0;
                };
            }
        }
    }
}


#if defined(__cplusplus_winrt)

#include <wrl.h>

namespace Microsoft
{
    namespace Graphics
    {
        namespace Canvas
        {
            template<class WRAPPER>
            WRAPPER^ GetOrCreate(IUnknown* resource)
            {
                return GetOrCreate<WRAPPER>(static_cast<CanvasDevice^>(nullptr), resource, 0);
            }

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(CanvasDevice^ device, IUnknown* resource)
            {
                return GetOrCreate<WRAPPER>(device, resource, 0);
            }

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(CanvasDevice^ device, IUnknown* resource, float dpi)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
                    &factory));

                ComPtr<IInspectable> inspectableWrapper;
                __abi_ThrowIfFailed(factory->GetOrCreate(reinterpret_cast<abi::ICanvasDevice*>(device), resource, dpi, &inspectableWrapper));

                Platform::Object^ objectWrapper = reinterpret_cast<Platform::Object^>(inspectableWrapper.Get());

                return safe_cast<WRAPPER^>(objectWrapper);
            }


            template<class WRAPPER>
            WRAPPER^ GetOrCreate(ID2D1Device1* device, IUnknown* resource)
            {
                CanvasDevice^ canvasDevice = GetOrCreate<CanvasDevice>(device);
                return GetOrCreate<WRAPPER>(canvasDevice, resource);
            }

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(ID2D1Device1* device, IUnknown* resource, float dpi)
            {
                CanvasDevice^ canvasDevice = GetOrCreate<CanvasDevice>(device);
                return GetOrCreate<WRAPPER>(canvasDevice, resource, dpi);
            }


            template<typename T, typename U>
            Microsoft::WRL::ComPtr<T> GetWrappedResource(U^ wrapper)
            {
                return GetWrappedResource<T>(static_cast<CanvasDevice^>(nullptr), wrapper, 0);
            }

            template<typename T, typename U>
            Microsoft::WRL::ComPtr<T> GetWrappedResource(CanvasDevice^ device, U^ wrapper)
            {
                return GetWrappedResource<T>(device, wrapper, 0);
            }

            template<typename T, typename U>
            Microsoft::WRL::ComPtr<T> GetWrappedResource(CanvasDevice^ device, U^ wrapper, float dpi)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                Platform::Object^ objectWrapper = wrapper;
                IInspectable* inspectableWrapper = reinterpret_cast<IInspectable*>(objectWrapper);

                ComPtr<abi::ICanvasResourceWrapperNative> nativeWrapper;
                __abi_ThrowIfFailed(inspectableWrapper->QueryInterface(nativeWrapper.GetAddressOf()));

                ComPtr<T> resource;
                __abi_ThrowIfFailed(nativeWrapper->GetNativeResource(reinterpret_cast<abi::ICanvasDevice*>(device), dpi, IID_PPV_ARGS(&resource)));

                return resource;
            }
        }
    }
}

#endif // __cplusplus_winrt
