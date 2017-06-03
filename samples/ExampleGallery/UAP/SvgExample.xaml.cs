// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Svg;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.IO;
using System.Numerics;
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

        class PointerDrag
        {
            Point startLocation;
            Point currentLocation;

            public PointerDrag(Point location)
            {
                startLocation = location;
                currentLocation = location;
            }

            public void UpdateDraggedLocation(Point location)
            {
                currentLocation = location;
            }

            public Point StartLocation { get { return startLocation; } }

            public Point CurrentLocation { get { return currentLocation; } }

            public Rect GetRectangle()
            {
                return new Rect(startLocation, currentLocation);
            }

            public struct Ellipse
            {
                public float CenterX, CenterY, RadiusX, RadiusY;
            }

            public Ellipse GetEllipse()
            {
                var boundingRectangle = GetRectangle();

                Ellipse result = new Ellipse();
                result.CenterX = (float)(boundingRectangle.Right + boundingRectangle.Left) / 2;
                result.CenterY = (float)(boundingRectangle.Bottom + boundingRectangle.Top) / 2;
                result.RadiusX = result.CenterX - (float)(boundingRectangle.Left);
                result.RadiusY = result.CenterY - (float)(boundingRectangle.Top);
                return result;
            }

            public struct Circle
            {
                public Vector2 Center;
                public float Radius;
            }

            public Circle GetCircle()
            {
                Circle result = new Circle();
                result.Center = startLocation.ToVector2();

                var dx = currentLocation.X - startLocation.X;
                var dy = currentLocation.Y - startLocation.Y;
                result.Radius = (float)Math.Sqrt((dx * dx) + (dy * dy));

                return result;
            }
        }
        PointerDrag pointerDrag; // Null if not currently being used

        public enum ShapeType
        {
            Rectangle,
            Ellipse,
            Circle,
            Line,
        }
        public List<ShapeType> Shapes { get { return Utils.GetEnumAsList<ShapeType>(); } }
        public ShapeType CurrentShapeType { get; set; }

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

        void canvasControl_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                LoadSampleFile();
            }
            else
            {
                CreateSomeSimplePlaceholderDocument();
            }
        }
        
        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            Size viewportSize = new Size() { Width = 1000, Height = 1000 };

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
            
            if (pointerDrag != null)
            {
                if (CurrentShapeType == ShapeType.Rectangle)
                {
                    // Show ghost
                    args.DrawingSession.DrawRectangle(pointerDrag.GetRectangle(), Colors.Magenta);
                }
                else if (CurrentShapeType == ShapeType.Ellipse)
                {
                    var ellipse = pointerDrag.GetEllipse();
                    args.DrawingSession.DrawEllipse(ellipse.CenterX, ellipse.CenterY, ellipse.RadiusX, ellipse.RadiusY, Colors.Magenta);
                }
                else if (CurrentShapeType == ShapeType.Circle)
                {
                    var circle = pointerDrag.GetCircle();
                    args.DrawingSession.DrawCircle(circle.Center, circle.Radius, Colors.Magenta);
                    args.DrawingSession.DrawLine(circle.Center, pointerDrag.CurrentLocation.ToVector2(), Colors.Magenta);
                }
                else if (CurrentShapeType == ShapeType.Line)
                {
                    args.DrawingSession.DrawLine(pointerDrag.StartLocation.ToVector2(), pointerDrag.CurrentLocation.ToVector2(), Colors.Magenta);
                }
            }
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canvasControl.Invalidate();
        }

        void CreateSomeSimplePlaceholderDocument()
        {
            svgDocument = new CanvasSvgDocument(canvasControl);

            svgDocument.Root.AppendChild(svgDocument.LoadElementFromXml(
                "<rect stroke=\"#000000\" stroke-width=\"5\" fill=\"#FFFF00\" x=\"20\" y=\"20\" width=\"100\" height=\"100\"/>"));

            svgDocument.Root.AppendChild(svgDocument.LoadElementFromXml(
                "<circle stroke=\"#000000\" stroke-width=\"5\" fill=\"#8B0000\" cx=\"140\" cy=\"140\" r=\"70\"/>"));

            svgDocument.Root.AppendChild(svgDocument.LoadElementFromXml(
                "<ellipse stroke=\"#000000\" stroke-width=\"5\" fill=\"#6495ED\" cx=\"200\" cy=\"200\" rx=\"150\" ry=\"15\"/>"));

            svgDocument.Root.AppendChild(svgDocument.LoadElementFromXml(
                "<line stroke=\"#000000\" stroke-width=\"5\" x1=\"20\" y1=\"20\" x2=\"300\" y2=\"180\"/>"));
        }

        private void NewDocument_Click(object sender, RoutedEventArgs e)
        {
            CreateSomeSimplePlaceholderDocument();
            canvasControl.Invalidate();
        }

        private void Clear_Clicked(object sender, RoutedEventArgs e)
        {
            // Clears everything.
            svgDocument = new CanvasSvgDocument(canvasControl);
            canvasControl.Invalidate();
        }

        private async void Save_Clicked(object sender, RoutedEventArgs e)
        {
            if (svgDocument == null)
                return;

            using (var stream = await ApplicationData.Current.LocalFolder.OpenStreamForWriteAsync(
                "SvgExample.svg", 
                CreationCollisionOption.ReplaceExisting))
            {
                await svgDocument.SaveAsync(stream.AsRandomAccessStream());
            }
        }

        private async void LoadSampleFile()
        {
            var file = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/sample.svg"));
            using (var fileStream = await file.OpenReadAsync())
            {
                svgDocument = await CanvasSvgDocument.LoadAsync(canvasControl, fileStream);
            }
        }

        private void LoadSampleFile_Click(object sender, RoutedEventArgs e)
        {
            LoadSampleFile();

            canvasControl.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
        }

        private void canvasControl_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            pointerDrag = new PointerDrag(e.GetCurrentPoint(canvasControl).Position);
            canvasControl.Invalidate();
        }

        private void canvasControl_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (pointerDrag == null)
                return; // Nothing to do

            pointerDrag.UpdateDraggedLocation(e.GetCurrentPoint(canvasControl).Position);
            canvasControl.Invalidate();
        }

        private void canvasControl_PointerReleased(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (pointerDrag == null)
                return; // Nothing to do

            // Commit the shape into the document
            string xml = null;

            // TODO: Use programmatic attributes once supported.
            Color fillColor = colorPicker.CurrentColor;
            string svgColor = fillColor.ToString().Remove(1, 2);
            string style = $"fill=\"{svgColor}\" stroke=\"#000000\" stroke-width=\"4\"";

            if (CurrentShapeType == ShapeType.Rectangle)
            {
                Rect r = pointerDrag.GetRectangle();
                xml =
                    $"<rect x=\"{ r.Left.ToString() }\" y=\"{ r.Top.ToString() }\" width=\"{ r.Width.ToString() }\" height=\"{ r.Height.ToString() }\" {style} />";
            }
            else if (CurrentShapeType == ShapeType.Ellipse)
            {
                var ellipse = pointerDrag.GetEllipse();
                xml =
                    $"<ellipse cx=\"{ ellipse.CenterX }\" cy=\"{ ellipse.CenterY }\" rx=\"{ ellipse.RadiusX }\" ry=\"{ ellipse.RadiusY }\" {style} />";
            }
            else if (CurrentShapeType == ShapeType.Circle)
            {
                var circle = pointerDrag.GetCircle();
                xml =
                    $"<circle cx=\"{ circle.Center.X }\" cy=\"{ circle.Center.Y }\" r=\"{ circle.Radius }\" {style} />";
            }
            else if (CurrentShapeType == ShapeType.Line)
            {
                var start = pointerDrag.StartLocation.ToVector2();
                var end = pointerDrag.CurrentLocation.ToVector2();
                xml =
                    $"<line x1=\"{ start.X }\" y1=\"{ start.Y }\" x2=\"{ end.X }\" y2=\"{ end.Y }\" {style} />";
            }

            var newChild = svgDocument.LoadElementFromXml(xml);
            svgDocument.Root.AppendChild(newChild);

            pointerDrag = null;
            canvasControl.Invalidate();
        }
    }
}
