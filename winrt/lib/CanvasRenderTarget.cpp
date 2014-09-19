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
    CanvasRenderTargetFactory::CanvasRenderTargetFactory()
        : m_drawingSessionFactory(std::make_shared<CanvasRenderTargetDrawingSessionFactory>())
    {
    }

    IFACEMETHODIMP CanvasRenderTargetFactory::Create(
        ICanvasResourceCreator *resourceCreator,
        ABI::Windows::Foundation::Size sizeInPixels,
        ICanvasRenderTarget **renderTarget)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(renderTarget);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto bitmap = Make<CanvasRenderTarget>(canvasDevice.Get(), sizeInPixels, m_drawingSessionFactory);
                CheckMakeResult(bitmap);

                ThrowIfFailed(bitmap.CopyTo(renderTarget));
            });
    }

    CanvasRenderTargetDrawingSessionFactory::CanvasRenderTargetDrawingSessionFactory()
        : m_drawingSessionManager(CanvasDrawingSessionFactory::GetOrCreateManager())
    {
    }

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

    ComPtr<ICanvasDrawingSession> CanvasRenderTargetDrawingSessionFactory::Create(
        ICanvasDevice* owner,
        ID2D1Bitmap1* targetBitmap) const
    {
        assert(owner != nullptr);
        assert(targetBitmap != nullptr);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(owner->QueryInterface(deviceInternal.GetAddressOf()));
        auto d2dDevice = deviceInternal->GetD2DDevice();

        ComPtr<ID2D1DeviceContext1> deviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));
        deviceContext->SetTarget(targetBitmap);

        auto adapter = std::make_shared<CanvasBitmapDrawingSessionAdapter>(deviceContext.Get());

        return m_drawingSessionManager->Create(deviceContext.Get(), adapter);
    }

    //
    // TODO #2449 - the CanvasRenderTarget constructor will get tidied up when
    // we move to the resource wrapper pattern
    //
    ComPtr<ID2D1Bitmap1> CreateBitmap(ICanvasDevice* canvasDevice, Size size)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        return canvasDeviceInternal->CreateBitmap(size);
    }

    CanvasRenderTarget::CanvasRenderTarget(
        ICanvasDevice* canvasDevice,
        Size size,
        std::shared_ptr<ICanvasRenderTargetDrawingSessionFactory> drawingSessionFactory)
        : CanvasBitmapImpl(CreateBitmap(canvasDevice, size).Get())
        , m_drawingSessionFactory(drawingSessionFactory)
        , m_device(canvasDevice)
    {
    }

    IFACEMETHODIMP CanvasRenderTarget::CreateDrawingSession(
        _COM_Outptr_ ICanvasDrawingSession** drawingSession)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(drawingSession);
                
                auto resource = GetD2DBitmap();

                auto newDrawingSession = m_drawingSessionFactory->Create(
                    m_device.Get(),
                    resource.Get());

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
            });
    }

    ActivatableClassWithFactory(CanvasRenderTarget, CanvasRenderTargetFactory);
}}}}
