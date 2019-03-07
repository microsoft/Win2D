// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Text;
using Win2D = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasSwapChainPanel : WindowsXamlHostBase
    {
        internal Win2D.UI.Xaml.CanvasSwapChainPanel UwpControl => ChildInternal as Win2D.UI.Xaml.CanvasSwapChainPanel;

        public CanvasSwapChainPanel()
            : base(typeof(Win2D.UI.Xaml.CanvasSwapChainPanel).FullName)
        { }

        public Win2D.CanvasSwapChain SwapChain
        {
            get => UwpControl.SwapChain;
            set => UwpControl.SwapChain = value;
        }

        protected override void Dispose(bool disposing)
        {
            UwpControl.RemoveFromVisualTree();

            base.Dispose(disposing);
        }

    }
}
