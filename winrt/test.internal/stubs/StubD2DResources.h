// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

// This device derives from MockCanvasDevice, but it allows creation of stroke styles.
class StubD2DFactoryWithCreateStrokeStyle : public MockD2DFactory
{
public:
    StubD2DFactoryWithCreateStrokeStyle()
        : m_numCallsToCreateStrokeStyle(0)
        , m_startCap(D2D1_CAP_STYLE_FLAT)
        , m_endCap(D2D1_CAP_STYLE_FLAT)
        , m_dashCap(D2D1_CAP_STYLE_FLAT)
        , m_lineJoin(D2D1_LINE_JOIN_MITER)
        , m_miterLimit(10.0f)
        , m_dashStyle(D2D1_DASH_STYLE_SOLID)
        , m_dashOffset(0)
        , m_transformBehavior(D2D1_STROKE_TRANSFORM_TYPE_NORMAL)
    {}


    STDMETHOD(CreateStrokeStyle)(
        _In_ CONST D2D1_STROKE_STYLE_PROPERTIES1 *strokeStyleProperties,
        _In_reads_opt_(dashesCount) CONST FLOAT *dashes,
        UINT32 dashesCount,
        _Outptr_ ID2D1StrokeStyle1 **strokeStyle
        );

    int m_numCallsToCreateStrokeStyle;
    D2D1_CAP_STYLE m_startCap;
    D2D1_CAP_STYLE m_endCap;
    D2D1_CAP_STYLE m_dashCap;
    D2D1_LINE_JOIN m_lineJoin;
    float m_miterLimit;
    D2D1_DASH_STYLE m_dashStyle;
    float m_dashOffset;
    std::vector<float> m_customDashElements;
    D2D1_STROKE_TRANSFORM_TYPE m_transformBehavior;
};

class StubD2DDeviceContextWithGetFactory : public MockD2DDeviceContext
{
public:

    ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_factory;

    StubD2DDeviceContextWithGetFactory()
    {
        m_factory = Make<StubD2DFactoryWithCreateStrokeStyle>();

        CheckMakeResult(m_factory);

        SetTextAntialiasModeMethod.AllowAnyCall();
    }

    IFACEMETHODIMP_(void) GetFactory(ID2D1Factory** factory) const override
    {
        ThrowIfFailed(m_factory.CopyTo(factory));
    }

    IFACEMETHODIMP CreateBitmap(
        D2D1_SIZE_U size,
        const void *data,
        UINT32 dataSize,
        const D2D1_BITMAP_PROPERTIES1* properties,
        ID2D1Bitmap1** out) override;
};

class StubD2DDevice : public MockD2DDevice
{
    IFACEMETHODIMP CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS deviceContextOptions, 
        ID2D1DeviceContext** deviceContext) override
    {
        auto stubDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        return stubDeviceContext.CopyTo(deviceContext);
    }

#if WINVER > _WIN32_WINNT_WINBLUE
    IFACEMETHODIMP GetDxgiDevice(
        IDXGIDevice **dxgiDevice) override
    {
        return m_dxgiDevice.CopyTo(dxgiDevice);
    }
#endif
};

class StubDxgiSurface : public MockDxgiSurface
{
public:
    STDMETHODIMP GetDevice(IID const& iid, void ** out) override
    {
        ComPtr<IDXGIDevice> device = Make<StubDxgiDevice>();

        return device.CopyTo(reinterpret_cast<IDXGIDevice**>(out));
    }
};

class StubD2DBitmap : public MockD2DBitmap
{
    ComPtr<IDXGISurface> m_surface;
    D2D1_BITMAP_PROPERTIES1 m_properties;
    float m_dpi;

public:
    StubD2DBitmap(D2D1_BITMAP_OPTIONS options = D2D1_BITMAP_OPTIONS_NONE, float dpi = DEFAULT_DPI)
        : m_properties(D2D1::BitmapProperties1(options))
        , m_surface(Make<StubDxgiSurface>())
        , m_dpi(dpi)
    {
    }

    StubD2DBitmap(IDXGISurface* surface, D2D1_BITMAP_PROPERTIES1 const* properties, float dpi = DEFAULT_DPI)
        : m_surface(surface)
        , m_properties(properties ? *properties : D2D1_BITMAP_PROPERTIES1{})
        , m_dpi(dpi)
    {
    }


    STDMETHOD_(D2D1_BITMAP_OPTIONS, GetOptions)(
        ) CONST
    {
        return m_properties.bitmapOptions;
    }
    
    STDMETHOD(GetSurface)(
        IDXGISurface **out
        ) CONST override
    {
        return m_surface.CopyTo(out);
    }

    STDMETHOD_(void, GetDpi)(
        FLOAT *dpiX,
        FLOAT *dpiY) CONST override
    {
        *dpiX = m_dpi;
        *dpiY = m_dpi;
    }
};
