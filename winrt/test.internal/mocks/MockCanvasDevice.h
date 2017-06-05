// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockCanvasDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasDevice,
        CloakedIid<ICanvasDeviceInternal>,
        ICanvasResourceCreator,
        IDirect3DDevice,
        IDirect3DDxgiInterfaceAccess>
    {
    public:        
        std::function<ComPtr<ID2D1Device1>()> MockGetD2DDevice;
        std::function<void(ICanvasDevice**)> Mockget_Device;
        std::function<ComPtr<ID2D1SolidColorBrush>(D2D1_COLOR_F const&)> MockCreateSolidColorBrush;
        std::function<ComPtr<ID2D1ImageBrush>(ID2D1Image* image)> MockCreateImageBrush;
        std::function<ComPtr<ID2D1BitmapBrush1>(ID2D1Bitmap1* bitmap)> MockCreateBitmapBrush;
        std::function<ComPtr<ID2D1Bitmap1>(IWICBitmapSource* converter, CanvasAlphaMode alpha, float dpi)> MockCreateBitmapFromWicResource;

        std::function<ComPtr<ID2D1GradientStopCollection1>(
            std::vector<D2D1_GRADIENT_STOP>&&,
            D2D1_COLOR_SPACE,
            D2D1_COLOR_SPACE,
            D2D1_BUFFER_PRECISION,
            D2D1_EXTEND_MODE,
            D2D1_COLOR_INTERPOLATION_MODE)> MockCreateGradientStopCollection;

        std::function<ComPtr<ID2D1LinearGradientBrush>(
            ID2D1GradientStopCollection1* stopCollection)> MockCreateLinearGradientBrush;

        std::function<ComPtr<ID2D1RadialGradientBrush>(
            ID2D1GradientStopCollection1* stopCollection)> MockCreateRadialGradientBrush;

        CALL_COUNTER_WITH_MOCK(TrimMethod, HRESULT());
        CALL_COUNTER_WITH_MOCK(GetInterfaceMethod, HRESULT(REFIID,void**));
        CALL_COUNTER_WITH_MOCK(CreateDeviceContextForDrawingSessionMethod, ComPtr<ID2D1DeviceContext1>());
        CALL_COUNTER_WITH_MOCK(CreateBitmapFromBytesMethod, ComPtr<ID2D1Bitmap1>(uint8_t*, uint32_t, int32_t, int32_t, float, DirectXPixelFormat, CanvasAlphaMode));
        CALL_COUNTER_WITH_MOCK(CreateBitmapFromSurfaceMethod, ComPtr<ID2D1Bitmap1>(IDirect3DSurface*, float, CanvasAlphaMode));
        CALL_COUNTER_WITH_MOCK(CreateRenderTargetBitmapMethod, ComPtr<ID2D1Bitmap1>(float, float, float, DirectXPixelFormat, CanvasAlphaMode));
        CALL_COUNTER_WITH_MOCK(CreateSwapChainForCompositionMethod, ComPtr<IDXGISwapChain1>(int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode));
        CALL_COUNTER_WITH_MOCK(CreateSwapChainForCoreWindowMethod, ComPtr<IDXGISwapChain1>(ICoreWindow*, int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode));
        CALL_COUNTER_WITH_MOCK(CreateCommandListMethod, ComPtr<ID2D1CommandList>());

        CALL_COUNTER_WITH_MOCK(CreateRectangleGeometryMethod, ComPtr<ID2D1RectangleGeometry>(D2D1_RECT_F const&));
        CALL_COUNTER_WITH_MOCK(CreateEllipseGeometryMethod, ComPtr<ID2D1EllipseGeometry>(D2D1_ELLIPSE const&));
        CALL_COUNTER_WITH_MOCK(CreateRoundedRectangleGeometryMethod, ComPtr<ID2D1RoundedRectangleGeometry>(D2D1_ROUNDED_RECT const&));

        CALL_COUNTER_WITH_MOCK(CreatePathGeometryMethod, ComPtr<ID2D1PathGeometry1>());

        CALL_COUNTER_WITH_MOCK(CreateGeometryGroupMethod, ComPtr<ID2D1GeometryGroup>(D2D1_FILL_MODE, ID2D1Geometry**, UINT32));
        CALL_COUNTER_WITH_MOCK(CreateTransformedGeometryMethod, ComPtr<ID2D1TransformedGeometry>(ID2D1Geometry*, D2D1_MATRIX_3X2_F*));

        CALL_COUNTER_WITH_MOCK(CreateFilledGeometryRealizationMethod, ComPtr<ID2D1GeometryRealization>(ID2D1Geometry*, float));
        CALL_COUNTER_WITH_MOCK(CreateStrokedGeometryRealizationMethod, ComPtr<ID2D1GeometryRealization>(ID2D1Geometry*, float, ID2D1StrokeStyle*, float));

        CALL_COUNTER_WITH_MOCK(CreatePrintControlMethod, ComPtr<ID2D1PrintControl>(IPrintDocumentPackageTarget*, float));
        
        CALL_COUNTER_WITH_MOCK(GetResourceCreationDeviceContextMethod, DeviceContextLease());

        CALL_COUNTER_WITH_MOCK(GetPrimaryDisplayOutputMethod, ComPtr<IDXGIOutput>());

        CALL_COUNTER_WITH_MOCK(LeaseHistogramEffectMethod, HistogramAndAtlasEffects(ID2D1DeviceContext*));
        CALL_COUNTER_WITH_MOCK(ReleaseHistogramEffectMethod, void(HistogramAndAtlasEffects));

        CALL_COUNTER_WITH_MOCK(IsBufferPrecisionSupportedMethod, HRESULT(CanvasBufferPrecision, boolean*));

        CALL_COUNTER_WITH_MOCK(RaiseDeviceLostMethod, HRESULT());

        CALL_COUNTER_WITH_MOCK(LockMethod, HRESULT(ICanvasLock**));

        CALL_COUNTER_WITH_MOCK(add_DeviceLostMethod, HRESULT(DeviceLostHandlerType*, EventRegistrationToken*));
        CALL_COUNTER_WITH_MOCK(remove_DeviceLostMethod, HRESULT(EventRegistrationToken));

        CALL_COUNTER_WITH_MOCK(GetDeviceRemovedErrorCodeMethod, HRESULT());

        CALL_COUNTER_WITH_MOCK(IsDeviceLostMethod, HRESULT(int, boolean*));

#if WINVER > _WIN32_WINNT_WINBLUE
        CALL_COUNTER_WITH_MOCK(CreateGradientMeshMethod, ComPtr<ID2D1GradientMesh>(D2D1_GRADIENT_MESH_PATCH const*, UINT32));
        CALL_COUNTER_WITH_MOCK(IsSpriteBatchQuirkRequiredMethod, bool());

        CALL_COUNTER_WITH_MOCK(CreateSvgDocumentMethod, ComPtr<ID2D1SvgDocument>(IStream*));
#endif

        //
        // ICanvasDevice
        //

        IFACEMETHODIMP get_ForceSoftwareRenderer(boolean *) override
        {
            Assert::Fail(L"Unexpected call to get_ForceSoftwareRenderer");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_MaximumBitmapSizeInPixels(int32_t* value) override
        {
            Assert::Fail(L"Unexpected call to get_MaximumBitmapSizeInPixels");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP IsPixelFormatSupported(DirectXPixelFormat pixelFormat, boolean* value) override
        {
            Assert::Fail(L"Unexpected call to IsPixelFormatSupported");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP IsBufferPrecisionSupported(CanvasBufferPrecision bufferPrecision, boolean* value) override
        {
            return IsBufferPrecisionSupportedMethod.WasCalled(bufferPrecision, value);
        }

        IFACEMETHODIMP get_MaximumCacheSize(UINT64* value) override
        {
            Assert::Fail(L"Unexpected call to get_MaximumCacheSize");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP put_MaximumCacheSize(UINT64 value) override
        {
            Assert::Fail(L"Unexpected call to put_MaximumCacheSize");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_LowPriority(boolean* value) override
        {
            Assert::Fail(L"Unexpected call to get_LowPriority");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP put_LowPriority(boolean value) override
        {
            Assert::Fail(L"Unexpected call to put_LowPriority");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_DeviceLost(
            DeviceLostHandlerType* value,
            EventRegistrationToken* token)
        {
            return add_DeviceLostMethod.WasCalled(value, token);
        }

        IFACEMETHODIMP remove_DeviceLost(
            EventRegistrationToken token)
        {
            return remove_DeviceLostMethod.WasCalled(token);
        }

        IFACEMETHODIMP IsDeviceLost(
            int hresult,
            boolean* value)
        {
            return IsDeviceLostMethod.WasCalled(hresult, value);
        }

        IFACEMETHODIMP RaiseDeviceLost()
        {
            return RaiseDeviceLostMethod.WasCalled();
        }

        IFACEMETHODIMP Lock(ICanvasLock** value)
        {
            return LockMethod.WasCalled(value);
        }

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value) override
        {
            if (!Mockget_Device)
            {
                Assert::Fail(L"Unexpected call to get_Device");
                return E_NOTIMPL;
            }

            Mockget_Device(value);
            return S_OK;
        }

        //
        // IDirect3DDevice
        //

        IFACEMETHODIMP Trim() override
        {
            return TrimMethod.WasCalled();
        }

        //
        // IDirect3DDxgiInterfaceAccess
        //
        IFACEMETHODIMP GetInterface(REFIID iid, void** p) override
        {
            return GetInterfaceMethod.WasCalled(iid, p);
        }

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            if (!MockGetD2DDevice)
            {
                Assert::Fail(L"Unexpected call to GetDevice");
                ThrowHR(E_NOTIMPL);
            }

            return MockGetD2DDevice();
        }

        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContextForDrawingSession() override
        {
            return CreateDeviceContextForDrawingSessionMethod.WasCalled();
        }

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(D2D1_COLOR_F const& color) override
        {
            if (!MockCreateSolidColorBrush)
            {
                Assert::Fail(L"Unexpected call to CreateSolidColorBrush");
                return nullptr;
            }

            return MockCreateSolidColorBrush(color);
        }

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromWicResource(
            IWICBitmapSource* converter,
            float dpi,
            CanvasAlphaMode alpha) override
        {
            if (!MockCreateBitmapFromWicResource)
            {
                Assert::Fail(L"Unexpected call to CreateBitmapFromWicResource");
                return nullptr;
            }
            return MockCreateBitmapFromWicResource(converter, alpha, dpi);
        }

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromBytes(
            uint8_t* bytes,
            uint32_t pitch,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alphaMode) override
        {
            return CreateBitmapFromBytesMethod.WasCalled(bytes, pitch, widthInPixels, heightInPixels, dpi, format, alphaMode);
        }

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromSurface(
            IDirect3DSurface* surface,
            float dpi,
            CanvasAlphaMode alphaMode) override
        {
            return CreateBitmapFromSurfaceMethod.WasCalled(surface, dpi, alphaMode);
        }

        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) override
        {
            return CreateRenderTargetBitmapMethod.WasCalled(width, height, dpi, format, alpha);
        }

        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* Bitmap) override
        {
            if (!MockCreateBitmapBrush)
            {
                Assert::Fail(L"Unexpected call to CreateBitmapBrush");
                return nullptr;
            }

            return MockCreateBitmapBrush(Bitmap);
        }

        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) override
        {
            if (!MockCreateImageBrush)
            {
                Assert::Fail(L"Unexpected call to CreateImageBrush");
                return nullptr;
            }

            return MockCreateImageBrush(image);
        }

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            std::vector<D2D1_GRADIENT_STOP>&& stops,
            D2D1_COLOR_SPACE preInterpolationSpace,
            D2D1_COLOR_SPACE postInterpolationSpace,
            D2D1_BUFFER_PRECISION bufferPrecision,
            D2D1_EXTEND_MODE extendMode,
            D2D1_COLOR_INTERPOLATION_MODE interpolationMode) override
        {
            if (!MockCreateGradientStopCollection)
            {
                Assert::Fail(L"Unexpected call to CreateGradientStopCollection");
                return nullptr;
            }

            return MockCreateGradientStopCollection(
                std::move(stops),
                preInterpolationSpace,
                postInterpolationSpace,
                bufferPrecision,
                extendMode,
                interpolationMode);
        }

        virtual ComPtr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override
        {
            if (!MockCreateLinearGradientBrush)
            {
                Assert::Fail(L"Unexpected call to CreateLinearGradientBrush");
                return nullptr;
            }

            return MockCreateLinearGradientBrush(stopCollection);
        }

        virtual ComPtr<ID2D1RadialGradientBrush> CreateRadialGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override
        {
            if (!MockCreateRadialGradientBrush)
            {
                Assert::Fail(L"Unexpected call to CreateRadialGradientBrush");
                return nullptr;
            }

            return MockCreateRadialGradientBrush(stopCollection);
        }

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForComposition(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) override
        {
            return CreateSwapChainForCompositionMethod.WasCalled(widthInPixels, heightInPixels, format, bufferCount, alphaMode);
        }

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForCoreWindow(
            ICoreWindow* coreWindow,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) override
        {
            return CreateSwapChainForCoreWindowMethod.WasCalled(coreWindow, widthInPixels, heightInPixels, format, bufferCount, alphaMode);
        }

        virtual ComPtr<ID2D1CommandList> CreateCommandList() override
        {
            return CreateCommandListMethod.WasCalled();
        }

        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(D2D1_RECT_F const& rect) override
        {
            return CreateRectangleGeometryMethod.WasCalled(rect);
        }

        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(D2D1_ELLIPSE const& ellipse) override
        {
            return CreateEllipseGeometryMethod.WasCalled(ellipse);
        }

        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(D2D1_ROUNDED_RECT const& roundedRect) override
        {
            return CreateRoundedRectangleGeometryMethod.WasCalled(roundedRect);
        }

        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry() override
        {
            return CreatePathGeometryMethod.WasCalled();
        }

        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) override
        {
            return CreateGeometryGroupMethod.WasCalled(fillMode, d2dGeometries, geometryCount);
        }

        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) override
        {
            return CreateTransformedGeometryMethod.WasCalled(d2dGeometry, transform);
        }

        virtual ComPtr<ID2D1GeometryRealization> CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance) override
        {
            return CreateFilledGeometryRealizationMethod.WasCalled(geometry, flatteningTolerance);
        }

        virtual ComPtr<ID2D1GeometryRealization> CreateStrokedGeometryRealization(
            ID2D1Geometry* geometry,
            float strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            float flatteningTolerance) override
        {
            return CreateStrokedGeometryRealizationMethod.WasCalled(geometry, strokeWidth, strokeStyle, flatteningTolerance);
        }

        virtual ComPtr<ID2D1PrintControl> CreatePrintControl(IPrintDocumentPackageTarget* target, float dpi) override
        {
            return CreatePrintControlMethod.WasCalled(target, dpi);
        }

        virtual DeviceContextLease GetResourceCreationDeviceContext() override
        {
            return GetResourceCreationDeviceContextMethod.WasCalled();
        }

        virtual ComPtr<IDXGIOutput> GetPrimaryDisplayOutput() override
        {
            return GetPrimaryDisplayOutputMethod.WasCalled();
        }

        virtual void ThrowIfCreateSurfaceFailed(HRESULT hr, wchar_t const* typeName, uint32_t width, uint32_t height) override
        {
            ThrowIfFailed(hr);
        }

        virtual HistogramAndAtlasEffects LeaseHistogramEffect(ID2D1DeviceContext* d2dContext) override
        {
            return LeaseHistogramEffectMethod.WasCalled(d2dContext);
        }

        virtual void ReleaseHistogramEffect(HistogramAndAtlasEffects&& effects) override
        {
            return ReleaseHistogramEffectMethod.WasCalled(effects);
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        virtual ComPtr<ID2D1GradientMesh> CreateGradientMesh(
            D2D1_GRADIENT_MESH_PATCH const* patches,
            UINT32 patchCount) override
        {
            return CreateGradientMeshMethod.WasCalled(patches, patchCount);
        }

        virtual bool IsSpriteBatchQuirkRequired()
        {
            return IsSpriteBatchQuirkRequiredMethod.WasCalled();
        }

        ComPtr<ID2D1SvgDocument> CreateSvgDocument(IStream* inputXmlStream)
        {
            return CreateSvgDocumentMethod.WasCalled(inputXmlStream);
        }
#endif
    };
}

