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

#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;

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

    struct CanvasSolidColorBrushTraits
    {
        typedef ID2D1SolidColorBrush resource_t;
        typedef CanvasSolidColorBrush wrapper_t;
        typedef ICanvasSolidColorBrush wrapper_interface_t;
        typedef CanvasSolidColorBrushManager manager_t;
    };

    class CanvasBrush
        : public Implements<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasBrush, 
            CloakedIid<ICanvasBrushInternal>>
    {
    public:
        IFACEMETHOD(get_Opacity)(_Out_ float *value) override;

        IFACEMETHOD(put_Opacity)(_In_ float value) override;

        IFACEMETHOD(get_Transform)(_Out_ Numerics::Matrix3x2 *value) override;

        IFACEMETHOD(put_Transform)(_In_ Numerics::Matrix3x2 value) override;
    };

    class CanvasSolidColorBrush : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasSolidColorBrushTraits, 
        ChainInterfaces<CloakedIid<ICanvasSolidColorBrushInternal>, CloakedIid<ICanvasBrushInternal>>,
        MixIn<CanvasSolidColorBrush, CanvasBrush>),
        public CanvasBrush
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

    public:
        CanvasSolidColorBrush(
            std::shared_ptr<CanvasSolidColorBrushManager> manager,
            ID2D1SolidColorBrush* brush);

        IFACEMETHOD(get_Color)(_Out_ ABI::Windows::UI::Color *value) override;

        IFACEMETHOD(put_Color)(_In_ ABI::Windows::UI::Color value) override;

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
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color);

        ComPtr<CanvasSolidColorBrush> CreateWrapper(
            ID2D1SolidColorBrush* resource);
    };

    class CanvasSolidColorBrushFactory 
        : public ActivationFactory<
            ICanvasSolidColorBrushFactory,
            CloakedIid<ICanvasFactoryNative>>,
          public PerApplicationManager<CanvasSolidColorBrushFactory, CanvasSolidColorBrushManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSolidColorBrush, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASFACTORYNATIVE();

        //
        // ICanvasSolidColorBrushFactory
        //

        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::UI::Color color,
            ICanvasSolidColorBrush** canvasSolidColorBrush) override;

    };
}}}}
