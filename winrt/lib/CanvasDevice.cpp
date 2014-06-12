// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "CanvasDevice.h"

namespace canvas
{
    //
    // This implementation of the adapter does the normal thing, and calls into
    // D2D. It is separated out so tests can inject their own implementation.
    //
    ComPtr<ID2D1Factory2> DefaultDeviceResourceCreationAdapter::CreateD2DFactory(
        CanvasDebugLevel debugLevel)
    {
        D2D1_FACTORY_OPTIONS factoryOptions;
        factoryOptions.debugLevel = static_cast<D2D1_DEBUG_LEVEL>(debugLevel);

        ComPtr<ID2D1Factory2> createdFactory;
        ThrowIfFailed(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED, 
            __uuidof(ID2D1Factory2), 
            &factoryOptions, 
            &createdFactory));

        return createdFactory;
    }

    bool DefaultDeviceResourceCreationAdapter::TryCreateD3DDevice(
        CanvasHardwareAcceleration hardwareAcceleration, 
        ComPtr<ID3D11Device>* device)
    {
        // Precondition: hardware acceleration is not Auto.
        // TODO: Consider assert here.

        D3D_DRIVER_TYPE driverType = 
            hardwareAcceleration == CanvasHardwareAcceleration::On ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_WARP;

        ComPtr<ID3D11Device> createdDevice;
        if (SUCCEEDED(D3D11CreateDevice(
            NULL, // adapter
            driverType,
            NULL, // software handle
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            NULL, // feature level array
            0,  // feature level count
            D3D11_SDK_VERSION,
            &createdDevice,
            NULL,
            NULL)))
        {
            *device = createdDevice;
            return true;
        }
        else
        {
            return false;
        }
    }

    CanvasDebugLevel ValidateDebugLevel(CanvasDebugLevel debugLevel)
    {
        // Why not do this from the adapter's CreateD2DDevice? Because the unit 
        // tests use their own adapter.
        switch (debugLevel)
        {
        case CanvasDebugLevel::None:
        case CanvasDebugLevel::Warning:
        case CanvasDebugLevel::Error:
        case CanvasDebugLevel::Information:
            break;
        default:
            throw InvalidArgException();
        }
        return debugLevel;
    }

    CanvasDeviceFactory::CanvasDeviceFactory() 
        : m_resourceCreationAdapter(std::make_shared<DefaultDeviceResourceCreationAdapter>())
    {}

    IFACEMETHODIMP CanvasDeviceFactory::CreateWithDebugLevel(
        CanvasDebugLevel debugLevel,
        ICanvasDevice **canvasDevice
        )
    {
        return CreateWithDebugLevelAndHardwareAcceleration(
            debugLevel,
            CanvasHardwareAcceleration::Auto,
            canvasDevice
            );
    }

    IFACEMETHODIMP CanvasDeviceFactory::CreateWithDebugLevelAndHardwareAcceleration(
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration hardwareAcceleration,
        ICanvasDevice **canvasDevice
        )
    {
        return ExceptionBoundary(
            [&]()
        {
            CheckAndClearOutPointer(canvasDevice);

            auto newCanvasDevice = Make<CanvasDevice>(debugLevel, hardwareAcceleration, m_resourceCreationAdapter);

            CheckMakeResult(newCanvasDevice);

            ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
        });
    }

    // From ActivationFactory
    IFACEMETHODIMP CanvasDeviceFactory::ActivateInstance(_COM_Outptr_ IInspectable **object)
    {
        return ExceptionBoundary(
            [&]()
        {
            auto newCanvasDevice = Make<CanvasDevice>(m_resourceCreationAdapter);

            CheckMakeResult(newCanvasDevice);

            ThrowIfFailed(newCanvasDevice.CopyTo(object));
        });
    }

    CanvasDevice::CanvasDevice(std::shared_ptr<ICanvasDeviceResourceCreationAdapter> factoryCreationAdapter)
        : CanvasDevice(CanvasDebugLevel::None, CanvasHardwareAcceleration::Auto, factoryCreationAdapter)
    {
    }

    CanvasDevice::CanvasDevice(
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration hardwareAcceleration,
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> resourceCreationAdapter
        )
        : CanvasDevice(
            ValidateDebugLevel(debugLevel),
            hardwareAcceleration, 
            resourceCreationAdapter, 
            resourceCreationAdapter->CreateD2DFactory(debugLevel))
    {}

    CanvasDevice::CanvasDevice(
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration hardwareAcceleration,
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter,
        ComPtr<ID2D1Factory2> d2dFactory
        )
        : m_resourceCreationAdapter(deviceResourceCreationAdapter)
        , m_hardwareAccelerationRoundTrip(hardwareAcceleration)
        , m_debugLevel(debugLevel)
    {
        // Note: CreateD3DDevice requires the initialization of the members in the
        // initialization list above.

        ComPtr<IDirectX11Device> directX11Device = CreateDirectX11Device();

        SetDevice(directX11Device.Get(), d2dFactory.Get());
    }

    CanvasDevice::CanvasDevice(
        CanvasDebugLevel debugLevel,
        IDirectX11Device* directX11Device,
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter
        )
        : CanvasDevice(
            ValidateDebugLevel(debugLevel), 
            directX11Device,
            deviceResourceCreationAdapter,
            deviceResourceCreationAdapter->CreateD2DFactory(debugLevel))
    {
    }

    CanvasDevice::CanvasDevice(
        CanvasDebugLevel debugLevel,
        IDirectX11Device* directX11Device,
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> deviceResourceCreationAdapter,
        ComPtr<ID2D1Factory2> d2dFactory
        )
        : m_hardwareAccelerationRoundTrip(CanvasHardwareAcceleration::Auto)
        , m_hardwareAccelerationRetrievable(CanvasHardwareAcceleration::Auto) // TODO: Consider using an error value for this.
        , m_debugLevel(debugLevel)
        , m_resourceCreationAdapter(deviceResourceCreationAdapter)
    {
        CheckInPointer(directX11Device);

        SetDevice(directX11Device, d2dFactory.Get());
    }

    IFACEMETHODIMP CanvasDevice::RecoverLostDevice(
        ICanvasDevice **canvasDevice
        )
    {
        return ExceptionBoundary(
            [&]()
        {
            CheckAndClearOutPointer(canvasDevice);

            ThrowIfClosed();

            auto newCanvasDevice = Make<CanvasDevice>(
                m_debugLevel,
                m_hardwareAccelerationRoundTrip,
                m_resourceCreationAdapter,
                GetD2DFactory());

            CheckMakeResult(newCanvasDevice);

            ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
        });
    }

    IFACEMETHODIMP CanvasDevice::CreateCompatibleDevice(
        IDirectX11Device* directX11Device,
        ICanvasDevice **canvasDevice
        )
    {
        return ExceptionBoundary([&]()
        {
            CheckAndClearOutPointer(canvasDevice);

            ThrowIfClosed();

            auto newCanvasDevice = Make<CanvasDevice>(
                m_debugLevel,
                directX11Device,
                m_resourceCreationAdapter,
                GetD2DFactory()
                );

            CheckMakeResult(newCanvasDevice);

            ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
        });
    }

    void CanvasDevice::ThrowIfClosed()
    {
        //
        // This takes advantage the fact that this clases's members are CloseablePtrs,
        // they are all non-NULL upon initialization, and the implementation of Close 
        // closes them. Therefore, it's sufficient to just check if one of the pointers has been closed. 
        //
        m_directX11Device.EnsureNotClosed();
    }

    ComPtr<IDirectX11Device> CanvasDevice::CreateDirectX11Device()
    {
        ComPtr<ID3D11Device> d3dDevice;

        switch (m_hardwareAccelerationRoundTrip)
        {

        case CanvasHardwareAcceleration::Auto:
            if (m_resourceCreationAdapter->TryCreateD3DDevice(CanvasHardwareAcceleration::On, &d3dDevice))
            {
                m_hardwareAccelerationRetrievable = CanvasHardwareAcceleration::On;
            }
            else
            {
                m_hardwareAccelerationRetrievable = CanvasHardwareAcceleration::Off;
                if (!m_resourceCreationAdapter->TryCreateD3DDevice(CanvasHardwareAcceleration::Off, &d3dDevice))
                {
                    throw ComException(E_FAIL);
                }
            }
            break;

        case CanvasHardwareAcceleration::On:
            m_hardwareAccelerationRetrievable = CanvasHardwareAcceleration::On;
            if (!m_resourceCreationAdapter->TryCreateD3DDevice(CanvasHardwareAcceleration::On, &d3dDevice))
            {
                throw ComException(E_FAIL);
            }
            break;

        case CanvasHardwareAcceleration::Off:
            m_hardwareAccelerationRetrievable = CanvasHardwareAcceleration::Off;
            if (!m_resourceCreationAdapter->TryCreateD3DDevice(CanvasHardwareAcceleration::Off, &d3dDevice))
            {
                throw ComException(E_FAIL);
            }
            break;

        default:
            throw InvalidArgException();
            break;
        }

        // Wrap the native D3D device in a projected DirectX11Device object.
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        ComPtr<IDirectX11Device> directX11Device;
        ThrowIfFailed(CreateDirectX11DeviceFromDXGIDevice(dxgiDevice.Get(), &directX11Device));

        return directX11Device;
    }

    void CanvasDevice::SetDevice(IDirectX11Device* directX11Device, ID2D1Factory2* d2dFactory)
    {
        m_directX11Device = directX11Device;

        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(GetDXGIInterfaceFromDirectX11Device(directX11Device, __uuidof(IDXGIDevice), &dxgiDevice));

        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(dxgiDevice.As(&d3dDevice));

        ComPtr<ID2D1Device> d2dBaseDevice;
        ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dBaseDevice));

        ComPtr<ID2D1Device1> d2dDevice;
        ThrowIfFailed(d2dBaseDevice.As(&d2dDevice));

        m_d2dDevice = d2dDevice; // This assignment is necessary because m_d2dDevice is ClosablePtr.

        ComPtr<ID2D1DeviceContext> d2dResourceCreationDeviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2dResourceCreationDeviceContext
            ));

        m_d2dResourceCreationDeviceContext = d2dResourceCreationDeviceContext;
    }

    ComPtr<ID2D1Factory2> CanvasDevice::GetD2DFactory()
    {
        ComPtr<ID2D1Device1> d2dDevice = GetD2DDevice();

        ComPtr<ID2D1Factory> d2dBaseFactory;
        d2dDevice->GetFactory(&d2dBaseFactory);

        ComPtr<ID2D1Factory2> d2dDerivedFactory;
        d2dBaseFactory.As(&d2dDerivedFactory);

        return d2dDerivedFactory;
    }

    IFACEMETHODIMP CanvasDevice::get_HardwareAcceleration(_Out_ CanvasHardwareAcceleration *value)
    {
        return ExceptionBoundary(
            [&]()
        {
            CheckInPointer(value);

            ThrowIfClosed();

            *value = m_hardwareAccelerationRetrievable;

            return S_OK;
        });
    }

    IFACEMETHODIMP CanvasDevice::get_DirectX11Device(_Out_ IDirectX11Device **value)
    {
        return ExceptionBoundary(
            [&]()
        {
            CheckAndClearOutPointer(value);
            ThrowIfClosed();
            ComPtr<IDirectX11Device> directX11Device = m_directX11Device.EnsureNotClosed();
            ThrowIfFailed(directX11Device.CopyTo(value));
        });
    }

    //
    // From IClosable
    // 
    IFACEMETHODIMP CanvasDevice::Close()
    {
        m_directX11Device.Close();
        m_d2dDevice.Close();
        m_d2dResourceCreationDeviceContext.Close();
        return S_OK;
    }

    //
    // From ICanvasDeviceInternal
    //
    ComPtr<ID2D1Device1> CanvasDevice::GetD2DDevice()
    {
        ComPtr<ID2D1Device1> d2dDevice = m_d2dDevice.EnsureNotClosed();
        return d2dDevice;
    }

    CanvasHardwareAcceleration CanvasDevice::GetRoundTripHardwareAcceleration()
    {
        return m_hardwareAccelerationRoundTrip;
    }

    ComPtr<ID2D1DeviceContext> CanvasDevice::GetD2DResourceCreationDeviceContext()
    {
        ComPtr<ID2D1DeviceContext> d2dResourceCreationDeviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();
        return d2dResourceCreationDeviceContext;
    }

    ActivatableClassWithFactory(CanvasDevice, CanvasDeviceFactory);
}