// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "DeviceContextPool.h"
#include "Utils/GuidUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace WinRTDirectX;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::ApplicationModel::Core;

    class CanvasDevice;
    class SharedDeviceState;
    class DefaultDeviceAdapter;


    //
    // Abstracts away some lower-level resource access, allowing unit
    // tests to provide test doubles. Because they are internal, they can return
    // ComPtrs and throw exceptions on failure.
    //
    class CanvasDeviceAdapter : public Singleton<CanvasDeviceAdapter, DefaultDeviceAdapter>
    {
    public:
        virtual ~CanvasDeviceAdapter() = default;

        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) = 0;

        virtual bool TryCreateD3DDevice(bool forceSoftwareRenderer, bool useDebugD3DDevice, ComPtr<ID3D11Device>* device) = 0;

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) = 0;

        virtual ComPtr<ICoreApplication> GetCoreApplication() = 0;

        virtual ComPtr<IPropertyValueStatics> GetPropertyValueStatics() = 0;
    };


    //
    // Default implementation of the adapter, used in production.
    //
    class DefaultDeviceAdapter : public CanvasDeviceAdapter
    {
        ComPtr<IPropertyValueStatics> m_propertyValueStatics;

    public:
        DefaultDeviceAdapter();

        virtual ComPtr<ID2D1Factory2> CreateD2DFactory(CanvasDebugLevel debugLevel) override;

        virtual bool TryCreateD3DDevice(bool forceSoftwareRenderer, bool useDebugD3DDevice, ComPtr<ID3D11Device>* device) override;

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
    class __declspec(uuid("E2C64DD1-4126-4C7A-A5D2-D5E8B2C7235C"))
    ICanvasDeviceInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Device1> GetD2DDevice() = 0;

        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContextForDrawingSession() = 0;

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(D2D1_COLOR_F const& color) = 0;

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromWicResource(
            IWICBitmapSource* wicBitmapSource,
            float dpi,
            CanvasAlphaMode alpha) = 0;

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromBytes(
            uint8_t* bytes,
            uint32_t pitch,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromSurface(
            IDirect3DSurface* surface,
            float dpi,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) = 0;

        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* bitmap) = 0;

        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) = 0;

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            std::vector<D2D1_GRADIENT_STOP>&& stops,
            D2D1_COLOR_SPACE preInterpolationSpace,
            D2D1_COLOR_SPACE postInterpolationSpace,
            D2D1_BUFFER_PRECISION bufferPrecision,
            D2D1_EXTEND_MODE extendMode,
            D2D1_COLOR_INTERPOLATION_MODE interpolationMode) = 0;

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

        virtual ComPtr<ID2D1GeometryRealization> CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance) = 0;
        virtual ComPtr<ID2D1GeometryRealization> CreateStrokedGeometryRealization(
            ID2D1Geometry* geometry, 
            float strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            float flatteningTolerance) = 0;

        virtual ComPtr<ID2D1PrintControl> CreatePrintControl(IPrintDocumentPackageTarget*, float dpi) = 0;

        virtual DeviceContextLease GetResourceCreationDeviceContext() = 0;

        virtual ComPtr<IDXGIOutput> GetPrimaryDisplayOutput() = 0;

        virtual void ThrowIfCreateSurfaceFailed(HRESULT hr, wchar_t const* typeName, uint32_t width, uint32_t height) = 0;

        struct HistogramAndAtlasEffects
        {
            ComPtr<ID2D1Effect> HistogramEffect;
            ComPtr<ID2D1Effect> AtlasEffect;
        };

        virtual HistogramAndAtlasEffects LeaseHistogramEffect(ID2D1DeviceContext* d2dContext) = 0;
        virtual void ReleaseHistogramEffect(HistogramAndAtlasEffects&& effects) = 0;

#if WINVER > _WIN32_WINNT_WINBLUE
        virtual ComPtr<ID2D1GradientMesh> CreateGradientMesh(D2D1_GRADIENT_MESH_PATCH const* patches, uint32_t patchCount) = 0;

        virtual bool IsSpriteBatchQuirkRequired() = 0;

        virtual ComPtr<ID2D1SvgDocument> CreateSvgDocument(IStream* inputXmlStream) = 0;
#endif
    };


    typedef ITypedEventHandler<CanvasDevice*, IInspectable*> DeviceLostHandlerType;

    //
    // The CanvasDevice class itself.
    //
    class CanvasDevice : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1Device1,
        CanvasDevice,
        ICanvasDevice,
        CloakedIid<ICanvasDeviceInternal>,
        ICanvasResourceCreator,
        IDirect3DDevice,
        CloakedIid<IDirect3DDxgiInterfaceAccess>,
        CloakedIid<ID2D1DeviceContextPool>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

        // This is used for the public-facing property value. If this device was
        // created using interop, this is set to false.
        bool m_forceSoftwareRenderer; 
        
        ClosablePtr<IDXGIDevice3> m_dxgiDevice;
        
        // Null-versus-non-null is not necessarily tied to whether the 
        // device object is open or closed.
        ComPtr<IDXGIOutput> m_primaryOutput;

        EventSource<DeviceLostHandlerType, InvokeModeOptions<StopOnFirstError>> m_deviceLostEventList;

        // Backreference keeps the shared device state alive as long as any device exists.
        std::shared_ptr<SharedDeviceState> m_sharedState;

        DeviceContextPool m_deviceContextPool;

        ComPtr<ID2D1Effect> m_histogramEffect;
        ComPtr<ID2D1Effect> m_atlasEffect;

