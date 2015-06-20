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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace WinRTDirectX;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::ApplicationModel::Core;

    class CanvasDevice;
    class CanvasDeviceManager;

    //
    // Abstracts away some lower-level resource access, allowing unit
    // tests to provide test doubles. Because they are internal, they can return
    // ComPtrs and throw exceptions on failure.
    //
    class ICanvasDeviceResourceCreationAdapter
    {
    public:
        virtual ~ICanvasDeviceResourceCreationAdapter() = default;

        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) = 0;

        virtual bool TryCreateD3DDevice(bool useSoftwareRenderer, ComPtr<ID3D11Device>* device) = 0;

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) = 0;

        virtual ComPtr<ICoreApplication> GetCoreApplication() = 0;

        virtual ComPtr<IPropertyValueStatics> GetPropertyValueStatics() = 0;
    };


    //
    // Default implementation of the adapter, used in production.
    //
    class DefaultDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter,
                                                 private LifespanTracker<DefaultDeviceResourceCreationAdapter>
    {
        ComPtr<IPropertyValueStatics> m_propertyValueStatics;

    public:
        DefaultDeviceResourceCreationAdapter();

        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) override;

        virtual bool TryCreateD3DDevice(bool useSoftwareRenderer, ComPtr<ID3D11Device>* device) override;

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) override;

        virtual ComPtr<ICoreApplication> GetCoreApplication() override;

        virtual ComPtr<IPropertyValueStatics> GetPropertyValueStatics() override;
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
            IWICBitmapSource* wicBitmapSource,
            float dpi,
            CanvasAlphaMode alpha) = 0;
        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) = 0;
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

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForComposition(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForCoreWindow(
            ICoreWindow* coreWindow,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<ID2D1CommandList> CreateCommandList() = 0;

        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(D2D1_RECT_F const& rectangle) = 0;
        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(D2D1_ELLIPSE const& ellipse) = 0;
        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(D2D1_ROUNDED_RECT const& roundedRect) = 0;
        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry() = 0;
        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) = 0;
        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) = 0;

        virtual ComPtr<ID2D1GeometryRealization> CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance) = 0;
        virtual ComPtr<ID2D1GeometryRealization> CreateStrokedGeometryRealization(
            ID2D1Geometry* geometry, 
            float strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            float flatteningTolerance) = 0;

        virtual ComPtr<ID2D1DeviceContext1> GetResourceCreationDeviceContext() = 0;

        virtual ComPtr<IDXGIOutput> GetPrimaryDisplayOutput() = 0;
    };


    struct CanvasDeviceTraits
    {
        typedef ID2D1Device1 resource_t;
        typedef CanvasDevice wrapper_t;
        typedef ICanvasDevice wrapper_interface_t;
        typedef CanvasDeviceManager manager_t;
    };

    typedef ITypedEventHandler<CanvasDevice*, IInspectable*> DeviceLostHandlerType;

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

        // This is used for the public-facing property value. If this device was
        // created using interop, this is set to false.
        bool m_forceSoftwareRenderer; 
        
        ClosablePtr<IDXGIDevice3> m_dxgiDevice;
        ClosablePtr<ID2D1DeviceContext1> m_d2dResourceCreationDeviceContext;

        // Null-versus-non-null is not necessarily tied to whether the 
        // device object is open or closed.
        ComPtr<IDXGIOutput> m_primaryOutput;

        EventSource<DeviceLostHandlerType, InvokeModeOptions<StopOnFirstError>> m_deviceLostEventList;

    public:
        CanvasDevice(
            std::shared_ptr<CanvasDeviceManager> manager,
            bool forceSoftwareRenderer,
            IDXGIDevice3* dxgiDevice,
            ID2D1Device1* d2dDevice);

        //
        // ICanvasDevice
        //

        IFACEMETHOD(get_ForceSoftwareRenderer)(boolean* value) override;

        IFACEMETHOD(get_MaximumBitmapSizeInPixels)(int32_t* value) override;

        IFACEMETHOD(add_DeviceLost)(DeviceLostHandlerType* value, EventRegistrationToken* token) override;

        IFACEMETHOD(remove_DeviceLost)(EventRegistrationToken token) override;

        IFACEMETHOD(IsDeviceLost)(int hresult, boolean* value) override;

        IFACEMETHOD(RaiseDeviceLost)() override;

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
            IWICBitmapSource* wicBitmapSource,
            float dpi,
            CanvasAlphaMode alpha) override;
        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) override;
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

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForComposition(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) override;

        virtual ComPtr<IDXGISwapChain1> CreateSwapChainForCoreWindow(
            ICoreWindow* coreWindow,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode) override;

        virtual ComPtr<ID2D1CommandList> CreateCommandList() override;

        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(D2D1_RECT_F const& rectangle) override;
        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(D2D1_ELLIPSE const& ellipse) override;
        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(D2D1_ROUNDED_RECT const& roundedRect) override;
        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry() override;
        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) override;
        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) override;

        virtual ComPtr<ID2D1GeometryRealization> CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance) override;
        virtual ComPtr<ID2D1GeometryRealization> CreateStrokedGeometryRealization(
            ID2D1Geometry* geometry,
            float strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            float flatteningTolerance) override;

        virtual ComPtr<ID2D1DeviceContext1> GetResourceCreationDeviceContext() override;

        virtual ComPtr<IDXGIOutput> GetPrimaryDisplayOutput() override;

        //
        // IDirect3DDevice
        //

        IFACEMETHOD(Trim)() override;

        //
        // IDirect3DDxgiInterfaceAccess
        //

        IFACEMETHOD(GetInterface)(IID const&, void**) override;

        //
        // Internal
        //
        HRESULT GetDeviceRemovedErrorCode();

    private:
        template<typename FN>
        ComPtr<IDXGISwapChain1> CreateSwapChain(
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            int32_t bufferCount,
            CanvasAlphaMode alphaMode,
            FN&& createFn);

        ComPtr<ID2D1Factory2> GetD2DFactory();

        void InitializePrimaryOutput(IDXGIDevice3* dxgiDevice);
    };


    //
    // Responsible for creating and tracking CanvasDevice instances and the
    // ID2D1Device they wrap.
    //
    class CanvasDeviceManager : public ResourceManager<CanvasDeviceTraits>
    {
        std::shared_ptr<ICanvasDeviceResourceCreationAdapter> m_adapter;

        WeakRef m_sharedDevices[2];
        std::mutex m_mutex;

        std::atomic<CanvasDebugLevel> m_debugLevel;
    public:
        CanvasDeviceManager(std::shared_ptr<ICanvasDeviceResourceCreationAdapter> adapter);

        virtual ~CanvasDeviceManager();
        
        ComPtr<CanvasDevice> CreateNew(bool forceSoftwareRenderer);
        ComPtr<CanvasDevice> CreateNew(bool forceSoftwareRenderer, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateNew(IDirect3DDevice* direct3DDevice);
        ComPtr<CanvasDevice> CreateNew(IDirect3DDevice* direct3DDevice, ID2D1Factory2* d2dFactory);
        ComPtr<CanvasDevice> CreateWrapper(ID2D1Device1* d2dDevice);

        ComPtr<ICanvasDevice> GetSharedDevice(bool forceSoftwareRenderer);

        CanvasDebugLevel GetDebugLevel();
        void SetDebugLevel(CanvasDebugLevel const& value);

    private:
        ComPtr<IDXGIDevice3> MakeDXGIDevice(bool forceSoftwareRenderer) const;

        ComPtr<ID3D11Device> MakeD3D11Device(bool forceSoftwareRenderer) const;

        CanvasDebugLevel LoadDebugLevelProperty();
        void SaveDebugLevelProperty(CanvasDebugLevel debugLevel);

        struct PropertyData
        {
            HStringReference KeyName;
            ComPtr<IMap<HSTRING, IInspectable*>> PropertyMap;
            ComPtr<IInspectable> PropertyHolder;
            HRESULT LookupResult;
        };
        PropertyData GetDebugLevelPropertyData();
        void StoreValueToPropertyKey(PropertyData key, CanvasDebugLevel value);
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

        IFACEMETHOD(CreateWithForceSoftwareRendererOption)(
            boolean forceSoftwareRenderer,
            ICanvasDevice** canvasDevice) override;

        IFACEMETHOD(CreateFromDirect3D11Device)(
            IDirect3DDevice* direct3DDevice,
            ICanvasDevice** canvasDevice) override;

        //
        // ICanvasDeviceStatics
        //
        IFACEMETHOD(GetSharedDevice)(
            boolean forceSoftwareRenderer,
            ICanvasDevice** device);

        IFACEMETHOD(put_DebugLevel)(CanvasDebugLevel debugLevel);
        IFACEMETHOD(get_DebugLevel)(CanvasDebugLevel* debugLevel);

        //
        // Used by PerApplicationManager
        //
        static std::shared_ptr<CanvasDeviceManager> CreateManager();
    };
}}}}
