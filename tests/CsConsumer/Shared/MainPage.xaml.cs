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
using Microsoft.Graphics.Canvas.Effects;
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
        CanvasBitmap m_bitmap_tiger;

        enum DrawnContentType
        {
            Clear_Only,
            Bitmap,
            Effect_Blur,
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
            Circle_Fill,
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

            m_canvasControl.CreateResources += m_canvasControl_CreateResources;
            m_canvasControl.Draw += m_canvasControl_Draw;

            // 
            // Populate the drawnContentType combo box.
            //
            var drawnContentElements = new List<DrawnContentType>();
            for (int i = 0; i < (int)(DrawnContentType.Total); ++i)
            {
                drawnContentElements.Add((DrawnContentType)(i));
            }
            m_drawnContentTypeCombo.ItemsSource = drawnContentElements;
            m_drawnContentTypeCombo.SelectedIndex = 0;
            m_drawnContentTypeCombo.SelectionChanged += ImageContentChanged;                     
        }

        void m_canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawStuff(args.DrawingSession);
        }

        void m_canvasControl_CreateResources(CanvasControl sender, object args)
        {
            UpdateCanvasControlSize();
        }

        private void OnRedrawClicked(object sender, RoutedEventArgs e)
        {
            m_canvasControl.Invalidate();
        }


        private void ImageContentChanged(object sender, SelectionChangedEventArgs e)
        {
            m_canvasControl.Invalidate();
        }

        private void UpdateCanvasControlSize()
        {
            try
            {
                //
                // Create a new image source for the specified width/height
                //
                int width = (int)m_canvasControl.ActualWidth;
                int height = (int)m_canvasControl.ActualHeight;

                m_statusMessage.Text = String.Format("{0}x{1} canvas control", width, height);
            }
            catch (Exception e)
            {
                m_statusMessage.Text = e.Message;
            }
        }
        void DrawDashedLines(
            CanvasDrawingSession ds,
            Color color,
            int horizontalLimit,
            int verticalLimit)
        {
            CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
            strokeStyle.DashStyle = CanvasDashStyle.Dash;

            for (int i = 0; i < 100; i++)
            {
                ds.DrawLine(
                    NextRandomPoint(horizontalLimit, verticalLimit),
                    NextRandomPoint(horizontalLimit, verticalLimit),
                    color,
                    5.0f,
                    strokeStyle);
            }
        }

        void DrawStuff(CanvasDrawingSession ds)
        {
            int horizontalLimit = (int)m_canvasControl.ActualWidth;
            int verticalLimit = (int)m_canvasControl.ActualHeight;
            const float thickStrokeWidth = 80.0f;

            DrawnContentType drawnContentType = (DrawnContentType)m_drawnContentTypeCombo.SelectedValue;

            ds.Clear(NextRandomColor());
                
            Rect rect;
            Point point;
            float radiusX;
            float radiusY;

            switch (drawnContentType)
            {
                case DrawnContentType.Clear_Only:
                    break;

                case DrawnContentType.Bitmap:
                    if (m_bitmap_tiger != null)
                    {
                        ds.DrawImage(m_bitmap_tiger, NextRandomPoint(horizontalLimit, verticalLimit));
                    }
                    else
                    {
                        DrawNoBitmapErrorMessage(ds, horizontalLimit / 2, verticalLimit / 2);
                    }
                    break;
                case DrawnContentType.Effect_Blur:
                    if (m_bitmap_tiger != null)
                    {
                        GaussianBlurEffect blurEffect = new GaussianBlurEffect();
                        blurEffect.StandardDeviation = 2.0f;
                        blurEffect.Source = m_bitmap_tiger;
                        ds.DrawImage(blurEffect, NextRandomPoint(horizontalLimit, verticalLimit));
                    }
                    else
                    {
                        DrawNoBitmapErrorMessage(ds, horizontalLimit / 2, verticalLimit / 2);
                    }
                    break;
                case DrawnContentType.Line_Thin:
                    ds.DrawLine(
                        NextRandomPoint(horizontalLimit, verticalLimit),
                        NextRandomPoint(horizontalLimit, verticalLimit),
                        NextRandomColor());
                    break;

                case DrawnContentType.Line_Thick:
                    ds.DrawLine(
                        NextRandomPoint(horizontalLimit, verticalLimit),
                        NextRandomPoint(horizontalLimit, verticalLimit),
                        NextRandomColor(),
                        thickStrokeWidth);
                    break;

                case DrawnContentType.Rectangle_Thin:
                    ds.DrawRectangle(
                        NextRandomRect(horizontalLimit, verticalLimit),
                        NextRandomColor());
                    break;

                case DrawnContentType.Rectangle_Thick:
                    ds.DrawRectangle(
                        NextRandomRect(horizontalLimit, verticalLimit),
                        NextRandomColor(),
                        thickStrokeWidth);
                    break;

                case DrawnContentType.Rectangle_Filled:
                    ds.FillRectangle(
                        NextRandomRect(horizontalLimit, verticalLimit),
                        NextRandomColor());
                    break;

                case DrawnContentType.RoundedRectangle_Thin:
                    NextRandomRoundedRect(horizontalLimit, verticalLimit, out rect, out radiusX, out radiusY);
                    ds.DrawRoundedRectangle(
                        rect, radiusX, radiusY,
                        NextRandomColor());
                    break;

                case DrawnContentType.RoundedRectangle_Thick:
                    NextRandomRoundedRect(horizontalLimit, verticalLimit, out rect, out radiusX, out radiusY);
                    ds.DrawRoundedRectangle(
                        rect, radiusX, radiusY,
                        NextRandomColor(),
                        thickStrokeWidth);
                    break;

                case DrawnContentType.Ellipse_Thin:
                    NextRandomEllipse(horizontalLimit, verticalLimit, out point, out radiusX, out radiusY);
                    ds.DrawEllipse(
                        point, radiusX, radiusY,
                        NextRandomColor());
                    break;

                case DrawnContentType.Ellipse_Thick:
                    NextRandomEllipse(horizontalLimit, verticalLimit, out point, out radiusX, out radiusY);
                    ds.DrawEllipse(
                        point, radiusX, radiusY,
                        NextRandomColor(),
                        thickStrokeWidth);
                    break;

                case DrawnContentType.Ellipse_Fill:
                    NextRandomEllipse(horizontalLimit, verticalLimit, out point, out radiusX, out radiusY);
                    ds.FillEllipse(
                        point, radiusX, radiusY,
                        NextRandomColor());
                    break;

                case DrawnContentType.Circle_Fill:
                    ds.FillCircle(
                        NextRandomPoint(horizontalLimit, verticalLimit),
                        100,
                        NextRandomColor());
                    break;

                case DrawnContentType.Dashed_Lines:
                    DrawDashedLines(ds, NextRandomColor(), horizontalLimit, verticalLimit);
                    break;

                case DrawnContentType.Text:
                    var p = NextRandomPoint(horizontalLimit, verticalLimit);
                    var x = p.X;
                    var y = p.Y;
                    var color = NextRandomColor();
                    ds.DrawLine(new Point(x, 0), new Point(x, verticalLimit), color);
                    ds.DrawLine(new Point(0, y), new Point(horizontalLimit, y), color);
                    ds.DrawText(
                        "Centered",
                        p,
                        color,
                        new CanvasTextFormat()
                        {
                            FontSize = 18,
                            VerticalAlignment = CanvasVerticalAlignment.Center,
                            ParagraphAlignment = ParagraphAlignment.Center
                        });

                    var r = NextRandomRect(horizontalLimit, verticalLimit);
                    ds.DrawRectangle(r, color);
                    ds.DrawText(
                        m_quiteLongText,
                        r,
                        NextRandomColor(),
                        new CanvasTextFormat()
                        {
                            FontFamily = "Comic Sans MS",
                            FontSize = 18,
                            ParagraphAlignment = ParagraphAlignment.Justify,
                            Options = CanvasDrawTextOptions.Clip
                        });
                    break;

                case DrawnContentType.Test_Scene0_Default:
                    GeometryTestScene0.DrawGeometryTestScene(ds, TestSceneRenderingType.Default);
                    break;

                case DrawnContentType.Test_Scene0_Wireframe:
                    GeometryTestScene0.DrawGeometryTestScene(ds, TestSceneRenderingType.Wireframe);
                    break;

                case DrawnContentType.Test_Scene1_Default:
                    GeometryTestScene1.DrawGeometryTestScene(ds, TestSceneRenderingType.Default);
                    break;

                case DrawnContentType.Test_Scene1_Randomized:
                    GeometryTestScene1.DrawGeometryTestScene(ds, TestSceneRenderingType.Randomized);
                    break;

                case DrawnContentType.Test_Scene1_Wireframe:
                    GeometryTestScene1.DrawGeometryTestScene(ds, TestSceneRenderingType.Wireframe);
                    break;

                default:
                    System.Diagnostics.Debug.Assert(false); // Unexpected
                    break;
            }
        }

        private void NextRandomEllipse(int horizontalLimit, int verticalLimit, out Point point, out float radiusX, out float radiusY)
        {
            point = NextRandomPoint(horizontalLimit, verticalLimit);
            radiusX = m_random.Next(horizontalLimit / 2);
            radiusY = m_random.Next(verticalLimit / 2);
        }

        private void NextRandomRoundedRect(int horizontalLimit, int verticalLimit, out Rect rectangle, out float radiusX, out float radiusY)
        {
            rectangle = NextRandomRect(horizontalLimit, verticalLimit);
            radiusX = m_random.Next((int)(rectangle.Width) / 2);
            radiusY = m_random.Next((int)(rectangle.Height) / 2);
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

        private void DrawNoBitmapErrorMessage(CanvasDrawingSession ds, int x, int y)
        {
            ds.DrawText(
                "Please load bitmap before drawing it",
                new Point(x, y),
                Colors.Red,
                new CanvasTextFormat()
                {
                    FontSize = 24,
                    VerticalAlignment = CanvasVerticalAlignment.Center,
                    ParagraphAlignment = ParagraphAlignment.Center
                });
        }

        // http://www.gutenberg.org/files/32572/32572-0.txt
        static string m_quiteLongText = "Many years ago there was an emperor who was so fond of new clothes that he spent all his money on them. " +
            "He did not give himself any concern about his army; he cared nothing about the theater or for driving about in the woods, " +
            "except for the sake of showing himself off in new clothes. He had a costume for every hour in the day, and just as they say " +
            "of a king or emperor, \"He is in his council chamber,\" they said of him, \"The emperor is in his dressing room.\"";

        async private void OnLoadBitmapAsyncClicked(object sender, RoutedEventArgs e)
        {
            string fileNameTiger = "imageTiger.jpg";
            m_bitmap_tiger = await CanvasBitmap.LoadAsync(m_canvasControl, fileNameTiger);

            m_canvasControl.Invalidate();
        }
    }
}
