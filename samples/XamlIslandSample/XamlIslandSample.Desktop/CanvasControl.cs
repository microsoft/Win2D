// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;
using Win2d = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasControl : WindowsXamlHostBase
    {
        public event EventHandler<Win2d.UI.CanvasCreateResourcesEventArgs> CreateResources;
        public event EventHandler<Win2d.UI.Xaml.CanvasDrawEventArgs> Draw;

        internal Win2d.UI.Xaml.CanvasControl UwpControl => ChildInternal as Win2d.UI.Xaml.CanvasControl;

        public CanvasControl()
            : base(typeof(Win2d.UI.Xaml.CanvasControl).FullName)
        { }

        protected override void OnInitialized(EventArgs e)
        {
            UwpControl.CreateResources += OnCreateResources;
            UwpControl.Draw += OnDraw;

            base.OnInitialized(e);
        }

        protected override void Dispose(bool disposing)
        {
            UwpControl.CreateResources -= OnCreateResources;
            UwpControl.Draw -= OnDraw;
            UwpControl.RemoveFromVisualTree();

            base.Dispose(disposing);
        }


        private void OnCreateResources(Win2d.UI.Xaml.CanvasControl sender, Win2d.UI.CanvasCreateResourcesEventArgs args)
        {
            CreateResources?.Invoke(this, args);
        }


        private void OnDraw(Win2d.UI.Xaml.CanvasControl sender, Win2d.UI.Xaml.CanvasDrawEventArgs args)
        {
            Draw?.Invoke(this, args);
        }
    }
}
