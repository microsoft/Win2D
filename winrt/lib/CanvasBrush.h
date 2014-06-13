// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "CanvasBrush.abi.h"
#include "d2dwinrt.h"
#include <ClosablePtr.h>

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    [uuid(3A6BF1D2-731A-4EBB-AA40-1419A89302F6)]
    class ICanvasBrushInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Brush> GetD2DBrush() = 0;
    };

    //
    // This allows tests to replace a CanvasSolidColorBrushes's m_d2dSolidColorBrush with
    // a type of their choosing.
    //
    class ICanvasSolidColorBrushResourceCreationAdapter
    {
    public:
        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(
            ID2D1DeviceContext* deviceContext, 
            ABI::Windows::UI::Color color
            ) = 0;
    };
    
    class CanvasSolidColorBrushFactory : public ActivationFactory<ICanvasSolidColorBrushFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

        std::shared_ptr<ICanvasSolidColorBrushResourceCreationAdapter> m_resourceCreationAdapter;

    public:

        CanvasSolidColorBrushFactory();

        IFACEMETHOD(Create)(
            ICanvasDevice* canvasDevice,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush **canvasSolidColorBrush) override;
    };

    class CanvasSolidColorBrush : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasSolidColorBrush,
        ICanvasBrush,
        ABI::Windows::Foundation::IClosable,
        CloakedIid<ICanvasBrushInternal >>
    {

        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

        ClosablePtr<ID2D1SolidColorBrush> m_d2dSolidColorBrush;

    public:

        CanvasSolidColorBrush(
            ICanvasDevice* canvasDevice,
            ABI::Windows::UI::Color color,
            std::shared_ptr<ICanvasSolidColorBrushResourceCreationAdapter> resourceCreationAdapter
            );

        IFACEMETHOD(get_Color)(_Out_ ABI::Windows::UI::Color *value) override;

        IFACEMETHOD(put_Color)(_In_ ABI::Windows::UI::Color value) override;

        IFACEMETHOD(get_Opacity)(_Out_ float *value) override;

        IFACEMETHOD(put_Opacity)(_In_ float value) override;

        IFACEMETHOD(get_Transform)(_Out_ Math::Matrix3x2 *value) override;

        IFACEMETHOD(put_Transform)(_In_ Math::Matrix3x2 value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasBrushInternal
        ComPtr<ID2D1Brush> GetD2DBrush() override;
    };
}
