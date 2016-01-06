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

        ComPtr<ICanvasDevice> m_device;

    public:
        ColorManagementProfile(CanvasColorSpace colorSpace);
        ColorManagementProfile(uint32_t iccProfileCount, uint8_t* iccProfile);
        ColorManagementProfile(ICanvasDevice* device, ID2D1ColorContext* d2dColorContext);

        // IColorManagementProfile
        IFACEMETHOD(get_ColorSpace)(CanvasColorSpace* value) override;
        IFACEMETHOD(get_IccProfile)(uint32_t* valueCount, uint8_t** valueElements) override;

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
        : public ActivationFactory<IColorManagementProfileFactory, IColorManagementProfileStatics>
        , private LifespanTracker<ColorManagementProfileFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementProfile, BaseTrust);

    public:
        // IColorManagementProfileFactory
        IFACEMETHOD(Create)(CanvasColorSpace colorSpace, IColorManagementProfile** result) override;

        // IColorManagementProfileStatics
        IFACEMETHOD(CreateCustom)(uint32_t iccProfileCount, uint8_t* iccProfile, IColorManagementProfile** result) override;
    };

}}}}}
