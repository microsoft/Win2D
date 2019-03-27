using Microsoft.Toolkit.Wpf.UI.XamlHost;
using System;
using System.Collections.Generic;
using System.Text;
using Windows.Foundation;
using Win2D = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    public sealed class CanvasAnimatedControl : WindowsXamlHostBase
    {
        public event TypedEventHandler<Win2D.UI.Xaml.CanvasAnimatedControl, Win2D.UI.CanvasCreateResourcesEventArgs> CreateResources
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
        public event TypedEventHandler<Win2D.UI.Xaml.ICanvasAnimatedControl, Win2D.UI.Xaml.CanvasAnimatedDrawEventArgs> Draw
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
        public event TypedEventHandler<Win2D.UI.Xaml.ICanvasAnimatedControl, object> GameLoopStarting
        {
            add
            {
                UwpControl.GameLoopStarting += value;
            }
            remove
            {
                UwpControl.GameLoopStarting -= value;
            }
        }
        public event TypedEventHandler<Win2D.UI.Xaml.ICanvasAnimatedControl, object> GameLoopStopped
        {
            add
            {
                UwpControl.GameLoopStopped += value;
            }
            remove
            {
                UwpControl.GameLoopStopped -= value;
            }
        }
        public event TypedEventHandler<Win2D.UI.Xaml.ICanvasAnimatedControl, Win2D.UI.Xaml.CanvasAnimatedUpdateEventArgs> Update
        {
            add
            {
                UwpControl.Update += value;
            }
            remove
            {
                UwpControl.Update -= value;
            }
        }

        internal Win2D.UI.Xaml.CanvasAnimatedControl UwpControl => ChildInternal as Win2D.UI.Xaml.CanvasAnimatedControl;

        public CanvasAnimatedControl()
            : base(typeof(Win2D.UI.Xaml.CanvasAnimatedControl).FullName)
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
