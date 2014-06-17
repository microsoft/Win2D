// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <CanvasDevice.abi.h>

#include "ClosablePtr.h"
#include "ResourceManager.h"

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class CanvasDevice;
    class CanvasDeviceManager;

    //
    // Abstracts away the creation of a D2D factory / D3D device, allowing unit
    // tests to provide test doubles. Because they are internal, they can return
    // ComPtrs and throw exceptions on failure.
    //
    class ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) = 0;

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device) = 0;

        virtual ComPtr<IDXGIDevice> GetDxgiDevice(ID2D1Device1* d2dDevice) = 0;
    };


    //
    // Default implementation of the adapter that actually talks to D3D / D2D
    // that is used in production.
    //
    class DefaultDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) override;

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device) override;

        virtual ComPtr<IDXGIDevice> GetDxgiDevice(ID2D1Device1* d2dDevice) override;
    };


    //
    // WinRT activation factory for the CanvasDevice runtimeclass.
    //
    class CanvasDeviceFactory : public ActivationFactory<
        ICanvasDeviceFactory, 
        ICanvasDeviceStatics, 
        CloakedIid<ICanvasFactoryNative>>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

        // TODO: #1442 - the factory can't own this since there's no guarantee
        // the factory stays around.  We need some way for a newly created
        // factory to find any previous existing manager and use that.
        std::shared_ptr<CanvasDeviceManager> m_manager;

    public:
        CanvasDeviceFactory();

        //
        // ActivationFactory
        //

        IFACEMETHOD(ActivateInstance)(_COM_Outptr_ IInspectable** ppvObject) override;

        //
        // ICanvasDeviceFactory
        //

        IFACEMETHOD(CreateWithDebugLevel)(
            CanvasDebugLevel debugLevel,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateWithDebugLevelAndHardwareAcceleration)(
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateFromDirectX11Device)(
            CanvasDebugLevel debugLevel,
            IDirectX11Device* directX11Device,
            ICanvasDevice** canvasDevice) override;

        //
        // ICanvasFactoryNative
        //
        
        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;
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
        virtual CanvasHardwareAcceleration GetRequestedHardwareAcceleration() = 0;
        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color) = 0;
    };


    struct CanvasDeviceTraits
    {
        typedef ID2D1Device1 resource_t;
        typedef CanvasDevice wrapper_t;
        typedef ICanvasDevice wrapper_interface_t;
        typedef CanvasDeviceManager manager_t;
    };

    //
    // The CanvasDevice class itself.
    //
    class CanvasDevice : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasDeviceTraits, 
        CloakedIid<ICanvasDeviceInternal>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

        // This is the exact same option that the CanvasDevice was created with.
        // Compatible devices are created with the same option.
        CanvasHardwareAcceleration m_requestedHardwareAcceleration;

        // This is used for the public-facing property value. Can be any value
        // other than Auto.
        CanvasHardwareAcceleration m_actualHardwareAcceleration; 

        CanvasDebugLevel m_debugLevel;
        
        ClosablePtr<IDirectX11Device> m_directX11Device;
        ClosablePtr<ID2D1DeviceContext> m_d2dResourceCreationDeviceContext;

    public:
        CanvasDevice(
            std::shared_ptr<CanvasDeviceManager> manager,
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration requestedHardwareAcceleration,
            CanvasHardwareAcceleration actualHardwareAcceleration,
            IDirectX11Device* directX11Device,
            ID2D1Device1* d2dDevice);

        //
        // ICanvasDevice
        //

        IFACEMETHOD(RecoverLostDevice)(
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateCompatibleDevice)(
            IDirectX11Device* directX11Device,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(get_HardwareAcceleration)(_Out_ CanvasHardwareAcceleration* value) override;

        IFACEMETHOD(get_DirectX11Device)(_Out_ IDirectX11Device** value) override;

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override;
        virtual CanvasHardwareAcceleration GetRequestedHardwareAcceleration() override;
        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color) override;
        
    private:
        ComPtr<ID2D1Factory2> GetD2DFactory();
    };


    //
    // Responsible for creating and tracking CanvasDevice instances and the
    // ID2D1Device they wrap.
    //
    class CanvasDeviceManager : public ResourceManager<CanvasDeviceTraits>
    {
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> m_adapter;
    public:
        CanvasDeviceManager(std::shared_ptr<ICanvasDeviceResourceCreationAdapter> adapter);
        
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, CanvasHardwareAcceleration hardwareAcceleration);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, CanvasHardwareAcceleration hardwareAcceleration, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, IDirectX11Device* directX11Device);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, IDirectX11Device* directX11Device, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateWrapper(ID2D1Device1* d2dDevice);

    private:
        ComPtr<IDirectX11Device> MakeDirectX11Device(
            CanvasHardwareAcceleration requestedHardwareAcceleration,
            CanvasHardwareAcceleration* actualHardwareAcceleration) const;

        ComPtr<ID3D11Device> MakeD3D11Device(
            CanvasHardwareAcceleration requestedHardwareAcceleration,
            CanvasHardwareAcceleration* actualHardwareAcceleration) const;

        static ComPtr<ID2D1Device1> MakeD2DDevice(
            IDirectX11Device* device,
            ID2D1Factory2* d2dFactory);        
    };

}
