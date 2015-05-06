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
        std::function<ComPtr<ID2D1Bitmap1>(
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha,
            float dpi)> MockCreateRenderTargetBitmap;
        std::function<ComPtr<ID2D1Image>(ICanvasImage* canvasImage)> MockGetD2DImage;

        std::function<ComPtr<ID2D1GradientStopCollection1>(
            UINT gradientStopCount,
            CanvasGradientStop const* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            CanvasAlphaMode alphaMode)> MockCreateGradientStopCollection;

        std::function<ComPtr<ID2D1LinearGradientBrush>(
            ID2D1GradientStopCollection1* stopCollection)> MockCreateLinearGradientBrush;

        std::function<ComPtr<ID2D1RadialGradientBrush>(
            ID2D1GradientStopCollection1* stopCollection)> MockCreateRadialGradientBrush;

        CALL_COUNTER_WITH_MOCK(TrimMethod, HRESULT());
        CALL_COUNTER_WITH_MOCK(GetInterfaceMethod, HRESULT(REFIID,void**));
        CALL_COUNTER_WITH_MOCK(CreateDeviceContextMethod, ComPtr<ID2D1DeviceContext1>());
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

        CALL_COUNTER_WITH_MOCK(GetResourceCreationDeviceContextMethod, ComPtr<ID2D1DeviceContext1>());

        //
        // ICanvasDevice
        //

        IFACEMETHODIMP get_HardwareAcceleration(CanvasHardwareAcceleration *) override
        {
            Assert::Fail(L"Unexpected call to get_HardwareAcceleration");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_MaximumBitmapSizeInPixels(int32_t* value) override
        {
            Assert::Fail(L"Unexpected call to get_MaximumBitmapSizeInPixels");
            return E_NOTIMPL;
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

        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContext() override
        {
            return CreateDeviceContextMethod.WasCalled();
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

        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) override
        {
            if (!MockCreateRenderTargetBitmap)
            {
                Assert::Fail(L"Unexpected call to CreateRenderTargetBitmap");
                return nullptr;
            }
            return MockCreateRenderTargetBitmap(width, height, format, alpha, dpi);
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

        virtual ComPtr<ID2D1Image> GetD2DImage(ICanvasImage* canvasImage) override
        {
            if (!MockGetD2DImage)
            {
                Assert::Fail(L"Unexpected call to GetD2DImage");
                return nullptr;
            }

            return MockGetD2DImage(canvasImage);
        }

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            uint32_t gradientStopCount,
            CanvasGradientStop const* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            CanvasAlphaMode alphaMode) override
        {
            if (!MockCreateGradientStopCollection)
            {
                Assert::Fail(L"Unexpected call to CreateGradientStopCollection");
                return nullptr;
            }

            return MockCreateGradientStopCollection(
                gradientStopCount,
                gradientStops,
                edgeBehavior,
                preInterpolationSpace,
                postInterpolationSpace,
                bufferPrecision,
                alphaMode);
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

        virtual ComPtr<ID2D1DeviceContext1> GetResourceCreationDeviceContext()
        {
            return GetResourceCreationDeviceContextMethod.WasCalled();
        }
    };
}

