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

#include "CanvasSwapChain.h"
#include "CanvasDevice.h"
#include "CanvasDrawingSession.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // CanvasSwapChainFactory
    //

    CanvasSwapChainFactory::CanvasSwapChainFactory()
        : m_drawingSessionFactory(std::make_shared<CanvasSwapChainDrawingSessionFactory>())
    {

    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithSize(
        ICanvasResourceCreator* resourceCreator,
        int32_t widthInPixels,
        int32_t heightInPixels,
        ICanvasSwapChain** swapChain)
    {
        return CreateWithAllOptions(
            resourceCreator,
            widthInPixels,
            heightInPixels,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            CanvasAlphaBehavior::Premultiplied,
            swapChain);
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithSizeAndFormat(
        ICanvasResourceCreator* resourceCreator,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        ICanvasSwapChain** swapChain)
    {
        return CreateWithAllOptions(
            resourceCreator, 
            widthInPixels, 
            heightInPixels, 
            format, 
            2, 
            CanvasAlphaBehavior::Premultiplied, 
            swapChain);
    }

    IFACEMETHODIMP CanvasSwapChainFactory::CreateWithAllOptions(
        ICanvasResourceCreator* resourceCreator,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaBehavior alphaBehavior,
        ICanvasSwapChain** swapChain)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(swapChain);
                
                auto newCanvasSwapChain = GetManager()->Create(
                    resourceCreator,
                    widthInPixels,
                    heightInPixels,
                    format,
                    bufferCount,
                    alphaBehavior,
                    m_drawingSessionFactory);

                ThrowIfFailed(newCanvasSwapChain.CopyTo(swapChain));
            });
    }

    //
    // ICanvasFactoryNative
    //


    IFACEMETHODIMP CanvasSwapChainFactory::GetOrCreate(
        ICanvasDevice* device,
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<IDXGISwapChain2> dxgiSwapChain;
                ThrowIfFailed(resource->QueryInterface(dxgiSwapChain.GetAddressOf()));

                auto newCanvasSwapChain = GetManager()->GetOrCreate(device, dxgiSwapChain.Get());

                ThrowIfFailed(newCanvasSwapChain.CopyTo(wrapper));
            });
    }

    CanvasSwapChain::CanvasSwapChain(
        ICanvasResourceCreator* resourceCreator,
        std::shared_ptr<CanvasSwapChainManager> swapChainManager,
        std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> drawingSessionFactory,
        IDXGISwapChain2* dxgiSwapChain)
        : ResourceWrapper(swapChainManager, dxgiSwapChain)
        , m_drawingSessionFactory(drawingSessionFactory)
    {
        ThrowIfFailed(resourceCreator->get_Device(&m_device));
    }

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

                auto newDrawingSession = m_drawingSessionFactory->Create(
                    device.Get(),
                    dxgiSwapChain.Get(),
                    clearColor);

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_Width(int32_t* value) 
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = GetResourceDescription().Width;
            });
    }

    IFACEMETHODIMP CanvasSwapChain::get_Height(int32_t* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = GetResourceDescription().Height;
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

    IFACEMETHODIMP CanvasSwapChain::get_AlphaMode(CanvasAlphaBehavior* value) 
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = FromDxgiAlphaMode(GetResourceDescription().AlphaMode);
            });
    }

    IFACEMETHODIMP CanvasSwapChain::Present() 
    {
        return ExceptionBoundary(
            [&]
            {
                auto resource = GetResource();

                DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
                ThrowIfFailed(resource->Present1(1, 0, &presentParameters));
            });
    }

    IFACEMETHODIMP CanvasSwapChain::ResizeBuffers(
        int32_t bufferCount,
        int32_t newWidth,
        int32_t newHeight,
        DirectXPixelFormat newFormat) 
    {
        return ExceptionBoundary(
            [&]
            {
                auto resource = GetResource();

                ThrowIfNegative(bufferCount);
                ThrowIfNegative(newWidth);
                ThrowIfNegative(newHeight);

                ThrowIfFailed(resource->ResizeBuffers(
                    bufferCount, 
                    newWidth,
                    newHeight, 
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
        return S_OK;
    }

    // ICanvasResourceCreator

    IFACEMETHODIMP CanvasSwapChain::get_Device(ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto& device = m_device.EnsureNotClosed();

                device.CopyTo(value);
            });
    }


    DXGI_SWAP_CHAIN_DESC1 CanvasSwapChain::GetResourceDescription()
    {
        auto& resource = GetResource();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ThrowIfFailed(resource->GetDesc1(&swapChainDesc));

        return swapChainDesc;
    }

    class CanvasSwapChainDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
        ComPtr<ID2D1DeviceContext1> m_deviceContext;

    public:
        static std::shared_ptr<CanvasSwapChainDrawingSessionAdapter> Create(
            ICanvasDevice* owner,
            IDXGISwapChain2* swapChainResource,
            D2D1_COLOR_F const& clearColor,
            ID2D1DeviceContext1** outDeviceContext)
        {
            auto deviceInternal = As<ICanvasDeviceInternal>(owner);

            auto d2dDevice = deviceInternal->GetD2DDevice();

            ComPtr<ID2D1DeviceContext1> deviceContext;
            ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

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

    CanvasSwapChainDrawingSessionFactory::CanvasSwapChainDrawingSessionFactory()
        : m_drawingSessionManager(CanvasDrawingSessionFactory::GetOrCreateManager())
    {

    }

    ComPtr<ICanvasDrawingSession> CanvasSwapChainDrawingSessionFactory::Create(
        ICanvasDevice* owner,
        IDXGISwapChain2* swapChainResource,
        Color const& clearColor) const
    {
        CheckInPointer(owner);

        ComPtr<ID2D1DeviceContext1> deviceContext;
        auto adapter = CanvasSwapChainDrawingSessionAdapter::Create(
            owner,
            swapChainResource,
            ToD2DColor(clearColor),
            &deviceContext);


        return m_drawingSessionManager->Create(deviceContext.Get(), adapter);

    }

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        int32_t bufferCount,
        CanvasAlphaBehavior alphaBehavior,
        std::shared_ptr<ICanvasSwapChainDrawingSessionFactory> const& drawingSessionFactory)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device.As(&deviceInternal));

        ComPtr<IDXGISwapChain2> dxgiSwapChain = deviceInternal->CreateSwapChain(
            widthInPixels,
            heightInPixels,
            format,
            bufferCount,
            alphaBehavior);

        auto canvasSwapChain = Make<CanvasSwapChain>(
            resourceCreator,
            shared_from_this(),
            drawingSessionFactory,
            dxgiSwapChain.Get());
        CheckMakeResult(canvasSwapChain);

        return canvasSwapChain;
    }

    ComPtr<CanvasSwapChain> CanvasSwapChainManager::CreateWrapper(
        ICanvasDevice* device,
        IDXGISwapChain2* resource)
    {
        auto resourceCreator = As<ICanvasResourceCreator>(device);

        auto swapChain = Make<CanvasSwapChain>(
            resourceCreator.Get(),
            shared_from_this(),
            nullptr, // drawing session factory
            resource);
        CheckMakeResult(swapChain);

        return swapChain;
    }

    ActivatableClassWithFactory(CanvasSwapChain, CanvasSwapChainFactory);
}}}}