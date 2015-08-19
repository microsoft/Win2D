// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    //
    // CanvasSwapChainAdapter
    //

    class CanvasSwapChainAdapter : public ICanvasSwapChainAdapter
    {
    public:

        virtual void Sleep(DWORD timeInMs) override
        {
            ::Sleep(timeInMs);
        }

    };

    //
    // CanvasSwapChainFactory
    //

    CanvasSwapChainFactory::CanvasSwapChainFactory()
    {

    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithSize(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        Size size,
        ICanvasSwapChain** swapChain)
    {
        return CreateWithWidthAndHeight(
            resourceCreator,
            size.Width,
            size.Height,
            swapChain);
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithWidthAndHeight(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        float width,
        float height,
        ICanvasSwapChain** swapChain)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(swapChain);
                
                float dpi;
                ThrowIfFailed(resourceCreator->get_Dpi(&dpi));

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(As<ICanvasResourceCreator>(resourceCreator)->get_Device(&device));

                auto newCanvasSwapChain = GetManager()->CreateNew(
                    device.Get(),
                    width,
                    height,
                    dpi,
                    CanvasSwapChain::DefaultPixelFormat,
                    CanvasSwapChain::DefaultBufferCount,
                    CanvasSwapChain::DefaultCompositionAlphaMode);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(swapChain));
            });
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithWidthAndHeightAndDpi(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        float dpi,
        ICanvasSwapChain** swapChain)
    {
        return CreateWithAllOptions(
            resourceCreator, 
            width, 
            height, 
            dpi,
            CanvasSwapChain::DefaultPixelFormat,
            CanvasSwapChain::DefaultBufferCount, 
            CanvasSwapChain::DefaultCompositionAlphaMode, 
            swapChain);
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithAllOptions(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaMode alphaMode,
        ICanvasSwapChain** swapChain)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(swapChain);

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));

                auto newCanvasSwapChain = GetManager()->CreateNew(
                    device.Get(),
                    width,
                    height,
                    dpi,
                    format,
                    bufferCount,
                    alphaMode);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(swapChain));
            });
    }

    //
    // ICanvasSwapChainStatics
    //

    IFACEMETHODIMP CanvasSwapChainFactory::CreateForCoreWindowWithDpi(
        ICanvasResourceCreator* resourceCreator,
        ICoreWindow* coreWindow,
        float dpi,
        ICanvasSwapChain** swapChain)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(swapChain);

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));

                auto newCanvasSwapChain = GetManager()->CreateNew(
                    device.Get(),
                    coreWindow,
                    dpi);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(swapChain));
            });
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateForCoreWindowWithAllOptions(
        ICanvasResourceCreator* resourceCreator,
        ICoreWindow* coreWindow,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        int32_t bufferCount,
        ICanvasSwapChain** swapChain)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(swapChain);

                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));

                auto newCanvasSwapChain = GetManager()->CreateNew(
                    device.Get(),
                    coreWindow,
                    width,
                    height,
                    dpi,
                    format,
                    bufferCount);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(swapChain));
            });
    }
    
    CanvasSwapChain::CanvasSwapChain(
        ICanvasDevice* device,
        std::shared_ptr<ICanvasSwapChainAdapter> swapChainAdapter,
        IDXGISwapChain1* dxgiSwapChain,
        float dpi)
        : ResourceWrapper(dxgiSwapChain)
        , m_device(device)
        , m_dpi(dpi)
        , m_adapter(swapChainAdapter)
    {
    }

    IFACEMETHODIMP CanvasSwapChain::get_Size(Size* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto desc = GetResourceDescription();

                *value = Size{ PixelsToDips(desc.Width, m_dpi),
                               PixelsToDips(desc.Height, m_dpi) };
        });
    }

    IFACEMETHODIMP CanvasSwapChain::get_SizeInPixels(BitmapSize* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto desc = GetResourceDescription();

                *value = BitmapSize{ desc.Width, desc.Height };
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_Dpi(float *value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = m_dpi;
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_Format(DirectXPixelFormat* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = static_cast<DirectXPixelFormat>(GetResourceDescription().Format);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_BufferCount(int32_t* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = GetResourceDescription().BufferCount;
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_AlphaMode(CanvasAlphaMode* value) 
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = FromDxgiAlphaMode(GetResourceDescription().AlphaMode);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_Rotation(CanvasSwapChainRotation* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto& resource = GetResource();

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                DXGI_MODE_ROTATION rotation;
                ThrowIfFailed(resource->GetRotation(&rotation));

                *value = FromDxgiRotation(rotation);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::put_Rotation(CanvasSwapChainRotation value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                ThrowIfFailed(resource->SetRotation(ToDxgiRotation(value)));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_SourceSize(Size* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                uint32_t width;
                uint32_t height;

                auto swapChain = As<IDXGISwapChain2>(GetResource());

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                ThrowIfFailed(swapChain->GetSourceSize(&width, &height));

                *value = Size{ PixelsToDips(width, m_dpi),
                               PixelsToDips(height, m_dpi) };
            });
    }

    IFACEMETHODIMP CanvasSwapChain::put_SourceSize(Size value)
    {
        return ExceptionBoundary(
            [&]
            {
                uint32_t width = SizeDipsToPixels(value.Width, m_dpi);
                uint32_t height = SizeDipsToPixels(value.Height, m_dpi);

                auto swapChain = As<IDXGISwapChain2>(GetResource());

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                ThrowIfFailed(swapChain->SetSourceSize(width, height));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_TransformMatrix(Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto swapChain = As<IDXGISwapChain2>(GetResource());

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                DXGI_MATRIX_3X2_F matrix = GetMatrixInternal(lock, swapChain);

                *value = *ReinterpretAs<Matrix3x2*>(&matrix);
            });
    }

    DXGI_MATRIX_3X2_F CanvasSwapChain::GetMatrixInternal(D2DResourceLock const&, ComPtr<IDXGISwapChain2> const& swapChain)
    {
        DXGI_MATRIX_3X2_F matrix;

        ThrowIfFailed(swapChain->GetMatrixTransform(&matrix));

        // Remove our extra DPI scaling from the transform.
        float dpiScale = m_dpi / DEFAULT_DPI;

        matrix._11 *= dpiScale;
        matrix._12 *= dpiScale;
        matrix._21 *= dpiScale;
        matrix._22 *= dpiScale;
        matrix._31 *= dpiScale;
        matrix._32 *= dpiScale;

        return matrix;
    }

    IFACEMETHODIMP CanvasSwapChain::put_TransformMatrix(Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]
            {
                // Insert additional scaling to account for display DPI.
                auto swapChain = As<IDXGISwapChain2>(GetResource());

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                SetMatrixInternal(lock, swapChain, ReinterpretAs<DXGI_MATRIX_3X2_F*>(&value));
            });
    }

    void CanvasSwapChain::SetMatrixInternal(D2DResourceLock const&, ComPtr<IDXGISwapChain2> const& swapChain, DXGI_MATRIX_3X2_F* transform)
    {
        float dpiScale = DEFAULT_DPI / m_dpi;

        transform->_11 *= dpiScale;
        transform->_12 *= dpiScale;
        transform->_21 *= dpiScale;
        transform->_22 *= dpiScale;
        transform->_31 *= dpiScale;
        transform->_32 *= dpiScale;

        ThrowIfFailed(swapChain->SetMatrixTransform(transform));
    }

    IFACEMETHODIMP CanvasSwapChain::ConvertPixelsToDips(int pixels, float* dips)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(dips);

                *dips = PixelsToDips(pixels, m_dpi);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ConvertDipsToPixels(float dips, CanvasDpiRounding dpiRounding, int* pixels)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(pixels);

                *pixels = DipsToPixels(dips, m_dpi, dpiRounding);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::Present() 
    {
        return PresentWithSyncInterval(1);
    }

    IFACEMETHODIMP CanvasSwapChain::PresentWithSyncInterval(int32_t syncInterval)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
                ThrowIfFailed(resource->Present1(syncInterval, 0, &presentParameters));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithSize(
        Size newSize)
    {
        return ResizeBuffersWithWidthAndHeight(
            newSize.Width, 
            newSize.Height);
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithWidthAndHeight(
        float newWidth,
        float newHeight)
    {
        return ExceptionBoundary(
            [&]
            {
                auto desc = GetResourceDescription();

                ThrowIfFailed(ResizeBuffersWithAllOptions(
                    newWidth,
                    newHeight,
                    m_dpi,
                    static_cast<DirectXPixelFormat>(desc.Format),
                    desc.BufferCount));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithWidthAndHeightAndDpi(
        float newWidth,
        float newHeight,
        float newDpi)
    {
        return ExceptionBoundary(
            [&]
            {
                auto desc = GetResourceDescription();

                ThrowIfFailed(ResizeBuffersWithAllOptions(
                    newWidth,
                    newHeight,
                    newDpi,
                    static_cast<DirectXPixelFormat>(desc.Format),
                    desc.BufferCount));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithAllOptions(
        float newWidth,
        float newHeight,
        float newDpi,
        DirectXPixelFormat newFormat,
        int32_t bufferCount)
    {
        return ExceptionBoundary(
            [&]
            {
                auto resource = As<IDXGISwapChain2>(GetResource());

                auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                SetDpi(lock, resource, newDpi);

                int widthInPixels = SizeDipsToPixels(newWidth, m_dpi);
                int heightInPixels = SizeDipsToPixels(newHeight, m_dpi);

                ThrowIfNegative(bufferCount);
                ThrowIfNegative(widthInPixels);
                ThrowIfNegative(heightInPixels);

                ThrowIfFailed(resource->ResizeBuffers(
                    bufferCount, 
                    widthInPixels,
                    heightInPixels,
                    static_cast<DXGI_FORMAT>(newFormat), 
                    0));
            });
    }

    void CanvasSwapChain::SetDpi(D2DResourceLock const& lock, ComPtr<IDXGISwapChain2> const& swapChain, float newDpi)
    {
        DXGI_MATRIX_3X2_F dpiIndependentTransform = GetMatrixInternal(lock, swapChain);

        m_dpi = newDpi;

        SetMatrixInternal(lock, swapChain, &dpiIndependentTransform);
    }

    // IClosable

    IFACEMETHODIMP CanvasSwapChain::Close()
    {
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;

        m_device.Close();
        return S_OK;
    }

    IFACEMETHODIMP CanvasSwapChain::get_Device(ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
            CheckInPointer(value);

                auto& device = m_device.EnsureNotClosed();

                ThrowIfFailed(device.CopyTo(value));
            });
    }


    DXGI_SWAP_CHAIN_DESC1 CanvasSwapChain::GetResourceDescription()
    {
        auto& resource = GetResource();

        auto d2dDevice = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed())->GetD2DDevice();
        D2DResourceLock lock(d2dDevice.Get());

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ThrowIfFailed(resource->GetDesc1(&swapChainDesc));

        return swapChainDesc;
    }

    class CanvasSwapChainDrawingSessionAdapter : public DrawingSessionBaseAdapter,
                                                 private LifespanTracker<CanvasSwapChainDrawingSessionAdapter>
    {
        ComPtr<ID2D1DeviceContext1> m_deviceContext;

    public:
        static std::shared_ptr<CanvasSwapChainDrawingSessionAdapter> Create(
            ICanvasDevice* owner,
            IDXGISwapChain1* swapChainResource,
            D2D1_COLOR_F const& clearColor,
            float dpi,
            ID2D1DeviceContext1** outDeviceContext)
        {
            auto deviceContext = As<ICanvasDeviceInternal>(owner)->CreateDeviceContext();

            DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
            ThrowIfFailed(swapChainResource->GetDesc1(&swapChainDescription));

            ComPtr<IDXGISurface2> backBufferSurface;
            ThrowIfFailed(swapChainResource->GetBuffer(0, IID_PPV_ARGS(&backBufferSurface)));

            ComPtr<ID2D1Bitmap1> d2dTargetBitmap;
            D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
            bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
            bitmapProperties.pixelFormat.format = swapChainDescription.Format;
            bitmapProperties.pixelFormat.alphaMode = ConvertDxgiAlphaModeToD2DAlphaMode(swapChainDescription.AlphaMode);
            ThrowIfFailed(deviceContext->CreateBitmapFromDxgiSurface(backBufferSurface.Get(), &bitmapProperties, &d2dTargetBitmap));

            deviceContext->SetTarget(d2dTargetBitmap.Get());

            deviceContext->BeginDraw();

            //
            // If this function fails then we need to call EndDraw
            //
            auto endDrawWarden = MakeScopeWarden([&] { ThrowIfFailed(deviceContext->EndDraw()); });

            ThrowIfFailed(deviceContext.CopyTo(outDeviceContext));

            auto adapter = std::make_shared<CanvasSwapChainDrawingSessionAdapter>(deviceContext.Get());

            deviceContext->Clear(&clearColor);

            deviceContext->SetDpi(dpi, dpi);

            //
            // This function can't fail now, so we can dismiss the end draw warden.
            //
            endDrawWarden.Dismiss();

            return adapter;
        }

        CanvasSwapChainDrawingSessionAdapter(ID2D1DeviceContext1* deviceContext)
            : m_deviceContext(deviceContext)
        {

        }

        virtual void EndDraw() override
        {
            ThrowIfFailed(m_deviceContext->EndDraw());
        }

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset()
        {
            return D2D1::Point2F(0, 0);
        }
    };

    IFACEMETHODIMP CanvasSwapChain::CreateDrawingSession(
        Color clearColor,
        ICanvasDrawingSession** drawingSession) 
    {
        return ExceptionBoundary(
            [&]
            {            
                CheckAndClearOutPointer(drawingSession);
                auto& dxgiSwapChain = GetResource();
                auto& device = m_device.EnsureNotClosed();

                auto d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();
                D2DResourceLock lock(d2dDevice.Get());

                ComPtr<ID2D1DeviceContext1> deviceContext;
                auto adapter = CanvasSwapChainDrawingSessionAdapter::Create(
                    device.Get(),
                    dxgiSwapChain.Get(),
                    ToD2DColor(clearColor),
                    m_dpi,
                    &deviceContext);
                
                auto drawingSessionManager = CanvasDrawingSessionFactory::GetManager();
                auto newDrawingSession = drawingSessionManager->CreateNew(deviceContext.Get(), adapter);

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::WaitForVerticalBlank()
    {
        return ExceptionBoundary(
            [&]
            {            
                auto& device = m_device.EnsureNotClosed();

                auto deviceInternal = As<ICanvasDeviceInternal>(device);

                auto output = deviceInternal->GetPrimaryDisplayOutput();

                //
                // Only hardware devices and WARP devices under MSBDA have
                // access to display output. We do a yield on
                // render-only (explicitly requested) SW devices, where 
                // the output will be null.
                //
                if (output)
                {
                    ThrowIfFailed(output->WaitForVBlank());
                }
                else
                {
                    m_adapter->Sleep(0);
                }
            });
    }

    CanvasSwapChainManager::CanvasSwapChainManager()
        : m_adapter(std::make_shared<CanvasSwapChainAdapter>())
    {}

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateNew(
        ICanvasDevice* device,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaMode alphaMode)
    {
        auto deviceInternal = As<ICanvasDeviceInternal>(device);

        int widthInPixels = SizeDipsToPixels(width, dpi);
        int heightInPixels = SizeDipsToPixels(height, dpi);

        ComPtr<IDXGISwapChain1> dxgiSwapChain = deviceInternal->CreateSwapChainForComposition(
            widthInPixels,
            heightInPixels,
            format,
            bufferCount,
            alphaMode);

        auto canvasSwapChain = Make<CanvasSwapChain>(
            device,
            m_adapter,
            dxgiSwapChain.Get(),
            dpi);
        CheckMakeResult(canvasSwapChain);

        ThrowIfFailed(canvasSwapChain->put_TransformMatrix(Matrix3x2{ 1, 0, 0, 1, 0, 0 }));

        return canvasSwapChain;
    }

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateNew(
        ICanvasDevice* device,
        ICoreWindow* coreWindow,
        float dpi)
    {
        CheckInPointer(device);
        CheckInPointer(coreWindow);

        Rect bounds;
        ThrowIfFailed(coreWindow->get_Bounds(&bounds));

        return CreateNew(
            device,
            coreWindow,
            bounds.Width,
            bounds.Height,
            dpi,
            CanvasSwapChain::DefaultPixelFormat,
            CanvasSwapChain::DefaultBufferCount);
    }

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateNew(
        ICanvasDevice* device,
        ICoreWindow* coreWindow,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        int32_t bufferCount)
    {
        CheckInPointer(device);
        CheckInPointer(coreWindow);

        auto dxgiSwapChain = As<ICanvasDeviceInternal>(device)->CreateSwapChainForCoreWindow(
            coreWindow,
            SizeDipsToPixels(width, dpi),
            SizeDipsToPixels(height, dpi),
            format,
            bufferCount,
            CanvasAlphaMode::Ignore);

        auto canvasSwapChain = Make<CanvasSwapChain>(
            device,
            m_adapter,
            dxgiSwapChain.Get(),
            dpi);
        CheckMakeResult(canvasSwapChain);

        return canvasSwapChain;
    }

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateWrapper(
        ICanvasDevice* device,
        IDXGISwapChain1* resource,
        float dpi)
    {
        auto swapChain = Make<CanvasSwapChain>(
            device,
            m_adapter,
            resource,
            dpi);
        CheckMakeResult(swapChain);

        return swapChain;
    }

    ActivatableClassWithFactory(CanvasSwapChain, CanvasSwapChainFactory);
}}}}
