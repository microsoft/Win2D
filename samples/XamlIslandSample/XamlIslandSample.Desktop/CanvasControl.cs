// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;
using Windows.Foundation;
using Win2D = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasControl : WindowsXamlHostBase
    {
        public event TypedEventHandler<Win2D.UI.Xaml.CanvasControl, Win2D.UI.CanvasCreateResourcesEventArgs> CreateResources
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
        public event TypedEventHandler<Win2D.UI.Xaml.CanvasControl, Win2D.UI.Xaml.CanvasDrawEventArgs> Draw
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

        internal Win2D.UI.Xaml.CanvasControl UwpControl => ChildInternal as Win2D.UI.Xaml.CanvasControl;

        public CanvasControl()
            : base(typeof(Win2D.UI.Xaml.CanvasControl).FullName)
        {
            UwpControl.VisibilityMode = Win2D.UI.Xaml.CanvasVisibilityMode.ForceVisible;
        }

        protected override void Dispose(bool disposing)
        {
            UwpControl.RemoveFromVisualTree();

            base.Dispose(disposing);
        }

    }
}
