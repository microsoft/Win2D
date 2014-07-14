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

#include <Canvas.abi.h>   // TODO: #1432 we don't want to be shipping these idls, we should be shipping the ones generated via winmdidl 
#include <d2d1_2.h>

namespace ABI
{
    namespace Microsoft
    {
        namespace Graphics
        {
            namespace Canvas
            {
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
                // Interface provided by various Canvas objects that is able to
                // retrieve the wrapped resource.
                //
                [uuid(5F10688D-EA55-4D55-A3B0-4DDB55C0C20A)]
                class ICanvasResourceWrapperNative : public IUnknown
                {
                public:
                    IFACEMETHOD(GetResource)(IUnknown** resource) = 0;
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


            template<typename T, typename U>
            Microsoft::WRL::ComPtr<T> GetWrappedResource(U^ wrapper)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                Platform::Object^ objectWrapper = wrapper;
                IInspectable* inspectableWrapper = reinterpret_cast<IInspectable*>(objectWrapper);

                ComPtr<abi::ICanvasResourceWrapperNative> nativeWrapper;
                __abi_ThrowIfFailed(inspectableWrapper->QueryInterface(nativeWrapper.GetAddressOf()));

                ComPtr<IUnknown> unknownResource;
                __abi_ThrowIfFailed(nativeWrapper->GetResource(&unknownResource));

                ComPtr<T> resource;
                __abi_ThrowIfFailed(unknownResource.As(&resource));
                
                return resource;
            }
        }
    }
}

#endif // __cplusplus_winrt
