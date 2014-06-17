// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <CanvasBrush.abi.h>

#include "ClosablePtr.h"
#include "ResourceManager.h"

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class CanvasSolidColorBrush;
    class CanvasSolidColorBrushManager;

    [uuid(3A6BF1D2-731A-4EBB-AA40-1419A89302F6)]
    class ICanvasBrushInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Brush> GetD2DBrush() = 0;
    };

    [uuid(8FE46BCD-8594-44F4-AAB2-16E192BDC05F)]
    class ICanvasSolidColorBrushInternal : public ICanvasBrushInternal
    {
    public:
        virtual ComPtr<ID2D1SolidColorBrush> GetD2DSolidColorBrush() = 0;
    };

    class CanvasSolidColorBrushFactory : public ActivationFactory<
        ICanvasSolidColorBrushFactory,
        CloakedIid<ICanvasFactoryNative>>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

        std::shared_ptr<CanvasSolidColorBrushManager> m_manager; // TODO: #1442 - see CanvasDevice.h

    public:
        CanvasSolidColorBrushFactory();

        //
        // ICanvasSolidColorBrushFactory
        //

        IFACEMETHOD(Create)(
            ICanvasDevice* canvasDevice,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;

        //
        // ICanvasFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;
    };

    struct CanvasSolidColorBrushTraits
    {
        typedef ID2D1SolidColorBrush resource_t;
        typedef CanvasSolidColorBrush wrapper_t;
        typedef ICanvasSolidColorBrush wrapper_interface_t;
        typedef CanvasSolidColorBrushManager manager_t;
    };

    class CanvasSolidColorBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasSolidColorBrushTraits, 
        ICanvasBrush, 
        ChainInterfaces<CloakedIid<ICanvasSolidColorBrushInternal>, CloakedIid<ICanvasBrushInternal>>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

    public:
        CanvasSolidColorBrush(
            std::shared_ptr<CanvasSolidColorBrushManager> manager,
            ID2D1SolidColorBrush* brush);

        IFACEMETHOD(get_Color)(_Out_ ABI::Windows::UI::Color *value) override;

        IFACEMETHOD(put_Color)(_In_ ABI::Windows::UI::Color value) override;

        IFACEMETHOD(get_Opacity)(_Out_ float *value) override;

        IFACEMETHOD(put_Opacity)(_In_ float value) override;

        IFACEMETHOD(get_Transform)(_Out_ Math::Matrix3x2 *value) override;

        IFACEMETHOD(put_Transform)(_In_ Math::Matrix3x2 value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        virtual ComPtr<ID2D1Brush> GetD2DBrush() override;

        // ICanvasSolidColorBrushInternal
        virtual ComPtr<ID2D1SolidColorBrush> GetD2DSolidColorBrush() override;
    };


    class CanvasSolidColorBrushManager : public ResourceManager<CanvasSolidColorBrushTraits>
    {
    public:
        ComPtr<CanvasSolidColorBrush> CreateNew(
            ICanvasDevice* canvasDevice,
            ABI::Windows::UI::Color color);

        ComPtr<CanvasSolidColorBrush> CreateWrapper(
            ID2D1SolidColorBrush* resource);
    };
}
