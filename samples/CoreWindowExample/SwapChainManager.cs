// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using System;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.UI.Core;

namespace CoreWindowExample
{
    class SwapChainManager
    {
        public CanvasSwapChain SwapChain { get; private set; }

        public SwapChainManager(CoreWindow window, CanvasDevice device)
        {
            float currentDpi = DisplayInformation.GetForCurrentView().LogicalDpi;
            SwapChain = CanvasSwapChain.CreateForCoreWindow(device, window, currentDpi);
        }

        public void EnsureMatchesWindow(CoreWindow window)
        {
            var bounds = window.Bounds;
            Size windowSize = new Size(bounds.Width, bounds.Height);
            float dpi = DisplayInformation.GetForCurrentView().LogicalDpi;

            if (!SizeEqualsWithTolerance(windowSize, SwapChain.Size) || dpi != SwapChain.Dpi)
            {
                // Note: swapchain size & window size may not be exactly equal since they are represented with
                // floating point numbers and are calculated via different code paths.
                SwapChain.ResizeBuffers((float)windowSize.Width, (float)windowSize.Height, dpi);
            }
        }

        static public bool SizeEqualsWithTolerance(Size sizeA, Size sizeB)
        {
            const float tolerance = 0.1f;

            if (Math.Abs(sizeA.Width - sizeB.Width) > tolerance)
                return false;

            if (Math.Abs(sizeA.Height - sizeB.Height) > tolerance)
                return false;

            return true;
        }

    }
}
