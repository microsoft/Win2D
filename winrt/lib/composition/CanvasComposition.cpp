// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasComposition.h"

#include "../utils/ApiInformationAdapter.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI::Composition;
using namespace ABI::Windows::UI::Composition;


ActivatableStaticOnlyFactory(CanvasCompositionStatics);


HRESULT CanvasCompositionStatics::RuntimeClassInitialize()
{
    return ExceptionBoundary(
        [&]
        {
            //
            // The composition APIs are only available when the Universal API
            // Contract v2 is present.  To nip any problems in the bud we
            // prevent any of the CanvasComposition APIs from being called if
            // this contract isn't present.
            //
            bool compositionApiPresent = ApiInformationAdapter::GetInstance()->IsApiContractPresent(UNIVERSAL_API_CONTRACT, 2);

            if (!compositionApiPresent)
                ThrowHR(E_NOTIMPL);
        });
}


IFACEMETHODIMP CanvasCompositionStatics::CreateCompositionGraphicsDevice(
    ICompositor* compositor,
    ICanvasDevice* canvasDevice,
    ICompositionGraphicsDevice** graphicsDevice)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(compositor);
            CheckInPointer(canvasDevice);
            CheckAndClearOutPointer(graphicsDevice);
            
            auto d2dDevice = GetWrappedResource<ID2D1Device1>(canvasDevice);
            auto compositorInterop = As<ICompositorInterop>(compositor);
            
            ThrowIfFailed(compositorInterop->CreateGraphicsDevice(d2dDevice.Get(), graphicsDevice));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::CreateCompositionSurfaceForSwapChain( 
    ICompositor* compositor,
    ICanvasSwapChain* swapChain,
    ICompositionSurface** compositionSurface)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(compositor);
            CheckInPointer(swapChain);
            CheckAndClearOutPointer(compositionSurface);
            
            auto compositorInterop = As<ICompositorInterop>(compositor);
            auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain>(swapChain);
            
            ThrowIfFailed(compositorInterop->CreateCompositionSurfaceForSwapChain(dxgiSwapChain.Get(), compositionSurface));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::GetCanvasDevice( 
    ICompositionGraphicsDevice* graphicsDevice,
    ICanvasDevice** canvasDevice)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(graphicsDevice);
            CheckAndClearOutPointer(canvasDevice);

            auto interop = As<ICompositionGraphicsDeviceInterop>(graphicsDevice);

            ComPtr<IUnknown> renderingDevice;
            ThrowIfFailed(interop->GetRenderingDevice(&renderingDevice));

            auto device = ResourceManager::GetOrCreate<ICanvasDevice>(renderingDevice.Get());
            ThrowIfFailed(device.CopyTo(canvasDevice));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::SetCanvasDevice( 
    ICompositionGraphicsDevice* graphicsDevice,
    ICanvasDevice* canvasDevice)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(graphicsDevice);
            CheckInPointer(canvasDevice);

            auto interop = As<ICompositionGraphicsDeviceInterop>(graphicsDevice);
            auto d2dDevice = GetWrappedResource<ID2D1Device>(canvasDevice);
            ThrowIfFailed(interop->SetRenderingDevice(d2dDevice.Get()));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::CreateDrawingSession( 
    ICompositionDrawingSurface* drawingSurface,
    ICanvasDrawingSession** drawingSession)
{
    return CreateDrawingSessionImpl(drawingSurface, nullptr, DEFAULT_DPI, drawingSession);
}


static RECT ToRECTForCompositor(Rect const& rect)
{
    RECT r{};
    r.left   = static_cast<LONG>(std::round(rect.X));
    r.right  = static_cast<LONG>(std::round(rect.X + rect.Width));
    r.top    = static_cast<LONG>(std::round(rect.Y));
    r.bottom = static_cast<LONG>(std::round(rect.Y + rect.Height));
    return r;
}


IFACEMETHODIMP CanvasCompositionStatics::CreateDrawingSessionWithUpdateRect( 
    ICompositionDrawingSurface* drawingSurface,
    Rect updateRect,
    ICanvasDrawingSession** drawingSession)
{
    RECT rect = ToRECTForCompositor(updateRect);

    return CreateDrawingSessionImpl(drawingSurface, &rect, DEFAULT_DPI, drawingSession);
}


IFACEMETHODIMP CanvasCompositionStatics::CreateDrawingSessionWithUpdateRectAndDpi(
    ICompositionDrawingSurface* drawingSurface,
    Rect updateRectInPixels,
    float dpi,
    ICanvasDrawingSession** drawingSession)
{
    RECT rect = ToRECTForCompositor(updateRectInPixels);

    return CreateDrawingSessionImpl(drawingSurface, &rect, dpi, drawingSession);
}


class CompositionDrawingSurfaceDrawingSessionAdapter
    : public ICanvasDrawingSessionAdapter
    , private LifespanTracker<CompositionDrawingSurfaceDrawingSessionAdapter>
{
    ComPtr<ICompositionDrawingSurfaceInterop> m_drawingSurface;
    
public:
    CompositionDrawingSurfaceDrawingSessionAdapter(ComPtr<ICompositionDrawingSurfaceInterop>&& drawingSurface)
        : m_drawingSurface(std::move(drawingSurface))
    {
    }

    virtual void EndDraw(ID2D1DeviceContext1*) override
    {
        ThrowIfFailed(m_drawingSurface->EndDraw());
    }
};


HRESULT CanvasCompositionStatics::CreateDrawingSessionImpl(ICompositionDrawingSurface* drawingSurface, RECT const* updateRect, float dpi, ICanvasDrawingSession** drawingSession)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSurface);
            CheckAndClearOutPointer(drawingSession);
            
            auto drawingSurfaceInterop = As<ICompositionDrawingSurfaceInterop>(drawingSurface);

            ComPtr<ID2D1DeviceContext> deviceContext;
            POINT offset;            
            ThrowIfFailed(drawingSurfaceInterop->BeginDraw(updateRect, IID_PPV_ARGS(&deviceContext), &offset));

            float offsetX = PixelsToDips(offset.x, dpi);
            float offsetY = PixelsToDips(offset.y, dpi);

            deviceContext->SetTransform(D2D1::Matrix3x2F::Translation(offsetX, offsetY));
            deviceContext->SetDpi(dpi, dpi);

            // Although we could look up the owner using interop, via the
            // deviceContext, drawing session will do this lazily for us if
            // anyone actually requests it.
            ICanvasDevice* owner = nullptr;

            auto newDs = CanvasDrawingSession::CreateNew(
                As<ID2D1DeviceContext1>(deviceContext).Get(),
                std::make_shared<CompositionDrawingSurfaceDrawingSessionAdapter>(std::move(drawingSurfaceInterop)),
                owner,
                nullptr,
                D2D1_POINT_2F{ offsetX, offsetY });
            ThrowIfFailed(newDs.CopyTo(drawingSession));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::Resize( 
    ICompositionDrawingSurface* drawingSurface,
    Size size)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSurface);

            SIZE newSize{};
            newSize.cx = static_cast<LONG>(std::round(size.Width));
            newSize.cy = static_cast<LONG>(std::round(size.Height));

            auto drawingSurfaceInterop = As<ICompositionDrawingSurfaceInterop>(drawingSurface);

            ThrowIfFailed(drawingSurfaceInterop->Resize(newSize));
        });
}

#endif
