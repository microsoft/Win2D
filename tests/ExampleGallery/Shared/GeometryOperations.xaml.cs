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
using System.Linq;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.Numerics;
using System.Threading;

namespace ExampleGallery
{
    public sealed partial class GeometryOperations : UserControl
    {
        CanvasGeometry leftGeometry;

        Matrix3x2 interGeometryTransform;
        CanvasGeometry rightGeometry;

        CanvasGeometry combinedGeometry;
        bool showSourceGeometry;

        float currentDistanceOnContourPath;
        float totalDistanceOnContourPath;
        Vector2 pointOnContourPath;
        Microsoft.Graphics.Canvas.Numerics.Vector2 tangentOnContourPath;

        bool needsToRecreateResources;

        public GeometryOperations()
        {
            this.InitializeComponent();

            LeftGeometryType = GeometryType.Rectangle;
            RightGeometryType = GeometryType.Star;
            WhichCombineType = CanvasGeometryCombine.Union;

            interGeometryTransform = Matrix3x2.CreateTranslation(200, 100);

            CurrentContourTracingAnimation = ContourTracingAnimationOption.None;

            showSourceGeometry = false;

            needsToRecreateResources = true;
        }

        public enum GeometryType
        {
            Rectangle,
            RoundedRectangle,
            Ellipse,
            Star
        }

        public enum FillOrStroke
        {
            Fill,
            Stroke
        }

        public enum ContourTracingAnimationOption
        {
            None,
            Slow,
            Fast
        }

        public List<GeometryType> Geometries { get { return Utils.GetEnumAsList<GeometryType>(); } }
        public List<FillOrStroke> FillOrStrokeOptions { get { return Utils.GetEnumAsList<FillOrStroke>(); } }
        public List<CanvasGeometryCombine> CanvasGeometryCombines { get { return Utils.GetEnumAsList<CanvasGeometryCombine>(); } }
        public List<ContourTracingAnimationOption> ContourTracingAnimationOptions { get { return Utils.GetEnumAsList<ContourTracingAnimationOption>(); } }
        public GeometryType LeftGeometryType { get; set; }
        public GeometryType RightGeometryType { get; set; }
        public FillOrStroke UseFillOrStroke { get; set; }
        public ContourTracingAnimationOption CurrentContourTracingAnimation { get; set; }
        public CanvasGeometryCombine WhichCombineType { get; set; }

        private CanvasGeometry CreateGeometry(ICanvasResourceCreator resourceCreator, GeometryType type)
        {
            switch (type)
            {
                case GeometryType.Rectangle: return CanvasGeometry.CreateRectangle(resourceCreator, 100, 100, 300, 350);
                case GeometryType.RoundedRectangle: return CanvasGeometry.CreateRoundedRectangle(resourceCreator, 80, 80, 400, 400, 100, 100);
                case GeometryType.Ellipse: return CanvasGeometry.CreateEllipse(resourceCreator, 275, 275, 225, 275);
                case GeometryType.Star:
                    {
                        CanvasPathBuilder pathBuilder = new CanvasPathBuilder(resourceCreator);
                        pathBuilder.BeginFigure(new Vector2(190, 190));
                        pathBuilder.AddLine(250, 0);
                        pathBuilder.AddLine(310, 190);
                        pathBuilder.AddLine(500, 200);
                        pathBuilder.AddLine(350, 300);
                        pathBuilder.AddLine(400, 500);
                        pathBuilder.AddLine(250, 390);
                        pathBuilder.AddLine(100, 500);
                        pathBuilder.AddLine(150, 300);
                        pathBuilder.AddLine(0, 200);
                        pathBuilder.EndFigure(CanvasFigureLoop.Closed);
                        return CanvasGeometry.CreatePath(pathBuilder);
                    }
            }
            System.Diagnostics.Debug.Assert(false);
            return null;
        }

        private void Canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            Matrix3x2 displayTransform = Utils.GetDisplayTransform(sender.Size, canvas, 1000, 1000);

            args.DrawingSession.Transform = displayTransform;
            args.DrawingSession.FillGeometry(combinedGeometry, Colors.Blue);

