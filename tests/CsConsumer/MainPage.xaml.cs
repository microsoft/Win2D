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
            DxFactory.SetDefaultOptions(DxDebugLevel.Information); // On CHK builds, enable the debug layer.
#endif
            DxDevice device = new DxDevice();
            DxDeviceContext m_deviceContext = new DxDeviceContext(device);

            // Create a DX image source and assign it to the Canvas XAML control as a background brush.
            DirectXImageSource m_surfaceImageSource = new DirectXImageSource(device, m_canvas.Width, m_canvas.Height);
            m_imageBrush = new ImageBrush();
            m_imageBrush.ImageSource = m_surfaceImageSource;
            m_canvas.Background = m_imageBrush;

            // Declare a red solid color brush.
            DxSolidColorBrush solidColorBrush = new DxSolidColorBrush(device, Color.FromArgb(0, 1, 1, 1));
            
            const int cellDim = 200;

            // Strokestyle
            DxStrokeStyleProperties strokeStyleProperties = new DxStrokeStyleProperties();
            strokeStyleProperties.DashStyle = DxDashStyle.Dash;
            strokeStyleProperties.DashCap = DxCapStyle.Round;
            DxStrokeStyle strokeStyle = new DxStrokeStyle(device, strokeStyleProperties);

            // Create a freeform geometry.
            DxPathGeometry pathGeometry = new DxPathGeometry(device);
            using (PathGeometryCreator pgc = new PathGeometryCreator(pathGeometry))
            {
                pgc.BeginFigure(new Point(cellDim, cellDim), DxFigureBegin.Filled);
                pgc.AddLine(new Point(cellDim * 2, cellDim + 10));
                pgc.AddLine(new Point(cellDim, cellDim + 20));
                pgc.AddLine(new Point(cellDim * 2, cellDim + 50));
                pgc.AddLine(new Point(cellDim, cellDim + 100));
                pgc.AddLine(new Point(cellDim * 2, cellDim * 2));
                pgc.EndFigure(DxFigureEnd.Closed);
            }
            // Turn it into a mesh.
            DxMesh mesh = new DxMesh(device, pathGeometry);

            // Draw a white X to an intermediate.
            DxBitmap whiteXBitmap = new DxBitmap(device, DxBitmapOptions.Target, cellDim, cellDim);
            using (DxDrawingSession ds = whiteXBitmap.CreateDrawingSession())
            {
                ds.SetAntialiasMode(DxAntialiasMode.PerPrimitive);
                solidColorBrush.Color = Colors.White; 
                ds.DrawLine(new Point(30, 30), new Point(cellDim - 30, cellDim - 30), solidColorBrush, 10.0f);
                ds.DrawLine(new Point(cellDim - 30, 30), new Point(30, cellDim - 30), solidColorBrush, 10.0f);
            }

            DxBitmap leafBitmap = new DxBitmap(device, "leaf.png");

            // Drawing to the Canvas XAML control.
            using (DxDrawingSession ds = m_surfaceImageSource.CreateDrawingSession())
            {
                ds.SetTransform(DxMatrix3x2F.RotationInDegrees(5));

                ds.DrawBitmap(whiteXBitmap, new Point(0, 0));

                solidColorBrush.Color = Colors.Blue; 
                ds.DrawBitmap(leafBitmap, new Point(cellDim, 10));
                ds.DrawRectangle(new Windows.Foundation.Rect(cellDim + 20, 20, cellDim - 40, cellDim - 40), solidColorBrush, 11);
                
                // Aliased and antialiased ellipses
                ds.SetAntialiasMode(DxAntialiasMode.Aliased);
                solidColorBrush.Color = Colors.Red; 
                ds.FillEllipse(new Point(cellDim / 2 - 20, cellDim + (cellDim / 2) - 20), cellDim / 3, cellDim / 3, solidColorBrush);
                ds.SetAntialiasMode(DxAntialiasMode.PerPrimitive);

                // Make an ellipse geometry.
                DxEllipseGeometry ellipseGeometry = new DxEllipseGeometry(device, new DxEllipse(new Point(cellDim / 2 + 20, cellDim + (cellDim / 2) + 20), cellDim / 3, cellDim / 3));
                // Draw the tessellated ellipse.
                System.Collections.Generic.IReadOnlyList < DxTriangle > tessellation = ellipseGeometry.GetTessellation();
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

        ImageBrush m_imageBrush;
    }


}
