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
                // Interface provided by the various Canvas factories that is
                // able to get or create objects that wrap resources.
                //
                [uuid(6F18BC37-016D-4510-A0BC-99B523D65E6C)]
                class ICanvasFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(IUnknown* resource, IInspectable** wrapper) = 0;
                };

                //
                // Interface provided by the various Canvas factories that is
                // able to get or create objects that wrap resources that are
                // associated with a specific CanvasDevice.
                //
                [uuid(67FE1FBC-9773-4F58-9313-E74C8CADE01D)]
                class ICanvasDeviceResourceFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(ICanvasDevice* device, IUnknown* resource, IInspectable** wrapper) = 0;
                };

                //
                // Interface provided by the various Canvas factories that is
                // able to get or create objects that wrap resources that are
                // associated with a specific CanvasDevice and need to know DPI.
                //
                [uuid({BDE18E86-4EDC-497D-9EB8-8C26B1EC149C})]
                class ICanvasDeviceResourceWithDpiFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(ICanvasDevice* device, IUnknown* resource, float dpi, IInspectable** wrapper) = 0;
                };

                //
                // Interface provided by various Canvas objects that is able to
                // retrieve the wrapped resource.
                //
                [uuid(5F10688D-EA55-4D55-A3B0-4DDB55C0C20A)]
                class ICanvasResourceWrapperNative : public IUnknown
                {
                public:
                    IFACEMETHOD(GetResource)(REFIID iid, void** resource) = 0;
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
            // TODO 2523: better error reporting when the wrong GetOrCreate is used

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(IUnknown* resource)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;
                
                ComPtr<abi::ICanvasFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(WRAPPER::typeid->FullName),
                    &factory));

                ComPtr<IInspectable> inspectableWrapper;
                __abi_ThrowIfFailed(factory->GetOrCreate(resource, &inspectableWrapper));

                Platform::Object^ objectWrapper = reinterpret_cast<Platform::Object^>(inspectableWrapper.Get());
                
                return safe_cast<WRAPPER^>(objectWrapper);
            }

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(CanvasDevice^ device, IUnknown* resource)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;
                
                ComPtr<abi::ICanvasDeviceResourceFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(WRAPPER::typeid->FullName),
                    &factory));

                ComPtr<IInspectable> inspectableWrapper;
                __abi_ThrowIfFailed(factory->GetOrCreate(reinterpret_cast<abi::ICanvasDevice*>(device), resource, &inspectableWrapper));

                Platform::Object^ objectWrapper = reinterpret_cast<Platform::Object^>(inspectableWrapper.Get());
                
                return safe_cast<WRAPPER^>(objectWrapper);
            }

            template<class WRAPPER>
            WRAPPER^ GetOrCreate(CanvasDevice^ device, IUnknown* resource, float dpi)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasDeviceResourceWithDpiFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(WRAPPER::typeid->FullName),
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
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                Platform::Object^ objectWrapper = wrapper;
                IInspectable* inspectableWrapper = reinterpret_cast<IInspectable*>(objectWrapper);

                ComPtr<abi::ICanvasResourceWrapperNative> nativeWrapper;
                __abi_ThrowIfFailed(inspectableWrapper->QueryInterface(nativeWrapper.GetAddressOf()));

                ComPtr<T> resource;
                __abi_ThrowIfFailed(nativeWrapper->GetResource(IID_PPV_ARGS(&resource)));

                return resource;
            }
        }
    }
}

#endif // __cplusplus_winrt
