// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    //
    // This control is used by the TextDirectionsExample to draw arrows
    // representing the various values for CanvasTextDirection.
    //
    public sealed class TextDirectionControl : UserControl
    {
        public CanvasTextDirection TextDirection
        {
            get { return (CanvasTextDirection)GetValue(TextDirectionProperty); }
            set { SetValue(TextDirectionProperty, value); }
        }

        public static readonly DependencyProperty TextDirectionProperty =
            DependencyProperty.Register(
                "TextDirection",
                typeof(CanvasTextDirection),
                typeof(TextDirectionControl),
                new PropertyMetadata(CanvasTextDirection.LeftToRightThenTopToBottom, new PropertyChangedCallback(OnPropertyChanged)));

        CanvasControl canvas;
        SharedResources sharedResources;

        public TextDirectionControl()
        {
            Loaded += TextDirectionControl_Loaded;
            Unloaded += TextDirectionControl_Unloaded;
        }

        void TextDirectionControl_Loaded(object sender, RoutedEventArgs e)
        {
            canvas = new CanvasControl();
            canvas.Draw += OnDraw;

            Content = canvas;
        }

        void TextDirectionControl_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            if (canvas != null)
            {
                canvas.RemoveFromVisualTree();
                canvas = null;
            }
        }

        static void OnPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var instance = d as TextDirectionControl;
            if (instance == null)
                return;

            if (instance.canvas != null)
            {
                instance.canvas.Invalidate();
            }
        }

        void OnDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            float strokeWidth = 1;

            sharedResources = GetSharedResources(sender);
            var arrow = sharedResources.GetArrow(TextDirection);
            var bounds = arrow.ComputeStrokeBounds(strokeWidth);

            var ds = args.DrawingSession;

            var foregroundBrush = (SolidColorBrush)this.Foreground;
            var color = foregroundBrush.Color;

            arrow = arrow.Transform(CalculateTransform(bounds, new Rect(new Point(0, 0), sender.Size)));

            ds.DrawGeometry(arrow, color, strokeWidth, ArrowStrokeStyle);
        }

        public static Matrix3x2 CalculateTransform(Rect sourceBounds, Rect destBounds)
        {
            float scaleAmount = (float)Math.Min(destBounds.Height / sourceBounds.Height, destBounds.Width / sourceBounds.Width);

            var sourceCenter = new Vector2((float)(sourceBounds.Left + sourceBounds.Width / 2), (float)(sourceBounds.Top + sourceBounds.Height / 2));
            var destCenter = new Vector2((float)(destBounds.Left + destBounds.Width / 2), (float)(destBounds.Top + destBounds.Height / 2));

            return Matrix3x2.CreateScale(scaleAmount, scaleAmount, sourceCenter) * Matrix3x2.CreateTranslation(destCenter - sourceCenter);
        }

        static public CanvasStrokeStyle ArrowStrokeStyle = new CanvasStrokeStyle
        {
            TransformBehavior = CanvasStrokeTransformBehavior.Fixed,
            StartCap = CanvasCapStyle.Round,
            EndCap = CanvasCapStyle.Round,
            LineJoin = CanvasLineJoin.Round
        };


        //
        // SharedResources are shared between multiple CanvasControl instances.  Since all CanvasControls share the same CanvasDevice
        // we can avoid creating multiple copies of device dependent resources (such as the arrow CanvasGeometry).
        //
        // We arrange for there to be a single instance of SharedResources that can be reused by TextDirectionControl as well as 
        // TextDirectionsExample.  If the device is lost then a new SharedResources will be created.
        //       
        public class SharedResources
        {
            public CanvasDevice Device { get; private set; }
            CanvasGeometry arrow;

            public SharedResources(CanvasDevice device)
            {
                Device = device;
                arrow = MakeDirectionIcon();
            }

            public CanvasGeometry GetArrow(CanvasTextDirection direction)
            {
                return arrow.Transform(GetDirectionTransform(direction));
            }

            Matrix3x2 GetDirectionTransform(CanvasTextDirection direction)
            {
                var arrowBounds = arrow.ComputeStrokeBounds(4);
                var arrowCenter = new Size(arrowBounds.Width, arrowBounds.Height).ToVector2() / 2;
                Matrix3x2 directionTransform = Matrix3x2.Identity;

                switch (direction)
                {
                    case CanvasTextDirection.LeftToRightThenTopToBottom:
                        directionTransform = Matrix3x2.Identity;
                        break;

                    case CanvasTextDirection.RightToLeftThenTopToBottom:
                        directionTransform = Matrix3x2.CreateScale(-1, 1);
                        break;

                    case CanvasTextDirection.LeftToRightThenBottomToTop:
                        directionTransform = Matrix3x2.CreateScale(1, -1);
                        break;

                    case CanvasTextDirection.RightToLeftThenBottomToTop:
                        directionTransform = Matrix3x2.CreateScale(-1, -1);
                        break;

                    case CanvasTextDirection.TopToBottomThenLeftToRight:
                        directionTransform = Matrix3x2.CreateRotation((float)Math.PI / 2) * Matrix3x2.CreateScale(-1, 1);
                        break;

                    case CanvasTextDirection.BottomToTopThenLeftToRight:
                        directionTransform = Matrix3x2.CreateRotation((float)-Math.PI / 2);
                        break;

                    case CanvasTextDirection.TopToBottomThenRightToLeft:
                        directionTransform = Matrix3x2.CreateRotation((float)Math.PI / 2);
                        break;

                    case CanvasTextDirection.BottomToTopThenRightToLeft:
                        directionTransform = Matrix3x2.CreateRotation((float)-Math.PI / 2) * Matrix3x2.CreateScale(-1, 1);
                        break;
                }

                directionTransform = Matrix3x2.CreateTranslation(-arrowCenter) * directionTransform * Matrix3x2.CreateTranslation(arrowCenter);
                return directionTransform;
            }

            CanvasGeometry MakeDirectionIcon()
            {
                var builder = new CanvasPathBuilder(Device);

                float arrowHeadSize = 15;
                int lineCount = 4;

                var verticalLineTop = new Vector2(0, 0);
                var lineHeight = new Vector2(0, 20);
                var lineWidth = new Vector2(lineHeight.Y * lineCount, 0);
                var verticalLineBottom = verticalLineTop + lineHeight * lineCount;
                var arrowHeadWidth = new Vector2(arrowHeadSize / 2, 0);
                var arrowHeadHeight = new Vector2(0, arrowHeadSize / 2);

                // This is what we're drawing:
                //
                //  |------->
                //  |------->
                //  |------->
                //  V

                // A single stroke draws the first horizontal line and the vertical line:
                //
                //  |-------
                //  |
                //  |
                //  
                //
                builder.BeginFigure(verticalLineTop + lineWidth);
                builder.AddLine(verticalLineTop);
                builder.AddLine(verticalLineBottom);
                builder.EndFigure(CanvasFigureLoop.Open);

                //
                // Now draw the bottom arrow
                //
                builder.BeginFigure(verticalLineBottom - arrowHeadWidth - arrowHeadHeight);
                builder.AddLine(verticalLineBottom);
                builder.AddLine(verticalLineBottom + arrowHeadWidth - arrowHeadHeight);
                builder.EndFigure(CanvasFigureLoop.Open);

                //
                // Now the horizontal lines and arrows
                //
                for (int i = 0; i < lineCount; ++i)
                {
                    var left = verticalLineTop + lineHeight * i;
                    var right = left + lineWidth;

                    // The first line we drew already, so we don't need to draw it again
                    if (i != 0)
                    {
                        // Draw the horizontal line
                        builder.BeginFigure(left);
                        builder.AddLine(right);
                        builder.EndFigure(CanvasFigureLoop.Open);
                    }

                    // Draw the arrow at the end of the line
                    builder.BeginFigure(right - arrowHeadWidth - arrowHeadHeight);
                    builder.AddLine(right);
                    builder.AddLine(right - arrowHeadWidth + arrowHeadHeight);
                    builder.EndFigure(CanvasFigureLoop.Open);
                }

                return CanvasGeometry.CreatePath(builder);
            }
        }

        static WeakReference<SharedResources> currentSharedResources = new WeakReference<SharedResources>(null);

        public static SharedResources GetSharedResources(ICanvasResourceCreator resourceCreator)
        {
            var device = resourceCreator.Device;
            SharedResources sharedResources;

            if (currentSharedResources.TryGetTarget(out sharedResources))
            {
                if (sharedResources.Device == device)
                {
                    return sharedResources;
                }
            }

            sharedResources = new SharedResources(device);
            currentSharedResources.SetTarget(sharedResources);
            return sharedResources;
        }
    }
}
