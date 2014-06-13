// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <CanvasDevice.abi.h>   // TODO: #1432 we don't want to be shipping these idls, we should be shipping the ones generated via winmdidl 
#include <d2d1_2.h>

namespace ABI
{
    namespace Microsoft
    {
        namespace Graphics
        {
            namespace Canvas
            {
                [uuid(8A6C0118-C2E2-4F50-81AD-8E457D02678F)]
                class ICanvasDeviceFactoryNative : public IInspectable
                {
                public:
                    IFACEMETHOD(GetOrCreate)(ID2D1Device1* d2dDevice, ICanvasDevice** canvasDevice) = 0;
                };

                [uuid(1F33032E-BF94-469C-9B14-7751DCCDFF72)]
                class ICanvasDeviceNative : public IUnknown
                {
                public:
                    IFACEMETHOD(GetD2DDevice)(ID2D1Device1** d2dDevice) = 0;
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
            inline CanvasDevice^ GetOrCreateCanvasDevice(ID2D1Device1* d2dDevice)
            {
                using namespace Microsoft::WRL;
                using namespace Microsoft::WRL::Wrappers;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasDeviceFactory> factory;
                __abi_ThrowIfFailed(Windows::Foundation::GetActivationFactory(
                    HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice).Get(),
                    &factory));

                ComPtr<abi::ICanvasDeviceFactoryNative> factoryNative;
                __abi_ThrowIfFailed(factory.As(&factoryNative));

                ComPtr<abi::ICanvasDevice> canvasDevice;
                __abi_ThrowIfFailed(factoryNative->GetOrCreate(d2dDevice, &canvasDevice));

                return reinterpret_cast<CanvasDevice^>(canvasDevice.Get());
            }

            template<typename T, typename U>
            Microsoft::WRL::ComPtr<T> GetWrappedResource(U^ wrapper);

            template<>
            inline Microsoft::WRL::ComPtr<ID2D1Device1> GetWrappedResource(CanvasDevice^ wrapper)
            {
                using namespace Microsoft::WRL;
                namespace abi = ABI::Microsoft::Graphics::Canvas;

                ComPtr<abi::ICanvasDevice> canvasDevice(reinterpret_cast<abi::ICanvasDevice*>(wrapper));

                ComPtr<abi::ICanvasDeviceNative> canvasDeviceNative;
                __abi_ThrowIfFailed(canvasDevice.As(&canvasDeviceNative));

                ComPtr<ID2D1Device1> d2dDevice;
                __abi_ThrowIfFailed(canvasDeviceNative->GetD2DDevice(&d2dDevice));

                return d2dDevice;
            }
            
        }
    }
}

#endif // __cplusplus_winrt
