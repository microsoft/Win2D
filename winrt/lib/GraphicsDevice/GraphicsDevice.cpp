// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsDevice.h"

namespace canvas
{
    DirectX11Device::DirectX11Device(IDXGIDevice* dxgiDevice)
    {
        if (dxgiDevice == nullptr)
            throw ComException(E_INVALIDARG);

        ComPtr<IDXGIDevice3> dxgiDevice3;
        ThrowIfFailed(dxgiDevice->QueryInterface(dxgiDevice3.GetAddressOf()));

        m_DxgiDevice = dxgiDevice3;
    }


    IFACEMETHODIMP DirectX11Device::Close()
    {
        m_DxgiDevice.Close();
        return S_OK;
    }


    IFACEMETHODIMP DirectX11Device::Trim()
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& device = m_DxgiDevice.EnsureNotClosed();
                
                // Trim() does not return an HRESULT
                device->Trim();
            });
    }


    void DirectX11Device::GetDXGIInterface(REFIID iid, void** p)
    {
        auto& device = m_DxgiDevice.EnsureNotClosed();
        ThrowIfFailed(device.CopyTo(iid, p));
    }
}
