// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class VectorArt : UserControl
    {
        int whichScene;
        bool isWireframe;
        int? randomSeed;

        CanvasDrawingSession currentDrawingSession;

        static Vector2[] sceneSizes =
        {
            new Vector2(1200, 900),
            new Vector2(960, 720),
        };

        public VectorArt()
        {
            this.InitializeComponent();

            sceneListBox.SelectedIndex = 0;
        }

        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            // Transform and clip so the scene will fit whatever size display we are running on.
            Vector2 sceneSize = sceneSizes[whichScene];

            args.DrawingSession.Transform = Utils.GetDisplayTransform(sender.Size.ToVector2(), sceneSize);

            using (args.DrawingSession.CreateLayer(1f, new Rect(0, 0, sceneSize.X, sceneSize.Y)))
            {
                // Draw the vector art.
                currentDrawingSession = args.DrawingSession;

                switch (whichScene)
                {
                    case 0:
                        DrawScene0();
                        break;

                    case 1:
                        DrawScene1(randomSeed);
                        break;
                }
            }
        }

        void SceneChanged(object sender, SelectionChangedEventArgs e)
        {
            whichScene = sceneListBox.SelectedIndex;

            randomSeed = null;
            randomizeButton.Visibility = (whichScene > 0) ? Visibility.Visible : Visibility.Collapsed;

            canvas.Invalidate();
        }

        void WireframeChanged(object sender, RoutedEventArgs e)
        {
            isWireframe = wireframeButton.IsChecked.Value;

            if (isWireframe)
            {
                randomSeed = null;
            }

            canvas.Invalidate();
        }

        void RandomizeButtonClick(object sender, RoutedEventArgs e)
        {
            randomSeed = new Random().Next();

            wireframeButton.IsChecked = false;

            canvas.Invalidate();
        }

        void Clear(Color color)
        {
            if (!isWireframe)
            {
                currentDrawingSession.Clear(color);
            }
            else
            {
                currentDrawingSession.Clear(Colors.White);
            }
        }

        void FillRectangle(Rect rect, Color color)
        {
            if (!isWireframe)
            {
                currentDrawingSession.FillRectangle(rect, color);
            }
            else
            {
                currentDrawingSession.DrawRectangle(rect, Colors.Black);
            }
        }

        void DrawLine(Vector2 p1, Vector2 p2, Color color, float strokeWidth)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawLine(p1, p2, color, strokeWidth);
            }
            else
            {
                currentDrawingSession.DrawLine(p1, p2, Colors.Black);
            }
        }

        void DrawLine(Vector2 p1, Vector2 p2, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawLine(p1, p2, color, strokeWidth, strokeStyle);
            }
            else
            {
                currentDrawingSession.DrawLine(p1, p2, Colors.Black);
            }
        }

        void FillEllipse(Vector2 center, float radiusX, float radiusY, Color color)
        {
            if (!isWireframe)
            {
                currentDrawingSession.FillEllipse(center, radiusX, radiusY, color);
            }
            else
            {
                currentDrawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        void DrawEllipse(Vector2 center, float radiusX, float radiusY, Color color, float strokeWidth)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawEllipse(center, radiusX, radiusY, color, strokeWidth);
            }
            else
            {
                currentDrawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        void DrawEllipse(Vector2 center, float radiusX, float radiusY, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawEllipse(center, radiusX, radiusY, color, strokeWidth, strokeStyle);
            }
            else
            {
                currentDrawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        void FillCircle(Vector2 centerPoint, float radius, Color color)
        {
            if (!isWireframe)
            {
                currentDrawingSession.FillCircle(centerPoint, radius, color);
            }
            else
            {
                currentDrawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        void DrawCircle(Vector2 centerPoint, float radius, Color color, float strokeWidth)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawCircle(centerPoint, radius, color, strokeWidth);
            }
            else
            {
                currentDrawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        void DrawCircle(Vector2 centerPoint, float radius, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (!isWireframe)
            {
                currentDrawingSession.DrawCircle(centerPoint, radius, color, strokeWidth, strokeStyle);
            }
            else
            {
                currentDrawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
