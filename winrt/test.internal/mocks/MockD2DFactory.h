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

#pragma once

namespace canvas
{
    class MockD2DFactory : public RuntimeClass <
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces < ID2D1Factory2, ID2D1Factory1, ID2D1Factory >, ID2D1Multithread >
    {
        int m_enterCount;
        int m_leaveCount;

    public:
        std::function<void(IDXGIDevice *dxgiDevice, ID2D1Device1 **d2dDevice1)> MockCreateDevice;

        MockD2DFactory()
          : m_enterCount(0)
          , m_leaveCount(0)
        { }

        STDMETHOD(ReloadSystemMetrics)(
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD_(void, GetDesktopDpi)(
            _Out_ FLOAT *dpiX,
            _Out_ FLOAT *dpiY
            )
        {
        }

        STDMETHOD(CreateRectangleGeometry)(
            _In_ CONST D2D1_RECT_F *rectangle,
            _Outptr_ ID2D1RectangleGeometry **rectangleGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateRoundedRectangleGeometry)(
            _In_ CONST D2D1_ROUNDED_RECT *roundedRectangle,
            _Outptr_ ID2D1RoundedRectangleGeometry **roundedRectangleGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateEllipseGeometry)(
            _In_ CONST D2D1_ELLIPSE *ellipse,
            _Outptr_ ID2D1EllipseGeometry **ellipseGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateGeometryGroup)(
            D2D1_FILL_MODE fillMode,
            _In_reads_(geometriesCount) ID2D1Geometry **geometries,
            UINT32 geometriesCount,
            _Outptr_ ID2D1GeometryGroup **geometryGroup
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateTransformedGeometry)(
            _In_ ID2D1Geometry *sourceGeometry,
            _In_ CONST D2D1_MATRIX_3X2_F *transform,
            _Outptr_ ID2D1TransformedGeometry **transformedGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreatePathGeometry)(
            _Outptr_ ID2D1PathGeometry **pathGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateStrokeStyle)(
            _In_ CONST D2D1_STROKE_STYLE_PROPERTIES *strokeStyleProperties,
            _In_reads_opt_(dashesCount) CONST FLOAT *dashes,
            UINT32 dashesCount,
            _Outptr_ ID2D1StrokeStyle **strokeStyle
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDrawingStateBlock)(
            _In_opt_ CONST D2D1_DRAWING_STATE_DESCRIPTION *drawingStateDescription,
            _In_opt_ IDWriteRenderingParams *textRenderingParams,
            _Outptr_ ID2D1DrawingStateBlock **drawingStateBlock
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateWicBitmapRenderTarget)(
            _In_ IWICBitmap *target,
            _In_ CONST D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties,
            _Outptr_ ID2D1RenderTarget **renderTarget
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateHwndRenderTarget)(
            _In_ CONST D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties,
            _In_ CONST D2D1_HWND_RENDER_TARGET_PROPERTIES *hwndRenderTargetProperties,
            _Outptr_ ID2D1HwndRenderTarget **hwndRenderTarget
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDxgiSurfaceRenderTarget)(
            _In_ IDXGISurface *dxgiSurface,
            _In_ CONST D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties,
            _Outptr_ ID2D1RenderTarget **renderTarget
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDCRenderTarget)(
            _In_ CONST D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties,
            _Outptr_ ID2D1DCRenderTarget **dcRenderTarget
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDevice)(
            _In_ IDXGIDevice *dxgiDevice,
            _Outptr_ ID2D1Device **d2dDevice
            )
        {
            Assert::Fail(L"Unexpected call to CreateDevice(...ID2D1Device).  ID2D1Device1 overload is expected to be used.");
            return E_NOTIMPL;
        }

        STDMETHOD(CreateStrokeStyle)(
            _In_ CONST D2D1_STROKE_STYLE_PROPERTIES1 *strokeStyleProperties,
            _In_reads_opt_(dashesCount) CONST FLOAT *dashes,
            UINT32 dashesCount,
            _Outptr_ ID2D1StrokeStyle1 **strokeStyle
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreatePathGeometry)(
            _Outptr_ ID2D1PathGeometry1 **pathGeometry
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDrawingStateBlock)(
            _In_opt_ CONST D2D1_DRAWING_STATE_DESCRIPTION1 *drawingStateDescription,
            _In_opt_ IDWriteRenderingParams *textRenderingParams,
            _Outptr_ ID2D1DrawingStateBlock1 **drawingStateBlock
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateGdiMetafile)(
            _In_ IStream *metafileStream,
            _Outptr_ ID2D1GdiMetafile **metafile
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(RegisterEffectFromStream)(
            _In_ REFCLSID classId,
            _In_ IStream *propertyXml,
            _In_reads_opt_(bindingsCount) CONST D2D1_PROPERTY_BINDING *bindings,
            UINT32 bindingsCount,
            _In_ CONST PD2D1_EFFECT_FACTORY effectFactory
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(RegisterEffectFromString)(
            _In_ REFCLSID classId,
            _In_ PCWSTR propertyXml,
            _In_reads_opt_(bindingsCount) CONST D2D1_PROPERTY_BINDING *bindings,
            UINT32 bindingsCount,
            _In_ CONST PD2D1_EFFECT_FACTORY effectFactory
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(UnregisterEffect)(
            _In_ REFCLSID classId
            )
        {
            return E_NOTIMPL;
        }

        STDMETHOD(GetRegisteredEffects)(
            _Out_writes_to_opt_(effectsCount, *effectsReturned) CLSID *effects,
            UINT32 effectsCount,
            _Out_opt_ UINT32 *effectsReturned,
            _Out_opt_ UINT32 *effectsRegistered
            ) CONST
        {
            return E_NOTIMPL;
        }

        STDMETHOD(GetEffectProperties)(
            _In_ REFCLSID effectId,
            _Outptr_ ID2D1Properties **properties
            ) CONST
        {
            return E_NOTIMPL;
        }

        STDMETHOD(CreateDevice)(
            _In_ IDXGIDevice *dxgiDevice,
            _Outptr_ ID2D1Device1 **d2dDevice1
            )
        {
            if (MockCreateDevice)
            {
                MockCreateDevice(dxgiDevice, d2dDevice1);
                return S_OK;
            }
            else
            {
                ComPtr<MockD2DDevice> mockD2DDevice = Make<MockD2DDevice>(this);
                return mockD2DDevice.CopyTo(d2dDevice1);
            }
        }

        STDMETHOD_(BOOL, GetMultithreadProtected)() CONST
        {
            return true;
        }

        STDMETHOD_(void, Enter)()
        {
            m_enterCount++;
        }

        STDMETHOD_(void, Leave)()
        {
            m_leaveCount++;
        }

        int GetEnterCount() const { return m_enterCount; }
        int GetLeaveCount() const { return m_leaveCount; }
    };


    inline ComPtr<ID2D1Factory2> MakeMockD2DFactory()
    {
        return Make<MockD2DFactory>();
    }
}
