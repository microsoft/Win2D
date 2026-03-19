// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "CanvasAnimatedControl.h"
#include "CanvasGameLoop.h"
#include "GameLoopThread.h"

using namespace ::ABI::Microsoft::Graphics::Canvas;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ::Microsoft::WRL::Wrappers;

//
// CanvasAnimatedControlAdapter
//

class CanvasAnimatedControlAdapter : public BaseControlAdapter<CanvasAnimatedControlTraits>
{
    ComPtr<ICanvasSwapChainFactory> m_canvasSwapChainFactory;
    std::shared_ptr<CanvasSwapChainPanelAdapter> m_canvasSwapChainPanelAdapter;
    ComPtr<IActivationFactory> m_canvasSwapChainPanelActivationFactory;

public:
    CanvasAnimatedControlAdapter()
        : m_canvasSwapChainPanelAdapter(std::make_shared<CanvasSwapChainPanelAdapter>())        
    {
        auto& module = Module<InProc>::GetModule();

        ComPtr<IActivationFactory> swapChainActivationFactory;

        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain).Get(),
            &swapChainActivationFactory));

        m_canvasSwapChainFactory = As<ICanvasSwapChainFactory>(swapChainActivationFactory);

        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasSwapChainPanel).Get(),
            &m_canvasSwapChainPanelActivationFactory));
    }        

    virtual ComPtr<CanvasSwapChainPanel> CreateCanvasSwapChainPanel() override
    {
        ComPtr<IInspectable> canvasSwapChainPanelInspectable;
        ThrowIfFailed(m_canvasSwapChainPanelActivationFactory->ActivateInstance(&canvasSwapChainPanelInspectable));

        auto canvasSwapChainPanel = As<ICanvasSwapChainPanel>(canvasSwapChainPanelInspectable);

        return static_cast<CanvasSwapChainPanel*>(canvasSwapChainPanel.Get());
    }

    virtual ComPtr<CanvasSwapChain> CreateCanvasSwapChain(
        ICanvasDevice* device,
        float width,
        float height,
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        ComPtr<ICanvasSwapChain> swapChain;

        ThrowIfFailed(m_canvasSwapChainFactory->CreateWithAllOptions(
            As<ICanvasResourceCreator>(device).Get(),
            width, 
            height, 
            dpi,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            2, 
            alphaMode,
            &swapChain));

        return static_cast<CanvasSwapChain*>(swapChain.Get());
    }

    virtual ComPtr<IShape> CreateDesignModeShape() override
    {
        ComPtr<IActivationFactory> rectangleFactory;

        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_UI_Xaml_Shapes_Rectangle).Get(),
            &rectangleFactory));
        
        ComPtr<IInspectable> rectangleInspectable;
        ThrowIfFailed(rectangleFactory->ActivateInstance(&rectangleInspectable));

        ComPtr<IActivationFactory> brushFactory;
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_UI_Xaml_Media_SolidColorBrush).Get(),
            &brushFactory));

        ComPtr<IInspectable> brushInspectable;
        ThrowIfFailed(brushFactory->ActivateInstance(&brushInspectable));

        ThrowIfFailed(As<IShape>(rectangleInspectable)->put_Fill(As<IBrush>(brushInspectable).Get()));

        return As<IShape>(rectangleInspectable);
    }

    virtual std::unique_ptr<CanvasGameLoop> CreateAndStartGameLoop(CanvasAnimatedControl* control, ISwapChainPanel* swapChainPanel) override
    {
        //
        // This needs to start a new thread and, while executing code on that
        // thread, then get a CoreDispatcher set up on that thread, and then, on
        // the original thread, create a CanvasGameLoop that has access to that
        // dispatcher.
        //

        return std::make_unique<CanvasGameLoop>(control, CreateGameLoopThread(swapChainPanel, control));
    }

    virtual ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel) override
    {
        return m_canvasSwapChainPanelAdapter->CreateSwapChainPanel(canvasSwapChainPanel);
    }

    virtual void Sleep(DWORD timeInMs) override
    {
        ::Sleep(timeInMs);
    }

    virtual int64_t GetPerformanceCounter() override
    {
        LARGE_INTEGER counter;
        if (QueryPerformanceCounter(&counter) == 0)
        {
            ThrowHR(E_FAIL);
        }
        return counter.QuadPart;
    }

    virtual int64_t GetPerformanceFrequency() override
    {
        LARGE_INTEGER frequency;
        if (QueryPerformanceFrequency(&frequency) == 0)
        {
            ThrowHR(E_FAIL);
        }
        return frequency.QuadPart;
    }
};


namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter()
    {
        return std::make_shared<CanvasAnimatedControlAdapter>();
    }

} } } } } }
