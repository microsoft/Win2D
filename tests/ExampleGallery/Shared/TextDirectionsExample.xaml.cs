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
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Numerics;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace ExampleGallery
{
    public sealed partial class TextDirectionsExample : UserControl, INotifyPropertyChanged
    {
        public TextDirectionsExample()
        {
            this.InitializeComponent();

            if (DesignMode.DesignModeEnabled)
            {
                this.Background = new SolidColorBrush(Colors.Black);
            }

            if (ThumbnailGenerator.IsDrawingThumbnail)
                drawingColor = Colors.Indigo;

            DataContext = this;
        }

        Color drawingColor = Colors.White;

        public class DirectionInfo
        {
            public DirectionInfo(CanvasTextDirection direction, CanvasImageSource imageSource)
            {
                Direction = direction;
                ImageSource = imageSource;
            }

            public CanvasTextDirection Direction { get; set; }

            public CanvasImageSource ImageSource { get; set; }

            public string Name { get { return Direction.ToString().Replace("Then", " then "); } }            
        }

        public List<DirectionInfo> Directions { get; set; }

        int currentDirectionIndex;

        public int CurrentDirectionIndex
        {
            get { return currentDirectionIndex; }
            set
            {
                currentDirectionIndex = value;

                if (Directions == null || currentDirectionIndex < 0 || currentDirectionIndex >= Directions.Count)
                    return;

                var currentDirection = Directions[currentDirectionIndex].Direction;

                switch (currentDirection)
                {
                    case CanvasTextDirection.LeftToRightThenTopToBottom:
                        XamlFlowDirection = FlowDirection.LeftToRight;
                        UnsupportedVisibility = Visibility.Collapsed;
                        break;

                    case CanvasTextDirection.RightToLeftThenTopToBottom:
                        XamlFlowDirection = FlowDirection.RightToLeft;
                        UnsupportedVisibility = Visibility.Collapsed;
                        break;

                    case CanvasTextDirection.LeftToRightThenBottomToTop:
                    case CanvasTextDirection.RightToLeftThenBottomToTop:
                    case CanvasTextDirection.TopToBottomThenLeftToRight:
                    case CanvasTextDirection.TopToBottomThenRightToLeft:
                    case CanvasTextDirection.BottomToTopThenLeftToRight:
                    case CanvasTextDirection.BottomToTopThenRightToLeft:
                        XamlFlowDirection = FlowDirection.LeftToRight;
                        UnsupportedVisibility = Visibility.Visible;
                        break;
                }

                canvas.Invalidate();
            }
        }

        private CanvasTextDirection CurrentDirection { get { return Directions[CurrentDirectionIndex].Direction; } }

        FlowDirection xamlFlowDirection;
        public FlowDirection XamlFlowDirection
        {
            get
            {
                return xamlFlowDirection;
            }
            set
            {
                xamlFlowDirection = value;
                if (PropertyChanged != null)
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("XamlFlowDirection"));
            }
        }

        Visibility unsupportedVisibility;
        public Visibility UnsupportedVisibility
        {
            get
            {
                return unsupportedVisibility;
            }
            set
            {
                unsupportedVisibility = value;
                if (PropertyChanged != null)
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs("UnsupportedVisibility"));
            }
        }

        CanvasGeometry arrow;

        private void canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            arrow = MakeDirectionIcon(sender);
            
            var bounds = arrow.ComputeStrokeBounds(3);
            var outline = arrow.Stroke(2).Outline();

            var foregroundBrush = (SolidColorBrush)Application.Current.Resources["ComboBoxForegroundThemeBrush"];
            var color = foregroundBrush.Color;

            Directions = new List<DirectionInfo>();

            foreach (CanvasTextDirection direction in Enum.GetValues(typeof(CanvasTextDirection)))
            {
                var arrowImage = new CanvasImageSource(sender, 64, 64);
                using (var ds = arrowImage.CreateDrawingSession(Colors.Transparent))
                {

                    var directionTransform = GetDirectionTransform(direction);
                    ds.Transform = directionTransform * Matrix3x2.CreateTranslation((float)(32 - bounds.Width / 2), (float)(32 - bounds.Height / 2));

                    ds.DrawGeometry(arrow, color, 1);
                }

                Directions.Add(new DirectionInfo(direction, arrowImage));
            }

            // Poke the property so that the control gets a chance to pick up the new images
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs("Directions"));
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs("CurrentDirectionIndex"));
            }
        }

        private void canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            Rect rect = new Rect(0,0, sender.ActualWidth, sender.ActualHeight);

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                rect.Width = sender.ActualWidth / 2;
            }

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                ds.DrawText("Win2D DrawText", rect, drawingColor,
                    new CanvasTextFormat()
                    {
                        FontSize = 14,
                        HorizontalAlignment = CanvasHorizontalAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Top
                    });

                    DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Left, CanvasVerticalAlignment.Top);
                    DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Right, CanvasVerticalAlignment.Top);
                    DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Left, CanvasVerticalAlignment.Bottom);
                    DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Right, CanvasVerticalAlignment.Bottom);
            }

            var screenCenter = new Size(sender.ActualWidth, sender.ActualHeight).ToVector2() / 2;

            Matrix3x2 directionTransform = GetDirectionTransform(CurrentDirection);

            var arrowBounds = arrow.ComputeStrokeBounds(4);
            var arrowCenter = new Size(arrowBounds.Width, arrowBounds.Height).ToVector2() / 2;

            float scaleAmount = (float)Math.Max(5.0, (rect.Width / arrowBounds.Width) / 4);
            var scaleAndPositionTransform = Matrix3x2.CreateScale(scaleAmount, scaleAmount, arrowCenter) * Matrix3x2.CreateTranslation(screenCenter - arrowCenter);
            ds.Transform = directionTransform * scaleAndPositionTransform;
            ds.DrawGeometry(MakeDirectionIcon(ds), drawingColor, 4, new CanvasStrokeStyle() { TransformBehavior = CanvasStrokeTransformBehavior.Fixed });
        }

        private Matrix3x2 GetDirectionTransform(CanvasTextDirection direction)
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

        void DrawAlignedText(CanvasDrawingSession ds, Rect rect, CanvasHorizontalAlignment horizontalAlignment, CanvasVerticalAlignment verticalAlignment)
        {
            ds.DrawText(string.Format("{0}-{1}\nAligned", verticalAlignment.ToString(), horizontalAlignment.ToString()),
                rect, drawingColor,
                new CanvasTextFormat()
                {
                    FontSize = 20,
                    HorizontalAlignment = horizontalAlignment,
                    VerticalAlignment = verticalAlignment,
                    Direction = CurrentDirection
                });
        }

        CanvasGeometry MakeDirectionIcon(ICanvasResourceCreator creator)
        {
            var builder = new CanvasPathBuilder(creator);
            
            float radius = 3;
            float lineWidth = 20;

            builder.BeginFigure(0, 0);
            builder.AddLine(0, radius * 2);
            builder.EndFigure(CanvasFigureLoop.Open);

            float y = radius;
            builder.BeginFigure(0, y);
            builder.AddArc(new Vector2(lineWidth + radius, y + radius), radius, radius, -(float)Math.PI/2, (float)Math.PI);
            y += radius * 2;
            builder.AddArc(new Vector2(radius, y + radius), radius, radius, -(float)Math.PI/2, -(float)Math.PI);

            y += radius * 2;
            float x = lineWidth * 2 / 3;
            builder.AddLine(x, y);

            builder.EndFigure(CanvasFigureLoop.Open);

            builder.BeginFigure(x - radius, y - radius/3);
            builder.AddLine(x, y);
            builder.AddLine(x - radius, y + radius/3);
            builder.EndFigure(CanvasFigureLoop.Open);

            return CanvasGeometry.CreatePath(builder);
            
        }


        public event PropertyChangedEventHandler PropertyChanged;
    }
}
