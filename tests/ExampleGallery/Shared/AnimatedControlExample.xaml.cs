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
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Reflection;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace ExampleGallery
{
    public sealed partial class AnimatedControlExample : UserControl
    {
        public AnimatedControlExample()
        {
            this.InitializeComponent();

            // Populate the colors combo
            var colors = typeof(Colors).GetTypeInfo().DeclaredProperties;
            PropertyInfo transparentPropertyInfo = null;
            foreach (var item in colors)
            {
                clearColor.Items.Add(item);
                if (item.Name == "Transparent")
                    transparentPropertyInfo = item;
            }

            animatedControl.ClearColor = Colors.Transparent;
            clearColor.SelectedItem = transparentPropertyInfo;

            CurrentTimestepType = TimestepTypeOption.Fixed;
        }

        public int SelectedTargetElapsedTime
        {
            get
            {
                return (int)animatedControl.TargetElapsedTime.Ticks;
            }

            set
            {
                animatedControl.TargetElapsedTime = new TimeSpan(value);

                // If we're paused then do one step to allow the display to update for the new
                // value.
                if (animatedControl.Paused)
                {
                    step = true;
                    animatedControl.Paused = false;
                }
            }
        }


        private SweepRenderer sweepRenderer;
        private UpdatesPerDrawRenderer updatesPerDrawRenderer;
        private TouchPointsRenderer touchPointsRenderer;
        private bool step = false;

        public enum TimestepTypeOption
        {
            Fixed,
            Variable
        }

        public List<TimestepTypeOption> TimestepTypeOptions { get { return Utils.GetEnumAsList<TimestepTypeOption>(); } }
        public TimestepTypeOption CurrentTimestepType { get; set; }

        private void OnCreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }

        private async Task Canvas_CreateResourcesAsync(CanvasAnimatedControl sender)
        {
            sweepRenderer = await SweepRenderer.Create(sender);
            updatesPerDrawRenderer = new UpdatesPerDrawRenderer();
            touchPointsRenderer = new TouchPointsRenderer(sender);
        }

        private const float width = 1000;
        private const float height = 1000;

        private void OnDraw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            int updateCount = (int)(args.Timing.UpdateCount);

            var ds = args.DrawingSession;

            //
            // Pick layout
            //
            var size = sender.Size;

            Matrix3x2 counterTransform;
            Matrix3x2 graphTransform;
            CalculateLayout(size, width, height, out counterTransform, out graphTransform);

            //
            // Draw
            //
            ds.Transform = counterTransform;
            sweepRenderer.Draw(sender, args.Timing, ds);

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                ds.Transform = Matrix3x2.CreateTranslation(graphTransform.Translation);
                updatesPerDrawRenderer.Draw(args, ds, width * graphTransform.M11, height * graphTransform.M22);

                ds.Transform = Matrix3x2.Identity;
                touchPointsRenderer.Draw(ds);
            }
        }

        private static void CalculateLayout(Size size, float width, float height, out Matrix3x2 counterTransform, out Matrix3x2 graphTransform)
        {
            bool verticalLayout = true;
            if (size.Width > size.Height)
                verticalLayout = false;

            if (verticalLayout)
            {
                float targetWidth = (float)size.Width;
                float targetHeight = (float)size.Height / 2;

                float scaleFactor = targetHeight / height;

                if ((width * scaleFactor) > targetWidth)
                {
                    scaleFactor = targetWidth / width;
                }

                float xoffset = (targetWidth / 2) - (height * scaleFactor) / 2;
                counterTransform = Matrix3x2.CreateScale(scaleFactor, scaleFactor) * Matrix3x2.CreateTranslation(xoffset, 0);
                graphTransform = Matrix3x2.CreateScale(scaleFactor, scaleFactor) * Matrix3x2.CreateTranslation(xoffset, targetHeight);
            }
            else
            {
                float targetWidth = (float)size.Width / 2;
                float targetHeight = (float)size.Height;

                float scaleFactor = targetWidth / width;

                if ((height * scaleFactor) > targetHeight)
                {
                    scaleFactor = targetHeight / height;
                }

                float yoffset = (targetHeight / 2) - (height * scaleFactor) / 2;
                counterTransform = Matrix3x2.CreateScale(scaleFactor, scaleFactor) * Matrix3x2.CreateTranslation(0, yoffset);
                graphTransform = Matrix3x2.CreateScale(scaleFactor, scaleFactor) * Matrix3x2.CreateTranslation(targetWidth, yoffset);
            }
        }

        private void OnUpdate(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            updatesPerDrawRenderer.Update();

            if (step)
            {
                sender.Paused = true;
                step = false;
            }
        }

        private void Pause_Checked(object sender, RoutedEventArgs e)
        {
            var button = (ToggleButton)sender;
            this.animatedControl.Paused = button.IsChecked.Value;
            this.stepButton.IsEnabled = true;
        }

        private void Pause_Unchecked(object sender, RoutedEventArgs e)
        {
            var button = (ToggleButton)sender;
            this.animatedControl.Paused = button.IsChecked.Value;
            this.stepButton.IsEnabled = false;
        }

        private void Step_Clicked(object sender, RoutedEventArgs e)
        {
            step = true;
            this.animatedControl.Paused = false;
        }

        private void clearColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selectedItem = clearColor.SelectedItem as PropertyInfo;
            if (selectedItem != null)
            {
                var color = (Color)selectedItem.GetValue(null);
                animatedControl.ClearColor = color;
            }
        }

        public void TimestepTypeOptionsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.animatedControl.IsFixedTimeStep = CurrentTimestepType == TimestepTypeOption.Fixed;

            targetElapsedTime.IsEnabled = CurrentTimestepType == TimestepTypeOption.Fixed;
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            animatedControl.RemoveFromVisualTree();
            animatedControl = null;
        }
    }

    class SweepRenderer
    {
        public static async Task<SweepRenderer> Create(CanvasAnimatedControl sender)
        {
            var bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
            return new SweepRenderer(sender, bitmapTiger);
        }

        SweepRenderer(CanvasAnimatedControl sender, CanvasBitmap bitmapTiger)
        {
            CreateBrushes(sender, bitmapTiger);
            CreateClockFace(sender);
        }

        void CreateBrushes(CanvasAnimatedControl sender, CanvasBitmap bitmapTiger)
        {
            var bitmapSize = bitmapTiger.Size;
            var scale = (radius * 2) / (float)bitmapSize.Height;

            var backgroundEffect = new Transform2DEffect()
            {
                Source = bitmapTiger,
                TransformMatrix = Matrix3x2.CreateScale(scale, scale) * Matrix3x2.CreateTranslation(center - radius, center - radius)
            };

            backgroundBrush = new CanvasImageBrush(sender, backgroundEffect)
            {
                SourceRectangle = new Rect(0, 0, size, size),
                Opacity = 0.6f
            };

            hueRotationEffect = new HueRotationEffect()
            {
                Source = backgroundEffect,
                Angle = (float)Math.PI * 0.5f
            };

            var foregroundEffect = new GaussianBlurEffect()
            {
                Source = hueRotationEffect,
                BlurAmount = 10
            };

            foregroundBrush = new CanvasImageBrush(sender, foregroundEffect)
            {
                SourceRectangle = new Rect(0, 0, size, size)
            };
        }

        void CreateClockFace(ICanvasResourceCreator sender)
        {
            const float begin = center - radius - lineLength / 2;
            const float end = center + radius + lineLength / 2;

            using (var builder = new CanvasPathBuilder(sender))
            {

                // Since we have concentric circles that we want to remain filled we want to use Winding to determine the filled region.
                builder.SetFilledRegionDetermination(CanvasFilledRegionDetermination.Winding);

                builder.AddCircleFigure(new Vector2(center), radius);
                builder.AddCircleFigure(new Vector2(center), radius * 0.6f);

                builder.AddOneLineFigure(center, begin, center, begin + lineLength);
                builder.AddOneLineFigure(center, end - lineLength, center, end);
                builder.AddOneLineFigure(begin, center, begin + lineLength, center);
                builder.AddOneLineFigure(end - lineLength, center, end, center);

                using (CanvasGeometry clockFaceGeometry = CanvasGeometry.CreatePath(builder))
                {
                    clockFaceCachedFill = CanvasCachedGeometry.CreateFill(clockFaceGeometry);
                    clockFaceCachedStroke18 = CanvasCachedGeometry.CreateStroke(clockFaceGeometry, 18, timeCircleStrokeStyle);
                    clockFaceCachedStroke16 = CanvasCachedGeometry.CreateStroke(clockFaceGeometry, 16, timeCircleStrokeStyle);
                }
            }
        }

        public void Draw(ICanvasAnimatedControl sender, CanvasTimingInformation timingInformation, CanvasDrawingSession ds)
        {
            ds.DrawCachedGeometry(clockFaceCachedFill, backgroundBrush);

            double fractionSecond;
            int seconds;

            if(sender.IsFixedTimeStep)
            {
                double updatesPerSecond = 1000.0 / sender.TargetElapsedTime.TotalMilliseconds;
                seconds = (int)((timingInformation.UpdateCount / updatesPerSecond) % 10);

                double updates = (double)timingInformation.UpdateCount;
                fractionSecond = (updates / updatesPerSecond) % 1.0;
            }
            else
            {
                double totalMilliseconds = timingInformation.TotalTime.TotalMilliseconds;
                double millisecondsThisIteration = totalMilliseconds % 1000;

                fractionSecond = millisecondsThisIteration / 1000.0f;
                seconds = (int)timingInformation.TotalTime.TotalSeconds % 10;
            }

            hueRotationEffect.Angle = (float)Math.PI * (seconds / 10.0f) * 2.0f;

            using (var timeSegmentGeometry = CreateTimeSegmentGeometry(ds, fractionSecond))
            {
                ds.FillGeometry(timeSegmentGeometry, foregroundBrush);

                DrawSecondsText(ds, new Vector2(center), seconds);

                ds.DrawGeometry(timeSegmentGeometry, Colors.White, 1, hairlineStrokeStyle);
            }


            ds.DrawCachedGeometry(clockFaceCachedStroke18, Colors.White);
            ds.DrawCachedGeometry(clockFaceCachedStroke16, Colors.Black);
        }

        static CanvasGeometry CreateTimeSegmentGeometry(CanvasDrawingSession ds, double fractionSecond)
        {
            double fractionSecondAngle = 2 * Math.PI * fractionSecond;
            double angle = fractionSecondAngle % (2 * Math.PI);

            using (var builder = new CanvasPathBuilder(ds))
            {
                builder.BeginFigure(center, center);
                builder.AddArc(new Vector2(center), radius, radius, (float)Math.PI * 3 / 2, (float)angle);
                builder.EndFigure(CanvasFigureLoop.Closed);

                return CanvasGeometry.CreatePath(builder);
            }
        }

        static void DrawSecondsText(CanvasDrawingSession ds, Vector2 pos, int seconds)
        {
            // The text is drawn via a command list so we can use DrawImage to specify a difference CanvasComposite mode.
            using (var cl = new CanvasCommandList(ds))
            {
                using (var clds = cl.CreateDrawingSession())
                {
                    clds.DrawText(seconds.ToString(), Vector2.Zero, Colors.White, textFormat);
                }

                Rect textBounds = cl.GetBounds(ds);
                var offset = new Vector2((float)textBounds.Left, (float)textBounds.Top);

                ds.DrawImage(cl, pos + offset, cl.GetBounds(ds), 1, CanvasImageInterpolation.NearestNeighbor, CanvasComposite.MaskInvert);
            }
        }

        CanvasCachedGeometry clockFaceCachedFill;
        CanvasCachedGeometry clockFaceCachedStroke18;
        CanvasCachedGeometry clockFaceCachedStroke16;
        HueRotationEffect hueRotationEffect;
        CanvasImageBrush backgroundBrush;
        CanvasImageBrush foregroundBrush;

        const float size = 1000;
        const float center = size / 2;
        const float lineLength = size / 10;
        const float radius = (size / 2) - lineLength;

        static readonly CanvasStrokeStyle timeCircleStrokeStyle = new CanvasStrokeStyle()
        {
            StartCap = CanvasCapStyle.Round,
            EndCap = CanvasCapStyle.Round,
        };

        static readonly CanvasStrokeStyle hairlineStrokeStyle = new CanvasStrokeStyle()
        {
            TransformBehavior = CanvasStrokeTransformBehavior.Hairline
        };

        static readonly CanvasTextFormat textFormat = new CanvasTextFormat()
        {
            FontFamily = "Consolas",
            FontSize = size / 3,
            HorizontalAlignment = CanvasHorizontalAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center
        };
    }

    class UpdatesPerDrawRenderer
    {
        Queue<int> updatesPerDraw = new Queue<int>();
        int updatesThisDraw;
        int drawCount;

        public void Update()
        {
            updatesThisDraw++;
        }

        private CanvasStrokeStyle maxStroke = new CanvasStrokeStyle()
        {
            TransformBehavior = CanvasStrokeTransformBehavior.Hairline,
            CustomDashStyle = new float[] { 5, 20 }
        };

        public void Draw(CanvasAnimatedDrawEventArgs args, CanvasDrawingSession ds, float width, float height)
        {
            drawCount++;

            const int maxEntries = 120;
            updatesPerDraw.Enqueue(updatesThisDraw);
            while (updatesPerDraw.Count > maxEntries)
                updatesPerDraw.Dequeue();

            ds.Antialiasing = CanvasAntialiasing.Aliased;

            var barWidth = width / (float)maxEntries;

            const float heightPerUpdate = 10.0f;
            float barBottom = height * 0.25f;

            int maxUpdates = 0;
            int maxIndex = -1;

            int index = 0;
            foreach (int update in updatesPerDraw)
            {
                float barHeight = update * heightPerUpdate;
                Color color = Colors.Gray;
                if ((Math.Max(0, drawCount - maxEntries) + index) % 60 == 0)
                    color = Colors.White;

                ds.FillRectangle(barWidth * index, height - barHeight - barBottom, barWidth, barHeight + barBottom, color);

                if (update > maxUpdates)
                {
                    maxIndex = index;
                    maxUpdates = update;
                }
                index++;
            }

            if (maxUpdates > 0)
            {
                var y = height - maxUpdates * heightPerUpdate - barBottom;

                ds.DrawLine(0, y, width, y, Colors.White, 1, maxStroke);

                ds.DrawText(
                    maxUpdates.ToString(),
                    0, 
                    height - maxUpdates * heightPerUpdate - barBottom,
                    Colors.White,
                    new CanvasTextFormat()
                    {
                        FontSize = heightPerUpdate * 2,
                        HorizontalAlignment = CanvasHorizontalAlignment.Left,
                        VerticalAlignment = CanvasVerticalAlignment.Bottom
                    });
            }

            using (var textFormat = new CanvasTextFormat()
            {
                FontSize = width * 0.05f,
                HorizontalAlignment = CanvasHorizontalAlignment.Left,
                VerticalAlignment = CanvasVerticalAlignment.Top
            })
            {
                float y = 1;
                ds.DrawText("Updates per Draw", 1, y, Colors.White, textFormat);
                y += textFormat.FontSize * 2;

                textFormat.FontSize *= 0.6f;

                ds.DrawText(string.Format("{0} total updates", args.Timing.UpdateCount), 1, y, Colors.Red, textFormat);
                y += textFormat.FontSize * 1.2f;

                ds.DrawText(string.Format("{0} total draws", drawCount), 1, y, Colors.Green, textFormat);
            }

            updatesThisDraw = 0;
        }
    }

    class TouchPointsRenderer
    {
        Queue<Vector2> points = new Queue<Vector2>();
        const int maxPoints = 100;

        public TouchPointsRenderer(CanvasAnimatedControl animatedControl)
        {
            animatedControl.Input.PointerPressed += OnPointerPressed;
            animatedControl.Input.PointerMoved += OnPointerMoved;
        }

        private void OnPointerPressed(object sender, PointerEventArgs args)
        {
            points.Clear();
        }

        private void OnPointerMoved(object sender, PointerEventArgs args)
        {            
            foreach (var point in args.GetIntermediatePoints())
            {
                if (point.IsInContact)
                {
                    if (points.Count > maxPoints)
                    {
                        points.Dequeue();
                    }

                    points.Enqueue(new Vector2((float)point.Position.X, (float)point.Position.Y));
                }
            }
        }

        public void Draw(CanvasDrawingSession ds)
        {
            int pointerPointIndex = 0;
            Vector2 prev = new Vector2(0, 0);
            const float penRadius = 10;
            foreach (Vector2 p in points)
            {
                if (pointerPointIndex != 0)
                {
                    ds.DrawLine(prev, p, Colors.DarkRed, penRadius * 2);
                }
                ds.FillEllipse(p, penRadius, penRadius, Colors.DarkRed);
                prev = p;
                pointerPointIndex++;
            }

            if (points.Count > 0)
                points.Dequeue();
        }
    }

    static class CanvasPathBuilderExtensions
    {
        public static void AddOneLineFigure(this CanvasPathBuilder builder, float x1, float y1, float x2, float y2)
        {
            builder.BeginFigure(x1, y1);
            builder.AddLine(x2, y2);
            builder.EndFigure(CanvasFigureLoop.Open);
        }

        public static void AddCircleFigure(this CanvasPathBuilder builder, Vector2 center, float radius)
        {
            builder.BeginFigure(center + Vector2.UnitX * radius);
            builder.AddArc(center, radius, radius, 0, (float)Math.PI * 2);
            builder.EndFigure(CanvasFigureLoop.Closed);
        }
    }
}
