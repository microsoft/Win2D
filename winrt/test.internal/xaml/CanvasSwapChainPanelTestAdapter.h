// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs/StubSwapChainPanel.h"

class CanvasSwapChainPanelTestAdapter : public ICanvasSwapChainPanelAdapter
{
public:
    ComPtr<StubSwapChainPanel> m_stubSwapChainPanel;

    CanvasSwapChainPanelTestAdapter()
        : m_stubSwapChainPanel(Make<StubSwapChainPanel>())
    {
    }

    CanvasSwapChainPanelTestAdapter(ComPtr<StubSwapChainPanel> const& swapChainPanel)
        : m_stubSwapChainPanel(swapChainPanel)
    {
    }

    virtual ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChain) override
    {
        return As<IInspectable>(m_stubSwapChainPanel);
    }
};
