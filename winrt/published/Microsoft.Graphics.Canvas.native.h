// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <inspectable.h>
#include <windows.foundation.numerics.h>
#include <wrl.h>

#ifndef __cplusplus
#error "Requires C++"
#endif

#ifdef WIN2D_DLL_EXPORT
#define WIN2DAPI extern "C" __declspec(dllexport) HRESULT __stdcall
#else
#define WIN2DAPI extern "C" __declspec(dllimport) HRESULT __stdcall
#endif

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
                interface ICanvasResourceCreatorWithDpi;

                //
                // An interface for a factory that can create a resource wrapper from a native D2D effect.
                // This is meant to be used by custom effects to register wrapper factories to create a
                // managed wrapper to return to users if one is no longer available for a given resource.
                //
                class __declspec(uuid("29BA1A1F-1CFE-44C3-984D-426D61B51427"))
                ICanvasEffectFactoryNative : public IUnknown
                {
                public:
                    IFACEMETHOD(CreateWrapper)(ICanvasDevice* device, ID2D1Effect* resource, float dpi, IInspectable** wrapper) = 0;
                };

                //
                // Interface provided by the CanvasDevice factory that is
                // able to get or create objects that wrap resources.
                // 
                // Note: the GetOrCreate was introduced long before support for custom effects was introduced in Win2D.
                // Because of this, the name was kept simple given that there were no other APIs next to it that there
                // was a need to differentiate with. With the introduction of ICanvasImageInterop, new APIs are needed
                // to allow authors of external effects to ensure those custom effects can seamlessly interoperate with
                // Win2D and support all the scenarios where built-in effects can also work, such as wrapping from a
                // native resources. In order to make the behaviors of these new APIs clearer, we use the "Wrapper"
                // suffix, to clearly indicate that the new APIs allow developers to register a pair of resource-wrapper,
                // and a pair of effectId-wrapperFactory. This is not consistent with "GetOrCreate" (as it would've been
                // if it had been called "GetOrCreateWrapper", but we feel the suffix and new naming scheme makes things
                // much easier to understand with the new APIs that it's worth the slight variation from the previous one.
                //
                class __declspec(uuid("695C440D-04B3-4EDD-BFD9-63E51E9F7202"))
                ICanvasFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(ICanvasDevice* device, IUnknown* resource, float dpi, IInspectable** wrapper) = 0;
                    IFACEMETHOD(RegisterWrapper)(IUnknown* resource, IInspectable* wrapper) = 0;
                    IFACEMETHOD(UnregisterWrapper)(IUnknown* resource) = 0;
                    IFACEMETHOD(RegisterEffectFactory)(REFIID effectId, ICanvasEffectFactoryNative* factory) = 0;
                    IFACEMETHOD(UnregisterEffectFactory)(REFIID effectId) = 0;
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

                // The type of canvas device returned by ICanvasImageInterop::GetDevice, describing how the device is associated with the canvas image.
                typedef enum WIN2D_GET_DEVICE_ASSOCIATION_TYPE
                {
                    WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED = 0,          // No device info is available, callers will handle the returned device with their own logic
                    WIN2D_GET_DEVICE_ASSOCIATION_TYPE_REALIZATION_DEVICE = 1,   // The returned device is the one the image is currently realized on, if any
                    WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE = 2,      // The returned device is the one that created the image resource, and cannot change
                    WIN2D_GET_DEVICE_ASSOCIATION_TYPE_FORCE_DWORD = 0xffffffff
                } WIN2D_GET_DEVICE_ASSOCIATION_TYPE;

                // Options for fine-tuning the behavior of ICanvasImageInterop::GetD2DImage.
                typedef enum WIN2D_GET_D2D_IMAGE_FLAGS
                {
                    WIN2D_GET_D2D_IMAGE_FLAGS_NONE = 0,
                    WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT = 1,    // Ignore the targetDpi parameter - read DPI from deviceContext instead
                    WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION = 2,  // Ignore the targetDpi parameter - always insert DPI compensation
                    WIN2D_GET_D2D_IMAGE_FLAGS_NEVER_INSERT_DPI_COMPENSATION = 4,   // Ignore the targetDpi parameter - never insert DPI compensation
                    WIN2D_GET_D2D_IMAGE_FLAGS_MINIMAL_REALIZATION = 8,             // Do the bare minimum to get back an ID2D1Image - no validation or recursive realization
                    WIN2D_GET_D2D_IMAGE_FLAGS_ALLOW_NULL_EFFECT_INPUTS = 16,       // Allow partially configured effect graphs where some inputs are null
                    WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE = 32,           // If an input is invalid, unrealize the effect and set the output image to null
                    WIN2D_GET_D2D_IMAGE_FLAGS_FORCE_DWORD = 0xffffffff
                } WIN2D_GET_D2D_IMAGE_FLAGS;

                DEFINE_ENUM_FLAG_OPERATORS(WIN2D_GET_D2D_IMAGE_FLAGS)

                //
                // Interface implemented by all effects and also exposed to allow external users to implement custom effects.
                //
                class __declspec(uuid("E042D1F7-F9AD-4479-A713-67627EA31863"))
                ICanvasImageInterop : public IUnknown
                {
                public:
                    IFACEMETHOD(GetDevice)(ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type) = 0;

                    IFACEMETHOD(GetD2DImage)(
                        ICanvasDevice* device,
                        ID2D1DeviceContext* deviceContext,
                        WIN2D_GET_D2D_IMAGE_FLAGS flags,
                        float targetDpi,
                        float* realizeDpi,
                        ID2D1Image** ppImage) = 0;
                };

                //
                // Supporting interfaces to allow external effects to access Win2D's pool of ID2D1DeviceContext objects for each device.
                //
                class __declspec(uuid("A0928F38-F7D5-44DD-A5C9-E23D94734BBB"))
                ID2D1DeviceContextLease : public IUnknown
                {
                public:
                    IFACEMETHOD(GetD2DDeviceContext)(ID2D1DeviceContext** deviceContext) = 0;
                };

                class __declspec(uuid("454A82A1-F024-40DB-BD5B-8F527FD58AD0"))
                ID2D1DeviceContextPool : public IUnknown
                {
                public:
                    IFACEMETHOD(GetDeviceContextLease)(ID2D1DeviceContextLease** lease) = 0;
                };

                //
                // Exported method to allow ICanvasImageInterop implementors to implement ICanvasImage properly.
                //
                WIN2DAPI GetBoundsForICanvasImageInterop(
                    ICanvasResourceCreator* resourceCreator,
                    ICanvasImageInterop* image,
                    Numerics::Matrix3x2 const* transform,
                    Rect* rect) noexcept;

                namespace Effects
                {
                    interface ICanvasEffect;

                    //
                    // Exported methods to allow ICanvasImageInterop implementors to implement ICanvasEffect properly.
                    //
                    WIN2DAPI InvalidateSourceRectangleForICanvasImageInterop(
                        ICanvasResourceCreatorWithDpi* resourceCreator,
                        ICanvasImageInterop* image,
                        uint32_t sourceIndex,
                        Rect const* invalidRectangle) noexcept;

                    WIN2DAPI GetInvalidRectanglesForICanvasImageInterop(
                        ICanvasResourceCreatorWithDpi* resourceCreator,
                        ICanvasImageInterop* image,
                        uint32_t* valueCount,
                        Rect** valueElements) noexcept;

                    WIN2DAPI GetRequiredSourceRectanglesForICanvasImageInterop(
                        ICanvasResourceCreatorWithDpi* resourceCreator,
                        ICanvasImageInterop* image,
                        Rect const* outputRectangle,
                        uint32_t sourceEffectCount,
                        ICanvasEffect* const* sourceEffects,
                        uint32_t sourceIndexCount,
                        uint32_t const* sourceIndices,
                        uint32_t sourceBoundsCount,
                        Rect const* sourceBounds,
                        uint32_t valueCount,
                        Rect* valueElements) noexcept;
                }
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

            template<class WRAPPER>
            bool RegisterWrapper(IUnknown* resource, WRAPPER^ wrapper)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
                    &factory));

                Platform::Object^ objectWrapper = wrapper;
                IInspectable* inspectableWrapper = reinterpret_cast<IInspectable*>(objectWrapper);

                HRESULT hresult = factory->RegisterWrapper(resource, inspectableWrapper);

                __abi_ThrowIfFailed(hresult);

                return hresult == S_OK;
            }

            inline bool UnregisterWrapper(IUnknown* resource)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
                    &factory));

                HRESULT hresult = factory->UnregisterWrapper(resource);

                __abi_ThrowIfFailed(hresult);

                return hresult == S_OK;
            }

            inline bool RegisterEffectFactory(REFIID effectId, ABI::Microsoft::Graphics::Canvas::ICanvasEffectFactoryNative* factory)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasFactoryNative> activationFactory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
                    &activationFactory));

                HRESULT hresult = activationFactory->RegisterEffectFactory(effectId, factory);

                __abi_ThrowIfFailed(hresult);

                return hresult == S_OK;
            }

            inline bool UnregisterEffectFactory(REFIID effectId)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasFactoryNative> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    reinterpret_cast<HSTRING>(CanvasDevice::typeid->FullName),
                    &factory));

                HRESULT hresult = factory->UnregisterEffectFactory(effectId);

                __abi_ThrowIfFailed(hresult);

                return hresult == S_OK;
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
