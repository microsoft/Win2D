// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Svg;
using Microsoft.Graphics.Canvas.Text;
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

        bool svgSupported;

        public SvgExample()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
                CurrentEffectType = EffectType.EdgeDetection;

            svgSupported = CanvasSvgDocument.IsSupported(CanvasDevice.GetSharedDevice());

            if (!svgSupported)
            {
                optionsPanel.Visibility = Visibility.Collapsed;
            }
        }

        void canvasControl_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            if (!svgSupported)
                return;

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                LoadSampleFile();
            }
            else
            {
                CreateSomeSimplePlaceholderDocument();
            }
        }

        private void DrawNotSupportedMessage(CanvasDrawingSession ds, Size layoutRectangle)
        {
            ds.DrawText("This version of Windows does not support SVG features, so this example is not available.",
                new Rect(new Point(0, 0), layoutRectangle), Colors.White, new CanvasTextFormat()
                {
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
                    VerticalAlignment = CanvasVerticalAlignment.Center
                });
        }
        
        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            if (!svgSupported)
            {
                args.DrawingSession.Clear(Colors.Black);
                DrawNotSupportedMessage(args.DrawingSession, sender.Size);
                return;
            }

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

            {
                var rect = svgDocument.Root.CreateAndAppendNamedChildElement("rect");
                UseDefaultStroke(rect);
                rect.SetColorAttribute("fill", Color.FromArgb(0xFF, 0xFF, 0xFF, 0x0));
                rect.SetFloatAttribute("x", 20);
                rect.SetFloatAttribute("y", 20);
                rect.SetFloatAttribute("width", 100);
                rect.SetFloatAttribute("height", 100);
            }
            {
                var circle = svgDocument.Root.CreateAndAppendNamedChildElement("circle");
                UseDefaultStroke(circle);
                circle.SetColorAttribute("fill", Color.FromArgb(0xFF, 0x8B, 0x0, 0x0));
                circle.SetFloatAttribute("cx", 140);
                circle.SetFloatAttribute("cy", 140);
                circle.SetFloatAttribute("r", 70);
            }
            {
                var ellipse = svgDocument.Root.CreateAndAppendNamedChildElement("ellipse");
                UseDefaultStroke(ellipse);
                ellipse.SetColorAttribute("fill", Color.FromArgb(0xFF, 0x64, 0x95, 0xED));
                ellipse.SetFloatAttribute("cx", 200);
                ellipse.SetFloatAttribute("cy", 200);
                ellipse.SetFloatAttribute("rx", 150);
                ellipse.SetFloatAttribute("ry", 15);
            }
            {
                var line = svgDocument.Root.CreateAndAppendNamedChildElement("line");
                UseDefaultStroke(line);
                line.SetFloatAttribute("x1", 20);
                line.SetFloatAttribute("y1", 20);
                line.SetFloatAttribute("x2", 300);
                line.SetFloatAttribute("y2", 180);
            }
        }

        private static void UseDefaultStroke(CanvasSvgNamedElement rect)
        {
            rect.SetColorAttribute("stroke", Colors.Black);
            rect.SetFloatAttribute("stroke-width", 5);
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

                canvasControl.Invalidate();
            }
        }

        private void LoadSampleFile_Click(object sender, RoutedEventArgs e)
        {
            LoadSampleFile();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
        }

        private void canvasControl_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (!svgSupported)
                return;

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
            CanvasSvgNamedElement newChild = null;

            if (CurrentShapeType == ShapeType.Rectangle)
            {
                Rect r = pointerDrag.GetRectangle();
                newChild = svgDocument.Root.CreateAndAppendNamedChildElement("rect");
                newChild.SetFloatAttribute("x", (float)r.Left);
                newChild.SetFloatAttribute("y", (float)r.Top);
                newChild.SetFloatAttribute("width", (float)r.Width);
                newChild.SetFloatAttribute("height", (float)r.Height);
            }
            else if (CurrentShapeType == ShapeType.Ellipse)
            {
                var ellipse = pointerDrag.GetEllipse();
                newChild = svgDocument.Root.CreateAndAppendNamedChildElement("ellipse");
                newChild.SetFloatAttribute("cx", ellipse.CenterX);
                newChild.SetFloatAttribute("cy", ellipse.CenterY);
                newChild.SetFloatAttribute("rx", ellipse.RadiusX);
                newChild.SetFloatAttribute("ry", ellipse.RadiusY);
            }
            else if (CurrentShapeType == ShapeType.Circle)
            {
                var circle = pointerDrag.GetCircle();
                newChild = svgDocument.Root.CreateAndAppendNamedChildElement("circle");
                newChild.SetFloatAttribute("cx", circle.Center.X);
                newChild.SetFloatAttribute("cy", circle.Center.Y);
                newChild.SetFloatAttribute("r", circle.Radius);
            }
            else if (CurrentShapeType == ShapeType.Line)
            {
                var start = pointerDrag.StartLocation.ToVector2();
                var end = pointerDrag.CurrentLocation.ToVector2();
                newChild = svgDocument.Root.CreateAndAppendNamedChildElement("line");
                newChild.SetFloatAttribute("x1", start.X);
                newChild.SetFloatAttribute("y1", start.Y);
                newChild.SetFloatAttribute("x2", end.X);
                newChild.SetFloatAttribute("y2", end.Y);
            }
            newChild.SetColorAttribute("fill", colorPicker.CurrentColor);
            newChild.SetColorAttribute("stroke", Colors.Black);
            newChild.SetFloatAttribute("stroke-width", 4.0f);

            pointerDrag = null;
            canvasControl.Invalidate();
        }
    }
}
