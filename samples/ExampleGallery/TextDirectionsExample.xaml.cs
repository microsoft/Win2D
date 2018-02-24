// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

namespace ExampleGallery
{
    public sealed partial class TextDirectionsExample : UserControl, INotifyPropertyChanged
    {
        public TextDirectionsExample()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
                drawingColor = Colors.Indigo;

            DataContext = this;

            Directions = new List<DirectionInfo>();

            foreach (CanvasTextDirection direction in Enum.GetValues(typeof(CanvasTextDirection)))
            {
                Directions.Add(new DirectionInfo(direction));
            }
        }

        Color drawingColor = Colors.White;

        public class DirectionInfo
        {
            public DirectionInfo(CanvasTextDirection direction)
            {
                Direction = direction;
            }

            public CanvasTextDirection Direction { get; set; }

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

        TextDirectionControl.SharedResources sharedResources;

        private void canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            sharedResources = TextDirectionControl.GetSharedResources(sender);

            var ds = args.DrawingSession;

            Rect rect = new Rect(0,0, sender.ActualWidth, sender.ActualHeight);

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                rect.Width = sender.ActualWidth / 2;
            }
            else
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

            var arrow = sharedResources.GetArrow(CurrentDirection);

            var arrowBounds = arrow.ComputeStrokeBounds(4);

            var destSize = new Size(sender.ActualWidth / 2, sender.ActualHeight / 2);
            var destBounds = new Rect(new Point(sender.ActualWidth / 2 - destSize.Width / 2, sender.ActualHeight / 2 - destSize.Height / 2), destSize);
            arrow = arrow.Transform(TextDirectionControl.CalculateTransform(arrowBounds, destBounds));

            ds.DrawGeometry(arrow, drawingColor, 4, TextDirectionControl.ArrowStrokeStyle);
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

        public event PropertyChangedEventHandler PropertyChanged;

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
