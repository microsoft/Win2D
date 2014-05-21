using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI;

#if IMPL
using Microsoft.DirectX;
#endif

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace CsConsumer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// 

    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

#if IMPL
            // Initialization.
#if DEBUG
            CanvasFactory.SetDefaultOptions(CanvasDebugLevel.Information); // On CHK builds, enable the debug layer.
#endif
            CanvasDevice device = new CanvasDevice();
            CanvasDeviceContext m_deviceContext = new CanvasDeviceContext(device);

            // Create a Canvas image source and assign it to the Canvas XAML control as a background brush.
            DirectXImageSource m_surfaceImageSource = new DirectXImageSource(device, m_canvas.Width, m_canvas.Height);
            m_imageBrush = new ImageBrush();
            m_imageBrush.ImageSource = m_surfaceImageSource;
            m_canvas.Background = m_imageBrush;

            // Declare a red solid color brush.
            CanvasSolidColorBrush solidColorBrush = new CanvasSolidColorBrush(device, Color.FromArgb(0, 1, 1, 1));
            
            const int cellDim = 200;

            // Strokestyle
            CanvasStrokeStyleProperties strokeStyleProperties = new CanvasStrokeStyleProperties();
            strokeStyleProperties.DashStyle = CanvasDashStyle.Dash;
            strokeStyleProperties.DashCap = CanvasCapStyle.Round;
            CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle(device, strokeStyleProperties);

            // Create a freeform geometry.
            CanvasPathGeometry pathGeometry = new CanvasPathGeometry(device);
            using (PathGeometryCreator pgc = new PathGeometryCreator(pathGeometry))
            {
                pgc.BeginFigure(new Point(cellDim, cellDim), CanvasFigureBegin.Filled);
                pgc.AddLine(new Point(cellDim * 2, cellDim + 10));
                pgc.AddLine(new Point(cellDim, cellDim + 20));
                pgc.AddLine(new Point(cellDim * 2, cellDim + 50));
                pgc.AddLine(new Point(cellDim, cellDim + 100));
                pgc.AddLine(new Point(cellDim * 2, cellDim * 2));
                pgc.EndFigure(CanvasFigureEnd.Closed);
            }
            // Turn it into a mesh.
            CanvasMesh mesh = new CanvasMesh(device, pathGeometry);

            // Draw a white X to an intermediate.
            CanvasBitmap whiteXBitmap = new CanvasBitmap(device, CanvasBitmapOptions.Target, cellDim, cellDim);
            using (CanvasDrawingSession ds = whiteXBitmap.CreateDrawingSession())
            {
                ds.SetAntialiasMode(CanvasAntialiasMode.PerPrimitive);
                solidColorBrush.Color = Colors.White; 
                ds.DrawLine(new Point(30, 30), new Point(cellDim - 30, cellDim - 30), solidColorBrush, 10.0f);
                ds.DrawLine(new Point(cellDim - 30, 30), new Point(30, cellDim - 30), solidColorBrush, 10.0f);
            }

            CanvasBitmap leafBitmap = new CanvasBitmap(device, "leaf.png");

            // Drawing to the Canvas XAML control.
            using (CanvasDrawingSession ds = m_surfaceImageSource.CreateDrawingSession())
            {
                ds.SetTransform(CanvasMatrix3x2F.RotationInDegrees(5));

                ds.DrawBitmap(whiteXBitmap, new Point(0, 0));

                solidColorBrush.Color = Colors.Blue; 
                ds.DrawBitmap(leafBitmap, new Point(cellDim, 10));
                ds.DrawRectangle(new Windows.Foundation.Rect(cellDim + 20, 20, cellDim - 40, cellDim - 40), solidColorBrush, 11);
                
                // Aliased and antialiased ellipses
                ds.SetAntialiasMode(CanvasAntialiasMode.Aliased);
                solidColorBrush.Color = Colors.Red; 
                ds.FillEllipse(new Point(cellDim / 2 - 20, cellDim + (cellDim / 2) - 20), cellDim / 3, cellDim / 3, solidColorBrush);
                ds.SetAntialiasMode(CanvasAntialiasMode.PerPrimitive);

                // Make an ellipse geometry.
                CanvasEllipseGeometry ellipseGeometry = new CanvasEllipseGeometry(device, new CanvasEllipse(new Point(cellDim / 2 + 20, cellDim + (cellDim / 2) + 20), cellDim / 3, cellDim / 3));
                // Draw the tessellated ellipse.
                System.Collections.Generic.IReadOnlyList < CanvasTriangle > tessellation = ellipseGeometry.GetTessellation();
                solidColorBrush.Color = Colors.LightGreen;
                for (int i = 0; i < tessellation.Count; i++)
                {
                    float inc = 0;
                    ds.DrawLine(new Point(tessellation[i].Point1.X + inc, tessellation[i].Point1.Y - inc), new Point(tessellation[i].Point2.X + inc, tessellation[i].Point2.Y - inc), solidColorBrush);
                    ds.DrawLine(new Point(tessellation[i].Point2.X + inc, tessellation[i].Point2.Y - inc), new Point(tessellation[i].Point3.X + inc, tessellation[i].Point3.Y - inc), solidColorBrush);
                    ds.DrawLine(new Point(tessellation[i].Point3.X + inc, tessellation[i].Point3.Y - inc), new Point(tessellation[i].Point1.X + inc, tessellation[i].Point1.Y - inc), solidColorBrush);
                }

                solidColorBrush.Color = Colors.Orange; 
                ds.FillMesh(mesh, solidColorBrush);
                solidColorBrush.Color = Colors.LightBlue; 
                ds.DrawGeometry(pathGeometry, solidColorBrush, 12);
                solidColorBrush.Color = Colors.DarkSlateGray; // Dashed line
                ds.DrawGeometry(pathGeometry, solidColorBrush, 8, strokeStyle);

            }
#endif
        }

#if IMPL
        ImageBrush m_imageBrush;
#endif
    }


}
