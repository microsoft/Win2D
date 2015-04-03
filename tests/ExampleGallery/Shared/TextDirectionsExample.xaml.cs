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
using System.ComponentModel;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace ExampleGallery
{
    public class HumanReadableDirection : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            CanvasTextDirection direction = (CanvasTextDirection)value;

            return direction.ToString().Replace("Then", " then ");
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public sealed partial class TextDirectionsExample : UserControl, INotifyPropertyChanged
    {
        public TextDirectionsExample()
        {
            this.InitializeComponent();

            if (DesignMode.DesignModeEnabled)
            {
                this.Background = new SolidColorBrush(Colors.Black);
            }

            DataContext = this;
        }


        public List<CanvasTextDirection> Directions { get { return Utils.GetEnumAsList<CanvasTextDirection>(); } }
        
        CanvasTextDirection currentDirection;
        public CanvasTextDirection CurrentDirection
        {
            get { return currentDirection; }
            set
            {
                currentDirection = value;

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

        private void CanvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            Rect rect = new Rect(0,0, sender.ActualWidth, sender.ActualHeight);

            ds.DrawText("Win2D DrawText", rect, Colors.White,
                new CanvasTextFormat()
                {
                    FontSize = 14,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
                    VerticalAlignment = CanvasVerticalAlignment.Center
                });

            DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Left, CanvasVerticalAlignment.Top);
            DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Right, CanvasVerticalAlignment.Top);
            DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Left, CanvasVerticalAlignment.Bottom);
            DrawAlignedText(ds, rect, CanvasHorizontalAlignment.Right, CanvasVerticalAlignment.Bottom);
        }

        void DrawAlignedText(CanvasDrawingSession ds, Rect rect, CanvasHorizontalAlignment horizontalAlignment, CanvasVerticalAlignment verticalAlignment)
        {
            ds.DrawText(string.Format("{0}-{1}\nAligned", verticalAlignment.ToString(), horizontalAlignment.ToString()),
                rect, Colors.White,
                new CanvasTextFormat()
                {
                    FontSize = 24,
                    HorizontalAlignment = horizontalAlignment,
                    VerticalAlignment = verticalAlignment,
                    Direction = CurrentDirection
                });
        }


        public event PropertyChangedEventHandler PropertyChanged;
    }
}
