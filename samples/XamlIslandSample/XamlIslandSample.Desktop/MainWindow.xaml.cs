// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using System;
using System.IO;
using System.Windows;
using Windows.UI;

namespace XamlIslandSample.Desktop
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // Test the deployment
            // This only works when the .deps.json file is published
            if (string.IsNullOrEmpty(AppDomain.CurrentDomain.GetData("FX_PRODUCT_VERSION") as string))
            {
                MessageBox.Show("No .deps.json file found.");
            }
        }

        private void CanvasSwapChainPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var oldSwapChain = canvasSwapChainPanel.SwapChain;

            var width = (float)e.NewSize.Width;
            var height = (float)e.NewSize.Height;
            var swapChain = new CanvasSwapChain(new CanvasDevice(), width, height, 96);

            canvasSwapChainPanel.SwapChain = swapChain;
            oldSwapChain?.Dispose();

            using (CanvasDrawingSession ds = swapChain.CreateDrawingSession(Colors.White))
                ds.DrawCircle(width / 2, height / 2, Math.Min(width, height) / 4, Colors.Red, 20);

            swapChain.Present();
        }

        private void CanvasControl_CreateResources(Microsoft.Graphics.Canvas.UI.Xaml.CanvasControl sender, Microsoft.Graphics.Canvas.UI.CanvasCreateResourcesEventArgs args)
        { }

        private void CanvasControl_Draw(Microsoft.Graphics.Canvas.UI.Xaml.CanvasControl sender, Microsoft.Graphics.Canvas.UI.Xaml.CanvasDrawEventArgs e)
        {
            var width = (float)canvasControl.ActualWidth;
            var height = (float)canvasControl.ActualHeight;
            e.DrawingSession.FillEllipse(width / 2, height / 2, width / 2 - 20, height / 2 - 20, Colors.LawnGreen);
        }

    }
}
