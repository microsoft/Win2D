// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsDevice.h"

namespace dxrt
{
    GraphicsDevice::GraphicsDevice(IDXGIDevice* dxgiDevice)
    {
        if (dxgiDevice == nullptr)
            throw ComException(E_INVALIDARG);

        ComPtr<IDXGIDevice3> dxgiDevice3;
        ThrowIfFailed(dxgiDevice->QueryInterface(dxgiDevice3.GetAddressOf()));

        m_DxgiDevice = dxgiDevice3;
    }


    IFACEMETHODIMP GraphicsDevice::Close()
    {
        m_DxgiDevice.Close();
        return S_OK;
    }


    IFACEMETHODIMP GraphicsDevice::Trim()
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& device = m_DxgiDevice.EnsureNotClosed();
                
                // Trim() does not return an HRESULT
                device->Trim();
            });
    }


    void GraphicsDevice::GetDXGIInterface(REFIID iid, void** p)
    {
        auto& device = m_DxgiDevice.EnsureNotClosed();
        ThrowIfFailed(device.CopyTo(iid, p));
    }
}
