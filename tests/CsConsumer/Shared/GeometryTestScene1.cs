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

using System;
using System.Diagnostics;
using System.Numerics;
using Windows.UI;
using Windows.Foundation;
using Microsoft.Graphics.Canvas;

namespace CsConsumer
{
    class GeometryTestScene1
    {
        //
        // Factored into a separate file for readability.
        // This test renders a number of geometry primitives to exercise stroke styles..
        // 


        public static void DrawGeometryTestScene(CanvasDrawingSession drawingSession, TestSceneRenderingType renderingType)
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
                ds.DrawCircle(new Vector2(484.5f, 351f), 305f, colors[0], 6f);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawCircle(new Vector2(485f, 351.5f), 294.5f, colors[0], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(219.51f, 144.907f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(842.5f, 348.899f), 505.99f, 464.503f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(145.035f, 354.898f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(481.018f, 683.883f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(485.518f, 23.419f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(222.031f, 560.394f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(751.005f, 569.394f), 505.99f, 464.504f, colors[2], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = dottedDashStyle;
                ds.DrawEllipse(new Vector2(752.005f, 129.915f), 505.99f, 464.503f, colors[2], 6f, strokeStyle);
            }
            ds.DrawLine(new Vector2(352.5f, 471f), new Vector2(398.5f, 573f), Colors.White, 20f);
            ds.DrawLine(new Vector2(407.5f, 573f), new Vector2(500.5f, 534f), Colors.White, 20f);
            ds.DrawLine(new Vector2(579.5f, 564f), new Vector2(502.5f, 538f), Colors.White, 20f);
            ds.DrawLine(new Vector2(595.5f, 568f), new Vector2(631.5f, 482f), Colors.White, 20f);
            ds.DrawLine(new Vector2(703.5f, 442f), new Vector2(619.5f, 472f), Colors.White, 20f);
            ds.DrawLine(new Vector2(709.5f, 435f), new Vector2(675.5f, 354f), Colors.White, 20f);
            ds.DrawLine(new Vector2(704.5f, 272f), new Vector2(681.5f, 337f), Colors.White, 20f);
            ds.DrawLine(new Vector2(698.5f, 265f), new Vector2(620.5f, 234f), Colors.White, 20f);
            ds.DrawLine(new Vector2(591.5f, 133f), new Vector2(622.5f, 198f), Colors.White, 20f);
            ds.DrawLine(new Vector2(581.5f, 129f), new Vector2(507.5f, 162f), Colors.White, 20f);
            ds.DrawLine(new Vector2(404.5f, 128f), new Vector2(470.5f, 155f), Colors.White, 20f);
            ds.DrawLine(new Vector2(395.5f, 129f), new Vector2(366.5f, 191f), Colors.White, 20f);
            ds.DrawLine(new Vector2(272.5f, 259f), new Vector2(348.5f, 233f), Colors.White, 20f);
            ds.DrawLine(new Vector2(270.5f, 267f), new Vector2(289.5f, 322f), Colors.White, 20f);
            ds.DrawLine(new Vector2(262.5f, 436f), new Vector2(295.5f, 363f), Colors.White, 20f);
            ds.DrawLine(new Vector2(269.5f, 445f), new Vector2(345.5f, 467f), Colors.White, 20f);
            ds.DrawLine(new Vector2(259.5f, 161f), new Vector2(305.5f, 160f), Colors.White, 20f);
            ds.DrawLine(new Vector2(310.5f, 115f), new Vector2(306.5f, 162f), Colors.White, 20f);
            ds.DrawLine(new Vector2(266.5f, 160f), new Vector2(305.5f, 122f), Colors.White, 20f);
            ds.DrawLine(new Vector2(191.5f, 320f), new Vector2(220.5f, 355f), Colors.White, 20f);
            ds.DrawLine(new Vector2(193.5f, 390f), new Vector2(214.5f, 361f), Colors.White, 20f);
            ds.DrawLine(new Vector2(196.5f, 333f), new Vector2(195.5f, 382f), Colors.White, 20f);
            ds.DrawLine(new Vector2(456.5f, 58f), new Vector2(487.5f, 79f), Colors.White, 20f);
            ds.DrawLine(new Vector2(528.5f, 60f), new Vector2(498.5f, 88f), Colors.White, 20f);
            ds.DrawLine(new Vector2(518.5f, 62f), new Vector2(467.5f, 64f), Colors.White, 20f);
            ds.DrawLine(new Vector2(667.5f, 122f), new Vector2(678.5f, 158f), Colors.White, 20f);
            ds.DrawLine(new Vector2(719.5f, 174f), new Vector2(674.5f, 171f), Colors.White, 20f);
            ds.DrawLine(new Vector2(672.5f, 134f), new Vector2(711.5f, 170f), Colors.White, 20f);
            ds.DrawLine(new Vector2(777.5f, 321f), new Vector2(752.5f, 346f), Colors.White, 20f);
            ds.DrawLine(new Vector2(774.5f, 395f), new Vector2(754.5f, 366f), Colors.White, 20f);
            ds.DrawLine(new Vector2(773.5f, 330f), new Vector2(772.5f, 389f), Colors.White, 20f);
            ds.DrawLine(new Vector2(720.5f, 532f), new Vector2(688.5f, 536f), Colors.White, 20f);
            ds.DrawLine(new Vector2(711.5f, 533f), new Vector2(676.5f, 569f), Colors.White, 20f);
            ds.DrawLine(new Vector2(256.5f, 534f), new Vector2(283.5f, 545f), Colors.White, 20f);
            ds.DrawLine(new Vector2(316.5f, 590f), new Vector2(301.5f, 558f), Colors.White, 20f);
            ds.DrawLine(new Vector2(268.5f, 543f), new Vector2(306.5f, 581f), Colors.White, 20f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(349.5f, 468f), new Vector2(489.5f, 490f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(630.5f, 469f), new Vector2(474.5f, 490f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(682.5f, 336f), new Vector2(580.5f, 476f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(622.5f, 189f), new Vector2(653.5f, 368f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(677.5f, 354f), new Vector2(568.5f, 222f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(571.5f, 223f), new Vector2(461.5f, 152f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(522.5f, 157f), new Vector2(399.5f, 221f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(407.5f, 224f), new Vector2(325.5f, 302f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(325.5f, 297f), new Vector2(290.5f, 368f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(363.5f, 179f), new Vector2(341.5f, 276f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(323.5f, 233f), new Vector2(560.5f, 217f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(546.5f, 213f), new Vector2(657.5f, 243f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(338.5f, 294f), new Vector2(345.5f, 462f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(284.5f, 320f), new Vector2(343.5f, 404f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(348.5f, 416f), new Vector2(490.5f, 531f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(566.5f, 486f), new Vector2(495.5f, 532f), Colors.White, 20f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(645.5f, 384f), new Vector2(635.5f, 489f), Colors.White, 20f, strokeStyle);
            }
            ds.DrawLine(new Vector2(316f, 41.5f), new Vector2(234.5f, 104f), Colors.White, 80f);
            ds.DrawLine(new Vector2(246.5f, 93.5f), new Vector2(173f, 179.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(317f, 661.5f), new Vector2(235.5f, 599f), Colors.White, 80f);
            ds.DrawLine(new Vector2(247.5f, 609.5f), new Vector2(174f, 523.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(653f, 41.5f), new Vector2(734.5f, 104f), Colors.White, 80f);
            ds.DrawLine(new Vector2(722.5f, 93.5f), new Vector2(796f, 179.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(652f, 661.5f), new Vector2(733.5f, 599f), Colors.White, 80f);
            ds.DrawLine(new Vector2(721.5f, 609.5f), new Vector2(795f, 523.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(145f, 260f), new Vector2(134f, 348.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(132f, 341f), new Vector2(143.5f, 429.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(824f, 260f), new Vector2(835f, 348.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(837f, 341f), new Vector2(825.5f, 429.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(74.61f, -5.391f), new Vector2(74.61f, 736f), Colors.White, 150f);
            ds.DrawLine(new Vector2(874.609f, -6.391f), new Vector2(874.609f, 735f), Colors.White, 150f);
            ds.DrawLine(new Vector2(397.75f, 691.25f), new Vector2(486.25f, 702.25f), Colors.White, 80f);
            ds.DrawLine(new Vector2(478.75f, 704.25f), new Vector2(567.25f, 692.75f), Colors.White, 80f);
            ds.DrawLine(new Vector2(394.75f, 11f), new Vector2(483.25f, 0f), Colors.White, 80f);
            ds.DrawLine(new Vector2(475.75f, -2f), new Vector2(564.25f, 9.5f), Colors.White, 80f);
            ds.DrawLine(new Vector2(268.5f, -18f), new Vector2(254.5f, 42f), Colors.White, 20f);
            ds.DrawLine(new Vector2(144.5f, 101f), new Vector2(206.5f, 94f), Colors.White, 20f);
            ds.DrawLine(new Vector2(140.5f, 603f), new Vector2(214.5f, 606f), Colors.White, 20f);
            ds.DrawLine(new Vector2(244.5f, 646f), new Vector2(287.5f, 757f), Colors.White, 20f);
            ds.DrawLine(new Vector2(721.5f, 639f), new Vector2(676.5f, 772f), Colors.White, 20f);
            ds.DrawLine(new Vector2(691.5f, -19f), new Vector2(717.5f, 57f), Colors.White, 20f);
            ds.DrawLine(new Vector2(768.5f, 102f), new Vector2(844.5f, 112f), Colors.White, 20f);
            ds.DrawLine(new Vector2(927.5f, 131f), new Vector2(1007.5f, 167f), Colors.White, 20f);
            ds.DrawLine(new Vector2(988.5f, 111f), new Vector2(940.5f, 241f), Colors.White, 20f);
            ds.DrawLine(new Vector2(944.5f, 491f), new Vector2(999.5f, 667f), Colors.White, 20f);
            ds.DrawLine(new Vector2(918.5f, 575f), new Vector2(1022.5f, 520f), Colors.White, 20f);
            ds.DrawLine(new Vector2(757.5f, 601f), new Vector2(853.5f, 582f), Colors.White, 20f);
            ds.DrawLine(new Vector2(459.5f, 644f), new Vector2(481.5f, 622f), Colors.White, 20f);
            ds.DrawLine(new Vector2(533.5f, 641f), new Vector2(506.5f, 620f), Colors.White, 20f);
            ds.DrawLine(new Vector2(471.5f, 642f), new Vector2(520.5f, 640f), Colors.White, 20f);
            ds.DrawLine(new Vector2(669.5f, 580f), new Vector2(670.5f, 557f), Colors.White, 20f);
            ds.DrawLine(new Vector2(464.5f, 43f), new Vector2(517.5f, 45f), Colors.White, 20f);
            ds.DrawLine(new Vector2(679.833f, 113.334f), new Vector2(721.167f, 157.334f), Colors.White, 20f);
            ds.DrawLine(new Vector2(789.167f, 333f), new Vector2(789.167f, 378f), Colors.White, 20f);
            ds.DrawLine(new Vector2(463.167f, 654.664f), new Vector2(525.167f, 652.664f), Colors.White, 20f);
            ds.DrawLine(new Vector2(251.833f, 548.665f), new Vector2(298.5f, 596.665f), Colors.White, 20f);
            ds.DrawLine(new Vector2(180.5f, 329f), new Vector2(181.833f, 380.5f), Colors.White, 20f);
            ds.DrawLine(new Vector2(253.5f, 149.667f), new Vector2(297.5f, 109f), Colors.White, 20f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(241.205f, 551.138f), 482.285f, 442.742f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(744.125f, 139.766f), 487.856f, 447.857f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(746.109f, 138.625f), 471.357f, 432.711f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(243.985f, 547f), 457.63f, 420.108f, colors[3], 6f, strokeStyle);
            }
            ds.FillCircle(new Vector2(297.5f, 267.333f), 11.333f, colors[1]);
            ds.FillCircle(new Vector2(413.167f, 153.667f), 9f, colors[1]);
            ds.FillCircle(new Vector2(384.5f, 239.334f), 6.667f, colors[1]);
            ds.DrawLine(new Vector2(714.079f, 590.08f), new Vector2(703.5f, 785f), Colors.White, 40f);
            ds.DrawLine(new Vector2(726.079f, 578.58f), new Vector2(865.167f, 584f), Colors.White, 40f);
            ds.DrawLine(new Vector2(831.573f, 574.08f), new Vector2(1016.064f, 508.083f), Colors.White, 40f);
            ds.DrawLine(new Vector2(293.167f, 110.667f), new Vector2(291.833f, 152f), colors[4], 5f);
            ds.DrawLine(new Vector2(251.833f, 149.333f), new Vector2(289.167f, 149.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(268.5f, 264f), new Vector2(311.833f, 246f), colors[4], 5f);
            ds.DrawLine(new Vector2(311.833f, 246f), new Vector2(321.167f, 276f), colors[4], 5f);
            ds.DrawLine(new Vector2(372.5f, 224f), new Vector2(367.167f, 260.667f), colors[4], 5f);
            ds.DrawLine(new Vector2(373.167f, 226.667f), new Vector2(403.167f, 223.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(405.833f, 131.333f), new Vector2(391.167f, 162.667f), colors[4], 5f);
            ds.DrawLine(new Vector2(391.167f, 163.333f), new Vector2(416.5f, 173.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(297.167f, 269.333f), new Vector2(385.167f, 237.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(421.167f, 136.667f), new Vector2(380.5f, 240f), colors[4], 5f);
            ds.DrawLine(new Vector2(381.167f, 239.333f), new Vector2(379.833f, 256f), colors[4], 5f);
            ds.DrawLine(new Vector2(381.833f, 237.333f), new Vector2(395.167f, 238f), colors[4], 5f);
            ds.DrawLine(new Vector2(301.167f, 284.667f), new Vector2(294.5f, 263.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(296.5f, 270.667f), new Vector2(281.167f, 279.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(397.167f, 147.333f), new Vector2(425.167f, 161.333f), colors[4], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(448.5f, 159.667f), new Vector2(516.5f, 190.333f), Colors.White, 40f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(301.167f, 306f), new Vector2(325.833f, 360.667f), Colors.White, 40f, strokeStyle);
            }
            ds.DrawLine(new Vector2(252.5f, 118f), new Vector2(124.5f, 124.667f), Colors.White, 40f);
            ds.DrawLine(new Vector2(267.167f, 105.333f), new Vector2(272.5f, 44f), Colors.White, 40f);
            ds.DrawLine(new Vector2(274.5f, 49.333f), new Vector2(295.167f, -20f), Colors.White, 40f);
            ds.DrawLine(new Vector2(266.5f, 118f), new Vector2(283.167f, 118.667f), Colors.White, 12f);
            ds.DrawLine(new Vector2(270.5f, 126.667f), new Vector2(260.5f, 126f), Colors.White, 12f);
            ds.DrawCircle(new Vector2(254.167f, 105.001f), 6.667f, colors[5], 5f);
            ds.DrawLine(new Vector2(298f, 281f), new Vector2(391.5f, 249.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(429.5f, 144f), new Vector2(390.5f, 251.5f), colors[6], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(377.667f, 225.834f), new Vector2(363f, 220.001f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(363f, 220.001f), new Vector2(347.833f, 220.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(347.667f, 220.168f), new Vector2(332.667f, 225.334f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(332.833f, 225.334f), new Vector2(320.167f, 237.501f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(320.167f, 237.501f), new Vector2(319.833f, 227.834f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(320f, 228.334f), new Vector2(313.667f, 232.501f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(313.5f, 232.501f), new Vector2(316.167f, 223.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(316.167f, 223.001f), new Vector2(323.167f, 215.001f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(316.667f, 221.501f), new Vector2(308.333f, 227.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(308.333f, 227.834f), new Vector2(308.333f, 221.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(308.333f, 221.001f), new Vector2(303.333f, 225.001f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(303.167f, 225.001f), new Vector2(305.167f, 219.834f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(305.167f, 219.668f), new Vector2(301f, 214.501f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(301f, 214.501f), new Vector2(308.167f, 213.834f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(308.167f, 213.668f), new Vector2(301.833f, 206.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(301.833f, 206.668f), new Vector2(310.5f, 208.501f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(310.333f, 208.501f), new Vector2(310.667f, 203.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(310.667f, 203.501f), new Vector2(339.167f, 205.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(339.333f, 205.168f), new Vector2(336.333f, 199.834f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(336.333f, 199.834f), new Vector2(292.333f, 173.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(292.167f, 173.168f), new Vector2(296.167f, 173.501f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(296.167f, 173.334f), new Vector2(289.667f, 167.334f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(289.667f, 167.334f), new Vector2(299.667f, 168.334f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(300.333f, 168.168f), new Vector2(290.333f, 149.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(290.333f, 149.168f), new Vector2(293.5f, 149.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(293.667f, 149.168f), new Vector2(293.167f, 143.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(293.167f, 143.002f), new Vector2(297.5f, 149.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(297.667f, 149.168f), new Vector2(314f, 157.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(314f, 157.502f), new Vector2(313.833f, 149.502f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(313.833f, 149.502f), new Vector2(317f, 152.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(317f, 152.668f), new Vector2(317.167f, 144.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(317.167f, 144.002f), new Vector2(343f, 184.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(343f, 184.502f), new Vector2(355.5f, 201.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(355.5f, 201.668f), new Vector2(360.667f, 205.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(360.833f, 205.168f), new Vector2(356f, 180.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(356.167f, 180.168f), new Vector2(354.167f, 156.835f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(354.167f, 156.668f), new Vector2(360f, 159.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(360.167f, 159.168f), new Vector2(364.167f, 150.335f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(364.167f, 150.168f), new Vector2(368.5f, 163.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(368.333f, 163.168f), new Vector2(372.333f, 143.335f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(372.333f, 143.502f), new Vector2(377.333f, 149.502f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(377.333f, 149.668f), new Vector2(380.167f, 146.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(380.333f, 146.168f), new Vector2(382.5f, 161.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(382.5f, 161.502f), new Vector2(384.5f, 165.835f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(384.5f, 165.835f), new Vector2(389.5f, 170.502f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(389.5f, 170.502f), new Vector2(377f, 174.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(377.167f, 174.668f), new Vector2(366.667f, 182.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(366.5f, 182.168f), new Vector2(379.5f, 177.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(379.333f, 177.168f), new Vector2(384f, 176.835f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(384f, 176.835f), new Vector2(376.833f, 182.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(376.667f, 182.335f), new Vector2(371.5f, 187.835f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(372f, 187.668f), new Vector2(375.333f, 186.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(375.5f, 186.168f), new Vector2(373.667f, 198.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(374.5f, 196.335f), new Vector2(372.667f, 209.502f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(372.5f, 209.502f), new Vector2(376.167f, 222.835f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(362f, 218.502f), new Vector2(344.667f, 215.168f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(344.833f, 215.335f), new Vector2(327.5f, 213.835f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(313.667f, 169.335f), new Vector2(335.5f, 186.835f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(335.333f, 186.835f), new Vector2(350.333f, 208.668f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(350.5f, 208.835f), new Vector2(354.167f, 216.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(327.833f, 179.668f), new Vector2(341.5f, 190.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(341.333f, 190.002f), new Vector2(355.167f, 211.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(355.167f, 211.502f), new Vector2(359f, 217.668f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(367.667f, 175.668f), new Vector2(365.833f, 186.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(365.833f, 186.502f), new Vector2(366.5f, 209.168f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(366.5f, 209.335f), new Vector2(366.5f, 218.168f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(367f, 180.335f), new Vector2(369f, 196.835f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(369f, 196.835f), new Vector2(368.333f, 212.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(368.333f, 212.002f), new Vector2(371.667f, 223.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(293.833f, 149.668f), new Vector2(304.167f, 169.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(304f, 169.002f), new Vector2(297.667f, 171.502f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(297.5f, 171.502f), new Vector2(302.5f, 175.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(302.5f, 175.502f), new Vector2(336.333f, 195.835f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(337f, 195.335f), new Vector2(351.833f, 215.835f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(306f, 212.835f), new Vector2(317.333f, 207.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(317.5f, 207.668f), new Vector2(347.333f, 209.335f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(306.167f, 219.502f), new Vector2(321.833f, 207.335f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(345.333f, 190.502f), new Vector2(364.333f, 217.835f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(360.833f, 204.668f), new Vector2(371.667f, 222.835f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(361f, 200.335f), new Vector2(375f, 219.002f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(301.667f, 175.336f), new Vector2(324f, 153.503f), colors[6], 2f);
            ds.DrawLine(new Vector2(313f, 157.336f), new Vector2(303.333f, 167.503f), colors[6], 2f);
            ds.DrawLine(new Vector2(307.833f, 154.836f), new Vector2(301f, 163.503f), colors[6], 2f);
            ds.DrawLine(new Vector2(305.667f, 161.503f), new Vector2(302f, 165.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(305f, 153.669f), new Vector2(299.5f, 159.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(303.167f, 151.836f), new Vector2(298f, 157.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(304f, 169.336f), new Vector2(299.833f, 174.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(325.167f, 157.169f), new Vector2(305.167f, 178.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(328.667f, 162.503f), new Vector2(309.833f, 180.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(330.667f, 165.836f), new Vector2(313.833f, 182.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(317.667f, 184.836f), new Vector2(332.833f, 171.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(335.5f, 174.669f), new Vector2(321.333f, 188.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(338.5f, 177.336f), new Vector2(326.833f, 190.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(340.167f, 181.503f), new Vector2(331.333f, 192.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(324.333f, 209.003f), new Vector2(308.167f, 224.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(331.667f, 210.003f), new Vector2(314.833f, 228.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(320f, 228.669f), new Vector2(337.333f, 208.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(339.5f, 209.169f), new Vector2(324.333f, 232.503f), colors[6], 2f);
            ds.DrawLine(new Vector2(343.333f, 209.003f), new Vector2(337.333f, 223.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(346.167f, 210.003f), new Vector2(343.5f, 221.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(354.5f, 168.836f), new Vector2(366.5f, 155.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(359.833f, 158.503f), new Vector2(353.833f, 165.169f), colors[6], 2f);
            ds.DrawLine(new Vector2(367.5f, 160.336f), new Vector2(356f, 173.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(378f, 148.503f), new Vector2(369.667f, 158.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(375.833f, 146.836f), new Vector2(370.833f, 152.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(380.5f, 153.836f), new Vector2(356.833f, 180.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(381f, 149.003f), new Vector2(376.167f, 154.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(382f, 158.669f), new Vector2(357.667f, 185.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(385.167f, 163.503f), new Vector2(372.667f, 178.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(385.333f, 167.503f), new Vector2(380.167f, 174.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(373.667f, 174.003f), new Vector2(358.333f, 191.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(378.333f, 178.169f), new Vector2(359f, 195.836f), colors[6], 2f);
            ds.DrawLine(new Vector2(374f, 188.169f), new Vector2(360.667f, 199.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(374f, 193.503f), new Vector2(362.667f, 202.503f), colors[6], 2f);
            ds.DrawLine(new Vector2(374.5f, 196.669f), new Vector2(367f, 203.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(374.5f, 200.836f), new Vector2(368.167f, 206.836f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(285f, 157.169f), new Vector2(285.5f, 177.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(290.333f, 179.836f), new Vector2(284.667f, 198.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(297.5f, 182.669f), new Vector2(292.333f, 203.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(287.5f, 201.336f), new Vector2(289.833f, 222.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(294.167f, 204.669f), new Vector2(297f, 223.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(291.333f, 224.003f), new Vector2(294.5f, 243.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(297.5f, 226.169f), new Vector2(300.167f, 245.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(304.5f, 231.503f), new Vector2(316.333f, 243.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(304.333f, 187.003f), new Vector2(299.5f, 205.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(307.5f, 199.003f), new Vector2(321.167f, 198.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(309f, 192.669f), new Vector2(323.333f, 192.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(322f, 198.669f), new Vector2(333f, 201.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(301.833f, 146.336f), new Vector2(310.667f, 132.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(308.333f, 150.503f), new Vector2(317f, 136.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(297f, 140.836f), new Vector2(300.167f, 131.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(327.833f, 136.503f), new Vector2(319.333f, 148.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(332.667f, 140.503f), new Vector2(326.833f, 159.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(336.5f, 147.503f), new Vector2(336.833f, 164.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(338.833f, 140.503f), new Vector2(347.833f, 159.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(345.667f, 159.003f), new Vector2(336.667f, 174.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(348.333f, 142.169f), new Vector2(352.333f, 162.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(360.167f, 145.336f), new Vector2(354.667f, 157.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(350.667f, 163.003f), new Vector2(350.167f, 181.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(345.5f, 170.169f), new Vector2(343.5f, 183.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(348.167f, 184.336f), new Vector2(354.333f, 194.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(347.5f, 226.669f), new Vector2(346.333f, 246.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(354.5f, 225.003f), new Vector2(357.167f, 235.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(365.333f, 238.169f), new Vector2(350.667f, 245.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(361.833f, 225.169f), new Vector2(368.333f, 232.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(341.167f, 234.336f), new Vector2(329.833f, 241.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(341.5f, 226.836f), new Vector2(328.5f, 234.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(326.5f, 243.336f), new Vector2(345.333f, 246.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(335f, 249.669f), new Vector2(321.667f, 255.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(325.667f, 248.336f), new Vector2(314.667f, 247.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(379.333f, 219.503f), new Vector2(382.333f, 203.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(378.333f, 200.503f), new Vector2(388.167f, 195.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(387.5f, 190.003f), new Vector2(375.833f, 192.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(389.167f, 183.003f), new Vector2(395.333f, 193.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(391.167f, 176.003f), new Vector2(400.167f, 185.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(395f, 169.336f), new Vector2(401.333f, 178.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(384.833f, 154.169f), new Vector2(389.333f, 164.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(375.833f, 208.336f), new Vector2(385.667f, 211.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(388.667f, 202.169f), new Vector2(388.667f, 214.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(390.667f, 194.503f), new Vector2(393.5f, 201.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(279.333f, 157.003f), new Vector2(280.167f, 178.503f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(281.5f, 183.669f), new Vector2(283.833f, 197.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(283.667f, 196.669f), new Vector2(286.667f, 205.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(287.5f, 218.336f), new Vector2(289.333f, 225.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(291.667f, 236.169f), new Vector2(294.333f, 247.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(300.333f, 246.003f), new Vector2(309.333f, 239.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                ds.DrawLine(new Vector2(293.667f, 162.336f), new Vector2(287.667f, 158.003f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(354.5f, 172.5f), new Vector2(353.833f, 182.167f), colors[6], 4f);
            ds.DrawLine(new Vector2(356.833f, 182.333f), new Vector2(355.167f, 190.167f), colors[6], 4f);
            ds.DrawLine(new Vector2(358.333f, 191.167f), new Vector2(358.5f, 202f), colors[6], 4f);
            ds.DrawLine(new Vector2(321f, 145.667f), new Vector2(323.667f, 152.667f), colors[6], 4f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(736.795f, 558.805f), 482.285f, 442.742f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(227.875f, 150.432f), 487.856f, 447.857f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(226.892f, 151.291f), 471.357f, 432.711f, colors[3], 6f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.DashStyle = predefinedDashStyle;
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawEllipse(new Vector2(732.015f, 554.666f), 457.63f, 420.109f, colors[3], 6f, strokeStyle);
            }
            ds.FillCircle(new Vector2(574.833f, 156.833f), 9f, colors[1]);
            ds.FillCircle(new Vector2(601.5f, 245f), 6.667f, colors[1]);
            ds.DrawLine(new Vector2(463.91f, 192.267f), new Vector2(333.417f, 336.259f), Colors.White, 40f);
            ds.DrawLine(new Vector2(270.003f, 599.508f), new Vector2(281.833f, 745f), Colors.White, 40f);
            ds.DrawLine(new Vector2(252.5f, 590.5f), new Vector2(112.007f, 595.5f), Colors.White, 40f);
            ds.DrawLine(new Vector2(141.427f, 588.746f), new Vector2(-43.064f, 522.75f), Colors.White, 40f);
            ds.DrawLine(new Vector2(718.167f, 156f), new Vector2(680.833f, 156f), colors[4], 5f);
            ds.DrawLine(new Vector2(583.167f, 137f), new Vector2(597.833f, 168.333f), colors[4], 5f);
            ds.DrawLine(new Vector2(571.833f, 151.333f), new Vector2(612.5f, 254.667f), colors[4], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(671.833f, 320.667f), new Vector2(647.167f, 375.333f), Colors.White, 40f, strokeStyle);
            }
            ds.DrawLine(new Vector2(719.5f, 130.667f), new Vector2(847.5f, 137.333f), Colors.White, 40f);
            ds.DrawLine(new Vector2(705.833f, 116f), new Vector2(700.5f, 54.667f), Colors.White, 40f);
            ds.DrawLine(new Vector2(698.5f, 64f), new Vector2(677.833f, -5.333f), Colors.White, 40f);
            ds.DrawLine(new Vector2(701.5f, 138.333f), new Vector2(711.5f, 137.667f), Colors.White, 12f);
            ds.DrawCircle(new Vector2(716.833f, 115.667f), 6.667f, colors[5], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(595.333f, 240.501f), new Vector2(610.001f, 234.668f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(604.667f, 155.835f), new Vector2(602.501f, 171.335f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(605.667f, 186.835f), new Vector2(601.001f, 186.502f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(604.667f, 226.668f), new Vector2(601.333f, 237.668f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(645.167f, 151.169f), new Vector2(653.667f, 163.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(600.167f, 163.836f), new Vector2(595.667f, 174.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(692.667f, 171.669f), new Vector2(691.833f, 193.169f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(689.501f, 198.336f), new Vector2(687.167f, 211.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(689.333f, 211.336f), new Vector2(686.333f, 219.836f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(685.501f, 233.002f), new Vector2(683.667f, 240.502f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(681.333f, 250.836f), new Vector2(678.667f, 262.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(656f, 153.5f), new Vector2(633f, 201.5f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(629.5f, 157.002f), new Vector2(630.5f, 174.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(630.5f, 174.669f), new Vector2(628.5f, 190.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(628.5f, 190.669f), new Vector2(623.834f, 209.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(628.167f, 154.669f), new Vector2(620.167f, 167.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(606.167f, 155.669f), new Vector2(619.834f, 164.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(663.5f, 144.669f), new Vector2(658.167f, 155.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(601.5f, 173.002f), new Vector2(593.834f, 176.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(597.5f, 176.669f), new Vector2(606.167f, 187.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(606.167f, 187.669f), new Vector2(609.167f, 200.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(609.167f, 200.336f), new Vector2(611.834f, 217.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(614.5f, 201.669f), new Vector2(610.834f, 230.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(612.834f, 228.002f), new Vector2(629.834f, 228.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(657.834f, 224.336f), new Vector2(634.5f, 223.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(648.834f, 220.002f), new Vector2(622.167f, 226.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(615.167f, 176.669f), new Vector2(618.167f, 197.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(620.834f, 187.336f), new Vector2(616.167f, 215.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(657.5f, 180.336f), new Vector2(637.167f, 206.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(668.834f, 172.002f), new Vector2(646.167f, 199.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(663.834f, 144.669f), new Vector2(667.167f, 161.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(682.167f, 153.336f), new Vector2(668.167f, 161.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(684.834f, 158.336f), new Vector2(677.834f, 171.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(687.167f, 166.002f), new Vector2(674.167f, 175.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(675.834f, 175.002f), new Vector2(686.167f, 174.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(690.834f, 174.002f), new Vector2(682.5f, 178.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(691.167f, 179.002f), new Vector2(673.834f, 188.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(682.834f, 187.669f), new Vector2(648.5f, 199.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(657.5f, 198.002f), new Vector2(648.834f, 211.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(674.167f, 208.669f), new Vector2(648.5f, 210.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(667.834f, 209.669f), new Vector2(680.834f, 214.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(681.5f, 214.336f), new Vector2(670.834f, 218.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(671.167f, 218.669f), new Vector2(684.167f, 226.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(678.5f, 225.336f), new Vector2(684.5f, 236.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(684.167f, 236.002f), new Vector2(668.167f, 239.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(664.834f, 236.002f), new Vector2(664.834f, 248.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(664.167f, 247.669f), new Vector2(659.167f, 247.336f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(651.5f, 238.336f), new Vector2(660.5f, 254.669f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(652.834f, 237.669f), new Vector2(622.5f, 230.002f), colors[6], 3f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(632.167f, 195.336f), new Vector2(616.167f, 226.002f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(639.834f, 199.669f), new Vector2(620.5f, 225.669f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(653.167f, 200.669f), new Vector2(625.167f, 222.669f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(671.5f, 252.336f), new Vector2(698.834f, 265.002f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(669.834f, 252.669f), new Vector2(656.834f, 295.002f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(611.5f, 246.002f), new Vector2(663.167f, 265.336f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(665.167f, 265.669f), new Vector2(696.167f, 282.002f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(603.167f, 256.002f), new Vector2(660.5f, 278.669f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(660.5f, 278.669f), new Vector2(690.5f, 295.669f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(595.834f, 166.336f), new Vector2(565.167f, 175.669f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(554.5f, 151.002f), new Vector2(567.5f, 178.336f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(660.5f, 154.669f), new Vector2(668.167f, 160.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(657.167f, 157.002f), new Vector2(678.167f, 175.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(669.167f, 160.002f), new Vector2(679.167f, 170.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(673.167f, 158.336f), new Vector2(681.5f, 165.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(676.834f, 157.336f), new Vector2(682.167f, 163.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(653.5f, 160.002f), new Vector2(682.167f, 184.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(650.167f, 162.336f), new Vector2(676.5f, 185.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(648.834f, 166.669f), new Vector2(672.5f, 188.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(647.834f, 171.669f), new Vector2(669.834f, 191.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(644.834f, 175.336f), new Vector2(665.167f, 192.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(644.167f, 179.336f), new Vector2(659.5f, 194.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(641.5f, 184.002f), new Vector2(657.5f, 197.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(639.167f, 188.336f), new Vector2(653.5f, 200.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(638.167f, 192.002f), new Vector2(647.5f, 198.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(623.167f, 162.669f), new Vector2(630.167f, 168.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(621.834f, 166.002f), new Vector2(628.834f, 172.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(604.834f, 158.669f), new Vector2(629.167f, 176.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(603.834f, 160.669f), new Vector2(629.167f, 182.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(604.834f, 165.669f), new Vector2(629.5f, 186.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(602.5f, 169.002f), new Vector2(628.167f, 191.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(600.834f, 172.336f), new Vector2(626.5f, 195.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(598.834f, 175.336f), new Vector2(626.834f, 198.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(606.5f, 188.336f), new Vector2(626.167f, 203.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(606.834f, 194.669f), new Vector2(626.167f, 207.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(653.167f, 209.336f), new Vector2(682.834f, 234.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(660.167f, 211.002f), new Vector2(678.834f, 227.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(665.167f, 211.002f), new Vector2(673.5f, 218.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(650.834f, 211.336f), new Vector2(677.834f, 237.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(643.167f, 210.002f), new Vector2(674.167f, 239.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(639.5f, 212.002f), new Vector2(668.167f, 240.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(634.167f, 214.336f), new Vector2(663.834f, 240.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(633.5f, 218.669f), new Vector2(663.834f, 246.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(627.834f, 218.336f), new Vector2(645.5f, 235.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(557.5f, 164.333f), new Vector2(523.833f, 179.333f), Colors.White, 40f);
            ds.DrawLine(new Vector2(524.833f, 168f), new Vector2(466.833f, 204.667f), Colors.White, 40f);
            ds.DrawLine(new Vector2(674.5f, 289f), new Vector2(662.833f, 311.333f), Colors.White, 40f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(567.167f, 177.667f), new Vector2(585.833f, 228f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(586.5f, 227.667f), new Vector2(589.5f, 257f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(589.167f, 257.333f), new Vector2(621.167f, 262.667f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(585.833f, 227.667f), new Vector2(611.167f, 229.667f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(610.167f, 229.667f), new Vector2(621.5f, 263.667f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(584.833f, 244.667f), new Vector2(564.5f, 245.667f), Colors.White, 20f);
            ds.DrawLine(new Vector2(681.833f, 121.667f), new Vector2(682.167f, 157f), colors[4], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(645.25f, 157.75f), new Vector2(641f, 174f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(634f, 157f), new Vector2(636f, 168.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(639f, 154.25f), new Vector2(644.5f, 143f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(650.25f, 149f), new Vector2(661.25f, 144f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(669.25f, 146f), new Vector2(679.75f, 145f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(670.25f, 152.75f), new Vector2(678.5f, 150.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(689.75f, 166f), new Vector2(694f, 160f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(683f, 196.25f), new Vector2(682.75f, 208.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(685.25f, 192.5f), new Vector2(688.25f, 184.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(678.75f, 192.5f), new Vector2(677.5f, 200.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(673f, 200.75f), new Vector2(663.25f, 206f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(673f, 195.25f), new Vector2(662.75f, 200f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(682.75f, 210.5f), new Vector2(683.25f, 222f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(674.75f, 241.75f), new Vector2(682.75f, 247.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(668.5f, 244.25f), new Vector2(678f, 249.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(664.75f, 261.75f), new Vector2(657f, 256.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(655f, 251.5f), new Vector2(645.5f, 248f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(644.75f, 243.25f), new Vector2(633f, 241.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(646f, 253.75f), new Vector2(656.25f, 257.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(641.75f, 251f), new Vector2(632.25f, 246.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(629.75f, 240.75f), new Vector2(618f, 233.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(629.75f, 248.25f), new Vector2(619.75f, 241.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(607f, 203.25f), new Vector2(606.5f, 215.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(604.5f, 194f), new Vector2(597.25f, 202f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(599.5f, 189.75f), new Vector2(592.5f, 196.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(597.5f, 183.75f), new Vector2(589.5f, 188.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(585.5f, 174.5f), new Vector2(589.5f, 185.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(590.5f, 171.25f), new Vector2(592.75f, 180.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(611.5f, 154.75f), new Vector2(621.25f, 158f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(618.25f, 151.75f), new Vector2(628.25f, 153.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(631.5f, 149.25f), new Vector2(639.5f, 150.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(635.75f, 170f), new Vector2(635f, 184f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(629.75f, 234.75f), new Vector2(641.25f, 239.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(600.75f, 215f), new Vector2(610f, 220f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(595.75f, 203f), new Vector2(604.25f, 211f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(525.25f, 202.25f), new Vector2(570.25f, 249f), Colors.White, 40f);
            ds.DrawLine(new Vector2(602.75f, 279f), new Vector2(637.5f, 320f), Colors.White, 40f);
            ds.DrawLine(new Vector2(607.75f, 263.25f), new Vector2(604f, 289.75f), Colors.White, 20f);
            ds.DrawLine(new Vector2(566f, 253f), new Vector2(597f, 285.75f), Colors.White, 20f);
            ds.DrawLine(new Vector2(692.75f, 127.5f), new Vector2(703.25f, 127f), Colors.White, 12f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(618.5f, 228.667f), new Vector2(611.833f, 238.667f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(619.5f, 206f), new Vector2(612.5f, 227.667f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(647.833f, 184f), new Vector2(639.833f, 211f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(646.833f, 234.333f), new Vector2(625.5f, 227f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(568.167f, 143.667f), new Vector2(575.5f, 157.667f), colors[6], 5f, strokeStyle);
            }
            ds.FillCircle(new Vector2(294.5f, 437f), 11.333f, colors[1]);
            ds.FillCircle(new Vector2(600.834f, 463.333f), 8.667f, colors[1]);
            ds.FillCircle(new Vector2(686.167f, 432.332f), 11.333f, colors[1]);
            ds.FillCircle(new Vector2(381.501f, 465.666f), 8.667f, colors[1]);
            ds.DrawLine(new Vector2(283.167f, 412.002f), new Vector2(313.167f, 428.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(312.5f, 428.669f), new Vector2(301.833f, 459.336f), colors[6], 5f);
            ds.DrawLine(new Vector2(302.5f, 459.336f), new Vector2(271.167f, 445.336f), colors[6], 5f);
            ds.DrawLine(new Vector2(363.167f, 448.336f), new Vector2(393.167f, 453.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(393.167f, 454.669f), new Vector2(399.167f, 482.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(397.833f, 481.336f), new Vector2(365.833f, 476.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(365.833f, 476.002f), new Vector2(365.167f, 448.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(311.167f, 428.002f), new Vector2(363.833f, 448.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(308.5f, 442.002f), new Vector2(365.167f, 464.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(251.167f, 550.669f), new Vector2(291.833f, 555.336f), colors[6], 5f);
            ds.DrawLine(new Vector2(291.833f, 554.669f), new Vector2(297.167f, 594.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(390.5f, 536.669f), new Vector2(405.167f, 572.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(390.5f, 538.002f), new Vector2(421.167f, 530.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(419.833f, 530.002f), new Vector2(433.833f, 561.336f), colors[6], 5f);
            ds.DrawLine(new Vector2(399.167f, 482.669f), new Vector2(416.5f, 528.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(387.167f, 481.336f), new Vector2(406.5f, 534.002f), colors[6], 5f);
            ds.FillCircle(new Vector2(413.167f, 549.667f), 9f, colors[1]);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(364f, 468f), new Vector2(338f, 487f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(363f, 471f), new Vector2(331.5f, 507.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(337.5f, 500f), new Vector2(293f, 527f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(293f, 527.5f), new Vector2(278.5f, 535f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(313f, 518.5f), new Vector2(284.5f, 540f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(330f, 508.5f), new Vector2(315f, 530f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(364f, 476.5f), new Vector2(336.5f, 510.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(363.5f, 478f), new Vector2(317.5f, 545f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(312.5f, 542f), new Vector2(294f, 553.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(300.5f, 536f), new Vector2(293f, 551.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(302.5f, 532.5f), new Vector2(290f, 540f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(310f, 545f), new Vector2(307.5f, 556f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(316.5f, 542.5f), new Vector2(304.5f, 566f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(317f, 546.5f), new Vector2(311.5f, 555f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(350f, 484.5f), new Vector2(322.5f, 500f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(323.5f, 497.5f), new Vector2(304.5f, 500.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(306.5f, 499f), new Vector2(290.5f, 496.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(305f, 487.5f), new Vector2(293.5f, 494.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(304f, 485.5f), new Vector2(290.5f, 474f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(308f, 467.5f), new Vector2(294.5f, 471.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(308.5f, 456f), new Vector2(309f, 468f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(309f, 459.5f), new Vector2(333.5f, 474.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(352.5f, 473.5f), new Vector2(332f, 473.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(345f, 481.5f), new Vector2(328f, 483.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(340.5f, 485.5f), new Vector2(311.5f, 483f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(361f, 485f), new Vector2(356.5f, 511f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(365f, 477f), new Vector2(364.5f, 492.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(366.5f, 477f), new Vector2(370f, 511.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(369f, 509f), new Vector2(379f, 527f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(372.5f, 522.5f), new Vector2(390f, 536.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(352f, 504f), new Vector2(351f, 536.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(347.5f, 535f), new Vector2(352.5f, 554f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(354f, 537f), new Vector2(354f, 550f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(355f, 536.5f), new Vector2(365f, 548f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(365f, 539.5f), new Vector2(375f, 551f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(375f, 533f), new Vector2(375f, 552.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(375f, 531f), new Vector2(383f, 538f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(311.5f, 462f), new Vector2(335.5f, 483f), colors[6], 2f);
            ds.DrawLine(new Vector2(309f, 467.5f), new Vector2(336f, 488.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(305.5f, 469.5f), new Vector2(336.5f, 494f), colors[6], 2f);
            ds.DrawLine(new Vector2(297.5f, 470.5f), new Vector2(331.5f, 497f), colors[6], 2f);
            ds.DrawLine(new Vector2(294.5f, 473f), new Vector2(323.5f, 498f), colors[6], 2f);
            ds.DrawLine(new Vector2(303.5f, 487.5f), new Vector2(319f, 498.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(300.5f, 491f), new Vector2(313f, 501.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(296f, 491f), new Vector2(304f, 498.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(335f, 475f), new Vector2(346.5f, 486.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(353.5f, 512.75f), new Vector2(374f, 539f), colors[6], 2f);
            ds.DrawLine(new Vector2(353.5f, 523.5f), new Vector2(373.25f, 540f), colors[6], 2f);
            ds.DrawLine(new Vector2(354.75f, 528.75f), new Vector2(364.75f, 541.25f), colors[6], 2f);
            ds.DrawLine(new Vector2(356f, 513f), new Vector2(375f, 531.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(357f, 508f), new Vector2(371.75f, 522.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(359f, 504.75f), new Vector2(369.5f, 514.25f), colors[6], 2f);
            ds.DrawLine(new Vector2(360.25f, 500.5f), new Vector2(368.75f, 506.25f), colors[6], 2f);
            ds.DrawLine(new Vector2(361.25f, 494f), new Vector2(366.25f, 498.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(298f, 543.25f), new Vector2(302.5f, 547.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(300.5f, 537.75f), new Vector2(305.75f, 544.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(302f, 534.75f), new Vector2(310.25f, 542.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(303f, 528f), new Vector2(315f, 542f), colors[6], 2f);
            ds.DrawLine(new Vector2(305.5f, 523.75f), new Vector2(320f, 540.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(308.75f, 521.75f), new Vector2(322.25f, 536.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(313f, 519f), new Vector2(324.5f, 533.25f), colors[6], 2f);
            ds.DrawLine(new Vector2(316.75f, 514f), new Vector2(327f, 528f), colors[6], 2f);
            ds.DrawLine(new Vector2(318.5f, 510.75f), new Vector2(331f, 525.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(324.25f, 510.25f), new Vector2(333.5f, 519f), colors[6], 2f);
            ds.DrawLine(new Vector2(346.75f, 491.5f), new Vector2(324.25f, 525.5f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(335.25f, 527f), new Vector2(332.75f, 545f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(343.5f, 532f), new Vector2(331.75f, 551.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(346.5f, 508.75f), new Vector2(346.5f, 526f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(345.5f, 545.5f), new Vector2(331f, 562f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(324.75f, 555.5f), new Vector2(315.5f, 568.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(328.5f, 535.5f), new Vector2(309f, 571.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(298.75f, 507f), new Vector2(281.5f, 501.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(283.75f, 510.25f), new Vector2(294.75f, 517f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(285.5f, 516.25f), new Vector2(279f, 528f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(302.25f, 511.25f), new Vector2(288.75f, 522.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(302.5f, 505f), new Vector2(316.5f, 509.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(333.25f, 498.25f), new Vector2(318.5f, 506.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(285.25f, 485.75f), new Vector2(286f, 496f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(287.75f, 476.25f), new Vector2(293.25f, 483.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(288.5f, 466f), new Vector2(304.5f, 463.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(313.5f, 448f), new Vector2(318.75f, 457f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(322.25f, 454.75f), new Vector2(326.75f, 465f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(328f, 455.25f), new Vector2(334.25f, 464f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(337.75f, 458f), new Vector2(346f, 466.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(337.75f, 468.5f), new Vector2(351.25f, 468.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(349.5f, 464.75f), new Vector2(357.5f, 467.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(371f, 483.25f), new Vector2(386.25f, 490f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(371.25f, 493f), new Vector2(382.5f, 496.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(386f, 496.25f), new Vector2(387.25f, 511f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(381.25f, 502.5f), new Vector2(374.25f, 502.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(375.25f, 510f), new Vector2(383.5f, 524f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(387.75f, 516.75f), new Vector2(396f, 519.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(393.5f, 513.25f), new Vector2(390.5f, 501.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(386f, 526.5f), new Vector2(395.5f, 523f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(390.75f, 531.75f), new Vector2(403f, 530.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(379f, 539.5f), new Vector2(379f, 550f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(390.75f, 546.5f), new Vector2(377.5f, 556f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(373.5f, 558f), new Vector2(359.5f, 556.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(355f, 562f), new Vector2(344f, 555.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(342.25f, 563f), new Vector2(322.75f, 566.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(297.25f, 559f), new Vector2(300.75f, 571.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(290f, 548f), new Vector2(277f, 546.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(291.25f, 485.75f), new Vector2(303.5f, 486.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(306.5f, 547f), new Vector2(302.5f, 554.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(350.5f, 567.5f), new Vector2(294f, 574f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(665.5f, 424f), new Vector2(696.5f, 410f), colors[6], 5f);
            ds.DrawLine(new Vector2(665f, 425f), new Vector2(674.5f, 456f), colors[6], 5f);
            ds.DrawLine(new Vector2(673f, 457f), new Vector2(708.5f, 439.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(665f, 440f), new Vector2(704.5f, 420.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(590.5f, 451f), new Vector2(614f, 446.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(613f, 447.5f), new Vector2(612.5f, 462.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(612f, 462.5f), new Vector2(612.5f, 478f), colors[6], 5f);
            ds.DrawLine(new Vector2(612.5f, 478.5f), new Vector2(599f, 477f), colors[6], 5f);
            ds.DrawLine(new Vector2(599f, 477.5f), new Vector2(580.5f, 481f), colors[6], 5f);
            ds.DrawLine(new Vector2(581f, 481f), new Vector2(586f, 467.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(587.5f, 467.5f), new Vector2(590f, 453f), colors[6], 5f);
            ds.DrawLine(new Vector2(583.5f, 480f), new Vector2(560f, 538.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(599f, 479f), new Vector2(573.5f, 542.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(596f, 538f), new Vector2(543f, 519f), colors[6], 5f);
            ds.DrawLine(new Vector2(597.5f, 537f), new Vector2(562.5f, 602.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(574.5f, 547.5f), new Vector2(545f, 596f), colors[6], 5f);
            ds.DrawLine(new Vector2(551f, 555.5f), new Vector2(559.5f, 539f), colors[6], 5f);
            ds.FillCircle(new Vector2(573.666f, 547.667f), 9f, colors[1]);
            ds.DrawLine(new Vector2(685f, 544f), new Vector2(684.5f, 583.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(685.5f, 545.5f), new Vector2(720.5f, 542.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(705f, 569f), new Vector2(718.5f, 570.5f), Colors.White, 12f);
            ds.DrawLine(new Vector2(692.5f, 571f), new Vector2(692f, 582f), Colors.White, 12f);
            ds.DrawLine(new Vector2(258f, 583f), new Vector2(271f, 582.5f), Colors.White, 12f);
            ds.DrawLine(new Vector2(275.5f, 593.5f), new Vector2(275.5f, 584.5f), Colors.White, 12f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(614.167f, 476.334f), new Vector2(612.167f, 494.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(612.834f, 494.667f), new Vector2(611.501f, 503.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(614.167f, 476.667f), new Vector2(619.501f, 495.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(620.834f, 486.334f), new Vector2(619.167f, 511.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(613.834f, 475.334f), new Vector2(631.834f, 500f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(615.834f, 477.334f), new Vector2(632.834f, 476f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(621.501f, 481.667f), new Vector2(636.501f, 482.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(623.167f, 484.334f), new Vector2(652.501f, 517.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(631.501f, 496.667f), new Vector2(652.167f, 531.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(629.834f, 491.334f), new Vector2(651.834f, 494.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(626.167f, 485.667f), new Vector2(654.834f, 483.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(621.834f, 490.334f), new Vector2(626.167f, 509.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(625.167f, 492f), new Vector2(628.834f, 527f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(609.167f, 504.334f), new Vector2(598.834f, 523f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(599.501f, 516.334f), new Vector2(595.834f, 528.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(603.167f, 519f), new Vector2(603.167f, 530f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(606.167f, 521.667f), new Vector2(606.501f, 543f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(618.834f, 529.334f), new Vector2(609.834f, 545.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(618.834f, 527.334f), new Vector2(625.167f, 544.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(624.834f, 526.667f), new Vector2(624.834f, 547.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(646.501f, 530.334f), new Vector2(661.501f, 545.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(659.167f, 533.667f), new Vector2(659.834f, 545.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(661.834f, 521.334f), new Vector2(665.834f, 542f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(664.501f, 531.667f), new Vector2(683.167f, 547f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(679.167f, 534.334f), new Vector2(684.501f, 549.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(677.501f, 523.667f), new Vector2(685.501f, 538.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(679.834f, 519.667f), new Vector2(692.167f, 525.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(632.501f, 496.667f), new Vector2(654.501f, 507.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(650.834f, 503.667f), new Vector2(676.834f, 514.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(676.167f, 513f), new Vector2(690.501f, 518f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(641.834f, 490.667f), new Vector2(670.501f, 490.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(662.501f, 482.667f), new Vector2(672.501f, 486f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(667.167f, 481.667f), new Vector2(675.834f, 480.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(677.167f, 469f), new Vector2(670.167f, 473.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(678.167f, 462.334f), new Vector2(658.167f, 468f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(663.501f, 453.667f), new Vector2(660.501f, 466.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(662.167f, 454.667f), new Vector2(649.167f, 463f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(649.501f, 463.667f), new Vector2(634.167f, 473.667f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(644.834f, 468.667f), new Vector2(631.501f, 480.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(659.167f, 458.334f), new Vector2(634.167f, 482.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(660.167f, 460.667f), new Vector2(636.501f, 484.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(661.501f, 466.334f), new Vector2(642.167f, 484.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(666.167f, 467f), new Vector2(649.834f, 482f), colors[6], 2f);
            ds.DrawLine(new Vector2(674.834f, 465f), new Vector2(654.501f, 483.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(660.167f, 475.667f), new Vector2(651.167f, 483f), colors[6], 2f);
            ds.DrawLine(new Vector2(669.167f, 473.334f), new Vector2(651.834f, 491f), colors[6], 2f);
            ds.DrawLine(new Vector2(667.167f, 482.667f), new Vector2(658.834f, 490.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(654.834f, 483f), new Vector2(648.834f, 490f), colors[6], 2f);
            ds.DrawLine(new Vector2(649.834f, 484.667f), new Vector2(643.834f, 490.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(641.834f, 484.667f), new Vector2(634.167f, 492.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(619.501f, 493f), new Vector2(612.834f, 501f), colors[6], 2f);
            ds.DrawLine(new Vector2(620.501f, 499.334f), new Vector2(604.167f, 514f), colors[6], 2f);
            ds.DrawLine(new Vector2(619.501f, 505f), new Vector2(603.167f, 522.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(626.167f, 504.334f), new Vector2(604.501f, 526.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(616.501f, 501f), new Vector2(616.167f, 524.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(624.834f, 511.667f), new Vector2(605.167f, 533.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(627.501f, 514.334f), new Vector2(605.167f, 534.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(627.167f, 520f), new Vector2(606.501f, 539.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(662.501f, 512f), new Vector2(649.834f, 525.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(659.167f, 509.334f), new Vector2(647.167f, 521f), colors[6], 2f);
            ds.DrawLine(new Vector2(654.834f, 507.334f), new Vector2(644.834f, 518f), colors[6], 2f);
            ds.DrawLine(new Vector2(668.167f, 512f), new Vector2(650.501f, 532.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(671.167f, 513.667f), new Vector2(651.167f, 536.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(675.501f, 516f), new Vector2(657.167f, 536f), colors[6], 2f);
            ds.DrawLine(new Vector2(680.167f, 515.667f), new Vector2(665.834f, 533.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(677.834f, 525f), new Vector2(668.167f, 534.334f), colors[6], 2f);
            ds.DrawLine(new Vector2(680.834f, 528.334f), new Vector2(668.834f, 540.334f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(657.167f, 500.667f), new Vector2(666.834f, 506.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(662.167f, 496.334f), new Vector2(672.834f, 501.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(670.167f, 507.667f), new Vector2(688.834f, 509.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(678.834f, 504.001f), new Vector2(679.167f, 493.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(674.834f, 489.001f), new Vector2(685.167f, 479.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(682.834f, 490.334f), new Vector2(690.834f, 483.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(692.5f, 489.334f), new Vector2(683.834f, 499.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(693.167f, 496.001f), new Vector2(688.834f, 506.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(693.167f, 508.334f), new Vector2(696.167f, 521.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(695.167f, 523.334f), new Vector2(695.834f, 535.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(686.834f, 529.001f), new Vector2(691.834f, 540.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(679.167f, 549.667f), new Vector2(671.167f, 557.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(681.167f, 552.001f), new Vector2(677.5f, 559.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(666.167f, 561.334f), new Vector2(654.5f, 554.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(672.5f, 545.667f), new Vector2(658.5f, 552.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(652.834f, 558.334f), new Vector2(649.5f, 547.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(655.167f, 543.667f), new Vector2(644.834f, 541.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(638.834f, 537.667f), new Vector2(638.834f, 549.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(642.834f, 555.334f), new Vector2(630.5f, 551.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(629.834f, 547.001f), new Vector2(633.834f, 522.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(639.834f, 532.334f), new Vector2(641.5f, 522.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(634.5f, 506.667f), new Vector2(637.167f, 521.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(680.5f, 474.334f), new Vector2(690.167f, 477.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(681.5f, 465.001f), new Vector2(689.5f, 469.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(664.5f, 459.001f), new Vector2(671.834f, 461.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(666.834f, 444.667f), new Vector2(652.167f, 455.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(653.834f, 450.667f), new Vector2(630.5f, 461.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(644.834f, 460.667f), new Vector2(629.167f, 473.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(630.5f, 463.001f), new Vector2(618.167f, 465.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(618.167f, 468.667f), new Vector2(629.167f, 474.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(603.5f, 481.001f), new Vector2(601.5f, 493.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(597.834f, 494.667f), new Vector2(608.5f, 502.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(594.834f, 501.334f), new Vector2(596.5f, 515.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(590.167f, 515.001f), new Vector2(593.167f, 528.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(585.5f, 527.334f), new Vector2(594.167f, 534.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(600.5f, 532.001f), new Vector2(604.5f, 544.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(595.167f, 544.667f), new Vector2(618.167f, 549.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(619.834f, 542.001f), new Vector2(623.5f, 554.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(553.5f, 492.001f), new Vector2(561.5f, 502.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(566.5f, 491.334f), new Vector2(568.167f, 503.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(574.5f, 488.667f), new Vector2(571.834f, 499.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(552.167f, 501.334f), new Vector2(561.834f, 508.001f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(567.834f, 504.334f), new Vector2(565.167f, 516.334f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(551.834f, 508.667f), new Vector2(563.5f, 513.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(555.167f, 517.667f), new Vector2(567.5f, 519.334f), colors[6], 4f, strokeStyle);
            }
            ds.DrawLine(new Vector2(612.5f, 448f), new Vector2(665.5f, 425f), colors[6], 5f);
            ds.DrawLine(new Vector2(608f, 459.5f), new Vector2(666f, 438.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(285.5f, 410.335f), new Vector2(329.5f, 328.335f), Colors.White, 40f);
            ds.DrawLine(new Vector2(378.5f, 447.334f), new Vector2(384.5f, 413.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(381.167f, 429.334f), new Vector2(334.5f, 359.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(397.167f, 464.001f), new Vector2(443.833f, 452.001f), Colors.White, 40f);
            ds.DrawLine(new Vector2(395.167f, 430.001f), new Vector2(427.833f, 445.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(431.167f, 548.668f), new Vector2(471.833f, 529.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(583.834f, 466.001f), new Vector2(540.5f, 453.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(602.5f, 446.001f), new Vector2(591.167f, 400.001f), Colors.White, 40f);
            ds.DrawLine(new Vector2(559.167f, 448.668f), new Vector2(645.834f, 382.668f), Colors.White, 40f);
            ds.DrawLine(new Vector2(556.5f, 538.667f), new Vector2(508.5f, 519.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(521.167f, 529.334f), new Vector2(425.833f, 469.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(458.5f, 533.334f), new Vector2(563.167f, 472.001f), Colors.White, 40f);
            ds.DrawLine(new Vector2(687.167f, 410.668f), new Vector2(672.5f, 373.334f), Colors.White, 40f);
            ds.DrawLine(new Vector2(691.834f, 378.668f), new Vector2(623.834f, 302.667f), Colors.White, 40f);
            ds.DrawLine(new Vector2(395.5f, 251.5f), new Vector2(377f, 255f), colors[6], 5f);
            ds.DrawLine(new Vector2(396.5f, 236f), new Vector2(394f, 252.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(833.073f, 133.102f), new Vector2(1004.064f, 202.099f), Colors.White, 80f);
            ds.DrawLine(new Vector2(137.108f, 124.103f), new Vector2(-38.383f, 196.099f), Colors.White, 80f);
            ds.DrawLine(new Vector2(285.601f, 658.076f), new Vector2(324.599f, 743.571f), Colors.White, 80f);
            ds.DrawLine(new Vector2(718.079f, 620.577f), new Vector2(670.081f, 770.57f), Colors.White, 80f);
            ds.DrawLine(new Vector2(701.5f, 571f), new Vector2(701f, 582f), Colors.White, 12f);
            ds.DrawLine(new Vector2(704f, 561f), new Vector2(718.5f, 561f), Colors.White, 12f);
            ds.DrawLine(new Vector2(720.5f, 551.5f), new Vector2(707f, 552f), Colors.White, 12f);
            ds.DrawLine(new Vector2(422.833f, 160.002f), new Vector2(490.834f, 190.668f), colors[6], 5f);
            ds.DrawLine(new Vector2(490.834f, 190.668f), new Vector2(568.167f, 157.668f), colors[6], 5f);
            ds.DrawLine(new Vector2(490.5f, 178.002f), new Vector2(502.167f, 187.335f), colors[6], 5f);
            ds.DrawLine(new Vector2(490.834f, 191.335f), new Vector2(505.834f, 201.668f), colors[6], 5f);
            ds.DrawLine(new Vector2(489.5f, 192.668f), new Vector2(475.833f, 200.335f), colors[6], 5f);
            ds.DrawLine(new Vector2(464.5f, 194.002f), new Vector2(493.834f, 211.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(490.834f, 210.668f), new Vector2(516.167f, 196.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(516.167f, 195.668f), new Vector2(565.834f, 174.668f), colors[6], 5f);
            ds.DrawLine(new Vector2(465.5f, 194.335f), new Vector2(418.833f, 172.335f), colors[6], 5f);
            ds.DrawLine(new Vector2(464.5f, 47.338f), new Vector2(491.167f, 73.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(491.5f, 73.338f), new Vector2(521.167f, 48.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(520.834f, 48.338f), new Vector2(493.167f, 21.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(492.834f, 21.338f), new Vector2(464.833f, 48.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(470.5f, 24.338f), new Vector2(491.834f, 46.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(491.834f, 46.338f), new Vector2(517.834f, 24.338f), colors[6], 5f);
            ds.DrawLine(new Vector2(517.5f, 24.338f), new Vector2(493.834f, 5.005f), colors[6], 5f);
            ds.DrawLine(new Vector2(493.834f, 4.672f), new Vector2(471.167f, 24.338f), colors[6], 5f);
            ds.FillCircle(new Vector2(491.833f, 46.667f), 9f, colors[1]);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(519.167f, 47.672f), new Vector2(542.834f, 77.005f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(542.834f, 76.672f), new Vector2(565.834f, 111.338f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(566.167f, 112.338f), new Vector2(581.834f, 138.338f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(496.167f, 51.005f), new Vector2(520.834f, 77.005f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(520.5f, 77.005f), new Vector2(537.167f, 101.005f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(537.5f, 101.005f), new Vector2(564.834f, 144.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(487.833f, 50.005f), new Vector2(464.167f, 79.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(463.833f, 79.672f), new Vector2(445.5f, 102.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(445.833f, 102.672f), new Vector2(422.833f, 139.005f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(466.5f, 46.338f), new Vector2(446.833f, 72.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(446.833f, 73.005f), new Vector2(428.167f, 98.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(428.167f, 98.672f), new Vector2(406.833f, 134.672f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(481.833f, 153.338f), new Vector2(491.5f, 180.338f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(492.167f, 161.005f), new Vector2(492.5f, 179.672f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(490.5f, 92.005f), new Vector2(489.167f, 129.672f), colors[6], 3f);
            ds.DrawLine(new Vector2(489.5f, 129.672f), new Vector2(491.834f, 161.005f), colors[6], 3f);
            ds.DrawLine(new Vector2(501.5f, 136.672f), new Vector2(494.5f, 162.338f), colors[6], 3f);
            ds.DrawLine(new Vector2(499.834f, 151.005f), new Vector2(495.167f, 170.672f), colors[6], 3f);
            ds.DrawLine(new Vector2(498.5f, 158.338f), new Vector2(512.167f, 140.672f), colors[6], 3f);
            ds.DrawLine(new Vector2(511.5f, 141.005f), new Vector2(523.5f, 130.672f), colors[6], 3f);
            ds.DrawLine(new Vector2(454.167f, 126.338f), new Vector2(469.5f, 134.672f), colors[6], 3f);
            ds.DrawLine(new Vector2(465.167f, 132.338f), new Vector2(481.833f, 156.005f), colors[6], 3f);
            ds.DrawLine(new Vector2(532.834f, 128.005f), new Vector2(522.5f, 131.005f), colors[6], 3f);
            ds.DrawLine(new Vector2(476.167f, 96.338f), new Vector2(488.5f, 116.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(475.833f, 98.338f), new Vector2(474.833f, 86.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(474.833f, 85.672f), new Vector2(478.5f, 78.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(478.5f, 79.005f), new Vector2(485.167f, 78.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(483.167f, 79.338f), new Vector2(491.5f, 84.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(489.833f, 84.672f), new Vector2(494.167f, 80.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(494.5f, 79.672f), new Vector2(500.167f, 79.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(500.5f, 80.005f), new Vector2(505.5f, 86.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(505.167f, 87.338f), new Vector2(503.5f, 96.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(503.5f, 97.005f), new Vector2(501.834f, 104.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(501.5f, 104.338f), new Vector2(495.167f, 113.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(494.834f, 113.338f), new Vector2(490.167f, 117.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(469.5f, 137.005f), new Vector2(467.833f, 122.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(467.5f, 122.005f), new Vector2(462.167f, 110.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(462.167f, 109.672f), new Vector2(454.5f, 107.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(454.5f, 107.005f), new Vector2(449.167f, 111.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(448.5f, 111.672f), new Vector2(451.167f, 120.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(450.833f, 121.005f), new Vector2(443.167f, 121.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(442.833f, 121.672f), new Vector2(439.167f, 126.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(440.5f, 127.005f), new Vector2(440.5f, 132.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(440.833f, 133.005f), new Vector2(446.833f, 138.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(447.167f, 138.338f), new Vector2(456.5f, 139.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(457.5f, 139.672f), new Vector2(468.5f, 139.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(487.5f, 125.005f), new Vector2(483.167f, 121.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(482.833f, 121.338f), new Vector2(479.5f, 128.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(478.833f, 128.338f), new Vector2(477.5f, 134.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(477.5f, 134.672f), new Vector2(491.834f, 168.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(490.5f, 125.338f), new Vector2(496.5f, 124.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(496.5f, 124.338f), new Vector2(502.5f, 127.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(502.5f, 127.338f), new Vector2(504.5f, 135.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(504.834f, 134.338f), new Vector2(500.834f, 144.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(515.834f, 136.672f), new Vector2(519.5f, 118.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(519.834f, 118.005f), new Vector2(522.834f, 108.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(522.834f, 108.005f), new Vector2(531.5f, 107.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(532.5f, 107.672f), new Vector2(537.834f, 109.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(537.834f, 110.005f), new Vector2(540.834f, 118.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(540.5f, 118.672f), new Vector2(535.834f, 125.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(536.167f, 123.672f), new Vector2(544.5f, 125.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(544.834f, 125.672f), new Vector2(547.167f, 131.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(547.167f, 132.338f), new Vector2(543.167f, 140.672f), colors[6], 4f);
            ds.DrawLine(new Vector2(542.834f, 141.005f), new Vector2(535.167f, 141.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(534.834f, 141.005f), new Vector2(518.834f, 133.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(511.834f, 178.672f), new Vector2(470.167f, 178.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(458.167f, 173.005f), new Vector2(527.5f, 174.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(542.5f, 167.338f), new Vector2(494.167f, 168.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(498.167f, 160.338f), new Vector2(555.834f, 162.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(555.5f, 154.005f), new Vector2(501.167f, 154.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.5f, 147.338f), new Vector2(498.5f, 148.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(501.834f, 141.672f), new Vector2(531.834f, 139.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(541.834f, 141.338f), new Vector2(560.5f, 141.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(547.5f, 134.672f), new Vector2(559.167f, 135.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(545.5f, 129.338f), new Vector2(555.834f, 129.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(540.5f, 122.338f), new Vector2(548.834f, 122.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(539.167f, 116.005f), new Vector2(547.167f, 117.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(515.5f, 134.672f), new Vector2(501.5f, 133.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(502.167f, 126.338f), new Vector2(516.5f, 126.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(517.167f, 119.672f), new Vector2(468.833f, 120.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(480.167f, 126.005f), new Vector2(468.5f, 126.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(431.833f, 126.338f), new Vector2(438.5f, 126.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(435.833f, 120.338f), new Vector2(443.167f, 120.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(439.5f, 116.338f), new Vector2(449.167f, 116.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(442.833f, 110.672f), new Vector2(450.833f, 110.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(464.5f, 115.338f), new Vector2(485.833f, 114.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(520.5f, 112.672f), new Vector2(495.167f, 113.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(501.834f, 106.005f), new Vector2(539.167f, 105.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(482.833f, 149.338f), new Vector2(430.5f, 150.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(433.833f, 143.338f), new Vector2(480.833f, 145.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(469.5f, 138.338f), new Vector2(478.167f, 138.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(476.833f, 131.672f), new Vector2(469.167f, 131.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(442.167f, 137.338f), new Vector2(425.5f, 137.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(427.5f, 131.338f), new Vector2(441.5f, 131.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(427.5f, 156.005f), new Vector2(482.5f, 157.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(485.5f, 163.005f), new Vector2(429.5f, 163.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(447.167f, 168.338f), new Vector2(489.5f, 168.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(468.833f, 74.005f), new Vector2(516.167f, 75.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(512.834f, 68.672f), new Vector2(496.834f, 68.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(486.833f, 69.005f), new Vector2(473.167f, 68.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(464.5f, 80.672f), new Vector2(477.167f, 80.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(499.5f, 80.005f), new Vector2(520.5f, 80.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(524.167f, 85.672f), new Vector2(503.5f, 86.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(529.5f, 93.338f), new Vector2(503.834f, 93.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(535.5f, 98.005f), new Vector2(503.5f, 99.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(483.5f, 109.338f), new Vector2(463.167f, 108.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(478.167f, 103.005f), new Vector2(447.833f, 103.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(453.833f, 95.672f), new Vector2(475.167f, 95.672f), colors[6], 2f);
            ds.DrawLine(new Vector2(472.833f, 88.005f), new Vector2(458.833f, 88.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(482.167f, 186.333f), new Vector2(468.833f, 195f), colors[6], 5f);
            ds.DrawLine(new Vector2(490.167f, 182f), new Vector2(487.167f, 188.667f), colors[6], 5f);
            ds.DrawLine(new Vector2(497.834f, 186.333f), new Vector2(513.167f, 196.333f), colors[6], 5f);
            ds.DrawLine(new Vector2(474.5f, 202.002f), new Vector2(436.833f, 229.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(507.834f, 201.335f), new Vector2(546.167f, 232.002f), colors[6], 5f);
            ds.DrawLine(new Vector2(546.5f, 232.002f), new Vector2(586.834f, 237.335f), colors[6], 5f);
            ds.DrawLine(new Vector2(396.833f, 235.669f), new Vector2(437.167f, 228.669f), colors[6], 5f);
            ds.DrawLine(new Vector2(439.167f, 228.669f), new Vector2(491.5f, 227.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(491.167f, 227.669f), new Vector2(544.834f, 231.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(399.167f, 222.335f), new Vector2(412.5f, 219.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(412.833f, 219.669f), new Vector2(421.167f, 219.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(421.5f, 219.335f), new Vector2(431.167f, 221.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(430.833f, 221.335f), new Vector2(435.167f, 223.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(435.167f, 224.002f), new Vector2(437.833f, 215.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(437.833f, 215.002f), new Vector2(446.833f, 208.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(446.833f, 208.002f), new Vector2(456.167f, 202.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(455.833f, 202.335f), new Vector2(467.833f, 195.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(517.834f, 196.002f), new Vector2(530.5f, 205.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(530.834f, 205.002f), new Vector2(543.5f, 213.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(543.834f, 213.002f), new Vector2(548.5f, 220.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(548.834f, 220.002f), new Vector2(551.167f, 226.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(551.167f, 227.002f), new Vector2(561.834f, 222.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(562.167f, 222.002f), new Vector2(575.834f, 223.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(575.834f, 223.335f), new Vector2(586.5f, 227.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(489.5f, 210.335f), new Vector2(482.833f, 219.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(482.167f, 219.002f), new Vector2(477.5f, 227.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(492.5f, 212.002f), new Vector2(502.167f, 222.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(502.167f, 222.002f), new Vector2(507.167f, 232.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(507.167f, 232.669f), new Vector2(508.834f, 236.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(509.167f, 236.669f), new Vector2(519.5f, 241.002f), colors[6], 3f);
            ds.DrawLine(new Vector2(519.834f, 241.002f), new Vector2(530.834f, 249.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(475.167f, 229.335f), new Vector2(474.833f, 237.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(474.167f, 238.335f), new Vector2(464.167f, 243.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(463.5f, 243.669f), new Vector2(451.167f, 250.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(451.167f, 251.002f), new Vector2(444.5f, 259.669f), colors[6], 3f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(559.167f, 182.003f), new Vector2(547.5f, 190.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(565.167f, 182.336f), new Vector2(558.834f, 202.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(557.5f, 189.669f), new Vector2(543.834f, 199.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(524.834f, 196.003f), new Vector2(540.834f, 204.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(531.834f, 193.669f), new Vector2(543.167f, 195.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(552.834f, 199.669f), new Vector2(553.5f, 218.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(545.5f, 203.669f), new Vector2(546.834f, 211.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(560.167f, 207.669f), new Vector2(561.834f, 216.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(565.5f, 198.336f), new Vector2(565.834f, 211.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(569.5f, 194.003f), new Vector2(574.5f, 207.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(573.834f, 211.003f), new Vector2(568.5f, 220.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(422.833f, 180.003f), new Vector2(416.5f, 188.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(429.5f, 182.336f), new Vector2(420.833f, 193.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(434.833f, 185.003f), new Vector2(422.833f, 202.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(438.833f, 190.003f), new Vector2(432.5f, 199.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(431.833f, 200.669f), new Vector2(423.833f, 211.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(446.833f, 191.669f), new Vector2(436.167f, 207.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(453.5f, 194.336f), new Vector2(446.833f, 204.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(434.5f, 208.669f), new Vector2(428.5f, 219.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(417.833f, 194.003f), new Vector2(412.833f, 206.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(410.5f, 205.669f), new Vector2(406.167f, 218.669f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(419.167f, 207.669f), new Vector2(414.5f, 219.003f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(423.167f, 213.669f), new Vector2(420.5f, 219.336f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(450.833f, 225.002f), new Vector2(476.833f, 206.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(480.5f, 208.002f), new Vector2(455.833f, 226.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(464.167f, 225.002f), new Vector2(482.167f, 211.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(481.5f, 216.335f), new Vector2(469.5f, 225.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(505.5f, 207.002f), new Vector2(498.167f, 215.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(508.5f, 207.668f), new Vector2(502.167f, 219.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(512.834f, 212.002f), new Vector2(533.834f, 229.335f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(508.5f, 212.668f), new Vector2(522.5f, 225.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(509.5f, 219.002f), new Vector2(520.167f, 228.002f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(506.5f, 220.335f), new Vector2(513.5f, 228.002f), colors[6], 2f, strokeStyle);
            }
            ds.DrawLine(new Vector2(154.5f, 328f), new Vector2(179f, 351.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(154f, 328f), new Vector2(133.5f, 350.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(133f, 350.5f), new Vector2(155.5f, 378.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(155f, 378.5f), new Vector2(179f, 353f), colors[6], 5f);
            ds.DrawLine(new Vector2(155.5f, 352.5f), new Vector2(183.5f, 326f), colors[6], 5f);
            ds.DrawLine(new Vector2(183.5f, 325.5f), new Vector2(208.5f, 349.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(208.5f, 350f), new Vector2(181.5f, 380.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(155f, 352.5f), new Vector2(182f, 380f), colors[6], 5f);
            ds.FillCircle(new Vector2(183.333f, 351.167f), 9f, colors[1]);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(272f, 263.5f), new Vector2(233.5f, 289f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(234f, 289f), new Vector2(209.5f, 305.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(209f, 305.5f), new Vector2(182.5f, 326f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(278.5f, 280f), new Vector2(251f, 297f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(251.5f, 296.5f), new Vector2(220f, 317f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(220f, 317.5f), new Vector2(187.5f, 344.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(185.5f, 358f), new Vector2(216.5f, 385f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(216f, 385f), new Vector2(247.5f, 406f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(181.5f, 379f), new Vector2(211f, 404.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(210.5f, 404.5f), new Vector2(239.5f, 423.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(239f, 423.5f), new Vector2(275.5f, 448f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(246f, 406.5f), new Vector2(287.5f, 432f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(494f, 658.5f), new Vector2(469f, 682f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(470.5f, 682.5f), new Vector2(495.5f, 700f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(495.5f, 700.5f), new Vector2(522f, 678f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(521.5f, 678f), new Vector2(490.5f, 657.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(495.5f, 683.5f), new Vector2(467.5f, 656.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(467f, 656.5f), new Vector2(491.5f, 630.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(491f, 630.5f), new Vector2(523f, 657f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(522.5f, 656.5f), new Vector2(495.5f, 684f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(420.5f, 562.5f), new Vector2(441f, 595f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(441f, 594.5f), new Vector2(459.5f, 621.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(460f, 622f), new Vector2(491.5f, 657.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(405.5f, 569.5f), new Vector2(419f, 595.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(419.5f, 595.5f), new Vector2(438.5f, 623f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(439f, 623.5f), new Vector2(460.5f, 651f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(460f, 651f), new Vector2(476.5f, 664.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(546.5f, 594f), new Vector2(528f, 619f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(527.5f, 620.5f), new Vector2(507f, 643.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(506.5f, 642f), new Vector2(495.5f, 658f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(563f, 601.5f), new Vector2(545f, 629.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(544.5f, 629.5f), new Vector2(521f, 654.5f), colors[6], 5f, strokeStyle);
            }
            ds.FillCircle(new Vector2(493.833f, 657.167f), 9f, colors[1]);
            ds.FillCircle(new Vector2(785.833f, 352.167f), 9f, colors[1]);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(699.5f, 266f), new Vector2(730.5f, 284.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(730f, 284.5f), new Vector2(757.5f, 303.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(757f, 303.5f), new Vector2(784f, 328f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(783.5f, 328f), new Vector2(808.5f, 354f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(808.5f, 353.5f), new Vector2(789f, 375f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(789f, 375.5f), new Vector2(758.5f, 403.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(758f, 403.5f), new Vector2(710.5f, 436.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(702f, 420f), new Vector2(728.5f, 404f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(728.5f, 403.5f), new Vector2(755.5f, 382f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(756f, 382f), new Vector2(781f, 357f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(760.5f, 348.5f), new Vector2(787f, 377f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(761.5f, 347.5f), new Vector2(784.5f, 326.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(697.5f, 285f), new Vector2(723f, 299f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(722.5f, 299f), new Vector2(749f, 317f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(749f, 317.5f), new Vector2(780.5f, 347f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(791f, 348f), new Vector2(807.5f, 329f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(807f, 329f), new Vector2(829.5f, 352f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(829.5f, 352f), new Vector2(807.5f, 376.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(808f, 376.5f), new Vector2(788.5f, 354f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(303.5f, 281f), new Vector2(330.5f, 336.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(326.5f, 341f), new Vector2(315.5f, 349f), colors[6], 5f);
            ds.DrawLine(new Vector2(316f, 349f), new Vector2(327f, 359f), colors[6], 5f);
            ds.DrawLine(new Vector2(327f, 342.5f), new Vector2(344f, 368f), colors[6], 5f);
            ds.DrawLine(new Vector2(333f, 351f), new Vector2(325.5f, 362.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(325.5f, 362f), new Vector2(298f, 420.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(327f, 364f), new Vector2(337f, 379f), colors[6], 5f);
            ds.DrawLine(new Vector2(336.5f, 378f), new Vector2(354.5f, 345f), colors[6], 5f);
            ds.DrawLine(new Vector2(319.5f, 273.5f), new Vector2(339.5f, 318.5f), colors[6], 5f);
            ds.DrawLine(new Vector2(339f, 318.5f), new Vector2(353f, 345f), colors[6], 5f);
            ds.DrawLine(new Vector2(328f, 337.5f), new Vector2(340.5f, 318.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(340f, 317f), new Vector2(351f, 300.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(351f, 300f), new Vector2(360f, 293.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(360f, 293f), new Vector2(364.5f, 291.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(365f, 291.5f), new Vector2(361.5f, 279.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(361.5f, 279f), new Vector2(368.5f, 256.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(377f, 254.5f), new Vector2(373f, 279.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(373f, 279f), new Vector2(370f, 304.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(370f, 305f), new Vector2(368.5f, 352f), colors[6], 3f);
            ds.DrawLine(new Vector2(368.5f, 351.5f), new Vector2(368.5f, 392f), colors[6], 3f);
            ds.DrawLine(new Vector2(368.5f, 392.5f), new Vector2(373.5f, 433f), colors[6], 3f);
            ds.DrawLine(new Vector2(373f, 433f), new Vector2(378f, 453.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(337.5f, 378f), new Vector2(342f, 390f), colors[6], 3f);
            ds.DrawLine(new Vector2(342f, 389.5f), new Vector2(347f, 399.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(347.5f, 400f), new Vector2(357f, 406.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(357f, 407f), new Vector2(356.5f, 423.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(356.5f, 424f), new Vector2(360.5f, 441f), colors[6], 3f);
            ds.DrawLine(new Vector2(360f, 441f), new Vector2(365f, 450.5f), colors[6], 3f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(341f, 368.5f), new Vector2(358f, 389f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(358f, 389.5f), new Vector2(374f, 411f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(374f, 411.5f), new Vector2(396f, 437.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(394.5f, 437.5f), new Vector2(416f, 456f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(415.5f, 456f), new Vector2(437.5f, 474f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(437f, 474f), new Vector2(448f, 481f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(388.5f, 469f), new Vector2(417.5f, 474.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(417.5f, 474f), new Vector2(462f, 481f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(462.5f, 481f), new Vector2(502f, 481.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(502.5f, 481.5f), new Vector2(542f, 479f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(542.5f, 479f), new Vector2(584f, 471f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(584.5f, 471f), new Vector2(598.5f, 467.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(414.5f, 548.5f), new Vector2(451f, 534.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(450.5f, 534.5f), new Vector2(485.5f, 516.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(486f, 516.5f), new Vector2(531f, 488f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(530.5f, 487.5f), new Vector2(577.5f, 446.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(577.5f, 446f), new Vector2(615f, 402f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(603f, 465.5f), new Vector2(610.5f, 431f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(610.5f, 430.5f), new Vector2(616.5f, 375f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(616f, 375f), new Vector2(615.5f, 326f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(615.5f, 325.5f), new Vector2(612f, 283f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(612f, 283.5f), new Vector2(603f, 242f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(312f, 425.5f), new Vector2(322.5f, 402f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(322f, 402f), new Vector2(335f, 375f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(662f, 280.5f), new Vector2(650.5f, 312f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(650f, 312f), new Vector2(638.5f, 334.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(638.5f, 335f), new Vector2(628.5f, 349.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(628f, 349.5f), new Vector2(639f, 367f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(639f, 367.5f), new Vector2(650.5f, 390f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(651f, 390f), new Vector2(660f, 409f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(660f, 410f), new Vector2(664f, 425.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(677f, 287.5f), new Vector2(662.5f, 326.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(662f, 326.5f), new Vector2(646.5f, 352f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(646.5f, 351.5f), new Vector2(655f, 370f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(655f, 371f), new Vector2(666.5f, 391f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(666.5f, 391.5f), new Vector2(678.5f, 421f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(552f, 235f), new Vector2(573.5f, 256.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(573f, 256.5f), new Vector2(596f, 281.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(596.5f, 281.5f), new Vector2(614.5f, 301.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(615f, 301.5f), new Vector2(631.5f, 327f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(632f, 327f), new Vector2(647f, 353.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(332f, 351f), new Vector2(351.5f, 318f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(352f, 318f), new Vector2(367.5f, 299f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(367f, 299f), new Vector2(385f, 279.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(385f, 279f), new Vector2(406f, 256f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(405.5f, 256f), new Vector2(423.5f, 241f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(423.5f, 240.5f), new Vector2(435.5f, 230.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(448.5f, 481.5f), new Vector2(471f, 496.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(470.5f, 496.5f), new Vector2(505.5f, 514.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(505.5f, 515f), new Vector2(531.5f, 529.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(532f, 529.5f), new Vector2(555f, 540f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(555f, 540.5f), new Vector2(571.5f, 545.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(417.5f, 529.5f), new Vector2(442.5f, 520f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(442f, 520f), new Vector2(468f, 508f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(468f, 507.5f), new Vector2(494.5f, 491f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(494.5f, 491.5f), new Vector2(518.5f, 504.5f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(518f, 504.5f), new Vector2(542f, 518f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(542.5f, 518f), new Vector2(576f, 530f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(399f, 479.5f), new Vector2(412.5f, 487.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(413f, 487.5f), new Vector2(428f, 490f), colors[6], 3f);
            ds.DrawLine(new Vector2(428.5f, 490f), new Vector2(436.5f, 487.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(435.5f, 487.5f), new Vector2(443f, 486f), colors[6], 3f);
            ds.DrawLine(new Vector2(440f, 486f), new Vector2(445.5f, 493f), colors[6], 3f);
            ds.DrawLine(new Vector2(445.5f, 493.5f), new Vector2(454.5f, 501.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(455f, 501.5f), new Vector2(464.5f, 504f), colors[6], 3f);
            ds.DrawLine(new Vector2(465f, 504f), new Vector2(477f, 513f), colors[6], 3f);
            ds.DrawLine(new Vector2(477.5f, 513f), new Vector2(488f, 524.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(488.5f, 524.5f), new Vector2(492f, 532f), colors[6], 3f);
            ds.DrawLine(new Vector2(492f, 532.5f), new Vector2(498f, 521.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(497.5f, 521.5f), new Vector2(507f, 513.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(507f, 513f), new Vector2(514.5f, 508f), colors[6], 3f);
            ds.DrawLine(new Vector2(515f, 508f), new Vector2(532f, 502f), colors[6], 3f);
            ds.DrawLine(new Vector2(531.5f, 502f), new Vector2(541.5f, 492f), colors[6], 3f);
            ds.DrawLine(new Vector2(541.5f, 491.5f), new Vector2(547.5f, 483.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(547f, 483.5f), new Vector2(561.5f, 485.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(561f, 485.5f), new Vector2(579f, 483f), colors[6], 3f);
            ds.DrawLine(new Vector2(579f, 482.5f), new Vector2(586.5f, 477.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(613f, 449f), new Vector2(619.5f, 433.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(619f, 433.5f), new Vector2(619.5f, 420.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(619f, 420.5f), new Vector2(618.5f, 407f), colors[6], 3f);
            ds.DrawLine(new Vector2(615.5f, 407f), new Vector2(627f, 400f), colors[6], 3f);
            ds.DrawLine(new Vector2(627f, 399.5f), new Vector2(637f, 387f), colors[6], 3f);
            ds.DrawLine(new Vector2(636.5f, 387.5f), new Vector2(640f, 374f), colors[6], 3f);
            ds.DrawLine(new Vector2(640f, 373.5f), new Vector2(651.5f, 360f), colors[6], 3f);
            ds.DrawLine(new Vector2(651.5f, 360f), new Vector2(659f, 350f), colors[6], 3f);
            ds.DrawLine(new Vector2(661f, 347f), new Vector2(652.5f, 339.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(652f, 339.5f), new Vector2(643.5f, 327.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(643f, 327.5f), new Vector2(636f, 306f), colors[6], 3f);
            ds.DrawLine(new Vector2(635.5f, 306f), new Vector2(630.5f, 298.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(630f, 298.5f), new Vector2(621f, 296f), colors[6], 3f);
            ds.DrawLine(new Vector2(620.5f, 296f), new Vector2(624.5f, 279.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(624f, 279.5f), new Vector2(623f, 266f), colors[6], 3f);
            ds.DrawLine(new Vector2(623f, 265.5f), new Vector2(615.5f, 248.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(615.5f, 248f), new Vector2(610f, 233.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(610.5f, 233.5f), new Vector2(611.5f, 225.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(610.5f, 227.5f), new Vector2(595f, 229f), colors[6], 3f);
            ds.DrawLine(new Vector2(595.5f, 229.5f), new Vector2(580.5f, 226.5f), colors[6], 3f);
            ds.DrawLine(new Vector2(284.5f, 289.334f), new Vector2(301.833f, 281.334f), colors[6], 5f);
            ds.DrawLine(new Vector2(617.834f, 231.667f), new Vector2(617.834f, 259.667f), colors[6], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(294.334f, 359.838f), new Vector2(321.333f, 350.172f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(302.001f, 349.505f), new Vector2(320.667f, 349.172f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(233.001f, 351.172f), new Vector2(270.667f, 352.505f), colors[6], 3f);
            ds.DrawLine(new Vector2(270.667f, 352.172f), new Vector2(302.001f, 349.838f), colors[6], 3f);
            ds.DrawLine(new Vector2(277.667f, 340.172f), new Vector2(303.333f, 347.172f), colors[6], 3f);
            ds.DrawLine(new Vector2(292.001f, 341.838f), new Vector2(311.667f, 346.505f), colors[6], 3f);
            ds.DrawLine(new Vector2(299.333f, 343.172f), new Vector2(281.667f, 329.505f), colors[6], 3f);
            ds.DrawLine(new Vector2(282.001f, 330.172f), new Vector2(271.667f, 318.172f), colors[6], 3f);
            ds.DrawLine(new Vector2(267.334f, 387.505f), new Vector2(275.667f, 372.172f), colors[6], 3f);
            ds.DrawLine(new Vector2(273.334f, 376.505f), new Vector2(297.001f, 359.838f), colors[6], 3f);
            ds.DrawLine(new Vector2(269.001f, 308.838f), new Vector2(272.001f, 319.172f), colors[6], 3f);
            ds.DrawLine(new Vector2(237.334f, 365.505f), new Vector2(257.334f, 353.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(239.334f, 365.838f), new Vector2(227.001f, 366.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(220.334f, 358.505f), new Vector2(225.334f, 350.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(220.667f, 347.172f), new Vector2(220.667f, 341.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(221.001f, 341.172f), new Vector2(227.667f, 336.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(228.334f, 336.505f), new Vector2(237.667f, 338.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(238.001f, 338.172f), new Vector2(245.001f, 339.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(245.334f, 340.172f), new Vector2(254.334f, 346.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(254.334f, 346.838f), new Vector2(258.334f, 351.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(278.001f, 372.172f), new Vector2(263.001f, 373.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(263.001f, 374.172f), new Vector2(251.001f, 379.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(250.667f, 379.505f), new Vector2(248.334f, 387.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(248.001f, 387.172f), new Vector2(252.667f, 392.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(252.667f, 393.172f), new Vector2(261.667f, 390.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(262.001f, 390.838f), new Vector2(262.667f, 398.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(268.001f, 401.172f), new Vector2(273.667f, 401.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(279.334f, 394.505f), new Vector2(280.001f, 385.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(280.667f, 384.172f), new Vector2(280.667f, 373.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(266.001f, 354.172f), new Vector2(262.334f, 358.505f), colors[6], 4f);
            ds.DrawLine(new Vector2(262.334f, 358.838f), new Vector2(269.001f, 362.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(269.334f, 362.838f), new Vector2(275.334f, 364.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(275.667f, 364.172f), new Vector2(309.667f, 349.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(266.334f, 351.172f), new Vector2(265.667f, 345.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(265.334f, 345.172f), new Vector2(268.001f, 339.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(268.334f, 339.172f), new Vector2(276.334f, 337.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(275.334f, 336.838f), new Vector2(285.667f, 340.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(277.667f, 325.838f), new Vector2(259.001f, 322.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(259.001f, 321.838f), new Vector2(249.334f, 318.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(249.001f, 318.838f), new Vector2(248.667f, 310.172f), colors[6], 4f);
            ds.DrawLine(new Vector2(251.001f, 303.838f), new Vector2(259.001f, 300.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(259.667f, 301.172f), new Vector2(266.667f, 305.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(282.001f, 306.838f), new Vector2(274.001f, 322.838f), colors[6], 4f);
            ds.DrawLine(new Vector2(301.333f, 343.505f), new Vector2(303.333f, 285.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(295.001f, 286.172f), new Vector2(295.001f, 340.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(288.334f, 289.172f), new Vector2(289.334f, 343.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(282.667f, 339.838f), new Vector2(280.667f, 309.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(275.667f, 294.172f), new Vector2(276.667f, 282.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(270.334f, 296.172f), new Vector2(270.667f, 285.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(263.334f, 301.172f), new Vector2(263.334f, 292.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(275.667f, 326.172f), new Vector2(274.334f, 340.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(267.334f, 339.505f), new Vector2(267.667f, 325.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(260.667f, 324.505f), new Vector2(261.667f, 372.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(267.001f, 361.505f), new Vector2(267.334f, 373.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(261.334f, 405.838f), new Vector2(261.334f, 398.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(253.667f, 321.172f), new Vector2(254.001f, 346.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(247.001f, 339.838f), new Vector2(246.667f, 302.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(290.334f, 358.838f), new Vector2(291.667f, 411.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(284.334f, 407.838f), new Vector2(286.001f, 360.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(279.334f, 372.172f), new Vector2(279.334f, 363.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(272.667f, 364.838f), new Vector2(272.667f, 372.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(278.334f, 399.505f), new Vector2(278.667f, 416.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(297.001f, 414.172f), new Vector2(298.667f, 359.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(304.001f, 356.172f), new Vector2(304.667f, 412.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(209.667f, 328.838f), new Vector2(209.667f, 344.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(221.001f, 342.172f), new Vector2(221.001f, 321.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(226.667f, 317.505f), new Vector2(227.667f, 338.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(234.334f, 312.172f), new Vector2(234.334f, 337.838f), colors[6], 2f);
            ds.DrawLine(new Vector2(239.001f, 306.172f), new Vector2(240.001f, 338.172f), colors[6], 2f);
            ds.DrawLine(new Vector2(250.334f, 358.172f), new Vector2(249.334f, 378.505f), colors[6], 2f);
            ds.DrawLine(new Vector2(244.001f, 363.505f), new Vector2(244.001f, 393.838f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(503.167f, 548.004f), new Vector2(493.5f, 521.005f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(492.834f, 540.337f), new Vector2(492.5f, 521.671f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(494.5f, 609.337f), new Vector2(495.834f, 571.671f), colors[6], 3f);
            ds.DrawLine(new Vector2(495.5f, 571.671f), new Vector2(493.166f, 540.337f), colors[6], 3f);
            ds.DrawLine(new Vector2(483.5f, 564.671f), new Vector2(490.5f, 539.005f), colors[6], 3f);
            ds.DrawLine(new Vector2(485.167f, 550.338f), new Vector2(489.834f, 530.671f), colors[6], 3f);
            ds.DrawLine(new Vector2(486.5f, 543.005f), new Vector2(472.833f, 560.671f), colors[6], 3f);
            ds.DrawLine(new Vector2(473.5f, 560.338f), new Vector2(461.5f, 570.671f), colors[6], 3f);
            ds.DrawLine(new Vector2(530.834f, 575.004f), new Vector2(515.5f, 566.671f), colors[6], 3f);
            ds.DrawLine(new Vector2(519.834f, 569.004f), new Vector2(503.167f, 545.337f), colors[6], 3f);
            ds.DrawLine(new Vector2(452.167f, 573.338f), new Vector2(462.5f, 570.338f), colors[6], 3f);
            ds.DrawLine(new Vector2(508.834f, 605.004f), new Vector2(496.5f, 585.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(509.167f, 603.004f), new Vector2(510.167f, 615.337f), colors[6], 4f);
            ds.DrawLine(new Vector2(501.834f, 622.005f), new Vector2(493.5f, 617.005f), colors[6], 4f);
            ds.DrawLine(new Vector2(490.5f, 621.671f), new Vector2(484.833f, 621.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(484.5f, 621.337f), new Vector2(479.5f, 614.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(479.833f, 614.005f), new Vector2(481.5f, 604.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(481.5f, 604.338f), new Vector2(483.167f, 597.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(483.5f, 597.004f), new Vector2(489.834f, 588.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(490.166f, 588.004f), new Vector2(494.834f, 584.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(515.5f, 564.338f), new Vector2(517.167f, 579.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(517.5f, 579.338f), new Vector2(522.834f, 591.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(522.834f, 591.671f), new Vector2(530.5f, 594.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(530.5f, 594.338f), new Vector2(535.834f, 589.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(536.5f, 589.671f), new Vector2(533.834f, 580.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(534.167f, 580.338f), new Vector2(541.834f, 579.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(544.5f, 574.338f), new Vector2(544.5f, 568.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(537.834f, 563.004f), new Vector2(528.5f, 562.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(527.5f, 561.671f), new Vector2(516.5f, 561.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(497.5f, 576.338f), new Vector2(501.834f, 580.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(502.167f, 580.004f), new Vector2(505.5f, 573.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(506.167f, 573.004f), new Vector2(507.5f, 567.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(507.5f, 566.671f), new Vector2(493.166f, 532.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(494.5f, 576.004f), new Vector2(488.5f, 576.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(488.5f, 577.004f), new Vector2(482.5f, 574.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(482.5f, 574.004f), new Vector2(480.5f, 566.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(480.167f, 567.004f), new Vector2(484.167f, 556.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(469.167f, 564.671f), new Vector2(465.5f, 583.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(465.167f, 583.338f), new Vector2(462.167f, 593.004f), colors[6], 4f);
            ds.DrawLine(new Vector2(462.167f, 593.338f), new Vector2(453.5f, 593.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(447.167f, 591.338f), new Vector2(444.167f, 583.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(444.5f, 582.671f), new Vector2(449.167f, 575.671f), colors[6], 4f);
            ds.DrawLine(new Vector2(450.167f, 560.338f), new Vector2(466.167f, 568.338f), colors[6], 4f);
            ds.DrawLine(new Vector2(486.833f, 541.005f), new Vector2(429.167f, 539.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(429.5f, 547.338f), new Vector2(483.833f, 547.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(432.5f, 554.004f), new Vector2(486.5f, 553.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(483.167f, 559.671f), new Vector2(453.167f, 561.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(437.5f, 566.671f), new Vector2(425.833f, 565.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(439.5f, 572.004f), new Vector2(429.167f, 571.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(444.5f, 579.004f), new Vector2(436.167f, 579.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(469.5f, 566.671f), new Vector2(483.5f, 568.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(482.833f, 575.004f), new Vector2(468.5f, 574.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(467.833f, 581.671f), new Vector2(516.167f, 580.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(504.834f, 575.338f), new Vector2(516.5f, 575.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(549.167f, 581.004f), new Vector2(541.834f, 581.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(464.5f, 588.671f), new Vector2(489.834f, 588.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(483.167f, 595.338f), new Vector2(445.833f, 595.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(502.167f, 552.004f), new Vector2(554.5f, 550.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(551.167f, 558.004f), new Vector2(504.167f, 556.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(515.5f, 563.004f), new Vector2(506.834f, 563.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(508.167f, 569.671f), new Vector2(515.834f, 569.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(542.834f, 564.004f), new Vector2(559.5f, 563.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(557.5f, 545.337f), new Vector2(502.5f, 543.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(499.5f, 538.337f), new Vector2(555.5f, 537.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(516.167f, 627.337f), new Vector2(468.833f, 626.337f), colors[6], 2f);
            ds.DrawLine(new Vector2(472.167f, 632.671f), new Vector2(488.167f, 632.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(498.167f, 632.337f), new Vector2(511.834f, 633.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(520.5f, 620.671f), new Vector2(507.834f, 621.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(485.5f, 621.337f), new Vector2(464.5f, 621.337f), colors[6], 2f);
            ds.DrawLine(new Vector2(460.833f, 615.671f), new Vector2(481.5f, 614.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(455.5f, 608.005f), new Vector2(481.167f, 608.005f), colors[6], 2f);
            ds.DrawLine(new Vector2(449.5f, 603.338f), new Vector2(481.5f, 602.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(501.5f, 592.004f), new Vector2(521.834f, 593.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(506.834f, 598.338f), new Vector2(537.167f, 598.338f), colors[6], 2f);
            ds.DrawLine(new Vector2(531.167f, 605.671f), new Vector2(509.834f, 605.671f), colors[6], 2f);
            ds.DrawLine(new Vector2(512.167f, 613.337f), new Vector2(526.167f, 613.337f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(680.501f, 339.669f), new Vector2(653.501f, 349.335f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(672.833f, 350.003f), new Vector2(654.167f, 350.335f), colors[6], 5f, strokeStyle);
            }
            ds.DrawLine(new Vector2(741.834f, 348.335f), new Vector2(704.167f, 347.003f), colors[6], 3f);
            ds.DrawLine(new Vector2(704.167f, 347.335f), new Vector2(672.833f, 349.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(697.167f, 359.335f), new Vector2(671.501f, 352.335f), colors[6], 3f);
            ds.DrawLine(new Vector2(682.833f, 357.669f), new Vector2(663.167f, 353.003f), colors[6], 3f);
            ds.DrawLine(new Vector2(675.501f, 356.335f), new Vector2(693.167f, 370.004f), colors[6], 3f);
            ds.DrawLine(new Vector2(692.833f, 369.336f), new Vector2(703.167f, 381.336f), colors[6], 3f);
            ds.DrawLine(new Vector2(707.501f, 312.003f), new Vector2(699.167f, 327.336f), colors[6], 3f);
            ds.DrawLine(new Vector2(701.501f, 323.003f), new Vector2(677.833f, 339.669f), colors[6], 3f);
            ds.DrawLine(new Vector2(705.833f, 390.67f), new Vector2(702.833f, 380.336f), colors[6], 3f);
            ds.DrawLine(new Vector2(737.5f, 334.003f), new Vector2(717.501f, 346.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(735.5f, 333.669f), new Vector2(747.834f, 332.669f), colors[6], 4f);
            ds.DrawLine(new Vector2(754.5f, 341.003f), new Vector2(749.5f, 349.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(754.167f, 352.335f), new Vector2(754.167f, 358.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(753.834f, 358.335f), new Vector2(747.167f, 363.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(746.5f, 363.003f), new Vector2(737.167f, 361.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(736.834f, 361.335f), new Vector2(729.833f, 359.669f), colors[6], 4f);
            ds.DrawLine(new Vector2(729.501f, 359.335f), new Vector2(720.501f, 353.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(720.501f, 352.669f), new Vector2(716.501f, 348.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(696.833f, 327.336f), new Vector2(711.833f, 325.669f), colors[6], 4f);
            ds.DrawLine(new Vector2(711.833f, 325.336f), new Vector2(723.833f, 320.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(724.167f, 320.003f), new Vector2(726.501f, 312.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(726.833f, 312.336f), new Vector2(722.167f, 307.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(722.167f, 306.336f), new Vector2(713.167f, 309.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(712.833f, 308.669f), new Vector2(712.167f, 301.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(706.833f, 298.336f), new Vector2(701.167f, 298.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(695.501f, 305.003f), new Vector2(694.833f, 314.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(694.167f, 315.336f), new Vector2(694.167f, 326.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(708.833f, 345.335f), new Vector2(712.501f, 341.003f), colors[6], 4f);
            ds.DrawLine(new Vector2(712.501f, 340.669f), new Vector2(705.833f, 337.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(705.501f, 336.669f), new Vector2(699.501f, 335.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(699.167f, 335.335f), new Vector2(665.167f, 349.669f), colors[6], 4f);
            ds.DrawLine(new Vector2(708.501f, 348.335f), new Vector2(709.167f, 354.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(709.501f, 354.335f), new Vector2(706.833f, 360.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(706.501f, 360.335f), new Vector2(698.501f, 362.335f), colors[6], 4f);
            ds.DrawLine(new Vector2(699.501f, 362.669f), new Vector2(689.167f, 358.669f), colors[6], 4f);
            ds.DrawLine(new Vector2(697.167f, 373.67f), new Vector2(715.833f, 377.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(715.833f, 377.67f), new Vector2(725.501f, 380.67f), colors[6], 4f);
            ds.DrawLine(new Vector2(725.833f, 380.67f), new Vector2(726.167f, 389.336f), colors[6], 4f);
            ds.DrawLine(new Vector2(723.833f, 395.67f), new Vector2(715.833f, 398.67f), colors[6], 4f);
            ds.DrawLine(new Vector2(715.167f, 398.336f), new Vector2(708.167f, 393.67f), colors[6], 4f);
            ds.DrawLine(new Vector2(692.833f, 392.67f), new Vector2(700.833f, 376.67f), colors[6], 4f);
            ds.DrawLine(new Vector2(673.501f, 356.003f), new Vector2(671.501f, 413.67f), colors[6], 2f);
            ds.DrawLine(new Vector2(679.833f, 413.336f), new Vector2(679.833f, 359.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(686.501f, 410.336f), new Vector2(685.501f, 356.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(692.167f, 359.669f), new Vector2(694.167f, 389.67f), colors[6], 2f);
            ds.DrawLine(new Vector2(699.167f, 405.336f), new Vector2(698.167f, 417.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(704.501f, 403.336f), new Vector2(704.167f, 413.67f), colors[6], 2f);
            ds.DrawLine(new Vector2(711.501f, 398.336f), new Vector2(711.501f, 406.67f), colors[6], 2f);
            ds.DrawLine(new Vector2(699.167f, 373.336f), new Vector2(700.501f, 359.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(707.501f, 360.003f), new Vector2(707.167f, 374.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(714.167f, 375.004f), new Vector2(713.167f, 326.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(707.833f, 338.003f), new Vector2(707.501f, 326.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(713.501f, 293.669f), new Vector2(713.501f, 301.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(721.167f, 378.336f), new Vector2(720.833f, 353.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(727.833f, 359.669f), new Vector2(728.167f, 397.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(684.501f, 340.669f), new Vector2(683.167f, 288.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(690.501f, 291.669f), new Vector2(688.833f, 338.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(695.501f, 327.336f), new Vector2(695.501f, 336.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(702.167f, 334.669f), new Vector2(702.167f, 327.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(696.501f, 300.003f), new Vector2(696.167f, 283.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(677.833f, 285.336f), new Vector2(676.167f, 340.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(670.833f, 343.335f), new Vector2(670.167f, 287.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(759.834f, 326.669f), new Vector2(758.834f, 374.004f), colors[6], 2f);
            ds.DrawLine(new Vector2(765.167f, 370.67f), new Vector2(765.167f, 354.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(764.834f, 344.669f), new Vector2(765.5f, 331.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(753.167f, 322.336f), new Vector2(753.5f, 335.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(753.834f, 357.335f), new Vector2(753.834f, 378.336f), colors[6], 2f);
            ds.DrawLine(new Vector2(748.167f, 382.004f), new Vector2(747.167f, 361.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(740.5f, 387.336f), new Vector2(740.5f, 361.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(735.834f, 393.336f), new Vector2(734.834f, 361.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(724.501f, 341.335f), new Vector2(725.501f, 321.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(730.833f, 336.003f), new Vector2(730.833f, 305.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(738.167f, 311.669f), new Vector2(738.167f, 333.003f), colors[6], 2f);
            ds.DrawLine(new Vector2(745.834f, 330.669f), new Vector2(745.834f, 316.669f), colors[6], 2f);
            ds.DrawLine(new Vector2(278.5f, 392f), new Vector2(273f, 402.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(260f, 403.5f), new Vector2(273f, 400.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(217.5f, 361.5f), new Vector2(230f, 368f), colors[6], 4f);
            ds.DrawLine(new Vector2(217.5f, 362.5f), new Vector2(219f, 359f), colors[6], 4f);
            ds.DrawLine(new Vector2(218.5f, 343.5f), new Vector2(223.5f, 353.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(265f, 305f), new Vector2(271f, 294f), colors[6], 4f);
            ds.DrawLine(new Vector2(270f, 294.5f), new Vector2(283f, 293f), colors[6], 4f);
            ds.DrawLine(new Vector2(281f, 291f), new Vector2(285.5f, 303f), colors[6], 4f);
            ds.DrawLine(new Vector2(286f, 303f), new Vector2(283f, 309f), colors[6], 4f);
            ds.DrawLine(new Vector2(251.5f, 302.5f), new Vector2(247f, 310f), colors[6], 4f);
            ds.DrawLine(new Vector2(221.5f, 355f), new Vector2(219f, 362.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(452f, 559f), new Vector2(438f, 561f), colors[6], 4f);
            ds.DrawLine(new Vector2(437.5f, 561f), new Vector2(435f, 567f), colors[6], 4f);
            ds.DrawLine(new Vector2(434.5f, 567f), new Vector2(439.5f, 574.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(439.5f, 575f), new Vector2(447.5f, 578.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(486.5f, 622f), new Vector2(495.5f, 610.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(494f, 609.5f), new Vector2(503f, 622f), colors[6], 4f);
            ds.DrawLine(new Vector2(510f, 610f), new Vector2(505f, 621f), colors[6], 4f);
            ds.DrawLine(new Vector2(536f, 563f), new Vector2(544f, 571f), colors[6], 4f);
            ds.DrawLine(new Vector2(544.5f, 569.5f), new Vector2(542.5f, 581.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(424f, 565.5f), new Vector2(434f, 558.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(745.5f, 331.5f), new Vector2(753.5f, 340f), colors[6], 4f);
            ds.DrawLine(new Vector2(746.5f, 347.5f), new Vector2(755f, 354f), colors[6], 4f);
            ds.DrawLine(new Vector2(726f, 387f), new Vector2(721f, 396f), colors[6], 4f);
            ds.DrawLine(new Vector2(693.5f, 391f), new Vector2(694f, 400.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(694.5f, 401f), new Vector2(701.5f, 405f), colors[6], 4f);
            ds.DrawLine(new Vector2(702f, 405f), new Vector2(710.5f, 395.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(712.5f, 303f), new Vector2(706.5f, 297f), colors[6], 4f);
            ds.DrawLine(new Vector2(700f, 296.5f), new Vector2(691.5f, 308f), colors[6], 4f);
            ds.DrawLine(new Vector2(692.5f, 306.5f), new Vector2(697f, 324.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(209.834f, 346f), new Vector2(209.834f, 378.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(203.834f, 355.333f), new Vector2(203.834f, 376f), colors[6], 2f);
            ds.DrawLine(new Vector2(215.834f, 323.333f), new Vector2(217.167f, 383.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(223.167f, 366f), new Vector2(223.834f, 390.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(230.5f, 367.333f), new Vector2(230.5f, 395.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(237.834f, 366.667f), new Vector2(235.834f, 400f), colors[6], 2f);
            ds.DrawLine(new Vector2(253.167f, 390.667f), new Vector2(252.5f, 410.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(254.5f, 354f), new Vector2(255.167f, 378f), colors[6], 2f);
            ds.DrawLine(new Vector2(245.167f, 389.333f), new Vector2(245.834f, 402.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(260.5f, 402f), new Vector2(260.5f, 415.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(268.5f, 398.667f), new Vector2(268.5f, 420f), colors[6], 2f);
            ds.DrawLine(new Vector2(273.834f, 396.667f), new Vector2(273.834f, 422f), colors[6], 2f);
            ds.DrawLine(new Vector2(310.5f, 354.5f), new Vector2(311.5f, 389.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(317f, 351f), new Vector2(317f, 372.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(521.5f, 177f), new Vector2(492f, 178.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(321.5f, 343.5f), new Vector2(331f, 334f), colors[6], 5f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(326.833f, 276.333f), new Vector2(334.5f, 295.333f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(336.5f, 273.667f), new Vector2(337.167f, 292f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(336.833f, 293f), new Vector2(340.167f, 305.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(341.833f, 289f), new Vector2(345.5f, 301f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(341.833f, 283.333f), new Vector2(344.833f, 268.667f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(354.833f, 264.667f), new Vector2(348.167f, 280f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(349.167f, 284.667f), new Vector2(350.833f, 297f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(354.833f, 282.667f), new Vector2(358.833f, 293.333f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(361.167f, 262.333f), new Vector2(355.5f, 280f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(338.833f, 308f), new Vector2(349.167f, 301f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(322.167f, 415.33f), new Vector2(329.167f, 433.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(319.5f, 428.997f), new Vector2(318.5f, 420.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(325.833f, 406.997f), new Vector2(340.5f, 440.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(328.833f, 399.664f), new Vector2(338.833f, 408.997f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(334.833f, 412.664f), new Vector2(347.167f, 424.997f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(340.833f, 427.33f), new Vector2(356.833f, 433.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(346.167f, 435.33f), new Vector2(357.167f, 440.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(340.833f, 408.997f), new Vector2(353.167f, 424.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(329.833f, 390.664f), new Vector2(339.5f, 399.33f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(341.5f, 400.997f), new Vector2(354.167f, 412.33f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(334.5f, 382.664f), new Vector2(338.5f, 390.664f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(406.25f, 489.75f), new Vector2(417.25f, 513.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(415.75f, 493f), new Vector2(421.5f, 504.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(423.5f, 512f), new Vector2(431.75f, 519.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(423f, 495f), new Vector2(432f, 503.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(431f, 505.75f), new Vector2(438.75f, 517.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(418f, 518.25f), new Vector2(425.5f, 522f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(436f, 499.5f), new Vector2(443.25f, 509f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(431.75f, 492.75f), new Vector2(445f, 493f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(442.75f, 499f), new Vector2(455.25f, 507.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(442.5f, 512.75f), new Vector2(451.5f, 510.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(533.5f, 506f), new Vector2(549.75f, 515.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(539f, 500.5f), new Vector2(545.5f, 505.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(544.75f, 494f), new Vector2(556.25f, 491.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(563.75f, 486.5f), new Vector2(557.75f, 499.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(550.75f, 500.25f), new Vector2(562.5f, 506.25f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(550f, 508.5f), new Vector2(568f, 513.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(555.75f, 516.5f), new Vector2(567f, 521.75f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(567f, 507f), new Vector2(567.5f, 498.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(568f, 491.5f), new Vector2(581.5f, 489.5f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(574.5f, 496.667f), new Vector2(571.5f, 509f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(625.5f, 405.335f), new Vector2(626.834f, 417.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(625.834f, 420.002f), new Vector2(628.5f, 431.002f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(623.167f, 437.335f), new Vector2(635.834f, 437.335f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(636.167f, 432.335f), new Vector2(634.167f, 416.002f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(634.834f, 410.335f), new Vector2(630.5f, 399.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(637.167f, 392.668f), new Vector2(641.5f, 403.002f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(641.834f, 381.668f), new Vector2(646.834f, 396.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(644.5f, 400.002f), new Vector2(655.834f, 408.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(640.167f, 408.335f), new Vector2(651.167f, 412.002f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(656.167f, 413.668f), new Vector2(646.5f, 419.668f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(642.167f, 415.668f), new Vector2(641.834f, 426.335f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(644.5f, 430.335f), new Vector2(657.834f, 424.002f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(627.167f, 272.339f), new Vector2(639.5f, 280.672f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(641.167f, 274.672f), new Vector2(651.834f, 291.339f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(648.5f, 279.672f), new Vector2(657.167f, 287.672f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(628.834f, 285.006f), new Vector2(639.167f, 284.672f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(637.834f, 290.339f), new Vector2(625.167f, 294.672f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(639.834f, 293.672f), new Vector2(644.5f, 304.672f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(645.834f, 292.339f), new Vector2(650.5f, 302.006f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(635.167f, 298.672f), new Vector2(641.5f, 312.006f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = innerDashStyle;
                ds.DrawLine(new Vector2(645.167f, 308.006f), new Vector2(645.5f, 318.006f), colors[6], 4f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(366.25f, 310.25f), new Vector2(365.5f, 330.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(362f, 316.75f), new Vector2(361f, 336.25f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(358f, 316.25f), new Vector2(357.75f, 331.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(354f, 324.25f), new Vector2(354f, 338f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(359.5f, 337.25f), new Vector2(356.75f, 342f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(349f, 364f), new Vector2(350.5f, 375.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(353f, 354.25f), new Vector2(355.25f, 379.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(357.25f, 358.25f), new Vector2(357.5f, 380.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(362.5f, 360.75f), new Vector2(361f, 370.25f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(364.5f, 364.5f), new Vector2(361.75f, 377f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(364.75f, 376.25f), new Vector2(364.25f, 392f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(461f, 485.5f), new Vector2(475.5f, 486.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(468.25f, 489.75f), new Vector2(482.5f, 490f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(474.75f, 495.25f), new Vector2(484f, 492.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(478.25f, 486f), new Vector2(487f, 486.25f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(502f, 486f), new Vector2(520.5f, 484.25f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(499.5f, 491f), new Vector2(512f, 490.25f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(506.5f, 493f), new Vector2(513f, 493f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(524.5f, 486.5f), new Vector2(515f, 489.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(618.5f, 316f), new Vector2(619.5f, 332.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(621.75f, 321.25f), new Vector2(621.75f, 334.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(624.75f, 327.5f), new Vector2(624.75f, 337.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(627.75f, 329.25f), new Vector2(627.5f, 342f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(630f, 332.75f), new Vector2(629.75f, 338.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(620.25f, 361.25f), new Vector2(620.75f, 379.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(623.25f, 359f), new Vector2(623.25f, 369.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(626.75f, 357.75f), new Vector2(626.75f, 368.5f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(624f, 371.5f), new Vector2(624f, 385f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(630.5f, 365.5f), new Vector2(626.25f, 376.75f), colors[6], 2f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = dotCapStyle;
                strokeStyle.EndCap = dotCapStyle;
                strokeStyle.DashCap = dotCapStyle;
                strokeStyle.CustomDashStyle = new float[] { 1f, 5f };
                ds.DrawLine(new Vector2(620.5f, 385.25f), new Vector2(618f, 392.75f), colors[6], 2f, strokeStyle);
            }
            ds.DrawLine(new Vector2(453.5f, 533.333f), new Vector2(489.5f, 534.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(463.833f, 528.667f), new Vector2(493.167f, 530f), colors[6], 2f);
            ds.DrawLine(new Vector2(473.833f, 524.667f), new Vector2(488.5f, 524.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(493.833f, 531.667f), new Vector2(537.167f, 532.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(524.5f, 526f), new Vector2(495.833f, 527.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(497.167f, 522f), new Vector2(513.833f, 522f), colors[6], 2f);
            ds.DrawLine(new Vector2(516.5f, 506f), new Vector2(508.5f, 511.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(579f, 262.5f), new Vector2(571f, 276f), colors[6], 4f);
            ds.DrawLine(new Vector2(576f, 275f), new Vector2(559f, 288f), colors[6], 4f);
            ds.DrawLine(new Vector2(564f, 272.5f), new Vector2(560.5f, 286.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(590f, 239.5f), new Vector2(584.5f, 256.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(589.5f, 252.5f), new Vector2(579f, 267f), colors[6], 4f);
            ds.DrawLine(new Vector2(592f, 262.5f), new Vector2(585f, 266.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(566f, 285.5f), new Vector2(557f, 295f), colors[6], 4f);
            ds.DrawLine(new Vector2(392f, 252f), new Vector2(402.5f, 256f), colors[6], 4f);
            ds.DrawLine(new Vector2(402f, 253f), new Vector2(409.5f, 264f), colors[6], 4f);
            ds.DrawLine(new Vector2(410f, 264f), new Vector2(418f, 273.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(418f, 270f), new Vector2(422.5f, 280.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(422.5f, 281f), new Vector2(428.5f, 285f), colors[6], 4f);
            ds.DrawLine(new Vector2(429.5f, 406f), new Vector2(420f, 419.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(431f, 408.5f), new Vector2(417f, 428.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(419f, 422.5f), new Vector2(405f, 433f), colors[6], 4f);
            ds.DrawLine(new Vector2(411f, 428.5f), new Vector2(401f, 445f), colors[6], 4f);
            ds.DrawLine(new Vector2(552.5f, 412.5f), new Vector2(564.5f, 426f), colors[6], 4f);
            ds.DrawLine(new Vector2(553.5f, 411.5f), new Vector2(554f, 422.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(551.5f, 410.5f), new Vector2(570.5f, 410.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(427.833f, 285f), new Vector2(414.167f, 290f), colors[6], 2f);
            ds.DrawLine(new Vector2(418.5f, 289f), new Vector2(400.5f, 296f), colors[6], 2f);
            ds.DrawLine(new Vector2(400.167f, 296f), new Vector2(392.833f, 300.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(425.833f, 286f), new Vector2(412.167f, 294f), colors[6], 2f);
            ds.DrawLine(new Vector2(426.5f, 285.667f), new Vector2(404.833f, 289f), colors[6], 2f);
            ds.DrawLine(new Vector2(404.5f, 289f), new Vector2(392.5f, 293.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(392.833f, 293.667f), new Vector2(381.833f, 301f), colors[6], 2f);
            ds.DrawLine(new Vector2(389.167f, 302.667f), new Vector2(379.167f, 302.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(388.5f, 303.667f), new Vector2(382.833f, 314f), colors[6], 2f);
            ds.DrawLine(new Vector2(392.833f, 306.333f), new Vector2(382.5f, 315.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(398.5f, 305.333f), new Vector2(391.167f, 312.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(397.167f, 304.333f), new Vector2(387.5f, 312.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(396.833f, 305.667f), new Vector2(396.833f, 317f), colors[6], 2f);
            ds.DrawLine(new Vector2(438.833f, 255.001f), new Vector2(434.167f, 267.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(435.167f, 267.668f), new Vector2(434.167f, 278.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(430.5f, 255.001f), new Vector2(430.5f, 271.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(430.5f, 272.001f), new Vector2(431.833f, 281.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(434.833f, 240.668f), new Vector2(431.5f, 255.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(434.167f, 245.001f), new Vector2(445.167f, 236.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(440.833f, 239.001f), new Vector2(452.833f, 234.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(453.5f, 233.001f), new Vector2(449.833f, 243.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(446.833f, 245.001f), new Vector2(456.167f, 239.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(451.5f, 245.335f), new Vector2(463.5f, 242.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(431.167f, 286.001f), new Vector2(424.167f, 272.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(426.167f, 277.335f), new Vector2(422.167f, 263.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(423.5f, 278.335f), new Vector2(412.5f, 274.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(423.833f, 280.335f), new Vector2(409.167f, 278.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(400.5f, 259.668f), new Vector2(389.5f, 259.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(400.833f, 256.335f), new Vector2(386.5f, 259.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(399.5f, 254.668f), new Vector2(403.167f, 243.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(403.833f, 242.001f), new Vector2(403.833f, 250.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(401.833f, 248.335f), new Vector2(403.5f, 257.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(397.5f, 254.668f), new Vector2(397.167f, 240.001f), colors[6], 2f);
            ds.DrawLine(new Vector2(426.167f, 409.666f), new Vector2(409.5f, 408.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(409.167f, 408.999f), new Vector2(402.167f, 407.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(402.167f, 407.666f), new Vector2(391.5f, 404.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(422.5f, 408.333f), new Vector2(408.5f, 404.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(408.5f, 404.666f), new Vector2(394.167f, 395.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(394.5f, 395.999f), new Vector2(386.833f, 386.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(386.5f, 386.333f), new Vector2(386.5f, 396.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(387.5f, 397.333f), new Vector2(379.5f, 388.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(375.5f, 386.999f), new Vector2(384.167f, 396.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(372.5f, 389.333f), new Vector2(378.833f, 401.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(375.833f, 387.666f), new Vector2(377.167f, 401.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(376.167f, 400.999f), new Vector2(383.167f, 405.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(385.167f, 405.999f), new Vector2(380.5f, 405.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(377.833f, 406.999f), new Vector2(389.167f, 413.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(388.833f, 413.333f), new Vector2(407.167f, 415.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(401.167f, 413.999f), new Vector2(416.167f, 412.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(415.833f, 412.999f), new Vector2(425.833f, 411.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(425.167f, 417.332f), new Vector2(425.167f, 432.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(425.167f, 432.666f), new Vector2(427.833f, 449.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(428.167f, 449.999f), new Vector2(435.167f, 460.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(435.5f, 455.999f), new Vector2(440.833f, 465.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(437.5f, 461.666f), new Vector2(449.167f, 467.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(449.167f, 468.332f), new Vector2(461.833f, 468.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(443.833f, 453.332f), new Vector2(451.833f, 461.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(451.5f, 461.666f), new Vector2(461.167f, 467.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(444.5f, 453.666f), new Vector2(454.833f, 456.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(443.833f, 445.666f), new Vector2(455.5f, 454.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(436.833f, 436.332f), new Vector2(448.5f, 448.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(428.167f, 416.999f), new Vector2(436.167f, 439.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(427.833f, 419.332f), new Vector2(431.833f, 434.999f), colors[6], 2f);
            ds.DrawLine(new Vector2(431.5f, 434.999f), new Vector2(435.5f, 447.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(435.5f, 447.332f), new Vector2(437.833f, 452.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(430.833f, 415.999f), new Vector2(437.167f, 435.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(437.5f, 435.666f), new Vector2(441.833f, 442.332f), colors[6], 2f);
            ds.DrawLine(new Vector2(429.833f, 411.332f), new Vector2(431.833f, 419.666f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.166f, 412f), new Vector2(546.833f, 427f), colors[6], 2f);
            ds.DrawLine(new Vector2(547.166f, 427.333f), new Vector2(545.5f, 435.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(545.166f, 435.333f), new Vector2(537.166f, 446.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(536.833f, 446.333f), new Vector2(528.5f, 455.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(538.833f, 450f), new Vector2(527.833f, 456.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(537.166f, 452f), new Vector2(533.166f, 459f), colors[6], 2f);
            ds.DrawLine(new Vector2(533.5f, 459.333f), new Vector2(526.833f, 467.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(522.166f, 468f), new Vector2(537.166f, 462.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(542.833f, 457.667f), new Vector2(536.833f, 466.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(542.833f, 458.667f), new Vector2(540.166f, 468f), colors[6], 2f);
            ds.DrawLine(new Vector2(537.166f, 469.333f), new Vector2(548.166f, 463.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(553.166f, 418.333f), new Vector2(551.5f, 431.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(554.166f, 421.667f), new Vector2(547.5f, 443.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.833f, 433.333f), new Vector2(544.5f, 448.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(554.833f, 422.333f), new Vector2(552.166f, 442.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.166f, 443f), new Vector2(547.5f, 457.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(547.5f, 458f), new Vector2(546.166f, 465.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(564.5f, 424.333f), new Vector2(572.833f, 435f), colors[6], 2f);
            ds.DrawLine(new Vector2(564.166f, 424.333f), new Vector2(580.166f, 440f), colors[6], 2f);
            ds.DrawLine(new Vector2(558.5f, 423f), new Vector2(560.5f, 432.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(561.166f, 432.667f), new Vector2(563.833f, 438f), colors[6], 2f);
            ds.DrawLine(new Vector2(560.5f, 419f), new Vector2(569.5f, 424f), colors[6], 2f);
            ds.DrawLine(new Vector2(569.833f, 424f), new Vector2(577.833f, 424f), colors[6], 2f);
            ds.DrawLine(new Vector2(562.166f, 408.333f), new Vector2(573.166f, 399f), colors[6], 2f);
            ds.DrawLine(new Vector2(563.833f, 407.333f), new Vector2(573.5f, 406.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(566.833f, 410.333f), new Vector2(583.5f, 400.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(583.5f, 401f), new Vector2(592.833f, 395f), colors[6], 2f);
            ds.DrawLine(new Vector2(567.833f, 407.667f), new Vector2(582.5f, 409.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(582.833f, 409.333f), new Vector2(597.166f, 403.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(597.166f, 403f), new Vector2(607.5f, 394.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(607.166f, 390.667f), new Vector2(599.166f, 392.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(601.166f, 394.333f), new Vector2(607.166f, 391.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(602.166f, 390f), new Vector2(607.166f, 382f), colors[6], 2f);
            ds.DrawLine(new Vector2(607.166f, 381.667f), new Vector2(608.166f, 369.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(606.5f, 370.333f), new Vector2(601.166f, 378f), colors[6], 2f);
            ds.DrawLine(new Vector2(605.5f, 374.667f), new Vector2(600.833f, 378.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(601.166f, 376.333f), new Vector2(597.166f, 383.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(596.833f, 383.333f), new Vector2(589.833f, 383.667f), colors[6], 2f);
            ds.DrawLine(new Vector2(589.833f, 383.667f), new Vector2(599.833f, 370f), colors[6], 2f);
            ds.DrawLine(new Vector2(553.166f, 248.668f), new Vector2(544.833f, 241.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(545.166f, 241.335f), new Vector2(538.5f, 237.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(538.166f, 237.335f), new Vector2(537.5f, 237.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(532.833f, 236.002f), new Vector2(536.166f, 244.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(535.5f, 237.335f), new Vector2(541.166f, 250.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(541.166f, 251.002f), new Vector2(529.833f, 246.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(546.166f, 251.335f), new Vector2(555.166f, 264.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.833f, 262.002f), new Vector2(556.5f, 279.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(556.5f, 284.335f), new Vector2(557.166f, 265.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(558.5f, 269.335f), new Vector2(559.5f, 252.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(553.833f, 241.335f), new Vector2(561.5f, 253.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(553.166f, 243.668f), new Vector2(555.5f, 252.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(563.833f, 292.668f), new Vector2(569.833f, 295.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(564.833f, 290.335f), new Vector2(576.833f, 294.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(565.833f, 290.002f), new Vector2(580.833f, 290.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(576.833f, 289.002f), new Vector2(590.5f, 289.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(586.833f, 291.335f), new Vector2(597.166f, 294.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(595.166f, 292.335f), new Vector2(600.833f, 299.335f), colors[6], 2f);
            ds.DrawLine(new Vector2(595.833f, 298.002f), new Vector2(604.833f, 305.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(604.5f, 306.002f), new Vector2(607.833f, 314.668f), colors[6], 2f);
            ds.DrawLine(new Vector2(595.833f, 307.002f), new Vector2(604.833f, 313.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(595.5f, 306.335f), new Vector2(596.5f, 316.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(590.5f, 312.002f), new Vector2(598.5f, 319.002f), colors[6], 2f);
            ds.DrawLine(new Vector2(401f, 442f), new Vector2(388f, 452.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(397.5f, 445f), new Vector2(393.5f, 455f), colors[6], 4f);
            ds.DrawLine(new Vector2(400.5f, 447f), new Vector2(400.5f, 456f), colors[6], 4f);
            ds.DrawLine(new Vector2(398.5f, 440f), new Vector2(392f, 441f), colors[6], 4f);
            ds.DrawLine(new Vector2(419f, 422.5f), new Vector2(405.5f, 425f), colors[6], 4f);
            ds.DrawLine(new Vector2(421f, 415f), new Vector2(407.5f, 418.5f), colors[6], 4f);
            ds.DrawLine(new Vector2(586.5f, 259f), new Vector2(604.5f, 259f), colors[6], 4f);
            ds.DrawLine(new Vector2(582.5f, 441.5f), new Vector2(581.5f, 456f), colors[6], 2f);
            ds.DrawLine(new Vector2(583.5f, 440.5f), new Vector2(590f, 454.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(584f, 442.5f), new Vector2(595f, 445f), colors[6], 2f);
            ds.DrawLine(new Vector2(587f, 443f), new Vector2(592.5f, 453f), colors[6], 2f);
            ds.DrawLine(new Vector2(599.5f, 443f), new Vector2(602f, 449f), colors[6], 2f);
            ds.DrawLine(new Vector2(397.5f, 444.5f), new Vector2(386.5f, 444f), colors[6], 2f);
            ds.DrawLine(new Vector2(400.5f, 440f), new Vector2(388.5f, 442f), colors[6], 2f);
            ds.DrawLine(new Vector2(412.5f, 431.5f), new Vector2(419.5f, 434f), colors[6], 2f);
            ds.DrawLine(new Vector2(415.5f, 427f), new Vector2(415f, 436f), colors[6], 2f);
            ds.DrawLine(new Vector2(573.5f, 277f), new Vector2(597.167f, 263.333f), colors[6], 2f);
            ds.DrawLine(new Vector2(594.834f, 263.667f), new Vector2(606.5f, 264f), colors[6], 2f);
            ds.DrawLine(new Vector2(565.25f, 282.75f), new Vector2(567f, 264.75f), colors[6], 2f);
            ds.DrawLine(new Vector2(395.75f, 459f), new Vector2(411.25f, 454f), colors[6], 2f);
            ds.DrawLine(new Vector2(434.25f, 456.25f), new Vector2(417.25f, 456.25f), colors[6], 2f);
            ds.DrawLine(new Vector2(310.5f, 294f), new Vector2(308.5f, 346.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(315f, 306f), new Vector2(315.5f, 347.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(321.5f, 319.5f), new Vector2(322f, 341.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(590.5f, 292.5f), new Vector2(605.5f, 307f), colors[6], 2f);
            ds.DrawLine(new Vector2(459f, 247f), new Vector2(477f, 231f), colors[6], 2f);
            ds.DrawLine(new Vector2(425f, 412f), new Vector2(418.5f, 434.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(426.5f, 410.5f), new Vector2(403f, 428.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(424f, 417f), new Vector2(414.5f, 438.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(420.5f, 418.5f), new Vector2(410f, 441.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(558.5f, 294f), new Vector2(559f, 270.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(564.5f, 288.5f), new Vector2(574.5f, 261f), colors[6], 2f);
            ds.DrawLine(new Vector2(571f, 273f), new Vector2(583.5f, 248f), colors[6], 2f);
            ds.DrawLine(new Vector2(568f, 279.5f), new Vector2(601f, 271.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(565f, 287.5f), new Vector2(587f, 287f), colors[6], 2f);
            ds.DrawLine(new Vector2(567.5f, 281.5f), new Vector2(595f, 278f), colors[6], 2f);
            ds.DrawLine(new Vector2(425f, 281.5f), new Vector2(422f, 254f), colors[6], 2f);
            ds.DrawLine(new Vector2(419.5f, 271.5f), new Vector2(397.5f, 273.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(417f, 268.5f), new Vector2(389f, 268.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(417f, 269f), new Vector2(387.5f, 260f), colors[6], 2f);
            ds.DrawLine(new Vector2(416.5f, 271.5f), new Vector2(417f, 251.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(413.5f, 267.5f), new Vector2(417.5f, 246.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(417f, 246f), new Vector2(412.5f, 243.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(411.5f, 264f), new Vector2(413.5f, 245f), colors[6], 2f);
            ds.DrawLine(new Vector2(405.5f, 257.5f), new Vector2(400.5f, 234f), colors[6], 2f);
            ds.DrawLine(new Vector2(402f, 252f), new Vector2(397.5f, 238.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(398.5f, 256f), new Vector2(376f, 259f), colors[6], 2f);
            ds.DrawLine(new Vector2(562.5f, 426f), new Vector2(566.5f, 438.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(556f, 420.5f), new Vector2(558f, 434f), colors[6], 2f);
            ds.DrawLine(new Vector2(564.5f, 424f), new Vector2(572f, 448f), colors[6], 2f);
            ds.DrawLine(new Vector2(572f, 430f), new Vector2(575.5f, 445f), colors[6], 2f);
            ds.DrawLine(new Vector2(552.5f, 413f), new Vector2(577.5f, 418f), colors[6], 2f);
            ds.DrawLine(new Vector2(554f, 413.5f), new Vector2(579f, 412.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(559.5f, 416.5f), new Vector2(575.5f, 422.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(563.5f, 424f), new Vector2(585f, 430.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(569.5f, 429f), new Vector2(584.5f, 437f), colors[6], 2f);
            ds.DrawLine(new Vector2(580.5f, 439f), new Vector2(597f, 440.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(599f, 370.5f), new Vector2(585f, 384.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(586f, 384.5f), new Vector2(574.5f, 396.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(577.5f, 393.5f), new Vector2(568f, 404.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(600f, 395.5f), new Vector2(611.5f, 392.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(560.5f, 290.5f), new Vector2(577.5f, 297f), colors[6], 2f);
            ds.DrawLine(new Vector2(575.5f, 297.5f), new Vector2(588f, 307.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(573f, 297.5f), new Vector2(591f, 315f), colors[6], 2f);
            ds.DrawLine(new Vector2(528.5f, 248f), new Vector2(545.5f, 265.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(542.5f, 261f), new Vector2(554.5f, 278f), colors[6], 2f);
            ds.DrawLine(new Vector2(551f, 278.5f), new Vector2(557f, 291.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(548.5f, 272.5f), new Vector2(556f, 283.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(460f, 247f), new Vector2(451f, 259.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(450.5f, 257.5f), new Vector2(439.5f, 270.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(442f, 266.5f), new Vector2(432.5f, 285.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(453.5f, 258f), new Vector2(442f, 266.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(397.5f, 316f), new Vector2(406f, 302f), colors[6], 2f);
            ds.DrawLine(new Vector2(405.5f, 303f), new Vector2(419f, 292.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(417f, 290f), new Vector2(428.5f, 287.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(433.5f, 285.5f), new Vector2(422f, 288f), colors[6], 2f);
            ds.DrawLine(new Vector2(427.5f, 287.5f), new Vector2(417.5f, 297.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(417.5f, 297.5f), new Vector2(406f, 303.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(355f, 345.5f), new Vector2(367f, 337f), colors[6], 2f);
            ds.DrawLine(new Vector2(367.5f, 337f), new Vector2(379.5f, 332.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(380f, 332.5f), new Vector2(390f, 331f), colors[6], 2f);
            ds.DrawLine(new Vector2(390f, 331f), new Vector2(391f, 331f), colors[6], 2f);
            ds.DrawLine(new Vector2(391.5f, 331f), new Vector2(392.5f, 321.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(393f, 321.5f), new Vector2(396f, 316f), colors[6], 2f);
            ds.DrawLine(new Vector2(356f, 348.5f), new Vector2(365f, 357.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(365f, 357f), new Vector2(374.5f, 363f), colors[6], 2f);
            ds.DrawLine(new Vector2(375f, 363f), new Vector2(385f, 368f), colors[6], 2f);
            ds.DrawLine(new Vector2(385.5f, 367.5f), new Vector2(388f, 381f), colors[6], 2f);
            ds.DrawLine(new Vector2(388.5f, 381f), new Vector2(399.5f, 392.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(399f, 392.5f), new Vector2(417f, 402.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(416.5f, 403f), new Vector2(431f, 406f), colors[6], 2f);
            ds.DrawLine(new Vector2(430.5f, 408.5f), new Vector2(437.5f, 429.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(437.5f, 430f), new Vector2(449.5f, 443f), colors[6], 2f);
            ds.DrawLine(new Vector2(449f, 443f), new Vector2(464f, 453.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(464.5f, 453.5f), new Vector2(478.5f, 461.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(478.5f, 462f), new Vector2(480f, 471f), colors[6], 2f);
            ds.DrawLine(new Vector2(480f, 471.5f), new Vector2(487.5f, 483f), colors[6], 2f);
            ds.DrawLine(new Vector2(498.5f, 485f), new Vector2(506.5f, 472.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(506.5f, 472f), new Vector2(510f, 461f), colors[6], 2f);
            ds.DrawLine(new Vector2(510f, 461f), new Vector2(521.5f, 454.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(522f, 454.5f), new Vector2(537.5f, 444.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(587.5f, 312.5f), new Vector2(595.5f, 321f), colors[6], 2f);
            ds.DrawLine(new Vector2(596f, 321.5f), new Vector2(598.5f, 331f), colors[6], 2f);
            ds.DrawLine(new Vector2(598f, 331f), new Vector2(608f, 335f), colors[6], 2f);
            ds.DrawLine(new Vector2(608f, 335f), new Vector2(618f, 339.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(618.5f, 339.5f), new Vector2(627.5f, 347.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(628f, 353f), new Vector2(617.5f, 356f), colors[6], 2f);
            ds.DrawLine(new Vector2(617.5f, 356.5f), new Vector2(603.5f, 360f), colors[6], 2f);
            ds.DrawLine(new Vector2(604f, 360f), new Vector2(599f, 361.5f), colors[6], 2f);
            ds.DrawLine(new Vector2(599.5f, 361.5f), new Vector2(593.5f, 376f), colors[6], 2f);
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(378.5f, 450.667f), new Vector2(380.5f, 468.667f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(377.833f, 458f), new Vector2(371.167f, 407.334f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(370.5f, 406f), new Vector2(371.167f, 294f), colors[6], 5f, strokeStyle);
            }
            {
                CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();
                strokeStyle.StartCap = CanvasCapStyle.Square;
                strokeStyle.EndCap = CanvasCapStyle.Square;
                strokeStyle.DashCap = CanvasCapStyle.Square;
                ds.DrawLine(new Vector2(371.167f, 296.667f), new Vector2(378.5f, 242f), colors[6], 5f, strokeStyle);
            }
            ds.FillCircle(new Vector2(493.5f, 297f), 53f, colors[7]);
            ds.FillCircle(new Vector2(492.5f, 401f), 53f, colors[7]);
            ds.FillEllipse(new Vector2(447.5f, 349f), 53f, 53f, colors[7]);
            ds.FillEllipse(new Vector2(537.5f, 351f), 53f, 53f, colors[7]);
            ds.FillEllipse(new Vector2(590.5f, 346f), 24.5f, 8.5f, colors[7]);
            ds.FillEllipse(new Vector2(395.5f, 348.5f), 24.5f, 8.5f, colors[7]);
            ds.FillEllipse(new Vector2(491.5f, 245f), 8.5f, 24.5f, colors[7]);
            ds.FillEllipse(new Vector2(494f, 451.5f), 8.5f, 24.5f, colors[7]);
            ds.DrawEllipse(new Vector2(253.5f, 600.668f), 6.667f, 6.667f, colors[5], 5f);
            ds.DrawEllipse(new Vector2(724.5f, 589.334f), 6.667f, 6.667f, colors[5], 5f);
            ds.DrawEllipse(new Vector2(457f, 311f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(435.667f, 308.667f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(452f, 288.667f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(534.334f, 284.667f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(531.333f, 307.333f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(554f, 308.667f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(530f, 385.667f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(551.334f, 389.666f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(532.333f, 408f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(451f, 386.334f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(449.667f, 410.334f), 16f, 16f, Colors.White, 2f);
            ds.DrawEllipse(new Vector2(429.333f, 390f), 16f, 16f, Colors.White, 2f);
            ds.FillEllipse(new Vector2(452.75f, 287.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(457.25f, 310.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(435.25f, 309.25f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(534.25f, 283.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(531.25f, 307.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(554.25f, 309.25f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(450.75f, 385.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(428.75f, 388.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(450.25f, 410.75f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(529.75f, 385.25f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(532.75f, 408.25f), 11.75f, 11.75f, Colors.White);
            ds.FillEllipse(new Vector2(551.25f, 389.25f), 11.75f, 11.75f, Colors.White);
            ds.DrawLine(new Vector2(435.5f, 290.5f), new Vector2(453f, 306f), Colors.White, 20f);
            ds.DrawLine(new Vector2(533.5f, 309.5f), new Vector2(552f, 293.5f), Colors.White, 20f);
            ds.DrawLine(new Vector2(533f, 388.5f), new Vector2(550.5f, 406.5f), Colors.White, 20f);
            ds.DrawLine(new Vector2(446.5f, 391f), new Vector2(433.5f, 404.5f), Colors.White, 20f);
            ds.FillEllipse(new Vector2(471.5f, 265.5f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(517.5f, 265.5f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(408f, 351f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(582.5f, 352.5f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(467.5f, 431.5f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(515.5f, 434f), 6f, 6f, Colors.White);
            ds.FillEllipse(new Vector2(451.5f, 289f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(435f, 307.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(457.5f, 309.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(536.5f, 285.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(533f, 307.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(553.5f, 306.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(532f, 387.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(549f, 392.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(532.5f, 406f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(450.5f, 387f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(449f, 409.5f), 8f, 8f, colors[1]);
            ds.FillEllipse(new Vector2(430.5f, 393f), 8f, 8f, colors[1]);

            ds.FillRectangle(new Rect(980, 0, 1000, 1000), Colors.White);
            ds.FillRectangle(new Rect(0, 735, 1000, 1000), Colors.White);
        }
    }
}
