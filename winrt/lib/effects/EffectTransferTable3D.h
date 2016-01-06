// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    class EffectTransferTable3D : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1LookupTable3D,
        EffectTransferTable3D,
        IEffectTransferTable3D,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_EffectTransferTable3D, BaseTrust);

        ClosablePtr<ICanvasDevice> m_device;

    public:
        static ComPtr<EffectTransferTable3D> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            Color* colors,
            int32_t sizeB,
            int32_t sizeG,
            int32_t sizeR);

        static ComPtr<EffectTransferTable3D> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t sizeB,
            int32_t sizeG,
            int32_t sizeR,
            DirectXPixelFormat format);

        EffectTransferTable3D(ICanvasDevice* device, ID2D1LookupTable3D* lookupTable);

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;
    };


    class EffectTransferTable3DFactory
        : public ActivationFactory<IEffectTransferTable3DStatics>
        , private LifespanTracker<EffectTransferTable3DFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_EffectTransferTable3D, BaseTrust);

    public:
        IFACEMETHOD(CreateFromColors)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            Color* colors,
            int32_t sizeB,
            int32_t sizeG,
            int32_t sizeR,
            IEffectTransferTable3D** result) override;

        IFACEMETHOD(CreateFromBytes)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t sizeB,
            int32_t sizeG,
            int32_t sizeR,
            DirectXPixelFormat format,
            IEffectTransferTable3D** result) override;
    };

}}}}}

#endif // _WIN32_WINNT_WIN10
