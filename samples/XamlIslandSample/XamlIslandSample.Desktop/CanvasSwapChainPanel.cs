// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Text;
using Win2d = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasSwapChainPanel : WindowsXamlHostBase
    {
        internal Win2d.UI.Xaml.CanvasSwapChainPanel UwpControl => ChildInternal as Win2d.UI.Xaml.CanvasSwapChainPanel;

        public CanvasSwapChainPanel()
            : base(typeof(Win2d.UI.Xaml.CanvasSwapChainPanel).FullName)
        { }

        protected override void Dispose(bool disposing)
        {
            UwpControl.RemoveFromVisualTree();

            base.Dispose(disposing);
        }

        public Win2d.CanvasSwapChain SwapChain
        {
            get => UwpControl.SwapChain;
            set => UwpControl.SwapChain = value;
        }

    }
}
