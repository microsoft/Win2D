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

#include "ClosablePtr.h"
#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11;

    class CanvasDevice;
    class CanvasDeviceManager;

    //
    // Abstracts away the creation of a D2D factory / D3D device, allowing unit
    // tests to provide test doubles. Because they are internal, they can return
    // ComPtrs and throw exceptions on failure.
    //
    class ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) = 0;

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device) = 0;

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) = 0;
    };


    //
    // Default implementation of the adapter that actually talks to D3D / D2D
    // that is used in production.
    //
    class DefaultDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) override;

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device) override;

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) override;
    };


    //
    // This internal interface is exposed by the CanvasDevice runtime class and
    // allows for internal access to non-WinRT methods.
    //
    // Since this is a native C++ (ie not COM) object and these methods never
    // cross DLL boundaries these use exceptions for error handling and can
    // take/return C++ types.
    //
    [uuid(E2C64DD1-4126-4C7A-A5D2-D5E8B2C7235C)]
    class ICanvasDeviceInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Device1> GetD2DDevice() = 0;

        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContext() = 0;

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(D2D1_COLOR_F const& color) = 0;
        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromWicResource(
            IWICFormatConverter* wicConverter,
            CanvasAlphaMode alpha,
            float dpi) = 0;
        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha,
            float dpi) = 0;
        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* bitmap) = 0;
        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) = 0;
        virtual ComPtr<ID2D1Image> GetD2DImage(ICanvasImage* canvasImage) = 0;

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            uint32_t gradientStopCount,
            CanvasGradientStop const* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) = 0;

        virtual ComPtr<ID2D1RadialGradientBrush> CreateRadialGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) = 0;

        virtual ComPtr<IDXGISwapChain2> CreateSwapChain(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<ID2D1CommandList> CreateCommandList() = 0;
    };


    struct CanvasDeviceTraits
    {
        typedef ID2D1Device1 resource_t;
        typedef CanvasDevice wrapper_t;
        typedef ICanvasDevice wrapper_interface_t;
        typedef CanvasDeviceManager manager_t;
    };


    //
    // The CanvasDevice class itself.
    //
    class CanvasDevice : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasDeviceTraits, 
        CloakedIid<ICanvasDeviceInternal>,
        ICanvasResourceCreator,
        IDirect3DDevice,
        CloakedIid<IDirect3DDxgiInterfaceAccess>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

        // This is used for the public-facing property value. Can be any value
        // other than Auto.
        CanvasHardwareAcceleration m_hardwareAcceleration; 

        CanvasDebugLevel m_debugLevel;
        
        ClosablePtr<IDXGIDevice3> m_dxgiDevice;
        ClosablePtr<ID2D1DeviceContext1> m_d2dResourceCreationDeviceContext;

    public:
        CanvasDevice(
            std::shared_ptr<CanvasDeviceManager> manager,
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            IDXGIDevice3* dxgiDevice,
            ID2D1Device1* d2dDevice);

        //
        // ICanvasDevice
        //

        IFACEMETHOD(get_HardwareAcceleration)(_Out_ CanvasHardwareAcceleration* value) override;

        IFACEMETHOD(get_MaximumBitmapSizeInPixels)(int32_t* value) override;

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value);

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override;
        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContext() override;
        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(D2D1_COLOR_F const& color) override;
        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromWicResource(
            IWICFormatConverter* wicConverter,
            CanvasAlphaMode alpha,
            float dpi) override;
        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha,
            float dpi) override;
        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* bitmap) override;
        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) override;
        virtual ComPtr<ID2D1Image> GetD2DImage(ICanvasImage* canvasImage) override;

        virtual ComPtr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override;

        virtual ComPtr<ID2D1RadialGradientBrush> CreateRadialGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override;

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            uint32_t gradientStopCount,
            CanvasGradientStop const* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            CanvasAlphaMode alphaMode) override;

        virtual ComPtr<IDXGISwapChain2> CreateSwapChain(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) override;

        virtual ComPtr<ID2D1CommandList> CreateCommandList() override;

        //
        // IDirect3DDevice
        //

        IFACEMETHOD(Trim)() override;

        //
        // IDirect3DDxgiInterfaceAccess
        //

        IFACEMETHOD(GetInterface)(IID const&, void**) override;

    private:
        ComPtr<ID2D1Factory2> GetD2DFactory();
    };


    //
    // Responsible for creating and tracking CanvasDevice instances and the
    // ID2D1Device they wrap.
    //
    class CanvasDeviceManager : public ResourceManager<CanvasDeviceTraits>
    {
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> m_adapter;
    public:
        CanvasDeviceManager(std::shared_ptr<ICanvasDeviceResourceCreationAdapter> adapter);
        
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, CanvasHardwareAcceleration hardwareAcceleration);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, CanvasHardwareAcceleration hardwareAcceleration, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, IDirect3DDevice* direct3DDevice);
        ComPtr<CanvasDevice> CreateNew(CanvasDebugLevel debugLevel, IDirect3DDevice* direct3DDevice, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateWrapper(ID2D1Device1* d2dDevice);

    private:
        ComPtr<IDXGIDevice3> MakeDXGIDevice(
            CanvasHardwareAcceleration requestedHardwareAcceleration,
            CanvasHardwareAcceleration* actualHardwareAcceleration) const;

        ComPtr<ID3D11Device> MakeD3D11Device(
            CanvasHardwareAcceleration requestedHardwareAcceleration,
            CanvasHardwareAcceleration* actualHardwareAcceleration) const;
    };


    //
    // WinRT activation factory for the CanvasDevice runtimeclass.
    //
    class CanvasDeviceFactory 
        : public ActivationFactory<
            ICanvasDeviceFactory, 
            ICanvasDeviceStatics, 
            CloakedIid<ICanvasFactoryNative>>,
          public PerApplicationManager<CanvasDeviceFactory, CanvasDeviceManager>
                                
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASFACTORYNATIVE();

        //
        // ActivationFactory
        //

        IFACEMETHOD(ActivateInstance)(_COM_Outptr_ IInspectable** ppvObject) override;

        //
        // ICanvasDeviceFactory
        //

        IFACEMETHOD(CreateWithDebugLevel)(
            CanvasDebugLevel debugLevel,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateWithDebugLevelAndHardwareAcceleration)(
            CanvasDebugLevel debugLevel,
            CanvasHardwareAcceleration hardwareAcceleration,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateFromDirect3D11Device)(
            IDirect3DDevice* direct3DDevice,
            CanvasDebugLevel debugLevel,
            ICanvasDevice** canvasDevice) override;

        //
        // Used by PerApplicationManager
        //
        static std::shared_ptr<CanvasDeviceManager> CreateManager();
    };
}}}}