#if WINVER > _WIN32_WINNT_WINBLUE
        std::mutex m_quirkMutex;
        
        enum class SpriteBatchQuirk
        {
            NeedsCheck,
            Required,
            NotRequired
        };

        SpriteBatchQuirk m_spriteBatchQuirk;
#endif

    public:
        static ComPtr<CanvasDevice> CreateNew(bool forceSoftwareRenderer);
        static ComPtr<CanvasDevice> CreateNew(IDirect3DDevice* direct3DDevice);

        CanvasDevice(
            ID2D1Device1* d2dDevice,
            IDXGIDevice3* dxgiDevice = nullptr,
            bool forceSoftwareRenderer = false);

        //
        // ICanvasDevice
        //

        IFACEMETHOD(get_ForceSoftwareRenderer)(boolean* value) override;

        IFACEMETHOD(get_MaximumBitmapSizeInPixels)(int32_t* value) override;

        IFACEMETHOD(IsPixelFormatSupported)(DirectXPixelFormat pixelFormat, boolean* value) override;

        IFACEMETHOD(IsBufferPrecisionSupported)(CanvasBufferPrecision bufferPrecision, boolean* value) override;

        IFACEMETHOD(get_MaximumCacheSize)(UINT64* value) override;
        IFACEMETHOD(put_MaximumCacheSize)(UINT64 value) override;

        IFACEMETHOD(get_LowPriority)(boolean* value) override;
        IFACEMETHOD(put_LowPriority)(boolean value) override;

        IFACEMETHOD(add_DeviceLost)(DeviceLostHandlerType* value, EventRegistrationToken* token) override;

        IFACEMETHOD(remove_DeviceLost)(EventRegistrationToken token) override;

        IFACEMETHOD(IsDeviceLost2)(boolean* value) override;
        IFACEMETHOD(IsDeviceLost)(int hresult, boolean* value) override;

        IFACEMETHOD(GetDeviceLostReason)(int* hresult) override;

        IFACEMETHOD(RaiseDeviceLost)() override;

        IFACEMETHOD(Lock)(ICanvasLock** value) override;

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

        virtual ComPtr<ID2D1DeviceContext1> CreateDeviceContextForDrawingSession() override;

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(D2D1_COLOR_F const& color) override;

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromWicResource(
            IWICBitmapSource* wicBitmapSource,
            float dpi,
            CanvasAlphaMode alpha) override;

        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromBytes(
            uint8_t* bytes,
            uint32_t pitch,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alphaMode) override;
            
        virtual ComPtr<ID2D1Bitmap1> CreateBitmapFromSurface(
            IDirect3DSurface* surface,
            float dpi,
            CanvasAlphaMode alphaMode) override;

        virtual ComPtr<ID2D1Bitmap1> CreateRenderTargetBitmap(
            float width,
            float height,
            float dpi,
            DirectXPixelFormat format,
            CanvasAlphaMode alpha) override;

        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* bitmap) override;

        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) override;

        virtual ComPtr<ID2D1LinearGradientBrush> CreateLinearGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override;

        virtual ComPtr<ID2D1RadialGradientBrush> CreateRadialGradientBrush(
            ID2D1GradientStopCollection1* stopCollection) override;

        virtual ComPtr<ID2D1GradientStopCollection1> CreateGradientStopCollection(
            std::vector<D2D1_GRADIENT_STOP>&& stops,
            D2D1_COLOR_SPACE preInterpolationSpace,
            D2D1_COLOR_SPACE postInterpolationSpace,
            D2D1_BUFFER_PRECISION bufferPrecision,
            D2D1_EXTEND_MODE extendMode,
            D2D1_COLOR_INTERPOLATION_MODE interpolationMode) override;

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

        virtual ComPtr<ID2D1GeometryRealization> CreateFilledGeometryRealization(ID2D1Geometry* geometry, float flatteningTolerance) override;
        virtual ComPtr<ID2D1GeometryRealization> CreateStrokedGeometryRealization(
            ID2D1Geometry* geometry,
            float strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            float flatteningTolerance) override;

        virtual ComPtr<ID2D1PrintControl> CreatePrintControl(
            IPrintDocumentPackageTarget*,
            float dpi) override;

        virtual DeviceContextLease GetResourceCreationDeviceContext() override final;

        virtual ComPtr<IDXGIOutput> GetPrimaryDisplayOutput() override;

        virtual void ThrowIfCreateSurfaceFailed(HRESULT hr, wchar_t const* typeName, uint32_t width, uint32_t height) override;

        virtual HistogramAndAtlasEffects LeaseHistogramEffect(ID2D1DeviceContext* d2dContext) override;
        virtual void ReleaseHistogramEffect(HistogramAndAtlasEffects&& effects) override;

