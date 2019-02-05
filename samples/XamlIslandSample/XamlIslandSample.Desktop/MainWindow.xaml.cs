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
        }

        private void ButtonExit_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
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
    }
}
