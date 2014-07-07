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
using System.Text;
using Windows.UI;
using Windows.Foundation;
using System.Diagnostics;

namespace CsConsumer
{
    class GeometryTestScene1
    {
        //
        // Factored into a separate file for readability.
        // This test renders a number of geometry primitives to exercise stroke styles..
        // 


        public static void DrawGeometryTestScene(CanvasDrawingSession drawingSession, CanvasSolidColorBrush brush, TestSceneRenderingType renderingType)
        {
            TestSceneRenderer ds;
            if(renderingType == TestSceneRenderingType.Default || renderingType == TestSceneRenderingType.Randomized)
            {
                ds = new TestSceneRenderer(drawingSession, TestSceneRenderingType.Default);
            }
            else
            {
                Debug.Assert(renderingType == TestSceneRenderingType.Wireframe);
                ds = new TestSceneRenderer(drawingSession, TestSceneRenderingType.Wireframe);
            }            
            
            ds.Clear(Colors.White);

            Color[] colors = new Color[8];
            float[] dottedDashStyle = new float[] { 0.1f, 2f };
            float[] innerDashStyle = new float[] { 1f, 6f }; 
            CanvasCapStyle dotCapStyle = CanvasCapStyle.Round;
            CanvasDashStyle predefinedDashStyle = CanvasDashStyle.Solid;

            if (renderingType == TestSceneRenderingType.Randomized)
            {
                Random r = new Random();
                for (int i = 0; i < colors.Length; i++)
                {
                    colors[i] = Color.FromArgb(
                                (byte)255,
                                (byte)r.Next(256),
                                (byte)r.Next(256),
                                (byte)r.Next(256));
                }

                dottedDashStyle[0] = (float)(r.Next(100) / 10.0f);
                dottedDashStyle[1] = (float)(r.Next(100) / 10.0f);
                dotCapStyle = (CanvasCapStyle)(r.Next((int)CanvasCapStyle.Triangle));

                innerDashStyle[0] = (float)(r.Next(10) / 10.0f);
                innerDashStyle[1] = (float)(r.Next(10) / 10.0f);

                predefinedDashStyle = (CanvasDashStyle)(r.Next((int)CanvasDashStyle.DashDotDot));
            }
            else
            {
                for (int i = 0; i < colors.Length; i++)
                {
                    colors[i] = Colors.Black;
                }
            }

            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(484.5, 351);
                ellipse.RadiusX = 305f;
                ellipse.RadiusY = 305f;
                brush.Color = colors[0];
                ds.DrawEllipse(ellipse, brush, 6f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(485, 351.5);
                ellipse.RadiusX = 294.5f;
                ellipse.RadiusY = 294.5f;
                brush.Color = colors[0];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(219.51, 144.907);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(842.5, 348.899);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.503f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(145.035, 354.898);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(481.018, 683.883);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(485.518, 23.419);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(222.031, 560.394);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(751.005, 569.394);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.504f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(752.005, 129.915);
                ellipse.RadiusX = 505.99f;
                ellipse.RadiusY = 464.503f;
                brush.Color = colors[2];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(352.5, 471), new Point(398.5, 573), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(407.5, 573), new Point(500.5, 534), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(579.5, 564), new Point(502.5, 538), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(595.5, 568), new Point(631.5, 482), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(703.5, 442), new Point(619.5, 472), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(709.5, 435), new Point(675.5, 354), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(704.5, 272), new Point(681.5, 337), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(698.5, 265), new Point(620.5, 234), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(591.5, 133), new Point(622.5, 198), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(581.5, 129), new Point(507.5, 162), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(404.5, 128), new Point(470.5, 155), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(395.5, 129), new Point(366.5, 191), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(272.5, 259), new Point(348.5, 233), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(270.5, 267), new Point(289.5, 322), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(262.5, 436), new Point(295.5, 363), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(269.5, 445), new Point(345.5, 467), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(259.5, 161), new Point(305.5, 160), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(310.5, 115), new Point(306.5, 162), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(266.5, 160), new Point(305.5, 122), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(191.5, 320), new Point(220.5, 355), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(193.5, 390), new Point(214.5, 361), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(196.5, 333), new Point(195.5, 382), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(456.5, 58), new Point(487.5, 79), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(528.5, 60), new Point(498.5, 88), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(518.5, 62), new Point(467.5, 64), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(667.5, 122), new Point(678.5, 158), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(719.5, 174), new Point(674.5, 171), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(672.5, 134), new Point(711.5, 170), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(777.5, 321), new Point(752.5, 346), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(774.5, 395), new Point(754.5, 366), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(773.5, 330), new Point(772.5, 389), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(720.5, 532), new Point(688.5, 536), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(711.5, 533), new Point(676.5, 569), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(256.5, 534), new Point(283.5, 545), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(316.5, 590), new Point(301.5, 558), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(268.5, 543), new Point(306.5, 581), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(349.5, 468), new Point(489.5, 490), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(630.5, 469), new Point(474.5, 490), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(682.5, 336), new Point(580.5, 476), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(622.5, 189), new Point(653.5, 368), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(677.5, 354), new Point(568.5, 222), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(571.5, 223), new Point(461.5, 152), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(522.5, 157), new Point(399.5, 221), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(407.5, 224), new Point(325.5, 302), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(325.5, 297), new Point(290.5, 368), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(363.5, 179), new Point(341.5, 276), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(323.5, 233), new Point(560.5, 217), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(546.5, 213), new Point(657.5, 243), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(338.5, 294), new Point(345.5, 462), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(284.5, 320), new Point(343.5, 404), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(348.5, 416), new Point(490.5, 531), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(566.5, 486), new Point(495.5, 532), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(645.5, 384), new Point(635.5, 489), brush, 20f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(316, 41.5), new Point(234.5, 104), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(246.5, 93.5), new Point(173, 179.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(317, 661.5), new Point(235.5, 599), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(247.5, 609.5), new Point(174, 523.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(653, 41.5), new Point(734.5, 104), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(722.5, 93.5), new Point(796, 179.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(652, 661.5), new Point(733.5, 599), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(721.5, 609.5), new Point(795, 523.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(145, 260), new Point(134, 348.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(132, 341), new Point(143.5, 429.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(824, 260), new Point(835, 348.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(837, 341), new Point(825.5, 429.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(74.61, -5.391), new Point(74.61, 736), brush, 150f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(874.609, -6.391), new Point(874.609, 735), brush, 150f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(397.75, 691.25), new Point(486.25, 702.25), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(478.75, 704.25), new Point(567.25, 692.75), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(394.75, 11), new Point(483.25, 0), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(475.75, -2), new Point(564.25, 9.5), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(268.5, -18), new Point(254.5, 42), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(144.5, 101), new Point(206.5, 94), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(140.5, 603), new Point(214.5, 606), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(244.5, 646), new Point(287.5, 757), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(721.5, 639), new Point(676.5, 772), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(691.5, -19), new Point(717.5, 57), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(768.5, 102), new Point(844.5, 112), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(927.5, 131), new Point(1007.5, 167), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(988.5, 111), new Point(940.5, 241), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(944.5, 491), new Point(999.5, 667), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(918.5, 575), new Point(1022.5, 520), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(757.5, 601), new Point(853.5, 582), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(459.5, 644), new Point(481.5, 622), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(533.5, 641), new Point(506.5, 620), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(471.5, 642), new Point(520.5, 640), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(669.5, 580), new Point(670.5, 557), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(464.5, 43), new Point(517.5, 45), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(679.833, 113.334), new Point(721.167, 157.334), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(789.167, 333), new Point(789.167, 378), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(463.167, 654.664), new Point(525.167, 652.664), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(251.833, 548.665), new Point(298.5, 596.665), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(180.5, 329), new Point(181.833, 380.5), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(253.5, 149.667), new Point(297.5, 109), brush, 20f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(241.205, 551.138);
                ellipse.RadiusX = 482.285f;
                ellipse.RadiusY = 442.742f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(744.125, 139.766);
                ellipse.RadiusX = 487.856f;
                ellipse.RadiusY = 447.857f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(746.109, 138.625);
                ellipse.RadiusX = 471.357f;
                ellipse.RadiusY = 432.711f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(243.985, 547);
                ellipse.RadiusX = 457.63f;
                ellipse.RadiusY = 420.108f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(297.5, 267.333);
                ellipse.RadiusX = 11.333f;
                ellipse.RadiusY = 11.333f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(413.167, 153.667);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(384.5, 239.334);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(714.079, 590.08), new Point(703.5, 785), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(726.079, 578.58), new Point(865.167, 584), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(831.573, 574.08), new Point(1016.064, 508.083), brush, 40f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(293.167, 110.667), new Point(291.833, 152), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(251.833, 149.333), new Point(289.167, 149.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(268.5, 264), new Point(311.833, 246), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(311.833, 246), new Point(321.167, 276), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(372.5, 224), new Point(367.167, 260.667), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(373.167, 226.667), new Point(403.167, 223.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(405.833, 131.333), new Point(391.167, 162.667), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(391.167, 163.333), new Point(416.5, 173.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(297.167, 269.333), new Point(385.167, 237.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(421.167, 136.667), new Point(380.5, 240), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(381.167, 239.333), new Point(379.833, 256), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(381.833, 237.333), new Point(395.167, 238), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(301.167, 284.667), new Point(294.5, 263.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(296.5, 270.667), new Point(281.167, 279.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(397.167, 147.333), new Point(425.167, 161.333), brush, 5f);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(448.5, 159.667), new Point(516.5, 190.333), brush, 40f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(301.167, 306), new Point(325.833, 360.667), brush, 40f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(252.5, 118), new Point(124.5, 124.667), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(267.167, 105.333), new Point(272.5, 44), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(274.5, 49.333), new Point(295.167, -20), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(266.5, 118), new Point(283.167, 118.667), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(270.5, 126.667), new Point(260.5, 126), brush, 12f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(254.167, 105.001);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[5];
                ds.DrawEllipse(ellipse, brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(298, 281), new Point(391.5, 249.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(429.5, 144), new Point(390.5, 251.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(377.667, 225.834), new Point(363, 220.001), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(363, 220.001), new Point(347.833, 220.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(347.667, 220.168), new Point(332.667, 225.334), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(332.833, 225.334), new Point(320.167, 237.501), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(320.167, 237.501), new Point(319.833, 227.834), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(320, 228.334), new Point(313.667, 232.501), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(313.5, 232.501), new Point(316.167, 223.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(316.167, 223.001), new Point(323.167, 215.001), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(316.667, 221.501), new Point(308.333, 227.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(308.333, 227.834), new Point(308.333, 221.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(308.333, 221.001), new Point(303.333, 225.001), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(303.167, 225.001), new Point(305.167, 219.834), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(305.167, 219.668), new Point(301, 214.501), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(301, 214.501), new Point(308.167, 213.834), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(308.167, 213.668), new Point(301.833, 206.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(301.833, 206.668), new Point(310.5, 208.501), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(310.333, 208.501), new Point(310.667, 203.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(310.667, 203.501), new Point(339.167, 205.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(339.333, 205.168), new Point(336.333, 199.834), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(336.333, 199.834), new Point(292.333, 173.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(292.167, 173.168), new Point(296.167, 173.501), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(296.167, 173.334), new Point(289.667, 167.334), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(289.667, 167.334), new Point(299.667, 168.334), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(300.333, 168.168), new Point(290.333, 149.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(290.333, 149.168), new Point(293.5, 149.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(293.667, 149.168), new Point(293.167, 143.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(293.167, 143.002), new Point(297.5, 149.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(297.667, 149.168), new Point(314, 157.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(314, 157.502), new Point(313.833, 149.502), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(313.833, 149.502), new Point(317, 152.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(317, 152.668), new Point(317.167, 144.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(317.167, 144.002), new Point(343, 184.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(343, 184.502), new Point(355.5, 201.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(355.5, 201.668), new Point(360.667, 205.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(360.833, 205.168), new Point(356, 180.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(356.167, 180.168), new Point(354.167, 156.835), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(354.167, 156.668), new Point(360, 159.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(360.167, 159.168), new Point(364.167, 150.335), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(364.167, 150.168), new Point(368.5, 163.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(368.333, 163.168), new Point(372.333, 143.335), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(372.333, 143.502), new Point(377.333, 149.502), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(377.333, 149.668), new Point(380.167, 146.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(380.333, 146.168), new Point(382.5, 161.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(382.5, 161.502), new Point(384.5, 165.835), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(384.5, 165.835), new Point(389.5, 170.502), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(389.5, 170.502), new Point(377, 174.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(377.167, 174.668), new Point(366.667, 182.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(366.5, 182.168), new Point(379.5, 177.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(379.333, 177.168), new Point(384, 176.835), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(384, 176.835), new Point(376.833, 182.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(376.667, 182.335), new Point(371.5, 187.835), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(372, 187.668), new Point(375.333, 186.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(375.5, 186.168), new Point(373.667, 198.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(374.5, 196.335), new Point(372.667, 209.502), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(372.5, 209.502), new Point(376.167, 222.835), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(362, 218.502), new Point(344.667, 215.168), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(344.833, 215.335), new Point(327.5, 213.835), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(313.667, 169.335), new Point(335.5, 186.835), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(335.333, 186.835), new Point(350.333, 208.668), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(350.5, 208.835), new Point(354.167, 216.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(327.833, 179.668), new Point(341.5, 190.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(341.333, 190.002), new Point(355.167, 211.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(355.167, 211.502), new Point(359, 217.668), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(367.667, 175.668), new Point(365.833, 186.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(365.833, 186.502), new Point(366.5, 209.168), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(366.5, 209.335), new Point(366.5, 218.168), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(367, 180.335), new Point(369, 196.835), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(369, 196.835), new Point(368.333, 212.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(368.333, 212.002), new Point(371.667, 223.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(293.833, 149.668), new Point(304.167, 169.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(304, 169.002), new Point(297.667, 171.502), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(297.5, 171.502), new Point(302.5, 175.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(302.5, 175.502), new Point(336.333, 195.835), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(337, 195.335), new Point(351.833, 215.835), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(306, 212.835), new Point(317.333, 207.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(317.5, 207.668), new Point(347.333, 209.335), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(306.167, 219.502), new Point(321.833, 207.335), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(345.333, 190.502), new Point(364.333, 217.835), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(360.833, 204.668), new Point(371.667, 222.835), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(361, 200.335), new Point(375, 219.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(301.667, 175.336), new Point(324, 153.503), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(313, 157.336), new Point(303.333, 167.503), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(307.833, 154.836), new Point(301, 163.503), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(305.667, 161.503), new Point(302, 165.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(305, 153.669), new Point(299.5, 159.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(303.167, 151.836), new Point(298, 157.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(304, 169.336), new Point(299.833, 174.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(325.167, 157.169), new Point(305.167, 178.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(328.667, 162.503), new Point(309.833, 180.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(330.667, 165.836), new Point(313.833, 182.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(317.667, 184.836), new Point(332.833, 171.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(335.5, 174.669), new Point(321.333, 188.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(338.5, 177.336), new Point(326.833, 190.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(340.167, 181.503), new Point(331.333, 192.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(324.333, 209.003), new Point(308.167, 224.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(331.667, 210.003), new Point(314.833, 228.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(320, 228.669), new Point(337.333, 208.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(339.5, 209.169), new Point(324.333, 232.503), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(343.333, 209.003), new Point(337.333, 223.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(346.167, 210.003), new Point(343.5, 221.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(354.5, 168.836), new Point(366.5, 155.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(359.833, 158.503), new Point(353.833, 165.169), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(367.5, 160.336), new Point(356, 173.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(378, 148.503), new Point(369.667, 158.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(375.833, 146.836), new Point(370.833, 152.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(380.5, 153.836), new Point(356.833, 180.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(381, 149.003), new Point(376.167, 154.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(382, 158.669), new Point(357.667, 185.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(385.167, 163.503), new Point(372.667, 178.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(385.333, 167.503), new Point(380.167, 174.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(373.667, 174.003), new Point(358.333, 191.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(378.333, 178.169), new Point(359, 195.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(374, 188.169), new Point(360.667, 199.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(374, 193.503), new Point(362.667, 202.503), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(374.5, 196.669), new Point(367, 203.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(374.5, 200.836), new Point(368.167, 206.836), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(285, 157.169), new Point(285.5, 177.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(290.333, 179.836), new Point(284.667, 198.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(297.5, 182.669), new Point(292.333, 203.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(287.5, 201.336), new Point(289.833, 222.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(294.167, 204.669), new Point(297, 223.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(291.333, 224.003), new Point(294.5, 243.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(297.5, 226.169), new Point(300.167, 245.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(304.5, 231.503), new Point(316.333, 243.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(304.333, 187.003), new Point(299.5, 205.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(307.5, 199.003), new Point(321.167, 198.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(309, 192.669), new Point(323.333, 192.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(322, 198.669), new Point(333, 201.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(301.833, 146.336), new Point(310.667, 132.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(308.333, 150.503), new Point(317, 136.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(297, 140.836), new Point(300.167, 131.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(327.833, 136.503), new Point(319.333, 148.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(332.667, 140.503), new Point(326.833, 159.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(336.5, 147.503), new Point(336.833, 164.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(338.833, 140.503), new Point(347.833, 159.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(345.667, 159.003), new Point(336.667, 174.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(348.333, 142.169), new Point(352.333, 162.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(360.167, 145.336), new Point(354.667, 157.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(350.667, 163.003), new Point(350.167, 181.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(345.5, 170.169), new Point(343.5, 183.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(348.167, 184.336), new Point(354.333, 194.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(347.5, 226.669), new Point(346.333, 246.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(354.5, 225.003), new Point(357.167, 235.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(365.333, 238.169), new Point(350.667, 245.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(361.833, 225.169), new Point(368.333, 232.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(341.167, 234.336), new Point(329.833, 241.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(341.5, 226.836), new Point(328.5, 234.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(326.5, 243.336), new Point(345.333, 246.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(335, 249.669), new Point(321.667, 255.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(325.667, 248.336), new Point(314.667, 247.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(379.333, 219.503), new Point(382.333, 203.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(378.333, 200.503), new Point(388.167, 195.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(387.5, 190.003), new Point(375.833, 192.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(389.167, 183.003), new Point(395.333, 193.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(391.167, 176.003), new Point(400.167, 185.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(395, 169.336), new Point(401.333, 178.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(384.833, 154.169), new Point(389.333, 164.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(375.833, 208.336), new Point(385.667, 211.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(388.667, 202.169), new Point(388.667, 214.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(390.667, 194.503), new Point(393.5, 201.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(279.333, 157.003), new Point(280.167, 178.503), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(281.5, 183.669), new Point(283.833, 197.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(283.667, 196.669), new Point(286.667, 205.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(287.5, 218.336), new Point(289.333, 225.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(291.667, 236.169), new Point(294.333, 247.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(300.333, 246.003), new Point(309.333, 239.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                ds.DrawLine(new Point(293.667, 162.336), new Point(287.667, 158.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(354.5, 172.5), new Point(353.833, 182.167), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(356.833, 182.333), new Point(355.167, 190.167), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(358.333, 191.167), new Point(358.5, 202), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(321, 145.667), new Point(323.667, 152.667), brush, 4f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(736.795, 558.805);
                ellipse.RadiusX = 482.285f;
                ellipse.RadiusY = 442.742f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(227.875, 150.432);
                ellipse.RadiusX = 487.856f;
                ellipse.RadiusY = 447.857f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(226.892, 151.291);
                ellipse.RadiusX = 471.357f;
                ellipse.RadiusY = 432.711f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(732.015, 554.666);
                ellipse.RadiusX = 457.63f;
                ellipse.RadiusY = 420.109f;
                brush.Color = colors[3];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(ellipse, brush, 6f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(574.833, 156.833);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(601.5, 245);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(463.91, 192.267), new Point(333.417, 336.259), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(270.003, 599.508), new Point(281.833, 745), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(252.5, 590.5), new Point(112.007, 595.5), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(141.427, 588.746), new Point(-43.064, 522.75), brush, 40f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(718.167, 156), new Point(680.833, 156), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(583.167, 137), new Point(597.833, 168.333), brush, 5f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(571.833, 151.333), new Point(612.5, 254.667), brush, 5f);
            }
            {
                brush.Color = Colors.White;
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(671.833, 320.667), new Point(647.167, 375.333), brush, 40f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(719.5, 130.667), new Point(847.5, 137.333), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(705.833, 116), new Point(700.5, 54.667), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(698.5, 64), new Point(677.833, -5.333), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(701.5, 138.333), new Point(711.5, 137.667), brush, 12f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(716.833, 115.667);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[5];
                ds.DrawEllipse(ellipse, brush, 5f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(595.333, 240.501), new Point(610.001, 234.668), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(604.667, 155.835), new Point(602.501, 171.335), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(605.667, 186.835), new Point(601.001, 186.502), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(604.667, 226.668), new Point(601.333, 237.668), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(645.167, 151.169), new Point(653.667, 163.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(600.167, 163.836), new Point(595.667, 174.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(692.667, 171.669), new Point(691.833, 193.169), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(689.501, 198.336), new Point(687.167, 211.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(689.333, 211.336), new Point(686.333, 219.836), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(685.501, 233.002), new Point(683.667, 240.502), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(681.333, 250.836), new Point(678.667, 262.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(656, 153.5), new Point(633, 201.5), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(629.5, 157.002), new Point(630.5, 174.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(630.5, 174.669), new Point(628.5, 190.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(628.5, 190.669), new Point(623.834, 209.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(628.167, 154.669), new Point(620.167, 167.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(606.167, 155.669), new Point(619.834, 164.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(663.5, 144.669), new Point(658.167, 155.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(601.5, 173.002), new Point(593.834, 176.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(597.5, 176.669), new Point(606.167, 187.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(606.167, 187.669), new Point(609.167, 200.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(609.167, 200.336), new Point(611.834, 217.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(614.5, 201.669), new Point(610.834, 230.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(612.834, 228.002), new Point(629.834, 228.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(657.834, 224.336), new Point(634.5, 223.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(648.834, 220.002), new Point(622.167, 226.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(615.167, 176.669), new Point(618.167, 197.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(620.834, 187.336), new Point(616.167, 215.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(657.5, 180.336), new Point(637.167, 206.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(668.834, 172.002), new Point(646.167, 199.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(663.834, 144.669), new Point(667.167, 161.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(682.167, 153.336), new Point(668.167, 161.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(684.834, 158.336), new Point(677.834, 171.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(687.167, 166.002), new Point(674.167, 175.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(675.834, 175.002), new Point(686.167, 174.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(690.834, 174.002), new Point(682.5, 178.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(691.167, 179.002), new Point(673.834, 188.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(682.834, 187.669), new Point(648.5, 199.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(657.5, 198.002), new Point(648.834, 211.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(674.167, 208.669), new Point(648.5, 210.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(667.834, 209.669), new Point(680.834, 214.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(681.5, 214.336), new Point(670.834, 218.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(671.167, 218.669), new Point(684.167, 226.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(678.5, 225.336), new Point(684.5, 236.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(684.167, 236.002), new Point(668.167, 239.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(664.834, 236.002), new Point(664.834, 248.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(664.167, 247.669), new Point(659.167, 247.336), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(651.5, 238.336), new Point(660.5, 254.669), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(652.834, 237.669), new Point(622.5, 230.002), brush, 3f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(632.167, 195.336), new Point(616.167, 226.002), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(639.834, 199.669), new Point(620.5, 225.669), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(653.167, 200.669), new Point(625.167, 222.669), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(671.5, 252.336), new Point(698.834, 265.002), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(669.834, 252.669), new Point(656.834, 295.002), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(611.5, 246.002), new Point(663.167, 265.336), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(665.167, 265.669), new Point(696.167, 282.002), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(603.167, 256.002), new Point(660.5, 278.669), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(660.5, 278.669), new Point(690.5, 295.669), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(595.834, 166.336), new Point(565.167, 175.669), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(554.5, 151.002), new Point(567.5, 178.336), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(660.5, 154.669), new Point(668.167, 160.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(657.167, 157.002), new Point(678.167, 175.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(669.167, 160.002), new Point(679.167, 170.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(673.167, 158.336), new Point(681.5, 165.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(676.834, 157.336), new Point(682.167, 163.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(653.5, 160.002), new Point(682.167, 184.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(650.167, 162.336), new Point(676.5, 185.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(648.834, 166.669), new Point(672.5, 188.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(647.834, 171.669), new Point(669.834, 191.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(644.834, 175.336), new Point(665.167, 192.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(644.167, 179.336), new Point(659.5, 194.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(641.5, 184.002), new Point(657.5, 197.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(639.167, 188.336), new Point(653.5, 200.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(638.167, 192.002), new Point(647.5, 198.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(623.167, 162.669), new Point(630.167, 168.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(621.834, 166.002), new Point(628.834, 172.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(604.834, 158.669), new Point(629.167, 176.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(603.834, 160.669), new Point(629.167, 182.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(604.834, 165.669), new Point(629.5, 186.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(602.5, 169.002), new Point(628.167, 191.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(600.834, 172.336), new Point(626.5, 195.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(598.834, 175.336), new Point(626.834, 198.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(606.5, 188.336), new Point(626.167, 203.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(606.834, 194.669), new Point(626.167, 207.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(653.167, 209.336), new Point(682.834, 234.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(660.167, 211.002), new Point(678.834, 227.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(665.167, 211.002), new Point(673.5, 218.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(650.834, 211.336), new Point(677.834, 237.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(643.167, 210.002), new Point(674.167, 239.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(639.5, 212.002), new Point(668.167, 240.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(634.167, 214.336), new Point(663.834, 240.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(633.5, 218.669), new Point(663.834, 246.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(627.834, 218.336), new Point(645.5, 235.336), brush, 2f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(557.5, 164.333), new Point(523.833, 179.333), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(524.833, 168), new Point(466.833, 204.667), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(674.5, 289), new Point(662.833, 311.333), brush, 40f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(567.167, 177.667), new Point(585.833, 228), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(586.5, 227.667), new Point(589.5, 257), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(589.167, 257.333), new Point(621.167, 262.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(585.833, 227.667), new Point(611.167, 229.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(610.167, 229.667), new Point(621.5, 263.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(584.833, 244.667), new Point(564.5, 245.667), brush, 20f);
            }
            {
                brush.Color = colors[4];
                ds.DrawLine(new Point(681.833, 121.667), new Point(682.167, 157), brush, 5f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(645.25, 157.75), new Point(641, 174), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(634, 157), new Point(636, 168.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(639, 154.25), new Point(644.5, 143), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(650.25, 149), new Point(661.25, 144), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(669.25, 146), new Point(679.75, 145), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(670.25, 152.75), new Point(678.5, 150.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(689.75, 166), new Point(694, 160), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(683, 196.25), new Point(682.75, 208.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(685.25, 192.5), new Point(688.25, 184.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(678.75, 192.5), new Point(677.5, 200.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(673, 200.75), new Point(663.25, 206), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(673, 195.25), new Point(662.75, 200), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(682.75, 210.5), new Point(683.25, 222), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(674.75, 241.75), new Point(682.75, 247.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(668.5, 244.25), new Point(678, 249.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(664.75, 261.75), new Point(657, 256.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(655, 251.5), new Point(645.5, 248), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(644.75, 243.25), new Point(633, 241.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(646, 253.75), new Point(656.25, 257.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(641.75, 251), new Point(632.25, 246.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(629.75, 240.75), new Point(618, 233.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(629.75, 248.25), new Point(619.75, 241.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(607, 203.25), new Point(606.5, 215.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(604.5, 194), new Point(597.25, 202), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(599.5, 189.75), new Point(592.5, 196.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(597.5, 183.75), new Point(589.5, 188.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(585.5, 174.5), new Point(589.5, 185.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(590.5, 171.25), new Point(592.75, 180.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(611.5, 154.75), new Point(621.25, 158), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(618.25, 151.75), new Point(628.25, 153.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(631.5, 149.25), new Point(639.5, 150.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(635.75, 170), new Point(635, 184), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(629.75, 234.75), new Point(641.25, 239.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(600.75, 215), new Point(610, 220), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(595.75, 203), new Point(604.25, 211), brush, 4f, strokeStyle);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(525.25, 202.25), new Point(570.25, 249), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(602.75, 279), new Point(637.5, 320), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(607.75, 263.25), new Point(604, 289.75), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(566, 253), new Point(597, 285.75), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(692.75, 127.5), new Point(703.25, 127), brush, 12f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(618.5, 228.667), new Point(611.833, 238.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(619.5, 206), new Point(612.5, 227.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(647.833, 184), new Point(639.833, 211), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(646.833, 234.333), new Point(625.5, 227), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(568.167, 143.667), new Point(575.5, 157.667), brush, 5f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(294.5, 437);
                ellipse.RadiusX = 11.333f;
                ellipse.RadiusY = 11.333f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(600.834, 463.333);
                ellipse.RadiusX = 8.667f;
                ellipse.RadiusY = 8.667f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(686.167, 432.332);
                ellipse.RadiusX = 11.333f;
                ellipse.RadiusY = 11.333f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(381.501, 465.666);
                ellipse.RadiusX = 8.667f;
                ellipse.RadiusY = 8.667f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(283.167, 412.002), new Point(313.167, 428.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(312.5, 428.669), new Point(301.833, 459.336), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(302.5, 459.336), new Point(271.167, 445.336), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(363.167, 448.336), new Point(393.167, 453.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(393.167, 454.669), new Point(399.167, 482.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.833, 481.336), new Point(365.833, 476.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(365.833, 476.002), new Point(365.167, 448.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(311.167, 428.002), new Point(363.833, 448.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(308.5, 442.002), new Point(365.167, 464.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(251.167, 550.669), new Point(291.833, 555.336), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(291.833, 554.669), new Point(297.167, 594.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(390.5, 536.669), new Point(405.167, 572.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(390.5, 538.002), new Point(421.167, 530.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(419.833, 530.002), new Point(433.833, 561.336), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(399.167, 482.669), new Point(416.5, 528.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(387.167, 481.336), new Point(406.5, 534.002), brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(413.167, 549.667);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(364, 468), new Point(338, 487), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(363, 471), new Point(331.5, 507.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(337.5, 500), new Point(293, 527), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(293, 527.5), new Point(278.5, 535), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(313, 518.5), new Point(284.5, 540), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(330, 508.5), new Point(315, 530), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(364, 476.5), new Point(336.5, 510.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(363.5, 478), new Point(317.5, 545), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(312.5, 542), new Point(294, 553.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(300.5, 536), new Point(293, 551.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(302.5, 532.5), new Point(290, 540), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(310, 545), new Point(307.5, 556), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(316.5, 542.5), new Point(304.5, 566), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(317, 546.5), new Point(311.5, 555), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(350, 484.5), new Point(322.5, 500), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(323.5, 497.5), new Point(304.5, 500.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(306.5, 499), new Point(290.5, 496.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(305, 487.5), new Point(293.5, 494.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(304, 485.5), new Point(290.5, 474), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(308, 467.5), new Point(294.5, 471.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(308.5, 456), new Point(309, 468), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(309, 459.5), new Point(333.5, 474.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(352.5, 473.5), new Point(332, 473.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(345, 481.5), new Point(328, 483.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(340.5, 485.5), new Point(311.5, 483), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(361, 485), new Point(356.5, 511), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(365, 477), new Point(364.5, 492.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(366.5, 477), new Point(370, 511.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(369, 509), new Point(379, 527), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(372.5, 522.5), new Point(390, 536.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(352, 504), new Point(351, 536.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(347.5, 535), new Point(352.5, 554), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(354, 537), new Point(354, 550), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(355, 536.5), new Point(365, 548), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(365, 539.5), new Point(375, 551), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(375, 533), new Point(375, 552.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(375, 531), new Point(383, 538), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(311.5, 462), new Point(335.5, 483), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(309, 467.5), new Point(336, 488.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(305.5, 469.5), new Point(336.5, 494), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(297.5, 470.5), new Point(331.5, 497), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(294.5, 473), new Point(323.5, 498), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(303.5, 487.5), new Point(319, 498.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(300.5, 491), new Point(313, 501.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(296, 491), new Point(304, 498.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(335, 475), new Point(346.5, 486.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(353.5, 512.75), new Point(374, 539), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(353.5, 523.5), new Point(373.25, 540), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(354.75, 528.75), new Point(364.75, 541.25), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(356, 513), new Point(375, 531.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(357, 508), new Point(371.75, 522.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(359, 504.75), new Point(369.5, 514.25), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(360.25, 500.5), new Point(368.75, 506.25), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(361.25, 494), new Point(366.25, 498.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(298, 543.25), new Point(302.5, 547.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(300.5, 537.75), new Point(305.75, 544.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(302, 534.75), new Point(310.25, 542.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(303, 528), new Point(315, 542), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(305.5, 523.75), new Point(320, 540.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(308.75, 521.75), new Point(322.25, 536.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(313, 519), new Point(324.5, 533.25), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(316.75, 514), new Point(327, 528), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(318.5, 510.75), new Point(331, 525.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(324.25, 510.25), new Point(333.5, 519), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(346.75, 491.5), new Point(324.25, 525.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(335.25, 527), new Point(332.75, 545), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(343.5, 532), new Point(331.75, 551.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(346.5, 508.75), new Point(346.5, 526), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(345.5, 545.5), new Point(331, 562), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(324.75, 555.5), new Point(315.5, 568.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(328.5, 535.5), new Point(309, 571.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(298.75, 507), new Point(281.5, 501.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(283.75, 510.25), new Point(294.75, 517), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(285.5, 516.25), new Point(279, 528), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(302.25, 511.25), new Point(288.75, 522.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(302.5, 505), new Point(316.5, 509.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(333.25, 498.25), new Point(318.5, 506.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(285.25, 485.75), new Point(286, 496), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(287.75, 476.25), new Point(293.25, 483.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(288.5, 466), new Point(304.5, 463.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(313.5, 448), new Point(318.75, 457), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(322.25, 454.75), new Point(326.75, 465), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(328, 455.25), new Point(334.25, 464), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(337.75, 458), new Point(346, 466.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(337.75, 468.5), new Point(351.25, 468.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(349.5, 464.75), new Point(357.5, 467.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(371, 483.25), new Point(386.25, 490), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(371.25, 493), new Point(382.5, 496.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(386, 496.25), new Point(387.25, 511), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(381.25, 502.5), new Point(374.25, 502.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(375.25, 510), new Point(383.5, 524), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(387.75, 516.75), new Point(396, 519.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(393.5, 513.25), new Point(390.5, 501.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(386, 526.5), new Point(395.5, 523), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(390.75, 531.75), new Point(403, 530.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(379, 539.5), new Point(379, 550), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(390.75, 546.5), new Point(377.5, 556), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(373.5, 558), new Point(359.5, 556.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(355, 562), new Point(344, 555.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(342.25, 563), new Point(322.75, 566.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(297.25, 559), new Point(300.75, 571.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(290, 548), new Point(277, 546.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(291.25, 485.75), new Point(303.5, 486.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(306.5, 547), new Point(302.5, 554.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(350.5, 567.5), new Point(294, 574), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(665.5, 424), new Point(696.5, 410), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(665, 425), new Point(674.5, 456), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(673, 457), new Point(708.5, 439.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(665, 440), new Point(704.5, 420.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(590.5, 451), new Point(614, 446.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(613, 447.5), new Point(612.5, 462.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(612, 462.5), new Point(612.5, 478), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(612.5, 478.5), new Point(599, 477), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(599, 477.5), new Point(580.5, 481), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(581, 481), new Point(586, 467.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(587.5, 467.5), new Point(590, 453), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(583.5, 480), new Point(560, 538.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(599, 479), new Point(573.5, 542.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(596, 538), new Point(543, 519), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(597.5, 537), new Point(562.5, 602.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(574.5, 547.5), new Point(545, 596), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(551, 555.5), new Point(559.5, 539), brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(573.666, 547.667);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(685, 544), new Point(684.5, 583.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(685.5, 545.5), new Point(720.5, 542.5), brush, 5f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(705, 569), new Point(718.5, 570.5), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(692.5, 571), new Point(692, 582), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(258, 583), new Point(271, 582.5), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(275.5, 593.5), new Point(275.5, 584.5), brush, 12f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(614.167, 476.334), new Point(612.167, 494.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(612.834, 494.667), new Point(611.501, 503.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(614.167, 476.667), new Point(619.501, 495.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(620.834, 486.334), new Point(619.167, 511.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(613.834, 475.334), new Point(631.834, 500), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(615.834, 477.334), new Point(632.834, 476), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(621.501, 481.667), new Point(636.501, 482.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(623.167, 484.334), new Point(652.501, 517.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(631.501, 496.667), new Point(652.167, 531.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(629.834, 491.334), new Point(651.834, 494.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(626.167, 485.667), new Point(654.834, 483.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(621.834, 490.334), new Point(626.167, 509.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(625.167, 492), new Point(628.834, 527), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(609.167, 504.334), new Point(598.834, 523), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(599.501, 516.334), new Point(595.834, 528.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(603.167, 519), new Point(603.167, 530), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(606.167, 521.667), new Point(606.501, 543), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(618.834, 529.334), new Point(609.834, 545.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(618.834, 527.334), new Point(625.167, 544.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(624.834, 526.667), new Point(624.834, 547.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(646.501, 530.334), new Point(661.501, 545.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(659.167, 533.667), new Point(659.834, 545.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(661.834, 521.334), new Point(665.834, 542), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(664.501, 531.667), new Point(683.167, 547), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(679.167, 534.334), new Point(684.501, 549.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(677.501, 523.667), new Point(685.501, 538.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(679.834, 519.667), new Point(692.167, 525.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(632.501, 496.667), new Point(654.501, 507.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(650.834, 503.667), new Point(676.834, 514.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(676.167, 513), new Point(690.501, 518), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(641.834, 490.667), new Point(670.501, 490.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(662.501, 482.667), new Point(672.501, 486), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(667.167, 481.667), new Point(675.834, 480.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(677.167, 469), new Point(670.167, 473.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(678.167, 462.334), new Point(658.167, 468), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(663.501, 453.667), new Point(660.501, 466.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(662.167, 454.667), new Point(649.167, 463), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(649.501, 463.667), new Point(634.167, 473.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(644.834, 468.667), new Point(631.501, 480.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(659.167, 458.334), new Point(634.167, 482.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(660.167, 460.667), new Point(636.501, 484.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(661.501, 466.334), new Point(642.167, 484.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(666.167, 467), new Point(649.834, 482), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(674.834, 465), new Point(654.501, 483.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(660.167, 475.667), new Point(651.167, 483), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(669.167, 473.334), new Point(651.834, 491), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(667.167, 482.667), new Point(658.834, 490.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(654.834, 483), new Point(648.834, 490), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(649.834, 484.667), new Point(643.834, 490.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(641.834, 484.667), new Point(634.167, 492.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(619.501, 493), new Point(612.834, 501), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(620.501, 499.334), new Point(604.167, 514), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(619.501, 505), new Point(603.167, 522.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(626.167, 504.334), new Point(604.501, 526.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(616.501, 501), new Point(616.167, 524.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(624.834, 511.667), new Point(605.167, 533.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(627.501, 514.334), new Point(605.167, 534.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(627.167, 520), new Point(606.501, 539.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(662.501, 512), new Point(649.834, 525.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(659.167, 509.334), new Point(647.167, 521), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(654.834, 507.334), new Point(644.834, 518), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(668.167, 512), new Point(650.501, 532.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(671.167, 513.667), new Point(651.167, 536.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(675.501, 516), new Point(657.167, 536), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(680.167, 515.667), new Point(665.834, 533.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(677.834, 525), new Point(668.167, 534.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(680.834, 528.334), new Point(668.834, 540.334), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(657.167, 500.667), new Point(666.834, 506.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(662.167, 496.334), new Point(672.834, 501.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(670.167, 507.667), new Point(688.834, 509.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(678.834, 504.001), new Point(679.167, 493.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(674.834, 489.001), new Point(685.167, 479.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(682.834, 490.334), new Point(690.834, 483.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(692.5, 489.334), new Point(683.834, 499.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(693.167, 496.001), new Point(688.834, 506.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(693.167, 508.334), new Point(696.167, 521.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(695.167, 523.334), new Point(695.834, 535.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(686.834, 529.001), new Point(691.834, 540.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(679.167, 549.667), new Point(671.167, 557.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(681.167, 552.001), new Point(677.5, 559.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(666.167, 561.334), new Point(654.5, 554.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(672.5, 545.667), new Point(658.5, 552.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(652.834, 558.334), new Point(649.5, 547.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(655.167, 543.667), new Point(644.834, 541.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(638.834, 537.667), new Point(638.834, 549.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(642.834, 555.334), new Point(630.5, 551.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(629.834, 547.001), new Point(633.834, 522.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(639.834, 532.334), new Point(641.5, 522.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(634.5, 506.667), new Point(637.167, 521.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(680.5, 474.334), new Point(690.167, 477.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(681.5, 465.001), new Point(689.5, 469.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(664.5, 459.001), new Point(671.834, 461.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(666.834, 444.667), new Point(652.167, 455.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(653.834, 450.667), new Point(630.5, 461.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(644.834, 460.667), new Point(629.167, 473.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(630.5, 463.001), new Point(618.167, 465.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(618.167, 468.667), new Point(629.167, 474.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(603.5, 481.001), new Point(601.5, 493.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(597.834, 494.667), new Point(608.5, 502.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(594.834, 501.334), new Point(596.5, 515.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(590.167, 515.001), new Point(593.167, 528.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(585.5, 527.334), new Point(594.167, 534.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(600.5, 532.001), new Point(604.5, 544.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(595.167, 544.667), new Point(618.167, 549.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(619.834, 542.001), new Point(623.5, 554.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(553.5, 492.001), new Point(561.5, 502.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(566.5, 491.334), new Point(568.167, 503.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(574.5, 488.667), new Point(571.834, 499.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(552.167, 501.334), new Point(561.834, 508.001), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(567.834, 504.334), new Point(565.167, 516.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(551.834, 508.667), new Point(563.5, 513.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(555.167, 517.667), new Point(567.5, 519.334), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(612.5, 448), new Point(665.5, 425), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(608, 459.5), new Point(666, 438.5), brush, 5f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(285.5, 410.335), new Point(329.5, 328.335), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(378.5, 447.334), new Point(384.5, 413.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(381.167, 429.334), new Point(334.5, 359.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(397.167, 464.001), new Point(443.833, 452.001), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(395.167, 430.001), new Point(427.833, 445.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(431.167, 548.668), new Point(471.833, 529.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(583.834, 466.001), new Point(540.5, 453.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(602.5, 446.001), new Point(591.167, 400.001), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(559.167, 448.668), new Point(645.834, 382.668), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(556.5, 538.667), new Point(508.5, 519.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(521.167, 529.334), new Point(425.833, 469.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(458.5, 533.334), new Point(563.167, 472.001), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(687.167, 410.668), new Point(672.5, 373.334), brush, 40f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(691.834, 378.668), new Point(623.834, 302.667), brush, 40f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(395.5, 251.5), new Point(377, 255), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(396.5, 236), new Point(394, 252.5), brush, 5f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(833.073, 133.102), new Point(1004.064, 202.099), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(137.108, 124.103), new Point(-38.383, 196.099), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(285.601, 658.076), new Point(324.599, 743.571), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(718.079, 620.577), new Point(670.081, 770.57), brush, 80f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(701.5, 571), new Point(701, 582), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(704, 561), new Point(718.5, 561), brush, 12f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(720.5, 551.5), new Point(707, 552), brush, 12f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(422.833, 160.002), new Point(490.834, 190.668), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.834, 190.668), new Point(568.167, 157.668), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.5, 178.002), new Point(502.167, 187.335), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.834, 191.335), new Point(505.834, 201.668), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(489.5, 192.668), new Point(475.833, 200.335), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 194.002), new Point(493.834, 211.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.834, 210.668), new Point(516.167, 196.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(516.167, 195.668), new Point(565.834, 174.668), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(465.5, 194.335), new Point(418.833, 172.335), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 47.338), new Point(491.167, 73.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(491.5, 73.338), new Point(521.167, 48.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(520.834, 48.338), new Point(493.167, 21.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(492.834, 21.338), new Point(464.833, 48.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(470.5, 24.338), new Point(491.834, 46.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(491.834, 46.338), new Point(517.834, 24.338), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(517.5, 24.338), new Point(493.834, 5.005), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(493.834, 4.672), new Point(471.167, 24.338), brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(491.833, 46.667);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(519.167, 47.672), new Point(542.834, 77.005), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(542.834, 76.672), new Point(565.834, 111.338), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(566.167, 112.338), new Point(581.834, 138.338), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(496.167, 51.005), new Point(520.834, 77.005), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(520.5, 77.005), new Point(537.167, 101.005), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(537.5, 101.005), new Point(564.834, 144.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(487.833, 50.005), new Point(464.167, 79.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(463.833, 79.672), new Point(445.5, 102.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(445.833, 102.672), new Point(422.833, 139.005), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(466.5, 46.338), new Point(446.833, 72.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(446.833, 73.005), new Point(428.167, 98.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(428.167, 98.672), new Point(406.833, 134.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(481.833, 153.338), new Point(491.5, 180.338), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(492.167, 161.005), new Point(492.5, 179.672), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.5, 92.005), new Point(489.167, 129.672), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(489.5, 129.672), new Point(491.834, 161.005), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.5, 136.672), new Point(494.5, 162.338), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(499.834, 151.005), new Point(495.167, 170.672), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(498.5, 158.338), new Point(512.167, 140.672), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(511.5, 141.005), new Point(523.5, 130.672), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(454.167, 126.338), new Point(469.5, 134.672), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(465.167, 132.338), new Point(481.833, 156.005), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(532.834, 128.005), new Point(522.5, 131.005), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(476.167, 96.338), new Point(488.5, 116.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(475.833, 98.338), new Point(474.833, 86.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(474.833, 85.672), new Point(478.5, 78.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(478.5, 79.005), new Point(485.167, 78.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.167, 79.338), new Point(491.5, 84.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(489.833, 84.672), new Point(494.167, 80.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(494.5, 79.672), new Point(500.167, 79.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(500.5, 80.005), new Point(505.5, 86.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(505.167, 87.338), new Point(503.5, 96.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(503.5, 97.005), new Point(501.834, 104.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.5, 104.338), new Point(495.167, 113.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(494.834, 113.338), new Point(490.167, 117.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(469.5, 137.005), new Point(467.833, 122.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(467.5, 122.005), new Point(462.167, 110.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(462.167, 109.672), new Point(454.5, 107.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(454.5, 107.005), new Point(449.167, 111.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(448.5, 111.672), new Point(451.167, 120.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(450.833, 121.005), new Point(443.167, 121.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(442.833, 121.672), new Point(439.167, 126.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(440.5, 127.005), new Point(440.5, 132.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(440.833, 133.005), new Point(446.833, 138.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(447.167, 138.338), new Point(456.5, 139.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(457.5, 139.672), new Point(468.5, 139.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(487.5, 125.005), new Point(483.167, 121.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.833, 121.338), new Point(479.5, 128.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(478.833, 128.338), new Point(477.5, 134.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(477.5, 134.672), new Point(491.834, 168.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.5, 125.338), new Point(496.5, 124.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(496.5, 124.338), new Point(502.5, 127.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(502.5, 127.338), new Point(504.5, 135.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(504.834, 134.338), new Point(500.834, 144.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(515.834, 136.672), new Point(519.5, 118.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(519.834, 118.005), new Point(522.834, 108.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(522.834, 108.005), new Point(531.5, 107.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(532.5, 107.672), new Point(537.834, 109.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(537.834, 110.005), new Point(540.834, 118.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(540.5, 118.672), new Point(535.834, 125.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(536.167, 123.672), new Point(544.5, 125.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(544.834, 125.672), new Point(547.167, 131.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(547.167, 132.338), new Point(543.167, 140.672), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.834, 141.005), new Point(535.167, 141.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(534.834, 141.005), new Point(518.834, 133.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(511.834, 178.672), new Point(470.167, 178.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(458.167, 173.005), new Point(527.5, 174.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.5, 167.338), new Point(494.167, 168.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(498.167, 160.338), new Point(555.834, 162.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(555.5, 154.005), new Point(501.167, 154.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.5, 147.338), new Point(498.5, 148.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.834, 141.672), new Point(531.834, 139.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(541.834, 141.338), new Point(560.5, 141.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(547.5, 134.672), new Point(559.167, 135.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(545.5, 129.338), new Point(555.834, 129.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(540.5, 122.338), new Point(548.834, 122.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(539.167, 116.005), new Point(547.167, 117.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(515.5, 134.672), new Point(501.5, 133.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(502.167, 126.338), new Point(516.5, 126.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(517.167, 119.672), new Point(468.833, 120.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(480.167, 126.005), new Point(468.5, 126.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(431.833, 126.338), new Point(438.5, 126.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.833, 120.338), new Point(443.167, 120.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(439.5, 116.338), new Point(449.167, 116.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(442.833, 110.672), new Point(450.833, 110.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 115.338), new Point(485.833, 114.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(520.5, 112.672), new Point(495.167, 113.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.834, 106.005), new Point(539.167, 105.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.833, 149.338), new Point(430.5, 150.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(433.833, 143.338), new Point(480.833, 145.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(469.5, 138.338), new Point(478.167, 138.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(476.833, 131.672), new Point(469.167, 131.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(442.167, 137.338), new Point(425.5, 137.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(427.5, 131.338), new Point(441.5, 131.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(427.5, 156.005), new Point(482.5, 157.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(485.5, 163.005), new Point(429.5, 163.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(447.167, 168.338), new Point(489.5, 168.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(468.833, 74.005), new Point(516.167, 75.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(512.834, 68.672), new Point(496.834, 68.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(486.833, 69.005), new Point(473.167, 68.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 80.672), new Point(477.167, 80.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(499.5, 80.005), new Point(520.5, 80.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(524.167, 85.672), new Point(503.5, 86.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(529.5, 93.338), new Point(503.834, 93.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(535.5, 98.005), new Point(503.5, 99.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.5, 109.338), new Point(463.167, 108.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(478.167, 103.005), new Point(447.833, 103.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(453.833, 95.672), new Point(475.167, 95.672), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(472.833, 88.005), new Point(458.833, 88.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.167, 186.333), new Point(468.833, 195), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.167, 182), new Point(487.167, 188.667), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(497.834, 186.333), new Point(513.167, 196.333), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(474.5, 202.002), new Point(436.833, 229.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(507.834, 201.335), new Point(546.167, 232.002), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(546.5, 232.002), new Point(586.834, 237.335), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(396.833, 235.669), new Point(437.167, 228.669), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(439.167, 228.669), new Point(491.5, 227.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(491.167, 227.669), new Point(544.834, 231.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(399.167, 222.335), new Point(412.5, 219.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(412.833, 219.669), new Point(421.167, 219.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(421.5, 219.335), new Point(431.167, 221.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(430.833, 221.335), new Point(435.167, 223.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.167, 224.002), new Point(437.833, 215.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.833, 215.002), new Point(446.833, 208.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(446.833, 208.002), new Point(456.167, 202.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(455.833, 202.335), new Point(467.833, 195.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(517.834, 196.002), new Point(530.5, 205.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(530.834, 205.002), new Point(543.5, 213.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(543.834, 213.002), new Point(548.5, 220.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(548.834, 220.002), new Point(551.167, 226.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(551.167, 227.002), new Point(561.834, 222.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(562.167, 222.002), new Point(575.834, 223.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(575.834, 223.335), new Point(586.5, 227.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(489.5, 210.335), new Point(482.833, 219.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.167, 219.002), new Point(477.5, 227.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(492.5, 212.002), new Point(502.167, 222.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(502.167, 222.002), new Point(507.167, 232.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(507.167, 232.669), new Point(508.834, 236.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(509.167, 236.669), new Point(519.5, 241.002), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(519.834, 241.002), new Point(530.834, 249.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(475.167, 229.335), new Point(474.833, 237.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(474.167, 238.335), new Point(464.167, 243.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(463.5, 243.669), new Point(451.167, 250.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(451.167, 251.002), new Point(444.5, 259.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(559.167, 182.003), new Point(547.5, 190.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(565.167, 182.336), new Point(558.834, 202.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(557.5, 189.669), new Point(543.834, 199.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(524.834, 196.003), new Point(540.834, 204.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(531.834, 193.669), new Point(543.167, 195.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(552.834, 199.669), new Point(553.5, 218.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(545.5, 203.669), new Point(546.834, 211.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(560.167, 207.669), new Point(561.834, 216.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(565.5, 198.336), new Point(565.834, 211.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(569.5, 194.003), new Point(574.5, 207.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(573.834, 211.003), new Point(568.5, 220.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(422.833, 180.003), new Point(416.5, 188.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(429.5, 182.336), new Point(420.833, 193.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(434.833, 185.003), new Point(422.833, 202.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(438.833, 190.003), new Point(432.5, 199.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(431.833, 200.669), new Point(423.833, 211.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(446.833, 191.669), new Point(436.167, 207.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(453.5, 194.336), new Point(446.833, 204.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(434.5, 208.669), new Point(428.5, 219.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(417.833, 194.003), new Point(412.833, 206.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(410.5, 205.669), new Point(406.167, 218.669), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(419.167, 207.669), new Point(414.5, 219.003), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(423.167, 213.669), new Point(420.5, 219.336), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(450.833, 225.002), new Point(476.833, 206.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(480.5, 208.002), new Point(455.833, 226.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(464.167, 225.002), new Point(482.167, 211.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(481.5, 216.335), new Point(469.5, 225.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(505.5, 207.002), new Point(498.167, 215.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(508.5, 207.668), new Point(502.167, 219.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(512.834, 212.002), new Point(533.834, 229.335), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(508.5, 212.668), new Point(522.5, 225.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(509.5, 219.002), new Point(520.167, 228.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(506.5, 220.335), new Point(513.5, 228.002), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(154.5, 328), new Point(179, 351.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(154, 328), new Point(133.5, 350.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(133, 350.5), new Point(155.5, 378.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(155, 378.5), new Point(179, 353), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(155.5, 352.5), new Point(183.5, 326), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(183.5, 325.5), new Point(208.5, 349.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(208.5, 350), new Point(181.5, 380.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(155, 352.5), new Point(182, 380), brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(183.333, 351.167);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(272, 263.5), new Point(233.5, 289), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(234, 289), new Point(209.5, 305.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(209, 305.5), new Point(182.5, 326), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(278.5, 280), new Point(251, 297), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(251.5, 296.5), new Point(220, 317), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(220, 317.5), new Point(187.5, 344.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(185.5, 358), new Point(216.5, 385), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(216, 385), new Point(247.5, 406), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(181.5, 379), new Point(211, 404.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(210.5, 404.5), new Point(239.5, 423.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(239, 423.5), new Point(275.5, 448), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(246, 406.5), new Point(287.5, 432), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(494, 658.5), new Point(469, 682), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(470.5, 682.5), new Point(495.5, 700), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(495.5, 700.5), new Point(522, 678), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(521.5, 678), new Point(490.5, 657.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(495.5, 683.5), new Point(467.5, 656.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(467, 656.5), new Point(491.5, 630.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(491, 630.5), new Point(523, 657), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(522.5, 656.5), new Point(495.5, 684), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(420.5, 562.5), new Point(441, 595), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(441, 594.5), new Point(459.5, 621.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(460, 622), new Point(491.5, 657.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(405.5, 569.5), new Point(419, 595.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(419.5, 595.5), new Point(438.5, 623), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(439, 623.5), new Point(460.5, 651), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(460, 651), new Point(476.5, 664.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(546.5, 594), new Point(528, 619), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(527.5, 620.5), new Point(507, 643.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(506.5, 642), new Point(495.5, 658), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(563, 601.5), new Point(545, 629.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(544.5, 629.5), new Point(521, 654.5), brush, 5f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(493.833, 657.167);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(785.833, 352.167);
                ellipse.RadiusX = 9f;
                ellipse.RadiusY = 9f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(699.5, 266), new Point(730.5, 284.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(730, 284.5), new Point(757.5, 303.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(757, 303.5), new Point(784, 328), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(783.5, 328), new Point(808.5, 354), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(808.5, 353.5), new Point(789, 375), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(789, 375.5), new Point(758.5, 403.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(758, 403.5), new Point(710.5, 436.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(702, 420), new Point(728.5, 404), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(728.5, 403.5), new Point(755.5, 382), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(756, 382), new Point(781, 357), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(760.5, 348.5), new Point(787, 377), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(761.5, 347.5), new Point(784.5, 326.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(697.5, 285), new Point(723, 299), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(722.5, 299), new Point(749, 317), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(749, 317.5), new Point(780.5, 347), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(791, 348), new Point(807.5, 329), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(807, 329), new Point(829.5, 352), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(829.5, 352), new Point(807.5, 376.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(808, 376.5), new Point(788.5, 354), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(303.5, 281), new Point(330.5, 336.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(326.5, 341), new Point(315.5, 349), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(316, 349), new Point(327, 359), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(327, 342.5), new Point(344, 368), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(333, 351), new Point(325.5, 362.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(325.5, 362), new Point(298, 420.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(327, 364), new Point(337, 379), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(336.5, 378), new Point(354.5, 345), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(319.5, 273.5), new Point(339.5, 318.5), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(339, 318.5), new Point(353, 345), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(328, 337.5), new Point(340.5, 318.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(340, 317), new Point(351, 300.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(351, 300), new Point(360, 293.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(360, 293), new Point(364.5, 291.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(365, 291.5), new Point(361.5, 279.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(361.5, 279), new Point(368.5, 256.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(377, 254.5), new Point(373, 279.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(373, 279), new Point(370, 304.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(370, 305), new Point(368.5, 352), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(368.5, 351.5), new Point(368.5, 392), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(368.5, 392.5), new Point(373.5, 433), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(373, 433), new Point(378, 453.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(337.5, 378), new Point(342, 390), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(342, 389.5), new Point(347, 399.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(347.5, 400), new Point(357, 406.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(357, 407), new Point(356.5, 423.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(356.5, 424), new Point(360.5, 441), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(360, 441), new Point(365, 450.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(341, 368.5), new Point(358, 389), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(358, 389.5), new Point(374, 411), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(374, 411.5), new Point(396, 437.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(394.5, 437.5), new Point(416, 456), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(415.5, 456), new Point(437.5, 474), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(437, 474), new Point(448, 481), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(388.5, 469), new Point(417.5, 474.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(417.5, 474), new Point(462, 481), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(462.5, 481), new Point(502, 481.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(502.5, 481.5), new Point(542, 479), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(542.5, 479), new Point(584, 471), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(584.5, 471), new Point(598.5, 467.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(414.5, 548.5), new Point(451, 534.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(450.5, 534.5), new Point(485.5, 516.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(486, 516.5), new Point(531, 488), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(530.5, 487.5), new Point(577.5, 446.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(577.5, 446), new Point(615, 402), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(603, 465.5), new Point(610.5, 431), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(610.5, 430.5), new Point(616.5, 375), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(616, 375), new Point(615.5, 326), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(615.5, 325.5), new Point(612, 283), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(612, 283.5), new Point(603, 242), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(312, 425.5), new Point(322.5, 402), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(322, 402), new Point(335, 375), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(662, 280.5), new Point(650.5, 312), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(650, 312), new Point(638.5, 334.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(638.5, 335), new Point(628.5, 349.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(628, 349.5), new Point(639, 367), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(639, 367.5), new Point(650.5, 390), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(651, 390), new Point(660, 409), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(660, 410), new Point(664, 425.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(677, 287.5), new Point(662.5, 326.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(662, 326.5), new Point(646.5, 352), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(646.5, 351.5), new Point(655, 370), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(655, 371), new Point(666.5, 391), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(666.5, 391.5), new Point(678.5, 421), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(552, 235), new Point(573.5, 256.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(573, 256.5), new Point(596, 281.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(596.5, 281.5), new Point(614.5, 301.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(615, 301.5), new Point(631.5, 327), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(632, 327), new Point(647, 353.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(332, 351), new Point(351.5, 318), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(352, 318), new Point(367.5, 299), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(367, 299), new Point(385, 279.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(385, 279), new Point(406, 256), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(405.5, 256), new Point(423.5, 241), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(423.5, 240.5), new Point(435.5, 230.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(448.5, 481.5), new Point(471, 496.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(470.5, 496.5), new Point(505.5, 514.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(505.5, 515), new Point(531.5, 529.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(532, 529.5), new Point(555, 540), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(555, 540.5), new Point(571.5, 545.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(417.5, 529.5), new Point(442.5, 520), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(442, 520), new Point(468, 508), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(468, 507.5), new Point(494.5, 491), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(494.5, 491.5), new Point(518.5, 504.5), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(518, 504.5), new Point(542, 518), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(542.5, 518), new Point(576, 530), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(399, 479.5), new Point(412.5, 487.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(413, 487.5), new Point(428, 490), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(428.5, 490), new Point(436.5, 487.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.5, 487.5), new Point(443, 486), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(440, 486), new Point(445.5, 493), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(445.5, 493.5), new Point(454.5, 501.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(455, 501.5), new Point(464.5, 504), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(465, 504), new Point(477, 513), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(477.5, 513), new Point(488, 524.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(488.5, 524.5), new Point(492, 532), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(492, 532.5), new Point(498, 521.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(497.5, 521.5), new Point(507, 513.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(507, 513), new Point(514.5, 508), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(515, 508), new Point(532, 502), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(531.5, 502), new Point(541.5, 492), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(541.5, 491.5), new Point(547.5, 483.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(547, 483.5), new Point(561.5, 485.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(561, 485.5), new Point(579, 483), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(579, 482.5), new Point(586.5, 477.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(613, 449), new Point(619.5, 433.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(619, 433.5), new Point(619.5, 420.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(619, 420.5), new Point(618.5, 407), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(615.5, 407), new Point(627, 400), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(627, 399.5), new Point(637, 387), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(636.5, 387.5), new Point(640, 374), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(640, 373.5), new Point(651.5, 360), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(651.5, 360), new Point(659, 350), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(661, 347), new Point(652.5, 339.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(652, 339.5), new Point(643.5, 327.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(643, 327.5), new Point(636, 306), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(635.5, 306), new Point(630.5, 298.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(630, 298.5), new Point(621, 296), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(620.5, 296), new Point(624.5, 279.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(624, 279.5), new Point(623, 266), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(623, 265.5), new Point(615.5, 248.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(615.5, 248), new Point(610, 233.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(610.5, 233.5), new Point(611.5, 225.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(610.5, 227.5), new Point(595, 229), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(595.5, 229.5), new Point(580.5, 226.5), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(284.5, 289.334), new Point(301.833, 281.334), brush, 5f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(617.834, 231.667), new Point(617.834, 259.667), brush, 5f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(294.334, 359.838), new Point(321.333, 350.172), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(302.001, 349.505), new Point(320.667, 349.172), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(233.001, 351.172), new Point(270.667, 352.505), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(270.667, 352.172), new Point(302.001, 349.838), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(277.667, 340.172), new Point(303.333, 347.172), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(292.001, 341.838), new Point(311.667, 346.505), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(299.333, 343.172), new Point(281.667, 329.505), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(282.001, 330.172), new Point(271.667, 318.172), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(267.334, 387.505), new Point(275.667, 372.172), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(273.334, 376.505), new Point(297.001, 359.838), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(269.001, 308.838), new Point(272.001, 319.172), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(237.334, 365.505), new Point(257.334, 353.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(239.334, 365.838), new Point(227.001, 366.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(220.334, 358.505), new Point(225.334, 350.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(220.667, 347.172), new Point(220.667, 341.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(221.001, 341.172), new Point(227.667, 336.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(228.334, 336.505), new Point(237.667, 338.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(238.001, 338.172), new Point(245.001, 339.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(245.334, 340.172), new Point(254.334, 346.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(254.334, 346.838), new Point(258.334, 351.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(278.001, 372.172), new Point(263.001, 373.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(263.001, 374.172), new Point(251.001, 379.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(250.667, 379.505), new Point(248.334, 387.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(248.001, 387.172), new Point(252.667, 392.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(252.667, 393.172), new Point(261.667, 390.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(262.001, 390.838), new Point(262.667, 398.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(268.001, 401.172), new Point(273.667, 401.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(279.334, 394.505), new Point(280.001, 385.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(280.667, 384.172), new Point(280.667, 373.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(266.001, 354.172), new Point(262.334, 358.505), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(262.334, 358.838), new Point(269.001, 362.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(269.334, 362.838), new Point(275.334, 364.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(275.667, 364.172), new Point(309.667, 349.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(266.334, 351.172), new Point(265.667, 345.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(265.334, 345.172), new Point(268.001, 339.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(268.334, 339.172), new Point(276.334, 337.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(275.334, 336.838), new Point(285.667, 340.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(277.667, 325.838), new Point(259.001, 322.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(259.001, 321.838), new Point(249.334, 318.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(249.001, 318.838), new Point(248.667, 310.172), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(251.001, 303.838), new Point(259.001, 300.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(259.667, 301.172), new Point(266.667, 305.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(282.001, 306.838), new Point(274.001, 322.838), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(301.333, 343.505), new Point(303.333, 285.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(295.001, 286.172), new Point(295.001, 340.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(288.334, 289.172), new Point(289.334, 343.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(282.667, 339.838), new Point(280.667, 309.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(275.667, 294.172), new Point(276.667, 282.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(270.334, 296.172), new Point(270.667, 285.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(263.334, 301.172), new Point(263.334, 292.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(275.667, 326.172), new Point(274.334, 340.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(267.334, 339.505), new Point(267.667, 325.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(260.667, 324.505), new Point(261.667, 372.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(267.001, 361.505), new Point(267.334, 373.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(261.334, 405.838), new Point(261.334, 398.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(253.667, 321.172), new Point(254.001, 346.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(247.001, 339.838), new Point(246.667, 302.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(290.334, 358.838), new Point(291.667, 411.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(284.334, 407.838), new Point(286.001, 360.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(279.334, 372.172), new Point(279.334, 363.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(272.667, 364.838), new Point(272.667, 372.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(278.334, 399.505), new Point(278.667, 416.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(297.001, 414.172), new Point(298.667, 359.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(304.001, 356.172), new Point(304.667, 412.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(209.667, 328.838), new Point(209.667, 344.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(221.001, 342.172), new Point(221.001, 321.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(226.667, 317.505), new Point(227.667, 338.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(234.334, 312.172), new Point(234.334, 337.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(239.001, 306.172), new Point(240.001, 338.172), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(250.334, 358.172), new Point(249.334, 378.505), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(244.001, 363.505), new Point(244.001, 393.838), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(503.167, 548.004), new Point(493.5, 521.005), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(492.834, 540.337), new Point(492.5, 521.671), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(494.5, 609.337), new Point(495.834, 571.671), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(495.5, 571.671), new Point(493.166, 540.337), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.5, 564.671), new Point(490.5, 539.005), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(485.167, 550.338), new Point(489.834, 530.671), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(486.5, 543.005), new Point(472.833, 560.671), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(473.5, 560.338), new Point(461.5, 570.671), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(530.834, 575.004), new Point(515.5, 566.671), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(519.834, 569.004), new Point(503.167, 545.337), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(452.167, 573.338), new Point(462.5, 570.338), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(508.834, 605.004), new Point(496.5, 585.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(509.167, 603.004), new Point(510.167, 615.337), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.834, 622.005), new Point(493.5, 617.005), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.5, 621.671), new Point(484.833, 621.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(484.5, 621.337), new Point(479.5, 614.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(479.833, 614.005), new Point(481.5, 604.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(481.5, 604.338), new Point(483.167, 597.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.5, 597.004), new Point(489.834, 588.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(490.166, 588.004), new Point(494.834, 584.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(515.5, 564.338), new Point(517.167, 579.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(517.5, 579.338), new Point(522.834, 591.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(522.834, 591.671), new Point(530.5, 594.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(530.5, 594.338), new Point(535.834, 589.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(536.5, 589.671), new Point(533.834, 580.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(534.167, 580.338), new Point(541.834, 579.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(544.5, 574.338), new Point(544.5, 568.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(537.834, 563.004), new Point(528.5, 562.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(527.5, 561.671), new Point(516.5, 561.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(497.5, 576.338), new Point(501.834, 580.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(502.167, 580.004), new Point(505.5, 573.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(506.167, 573.004), new Point(507.5, 567.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(507.5, 566.671), new Point(493.166, 532.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(494.5, 576.004), new Point(488.5, 576.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(488.5, 577.004), new Point(482.5, 574.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.5, 574.004), new Point(480.5, 566.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(480.167, 567.004), new Point(484.167, 556.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(469.167, 564.671), new Point(465.5, 583.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(465.167, 583.338), new Point(462.167, 593.004), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(462.167, 593.338), new Point(453.5, 593.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(447.167, 591.338), new Point(444.167, 583.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(444.5, 582.671), new Point(449.167, 575.671), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(450.167, 560.338), new Point(466.167, 568.338), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(486.833, 541.005), new Point(429.167, 539.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(429.5, 547.338), new Point(483.833, 547.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(432.5, 554.004), new Point(486.5, 553.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.167, 559.671), new Point(453.167, 561.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.5, 566.671), new Point(425.833, 565.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(439.5, 572.004), new Point(429.167, 571.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(444.5, 579.004), new Point(436.167, 579.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(469.5, 566.671), new Point(483.5, 568.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(482.833, 575.004), new Point(468.5, 574.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(467.833, 581.671), new Point(516.167, 580.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(504.834, 575.338), new Point(516.5, 575.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(549.167, 581.004), new Point(541.834, 581.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 588.671), new Point(489.834, 588.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(483.167, 595.338), new Point(445.833, 595.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(502.167, 552.004), new Point(554.5, 550.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(551.167, 558.004), new Point(504.167, 556.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(515.5, 563.004), new Point(506.834, 563.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(508.167, 569.671), new Point(515.834, 569.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.834, 564.004), new Point(559.5, 563.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(557.5, 545.337), new Point(502.5, 543.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(499.5, 538.337), new Point(555.5, 537.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(516.167, 627.337), new Point(468.833, 626.337), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(472.167, 632.671), new Point(488.167, 632.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(498.167, 632.337), new Point(511.834, 633.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(520.5, 620.671), new Point(507.834, 621.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(485.5, 621.337), new Point(464.5, 621.337), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(460.833, 615.671), new Point(481.5, 614.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(455.5, 608.005), new Point(481.167, 608.005), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(449.5, 603.338), new Point(481.5, 602.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(501.5, 592.004), new Point(521.834, 593.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(506.834, 598.338), new Point(537.167, 598.338), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(531.167, 605.671), new Point(509.834, 605.671), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(512.167, 613.337), new Point(526.167, 613.337), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(680.501, 339.669), new Point(653.501, 349.335), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(672.833, 350.003), new Point(654.167, 350.335), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(741.834, 348.335), new Point(704.167, 347.003), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(704.167, 347.335), new Point(672.833, 349.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(697.167, 359.335), new Point(671.501, 352.335), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(682.833, 357.669), new Point(663.167, 353.003), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(675.501, 356.335), new Point(693.167, 370.004), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(692.833, 369.336), new Point(703.167, 381.336), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(707.501, 312.003), new Point(699.167, 327.336), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(701.501, 323.003), new Point(677.833, 339.669), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(705.833, 390.67), new Point(702.833, 380.336), brush, 3f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(737.5, 334.003), new Point(717.501, 346.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(735.5, 333.669), new Point(747.834, 332.669), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(754.5, 341.003), new Point(749.5, 349.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(754.167, 352.335), new Point(754.167, 358.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(753.834, 358.335), new Point(747.167, 363.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(746.5, 363.003), new Point(737.167, 361.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(736.834, 361.335), new Point(729.833, 359.669), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(729.501, 359.335), new Point(720.501, 353.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(720.501, 352.669), new Point(716.501, 348.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(696.833, 327.336), new Point(711.833, 325.669), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(711.833, 325.336), new Point(723.833, 320.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(724.167, 320.003), new Point(726.501, 312.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(726.833, 312.336), new Point(722.167, 307.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(722.167, 306.336), new Point(713.167, 309.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(712.833, 308.669), new Point(712.167, 301.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(706.833, 298.336), new Point(701.167, 298.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(695.501, 305.003), new Point(694.833, 314.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(694.167, 315.336), new Point(694.167, 326.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(708.833, 345.335), new Point(712.501, 341.003), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(712.501, 340.669), new Point(705.833, 337.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(705.501, 336.669), new Point(699.501, 335.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(699.167, 335.335), new Point(665.167, 349.669), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(708.501, 348.335), new Point(709.167, 354.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(709.501, 354.335), new Point(706.833, 360.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(706.501, 360.335), new Point(698.501, 362.335), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(699.501, 362.669), new Point(689.167, 358.669), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(697.167, 373.67), new Point(715.833, 377.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(715.833, 377.67), new Point(725.501, 380.67), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(725.833, 380.67), new Point(726.167, 389.336), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(723.833, 395.67), new Point(715.833, 398.67), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(715.167, 398.336), new Point(708.167, 393.67), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(692.833, 392.67), new Point(700.833, 376.67), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(673.501, 356.003), new Point(671.501, 413.67), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(679.833, 413.336), new Point(679.833, 359.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(686.501, 410.336), new Point(685.501, 356.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(692.167, 359.669), new Point(694.167, 389.67), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(699.167, 405.336), new Point(698.167, 417.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(704.501, 403.336), new Point(704.167, 413.67), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(711.501, 398.336), new Point(711.501, 406.67), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(699.167, 373.336), new Point(700.501, 359.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(707.501, 360.003), new Point(707.167, 374.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(714.167, 375.004), new Point(713.167, 326.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(707.833, 338.003), new Point(707.501, 326.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(713.501, 293.669), new Point(713.501, 301.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(721.167, 378.336), new Point(720.833, 353.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(727.833, 359.669), new Point(728.167, 397.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(684.501, 340.669), new Point(683.167, 288.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(690.501, 291.669), new Point(688.833, 338.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(695.501, 327.336), new Point(695.501, 336.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(702.167, 334.669), new Point(702.167, 327.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(696.501, 300.003), new Point(696.167, 283.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(677.833, 285.336), new Point(676.167, 340.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(670.833, 343.335), new Point(670.167, 287.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(759.834, 326.669), new Point(758.834, 374.004), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(765.167, 370.67), new Point(765.167, 354.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(764.834, 344.669), new Point(765.5, 331.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(753.167, 322.336), new Point(753.5, 335.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(753.834, 357.335), new Point(753.834, 378.336), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(748.167, 382.004), new Point(747.167, 361.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(740.5, 387.336), new Point(740.5, 361.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(735.834, 393.336), new Point(734.834, 361.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(724.501, 341.335), new Point(725.501, 321.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(730.833, 336.003), new Point(730.833, 305.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(738.167, 311.669), new Point(738.167, 333.003), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(745.834, 330.669), new Point(745.834, 316.669), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(278.5, 392), new Point(273, 402.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(260, 403.5), new Point(273, 400.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(217.5, 361.5), new Point(230, 368), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(217.5, 362.5), new Point(219, 359), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(218.5, 343.5), new Point(223.5, 353.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(265, 305), new Point(271, 294), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(270, 294.5), new Point(283, 293), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(281, 291), new Point(285.5, 303), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(286, 303), new Point(283, 309), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(251.5, 302.5), new Point(247, 310), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(221.5, 355), new Point(219, 362.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(452, 559), new Point(438, 561), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.5, 561), new Point(435, 567), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(434.5, 567), new Point(439.5, 574.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(439.5, 575), new Point(447.5, 578.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(486.5, 622), new Point(495.5, 610.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(494, 609.5), new Point(503, 622), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(510, 610), new Point(505, 621), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(536, 563), new Point(544, 571), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(544.5, 569.5), new Point(542.5, 581.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(424, 565.5), new Point(434, 558.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(745.5, 331.5), new Point(753.5, 340), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(746.5, 347.5), new Point(755, 354), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(726, 387), new Point(721, 396), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(693.5, 391), new Point(694, 400.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(694.5, 401), new Point(701.5, 405), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(702, 405), new Point(710.5, 395.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(712.5, 303), new Point(706.5, 297), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(700, 296.5), new Point(691.5, 308), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(692.5, 306.5), new Point(697, 324.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(209.834, 346), new Point(209.834, 378.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(203.834, 355.333), new Point(203.834, 376), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(215.834, 323.333), new Point(217.167, 383.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(223.167, 366), new Point(223.834, 390.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(230.5, 367.333), new Point(230.5, 395.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(237.834, 366.667), new Point(235.834, 400), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(253.167, 390.667), new Point(252.5, 410.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(254.5, 354), new Point(255.167, 378), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(245.167, 389.333), new Point(245.834, 402.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(260.5, 402), new Point(260.5, 415.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(268.5, 398.667), new Point(268.5, 420), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(273.834, 396.667), new Point(273.834, 422), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(310.5, 354.5), new Point(311.5, 389.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(317, 351), new Point(317, 372.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(521.5, 177), new Point(492, 178.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(321.5, 343.5), new Point(331, 334), brush, 5f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(326.833, 276.333), new Point(334.5, 295.333), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(336.5, 273.667), new Point(337.167, 292), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(336.833, 293), new Point(340.167, 305.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(341.833, 289), new Point(345.5, 301), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(341.833, 283.333), new Point(344.833, 268.667), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(354.833, 264.667), new Point(348.167, 280), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(349.167, 284.667), new Point(350.833, 297), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(354.833, 282.667), new Point(358.833, 293.333), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(361.167, 262.333), new Point(355.5, 280), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(338.833, 308), new Point(349.167, 301), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(322.167, 415.33), new Point(329.167, 433.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(319.5, 428.997), new Point(318.5, 420.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(325.833, 406.997), new Point(340.5, 440.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(328.833, 399.664), new Point(338.833, 408.997), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(334.833, 412.664), new Point(347.167, 424.997), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(340.833, 427.33), new Point(356.833, 433.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(346.167, 435.33), new Point(357.167, 440.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(340.833, 408.997), new Point(353.167, 424.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(329.833, 390.664), new Point(339.5, 399.33), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(341.5, 400.997), new Point(354.167, 412.33), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(334.5, 382.664), new Point(338.5, 390.664), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(406.25, 489.75), new Point(417.25, 513.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(415.75, 493), new Point(421.5, 504.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(423.5, 512), new Point(431.75, 519.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(423, 495), new Point(432, 503.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(431, 505.75), new Point(438.75, 517.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(418, 518.25), new Point(425.5, 522), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(436, 499.5), new Point(443.25, 509), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(431.75, 492.75), new Point(445, 493), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(442.75, 499), new Point(455.25, 507.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(442.5, 512.75), new Point(451.5, 510.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(533.5, 506), new Point(549.75, 515.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(539, 500.5), new Point(545.5, 505.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(544.75, 494), new Point(556.25, 491.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(563.75, 486.5), new Point(557.75, 499.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(550.75, 500.25), new Point(562.5, 506.25), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(550, 508.5), new Point(568, 513.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(555.75, 516.5), new Point(567, 521.75), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(567, 507), new Point(567.5, 498.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(568, 491.5), new Point(581.5, 489.5), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(574.5, 496.667), new Point(571.5, 509), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(625.5, 405.335), new Point(626.834, 417.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(625.834, 420.002), new Point(628.5, 431.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(623.167, 437.335), new Point(635.834, 437.335), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(636.167, 432.335), new Point(634.167, 416.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(634.834, 410.335), new Point(630.5, 399.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(637.167, 392.668), new Point(641.5, 403.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(641.834, 381.668), new Point(646.834, 396.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(644.5, 400.002), new Point(655.834, 408.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(640.167, 408.335), new Point(651.167, 412.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(656.167, 413.668), new Point(646.5, 419.668), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(642.167, 415.668), new Point(641.834, 426.335), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(644.5, 430.335), new Point(657.834, 424.002), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(627.167, 272.339), new Point(639.5, 280.672), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(641.167, 274.672), new Point(651.834, 291.339), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(648.5, 279.672), new Point(657.167, 287.672), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(628.834, 285.006), new Point(639.167, 284.672), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(637.834, 290.339), new Point(625.167, 294.672), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(639.834, 293.672), new Point(644.5, 304.672), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(645.834, 292.339), new Point(650.5, 302.006), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(635.167, 298.672), new Point(641.5, 312.006), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Point(645.167, 308.006), new Point(645.5, 318.006), brush, 4f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(366.25, 310.25), new Point(365.5, 330.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(362, 316.75), new Point(361, 336.25), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(358, 316.25), new Point(357.75, 331.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(354, 324.25), new Point(354, 338), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(359.5, 337.25), new Point(356.75, 342), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(349, 364), new Point(350.5, 375.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(353, 354.25), new Point(355.25, 379.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(357.25, 358.25), new Point(357.5, 380.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(362.5, 360.75), new Point(361, 370.25), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(364.5, 364.5), new Point(361.75, 377), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(364.75, 376.25), new Point(364.25, 392), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(461, 485.5), new Point(475.5, 486.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(468.25, 489.75), new Point(482.5, 490), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(474.75, 495.25), new Point(484, 492.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(478.25, 486), new Point(487, 486.25), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(502, 486), new Point(520.5, 484.25), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(499.5, 491), new Point(512, 490.25), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(506.5, 493), new Point(513, 493), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(524.5, 486.5), new Point(515, 489.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(618.5, 316), new Point(619.5, 332.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(621.75, 321.25), new Point(621.75, 334.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(624.75, 327.5), new Point(624.75, 337.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(627.75, 329.25), new Point(627.5, 342), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(630, 332.75), new Point(629.75, 338.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(620.25, 361.25), new Point(620.75, 379.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(623.25, 359), new Point(623.25, 369.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(626.75, 357.75), new Point(626.75, 368.5), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(624, 371.5), new Point(624, 385), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(630.5, 365.5), new Point(626.25, 376.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Point(620.5, 385.25), new Point(618, 392.75), brush, 2f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(453.5, 533.333), new Point(489.5, 534.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(463.833, 528.667), new Point(493.167, 530), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(473.833, 524.667), new Point(488.5, 524.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(493.833, 531.667), new Point(537.167, 532.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(524.5, 526), new Point(495.833, 527.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(497.167, 522), new Point(513.833, 522), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(516.5, 506), new Point(508.5, 511.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(579, 262.5), new Point(571, 276), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(576, 275), new Point(559, 288), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564, 272.5), new Point(560.5, 286.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(590, 239.5), new Point(584.5, 256.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(589.5, 252.5), new Point(579, 267), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(592, 262.5), new Point(585, 266.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(566, 285.5), new Point(557, 295), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(392, 252), new Point(402.5, 256), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(402, 253), new Point(409.5, 264), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(410, 264), new Point(418, 273.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(418, 270), new Point(422.5, 280.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(422.5, 281), new Point(428.5, 285), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(429.5, 406), new Point(420, 419.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(431, 408.5), new Point(417, 428.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(419, 422.5), new Point(405, 433), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(411, 428.5), new Point(401, 445), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.5, 412.5), new Point(564.5, 426), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(553.5, 411.5), new Point(554, 422.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(551.5, 410.5), new Point(570.5, 410.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(427.833, 285), new Point(414.167, 290), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(418.5, 289), new Point(400.5, 296), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(400.167, 296), new Point(392.833, 300.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(425.833, 286), new Point(412.167, 294), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(426.5, 285.667), new Point(404.833, 289), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(404.5, 289), new Point(392.5, 293.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(392.833, 293.667), new Point(381.833, 301), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(389.167, 302.667), new Point(379.167, 302.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(388.5, 303.667), new Point(382.833, 314), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(392.833, 306.333), new Point(382.5, 315.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(398.5, 305.333), new Point(391.167, 312.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.167, 304.333), new Point(387.5, 312.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(396.833, 305.667), new Point(396.833, 317), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(438.833, 255.001), new Point(434.167, 267.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.167, 267.668), new Point(434.167, 278.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(430.5, 255.001), new Point(430.5, 271.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(430.5, 272.001), new Point(431.833, 281.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(434.833, 240.668), new Point(431.5, 255.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(434.167, 245.001), new Point(445.167, 236.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(440.833, 239.001), new Point(452.833, 234.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(453.5, 233.001), new Point(449.833, 243.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(446.833, 245.001), new Point(456.167, 239.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(451.5, 245.335), new Point(463.5, 242.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(431.167, 286.001), new Point(424.167, 272.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(426.167, 277.335), new Point(422.167, 263.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(423.5, 278.335), new Point(412.5, 274.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(423.833, 280.335), new Point(409.167, 278.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(400.5, 259.668), new Point(389.5, 259.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(400.833, 256.335), new Point(386.5, 259.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(399.5, 254.668), new Point(403.167, 243.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(403.833, 242.001), new Point(403.833, 250.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(401.833, 248.335), new Point(403.5, 257.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.5, 254.668), new Point(397.167, 240.001), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(426.167, 409.666), new Point(409.5, 408.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(409.167, 408.999), new Point(402.167, 407.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(402.167, 407.666), new Point(391.5, 404.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(422.5, 408.333), new Point(408.5, 404.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(408.5, 404.666), new Point(394.167, 395.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(394.5, 395.999), new Point(386.833, 386.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(386.5, 386.333), new Point(386.5, 396.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(387.5, 397.333), new Point(379.5, 388.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(375.5, 386.999), new Point(384.167, 396.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(372.5, 389.333), new Point(378.833, 401.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(375.833, 387.666), new Point(377.167, 401.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(376.167, 400.999), new Point(383.167, 405.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(385.167, 405.999), new Point(380.5, 405.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(377.833, 406.999), new Point(389.167, 413.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(388.833, 413.333), new Point(407.167, 415.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(401.167, 413.999), new Point(416.167, 412.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(415.833, 412.999), new Point(425.833, 411.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(425.167, 417.332), new Point(425.167, 432.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(425.167, 432.666), new Point(427.833, 449.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(428.167, 449.999), new Point(435.167, 460.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.5, 455.999), new Point(440.833, 465.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.5, 461.666), new Point(449.167, 467.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(449.167, 468.332), new Point(461.833, 468.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(443.833, 453.332), new Point(451.833, 461.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(451.5, 461.666), new Point(461.167, 467.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(444.5, 453.666), new Point(454.833, 456.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(443.833, 445.666), new Point(455.5, 454.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(436.833, 436.332), new Point(448.5, 448.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(428.167, 416.999), new Point(436.167, 439.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(427.833, 419.332), new Point(431.833, 434.999), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(431.5, 434.999), new Point(435.5, 447.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(435.5, 447.332), new Point(437.833, 452.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(430.833, 415.999), new Point(437.167, 435.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.5, 435.666), new Point(441.833, 442.332), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(429.833, 411.332), new Point(431.833, 419.666), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.166, 412), new Point(546.833, 427), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(547.166, 427.333), new Point(545.5, 435.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(545.166, 435.333), new Point(537.166, 446.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(536.833, 446.333), new Point(528.5, 455.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(538.833, 450), new Point(527.833, 456.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(537.166, 452), new Point(533.166, 459), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(533.5, 459.333), new Point(526.833, 467.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(522.166, 468), new Point(537.166, 462.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.833, 457.667), new Point(536.833, 466.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.833, 458.667), new Point(540.166, 468), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(537.166, 469.333), new Point(548.166, 463.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(553.166, 418.333), new Point(551.5, 431.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(554.166, 421.667), new Point(547.5, 443.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.833, 433.333), new Point(544.5, 448.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(554.833, 422.333), new Point(552.166, 442.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.166, 443), new Point(547.5, 457.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(547.5, 458), new Point(546.166, 465.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564.5, 424.333), new Point(572.833, 435), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564.166, 424.333), new Point(580.166, 440), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(558.5, 423), new Point(560.5, 432.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(561.166, 432.667), new Point(563.833, 438), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(560.5, 419), new Point(569.5, 424), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(569.833, 424), new Point(577.833, 424), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(562.166, 408.333), new Point(573.166, 399), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(563.833, 407.333), new Point(573.5, 406.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(566.833, 410.333), new Point(583.5, 400.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(583.5, 401), new Point(592.833, 395), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(567.833, 407.667), new Point(582.5, 409.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(582.833, 409.333), new Point(597.166, 403.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(597.166, 403), new Point(607.5, 394.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(607.166, 390.667), new Point(599.166, 392.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(601.166, 394.333), new Point(607.166, 391.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(602.166, 390), new Point(607.166, 382), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(607.166, 381.667), new Point(608.166, 369.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(606.5, 370.333), new Point(601.166, 378), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(605.5, 374.667), new Point(600.833, 378.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(601.166, 376.333), new Point(597.166, 383.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(596.833, 383.333), new Point(589.833, 383.667), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(589.833, 383.667), new Point(599.833, 370), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(553.166, 248.668), new Point(544.833, 241.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(545.166, 241.335), new Point(538.5, 237.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(538.166, 237.335), new Point(537.5, 237.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(532.833, 236.002), new Point(536.166, 244.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(535.5, 237.335), new Point(541.166, 250.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(541.166, 251.002), new Point(529.833, 246.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(546.166, 251.335), new Point(555.166, 264.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.833, 262.002), new Point(556.5, 279.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(556.5, 284.335), new Point(557.166, 265.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(558.5, 269.335), new Point(559.5, 252.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(553.833, 241.335), new Point(561.5, 253.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(553.166, 243.668), new Point(555.5, 252.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(563.833, 292.668), new Point(569.833, 295.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564.833, 290.335), new Point(576.833, 294.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(565.833, 290.002), new Point(580.833, 290.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(576.833, 289.002), new Point(590.5, 289.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(586.833, 291.335), new Point(597.166, 294.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(595.166, 292.335), new Point(600.833, 299.335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(595.833, 298.002), new Point(604.833, 305.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(604.5, 306.002), new Point(607.833, 314.668), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(595.833, 307.002), new Point(604.833, 313.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(595.5, 306.335), new Point(596.5, 316.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(590.5, 312.002), new Point(598.5, 319.002), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(401, 442), new Point(388, 452.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.5, 445), new Point(393.5, 455), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(400.5, 447), new Point(400.5, 456), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(398.5, 440), new Point(392, 441), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(419, 422.5), new Point(405.5, 425), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(421, 415), new Point(407.5, 418.5), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(586.5, 259), new Point(604.5, 259), brush, 4f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(582.5, 441.5), new Point(581.5, 456), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(583.5, 440.5), new Point(590, 454.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(584, 442.5), new Point(595, 445), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(587, 443), new Point(592.5, 453), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(599.5, 443), new Point(602, 449), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.5, 444.5), new Point(386.5, 444), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(400.5, 440), new Point(388.5, 442), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(412.5, 431.5), new Point(419.5, 434), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(415.5, 427), new Point(415, 436), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(573.5, 277), new Point(597.167, 263.333), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(594.834, 263.667), new Point(606.5, 264), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(565.25, 282.75), new Point(567, 264.75), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(395.75, 459), new Point(411.25, 454), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(434.25, 456.25), new Point(417.25, 456.25), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(310.5, 294), new Point(308.5, 346.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(315, 306), new Point(315.5, 347.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(321.5, 319.5), new Point(322, 341.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(590.5, 292.5), new Point(605.5, 307), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(459, 247), new Point(477, 231), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(425, 412), new Point(418.5, 434.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(426.5, 410.5), new Point(403, 428.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(424, 417), new Point(414.5, 438.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(420.5, 418.5), new Point(410, 441.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(558.5, 294), new Point(559, 270.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564.5, 288.5), new Point(574.5, 261), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(571, 273), new Point(583.5, 248), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(568, 279.5), new Point(601, 271.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(565, 287.5), new Point(587, 287), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(567.5, 281.5), new Point(595, 278), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(425, 281.5), new Point(422, 254), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(419.5, 271.5), new Point(397.5, 273.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(417, 268.5), new Point(389, 268.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(417, 269), new Point(387.5, 260), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(416.5, 271.5), new Point(417, 251.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(413.5, 267.5), new Point(417.5, 246.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(417, 246), new Point(412.5, 243.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(411.5, 264), new Point(413.5, 245), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(405.5, 257.5), new Point(400.5, 234), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(402, 252), new Point(397.5, 238.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(398.5, 256), new Point(376, 259), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(562.5, 426), new Point(566.5, 438.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(556, 420.5), new Point(558, 434), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(564.5, 424), new Point(572, 448), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(572, 430), new Point(575.5, 445), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(552.5, 413), new Point(577.5, 418), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(554, 413.5), new Point(579, 412.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(559.5, 416.5), new Point(575.5, 422.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(563.5, 424), new Point(585, 430.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(569.5, 429), new Point(584.5, 437), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(580.5, 439), new Point(597, 440.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(599, 370.5), new Point(585, 384.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(586, 384.5), new Point(574.5, 396.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(577.5, 393.5), new Point(568, 404.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(600, 395.5), new Point(611.5, 392.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(560.5, 290.5), new Point(577.5, 297), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(575.5, 297.5), new Point(588, 307.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(573, 297.5), new Point(591, 315), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(528.5, 248), new Point(545.5, 265.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(542.5, 261), new Point(554.5, 278), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(551, 278.5), new Point(557, 291.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(548.5, 272.5), new Point(556, 283.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(460, 247), new Point(451, 259.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(450.5, 257.5), new Point(439.5, 270.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(442, 266.5), new Point(432.5, 285.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(453.5, 258), new Point(442, 266.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(397.5, 316), new Point(406, 302), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(405.5, 303), new Point(419, 292.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(417, 290), new Point(428.5, 287.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(433.5, 285.5), new Point(422, 288), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(427.5, 287.5), new Point(417.5, 297.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(417.5, 297.5), new Point(406, 303.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(355, 345.5), new Point(367, 337), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(367.5, 337), new Point(379.5, 332.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(380, 332.5), new Point(390, 331), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(390, 331), new Point(391, 331), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(391.5, 331), new Point(392.5, 321.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(393, 321.5), new Point(396, 316), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(356, 348.5), new Point(365, 357.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(365, 357), new Point(374.5, 363), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(375, 363), new Point(385, 368), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(385.5, 367.5), new Point(388, 381), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(388.5, 381), new Point(399.5, 392.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(399, 392.5), new Point(417, 402.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(416.5, 403), new Point(431, 406), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(430.5, 408.5), new Point(437.5, 429.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(437.5, 430), new Point(449.5, 443), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(449, 443), new Point(464, 453.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(464.5, 453.5), new Point(478.5, 461.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(478.5, 462), new Point(480, 471), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(480, 471.5), new Point(487.5, 483), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(498.5, 485), new Point(506.5, 472.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(506.5, 472), new Point(510, 461), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(510, 461), new Point(521.5, 454.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(522, 454.5), new Point(537.5, 444.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(587.5, 312.5), new Point(595.5, 321), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(596, 321.5), new Point(598.5, 331), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(598, 331), new Point(608, 335), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(608, 335), new Point(618, 339.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(618.5, 339.5), new Point(627.5, 347.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(628, 353), new Point(617.5, 356), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(617.5, 356.5), new Point(603.5, 360), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(604, 360), new Point(599, 361.5), brush, 2f);
            }
            {
                brush.Color = colors[6];
                ds.DrawLine(new Point(599.5, 361.5), new Point(593.5, 376), brush, 2f);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(378.5, 450.667), new Point(380.5, 468.667), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(377.833, 458), new Point(371.167, 407.334), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(370.5, 406), new Point(371.167, 294), brush, 5f, strokeStyle);
            }
            {
                brush.Color = colors[6];
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Point(371.167, 296.667), new Point(378.5, 242), brush, 5f, strokeStyle);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(493.5, 297);
                ellipse.RadiusX = 53f;
                ellipse.RadiusY = 53f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(492.5, 401);
                ellipse.RadiusX = 53f;
                ellipse.RadiusY = 53f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(447.5, 349);
                ellipse.RadiusX = 53f;
                ellipse.RadiusY = 53f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(537.5, 351);
                ellipse.RadiusX = 53f;
                ellipse.RadiusY = 53f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(590.5, 346);
                ellipse.RadiusX = 24.5f;
                ellipse.RadiusY = 8.5f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(395.5, 348.5);
                ellipse.RadiusX = 24.5f;
                ellipse.RadiusY = 8.5f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(491.5, 245);
                ellipse.RadiusX = 8.5f;
                ellipse.RadiusY = 24.5f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(494, 451.5);
                ellipse.RadiusX = 8.5f;
                ellipse.RadiusY = 24.5f;
                brush.Color = colors[7];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(253.5, 600.668);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[5];
                ds.DrawEllipse(ellipse, brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(724.5, 589.334);
                ellipse.RadiusX = 6.667f;
                ellipse.RadiusY = 6.667f;
                brush.Color = colors[5];
                ds.DrawEllipse(ellipse, brush, 5f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(457, 311);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(435.667, 308.667);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(452, 288.667);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(534.334, 284.667);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(531.333, 307.333);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(554, 308.667);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(530, 385.667);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(551.334, 389.666);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(532.333, 408);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(451, 386.334);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(449.667, 410.334);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(429.333, 390);
                ellipse.RadiusX = 16f;
                ellipse.RadiusY = 16f;
                brush.Color = Colors.White;
                ds.DrawEllipse(ellipse, brush, 2f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(452.75, 287.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(457.25, 310.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(435.25, 309.25);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(534.25, 283.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(531.25, 307.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(554.25, 309.25);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(450.75, 385.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(428.75, 388.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(450.25, 410.75);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(529.75, 385.25);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(532.75, 408.25);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(551.25, 389.25);
                ellipse.RadiusX = 11.75f;
                ellipse.RadiusY = 11.75f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(435.5, 290.5), new Point(453, 306), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(533.5, 309.5), new Point(552, 293.5), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(533, 388.5), new Point(550.5, 406.5), brush, 20f);
            }
            {
                brush.Color = Colors.White;
                ds.DrawLine(new Point(446.5, 391), new Point(433.5, 404.5), brush, 20f);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(471.5, 265.5);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(517.5, 265.5);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(408, 351);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(582.5, 352.5);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(467.5, 431.5);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(515.5, 434);
                ellipse.RadiusX = 6f;
                ellipse.RadiusY = 6f;
                brush.Color = Colors.White;
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(451.5, 289);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(435, 307.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(457.5, 309.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(536.5, 285.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(533, 307.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(553.5, 306.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(532, 387.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(549, 392.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(532.5, 406);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(450.5, 387);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(449, 409.5);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }
            {
                CanvasEllipse ellipse = new CanvasEllipse();
                ellipse.Point = new Point(430.5, 393);
                ellipse.RadiusX = 8f;
                ellipse.RadiusY = 8f;
                brush.Color = colors[1];
                ds.FillEllipse(ellipse, brush);
            }

            brush.Color = Colors.White;
            ds.FillRectangle(new Rect(980, 0, 1000, 1000), brush);
            ds.FillRectangle(new Rect(0, 735, 1000, 1000), brush);

        }
    }
}
