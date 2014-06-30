using Microsoft.Graphics.Canvas;
using System;
using System.Collections.Generic;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Foundation;

namespace CsConsumer
{
    public sealed partial class MainPage : Page
    {
        Random m_random = new Random();
        CanvasDevice m_device = new CanvasDevice();
        CanvasImageSource m_imageSource;
        enum DrawnContentType
        {
            Clear_Only,
            Line_Thin,
            Line_Thick,
            Rectangle_Thin,
            Rectangle_Thick,
            Rectangle_Filled,
            RoundedRectangle_Thin,
            RoundedRectangle_Thick,
            Ellipse_Thin,
            Ellipse_Thick,
            Ellipse_Fill,
            Dashed_Lines,
            Test_Scene0_Default,
            Test_Scene0_Wireframe,
            Test_Scene1_Default,
            Test_Scene1_Randomized,
            Test_Scene1_Wireframe,
            Total
        }

        public MainPage()
        {
            this.InitializeComponent();

            //
            // Set up the width/height combo boxes.
            //
            var sizes = new List<int>();
            for (int i = 0; i < 16; ++i)
            {
                sizes.Add(1 << i);
            }

            m_widthCombo.ItemsSource = sizes;
            m_heightCombo.ItemsSource = sizes;

            m_widthCombo.SelectedIndex = sizes.Count / 2;
            m_heightCombo.SelectedIndex = sizes.Count / 2; 

            m_widthCombo.SelectionChanged += ImageSizeChangeRequested;
            m_heightCombo.SelectionChanged += ImageSizeChangeRequested;

            // 
            // Populate the drawnContentType combo box.
            //
            var drawnContentElements = new List<DrawnContentType>();
            for (int i=0; i<(int)(DrawnContentType.Total); ++i)
            {
                drawnContentElements.Add((DrawnContentType)(i));
            }
            m_drawnContentTypeCombo.ItemsSource = drawnContentElements;
            m_drawnContentTypeCombo.SelectedIndex = 0;
            m_drawnContentTypeCombo.SelectionChanged += ImageContentChanged;

            //
            // Create the initial image source
            //
            RecreateImageSource();
            DrawStuff();
        }

        private void OnRedrawClicked(object sender, RoutedEventArgs e)
        {
            DrawStuff();
        }

        private void ImageSizeChangeRequested(object sender, SelectionChangedEventArgs e)
        {
            RecreateImageSource();
            DrawStuff();
        }

        private void ImageContentChanged(object sender, SelectionChangedEventArgs e)
        {
            DrawStuff();
        }

        private void RecreateImageSource()
        {
            try
            {
                //
                // Wipe current state -- so if this fails for some reason we're 
                // left with no image source rather than the previous image source.
                //
                m_imageSource = null;
                m_theImage.Source = null;

                //
                // Create a new image source for the specified width/height
                //
                int width = (int)m_widthCombo.SelectedValue;
                int height = (int)m_heightCombo.SelectedValue;

                m_imageSource = new CanvasImageSource(m_device, width, height);
                m_theImage.Source = m_imageSource;                

                m_statusMessage.Text = String.Format("{0}x{1} image source", width, height);
            }
            catch (Exception e)
            {
                m_statusMessage.Text = e.Message;
            }
        }
        void DrawDashedLines(
            CanvasDrawingSession ds, 
            CanvasSolidColorBrush canvasSolidColorBrush, 
            int horizontalLimit, 
            int verticalLimit)
        {
            CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
            strokeStyle.DashStyle = CanvasDashStyle.Dash;

            for(int i=0; i<100; i++)
            {
                ds.DrawLine(
                    NextRandomPoint(horizontalLimit, verticalLimit),
                    NextRandomPoint(horizontalLimit, verticalLimit), canvasSolidColorBrush,
                    5.0f,
                    strokeStyle);
            }
        }

