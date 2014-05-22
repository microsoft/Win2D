// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsDevice.h"
#include "GraphicsSurface.h"

using namespace Microsoft::WRL;
using namespace canvas;

_Use_decl_annotations_
STDAPI CreateDirectX11DeviceFromDXGIDevice(
    IDXGIDevice* dxgiDevice,
    IInspectable** inspectableDirectX11Device)
{
    using canvas::DirectX11Device;

    return ExceptionBoundary(
        [&]()
        {
            ComPtr<DirectX11Device> graphicsDevice = Make<DirectX11Device>(dxgiDevice);
            ThrowIfFalse(graphicsDevice, E_OUTOFMEMORY);
            ThrowIfFailed(graphicsDevice.CopyTo(inspectableDirectX11Device));
        });
}


_Use_decl_annotations_
STDAPI CreateDirectX11SurfaceFromDXGISurface(
    IDXGISurface* dxgiSurface,
    IInspectable** inspectableDirectX11Surface)
{
    using canvas::DirectX11Surface;

    return ExceptionBoundary(
        [&]()
        {
            ComPtr<DirectX11Surface> graphicsSurface = Make<DirectX11Surface>(dxgiSurface);
            ThrowIfFalse(graphicsSurface, E_OUTOFMEMORY);
            ThrowIfFailed(graphicsSurface.CopyTo(inspectableDirectX11Surface));
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromDirectX11Device(
    IInspectable* graphicsDevice,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<IDirectX11DeviceInternal> graphicsDeviceInternal;
            ThrowIfFailed(graphicsDevice->QueryInterface(graphicsDeviceInternal.GetAddressOf()));

            graphicsDeviceInternal->GetDXGIInterface(iid, p);
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromDirectX11Surface(
    IInspectable* graphicsSurface,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<IDirectX11SurfaceInternal> graphicsSurfaceInternal;
            ThrowIfFailed(graphicsSurface->QueryInterface(graphicsSurfaceInternal.GetAddressOf()));
            graphicsSurfaceInternal->GetDXGIInterface(iid, p);
        });
}
