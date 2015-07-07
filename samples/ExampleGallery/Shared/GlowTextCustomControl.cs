// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed class GlowTextCustomControl : UserControl
    {
        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        public Color TextColor
        {
            get { return (Color)GetValue(TextColorProperty); }
            set { SetValue(TextColorProperty, value); }
        }

        public double GlowAmount
        {
            get  { return (double)GetValue(GlowAmountProperty); }
            set { SetValue(GlowAmountProperty, value); }
        }

        public double MaxGlowAmount
        {
            get { return (double)GetValue(MaxGlowAmountProperty); }
            set { SetValue(MaxGlowAmountProperty, value); }
        }

        public Color GlowColor
        {
            get { return (Color)GetValue(GlowColorProperty); }
            set { SetValue(GlowColorProperty, value); }
        }

        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register(
                "Text",
                typeof(string),
                typeof(GlowTextCustomControl),
                new PropertyMetadata("", new PropertyChangedCallback(OnPropertyChanged)));

        public static readonly DependencyProperty TextColorProperty =
            DependencyProperty.Register(
                "TextColor",
                typeof(Color),
                typeof(GlowTextCustomControl),
                new PropertyMetadata(Colors.White, new PropertyChangedCallback(OnPropertyChanged)));

        public static readonly DependencyProperty GlowAmountProperty =
            DependencyProperty.Register(
                "GlowAmount",
                typeof(double),
                typeof(GlowTextCustomControl),
                new PropertyMetadata(5.0, new PropertyChangedCallback(OnPropertyChanged)));

        public static readonly DependencyProperty MaxGlowAmountProperty =
            DependencyProperty.Register(
                "MaxGlowAmount",
                typeof(double),
                typeof(GlowTextCustomControl),
                new PropertyMetadata(5.0, new PropertyChangedCallback(OnPropertyChanged)));

        public static readonly DependencyProperty GlowColorProperty =
            DependencyProperty.Register(
                "GlowColor",
                typeof(Color),
                typeof(GlowTextCustomControl),
                new PropertyMetadata(Colors.Green, new PropertyChangedCallback(OnPropertyChanged)));

        private GlowEffectGraph glowEffectGraph = new GlowEffectGraph();
        private CanvasControl canvas;

        public GlowTextCustomControl()
        {
            Loaded += UserControl_Loaded;
            Unloaded += UserControl_Unloaded;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            canvas = new CanvasControl();

            if (ThumbnailGenerator.IsDrawingThumbnail)
                canvas.ClearColor = Colors.Black;

            canvas.Draw += OnDraw;
            Content = canvas;
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            if (canvas != null)
            {
                canvas.RemoveFromVisualTree();
                canvas = null;
            }
        }

        private static void OnPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var instance = d as GlowTextCustomControl;
            if (d == null)
                return;

            if (instance.canvas != null)
            {
                instance.canvas.Invalidate();
                instance.InvalidateMeasure();
            }            
        }

        // This is the amount that we grow the desired size by (to account for the glow)
        private double ExpandAmount { get { return Math.Max(GlowAmount, MaxGlowAmount) * 4; } }

        protected override Size MeasureOverride(Size availableSize)
        {
            // CanvasTextLayout cannot cope with infinite sizes, so we change
            // infinite to some-large-value.
            if (double.IsInfinity(availableSize.Width))
                availableSize.Width = 6000;

            if (double.IsInfinity(availableSize.Height))
                availableSize.Height = 6000;

            var device = CanvasDevice.GetSharedDevice(false);

            var layout = CreateTextLayout(device, availableSize);
            var bounds = layout.LayoutBounds;

            return new Size(Math.Min(availableSize.Width, bounds.Width + ExpandAmount), Math.Min(availableSize.Height, bounds.Height + ExpandAmount));
        }

        private void OnDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DoEffect(args.DrawingSession, sender.Size, (float)GlowAmount);
        }

        private void DoEffect(CanvasDrawingSession ds, Size size, float amount)
        {
            size.Width = size.Width - ExpandAmount;
            size.Height = size.Height - ExpandAmount;

            var offset = (float)(ExpandAmount / 2);           

            using (var textLayout = CreateTextLayout(ds, size))
            using (var textCommandList = new CanvasCommandList(ds))
            {
                using (var textDs = textCommandList.CreateDrawingSession())
                {                     
                    textDs.DrawTextLayout(textLayout, 0, 0, GlowColor);
                }

                glowEffectGraph.Setup(textCommandList, amount);
                ds.DrawImage(glowEffectGraph.Output, offset, offset);

                ds.DrawTextLayout(textLayout, offset, offset, TextColor);
            }
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, Size size)
        {
            var format = new CanvasTextFormat()
            {
                HorizontalAlignment = GetCanvasHorizontalAlignemnt(),
                VerticalAlignment = GetCanvasVerticalAlignment()
            };

            return new CanvasTextLayout(
                resourceCreator,
                Text,
                format,
                (float)size.Width,
                (float)size.Height);
        }

        private CanvasHorizontalAlignment GetCanvasHorizontalAlignemnt()
        {
            switch (HorizontalContentAlignment)
            {
                case HorizontalAlignment.Center:
                    return CanvasHorizontalAlignment.Center;

                case HorizontalAlignment.Left:
                    return CanvasHorizontalAlignment.Left;

                case HorizontalAlignment.Right:
                    return CanvasHorizontalAlignment.Right;

                default:
                    return CanvasHorizontalAlignment.Left;
            }
        }

        private CanvasVerticalAlignment GetCanvasVerticalAlignment()
        {
            switch (VerticalContentAlignment)
            {
                case VerticalAlignment.Center:
                    return CanvasVerticalAlignment.Center;

                case VerticalAlignment.Top:
                    return CanvasVerticalAlignment.Top;

                case VerticalAlignment.Bottom:
                    return CanvasVerticalAlignment.Bottom;

                default:
                    return CanvasVerticalAlignment.Top;
            }
        }
    }

    class GlowEffectGraph
    {
        public ICanvasImage Output
        {
            get
            {
                return blur;
            }
        }

        MorphologyEffect morphology = new MorphologyEffect()
        {
            Mode = MorphologyEffectMode.Dilate,
            Width = 1,
            Height = 1
        };

        GaussianBlurEffect blur = new GaussianBlurEffect()
        {
            BlurAmount = 0,
            BorderMode = EffectBorderMode.Soft
        };

        public GlowEffectGraph()
        {
            blur.Source = morphology;
        }

        public void Setup(ICanvasImage source, float amount)
        {
            morphology.Source = source;

            var halfAmount = Math.Min(amount / 2, 100);
            morphology.Width = (int)Math.Ceiling(halfAmount);
            morphology.Height = (int)Math.Ceiling(halfAmount);
            blur.BlurAmount = halfAmount;
        }
    }
}
