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
        assert(hardwareAcceleration == CanvasHardwareAcceleration::On || hardwareAcceleration == CanvasHardwareAcceleration::Off);

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

    //
    // We want to find the DXGI device associated with an ID2D1Device.
    // Unfortunately, there's no direct way to do this.  However, it is possible
    // to figure it out by creating a bitmap and querying that for the dxgi
    // surface and then its dxgi device.
    //
    // #1428 tracks getting direct support for this added to D2D.
    //
    ComPtr<IDXGIDevice> DefaultDeviceResourceCreationAdapter::GetDxgiDevice(ID2D1Device1* d2dDevice)
    {
        ComPtr<ID2D1DeviceContext> deviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &deviceContext));

        ComPtr<ID2D1Bitmap1> bitmap;
        ThrowIfFailed(deviceContext->CreateBitmap(
            D2D1_SIZE_U{1,1},
            nullptr,
            0,
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &bitmap));

        ComPtr<IDXGISurface> surface;
        ThrowIfFailed(bitmap->GetSurface(&surface));

        ComPtr<IDXGIDevice> device;
        ThrowIfFailed(surface->GetDevice(IID_PPV_ARGS(&device)));

        return device;
    }

    //
    // CanvasDeviceManager
    //

    void ThrowIfInvalid(CanvasDebugLevel debugLevel)
    {
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
    }

    CanvasDeviceManager::CanvasDeviceManager(
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> adapter)
        : m_adapter(adapter)
    {
    }


    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel, 
        CanvasHardwareAcceleration requestedHardwareAcceleration)
    {
        ThrowIfInvalid(debugLevel);

        auto d2dFactory = m_adapter->CreateD2DFactory(debugLevel);

        return CreateNew(debugLevel, requestedHardwareAcceleration, d2dFactory.Get());
    }


    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration requestedHardwareAcceleration,
        ID2D1Factory2* d2dFactory)
    {
        ThrowIfInvalid(debugLevel);
        CheckInPointer(d2dFactory);

        CanvasHardwareAcceleration actualHardwareAcceleration;

        auto directX11Device = MakeDirectX11Device(
            requestedHardwareAcceleration,
            &actualHardwareAcceleration);

        auto d2dDevice = MakeD2DDevice(
            directX11Device.Get(), 
            d2dFactory);
        
        auto device = Make<CanvasDevice>(
            shared_from_this(), 
            debugLevel,
            requestedHardwareAcceleration,
            actualHardwareAcceleration,
            directX11Device.Get(),
            d2dDevice.Get());
        CheckMakeResult(device);

        return device;
    }

    
    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel,
        IDirectX11Device* directX11Device)
    {
        ThrowIfInvalid(debugLevel);
        CheckInPointer(directX11Device);
        auto d2dFactory = m_adapter->CreateD2DFactory(debugLevel);

        return CreateNew(debugLevel, directX11Device, d2dFactory.Get());
    }


    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel,
        IDirectX11Device* directX11Device,
        ID2D1Factory2* d2dFactory)
    {
        ThrowIfInvalid(debugLevel);
        CheckInPointer(directX11Device);
        CheckInPointer(d2dFactory);

        auto d2dDevice = MakeD2DDevice(
            directX11Device, 
            d2dFactory);
        
        auto device = Make<CanvasDevice>(
            shared_from_this(), 
            debugLevel, 
            CanvasHardwareAcceleration::Unknown,
            CanvasHardwareAcceleration::Unknown,
            directX11Device, 
            d2dDevice.Get());
        CheckMakeResult(device);

        return device;
    }

    
    ComPtr<CanvasDevice> CanvasDeviceManager::CreateWrapper(
        ID2D1Device1* d2dDevice)
    {
        auto dxgiDevice = m_adapter->GetDxgiDevice(d2dDevice);

        ComPtr<IDirectX11Device> directX11Device;
        ThrowIfFailed(CreateDirectX11DeviceFromDXGIDevice(dxgiDevice.Get(), &directX11Device));

        auto canvasDevice = Make<CanvasDevice>(
            shared_from_this(),
            CanvasDebugLevel::None,
            CanvasHardwareAcceleration::Unknown,
            CanvasHardwareAcceleration::Unknown,
            directX11Device.Get(),
            d2dDevice);
        CheckMakeResult(canvasDevice);
        
        return canvasDevice;
    }


    ComPtr<IDirectX11Device> CanvasDeviceManager::MakeDirectX11Device(
        CanvasHardwareAcceleration requestedHardwareAcceleration,
        CanvasHardwareAcceleration* actualHardwareAccelerationOut) const
    {
        assert(actualHardwareAccelerationOut);
        *actualHardwareAccelerationOut = CanvasHardwareAcceleration::Unknown;

        CanvasHardwareAcceleration actualHardwareAcceleration = CanvasHardwareAcceleration::Unknown;

        ComPtr<ID3D11Device> d3dDevice = MakeD3D11Device(
            requestedHardwareAcceleration,
            &actualHardwareAcceleration);

        // Wrap the native D3D device in a projected DirectX11Device object.
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        ComPtr<IDirectX11Device> directX11Device;
        ThrowIfFailed(CreateDirectX11DeviceFromDXGIDevice(dxgiDevice.Get(), &directX11Device));

        *actualHardwareAccelerationOut = actualHardwareAcceleration;

        return directX11Device;
    }


    ComPtr<ID3D11Device> CanvasDeviceManager::MakeD3D11Device(
        CanvasHardwareAcceleration requestedHardwareAcceleration,
        CanvasHardwareAcceleration* actualHardwareAccelerationOut) const
    {
        assert(actualHardwareAccelerationOut);

        ComPtr<ID3D11Device> d3dDevice;

        switch (requestedHardwareAcceleration)
        {
        case CanvasHardwareAcceleration::Auto:
            if (m_adapter->TryCreateD3DDevice(CanvasHardwareAcceleration::On, &d3dDevice))
            {
                *actualHardwareAccelerationOut = CanvasHardwareAcceleration::On;
                return d3dDevice;
            }

            if (m_adapter->TryCreateD3DDevice(CanvasHardwareAcceleration::Off, &d3dDevice))
            {
                *actualHardwareAccelerationOut = CanvasHardwareAcceleration::Off;
                return d3dDevice;
            }

            break;

        case CanvasHardwareAcceleration::On:
        case CanvasHardwareAcceleration::Off:
            if (m_adapter->TryCreateD3DDevice(requestedHardwareAcceleration, &d3dDevice))
            {
                *actualHardwareAccelerationOut = requestedHardwareAcceleration;
                return d3dDevice;
            }

            break;

        default:
            throw InvalidArgException();
        }

        // If we end up here then we failed to create a d3d device
        throw ComException(E_FAIL);
    }


    /*static*/
    ComPtr<ID2D1Device1> CanvasDeviceManager::MakeD2DDevice(
        IDirectX11Device* directX11Device,
        ID2D1Factory2* d2dFactory)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(GetDXGIInterfaceFromDirectX11Device(directX11Device, __uuidof(IDXGIDevice), &dxgiDevice));

        ComPtr<ID2D1Device1> d2dDevice;
        ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));

        return d2dDevice;
    }


    //
    // CanvasDeviceFactory
    //

    std::shared_ptr<CanvasDeviceManager> CanvasDeviceFactory::CreateManager()
    {
        auto adapter = std::make_shared<DefaultDeviceResourceCreationAdapter>();
        return std::make_shared<CanvasDeviceManager>(adapter);
    }

    IFACEMETHODIMP CanvasDeviceFactory::CreateWithDebugLevel(
        CanvasDebugLevel debugLevel,
        ICanvasDevice** canvasDevice)
    {
        return CreateWithDebugLevelAndHardwareAcceleration(
            debugLevel,
            CanvasHardwareAcceleration::Auto,
            canvasDevice);
    }

    IFACEMETHODIMP CanvasDeviceFactory::CreateWithDebugLevelAndHardwareAcceleration(
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration hardwareAcceleration,
        ICanvasDevice** canvasDevice)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(canvasDevice);
                
                auto newCanvasDevice = GetManager()->Create(debugLevel, hardwareAcceleration);
                
                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
    }

    IFACEMETHODIMP CanvasDeviceFactory::CreateFromDirectX11Device(
        CanvasDebugLevel debugLevel,
        IDirectX11Device* directX11Device,
        ICanvasDevice** canvasDevice)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(directX11Device);
                CheckAndClearOutPointer(canvasDevice);

                auto newCanvasDevice = GetManager()->Create(debugLevel, directX11Device);

                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
    }

    IFACEMETHODIMP CanvasDeviceFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1Device1> d2dDevice;
                ThrowIfFailed(resource->QueryInterface(d2dDevice.GetAddressOf()));

                auto newCanvasDevice = GetManager()->GetOrCreate(d2dDevice.Get());

                ThrowIfFailed(newCanvasDevice.CopyTo(wrapper));
            });
    }

    _Use_decl_annotations_
    IFACEMETHODIMP CanvasDeviceFactory::ActivateInstance(IInspectable **object)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(object);

                auto newCanvasDevice = GetManager()->Create(
                    CanvasDebugLevel::None,
                    CanvasHardwareAcceleration::Auto);

                ThrowIfFailed(newCanvasDevice.CopyTo(object));
            });
    }

    //
    // CanvasDevice
    //

    CanvasDevice::CanvasDevice(
        std::shared_ptr<CanvasDeviceManager> deviceManager,
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration requestedHardwareAcceleration,
        CanvasHardwareAcceleration actualHardwareAcceleration,
        IDirectX11Device* directX11Device,
        ID2D1Device1* d2dDevice)
        : ResourceWrapper(deviceManager, d2dDevice)
        , m_requestedHardwareAcceleration(requestedHardwareAcceleration)
        , m_actualHardwareAcceleration(actualHardwareAcceleration)
        , m_debugLevel(debugLevel)
        , m_directX11Device(directX11Device)
    {
        CheckInPointer(directX11Device);

        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dResourceCreationDeviceContext));
    }

    
    IFACEMETHODIMP CanvasDevice::RecoverLostDevice(
        ICanvasDevice **canvasDevice)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(canvasDevice);

                auto newCanvasDevice = Manager()->Create(
                    m_debugLevel,
                    m_requestedHardwareAcceleration,
                    GetD2DFactory().Get());

                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
    }

    IFACEMETHODIMP CanvasDevice::CreateCompatibleDevice(
        IDirectX11Device* directX11Device,
        ICanvasDevice** canvasDevice)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(canvasDevice);

                auto newCanvasDevice = Manager()->Create(
                    m_debugLevel,
                    directX11Device,
                    GetD2DFactory().Get());

                CheckMakeResult(newCanvasDevice);

                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
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
                GetResource();  // this ensures that Close() hasn't been called
                *value = m_actualHardwareAcceleration;
                return S_OK;
            });
    }

    IFACEMETHODIMP CanvasDevice::get_DirectX11Device(_Out_ IDirectX11Device **value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(value);
                ComPtr<IDirectX11Device> directX11Device = m_directX11Device.EnsureNotClosed();
                ThrowIfFailed(directX11Device.CopyTo(value));
            });
    }

    IFACEMETHODIMP CanvasDevice::Close()
    {
        ResourceWrapper::Close();
        m_directX11Device.Close();
        m_d2dResourceCreationDeviceContext.Close();
        return S_OK;
    }

    ComPtr<ID2D1Device1> CanvasDevice::GetD2DDevice()
    {
        return GetResource();
    }

    CanvasHardwareAcceleration CanvasDevice::GetRequestedHardwareAcceleration()
    {
        return m_requestedHardwareAcceleration;
    }

    ComPtr<ID2D1SolidColorBrush> CanvasDevice::CreateSolidColorBrush(const D2D1_COLOR_F& color)
    {
        // TODO: this isn't very threadsafe - we should really have a different
        // resource creation context per-thread. Do this as part of #802.
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(deviceContext->CreateSolidColorBrush(color, &brush));

        return brush;
    }

    ActivatableClassWithFactory(CanvasDevice, CanvasDeviceFactory);
}
