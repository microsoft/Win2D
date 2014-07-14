// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using Microsoft.Graphics.Canvas;
using System;
using System.Collections.Generic;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Foundation;
using Windows.UI.Text;

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
            Text,
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

            m_widthCombo.SelectedItem = 1024;
            m_heightCombo.SelectedItem = 1024;

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

                        case DrawnContentType.Dashed_Lines:
                            DrawDashedLines(ds, canvasSolidColorBrush, horizontalLimit, verticalLimit);
                            break;

                        case DrawnContentType.Text:                            
                            var p = NextRandomPoint(horizontalLimit, verticalLimit);
                            var x = p.X;
                            var y = p.Y;
                            ds.DrawLine(new Point(x, 0), new Point(x, verticalLimit), canvasSolidColorBrush);
                            ds.DrawLine(new Point(0, y), new Point(horizontalLimit, y), canvasSolidColorBrush);
                            ds.DrawText(
                                "Centered", 
                                p, 
                                canvasSolidColorBrush, 
                                new CanvasTextFormat() 
                                { 
                                    FontSize = 18,
                                    VerticalAlignment = CanvasVerticalAlignment.Center, 
                                    ParagraphAlignment = ParagraphAlignment.Center 
                                });

                            var r = NextRandomRect(horizontalLimit, verticalLimit);
                            ds.DrawRectangle(r, canvasSolidColorBrush);
                            ds.DrawText(
                                m_quiteLongText,
                                r,
                                canvasSolidColorBrush,
                                new CanvasTextFormat()
                                {
                                    FontFamilyName = "Comic Sans MS",
                                    FontSize = 18,
                                    ParagraphAlignment = ParagraphAlignment.Justify,
                                    Options = CanvasDrawTextOptions.Clip
                                });
                            

                            break;

                        case DrawnContentType.Test_Scene0_Default:
                            GeometryTestScene0.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Default);
                            break;

                        case DrawnContentType.Test_Scene0_Wireframe:
                            GeometryTestScene0.DrawGeometryTestScene(ds, canvasSolidColorBrush, TestSceneRenderingType.Wireframe);
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

        // http://www.gutenberg.org/files/32572/32572-0.txt
        static string m_quiteLongText = "Many years ago there was an emperor who was so fond of new clothes that he spent all his money on them. " +
            "He did not give himself any concern about his army; he cared nothing about the theater or for driving about in the woods, " +
            "except for the sake of showing himself off in new clothes. He had a costume for every hour in the day, and just as they say " +
            "of a king or emperor, \"He is in his council chamber,\" they said of him, \"The emperor is in his dressing room.\"";
    }
}
