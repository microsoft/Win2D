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
#include "CanvasRenderTarget.h"
#include "CanvasDevice.h"
#include "CanvasDrawingSession.h"
#include "CanvasBitmap.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;

    //
    // CanvasRenderTargetManager
    //
    CanvasRenderTargetManager::CanvasRenderTargetManager(
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_adapter(adapter)
    {

    }

    ComPtr<CanvasRenderTarget> CanvasRenderTargetManager::CreateNew(
        ICanvasDevice* canvasDevice,
        float width,
        float height,
        DirectXPixelFormat format,
        CanvasAlphaBehavior alpha,
        float dpi)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateRenderTargetBitmap(width, height, format, alpha, dpi);

        return Make<CanvasRenderTarget>(shared_from_this(), d2dBitmap.Get(), canvasDevice);
    }


    ComPtr<CanvasRenderTarget> CanvasRenderTargetManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1Bitmap1* d2dBitmap)
    {
        auto renderTarget = Make<CanvasRenderTarget>(
            shared_from_this(),
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

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeight(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        float width,
        float height,
        ICanvasRenderTarget** renderTarget)
    {
        return CreateWithWidthAndHeightAndFormatAndAlpha(
            resourceCreator,
            width,
            height,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied,
            renderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeightAndDpi(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        float dpi,
        ICanvasRenderTarget** renderTarget)
    {
        return CreateWithWidthAndHeightAndFormatAndAlphaAndDpi(
            resourceCreator,
            width,
            height,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied,
            dpi,
            renderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeightAndFormatAndAlpha(
        ICanvasResourceCreatorWithDpi* resourceCreator,
        float width,
        float height,
        DirectXPixelFormat format,
        CanvasAlphaBehavior alpha,
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

                auto bitmap = GetManager()->CreateRenderTarget(
                    canvasDevice.Get(), 
                    width, 
                    height, 
                    format, 
                    alpha, 
                    dpi);

                ThrowIfFailed(bitmap.CopyTo(renderTarget));
            });
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateWithWidthAndHeightAndFormatAndAlphaAndDpi(
        ICanvasResourceCreator* resourceCreator,
        float width,
        float height,
        DirectXPixelFormat format,
        CanvasAlphaBehavior alpha,
        float dpi,
        ICanvasRenderTarget** renderTarget)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(renderTarget);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto bitmap = GetManager()->CreateRenderTarget(
                    canvasDevice.Get(), 
                    width, 
                    height, 
                    format, 
                    alpha, 
                    dpi);

                ThrowIfFailed(bitmap.CopyTo(renderTarget));
            });
    }
    
    IFACEMETHODIMP CanvasRenderTargetFactory::GetOrCreate(
        ICanvasDevice* device,
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1Bitmap1> d2dBitmap;
                ThrowIfFailed(resource->QueryInterface(d2dBitmap.GetAddressOf()));

                auto newCanvasRenderTarget = GetManager()->GetOrCreateRenderTarget(device, d2dBitmap.Get());

                ThrowIfFailed(newCanvasRenderTarget.CopyTo(wrapper));
            });
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11Surface(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        ICanvasRenderTarget** canvasRenderTarget)
    {
        return CreateFromDirect3D11SurfaceWithAlphaAndDpi(
            resourceCreator,
            surface,
            CanvasAlphaBehavior::Premultiplied,
            DEFAULT_DPI,
            canvasRenderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11SurfaceWithAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha,
        ICanvasRenderTarget** canvasRenderTarget)
    {
        return CreateFromDirect3D11SurfaceWithAlphaAndDpi(
            resourceCreator,
            surface,
            alpha,
            DEFAULT_DPI,
            canvasRenderTarget);
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::CreateFromDirect3D11SurfaceWithAlphaAndDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha,
        float dpi,
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

                auto newRenderTarget = GetManager()->CreateRenderTargetFromSurface(
                    canvasDevice.Get(), 
                    surface, 
                    alpha, 
                    dpi);

                ThrowIfFailed(newRenderTarget.CopyTo(canvasRenderTarget));
            });
    }


    //
    // CanvasBitmapDrawingSessionAdapter
    //


    class CanvasBitmapDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
        ComPtr<ID2D1DeviceContext1> m_d2dDeviceContext;

    public:
        CanvasBitmapDrawingSessionAdapter(ID2D1DeviceContext1* d2dDeviceContext)
            : m_d2dDeviceContext(d2dDeviceContext) 
        {
            d2dDeviceContext->BeginDraw();
        }

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
        {
            return D2D1::Point2F(0, 0);
        }

        virtual void EndDraw() override
        {
            ThrowIfFailed(m_d2dDeviceContext->EndDraw());
        }
    };

    static ComPtr<ICanvasDrawingSession> CreateDrawingSessionOverD2DBitmap(
        ICanvasDevice* owner,
        ID2D1Bitmap1* targetBitmap)
    {
        assert(owner != nullptr);
        assert(targetBitmap != nullptr);

        //
        // Create a new ID2D1DeviceContext
        //
        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(owner->QueryInterface(deviceInternal.GetAddressOf()));
        auto d2dDevice = deviceInternal->GetD2DDevice();

        ComPtr<ID2D1DeviceContext1> deviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

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

        auto adapter = std::make_shared<CanvasBitmapDrawingSessionAdapter>(deviceContext.Get());

        auto drawingSessionManager = CanvasDrawingSessionFactory::GetOrCreateManager();
        return drawingSessionManager->Create(owner, deviceContext.Get(), adapter);
    }

    //
    // CanvasRenderTarget
    //


    CanvasRenderTarget::CanvasRenderTarget(
        std::shared_ptr<CanvasRenderTargetManager> manager,
        ID2D1Bitmap1* d2dBitmap,
        ICanvasDevice* canvasDevice)
        : CanvasBitmapImpl(manager, d2dBitmap)
        , m_device(canvasDevice)
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


    IFACEMETHODIMP CanvasRenderTarget::get_Device(
        ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_device.CopyTo(value));
            });
    }

    ActivatableClassWithFactory(CanvasRenderTarget, CanvasRenderTargetFactory);
}}}}
