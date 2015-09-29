// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubD2DDeviceContext : public MockD2DDeviceContext
    {
        ComPtr<ID2D1Device> m_owner;
        ComPtr<ID2D1Image> m_target;
        D2D1_BITMAP_PROPERTIES1* m_overrideProperties;
        float m_dpiX;
        float m_dpiY;

    public:
        StubD2DDeviceContext(ID2D1Device* owner = nullptr)
            : m_owner(owner)
            , m_overrideProperties(nullptr)
            , m_dpiX(DEFAULT_DPI)
            , m_dpiY(DEFAULT_DPI)
        {
            ClearMethod.AllowAnyCall();

            SetDpiMethod.AllowAnyCall(
                [=] (float x, float y)
                {
                    m_dpiX = x;
                    m_dpiY = y;
                });
            GetDpiMethod.AllowAnyCall(
                [=] (float* x, float* y)
                {
                    *x = m_dpiX;
                    *y = m_dpiY;
                });
            
            GetImageLocalBoundsMethod.AllowAnyCall(
                [](ID2D1Image*, D2D1_RECT_F*)
                {
                    return S_OK;
                });

            SetTextAntialiasModeMethod.AllowAnyCall();
        }

        //
        // Allows callers to pick which bitmap properties are set when a
        // D2DBitmap is created.  It's up to the caller to ensure that the
        // D2D1_BITMAP_PROPERTIES1 pointed to remains valid for the duration.
        //
        void SetOverridePropertiesForCreateBitmap(D2D1_BITMAP_PROPERTIES1* properties)
        {
            m_overrideProperties = properties;
        }

        IFACEMETHODIMP_(void) GetDevice(ID2D1Device** device) const override
        {
            m_owner.CopyTo(device);
        }

        IFACEMETHODIMP_(void) BeginDraw() override
        {
        }

        IFACEMETHODIMP EndDraw(D2D1_TAG *,D2D1_TAG *) override
        {
            return S_OK;
        }

        IFACEMETHODIMP_(void) SetTarget(ID2D1Image* theValue) override
        {
            m_target = theValue;
        }

        IFACEMETHODIMP_(void) GetTarget(ID2D1Image** theValue) const override
        {
            m_target.CopyTo(theValue);
        }

        IFACEMETHODIMP CreateBitmapFromDxgiSurface(
            IDXGISurface* surface,
            D2D1_BITMAP_PROPERTIES1 const* properties,
            ID2D1Bitmap1** bitmap) override
        {
            auto overiddenProperties = m_overrideProperties ? m_overrideProperties : properties;

            return Make<StubD2DBitmap>(surface, overiddenProperties).CopyTo(bitmap);
        }
    };
}
