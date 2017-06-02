// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Svg;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class SvgExample : UserControl
    {
        CanvasSvgDocument svgDocument;

        public enum EffectType
        {
            None,
            EdgeDetection
        }
        public List<EffectType> Effects { get { return Utils.GetEnumAsList<EffectType>(); } }
        public EffectType CurrentEffectType { get; set; }

        public SvgExample()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
                CurrentEffectType = EffectType.EdgeDetection;
        }

        async Task Canvas_CreateResourcesAsync(CanvasControl sender)
        {
            var thumbnailFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/sample.svg"));
            using (var fileStream = await thumbnailFile.OpenReadAsync())
            {
                svgDocument = await CanvasSvgDocument.LoadAsync(sender, fileStream);
            }
        }

        void canvasControl_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }
        
        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            Size viewportSize = new Size() { Width = 1000, Height = 1000 };

            Matrix3x2 displayTransform = Utils.GetDisplayTransform(sender.Size.ToVector2(), viewportSize.ToVector2());
            args.DrawingSession.Transform = displayTransform;

            if (CurrentEffectType == EffectType.None)
            {
                args.DrawingSession.DrawSvg(svgDocument, viewportSize);
            }
            else if (CurrentEffectType == EffectType.EdgeDetection)
            {
                CanvasCommandList commandList = new CanvasCommandList(sender);

                using (var ds = commandList.CreateDrawingSession())
                {
                    ds.DrawSvg(svgDocument, viewportSize);
                }

                var edgeDetection = new EdgeDetectionEffect
                {
                    Source = commandList,
                    Amount = 1,
                    BlurAmount = 1
                };

                args.DrawingSession.DrawImage(edgeDetection);
            }
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canvasControl.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
        }
    }
}
