// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;
using Windows.Foundation;
using Win2d = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasControl : WindowsXamlHostBase
    {
        public event TypedEventHandler<Win2d.UI.Xaml.CanvasControl, Win2d.UI.CanvasCreateResourcesEventArgs> CreateResources
        {
            add
            {
                UwpControl.CreateResources += value;
            }
            remove
            {
                UwpControl.CreateResources -= value;
            }
        }
        public event TypedEventHandler<Win2d.UI.Xaml.CanvasControl, Win2d.UI.Xaml.CanvasDrawEventArgs> Draw
        {
            add
            {
                UwpControl.Draw += value;
            }
            remove
            {
                UwpControl.Draw -= value;
            }
        }

        internal Win2d.UI.Xaml.CanvasControl UwpControl => ChildInternal as Win2d.UI.Xaml.CanvasControl;

        public CanvasControl()
            : base(typeof(Win2d.UI.Xaml.CanvasControl).FullName)
        {
            UwpControl.SetVisible(true);
        }

        protected override void Dispose(bool disposing)
        {
            UwpControl.RemoveFromVisualTree();

            base.Dispose(disposing);
        }

    }
}
