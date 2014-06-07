// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "CanvasDevice.abi.h"
#include <ClosablePtr.h>

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    // These interfaces are used to abstract away the creation of a D2D factory / D3D device. Benefit
    // is for unit tests. Because they are internal, they can return ComPtrs and throw exceptions 
    // on failure.
    class ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) = 0;

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device) = 0;
    };

    class DefaultDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter
    {
    public:
        ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel);

        bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device);
    };

    class CanvasDeviceFactory : public ActivationFactory<ICanvasDeviceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

        std::shared_ptr<DefaultDeviceResourceCreationAdapter> m_resourceCreationAdapter;

    public:

        CanvasDeviceFactory();

        IFACEMETHOD(CreateWithDebugLevel)(
            CanvasDebugLevel debugLevel,
            ICanvasDevice **canvasDevice
            ) override;

        IFACEMETHOD(CreateWithDebugLevelAndHardwareAcceleration)(
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            ICanvasDevice **canvasDevice
            ) override;

        // From ActivationFactory
        IFACEMETHOD(ActivateInstance)(_Outptr_result_nullonfailure_ IInspectable **ppvObject) override;
    };

    //
    // This internal interface is exposed by the CanvasDevice runtime class and
    // allows for internal access to non-WinRT methods.
    //
    // Since this is a native C++ (ie not COM) object and these methods never
    // cross DLL boundaries these use exceptions for error handling and can
    // take/return C++ types.
    //
    [uuid(E2C64DD1-4126-4C7A-A5D2-D5E8B2C7235C)]
    class ICanvasDeviceInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Device1> GetD2DDevice() = 0;

        virtual CanvasHardwareAcceleration GetRoundTripHardwareAcceleration() = 0;
    };

    class CanvasDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>, 
        ICanvasDevice, 
        ABI::Windows::Foundation::IClosable, 
        CloakedIid<ICanvasDeviceInternal>>
    {
        // This is used to abstract the D2D stack away from the tests.
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> m_resourceCreationAdapter;

        // This is the exact same option that the CanvasDevice was created with. 
        // Compatible devices are created with the same option. Can be Auto.
        CanvasHardwareAcceleration m_hardwareAccelerationRoundTrip;

        // This is used for the public-facing property value. Only valid to have On or Off,
        // never Auto.
        CanvasHardwareAcceleration m_hardwareAccelerationRetrievable; 

        CanvasDebugLevel m_debugLevel;
        
        ClosablePtr<IDirectX11Device> m_directX11Device;
        ClosablePtr<ID2D1Device1> m_d2dDevice;

    public:

        CanvasDevice(std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter);

        CanvasDevice(
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter
            );

        CanvasDevice(
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter,
            ComPtr<ID2D1Factory2> d2dFactory
            );

        CanvasDevice(
            CanvasDebugLevel debugLevel,
            IDirectX11Device* directX11Device,
            std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter
            );

        CanvasDevice(
            CanvasDebugLevel debugLevel,
            IDirectX11Device* directX11Device,
            std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter,
            ComPtr<ID2D1Factory2> d2dFactory
            );

        IFACEMETHOD(RecoverLostDevice)(
            ICanvasDevice **canvasDevice
            ) override;

        IFACEMETHOD(CreateCompatibleDevice)(
            IDirectX11Device* directX11Device,
            ICanvasDevice **canvasDevice
            ) override;

        IFACEMETHOD(get_HardwareAcceleration)(_Out_ CanvasHardwareAcceleration *value) override;

        IFACEMETHOD(get_DirectX11Device)(_Out_ IDirectX11Device **value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasDeviceInternal
        ComPtr<ID2D1Device1> GetD2DDevice() override;

        CanvasHardwareAcceleration GetRoundTripHardwareAcceleration() override;

    private:

        ComPtr<IDirectX11Device> CreateDirectX11Device();

        void SetDevice(IDirectX11Device* directX11Device, ID2D1Factory2* d2dFactory);

        ComPtr<ID2D1Factory2> GetD2DFactory();

        void ThrowIfClosed();

        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

    };

}
