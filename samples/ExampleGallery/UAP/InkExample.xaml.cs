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
using System.IO;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Input.Inking;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public sealed partial class InkExample : UserControl
    {
        CanvasRenderTarget renderTarget;

        List<Point> selectionPolylinePoints;

        Rect? selectionBoundingRect;

        InkSynchronizer inkSynchronizer;

        CanvasTextFormat textFormat;

        bool needsClear;

        bool needsInkSurfaceValidation;

        bool needToCreateSizeDepdendentResources;

        bool showTextLabels;

        public enum DryInkRenderingType
        {
            BuiltIn,
            Lines,
            Geometry,
        }

        public List<DryInkRenderingType> DryInkRenderingTypes { get { return Utils.GetEnumAsList<DryInkRenderingType>(); } }
        public DryInkRenderingType SelectedDryInkRenderingType { get; set; }

        // Since this app uses custom drying, it can't use the built-in stroke container on the ink canvas.
        InkManager inkManager = new InkManager();
        List<InkStroke> strokeList = new List<InkStroke>();

        public InkExample()
        {
            this.InitializeComponent();

            inkCanvas.InkPresenter.InputDeviceTypes = Windows.UI.Core.CoreInputDeviceTypes.Mouse | Windows.UI.Core.CoreInputDeviceTypes.Pen | Windows.UI.Core.CoreInputDeviceTypes.Touch;
            
            // By default, pen barrel button or right mouse button is processed for inking
            // Set the configuration to instead allow processing these input on the UI thread
            inkCanvas.InkPresenter.InputProcessingConfiguration.RightDragAction = InkInputRightDragAction.LeaveUnprocessed;

            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed += UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved += UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased += UnprocessedInput_PointerReleased;
            
            inkCanvas.InkPresenter.StrokeInput.StrokeStarted += StrokeInput_StrokeStarted;

            inkCanvas.InkPresenter.StrokesCollected += InkPresenter_StrokesCollected;
            inkCanvas.InkPresenter.StrokesErased += InkPresenter_StrokesErased;

            inkSynchronizer = inkCanvas.InkPresenter.ActivateCustomDrying();

            textFormat = new CanvasTextFormat();

            // Set defaults
            SelectedDryInkRenderingType = DryInkRenderingType.BuiltIn;
            SelectColor(color0);
            showTextLabels = true;

            needToCreateSizeDepdendentResources = true;
        }

        private void InkPresenter_StrokesCollected(InkPresenter sender, InkStrokesCollectedEventArgs args)
        {
            strokeList.AddRange(args.Strokes);
            
            foreach (var s in args.Strokes)
            {
                inkManager.AddStroke(s);
            }

            canvasControl.Invalidate();
        }

        private void StrokeInput_StrokeStarted(InkStrokeInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            ClearSelection();

            canvasControl.Invalidate();
        }

        private void InkPresenter_StrokesErased(InkPresenter sender, InkStrokesErasedEventArgs args)
        {
            var removed = args.Strokes;
            foreach (var s in removed)
            {
                strokeList.Remove(s);
            }

            inkManager = new InkManager();

            foreach (var s in strokeList)
            {
                inkManager.AddStroke(s);
            }

            ClearSelection();

            canvasControl.Invalidate();
        }

        private void UnprocessedInput_PointerPressed(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            selectionPolylinePoints = new List<Point>();
            selectionPolylinePoints.Add(args.CurrentPoint.RawPosition);

            canvasControl.Invalidate();
        }

        private void UnprocessedInput_PointerMoved(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            selectionPolylinePoints.Add(args.CurrentPoint.RawPosition);

            canvasControl.Invalidate();
        }

        private void UnprocessedInput_PointerReleased(InkUnprocessedInput sender, Windows.UI.Core.PointerEventArgs args)
        {
            selectionPolylinePoints.Add(args.CurrentPoint.RawPosition);

            selectionBoundingRect = inkManager.SelectWithPolyLine(selectionPolylinePoints);

            selectionPolylinePoints = null;

            canvasControl.Invalidate();
        }

        private void ClearSelection()
        {
            selectionPolylinePoints = null;
            selectionBoundingRect = null;
        }

        private void CreateSizeDependentResources()
        {
            renderTarget = new CanvasRenderTarget(canvasControl, canvasControl.Size);

            textFormat.FontSize = (float)canvasControl.Size.Width / 10.0f;
        }

        private async Task LoadThumbnailResources(CanvasControl sender)
        {
            var thumbnailFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/InkThumbnailStrokes.bin"));
            using (var stream = await thumbnailFile.OpenReadAsync())
            {
                LoadStrokesFromStream(stream.AsStreamForRead());
            }
        }

        private void canvasControl_CreateResources(CanvasControl sender, Microsoft.Graphics.Canvas.UI.CanvasCreateResourcesEventArgs args)
        {
            CreateSizeDependentResources();

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                args.TrackAsyncAction(LoadThumbnailResources(sender).AsAsyncAction());
            }

            needToCreateSizeDepdendentResources = false;

            if(args.Reason != CanvasCreateResourcesReason.FirstTime)
            {
                needsInkSurfaceValidation = true;
            }
        }

        private void DrawSelectionLasso(CanvasControl sender, CanvasDrawingSession ds)
        {
            if (selectionPolylinePoints == null) return;

            if (selectionPolylinePoints.Count == 0) return;

            CanvasPathBuilder selectionLasso = new CanvasPathBuilder(canvasControl);
            selectionLasso.BeginFigure(selectionPolylinePoints[0].ToVector2());
            for (int i = 1; i < selectionPolylinePoints.Count; ++i)
            {
                selectionLasso.AddLine(selectionPolylinePoints[i].ToVector2());
            }
            selectionLasso.EndFigure(CanvasFigureLoop.Open);

            CanvasGeometry pathGeometry = CanvasGeometry.CreatePath(selectionLasso);
            ds.DrawGeometry(pathGeometry, Colors.Magenta, 5.0f);
        }

        private void DrawSelectionBoundingRect(CanvasDrawingSession ds)
        {
            if (selectionBoundingRect == null) return;
            if ((selectionBoundingRect.Value.Width == 0) || (selectionBoundingRect.Value.Height == 0) || selectionBoundingRect.Value.IsEmpty)
            {
                return;
            }

            ds.DrawRectangle(selectionBoundingRect.Value, Colors.Magenta);
        }

        private void DrawDryInk_LinesMethod(CanvasDrawingSession ds, IReadOnlyList<InkStroke> strokes)
        {
            //
            // This shows off the fact that apps can use the custom drying path
            // to render dry ink using Win2D, and not necessarily 
            // rely on the built-in rendering in CanvasDrawingSession.DrawInk.
            //
            foreach (var stroke in strokes)
            {
                var color = stroke.DrawingAttributes.Color;

                var inkPoints = stroke.GetInkPoints().Select(point => point.Position.ToVector2()).ToList();

                for (int i = 1; i < inkPoints.Count; i++)
                {
                    ds.DrawLine(inkPoints[i - 1], inkPoints[i], color);
                    ds.DrawCircle(inkPoints[i], 3, color);
                }
            }
        }

        private void DrawDryInk_GeometryMethod(CanvasDrawingSession ds, IReadOnlyList<InkStroke> strokes)
        {
            //
            // This converts the ink strokes to geometry, then draws the geometry outline
            // with a dotted stroke style.
            //
            var strokeStyle = new CanvasStrokeStyle { DashStyle = CanvasDashStyle.Dot };

            var strokesGroupedByColor = from stroke in strokes
                                        group stroke by stroke.DrawingAttributes.Color into strokesOfColor
                                        select strokesOfColor;

            foreach (var strokesOfColor in strokesGroupedByColor)
            {
                var geometry = CanvasGeometry.CreateInk(ds, strokesOfColor).Outline();

                ds.DrawGeometry(geometry, strokesOfColor.Key, 1, strokeStyle);
            }
        }

        private void DrawStrokeCollectionToInkSurface(IReadOnlyList<InkStroke> strokes)
        {
            using (CanvasDrawingSession ds = renderTarget.CreateDrawingSession())
            {
                switch (SelectedDryInkRenderingType)
                {
                    case DryInkRenderingType.BuiltIn:
                        ds.DrawInk(strokes);
                        break;

                    case DryInkRenderingType.Lines:
                        DrawDryInk_LinesMethod(ds, strokes);
                        break;

                    case DryInkRenderingType.Geometry:
                        DrawDryInk_GeometryMethod(ds, strokes);
                        break;
                }
            }
        }

        private void DrawBackgroundText(CanvasDrawingSession ds)
        {
            if (showTextLabels && !ThumbnailGenerator.IsDrawingThumbnail)
            {
                textFormat.VerticalAlignment = CanvasVerticalAlignment.Top;
                textFormat.HorizontalAlignment = CanvasHorizontalAlignment.Left;
                ds.DrawText("Dry Ink Background", new Vector2(10, 10), Colors.DarkGray, textFormat);
            }
        }

        private void DrawForegroundText(CanvasDrawingSession ds)
        {
            if (showTextLabels && !ThumbnailGenerator.IsDrawingThumbnail)
            {
                textFormat.VerticalAlignment = CanvasVerticalAlignment.Bottom;
                textFormat.HorizontalAlignment = CanvasHorizontalAlignment.Right;
                ds.DrawText("Dry Ink Foreground", new Vector2((float)canvasControl.Size.Width - 10, (float)canvasControl.Size.Height - 10), Colors.DarkGoldenrod, textFormat);
            }
        }

        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            if(needToCreateSizeDepdendentResources)
            {
                CreateSizeDependentResources();
            }

            if(needsClear || needsInkSurfaceValidation)
            {
                ClearInkSurface();
            }
            if(needsInkSurfaceValidation)
            {
                DrawStrokeCollectionToInkSurface(strokeList);
            }
            needToCreateSizeDepdendentResources = false;
            needsClear = false;
            needsInkSurfaceValidation = false;

            DrawBackgroundText(args.DrawingSession);

            var strokes = inkSynchronizer.BeginDry();

            DrawStrokeCollectionToInkSurface(strokes); // Incremental draw only.

            inkSynchronizer.EndDry();

            args.DrawingSession.DrawImage(renderTarget);

            DrawForegroundText(args.DrawingSession);

            DrawSelectionBoundingRect(args.DrawingSession);

            DrawSelectionLasso(sender, args.DrawingSession);
        }

        const string saveFileName = "savedFile.bin";

        private void DeleteSelected_Clicked(object sender, RoutedEventArgs e)
        {
            inkManager.DeleteSelected();
            strokeList.Clear();

            var strokes = inkManager.GetStrokes();
            strokeList.AddRange(strokes);

            selectionBoundingRect = null;

            needsInkSurfaceValidation = true;

            canvasControl.Invalidate();
        }

        private async void LoadStrokesFromStream(Stream stream)
        {
            await inkManager.LoadAsync(stream.AsInputStream());

            strokeList.Clear();
            strokeList.AddRange(inkManager.GetStrokes());

            needsInkSurfaceValidation = true;
        }

        private async void Load_Clicked(object sender, RoutedEventArgs e)
        {
            try
            {
                using (var stream = await ApplicationData.Current.LocalFolder.OpenStreamForReadAsync(saveFileName))
                {
                    LoadStrokesFromStream(stream);
                }
            }
            catch (FileNotFoundException)
            {
                MessageDialog dialog = new MessageDialog("No saved data was found.");
                await dialog.ShowAsync();
            }
            canvasControl.Invalidate();
        }

        private async void Save_Clicked(object sender, RoutedEventArgs e)
        {
            using (var stream = await ApplicationData.Current.LocalFolder.OpenStreamForWriteAsync(saveFileName, CreationCollisionOption.ReplaceExisting))
            {
                await inkManager.SaveAsync(stream.AsOutputStream());
            }
        }

        void ClearInkSurface()
        {
            using (CanvasDrawingSession ds = renderTarget.CreateDrawingSession())
            {
                ds.Clear(Colors.Transparent);
            }
        }

        private void Clear_Clicked(object sender, RoutedEventArgs e)
        {
            strokeList.Clear();
            inkManager = new InkManager();

            needsClear = true;

            canvasControl.Invalidate();
        }

        void ResetColorSelectors()
        {
            Button[] buttons = { color0, color1, color2, color3, color4, color5, color6, color7 };
            foreach (Button button in buttons)
            {
                button.BorderBrush = null;
                button.BorderThickness = new Thickness(0);
            }
        }

        void SelectColor(Button button)
        {
            ResetColorSelectors();

            button.BorderBrush = new SolidColorBrush(Colors.Red);
            button.BorderThickness = new Thickness(3);

            InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
            drawingAttributes.Color = ((SolidColorBrush)(button.Background)).Color;
            inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
        }

        private void ColorPickerButton_Clicked(object sender, RoutedEventArgs e)
        {
            SelectColor((Button)e.OriginalSource);
        }

        private void StrokeWidth_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
                drawingAttributes.Size = new Size(e.NewValue, drawingAttributes.Size.Height);
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        private void StrokeHeight_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
                drawingAttributes.Size = new Size(drawingAttributes.Size.Width, e.NewValue);
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        private void Rotation_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (inkCanvas != null)
            {
                InkDrawingAttributes drawingAttributes = inkCanvas.InkPresenter.CopyDefaultDrawingAttributes();
                float radians = Utils.DegreesToRadians((float)e.NewValue);
                drawingAttributes.PenTipTransform = Matrix3x2.CreateRotation(radians);
                inkCanvas.InkPresenter.UpdateDefaultDrawingAttributes(drawingAttributes);
            }
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            //
            // Changing the rendering type will invalidate any ink we've 
            // already drawn, so we need to wipe the intermediate, and re-draw
            // the strokes that have been collected so far.
            //

            needsInkSurfaceValidation = true;

            canvasControl.Invalidate();
        }

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            needToCreateSizeDepdendentResources = true;

            needsInkSurfaceValidation = true;

            canvasControl.Invalidate();
        }

        void ShowTextLabels_Checked(object sender, RoutedEventArgs e)
        {
            showTextLabels = true;

            if(canvasControl != null)
                canvasControl.Invalidate();
        }

        void ShowTextLabels_Unchecked(object sender, RoutedEventArgs e)
        {
            showTextLabels = false;

            if (canvasControl != null)
                canvasControl.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;

            // If we don't unregister these events, the control will leak.
            inkCanvas.InkPresenter.UnprocessedInput.PointerPressed -= UnprocessedInput_PointerPressed;
            inkCanvas.InkPresenter.UnprocessedInput.PointerMoved -= UnprocessedInput_PointerMoved;
            inkCanvas.InkPresenter.UnprocessedInput.PointerReleased -= UnprocessedInput_PointerReleased;
            inkCanvas.InkPresenter.StrokeInput.StrokeStarted -= StrokeInput_StrokeStarted;
            inkCanvas.InkPresenter.StrokesCollected -= InkPresenter_StrokesCollected;
            inkCanvas.InkPresenter.StrokesErased -= InkPresenter_StrokesErased;
        }
    }
}
