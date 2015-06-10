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

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
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
    ComPtr<IDXGIDevice3> DefaultDeviceResourceCreationAdapter::GetDxgiDevice(ID2D1Device1* d2dDevice)
    {
        ComPtr<ID2D1DeviceContext> deviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &deviceContext));

        ComPtr<ID2D1Bitmap1> bitmap;
        ThrowIfFailed(deviceContext->CreateBitmap(
            D2D1_SIZE_U{ 1, 1 },
            nullptr,
            0,
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            &bitmap));

        ComPtr<IDXGISurface> surface;
        ThrowIfFailed(bitmap->GetSurface(&surface));

        ComPtr<IDXGIDevice3> device;
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
            ThrowHR(E_INVALIDARG);
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

        CanvasHardwareAcceleration hardwareAcceleration;

        auto dxgiDevice = MakeDXGIDevice(
            requestedHardwareAcceleration,
            &hardwareAcceleration);

        ComPtr<ID2D1Device1> d2dDevice;
        ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));
        
        auto device = Make<CanvasDevice>(
            shared_from_this(), 
            debugLevel,
            hardwareAcceleration,
            dxgiDevice.Get(),
            d2dDevice.Get());
        CheckMakeResult(device);

        return device;
    }

    
    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel,
        IDirect3DDevice* direct3DDevice)
    {
        ThrowIfInvalid(debugLevel);
        CheckInPointer(direct3DDevice);
        auto d2dFactory = m_adapter->CreateD2DFactory(debugLevel);

        return CreateNew(debugLevel, direct3DDevice, d2dFactory.Get());
    }


    ComPtr<CanvasDevice> CanvasDeviceManager::CreateNew(
        CanvasDebugLevel debugLevel,
        IDirect3DDevice* direct3DDevice,
        ID2D1Factory2* d2dFactory)
    {
        ThrowIfInvalid(debugLevel);
        CheckInPointer(direct3DDevice);
        CheckInPointer(d2dFactory);

        ComPtr<IDirect3DDxgiInterfaceAccess> dxgiInterfaceAccess;
        ThrowIfFailed(direct3DDevice->QueryInterface(IID_PPV_ARGS(&dxgiInterfaceAccess)));

        ComPtr<IDXGIDevice3> dxgiDevice;
        ThrowIfFailed(dxgiInterfaceAccess->GetInterface(IID_PPV_ARGS(&dxgiDevice)));

        ComPtr<ID2D1Device1> d2dDevice;
        ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));
        
        auto device = Make<CanvasDevice>(
            shared_from_this(), 
            debugLevel, 
            CanvasHardwareAcceleration::Unknown,
            dxgiDevice.Get(), 
            d2dDevice.Get());
        CheckMakeResult(device);

        return device;
    }

    
    ComPtr<CanvasDevice> CanvasDeviceManager::CreateWrapper(
        ID2D1Device1* d2dDevice)
    {
        auto dxgiDevice = m_adapter->GetDxgiDevice(d2dDevice);

        auto canvasDevice = Make<CanvasDevice>(
            shared_from_this(),
            CanvasDebugLevel::None,
            CanvasHardwareAcceleration::Unknown,
            dxgiDevice.Get(),
            d2dDevice);
        CheckMakeResult(canvasDevice);
        
        return canvasDevice;
    }


    ComPtr<IDXGIDevice3> CanvasDeviceManager::MakeDXGIDevice(
        CanvasHardwareAcceleration requestedHardwareAcceleration,
        CanvasHardwareAcceleration* actualHardwareAccelerationOut) const
    {
        assert(actualHardwareAccelerationOut);
        *actualHardwareAccelerationOut = CanvasHardwareAcceleration::Unknown;

        CanvasHardwareAcceleration actualHardwareAcceleration = CanvasHardwareAcceleration::Unknown;

        ComPtr<ID3D11Device> d3dDevice = MakeD3D11Device(
            requestedHardwareAcceleration,
            &actualHardwareAcceleration);

        ComPtr<IDXGIDevice3> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        *actualHardwareAccelerationOut = actualHardwareAcceleration;

        return dxgiDevice;
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
            ThrowHR(E_INVALIDARG);
        }

        // If we end up here then we failed to create a d3d device
        ThrowHR(E_FAIL);
    }

    ComPtr<ICanvasDevice> CanvasDeviceManager::GetSharedDevice(
        CanvasHardwareAcceleration hardwareAcceleration)
    {
        if (hardwareAcceleration == CanvasHardwareAcceleration::Unknown)
            ThrowHR(E_INVALIDARG, HStringReference(Strings::HardwareAccelerationUnknownIsNotValid).Get());

        //
        // This code, unlike other non-control APIs, cannot rely on the D2D
        // API lock. CanvasDeviceManager keeps its own lock for this purpose.
        //
        Lock lock(m_mutex);

        int cacheIndex = static_cast<int>(hardwareAcceleration);
        assert(cacheIndex < _countof(m_sharedDevices));

        //
        // If the returned device is null, it has the side effect of nulling
        // out m_sharedDevices[cacheIndex].
        //
        ComPtr<ICanvasDevice> device = LockWeakRef<ICanvasDevice>(m_sharedDevices[cacheIndex]);

        if (m_sharedDevices[cacheIndex])
        {
            if (device && FAILED(static_cast<CanvasDevice*>(device.Get())->GetDeviceRemovedErrorCode()))
            {
                device->RaiseDeviceLost();
                m_sharedDevices[cacheIndex].Reset();
            }
        }

        if (!m_sharedDevices[cacheIndex])
        {
            auto newDevice = Create(CanvasDebugLevel::None, hardwareAcceleration);
            m_sharedDevices[cacheIndex] = AsWeak(newDevice.Get());
            return newDevice;
        }
        else
        {
            assert(device);
            return device;
        }

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
            [&]
            {
                CheckAndClearOutPointer(canvasDevice);
                
                auto newCanvasDevice = GetManager()->Create(debugLevel, hardwareAcceleration);
                
                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
    }

    IFACEMETHODIMP CanvasDeviceFactory::CreateFromDirect3D11Device(
        IDirect3DDevice* direct3DDevice,
        CanvasDebugLevel debugLevel,
        ICanvasDevice** canvasDevice)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(direct3DDevice);
                CheckAndClearOutPointer(canvasDevice);

                auto newCanvasDevice = GetManager()->Create(debugLevel, direct3DDevice);

                ThrowIfFailed(newCanvasDevice.CopyTo(canvasDevice));
            });
    }

    _Use_decl_annotations_
        IFACEMETHODIMP CanvasDeviceFactory::ActivateInstance(IInspectable **object)
    {
        return ExceptionBoundary(
            [&]
        {
            CheckAndClearOutPointer(object);

            auto newCanvasDevice = GetManager()->Create(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto);

            ThrowIfFailed(newCanvasDevice.CopyTo(object));
        });
    }

    IFACEMETHODIMP CanvasDeviceFactory::GetSharedDevice(
        CanvasHardwareAcceleration hardwareAcceleration,
        ICanvasDevice** device)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(device);

                auto sharedDevice = GetManager()->GetSharedDevice(hardwareAcceleration);

                ThrowIfFailed(sharedDevice.CopyTo(device));
            });
    }

    //
    // CanvasDevice
    //

    CanvasDevice::CanvasDevice(
        std::shared_ptr<CanvasDeviceManager> deviceManager,
        CanvasDebugLevel debugLevel,
        CanvasHardwareAcceleration hardwareAcceleration,
        IDXGIDevice3* dxgiDevice,
        ID2D1Device1* d2dDevice)
        : ResourceWrapper(deviceManager, d2dDevice)
        , m_hardwareAcceleration(hardwareAcceleration)
        , m_debugLevel(debugLevel)
        , m_dxgiDevice(dxgiDevice)
    {
        CheckInPointer(dxgiDevice);

        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dResourceCreationDeviceContext));

        InitializePrimaryOutput(dxgiDevice);
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
            [&]
            {
                CheckInPointer(value);
                GetResource();  // this ensures that Close() hasn't been called
                *value = m_hardwareAcceleration;
                return S_OK;
            });
    }


    IFACEMETHODIMP CanvasDevice::get_MaximumBitmapSizeInPixels(int32_t* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto& deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();
                UINT32 maximumBitmapSize = deviceContext->GetMaximumBitmapSize();

                assert(maximumBitmapSize <= INT_MAX);

                *value = static_cast<int32_t>(maximumBitmapSize);
            });
    }

    IFACEMETHODIMP CanvasDevice::add_DeviceLost(
        DeviceLostHandlerType* value, 
        EventRegistrationToken* token)
    {        
        return ExceptionBoundary(
            [&]
            {
                GetResource();  // this ensures that Close() hasn't been called
                CheckInPointer(value);
                CheckInPointer(token);

                ThrowIfFailed(m_deviceLostEventList.Add(value, token));
            });
    }

    IFACEMETHODIMP CanvasDevice::remove_DeviceLost(
        EventRegistrationToken token)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // This does not check if this object was closed, so to 
                // allow shutdown paths to freely remove events.
                //
                ThrowIfFailed(m_deviceLostEventList.Remove(token));
            });
    }

    IFACEMETHODIMP CanvasDevice::IsDeviceLost(
        int hresult,
        boolean* value)
    {        
        return ExceptionBoundary(
            [&]
            {
                GetResource();  // this ensures that Close() hasn't been called

                CheckInPointer(value);

                if (DeviceLostException::IsDeviceLostHResult(hresult))
                {
                    *value = GetDeviceRemovedErrorCode() != S_OK;
                }
                else
                {
                    *value = false;
                }
            });
    }

    IFACEMETHODIMP CanvasDevice::RaiseDeviceLost()
    {        
        return ExceptionBoundary(
            [&]
            {
                GetResource();  // this ensures that Close() hasn't been called

                ThrowIfFailed(m_deviceLostEventList.InvokeAll(this, nullptr));
            });
    }

    IFACEMETHODIMP CanvasDevice::Close()
    {
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;
        
        m_dxgiDevice.Close();
        m_d2dResourceCreationDeviceContext.Close();
        m_primaryOutput.Reset();

        return S_OK;
    }

    ComPtr<ID2D1Device1> CanvasDevice::GetD2DDevice()
    {
        return GetResource();
    }

    ComPtr<ID2D1DeviceContext1> CanvasDevice::CreateDeviceContext()
    {
        ComPtr<ID2D1DeviceContext1> dc;
        ThrowIfFailed(GetResource()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc));
        return dc;
    }

    ComPtr<ID2D1SolidColorBrush> CanvasDevice::CreateSolidColorBrush(D2D1_COLOR_F const& color)
    {
        // TODO #802: this isn't very threadsafe - we should really have a different
        // resource creation context per-thread.
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(deviceContext->CreateSolidColorBrush(color, &brush));

        return brush;
    }

    ComPtr<ID2D1Bitmap1> CanvasDevice::CreateBitmapFromWicResource(
        IWICBitmapSource* wicBitmapSource,
        float dpi,
        CanvasAlphaMode alpha)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.pixelFormat.alphaMode = ToD2DAlphaMode(alpha);
        bitmapProperties.dpiX = bitmapProperties.dpiY = dpi;

        ComPtr<ID2D1Bitmap1> bitmap;
        ThrowIfFailed(deviceContext->CreateBitmapFromWicBitmap(wicBitmapSource, &bitmapProperties, &bitmap));

        return bitmap;
    }


    ComPtr<ID2D1Bitmap1> CanvasDevice::CreateRenderTargetBitmap(
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        CanvasAlphaMode alpha)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1Bitmap1> bitmap;
        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        bitmapProperties.dpiX = dpi;
        bitmapProperties.dpiY = dpi;
        bitmapProperties.pixelFormat.format = static_cast<DXGI_FORMAT>(format);
        bitmapProperties.pixelFormat.alphaMode = ToD2DAlphaMode(alpha);

        auto pixelWidth = static_cast<uint32_t>(DipsToPixels(width, dpi));
        auto pixelHeight = static_cast<uint32_t>(DipsToPixels(height, dpi));

        ThrowIfFailed(deviceContext->CreateBitmap(
            D2D1_SIZE_U{ pixelWidth, pixelHeight },
            nullptr, // data 
            0,  // data pitch
            &bitmapProperties, 
            &bitmap));

        return bitmap;
    }
    
    IFACEMETHODIMP CanvasDevice::get_Device(ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
                ComPtr<ICanvasDevice> device(this);
                *value = device.Detach();
            });
    }

    ComPtr<ID2D1BitmapBrush1> CanvasDevice::CreateBitmapBrush(ID2D1Bitmap1* bitmap)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1BitmapBrush1> bitmapBrush;
        ThrowIfFailed(deviceContext->CreateBitmapBrush(bitmap, &bitmapBrush));

        return bitmapBrush;
    }

    ComPtr<ID2D1ImageBrush> CanvasDevice::CreateImageBrush(ID2D1Image* image)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1ImageBrush> imageBrush;
        ThrowIfFailed(deviceContext->CreateImageBrush(image, D2D1::ImageBrushProperties(D2D1::RectF()), &imageBrush));

        return imageBrush;
    }

    ComPtr<ID2D1Image> CanvasDevice::GetD2DImage(ICanvasImage* canvasImage)
    {
        ComPtr<ICanvasImageInternal> imageInternal;
        ThrowIfFailed(canvasImage->QueryInterface(imageInternal.GetAddressOf()));

        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();
        return imageInternal->GetD2DImage(deviceContext.Get());
    }

    IFACEMETHODIMP CanvasDevice::Trim()
    {
        return ExceptionBoundary(
            [&]
            {
                auto& dxgiDevice = m_dxgiDevice.EnsureNotClosed();

                dxgiDevice->Trim();
            });
    }

    IFACEMETHODIMP CanvasDevice::GetInterface(REFIID iid, void** p)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& dxgiDevice = m_dxgiDevice.EnsureNotClosed();

                ThrowIfFailed(dxgiDevice.CopyTo(iid, p));
            });
    }

    ComPtr<ID2D1GradientStopCollection1> CanvasDevice::CreateGradientStopCollection(
        uint32_t gradientStopCount,
        CanvasGradientStop const* gradientStops,
        CanvasEdgeBehavior edgeBehavior,
        CanvasColorSpace preInterpolationSpace,
        CanvasColorSpace postInterpolationSpace,
        CanvasBufferPrecision bufferPrecision,
        CanvasAlphaMode alphaMode)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        std::vector<D2D1_GRADIENT_STOP> d2dGradientStops;
        d2dGradientStops.resize(gradientStopCount);
        for (uint32_t i = 0; i < gradientStopCount; ++i)
        {
            d2dGradientStops[i].color = ToD2DColor(gradientStops[i].Color);
            d2dGradientStops[i].position = gradientStops[i].Position;
        }

        ComPtr<ID2D1GradientStopCollection1> gradientStopCollection;
        ThrowIfFailed(deviceContext->CreateGradientStopCollection(
            &d2dGradientStops[0],
            gradientStopCount,
            static_cast<D2D1_COLOR_SPACE>(preInterpolationSpace),
            static_cast<D2D1_COLOR_SPACE>(postInterpolationSpace),
            ToD2DBufferPrecision(bufferPrecision),
            static_cast<D2D1_EXTEND_MODE>(edgeBehavior),
            ToD2DColorInterpolation(alphaMode),
            gradientStopCollection.GetAddressOf()));

        return gradientStopCollection;
    }

    ComPtr<ID2D1LinearGradientBrush> CanvasDevice::CreateLinearGradientBrush(
        ID2D1GradientStopCollection1* stopCollection)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES linearGradientBrushProperties = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(),
            D2D1::Point2F());

        ComPtr<ID2D1LinearGradientBrush> linearGradientBrush;
        ThrowIfFailed(deviceContext->CreateLinearGradientBrush(
            linearGradientBrushProperties, 
            stopCollection,
            linearGradientBrush.GetAddressOf()));

        return linearGradientBrush;

    }

    ComPtr<ID2D1RadialGradientBrush> CanvasDevice::CreateRadialGradientBrush(
        ID2D1GradientStopCollection1* stopCollection)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientBrushProperties = D2D1::RadialGradientBrushProperties(
            D2D1::Point2F(),
            D2D1::Point2F(), 
            0, 
            0);

        ComPtr<ID2D1RadialGradientBrush> radialGradientBrush;
        ThrowIfFailed(deviceContext->CreateRadialGradientBrush(
            radialGradientBrushProperties,
            stopCollection,
            radialGradientBrush.GetAddressOf()));

        return radialGradientBrush;
    }


    template<typename FN>
    ComPtr<IDXGISwapChain1> CanvasDevice::CreateSwapChain(
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaMode alphaMode,
        FN&& createFn)
    {
        auto& dxgiDevice = m_dxgiDevice.EnsureNotClosed();

        ComPtr<IDXGIAdapter2> dxgiAdapter;
        ThrowIfFailed(dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)));

        ComPtr<IDXGIFactory2> dxgiFactory;
        ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

        ThrowIfNegative(widthInPixels);
        ThrowIfNegative(heightInPixels);
        ThrowIfNegative(bufferCount);

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
        swapChainDesc.Width = static_cast<UINT>(widthInPixels);
        swapChainDesc.Height = static_cast<UINT>(heightInPixels);
        swapChainDesc.Format = static_cast<DXGI_FORMAT>(format);
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferCount = static_cast<UINT>(bufferCount);
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.AlphaMode = ToDxgiAlphaMode(alphaMode);

        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(createFn(dxgiFactory.Get(), dxgiDevice.Get(), &swapChainDesc, &swapChain));

        return swapChain;
    }


    ComPtr<IDXGISwapChain1> CanvasDevice::CreateSwapChainForComposition(
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaMode alphaMode)
    {
        return CreateSwapChain(widthInPixels, heightInPixels, format, bufferCount, alphaMode,
            [] (IDXGIFactory2* factory, IDXGIDevice3* device, DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain)
            {
                return factory->CreateSwapChainForComposition(
                    device, 
                    desc, 
                    nullptr,  // restrictToOutput
                    swapChain);
            });
    }

    ComPtr<IDXGISwapChain1> CanvasDevice::CreateSwapChainForCoreWindow(
        ICoreWindow* coreWindow,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaMode alphaMode)
    {
        return CreateSwapChain(widthInPixels, heightInPixels, format, bufferCount, alphaMode,
            [coreWindow] (IDXGIFactory2* factory, IDXGIDevice3* device, DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain)
            {
                return factory->CreateSwapChainForCoreWindow(
                    device,
                    coreWindow,
                    desc,
                    nullptr,    // restrictToOutput
                    swapChain);
            });
    }


    ComPtr<ID2D1CommandList> CanvasDevice::CreateCommandList()
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1CommandList> cl;
        ThrowIfFailed(deviceContext->CreateCommandList(&cl));

        return cl;
    }

    ComPtr<ID2D1RectangleGeometry> CanvasDevice::CreateRectangleGeometry(D2D1_RECT_F const& rectangle)
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1RectangleGeometry> rectangleGeometry;
        ThrowIfFailed(factory->CreateRectangleGeometry(rectangle, &rectangleGeometry));

        return rectangleGeometry;
    }

    ComPtr<ID2D1EllipseGeometry> CanvasDevice::CreateEllipseGeometry(D2D1_ELLIPSE const& ellipse)
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1EllipseGeometry> ellipseGeometry;
        ThrowIfFailed(factory->CreateEllipseGeometry(ellipse, &ellipseGeometry));

        return ellipseGeometry;
    }

    ComPtr<ID2D1RoundedRectangleGeometry> CanvasDevice::CreateRoundedRectangleGeometry(D2D1_ROUNDED_RECT const& roundedRect)
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1RoundedRectangleGeometry> roundedRectangleGeometry;
        ThrowIfFailed(factory->CreateRoundedRectangleGeometry(roundedRect, &roundedRectangleGeometry));

        return roundedRectangleGeometry;
    }

    ComPtr<ID2D1PathGeometry1> CanvasDevice::CreatePathGeometry()
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1PathGeometry1> pathGeometry;
        ThrowIfFailed(factory->CreatePathGeometry(&pathGeometry));

        return pathGeometry;
    }

    ComPtr<ID2D1GeometryGroup> CanvasDevice::CreateGeometryGroup(D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount)
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1GeometryGroup> geometryGroup;
        ThrowIfFailed(factory->CreateGeometryGroup(fillMode, d2dGeometries, geometryCount, &geometryGroup));

        return geometryGroup;
    }

    ComPtr<ID2D1TransformedGeometry> CanvasDevice::CreateTransformedGeometry(ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform)
    {
        auto factory = GetD2DFactory();

        ComPtr<ID2D1TransformedGeometry> transformedGeometry;
        ThrowIfFailed(factory->CreateTransformedGeometry(d2dGeometry, transform, &transformedGeometry));

        return transformedGeometry;
    }

    ComPtr<ID2D1GeometryRealization> CanvasDevice::CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1GeometryRealization> geometryRealization;
        ThrowIfFailed(deviceContext->CreateFilledGeometryRealization(geometry, flatteningTolerance, &geometryRealization));

        return geometryRealization;
    }

    ComPtr<ID2D1GeometryRealization> CanvasDevice::CreateStrokedGeometryRealization(
        ID2D1Geometry* geometry,
        float strokeWidth,
        ID2D1StrokeStyle* strokeStyle,
        float flatteningTolerance)
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        ComPtr<ID2D1GeometryRealization> geometryRealization;
        ThrowIfFailed(deviceContext->CreateStrokedGeometryRealization(
            geometry,
            flatteningTolerance,
            strokeWidth,
            strokeStyle,
            &geometryRealization));

        return geometryRealization;
    }

    ComPtr<ID2D1DeviceContext1> CanvasDevice::GetResourceCreationDeviceContext()
    {
        auto deviceContext = m_d2dResourceCreationDeviceContext.EnsureNotClosed();

        return deviceContext;
    }

    void CanvasDevice::InitializePrimaryOutput(IDXGIDevice3* dxgiDevice)
    {
        //
        // Creating a CanvasDevice using CanvasHardwareAcceleration::Off 
        // creates a 'render-only' WARP device, which cannot be used to 
        // enumerate outputs.
        //
        // This is distinct from basic display, where EnumOutputs still works normally.
        //
        // But rather than check if hardware acceleration is on or off
        // before calling EnumOutputs, we try it and just let it fail
        // if it's not supported. Reason being, interop.
        //
        ComPtr<IDXGIAdapter> adapter;
        ThrowIfFailed(dxgiDevice->GetAdapter(&adapter));

        //
        // This holds onto the primary output, which is the recommendation
        // for synchronizing with vertical blank.
        //
        // If the primary display changes to a different one, or if
        // the monitor is no longer is plugged in, WaitForVBlank will
        // be emulated and the emulated behavior has shown to be reliable.
        //
        // In the uncommon scenario where no display is plugged in at
        // all, an emulated primary will still be enumerated here.
        //
        HRESULT enumOutputsHr = adapter->EnumOutputs(0, &m_primaryOutput);
        if (FAILED(enumOutputsHr) && enumOutputsHr != DXGI_ERROR_NOT_FOUND)
        {
            ThrowHR(enumOutputsHr);
        }
    }

    ComPtr<IDXGIOutput> CanvasDevice::GetPrimaryDisplayOutput()
    {
        return m_primaryOutput;
    }

    HRESULT CanvasDevice::GetDeviceRemovedErrorCode()
    {
        auto& dxgiDevice = m_dxgiDevice.EnsureNotClosed();

        auto d3dDevice = As<ID3D11Device>(dxgiDevice);

        return d3dDevice->GetDeviceRemovedReason();
    }

    ActivatableClassWithFactory(CanvasDevice, CanvasDeviceFactory);
}}}}
