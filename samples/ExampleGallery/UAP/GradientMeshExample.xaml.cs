// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    public sealed partial class GradientMeshExample : UserControl
    {
        float controlPointRadius;
        float controlPointRadiusSquared;

        CanvasGradientMesh gradientMesh;
        
        List<Vector2[]> patchPoints;

        struct SelectedPatchInfo
        {
            public int PatchIndex;
            public int PointIndex;
        }
        SelectedPatchInfo pickedUp;
        
        public class NamedColor
        {
            public NamedColor(string n, Color c) { Name = n;  Color = c; }

            public string Name { get; set; }
            public Color Color { get; set; }

            public override string ToString()
            {
                return Name;
            }
        }

        List<NamedColor> namedColors = new List<NamedColor>
        {
            new NamedColor("LightBlue", Colors.LightBlue),
            new NamedColor("PaleVioletRed", Colors.PaleVioletRed),
            new NamedColor("LightSteelBlue", Colors.LightSteelBlue),
            new NamedColor("LightGoldenrodYellow", Colors.LightGoldenrodYellow),
            new NamedColor("Red", Colors.Red),
            new NamedColor("White", Colors.White),
        };
           
        public List<NamedColor> AllColors { get { return namedColors; } }
        public NamedColor Color00 { get; set; }
        public NamedColor Color03 { get; set; }
        public NamedColor Color30 { get; set; }
        public NamedColor Color33 { get; set; }

        public bool ShowControlLines { get; set; } = true;
        public bool ShowBounds { get; set; } = false;

        public List<CanvasGradientMeshPatchEdge> CanvasGradientMeshPatchEdges { get { return Utils.GetEnumAsList<CanvasGradientMeshPatchEdge>(); } }
        public CanvasGradientMeshPatchEdge Edge00To03 { get; set; } = CanvasGradientMeshPatchEdge.Antialiased;
        public CanvasGradientMeshPatchEdge Edge03To33 { get; set; } = CanvasGradientMeshPatchEdge.Antialiased;
        public CanvasGradientMeshPatchEdge Edge33To30 { get; set; } = CanvasGradientMeshPatchEdge.Antialiased;
        public CanvasGradientMeshPatchEdge Edge30To00 { get; set; } = CanvasGradientMeshPatchEdge.Antialiased;

        public GradientMeshExample()
        {
            this.InitializeComponent();

            patchPoints = new List<Vector2[]>();

            Color00 = namedColors[0];
            Color03 = namedColors[1];
            Color30 = namedColors[2];
            Color33 = namedColors[3];

            pickedUp.PatchIndex = -1;
        }

        void EnsureMesh(ICanvasResourceCreator resourceCreator)
        {
            if (gradientMesh != null) return;

            CanvasGradientMeshPatch[] patchArray = new CanvasGradientMeshPatch[patchPoints.Count];

            for (int i=0; i<patchPoints.Count; ++i)
            {
                CanvasGradientMeshPatch patch;

                var points = patchPoints[i];
                var colors = new Color[] { Color00.Color, Color03.Color, Color30.Color, Color33.Color };
                var edges = new CanvasGradientMeshPatchEdge[] { Edge00To03, Edge03To33, Edge33To30, Edge30To00 };

                if (patchPoints[i].Length == 12)
                {
                    patch = CanvasGradientMesh.CreateCoonsPatch(points, colors, edges);
                }
                else
                {
                    Debug.Assert(patchPoints[i].Length == 16);

                    patch = CanvasGradientMesh.CreateTensorPatch(points, colors, edges);
                }

                patchArray[i] = patch;
            }

            // Gradient meshes are allowed to be zero-sized, so there is no need to 
            // account for zero here.
            gradientMesh = new CanvasGradientMesh(resourceCreator, patchArray);
        }

        void AddSamplePatch()
        {
            float sizeDim = Math.Min((float)canvasControl.Size.Width, (float)canvasControl.Size.Height);
            float scale = 722 / sizeDim;

            // Move everything upwards slightly so it looks better in the thumbnail.
            float yAdjust = -50.0f;

            Vector2[] points = new Vector2[12];
            points[0] = new Vector2(245.9669f * scale, (358.0068f + yAdjust) * scale);
            points[1] = new Vector2(157.9796f * scale, (127.9815f + yAdjust) * scale);
            points[2] = new Vector2(216.6f * scale, (72.2f + yAdjust) * scale);
            points[3] = new Vector2(288.8f * scale, (72.2f + yAdjust) * scale);

            points[11] = new Vector2(84.99683f * scale, (443.9924f + yAdjust) * scale);
            points[4] = new Vector2(288.8f * scale, (144.4f + yAdjust) * scale);
            points[10] = new Vector2(72.2f * scale, (216.6f + yAdjust) * scale);
            points[5] = new Vector2(288.8f * scale, (216.6f + yAdjust) * scale);

            points[9] = new Vector2(72.2f * scale, (288.8f + yAdjust) * scale);
            points[8] = new Vector2(144.4f * scale, (288.8f + yAdjust) * scale);
            points[7] = new Vector2(216.6f * scale, (288.8f + yAdjust) * scale);
            points[6] = new Vector2(292.9843f * scale, (289.9744f + yAdjust) * scale);

            patchPoints.Add(points);
            gradientMesh = null;
            canvasControl.Invalidate();
        }

        void AddDefaultPatch(bool tensor)
        {
            float sizeDim = Math.Min((float)canvasControl.Size.Width, (float)canvasControl.Size.Height);

            Vector2[] pointArray = new Vector2[tensor ? 16 : 12];

            int[] coonsIndices = { 0, 1, 2, 3, 11, 4, 10, 5, 9, 8, 7, 6 };

            float topLeftMargin = (sizeDim / 10) + ((sizeDim / 10) * patchPoints.Count);
            topLeftMargin = topLeftMargin % sizeDim;

            float spacing = sizeDim / 10.0f;
            int count = 0;
            for (int y = 0; y < 4; y++)
            {
                for (int x = 0; x < 4; x++)
                {
                    int index = 0;
                    if (tensor)
                    {
                        index = count;
                    }
                    else
                    {
                        int coordIndex = (y * 4 + x);
                        if (coordIndex == 5 || coordIndex == 6 || coordIndex == 9 || coordIndex == 10) continue;

                        index = coonsIndices[count];
                    }
                    pointArray[index] = new Vector2(topLeftMargin + (x * spacing), topLeftMargin + (y * spacing));

                    count++;
                }
            }

            patchPoints.Add(pointArray);
            gradientMesh = null;
            canvasControl.Invalidate();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            float sizeDim = Math.Min((float)canvasControl.Size.Width, (float)canvasControl.Size.Height);

            controlPointRadius = 10.0f * sizeDim / 722.0f;
            controlPointRadiusSquared = controlPointRadius * controlPointRadius;

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                AddSamplePatch();
            }
            else
            {
                AddDefaultPatch(false);
            }
        }

        void DrawBezier(CanvasDrawingSession drawingSession, Vector2 start, Vector2 ctrl0, Vector2 ctrl1, Vector2 end)
        {
            CanvasPathBuilder pathBuilder = new CanvasPathBuilder(canvasControl);
            pathBuilder.BeginFigure(start);
            pathBuilder.AddCubicBezier(ctrl0, ctrl1, end);
            pathBuilder.EndFigure(CanvasFigureLoop.Open);

            CanvasGeometry geometry = CanvasGeometry.CreatePath(pathBuilder);
            drawingSession.DrawGeometry(geometry, Colors.White, 5.0f);            
        }

        void DrawControlLines(CanvasDrawingSession drawingSession)
        {
            CanvasGradientMeshPatch[] patches = gradientMesh.Patches;

            for (int i=0; i<patchPoints.Count; ++i)
            {
                var pointArray = patchPoints[i];
                foreach (Vector2 point in pointArray)
                {
                    drawingSession.FillCircle(point, 7, Colors.White);
                }

                CanvasGradientMeshPatch patch = patches[i];
                bool showInterior = patchPoints[i].Length == 16;

                DrawBezier(drawingSession, patch.Point00, patch.Point01, patch.Point02, patch.Point03);
                DrawBezier(drawingSession, patch.Point03, patch.Point13, patch.Point23, patch.Point33);
                DrawBezier(drawingSession, patch.Point00, patch.Point10, patch.Point20, patch.Point30);
                DrawBezier(drawingSession, patch.Point30, patch.Point31, patch.Point32, patch.Point33);

                if (showInterior)
                {
                    DrawBezier(drawingSession, patch.Point10, patch.Point11, patch.Point12, patch.Point13);
                    DrawBezier(drawingSession, patch.Point20, patch.Point21, patch.Point22, patch.Point23);

                    DrawBezier(drawingSession, patch.Point01, patch.Point11, patch.Point21, patch.Point31);
                    DrawBezier(drawingSession, patch.Point02, patch.Point12, patch.Point22, patch.Point32);
                }

                drawingSession.DrawLine(patch.Point00, patch.Point01, Colors.White);
                drawingSession.DrawLine(patch.Point02, patch.Point03, Colors.White);

                drawingSession.DrawLine(patch.Point03, patch.Point13, Colors.White);
                drawingSession.DrawLine(patch.Point23, patch.Point33, Colors.White);

                drawingSession.DrawLine(patch.Point33, patch.Point32, Colors.White);
                drawingSession.DrawLine(patch.Point31, patch.Point30, Colors.White);

                drawingSession.DrawLine(patch.Point30, patch.Point20, Colors.White);
                drawingSession.DrawLine(patch.Point10, patch.Point00, Colors.White);

                if (showInterior)
                {
                    drawingSession.DrawLine(patch.Point01, patch.Point11, Colors.White);
                    drawingSession.DrawLine(patch.Point21, patch.Point31, Colors.White);

                    drawingSession.DrawLine(patch.Point02, patch.Point12, Colors.White);
                    drawingSession.DrawLine(patch.Point22, patch.Point32, Colors.White);

                    drawingSession.DrawLine(patch.Point10, patch.Point11, Colors.White);
                    drawingSession.DrawLine(patch.Point12, patch.Point13, Colors.White);

                    drawingSession.DrawLine(patch.Point20, patch.Point21, Colors.White);
                    drawingSession.DrawLine(patch.Point22, patch.Point23, Colors.White);
                }
            }
        }

        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureMesh(sender);

            args.DrawingSession.DrawGradientMesh(gradientMesh);

            if (ShowControlLines)
            {
                DrawControlLines(args.DrawingSession);
            }

            if (ShowBounds)
            {
                Rect bounds = gradientMesh.GetBounds(args.DrawingSession);
                args.DrawingSession.DrawRectangle(bounds, Colors.Magenta);
            }
        }

        private void AddCoonsPatch_Clicked(object sender, RoutedEventArgs e)
        {
            AddDefaultPatch(false);
        }

        private void AddTensorPatch_Clicked(object sender, RoutedEventArgs e)
        {
            AddDefaultPatch(true);
        }

        private void Clear_Clicked(object sender, RoutedEventArgs e)
        {
            patchPoints.Clear();
            gradientMesh = null;
            canvasControl.Invalidate();
        }        

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            gradientMesh = null;
            canvasControl.Invalidate();
        }

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            canvasControl.Invalidate();
        }

        void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            if (canvasControl != null)
                canvasControl.Invalidate();
        }

        void CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            if (canvasControl != null)
                canvasControl.Invalidate();
        }

        private void canvasControl_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
            for (int i = patchPoints.Count-1; i >= 0; i--) // Preference toward the 'top' patches
            {
                for (int j=0; j<patchPoints[i].Length; j++)
                {
                    Vector2 p = e.GetCurrentPoint(canvasControl).Position.ToVector2();
                    float dist = Vector2.DistanceSquared(p, patchPoints[i][j]);
                    if (dist < controlPointRadiusSquared)
                    {
                        pickedUp.PatchIndex = i;
                        pickedUp.PointIndex = j;
                        return;
                    }
                }
            }
            pickedUp.PatchIndex = -1;
        }

        private void canvasControl_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            pickedUp.PatchIndex = -1;
            e.Handled = true;
        }

        private void canvasControl_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            if (pickedUp.PatchIndex >= 0)
            {
                foreach (var point in e.GetIntermediatePoints(canvasControl))
                {
                    if (point.IsInContact)
                    {
                        patchPoints[pickedUp.PatchIndex][pickedUp.PointIndex] = point.Position.ToVector2();
                        gradientMesh = null;
                        break;
                    }
                }
                canvasControl.Invalidate();
                e.Handled = true;
            }
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
        }
    }
}
