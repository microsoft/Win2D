// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    class ColorManagementProfile : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1ColorContext,
        ColorManagementProfile,
        IColorManagementProfile,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementProfile, BaseTrust);

        std::mutex m_mutex;
        bool m_closed;

        CanvasColorSpace m_colorSpace;      // Always valid.
        std::vector<uint8_t> m_iccProfile;  // Only used when not realized (to avoid duplicating the data).

#if WINVER > _WIN32_WINNT_WINBLUE
        ColorManagementProfileType m_type;
        ColorManagementSimpleProfile m_simpleProfile;
        ExtendedColorSpace m_extendedColorSpace;
#endif

        ComPtr<ICanvasDevice> m_device;

    public:
        ColorManagementProfile(CanvasColorSpace colorSpace);
        ColorManagementProfile(uint32_t iccProfileCount, uint8_t* iccProfile);
#if WINVER > _WIN32_WINNT_WINBLUE
        ColorManagementProfile(ColorManagementSimpleProfile const& simpleProfile);
        ColorManagementProfile(ExtendedColorSpace colorSpace);
#endif
        ColorManagementProfile(ICanvasDevice* device, ID2D1ColorContext* d2dColorContext);

        // IColorManagementProfile
        IFACEMETHOD(get_ColorSpace)(CanvasColorSpace* value) override;
        IFACEMETHOD(get_IccProfile)(uint32_t* valueCount, uint8_t** valueElements) override;

#if WINVER > _WIN32_WINNT_WINBLUE
        IFACEMETHOD(get_Type)(ColorManagementProfileType* value) override;
        IFACEMETHOD(get_SimpleProfile)(IReference<ColorManagementSimpleProfile>** value) override;
        IFACEMETHOD(get_ExtendedColorSpace)(ExtendedColorSpace* value) override;
#endif

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** resource) override;

        // ICanvasResourceWrapperWithDevice
        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;

    private:
        void Realize(ICanvasDevice* device);
        void ThrowIfClosed();
    };


    class ColorManagementProfileFactory
        : public AgileActivationFactory<IColorManagementProfileFactory, IColorManagementProfileStatics>
        , private LifespanTracker<ColorManagementProfileFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementProfile, BaseTrust);

    public:
        // IColorManagementProfileFactory
        IFACEMETHOD(Create)(CanvasColorSpace colorSpace, IColorManagementProfile** result) override;

        // IColorManagementProfileStatics
        IFACEMETHOD(CreateCustom)(uint32_t iccProfileCount, uint8_t* iccProfile, IColorManagementProfile** result) override;

#if WINVER > _WIN32_WINNT_WINBLUE
        IFACEMETHOD(CreateSimple)(ColorManagementSimpleProfile simpleProfile, IColorManagementProfile** result) override;
        IFACEMETHOD(CreateExtended)(ExtendedColorSpace colorSpace, IColorManagementProfile** result) override;

        IFACEMETHOD(IsSupported)(ColorManagementProfileType type, ICanvasDevice* device, boolean* result) override;
#endif
    };

}}}}}
