// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    public sealed partial class ArcOptions : UserControl
    {
        // Which of the two CanvasPathBuilder.AddArc method overloads are we currently demonstrating?
        public enum AddArcOverload
        {
            AroundEllipse,
            PointToPoint,
        };

        public AddArcOverload CurrentOverload { get; set; }

        // Parameters used by both AddArc overloads.
        Vector2[] arcPoints = new Vector2[2];

        // Parameters used by AddArcOverload.AroundEllipse.
        public float ArcStartAngle { get; set; }
        public float ArcSweepAngle { get; set; }

        // Parameters used by AddArcOverload.PointToPoint.
        public float ArcRadiusX { get; set; }
        public float ArcRadiusY { get; set; }

        public float ArcRotation { get; set; }

        public CanvasSweepDirection ArcSweepDirection { get; set; }
        public CanvasArcSize ArcSize { get; set; }

        // Enum metadata for XAML databinding.
        public List<AddArcOverload> AddArcOverloads { get { return Utils.GetEnumAsList<AddArcOverload>(); } }
        public List<CanvasSweepDirection> SweepDirections { get { return Utils.GetEnumAsList<CanvasSweepDirection>(); } }
        public List<CanvasArcSize> ArcSizes { get { return Utils.GetEnumAsList<CanvasArcSize>(); } }

        // Pointer input state.
        int? activeDrag;
        Vector2 dragOffset;

        const float hitTestRadius = 32;

        // Rendering settings for displaying the arc.
        const float strokeWidth = 12;

        CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle()
        {
            StartCap = CanvasCapStyle.Round,
            EndCap = CanvasCapStyle.Triangle,
        };

        CanvasTextFormat textFormat = new CanvasTextFormat()
        {
            HorizontalAlignment = CanvasHorizontalAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center,
        };

        public ArcOptions()
        {
            this.InitializeComponent();
        }

        void Canvas_Loaded(object sender, RoutedEventArgs e)
        {
            arcPoints[0] = new Vector2(40);
            arcPoints[1] = Vector2.Min(new Vector2(480, 320), canvas.Size.ToVector2() - new Vector2(40));

            ArcStartAngle = 205;
            ArcSweepAngle = -165;

            ArcRadiusX = ArcRadiusY = Vector2.Distance(arcPoints[0], arcPoints[1]);
        }

        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var centerPoint = (arcPoints[0] + arcPoints[1]) / 2;

            // Draw the end point markers.
            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                for (int i = 0; i < 2; i++)
                {
                    ds.DrawCircle(arcPoints[i], hitTestRadius, (i == activeDrag) ? Colors.White : Colors.Gray);
                }
            }

            switch (CurrentOverload)
            {
                case AddArcOverload.AroundEllipse:
                    // Compute positions.
                    var ellipseRadius = (arcPoints[1] - arcPoints[0]) / 2;

                    ellipseRadius.X = Math.Abs(ellipseRadius.X);
                    ellipseRadius.Y = Math.Abs(ellipseRadius.Y);

                    float startAngle = Utils.DegreesToRadians(ArcStartAngle);
                    float sweepAngle = Utils.DegreesToRadians(ArcSweepAngle);

                    var startPoint = centerPoint + Vector2.Transform(Vector2.UnitX, Matrix3x2.CreateRotation(startAngle)) * ellipseRadius;

                    // Draw the bounding rectangle.
                    if (!ThumbnailGenerator.IsDrawingThumbnail)
                    {
                        ds.DrawRectangle(new Rect(arcPoints[0].ToPoint(), arcPoints[1].ToPoint()), Color.FromArgb(255, 64, 64, 64));
                    }

                    // Draw the arc.
                    using (var builder = new CanvasPathBuilder(sender))
                    {
                        builder.BeginFigure(startPoint);
                        builder.AddArc(centerPoint, ellipseRadius.X, ellipseRadius.Y, startAngle, sweepAngle);
                        builder.EndFigure(CanvasFigureLoop.Open);

                        using (var geometry = CanvasGeometry.CreatePath(builder))
                        {
                            ds.DrawGeometry(geometry, Colors.Yellow, strokeWidth, strokeStyle);
                        }
                    }
                    break;
             
                case AddArcOverload.PointToPoint:
                    // Display a warning if this is an invalid arc configuration.
                    bool isRadiusTooSmall = IsArcRadiusTooSmall();

                    if (isRadiusTooSmall)
                    {
                        ds.DrawText("Radius is less than the\ndistance between the\nstart and end points", centerPoint, Colors.Red, textFormat);
                    }

                    // Draw the arc.
                    using (var builder = new CanvasPathBuilder(sender))
                    {
                        builder.BeginFigure(arcPoints[0]);
                        builder.AddArc(arcPoints[1], ArcRadiusX, ArcRadiusY, Utils.DegreesToRadians(ArcRotation), ArcSweepDirection, ArcSize);
                        builder.EndFigure(CanvasFigureLoop.Open);

                        using (var geometry = CanvasGeometry.CreatePath(builder))
                        {
                            ds.DrawGeometry(geometry, isRadiusTooSmall ? Colors.Red : Colors.Yellow, strokeWidth, strokeStyle);
                        }
                    }
                    break;
            }
        }

        bool IsArcRadiusTooSmall()
        {
            // Get the distance between the arc start/end points.
            Vector2 arc = arcPoints[1] - arcPoints[0];

            // Compensate for any ellipse rotation.
            arc = Vector2.Transform(arc, Matrix3x2.CreateRotation(-Utils.DegreesToRadians(ArcRotation)));

            // Normalize to a unit circle.
            arc /= new Vector2(ArcRadiusX, ArcRadiusY);

            // If the length is greater than 2, there is no possible way to fit an arc of
            // the specified radius between the specified endpoints. D2D will compensate
            // by scaling up the radius, but the result may not be what was intended.
            return arc.Length() > 2;
        }

        void canvas_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            var pointerPos = e.GetCurrentPoint(canvas).Position.ToVector2();

            float[] targetDistances =
            {
                Vector2.Distance(pointerPos, arcPoints[0]),
                Vector2.Distance(pointerPos, arcPoints[1]),
            };

            int closestTarget = (targetDistances[0] < targetDistances[1]) ? 0 : 1;

            if (targetDistances[closestTarget] <= hitTestRadius)
            {
                activeDrag = closestTarget;
                dragOffset = pointerPos - arcPoints[closestTarget];
                canvas.Invalidate();
            }
        }

        void canvas_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            if (activeDrag.HasValue)
            {
                activeDrag = null;
                canvas.Invalidate();
            }
        }

        void canvas_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            if (activeDrag.HasValue)
            {
                var pointerPos = e.GetCurrentPoint(canvas).Position.ToVector2();
                arcPoints[activeDrag.Value] = pointerPos - dragOffset;
                canvas.Invalidate();
            }
        }

        void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canvas.Invalidate();
        }

        void Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            canvas.Invalidate();
        }

        void Overload_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            aroundEllipseControls.Visibility = (CurrentOverload == AddArcOverload.AroundEllipse) ? Visibility.Visible : Visibility.Collapsed;
            pointToPointControls.Visibility  = (CurrentOverload == AddArcOverload.PointToPoint)  ? Visibility.Visible : Visibility.Collapsed;

            canvas.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
