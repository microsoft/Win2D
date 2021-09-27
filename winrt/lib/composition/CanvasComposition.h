// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Composition
{
    using namespace ABI::Microsoft::UI::Composition;

    class CanvasCompositionStatics : public AgileActivationFactory<ICanvasCompositionStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Composition_CanvasComposition, BaseTrust);

    public:
        HRESULT RuntimeClassInitialize();
        
        IFACEMETHODIMP CreateCompositionGraphicsDevice(
            ICompositor* compositor,
            ICanvasDevice* canvasDevice,
            ICompositionGraphicsDevice** graphicsDevice) override;

#if ENABLE_WIN2D_EXPERIMENTAL_FEATURES
        IFACEMETHODIMP CreateCompositionSurfaceForSwapChain( 
            ICompositor* compositor,
            ICanvasSwapChain* swapChain,
            ICompositionSurface** compositionSurface) override;
#endif
        
        IFACEMETHODIMP GetCanvasDevice( 
            ICompositionGraphicsDevice* graphicsDevice,
            ICanvasDevice** canvasDevice) override;
        
        IFACEMETHODIMP SetCanvasDevice( 
            ICompositionGraphicsDevice* graphicsDevice,
            ICanvasDevice* canvasDevice) override;
        
        IFACEMETHODIMP CreateDrawingSession( 
            ICompositionDrawingSurface* drawingSurface,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHODIMP CreateDrawingSessionWithUpdateRect( 
            ICompositionDrawingSurface* drawingSurface,
            Rect updateRect,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHODIMP CreateDrawingSessionWithUpdateRectAndDpi(
            ICompositionDrawingSurface* drawingSurface,
            Rect updateRectInPixels,
            float dpi,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHODIMP Resize( 
            ICompositionDrawingSurface* drawingSurface,
            Size size) override;

    private:
        HRESULT CreateDrawingSessionImpl(ICompositionDrawingSurface* drawingSurface, RECT const* rect, float dpi, ICanvasDrawingSession** drawingSession);
    };

} } } } } }

#endif