        void DrawStuff()
        {
            int horizontalLimit = (int)m_widthCombo.SelectedValue;
            int verticalLimit = (int)m_heightCombo.SelectedValue;
            const float thickStrokeWidth = 80.0f;

            using (var ds = m_imageSource.CreateDrawingSession())
            {
                DrawnContentType drawnContentType = (DrawnContentType)m_drawnContentTypeCombo.SelectedValue;

                ds.Clear(NextRandomColor());

                using (var canvasSolidColorBrush = new CanvasSolidColorBrush(m_device, NextRandomColor()))
                {
                    switch (drawnContentType)
                    {
                        case DrawnContentType.Clear_Only:
                            break;

                        case DrawnContentType.Line_Thin:
                            ds.DrawLine(
                                NextRandomPoint(horizontalLimit, verticalLimit),
                                NextRandomPoint(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.Line_Thick:
                            ds.DrawLine(
                                NextRandomPoint(horizontalLimit, verticalLimit),
                                NextRandomPoint(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush,
                                thickStrokeWidth);
                            break;

                        case DrawnContentType.Rectangle_Thin:
                            ds.DrawRectangle(
                                NextRandomRect(horizontalLimit, verticalLimit), 
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.Rectangle_Thick:
                            ds.DrawRectangle(
                                NextRandomRect(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush,
                                thickStrokeWidth);
                            break;

                        case DrawnContentType.Rectangle_Filled:
                            ds.FillRectangle(
                                NextRandomRect(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.RoundedRectangle_Thin:
                            ds.DrawRoundedRectangle(
                                NextRandomRoundedRect(horizontalLimit, verticalLimit), 
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.RoundedRectangle_Thick:
                            ds.DrawRoundedRectangle(
                                NextRandomRoundedRect(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush,
                                thickStrokeWidth);
                            break;

                        case DrawnContentType.Ellipse_Thin:
                            ds.DrawEllipse(
                                NextRandomEllipse(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.Ellipse_Thick:
                            ds.DrawEllipse(
                                NextRandomEllipse(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush,
                                thickStrokeWidth);
                            break;

                        case DrawnContentType.Ellipse_Fill:
                            ds.FillEllipse(
                                NextRandomEllipse(horizontalLimit, verticalLimit),
                                canvasSolidColorBrush);
                            break;

                        case DrawnContentType.Test_Scene0_Default:
                            GeometryTestScene0.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Default);
                            break;

                        case DrawnContentType.Test_Scene0_Wireframe:
                            GeometryTestScene0.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Wireframe);
                            break;

                        case DrawnContentType.Dashed_Lines:
                            DrawDashedLines(ds, canvasSolidColorBrush, horizontalLimit, verticalLimit);
                            break;

                        case DrawnContentType.Test_Scene1_Default:
                            GeometryTestScene1.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Default);
                            break;

                        case DrawnContentType.Test_Scene1_Randomized:
                            GeometryTestScene1.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Randomized);
                            break;

                        case DrawnContentType.Test_Scene1_Wireframe:
                            GeometryTestScene1.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Wireframe);
                            break;

                        default:
                            System.Diagnostics.Debug.Assert(false); // Unexpected
                            break;
                    }
                }
            }
        }

        private CanvasEllipse NextRandomEllipse(int horizontalLimit, int verticalLimit)
        {
            CanvasEllipse ellipse = new CanvasEllipse();
            ellipse.Point = NextRandomPoint(horizontalLimit, verticalLimit);
            ellipse.RadiusX = m_random.Next(horizontalLimit / 2);
            ellipse.RadiusY = m_random.Next(verticalLimit / 2);
            return ellipse;
        }

        private CanvasRoundedRectangle NextRandomRoundedRect(int horizontalLimit, int verticalLimit)
        {
            CanvasRoundedRectangle roundedRectangle = new CanvasRoundedRectangle();
            roundedRectangle.Rect = NextRandomRect(horizontalLimit, verticalLimit);
            roundedRectangle.RadiusX = m_random.Next((int)(roundedRectangle.Rect.Width) / 2);
            roundedRectangle.RadiusY = m_random.Next((int)(roundedRectangle.Rect.Height) / 2);
            return roundedRectangle;
        }

        private Rect NextRandomRect(int horizontalLimit, int verticalLimit)
        {
            return new Rect(
                NextRandomPoint(horizontalLimit, verticalLimit),
                NextRandomPoint(horizontalLimit, verticalLimit));
        }

        private Point NextRandomPoint(int horizontalLimit, int verticalLimit)
        {
            return new Point(m_random.Next(horizontalLimit), m_random.Next(verticalLimit));
        }

        private Color NextRandomColor()
        {
            return Color.FromArgb(
                                (byte)255,
                                (byte)m_random.Next(256),
                                (byte)m_random.Next(256),
                                (byte)m_random.Next(256));
        }
    }
}
