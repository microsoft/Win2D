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
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class LayersExample : UserControl
    {
        public enum LayerMode
        {
            LayerFade,
            PerPrimitiveFade,
            OpacityBrush,
            ClipGeometry,
        }

        public List<LayerMode> ModeList { get { return Utils.GetEnumAsList<LayerMode>(); } }
        public LayerMode CurrentMode { get; set; }

        BouncingBall[] bouncingBalls = new BouncingBall[]
        {
            new BouncingBall("One",   Colors.Red),
            new BouncingBall("Two",   Colors.Lime),
            new BouncingBall("Three", Colors.Blue),
            new BouncingBall("Four",  Colors.Yellow),
            new BouncingBall("Five",  Colors.Cyan),
            new BouncingBall("Six",   Colors.Magenta),
            new BouncingBall("Seven", Colors.LightGray),
            new BouncingBall("Eight", Colors.DarkGray),
            new BouncingBall("Nine",  Colors.Brown),
        };

        static CanvasTextFormat textFormat = new CanvasTextFormat
        {
            HorizontalAlignment = CanvasHorizontalAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center,
        };

        CanvasRadialGradientBrush gradientBrush;
        CanvasGeometry clipGeometry;

        public LayersExample()
        {
            this.InitializeComponent();
        }

        void CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            // Create a gradient brush, used to control layer opacity.
            var gradientStops = new CanvasGradientStop[]
            {
                new CanvasGradientStop { Position = 0,     Color = Colors.White       },
                new CanvasGradientStop { Position = 0.25f, Color = Colors.White       },
                new CanvasGradientStop { Position = 1,     Color = Colors.Transparent },
            };

            gradientBrush = new CanvasRadialGradientBrush(sender, gradientStops);

            // Create a star shaped geometry, used for layer clipping.
            clipGeometry = Utils.CreateStarGeometry(sender, 1, Vector2.Zero);
        }

        void Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            foreach (var ball in bouncingBalls)
            {
                ball.Update(sender.Size);
            }
        }

        void Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            foreach (var ball in bouncingBalls)
            {
                switch (CurrentMode)
                {
                    case LayerMode.LayerFade:
                        // Fade the layer opacity in and out.
                        // Note how this produces different results from LayerMode.PerPrimitiveFade!
                        using (ds.CreateLayer(ball.FadeAlpha))
                        {
                            ball.Draw(ds);
                        }
                        break;

                    case LayerMode.PerPrimitiveFade:
                        // Not using layers at all: fade in and out by individually modifying the alpha
                        // of each primitive that makes up the ball shape. The result is quite different
                        // from LayerMode.LayerFade, which fades out the entire contents of the layer
                        // as a single operation. When primitives are faded individually, making the
                        // main filled circle partially translucent allows the black drop shadow circle
                        // to show through from below it, so the balls become darker as they fade out.
                        // Also note how the inner portion of the white X becomes visible during fades,
                        // which it is not when using LayerMode.LayerFade.
                        ball.Draw(ds, ball.FadeAlpha);
                        break;

                    case LayerMode.OpacityBrush:
                        // Use a brush to modify opacity of the layer contents.
                        gradientBrush.Center = ball.Position;
                        gradientBrush.RadiusX = gradientBrush.RadiusY = ball.Radius * ball.FadeAlpha * 3;

                        using (ds.CreateLayer(gradientBrush))
                        {
                            ball.Draw(ds);
                        }
                        break;

                    case LayerMode.ClipGeometry:
                        // Clip the layer using a geometry region.
                        var clipTransform = Matrix3x2.CreateRotation(ball.FadeAge * 2) *
                                            Matrix3x2.CreateScale(ball.Radius * ball.FadeAlpha * 4) *
                                            Matrix3x2.CreateTranslation(ball.Position);

                        using (ds.CreateLayer(1, clipGeometry, clipTransform))
                        {
                            ball.Draw(ds);
                        }
                        break;
                }
            }

            if (CurrentMode == LayerMode.PerPrimitiveFade)
            {
                ds.DrawText("Not using layers!\n\n" +
                            "Note how the drop shadow circles and inside of\n" +
                            "the white X's show through as the shapes fade out",
                            sender.Size.ToVector2() / 2, Colors.White, textFormat);
            }
        }


        class BouncingBall
        {
            string label;
            Color color;

            Vector2 Velocity { get; set; }
            public Vector2 Position { get; private set; }
            public float Radius { get; private set; }

            float FadeSpeed { get; set; }
            public float FadeAge { get; private set; }

            public float FadeAlpha
            {
                get
                {
                    float alpha = FadeAge % 2;

                    if (alpha > 1)
                        alpha = 2 - alpha;

                    return alpha;
                }
            }

            public BouncingBall(string label, Color color)
            {
                this.label = label;
                this.color = color;

                Velocity = new Vector2(RandomBetween(-2, 2), RandomBetween(-2, 2));
                Radius = RandomBetween(25, 100);
                FadeSpeed = RandomBetween(0.005f, 0.01f);
            }

            public void Update(Size controlSize)
            {
                // Move the ball.
                Position += Velocity;

                // Bounce if we hit the edge.
                Vector2 topLeft = new Vector2(Radius);
                Vector2 bottomRight = controlSize.ToVector2() - new Vector2(Radius);

                float bounceX = (Position.X < topLeft.X || Position.X > bottomRight.X) ? -1 : 1;
                float bounceY = (Position.Y < topLeft.Y || Position.Y > bottomRight.Y) ? -1 : 1;

                Velocity *= new Vector2(bounceX, bounceY);

                Position = Vector2.Clamp(Position, topLeft, bottomRight);

                // Gradually fade in and out.
                FadeAge += FadeSpeed;
            }

            public void Draw(CanvasDrawingSession ds, float alpha = 1)
            {
                // Create a drop shadow by drawing a black circle with an offset position.
                const float dropShadowOffset = 4;

                ds.FillCircle(Position + new Vector2(dropShadowOffset), Radius, AdjustAlpha(Colors.Black, alpha));

                // Draw a white X.
                const float crossWidth = 3;
                float crossSize = Radius * 0.8f;

                Vector2 cross1 = new Vector2(crossSize, crossSize);
                Vector2 cross2 = new Vector2(crossSize, -crossSize);

                ds.DrawLine(Position - cross1, Position + cross1, AdjustAlpha(Colors.White, alpha), crossWidth);
                ds.DrawLine(Position - cross2, Position + cross2, AdjustAlpha(Colors.White, alpha), crossWidth);

                // Fill the circle with its unique color.
                ds.FillCircle(Position, Radius, AdjustAlpha(color, alpha));

                // White border around it.
                ds.DrawCircle(Position, Radius, AdjustAlpha(Colors.White, alpha));

                // Text label.
                ds.DrawText(label, Position, AdjustAlpha(Colors.White, alpha), textFormat);
            }

            static Random random = new Random();

            static float RandomBetween(float min, float max)
            {
                return (float)random.NextDouble() * (max - min) + min;
            }

            static Color AdjustAlpha(Color color, float alpha)
            {
                color.A = (byte)(color.A * alpha);

                return color;
            }
        }


        private void control_Unloaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
