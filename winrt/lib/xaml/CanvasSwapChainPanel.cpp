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

#include "pch.h"

#include "CanvasSwapChainPanel.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ::Microsoft::WRL::Wrappers;

    CanvasSwapChainPanelAdapter::CanvasSwapChainPanelAdapter()
    {
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_SwapChainPanel).Get(),
            &m_swapChainPanelFactory));
    }

    ComPtr<IInspectable> CanvasSwapChainPanelAdapter::CreateSwapChainPanel(IInspectable* canvasSwapChainPanel)
    {
        ComPtr<IInspectable> swapChainPanelInspectable;
        ComPtr<ISwapChainPanel> swapChainPanel;

        // Instantiates the SwapChainPanel XAML object.
        ThrowIfFailed(m_swapChainPanelFactory->CreateInstance(
            canvasSwapChainPanel,
            &swapChainPanelInspectable,
            &swapChainPanel));

        return swapChainPanelInspectable;
    }

    class CanvasSwapChainPanelFactory : public ActivationFactory<>,
                                        private LifespanTracker<CanvasSwapChainPanelFactory>
    {
        //
        // While CanvasSwapChainPanel publically has only the default 
        // constructor, its implementing class has only a non-default
        // constructor, and so this activation factory is used to
        // pass in the adapter.
        //
        std::weak_ptr<CanvasSwapChainPanelAdapter> m_adapter;

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable** obj) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(obj);

                    auto adapter = m_adapter.lock();

                    if (!adapter)
                        m_adapter = adapter = std::make_shared<CanvasSwapChainPanelAdapter>();

                    auto swapChainPanel = Make<CanvasSwapChainPanel>(adapter);
                    CheckMakeResult(swapChainPanel);

                    ThrowIfFailed(swapChainPanel.CopyTo(obj));
                });
        }
    };

    CanvasSwapChainPanel::CanvasSwapChainPanel(std::shared_ptr<ICanvasSwapChainPanelAdapter> adapter)
        : m_adapter(adapter)
    {
        auto base = m_adapter->CreateSwapChainPanel(static_cast<ICanvasSwapChainPanel*>(this));
        ThrowIfFailed(SetComposableBasePointers(base.Get(), nullptr));
    }

    STDMETHODIMP CanvasSwapChainPanel::get_SwapChain(ICanvasSwapChain** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);

                ThrowIfFailed(m_canvasSwapChain.CopyTo(value));
            });
    }

    STDMETHODIMP CanvasSwapChainPanel::put_SwapChain(ICanvasSwapChain* value)
    {
        return ExceptionBoundary(
            [&]
            {
                ComPtr<ISwapChainPanelNative> swapChainPanelNative = As<ISwapChainPanelNative>(this);

                ComPtr<IDXGISwapChain2> dxgiSwapChain;

                if (value)
                {
                    auto swapChainResourceWrapper = As<ICanvasResourceWrapperNative>(value);

                    ThrowIfFailed(swapChainResourceWrapper->GetResource(IID_PPV_ARGS(&dxgiSwapChain)));
                }

                ThrowIfFailed(swapChainPanelNative->SetSwapChain(dxgiSwapChain.Get()));

                m_canvasSwapChain = value;
            });
    }

    ActivatableClassWithFactory(CanvasSwapChainPanel, CanvasSwapChainPanelFactory);
}}}}