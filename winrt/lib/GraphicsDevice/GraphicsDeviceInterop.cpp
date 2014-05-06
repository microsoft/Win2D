// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsDevice.h"
#include "GraphicsSurface.h"

using namespace Microsoft::WRL;
using namespace dxrt;

_Use_decl_annotations_
STDAPI CreateGraphicsDeviceFromDXGIDevice(
    IDXGIDevice* dxgiDevice,
    IInspectable** inspectableGraphicsDevice)
{
    using dxrt::GraphicsDevice;

    return ExceptionBoundary(
        [&]()
        {
            ComPtr<GraphicsDevice> graphicsDevice = Make<GraphicsDevice>(dxgiDevice);
            ThrowIfFalse(graphicsDevice, E_OUTOFMEMORY);
            ThrowIfFailed(graphicsDevice.CopyTo(inspectableGraphicsDevice));
        });
}


_Use_decl_annotations_
STDAPI CreateGraphicsSurfaceFromDXGISurface(
    IDXGISurface* dxgiSurface,
    IInspectable** inspectableGraphicsSurface)
{
    using dxrt::GraphicsSurface;

    return ExceptionBoundary(
        [&]()
        {
            ComPtr<GraphicsSurface> graphicsSurface = Make<GraphicsSurface>(dxgiSurface);
            ThrowIfFalse(graphicsSurface, E_OUTOFMEMORY);
            ThrowIfFailed(graphicsSurface.CopyTo(inspectableGraphicsSurface));
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromGraphicsDevice(
    IInspectable* graphicsDevice,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<IGraphicsDeviceInternal> graphicsDeviceInternal;
            ThrowIfFailed(graphicsDevice->QueryInterface(graphicsDeviceInternal.GetAddressOf()));

            graphicsDeviceInternal->GetDXGIInterface(iid, p);
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromGraphicsSurface(
    IInspectable* graphicsSurface,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<IGraphicsSurfaceInternal> graphicsSurfaceInternal;
            ThrowIfFailed(graphicsSurface->QueryInterface(graphicsSurfaceInternal.GetAddressOf()));
            graphicsSurfaceInternal->GetDXGIInterface(iid, p);
        });
}
