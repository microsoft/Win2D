// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;

    //
    // CanvasRenderTargetManager
    //
    CanvasRenderTargetManager::CanvasRenderTargetManager()
        : m_adapter(CanvasBitmapFactory::GetManager()->GetAdapter())    // TODO interop fix this
    {
    }

    CanvasRenderTargetManager::CanvasRenderTargetManager(
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_adapter(adapter)
    {
    }

    ComPtr<CanvasRenderTarget> CanvasRenderTargetManager::CreateNew(
        ICanvasDevice* canvasDevice,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        CanvasAlphaMode alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateRenderTargetBitmap(width, height, dpi, format, alpha);

        return Make<CanvasRenderTarget>(d2dBitmap.Get(), canvasDevice);
    }


    ComPtr<CanvasRenderTarget> CanvasRenderTargetManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1Bitmap1* d2dBitmap)
    {
        auto renderTarget = Make<CanvasRenderTarget>(
            d2dBitmap,
            device);
        CheckMakeResult(renderTarget);
        
        return renderTarget;
    }

    ICanvasBitmapResourceCreationAdapter* CanvasRenderTargetManager::GetAdapter()
    {
        return m_adapter.get();
    }


    //
    // CanvasRenderTargetFactory
    //


    CanvasRenderTargetFactory::CanvasRenderTargetFactory()
    {
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithSize(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        Size size,
        ICanvasRenderTarget** renderTarget)
    {
        return CreateWithWidthAndHeight(
            resourceCreator,
            size.Width,
            size.Height,
            renderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeight(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        float width,
        float height,
        ICanvasRenderTarget** renderTarget)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(renderTarget);

                float dpi;
                ThrowIfFailed(resourceCreator->get_Dpi(&dpi));

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(As<ICanvasResourceCreator>(resourceCreator)->get_Device(&canvasDevice));

                auto bitmap = GetManager()->CreateNew(
                    canvasDevice.Get(), 
                    width, 
                    height, 
                    dpi,
                    PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                    CanvasAlphaMode::Premultiplied);

                ThrowIfFailed(bitmap.CopyTo(renderTarget));
            });
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeightAndDpi(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        float dpi,
        ICanvasRenderTarget** renderTarget)
    {
        return CreateWithWidthAndHeightAndDpiAndFormatAndAlpha(
            resourceCreator,
            width,
            height,
            dpi,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            CanvasAlphaMode::Premultiplied,
            renderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeightAndDpiAndFormatAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        float dpi,
        DirectXPixelFormat format,
        CanvasAlphaMode alpha,
        ICanvasRenderTarget** renderTarget)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(renderTarget);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto bitmap = GetManager()->CreateNew(
                    canvasDevice.Get(), 
                    width, 
                    height, 
                    dpi,
                    format, 
                    alpha);

                ThrowIfFailed(bitmap.CopyTo(renderTarget));
            });
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11Surface(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        ICanvasRenderTarget** canvasRenderTarget)
    {
        return CreateFromDirect3D11SurfaceWithDpiAndAlpha(
            resourceCreator,
            surface,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasRenderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11SurfaceWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        ICanvasRenderTarget** canvasRenderTarget)
    {
        return CreateFromDirect3D11SurfaceWithDpiAndAlpha(
            resourceCreator,
            surface,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasRenderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11SurfaceWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasRenderTarget** canvasRenderTarget)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(surface);
                CheckAndClearOutPointer(canvasRenderTarget);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto d2dBitmap = As<ICanvasDeviceInternal>(canvasDevice)->CreateBitmapFromSurface(surface, dpi, alpha);

                if (!IsRenderTargetBitmap(d2dBitmap.Get()))
                    ThrowHR(E_INVALIDARG);

                auto newRenderTarget = Make<CanvasRenderTarget>(d2dBitmap.Get(), canvasDevice.Get());
                CheckMakeResult(newRenderTarget);

                ThrowIfFailed(newRenderTarget.CopyTo(canvasRenderTarget));
            });
    }


    static ComPtr<ICanvasDrawingSession> CreateDrawingSessionOverD2DBitmap(
        ICanvasDevice* owner,
        ID2D1Bitmap1* targetBitmap)
    {
        assert(owner != nullptr);
        assert(targetBitmap != nullptr);

        //
        // Create a new ID2D1DeviceContext
        //
        auto deviceContext = As<ICanvasDeviceInternal>(owner)->CreateDeviceContextForDrawingSession();

        //
        // Set the target
        //
        deviceContext->SetTarget(targetBitmap);

        //
        // Set the DPI
        //
        float dpiX, dpiY;
        targetBitmap->GetDpi(&dpiX, &dpiY);
        deviceContext->SetDpi(dpiX, dpiY);

        auto adapter = std::make_shared<SimpleCanvasDrawingSessionAdapter>(deviceContext.Get());

        return CanvasDrawingSession::CreateNew(deviceContext.Get(), adapter, owner);
    }

    //
    // CanvasRenderTarget
    //


    CanvasRenderTarget::CanvasRenderTarget(
        ID2D1Bitmap1* d2dBitmap,
        ICanvasDevice* canvasDevice)
        : CanvasBitmapImpl(d2dBitmap, canvasDevice)
    {
        assert(IsRenderTargetBitmap(d2dBitmap) 
            && "CanvasRenderTarget should never be constructed with a non-target bitmap.  This should have been validated before construction.");
    }


    IFACEMETHODIMP CanvasRenderTarget::CreateDrawingSession(
        _COM_Outptr_ ICanvasDrawingSession** drawingSession)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(drawingSession);
                
                auto& resource = GetD2DBitmap();

                auto newDrawingSession = CreateDrawingSessionOverD2DBitmap(
                    m_device.Get(),
                    resource.Get());

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
            });
    }


    ActivatableClassWithFactory(CanvasRenderTarget, CanvasRenderTargetFactory);
}}}}
