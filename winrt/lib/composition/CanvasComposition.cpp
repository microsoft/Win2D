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
    return CreateDrawingSessionImpl(drawingSurface, nullptr, drawingSession);
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

    return CreateDrawingSessionImpl(drawingSurface, &rect, drawingSession);
}


class CompositionDrawingSurfaceDrawingSessionAdapter
    : public DrawingSessionBaseAdapter
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


HRESULT CanvasCompositionStatics::CreateDrawingSessionImpl(ICompositionDrawingSurface* drawingSurface, RECT const* updateRect, ICanvasDrawingSession** drawingSession)
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

            deviceContext->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));

            // Although we could look up the owner using interop, via the
            // deviceContext, drawing session will do this lazily for us if
            // anyone actually requests it.
            ICanvasDevice* owner = nullptr;

            auto newDs = CanvasDrawingSession::CreateNew(
                As<ID2D1DeviceContext1>(deviceContext).Get(),
                std::make_shared<CompositionDrawingSurfaceDrawingSessionAdapter>(std::move(drawingSurfaceInterop)),
                owner,
                D2D1_POINT_2F{ (float)offset.x, (float)offset.y });
            ThrowIfFailed(newDs.CopyTo(drawingSession));
        });
}


IFACEMETHODIMP CanvasCompositionStatics::SuspendDrawing(
    ICompositionDrawingSurface* drawingSurface)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSurface);

            auto drawingSurfaceInterop = As<ICompositionDrawingSurfaceInterop>(drawingSurface);

            ThrowIfFailed(drawingSurfaceInterop->SuspendDraw());
        });
}


IFACEMETHODIMP CanvasCompositionStatics::ResumeDrawing(
    ICompositionDrawingSurface* drawingSurface)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSurface);

            auto drawingSurfaceInterop = As<ICompositionDrawingSurfaceInterop>(drawingSurface);

            ThrowIfFailed(drawingSurfaceInterop->ResumeDraw());
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


static RECT MaybeGetRECT(IReference<Rect>* maybeRect)
{
    RECT r{};

    if (!maybeRect)
        return r;

    Rect rect;
    ThrowIfFailed(maybeRect->get_Value(&rect));

    return ToRECTForCompositor(rect);
}


IFACEMETHODIMP CanvasCompositionStatics::Scroll( 
    ICompositionDrawingSurface* drawingSurface,
    IReference<Rect>* scrollRect,
    IReference<Rect>* clipRect,
    Point offset)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSurface);
            
            RECT scroll = MaybeGetRECT(scrollRect);
            RECT clip   = MaybeGetRECT(clipRect);

            int offsetX = static_cast<int>(std::round(offset.X));
            int offsetY = static_cast<int>(std::round(offset.Y));

            auto drawingSurfaceInterop = As<ICompositionDrawingSurfaceInterop>(drawingSurface);
            ThrowIfFailed(drawingSurfaceInterop->Scroll(
                scrollRect ? &scroll : nullptr,
                clipRect   ? &clip   : nullptr,
                offsetX,
                offsetY));
        });
}

#endif
