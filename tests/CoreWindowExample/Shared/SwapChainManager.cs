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

            if (dpi != SwapChain.Dpi)
            {
                // If the DPI changes then we need to recreate the swapchain 
                // TODO: #3905 should ResizeBuffers change the DPI as well as the size?
                var device = SwapChain.Device;
                SwapChain.Dispose();
                SwapChain = CanvasSwapChain.CreateForCoreWindow(device, window, dpi);
            }
            else if (!SizeEqualsWithTolerance(windowSize, SwapChain.Size))
            {
                // Note: swapchain size & window size may not be exactly equal since they are represented with
                // floating point numbers and are calculated via different code paths.
#if WINDOWS_PHONE_APP
                // Windows Phone does not support ResizeBuffers for CoreWindow swap chains
                //
                var device = SwapChain.Device;
                SwapChain.Dispose();
                SwapChain = CanvasSwapChain.CreateForCoreWindow(device, window, dpi);
#else
                SwapChain.ResizeBuffers((float)windowSize.Width, (float)windowSize.Height);
#endif
            }

#if WINDOWS_PHONE_APP
            var orientation = DisplayInformation.GetForCurrentView().CurrentOrientation;
            CanvasSwapChainRotation rotation = CanvasSwapChainRotation.None;

            switch (orientation)
            {
                case DisplayOrientations.Portrait:
                    rotation = CanvasSwapChainRotation.None;
                    break;

                case DisplayOrientations.Landscape:
                    rotation = CanvasSwapChainRotation.Rotate270;
                    break;

                case DisplayOrientations.LandscapeFlipped:
                    rotation = CanvasSwapChainRotation.Rotate90;
                    break;

                case DisplayOrientations.PortraitFlipped:
                    rotation = CanvasSwapChainRotation.Rotate180;
                    break;
            }

            if (SwapChain.Rotation != rotation)
                SwapChain.Rotation = rotation;
#endif
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
