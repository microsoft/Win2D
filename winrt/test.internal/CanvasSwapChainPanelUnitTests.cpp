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
#include "StubSwapChainPanel.h"
#include "MockCanvasSwapChain.h"
#include "MockDXGISwapChain.h"

TEST_CLASS(CanvasSwapChainPanelUnitTests)
{
    class TestCanvasSwapChainPanelAdapter : public ICanvasSwapChainPanelAdapter
    {
    public:
        ComPtr<StubSwapChainPanel> m_stubSwapChainPanel;

        TestCanvasSwapChainPanelAdapter()
            : m_stubSwapChainPanel(Make<StubSwapChainPanel>())
        {
        }

        virtual std::pair<ComPtr<IInspectable>, ComPtr<ISwapChainPanel>> CreateSwapChainPanel(IInspectable* canvasSwapChain) override
        {
            ComPtr<IInspectable> inspectableControl;
            ThrowIfFailed(m_stubSwapChainPanel.As(&inspectableControl));

            return std::pair<ComPtr<IInspectable>, ComPtr<ISwapChainPanel>>(inspectableControl, m_stubSwapChainPanel);
        }
    };

    struct Fixture
    {
        std::shared_ptr<TestCanvasSwapChainPanelAdapter> m_adapter;

        ComPtr<CanvasSwapChainPanel> m_canvasSwapChainPanel;

        Fixture()
        {
            m_adapter = std::make_shared<TestCanvasSwapChainPanelAdapter>();

            m_canvasSwapChainPanel = Make<CanvasSwapChainPanel>(m_adapter);
        }
    };

    TEST_METHOD_EX(CanvasSwapChainPanel_Implements_Expected_Interfaces)
    {
        Fixture f;

        ASSERT_IMPLEMENTS_INTERFACE(f.m_canvasSwapChainPanel, ICanvasSwapChainPanel);
        ASSERT_IMPLEMENTS_INTERFACE(f.m_canvasSwapChainPanel, ISwapChainPanel);
        ASSERT_IMPLEMENTS_INTERFACE(f.m_canvasSwapChainPanel, ISwapChainPanelNative);
    }

    TEST_METHOD_EX(CanvasSwapChainPanel_put_SwapChain)
    {
        Fixture f;

        auto swapChain = Make<MockCanvasSwapChain>();

        auto dxgiResource = Make<MockDxgiSwapChain>();

        //
        // Verify that no swap chain is assigned upon initialization.
        // The fact that mocks have no expected calls is also implicitly
        // verifying this.
        //
        ComPtr<ICanvasSwapChain> retrievedSwapChain;
        f.m_canvasSwapChainPanel->get_SwapChain(&retrievedSwapChain);
        Assert::IsNull(retrievedSwapChain.Get());

        //
        // Verify that put_SwapChain with a non-null CanvasSwapChain
        // associates the correct resource.
        //

        swapChain->GetResourceMethod.SetExpectedCalls(1,  
            [dxgiResource](const IID& iid, void** out)
            {
                Assert::AreEqual(__uuidof(IDXGISwapChain2), iid);

                return dxgiResource.CopyTo(reinterpret_cast<IDXGISwapChain2**>(out));
            });

        const float testDpi = 144;

        swapChain->GetDpiMethod.SetExpectedCalls(1,
            [&](float* out)
            {
                *out = testDpi;
                return S_OK;
            });

        dxgiResource->SetMatrixTransformMethod.SetExpectedCalls(1,
            [&](const DXGI_MATRIX_3X2_F* value)
            {
                const float expectedDpiScale = DEFAULT_DPI / testDpi;

                Assert::AreEqual(expectedDpiScale, value->_11, FLT_EPSILON);
                Assert::AreEqual(0.0f, value->_12);

                Assert::AreEqual(0.0f, value->_21);
                Assert::AreEqual(expectedDpiScale, value->_22, FLT_EPSILON);
            
                Assert::AreEqual(0.0f, value->_31);
                Assert::AreEqual(0.0f, value->_32);

                return S_OK;
            });

        f.m_adapter->m_stubSwapChainPanel->SetSwapChainMethod.SetExpectedCalls(1,
            [dxgiResource](IDXGISwapChain* swapChain)
            {
                Assert::AreEqual(static_cast<IDXGISwapChain*>(dxgiResource.Get()), swapChain);

                return S_OK;
            });

        ThrowIfFailed(f.m_canvasSwapChainPanel->put_SwapChain(swapChain.Get()));

        // Verify that get_SwapChain returns the expected value.
        ThrowIfFailed(f.m_canvasSwapChainPanel->get_SwapChain(&retrievedSwapChain));
        Assert::AreEqual(static_cast<ICanvasSwapChain*>(swapChain.Get()), retrievedSwapChain.Get());

        // Verify that put_SwapChain with null clears the association.
        swapChain->GetResourceMethod.SetExpectedCalls(0);
        f.m_adapter->m_stubSwapChainPanel->SetSwapChainMethod.SetExpectedCalls(1,
            [](IDXGISwapChain* swapChain)
            {
                Assert::IsNull(swapChain);
                return S_OK;
            });

        ThrowIfFailed(f.m_canvasSwapChainPanel->put_SwapChain(nullptr));

        // Verify that get_SwapChain returns the expected value.
        ThrowIfFailed(f.m_canvasSwapChainPanel->get_SwapChain(&retrievedSwapChain));
        Assert::IsNull(retrievedSwapChain.Get());
    }
};