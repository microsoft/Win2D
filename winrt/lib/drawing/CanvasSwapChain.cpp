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
    // CanvasSwapChainFactory
    //

    CanvasSwapChainFactory::CanvasSwapChainFactory()
    {

    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithSize(
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

                auto newCanvasSwapChain = GetManager()->Create(
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

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithSizeAndDpi(
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

                auto newCanvasSwapChain = GetManager()->Create(
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

                auto newCanvasSwapChain = GetManager()->Create(
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

                auto newCanvasSwapChain = GetManager()->Create(
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
    

    //
    // ICanvasDeviceResourceWithDpiFactoryNative
    //


    IFACEMETHODIMP CanvasSwapChainFactory::GetOrCreate(
        ICanvasDevice* device,
        IUnknown* resource,
        float dpi,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<IDXGISwapChain1> dxgiSwapChain;
                ThrowIfFailed(resource->QueryInterface(dxgiSwapChain.GetAddressOf()));

                auto newCanvasSwapChain = GetManager()->GetOrCreate(device, dxgiSwapChain.Get(), dpi);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(wrapper));
            });
    }

    CanvasSwapChain::CanvasSwapChain(
        ICanvasDevice* device,
        std::shared_ptr<CanvasSwapChainManager> swapChainManager,
        IDXGISwapChain1* dxgiSwapChain,
        float dpi)
        : ResourceWrapper(swapChainManager, dxgiSwapChain)
        , m_device(device)
        , m_dpi(dpi)
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
                uint32_t width = DipsToPixels(value.Width, m_dpi);
                uint32_t height = DipsToPixels(value.Height, m_dpi);

                auto swapChain = As<IDXGISwapChain2>(GetResource());
                ThrowIfFailed(swapChain->SetSourceSize(width, height));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_TransformMatrix(Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);


                DXGI_MATRIX_3X2_F matrix;

                auto swapChain = As<IDXGISwapChain2>(GetResource());
                ThrowIfFailed(swapChain->GetMatrixTransform(&matrix));

                // Remove our extra DPI scaling from the transform.
                float dpiScale = m_dpi / DEFAULT_DPI;

                matrix._11 *= dpiScale;
                matrix._12 *= dpiScale;
                matrix._21 *= dpiScale;
                matrix._22 *= dpiScale;
                matrix._31 *= dpiScale;
                matrix._32 *= dpiScale;

                *value = *ReinterpretAs<Matrix3x2*>(&matrix);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::put_TransformMatrix(Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]
            {
                // Insert additional scaling to account for display DPI.
                float dpiScale = DEFAULT_DPI / m_dpi;

                value.M11 *= dpiScale;
                value.M12 *= dpiScale;
                value.M21 *= dpiScale;
                value.M22 *= dpiScale;
                value.M31 *= dpiScale;
                value.M32 *= dpiScale;

                auto swapChain = As<IDXGISwapChain2>(GetResource());
                ThrowIfFailed(swapChain->SetMatrixTransform(ReinterpretAs<DXGI_MATRIX_3X2_F*>(&value)));
            });
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

    IFACEMETHODIMP CanvasSwapChain::ConvertDipsToPixels(float dips, int* pixels)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(pixels);

                *pixels = DipsToPixels(dips, m_dpi);
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

                DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
                ThrowIfFailed(resource->Present1(syncInterval, 0, &presentParameters));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithSize(
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

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffersWithSizeAndDpi(
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
                auto& resource = GetResource();

                m_dpi = newDpi;

                int widthInPixels = DipsToPixels(newWidth, m_dpi);
                int heightInPixels = DipsToPixels(newHeight, m_dpi);

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

    // IClosable

    IFACEMETHODIMP CanvasSwapChain::Close()
    {
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;

        m_device.Close();
        m_primaryOutput.Close();
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

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ThrowIfFailed(resource->GetDesc1(&swapChainDesc));

        return swapChainDesc;
    }

    class CanvasSwapChainDrawingSessionAdapter : public ICanvasDrawingSessionAdapter,
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

                ComPtr<ID2D1DeviceContext1> deviceContext;
                auto adapter = CanvasSwapChainDrawingSessionAdapter::Create(
                    device.Get(),
                    dxgiSwapChain.Get(),
                    ToD2DColor(clearColor),
                    m_dpi,
                    &deviceContext);
                
                auto drawingSessionManager = CanvasDrawingSessionFactory::GetOrCreateManager();
                auto newDrawingSession = drawingSessionManager->Create(deviceContext.Get(), adapter);

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

                ThrowIfFailed(output->WaitForVBlank());
            });
    }

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

        int widthInPixels = DipsToPixels(width, dpi);
        int heightInPixels = DipsToPixels(height, dpi);

        ComPtr<IDXGISwapChain1> dxgiSwapChain = deviceInternal->CreateSwapChainForComposition(
            widthInPixels,
            heightInPixels,
            format,
            bufferCount,
            alphaMode);

        auto canvasSwapChain = Make<CanvasSwapChain>(
            device,
            shared_from_this(),
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
            DipsToPixels(width, dpi),
            DipsToPixels(height, dpi),
            format,
            bufferCount,
            CanvasAlphaMode::Ignore);

        auto canvasSwapChain = Make<CanvasSwapChain>(
            device,
            shared_from_this(),
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
            shared_from_this(),
            resource,
            dpi);
        CheckMakeResult(swapChain);

        return swapChain;
    }

    ActivatableClassWithFactory(CanvasSwapChain, CanvasSwapChainFactory);
}}}}
