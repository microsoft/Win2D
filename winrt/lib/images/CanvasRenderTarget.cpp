// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;


    //
    // CanvasRenderTargetFactory
    //

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

                auto bitmap = CanvasRenderTarget::CreateNew(
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

                auto bitmap = CanvasRenderTarget::CreateNew(
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

                auto newRenderTarget = Make<CanvasRenderTarget>(canvasDevice.Get(), d2dBitmap.Get());
                CheckMakeResult(newRenderTarget);

                ThrowIfFailed(newRenderTarget.CopyTo(canvasRenderTarget));
            });
    }


    static ComPtr<ICanvasDrawingSession> CreateDrawingSessionOverD2DBitmap(
        ICanvasDevice* owner,
        ID2D1Bitmap1* targetBitmap,
        std::shared_ptr<bool> hasActiveDrawingSession)
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

        return CanvasDrawingSession::CreateNew(deviceContext.Get(), adapter, owner, std::move(hasActiveDrawingSession));
    }


    //
    // CanvasRenderTarget
    //


    ComPtr<CanvasRenderTarget> CanvasRenderTarget::CreateNew(
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

        return Make<CanvasRenderTarget>(canvasDevice, d2dBitmap.Get());
    }


    CanvasRenderTarget::CanvasRenderTarget(
        ICanvasDevice* canvasDevice,
        ID2D1Bitmap1* d2dBitmap)
        : CanvasBitmapImpl(canvasDevice, d2dBitmap)
        , m_hasActiveDrawingSession(std::make_shared<bool>())
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
                
                if (*m_hasActiveDrawingSession)
                    ThrowHR(E_FAIL, Strings::CannotCreateDrawingSessionUntilPreviousOneClosed);

                auto& resource = GetD2DBitmap();

                auto newDrawingSession = CreateDrawingSessionOverD2DBitmap(
                    m_device.Get(),
                    resource.Get(),
                    m_hasActiveDrawingSession);

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
            });
    }


    ActivatableClassWithFactory(CanvasRenderTarget, CanvasRenderTargetFactory);
}}}}
