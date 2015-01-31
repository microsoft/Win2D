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

#include <CanvasSwapChainPanel.h>

#include "MockCanvasSwapChain.h"
#include "MockDXGISwapChain.h"
#include "CanvasSwapChainPanelTestAdapter.h"

TEST_CLASS(CanvasSwapChainPanelUnitTests)
{
    struct Fixture
    {
        std::shared_ptr<CanvasSwapChainPanelTestAdapter> Adapter;

        ComPtr<CanvasSwapChainPanel> SwapChainPanel;

        std::shared_ptr<MockCanvasSwapChainManager> SwapChainManager;

        Fixture()
            : Adapter(std::make_shared<CanvasSwapChainPanelTestAdapter>())
            , SwapChainPanel(Make<CanvasSwapChainPanel>(Adapter))
            , SwapChainManager(std::make_shared<MockCanvasSwapChainManager>())
        {
        }
    };

    TEST_METHOD_EX(CanvasSwapChainPanel_Implements_Expected_Interfaces)
    {
        Fixture f;

        ASSERT_IMPLEMENTS_INTERFACE(f.SwapChainPanel, ICanvasSwapChainPanel);
        ASSERT_IMPLEMENTS_INTERFACE(f.SwapChainPanel, ISwapChainPanel);
        ASSERT_IMPLEMENTS_INTERFACE(f.SwapChainPanel, ISwapChainPanelNative);
    }

    TEST_METHOD_EX(CanvasSwapChainPanel_put_SwapChain)
    {
        Fixture f;

        auto device = Make<StubCanvasDevice>();
        auto dxgiResource = Make<MockDxgiSwapChain>();
        
        auto swapChain = f.SwapChainManager->CreateMock(device.Get(), dxgiResource.Get());

        //
        // Verify that no swap chain is assigned upon initialization.
        // The fact that mocks have no expected calls is also implicitly
        // verifying this.
        //
        ComPtr<ICanvasSwapChain> retrievedSwapChain;
        f.SwapChainPanel->get_SwapChain(&retrievedSwapChain);
        Assert::IsNull(retrievedSwapChain.Get());

        //
        // Verify that put_SwapChain with a non-null CanvasSwapChain
        // associates the correct resource.
        //

        f.Adapter->m_stubSwapChainPanel->SetSwapChainMethod.SetExpectedCalls(1,
            [dxgiResource](IDXGISwapChain* swapChain)
            {
                Assert::AreEqual(static_cast<IDXGISwapChain*>(dxgiResource.Get()), swapChain);

                return S_OK;
            });

        ThrowIfFailed(f.SwapChainPanel->put_SwapChain(swapChain.Get()));

        // Verify that get_SwapChain returns the expected value.
        ThrowIfFailed(f.SwapChainPanel->get_SwapChain(&retrievedSwapChain));
        Assert::AreEqual(static_cast<ICanvasSwapChain*>(swapChain.Get()), retrievedSwapChain.Get());

        // Verify that put_SwapChain with null clears the association.
        f.Adapter->m_stubSwapChainPanel->SetSwapChainMethod.SetExpectedCalls(1,
            [](IDXGISwapChain* swapChain)
            {
                Assert::IsNull(swapChain);
                return S_OK;
            });

        ThrowIfFailed(f.SwapChainPanel->put_SwapChain(nullptr));

        // Verify that get_SwapChain returns the expected value.
        ThrowIfFailed(f.SwapChainPanel->get_SwapChain(&retrievedSwapChain));
        Assert::IsNull(retrievedSwapChain.Get());
    }
};