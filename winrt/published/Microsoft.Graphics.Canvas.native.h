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
                using namespace ABI::Windows::Foundation;

                interface ICanvasDevice;
                interface ICanvasResourceCreator;

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

                typedef enum CanvasImageGetD2DImageFlags
                {
                    None = 0,
                    ReadDpiFromDeviceContext = 1,    // Ignore the targetDpi parameter - read DPI from deviceContext instead
                    AlwaysInsertDpiCompensation = 2, // Ignore the targetDpi parameter - always insert DPI compensation
                    NeverInsertDpiCompensation = 4,  // Ignore the targetDpi parameter - never insert DPI compensation
                    MinimalRealization = 8,          // Do the bare minimum to get back an ID2D1Image - no validation or recursive realization
                    AllowNullEffectInputs = 16,      // Allow partially configured effect graphs where some inputs are null
                } CanvasImageGetD2DImageFlags;

                //
                // Interface implemented by all effects and also exposed to allow external users to implement custom effects.
                //
                class __declspec(uuid("E042D1F7-F9AD-4479-A713-67627EA31863"))
                ICanvasImageInterop : public IUnknown
                {
                public:
                    IFACEMETHOD(GetDevice)(ICanvasDevice** device) = 0;

                    IFACEMETHOD(GetD2DImage)(
                        ICanvasDevice* device,
                        ID2D1DeviceContext* deviceContext,
                        CanvasImageGetD2DImageFlags flags,
                        float targetDpi,
                        float* realizeDpi,
                        ID2D1Image** ppImage) = 0;
                };

                //
                // Exported method to allow ICanvasImageInterop implementors to implement ICanvasImage properly.
                //
                extern "C" __declspec(nothrow, dllexport) HRESULT __stdcall GetBoundsForICanvasImageInterop(
                    ICanvasResourceCreator* resourceCreator,
                    ICanvasImageInterop* image,
                    Numerics::Matrix3x2 const* transform,
                    Rect* rect);
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
