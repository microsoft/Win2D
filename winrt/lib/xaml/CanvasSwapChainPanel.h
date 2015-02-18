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
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::UI::Xaml::Controls;

    class ICanvasSwapChainPanelAdapter
    {
    public:
        virtual ~ICanvasSwapChainPanelAdapter() = default;

        virtual ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel) = 0;
    };
    
    class CanvasSwapChainPanelAdapter : public ICanvasSwapChainPanelAdapter,
                                        private LifespanTracker<CanvasSwapChainPanelAdapter>
    {
        ComPtr<ISwapChainPanelFactory> m_swapChainPanelFactory;

    public:
        CanvasSwapChainPanelAdapter();

        virtual ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChain) override;
    };

    class CanvasSwapChainPanel : public RuntimeClass<
        ICanvasSwapChainPanel,
        ComposableBase<>>,
        private LifespanTracker<CanvasSwapChainPanel>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChainPanel, BaseTrust);

        ComPtr<ICanvasSwapChain> m_canvasSwapChain;

        std::shared_ptr<ICanvasSwapChainPanelAdapter> m_adapter;

    public:
        CanvasSwapChainPanel(std::shared_ptr<ICanvasSwapChainPanelAdapter> adapter);

        // ICanvasSwapChainPanel

        IFACEMETHOD(get_SwapChain)(ICanvasSwapChain** value) override;

        IFACEMETHOD(put_SwapChain)(ICanvasSwapChain* value) override;

    };

}}}}