            if (showSourceGeometry)
            {
                args.DrawingSession.Transform = displayTransform;
                args.DrawingSession.DrawGeometry(leftGeometry, Colors.Red, 5.0f);

                args.DrawingSession.Transform = interGeometryTransform * displayTransform;
                args.DrawingSession.DrawGeometry(rightGeometry, Colors.Red, 5.0f);
            }

            if (CurrentContourTracingAnimation != ContourTracingAnimationOption.None)
            {
                args.DrawingSession.Transform = displayTransform;
                args.DrawingSession.FillCircle(pointOnContourPath, 2, Colors.White);

                const float arrowSize = 10.0f;

                Vector2 tangentLeft = new Vector2(
                    -tangentOnContourPath.Y,
                    tangentOnContourPath.X);

                Vector2 tangentRight = new Vector2(
                    tangentOnContourPath.Y,
                    -tangentOnContourPath.X);

                Vector2 bisectorLeft = new Vector2(
                    tangentOnContourPath.X + tangentLeft.X,
                    tangentOnContourPath.Y + tangentLeft.Y);

                Vector2 bisectorRight = new Vector2(
                    tangentOnContourPath.X + tangentRight.X,
                    tangentOnContourPath.Y + tangentRight.Y);

                Vector2 arrowheadFront = new Vector2(
                    pointOnContourPath.X + (tangentOnContourPath.X * arrowSize * 2),
                    pointOnContourPath.Y + (tangentOnContourPath.Y * arrowSize * 2));

                Vector2 arrowheadLeft = new Vector2(
                    arrowheadFront.X - (bisectorLeft.X * arrowSize),
                    arrowheadFront.Y - (bisectorLeft.Y * arrowSize));

                Vector2 arrowheadRight = new Vector2(
                    arrowheadFront.X - (bisectorRight.X * arrowSize),
                    arrowheadFront.Y - (bisectorRight.Y * arrowSize));

                const float strokeWidth = arrowSize / 4.0f;
                args.DrawingSession.DrawLine(pointOnContourPath, arrowheadFront, Colors.White, strokeWidth);
                args.DrawingSession.DrawLine(arrowheadFront, arrowheadLeft, Colors.White, strokeWidth);
                args.DrawingSession.DrawLine(arrowheadFront, arrowheadRight, Colors.White, strokeWidth);
            }

        }

        private void Canvas_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            if (needsToRecreateResources)
            {
                RecreateGeometry(sender);
                needsToRecreateResources = false;
            }

            if (CurrentContourTracingAnimation != ContourTracingAnimationOption.None)
            {
                float animationDistanceThisFrame = CurrentContourTracingAnimation == ContourTracingAnimationOption.Slow ? 1.0f : 20.0f;
                currentDistanceOnContourPath = (currentDistanceOnContourPath + animationDistanceThisFrame) % totalDistanceOnContourPath;

                pointOnContourPath = combinedGeometry.ComputePointOnPath(currentDistanceOnContourPath, Matrix3x2.Identity, 0.25f, out tangentOnContourPath);
            }
        }

        private void RecreateGeometry(ICanvasResourceCreator resourceCreator)
        {
            leftGeometry = CreateGeometry(resourceCreator, LeftGeometryType);
            rightGeometry = CreateGeometry(resourceCreator, RightGeometryType);


            combinedGeometry = leftGeometry.CombineWith(rightGeometry, interGeometryTransform, WhichCombineType);

            if (UseFillOrStroke == FillOrStroke.Stroke)
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = CanvasDashStyle.Dash;
                combinedGeometry = combinedGeometry.Stroke(15.0f, strokeStyle, Matrix3x2.Identity, 0.25f);
            }

            totalDistanceOnContourPath = combinedGeometry.ComputePathLength();
        }

        private void Canvas_CreateResources(CanvasAnimatedControl sender, object args)
        {
            needsToRecreateResources = true;
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            needsToRecreateResources = true;
        }
        void ShowSourceGeometry_Checked(object sender, RoutedEventArgs e)
        {
            showSourceGeometry = true;
        }


        void ShowSourceGeometry_Unchecked(object sender, RoutedEventArgs e)
        {
            showSourceGeometry = false;
        }

    }
}
