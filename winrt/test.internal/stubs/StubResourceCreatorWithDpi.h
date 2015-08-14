// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class StubResourceCreatorWithDpi : public RuntimeClass<ICanvasResourceCreator, ICanvasResourceCreatorWithDpi>
{
    ComPtr<ICanvasDevice> m_device;
    float m_dpi;
public:
    StubResourceCreatorWithDpi(ICanvasDevice* device)
        : m_device(device)
        , m_dpi(96.0f)
    {
    }

    IFACEMETHODIMP get_Device(ICanvasDevice** device)
    {
        return ExceptionBoundary(
            [=]
            {
                ThrowIfFailed(m_device.CopyTo(device));
            });
    }

    void SetDpi(float dpi)
    {
        m_dpi = dpi;
    }

    IFACEMETHODIMP get_Dpi(float* value) override
    {
        *value = m_dpi;
        return S_OK;
    }
    
    IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips) override
    {
        *dips = PixelsToDips(pixels, m_dpi);
        return S_OK;
    }
    
    IFACEMETHODIMP ConvertDipsToPixels(float dips, CanvasDpiRounding rounding, int* pixels) override
    {
        *pixels = DipsToPixels(dips, m_dpi, rounding);
        return S_OK;
    }
};
