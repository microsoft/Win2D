// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas.UI.Xaml;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SimpleSample
{
    /// <summary>
    /// Draws some graphics using Win2D
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;
        }

        void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            args.DrawingSession.DrawEllipse(155, 115, 80, 30, Colors.Black, 3);
            args.DrawingSession.DrawText("Hello, world!", 100, 100, Colors.Yellow);
        }
    }
}