#if WINVER > _WIN32_WINNT_WINBLUE
        virtual ComPtr<ID2D1GradientMesh> CreateGradientMesh(D2D1_GRADIENT_MESH_PATCH const* patches, uint32_t patchCount) override;

        virtual bool IsSpriteBatchQuirkRequired() override;

        virtual ComPtr<ID2D1SvgDocument> CreateSvgDocument(IStream* inputXmlStream) override;
#endif

        //
        // IDirect3DDevice
        //

        IFACEMETHOD(Trim)() override;

        //
        // IDirect3DDxgiInterfaceAccess
        //

        IFACEMETHOD(GetInterface)(IID const&, void**) override;

        //
        // ID2D1DeviceContextPool
        //
        IFACEMETHOD(GetDeviceContextLease)(ID2D1DeviceContextLease** lease) override;

        //
        // Internal
        //
        HRESULT GetDeviceRemovedErrorCode();

    private:
        static ComPtr<ID3D11Device> MakeD3D11Device(CanvasDeviceAdapter* adapter, bool forceSoftwareRenderer, bool useDebugD3DDevice);

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

        bool DetectIfSpriteBatchQuirkIsRequired();

        ComPtr<ID2D1Bitmap1> CreateBitmapFromWicBitmap(ID2D1DeviceContext* deviceContext, IWICBitmapSource* wicBitmapSource, float dpi, CanvasAlphaMode alpha);
        ComPtr<ID2D1Bitmap1> CreateBitmapFromDdsFrame(ID2D1DeviceContext* deviceContext, IWICBitmapSource* wicBitmapSource, IWICDdsFrameDecode* ddsFrame, float dpi, CanvasAlphaMode alpha);

        // telemetry
        static void LogCreateCanvasDevice();
    };

    //
    // ID2D1DeviceContextLease implementation that is returned by CanvasDevice::GetDeviceContextLease.
    // This type is just a very thin COM object wrapping a DeviviceContextLease, which is the lease
    // object holding a pooled ID2D1DeviceContext instance already used internally by Win2D effects.
    //
    class D2D1DeviceContextLease final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1DeviceContextLease>
    {
    public:
        D2D1DeviceContextLease(CanvasDevice* canvasDevice)
        {
            CheckInPointer(canvasDevice);

            m_deviceContext = canvasDevice->GetResourceCreationDeviceContext();
        }

        IFACEMETHOD(GetD2DDeviceContext)(ID2D1DeviceContext** deviceContext) override;

    private:
        DeviceContextLease m_deviceContext;
    };


    //
    // Singleton shared by all active CanvasDevice instances, responsible for tracking global
    // state such as the shared device pool and value of the debug level static property.
    //
    class SharedDeviceState : public Singleton<SharedDeviceState>
    {
        std::shared_ptr<CanvasDeviceAdapter> m_adapter;

        WeakRef m_sharedDevices[2];

        CanvasDebugLevel m_sharedDeviceDebugLevels[2];
        CanvasDebugLevel m_currentDebugLevel;

        int m_isID2D1Factory5Supported; // negative = not yet checked.
        std::map<const IID, bool, GuidComparer> m_cachedRegisteredEffects;

        std::recursive_mutex m_mutex;

    public:
        SharedDeviceState();

        virtual ~SharedDeviceState();
        
        ComPtr<ICanvasDevice> GetSharedDevice(bool forceSoftwareRenderer);

        CanvasDebugLevel GetDebugLevel();
        void SetDebugLevel(CanvasDebugLevel const& value);

        bool IsID2D1Factory5Supported();
        bool IsEffectRegistered(IID const& effectId, bool cacheResult); //Only set cacheResult to true for built-in effects, which cannot be unregistered.

        CanvasDeviceAdapter* GetAdapter() const { return m_adapter.get(); }

    private:
        CanvasDebugLevel LoadDebugLevelProperty();
        void SaveDebugLevelProperty(CanvasDebugLevel debugLevel);

        struct PropertyData
        {
            Wrappers::HStringReference KeyName;
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
        : public AgileActivationFactory<ICanvasDeviceFactory, ICanvasDeviceStatics, CloakedIid<ICanvasFactoryNative>>
        , private LifespanTracker<CanvasDeviceFactory>
                                
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice, BaseTrust);

    public:
        //
        // ActivationFactory
        //

        IFACEMETHOD(ActivateInstance)(IInspectable** ppvObject) override;

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
            ICanvasDevice** device);

        IFACEMETHOD(GetSharedDeviceWithForceSoftwareRenderer)(
            boolean forceSoftwareRenderer,
            ICanvasDevice** device);

        IFACEMETHOD(put_DebugLevel)(CanvasDebugLevel debugLevel);
        IFACEMETHOD(get_DebugLevel)(CanvasDebugLevel* debugLevel);

        //
        // ICanvasFactoryNative.
        //
        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            float dpi,
            IInspectable** wrapper) override;
    };

    static uint32_t const QUALCOMM_VENDOR_ID = 0x4D4F4351;

}}}}
