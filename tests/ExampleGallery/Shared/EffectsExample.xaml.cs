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
using Microsoft.Graphics.Canvas.Effects;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class EffectsExample : UserControl
    {
        public enum EffectType
        {
            ArithmeticComposite,
            Blend,
            Border,
            Brightness,
            ColorMatrix,
            Composite,
            ConvolveMatrix,
            Crop,
            DirectionalBlur,
            DiscreteTransfer,
            DisplacementMap,
            GaussianBlur,
            HueRotation,
            Lighting,
            LuminanceToAlpha,
            Morphology,
            Saturation,
            Shadow,
            Tile,
            Transform3D,
        }

        public EffectsExample()
        {
            this.InitializeComponent();

            CurrentEffect = EffectType.DisplacementMap;
        }

        public List<EffectType> EffectsList { get { return Utils.GetEnumAsList<EffectType>(); } }

        public EffectType CurrentEffect { get; set; }

        CanvasBitmap bitmapTiger;
        ICanvasImage effect;
        Vector2 currentEffectSize;
        string textLabel;
        Action<float> animationFunction;
        Stopwatch timer;

        void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }

        async Task Canvas_CreateResourcesAsync(CanvasControl sender)
        {
            bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
            effect = CreateEffect();
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            float w = (float)sender.ActualWidth;
            float h = (float)sender.ActualHeight;

            // Animate and then display the current image effect.
            animationFunction((float)timer.Elapsed.TotalSeconds);

            ds.DrawImage(effect, (new Vector2(w, h) - currentEffectSize) / 2);

            // Draw text showing which effects are in use, but only if the screen is large enough to fit it.
            const float minSizeToShowText = 550;

            if (w > minSizeToShowText && h > minSizeToShowText)
            {
                string text = GetActiveEffectNames();

                if (textLabel != null)
                {
                    text += "\n\n" + textLabel;
                }

                ds.DrawText(text, 0, 80, Colors.White);
            }

            sender.Invalidate();
        }

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (canvas.ReadyToDraw)
            {
                effect = CreateEffect();
            }
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var reallyChanged = e.AddedItems.Count != 1 ||
                                e.RemovedItems.Count != 1 ||
                                (EffectType)e.AddedItems[0] != (EffectType)e.RemovedItems[0];

            if (reallyChanged && canvas.ReadyToDraw)
            {
                effect = CreateEffect();
            }
        }

        private ICanvasImage CreateEffect()
        {
            timer = Stopwatch.StartNew();
            currentEffectSize = bitmapTiger.Size.ToVector2();
            textLabel = null;

            switch (CurrentEffect)
            {
                case EffectType.ArithmeticComposite:
                    return CreateArithmeticComposite();

                case EffectType.Blend:
                    return CreateBlend();

                case EffectType.Border:
                    return CreateBorder();

                case EffectType.Brightness:
                    return CreateBrightness();

                case EffectType.ColorMatrix:
                    return CreateColorMatrix();

                case EffectType.Composite:
                    return CreateComposite();

                case EffectType.ConvolveMatrix:
                    return CreateConvolveMatrix();

                case EffectType.Crop:
                    return CreateCrop();

                case EffectType.DirectionalBlur:
                    return CreateDirectionalBlur();

                case EffectType.DiscreteTransfer:
                    return CreateDiscreteTransfer();

                case EffectType.DisplacementMap:
                    return CreateDisplacementMap();

                case EffectType.GaussianBlur:
                    return CreateGaussianBlur();

                case EffectType.HueRotation:
                    return CreateHueRotation();

                case EffectType.Lighting:
                    return CreateLighting();

                case EffectType.LuminanceToAlpha:
                    return CreateLuminanceToAlpha();

                case EffectType.Morphology:
                    return CreateMorphology();

                case EffectType.Saturation:
                    return CreateSaturation();

                case EffectType.Shadow:
                    return CreateShadow();

                case EffectType.Tile:
                    return CreateTile();

                case EffectType.Transform3D:
                    return CreateTransform3D();

                default:
                    throw new NotSupportedException();
            }
        }

        private ICanvasImage CreateArithmeticComposite()
        {
            // Combine two tiger images, the second one slightly rotated.
            var arithmeticEffect = new ArithmeticCompositeEffect
            {
                Source1 = bitmapTiger,

                Source2 = new Transform2DEffect
                {
                    Source = bitmapTiger,
                    TransformMatrix = Matrix3x2.CreateRotation(0.5f)
                }
            };

            // Animate the effect by changing parameters using sine waves of different frequencies.
            animationFunction = elapsedTime =>
            {
                arithmeticEffect.MultiplyAmount = (float)Math.Sin(elapsedTime * 23) * Math.Max(0, (float)Math.Sin(elapsedTime * 0.7));
                arithmeticEffect.Source1Amount = (float)Math.Sin(elapsedTime * 1.1);
                arithmeticEffect.Source2Amount = (float)Math.Sin(elapsedTime * 1.7);
                arithmeticEffect.Offset = (float)Math.Sin(elapsedTime * 0.2) / 2 + 0.25f;
            };

            return arithmeticEffect;
        }

        private ICanvasImage CreateBlend()
        {
            var rotatedTiger = new Transform3DEffect
            {
                Source = bitmapTiger
            };

            var blendEffect = new BlendEffect
            {
                Background = bitmapTiger,
                Foreground = rotatedTiger
            };

            // Animation swings the second copy of the image back and forth,
            // while cycling through the different blend modes.
            int enumValueCount = Utils.GetEnumAsList<BlendEffectMode>().Count;

            animationFunction = elapsedTime =>
            {
                blendEffect.Mode = (BlendEffectMode)(elapsedTime / Math.PI % enumValueCount);
                textLabel = "Mode: " + blendEffect.Mode;
                rotatedTiger.TransformMatrix = Matrix4x4.CreateRotationZ((float)Math.Sin(elapsedTime));
            };

            return blendEffect;
        }

        private ICanvasImage CreateBorder()
        {
            var borderEffect = new BorderEffect
            {
                Source = bitmapTiger
            };

            // Animation cycles through the different edge behavior modes.
            int enumValueCount = Utils.GetEnumAsList<CanvasEdgeBehavior>().Count;

            animationFunction = elapsedTime =>
            {
                borderEffect.ExtendX =
                borderEffect.ExtendY = (CanvasEdgeBehavior)(elapsedTime % enumValueCount);

                textLabel = "Mode: " + borderEffect.ExtendX;
            };

            return AddSoftEdgedCrop(borderEffect);
        }

        private ICanvasImage CreateBrightness()
        {
            var brightnessEffect = new BrightnessEffect
            {
                Source = bitmapTiger
            };

            // Animate the effect by changing parameters using sine waves of different frequencies.
            animationFunction = elapsedTime =>
            {
                brightnessEffect.BlackPoint = new Vector2(((float)Math.Sin(elapsedTime * 0.7) + 1) / 5,
                                                          ((float)Math.Sin(elapsedTime * 1.1) + 1) / 5);

                brightnessEffect.WhitePoint = new Vector2(1 - ((float)Math.Sin(elapsedTime * 1.3) + 1) / 5,
                                                          1 - ((float)Math.Sin(elapsedTime * 1.7) + 1) / 5);
            };

            return brightnessEffect;
        }

        private ICanvasImage CreateColorMatrix()
        {
            var colorMatrixEffect = new ColorMatrixEffect
            {
                Source = bitmapTiger
            };

            // Animation cycles through different color settings.
            animationFunction = elapsedTime =>
            {
                var matrix = new Matrix5x4();

                matrix.M11 = (float)Math.Sin(elapsedTime * 1.5);
                matrix.M21 = (float)Math.Sin(elapsedTime * 1.4);
                matrix.M31 = (float)Math.Sin(elapsedTime * 1.3);
                matrix.M51 = (1 - matrix.M11 - matrix.M21 - matrix.M31) / 2;

                matrix.M12 = (float)Math.Sin(elapsedTime * 1.2);
                matrix.M22 = (float)Math.Sin(elapsedTime * 1.1);
                matrix.M32 = (float)Math.Sin(elapsedTime * 1.0);
                matrix.M52 = (1 - matrix.M12 - matrix.M22 - matrix.M32) / 2;

                matrix.M13 = (float)Math.Sin(elapsedTime * 0.9);
                matrix.M23 = (float)Math.Sin(elapsedTime * 0.8);
                matrix.M33 = (float)Math.Sin(elapsedTime * 0.7);
                matrix.M53 = (1 - matrix.M13 - matrix.M23 - matrix.M33) / 2;

                matrix.M44 = 1;

                colorMatrixEffect.ColorMatrix = matrix;
            };

            return colorMatrixEffect;
        }

        private ICanvasImage CreateComposite()
        {
            var compositeEffect = new CompositeEffect { Mode = CanvasComposite.SourceOver };

            var transformEffects = new List<Transform3DEffect>();

            const int petalCount = 12;

            for (int i = 0; i < petalCount; i++)
            {
                var transformEffect = new Transform3DEffect
                {
                    Source = bitmapTiger
                };

                compositeEffect.Inputs.Add(transformEffect);
                transformEffects.Add(transformEffect);
            }

            // Animation rotates each copy of the image at a different speed.
            animationFunction = elapsedTime =>
            {
                for (int i = 0; i < petalCount; i++)
                {
                    transformEffects[i].TransformMatrix = Matrix4x4.CreateRotationZ(elapsedTime * i / 3);
                }
            };

            currentEffectSize = Vector2.Zero;

            return compositeEffect;
        }

        private ICanvasImage CreateConvolveMatrix()
        {
            var convolveEffect = new ConvolveMatrixEffect
            {
                Source = bitmapTiger,
                KernelWidth = 3,
                KernelHeight = 3,
            };

            float[][] filters =
            {
                new float[] { 0, 1, 0, 1, -4, 1, 0, 1, 0 },
                new float[] { -2, -1, 0, -1, 1, 1, 0, 1, 2 },
                new float[] { -1, -2, -1, -2, 13, -2, -1, -2, -1 },
                new float[] { 1f / 9, 1f / 9, 1f / 9, 1f / 9, 1f / 9, 1f / 9, 1f / 9, 1f / 9, 1f / 9 },
            };

            string[] filterNames =
            {
                "Edge detect",
                "Emboss",
                "Sharpen",
                "Box blur",
            };

            // Animation interpolates between different convolve filter matrices.
            animationFunction = elapsedTime =>
            {
                int prevFilter = (int)(elapsedTime % filters.Length);
                int nextFilter = (prevFilter + 1) % filters.Length;

                float mu = elapsedTime % 1;

                var convolve = new float[9];

                for (int i = 0; i < 9; i++)
                {
                    convolve[i] = filters[prevFilter][i] * (1 - mu) +
                                  filters[nextFilter][i] * mu;
                }

                convolveEffect.KernelMatrix = convolve;

                textLabel = string.Format("{0}\n{{\n    {1:0.0}, {2:0.0}, {3:0.0},\n    {4:0.0}, {5:0.0}, {6:0.0},\n    {7:0.0}, {8:0.0}, {9:0.0}\n}}",
                                          filterNames[mu < 0.5 ? prevFilter : nextFilter],
                                          convolve[0], convolve[1], convolve[2],
                                          convolve[3], convolve[4], convolve[5],
                                          convolve[6], convolve[7], convolve[8]);
            };

            return convolveEffect;
        }

        private ICanvasImage CreateCrop()
        {
            var cropEffect = new CropEffect
            {
                Source = bitmapTiger
            };

            // Animation alters what area of the bitmap is selected.
            animationFunction = elapsedTime =>
            {
                var w = 50 + (float)Math.Sin(elapsedTime / 2) * 40;
                var h = 50 + (float)Math.Sin(elapsedTime / 3) * 40;

                var range = bitmapTiger.Size.ToVector2() - new Vector2(w, h);

                var x = (Math.Sin(elapsedTime * 4) + 1) / 2 * range.X;
                var y = (Math.Sin(elapsedTime * 5) + 1) / 2 * range.Y;

                cropEffect.SourceRectangle = new Rect(x, y, w, h);
            };

            return cropEffect;
        }

        private ICanvasImage CreateDirectionalBlur()
        {
            var blurEffect = new DirectionalBlurEffect
            {
                Source = bitmapTiger
            };

            // Animation changes the blur direction and amount.
            animationFunction = elapsedTime =>
            {
                blurEffect.Angle = elapsedTime;
                blurEffect.BlurAmount = ((float)Math.Sin(elapsedTime * 2) + 1) * 16;
            };

            return blurEffect;
        }

        private ICanvasImage CreateDiscreteTransfer()
        {
            var discreteTransferEffect = new DiscreteTransferEffect
            {
                Source = bitmapTiger
            };

            float[][] tables =
            {
                new float[] { 0, 1 },
                new float[] { 1, 0 },
                new float[] { 0, 0.5f, 1 },
            };

            // Animation switches between different quantisation color transfer tables.
            animationFunction = elapsedTime =>
            {
                int t = (int)(elapsedTime * 2);

                discreteTransferEffect.RedTable = tables[t % tables.Length];
                discreteTransferEffect.GreenTable = tables[(t / tables.Length) % tables.Length];
                discreteTransferEffect.BlueTable = tables[(t / tables.Length / tables.Length) % tables.Length];
            };

            return discreteTransferEffect;
        }

        private ICanvasImage CreateDisplacementMap()
        {
            var turbulence = new Transform2DEffect
            {
                Source = new TurbulenceEffect
                {
                    Octaves = 4
                }
            };

            var displacementEffect = new DisplacementMapEffect
            {
                Source = bitmapTiger,
                Displacement = turbulence,
                XChannelSelect = EffectChannelSelect.Red,
                YChannelSelect = EffectChannelSelect.Green,
            };

            // Animation moves the displacement map from side to side, which creates a rippling effect.
            // It also gradually changes the displacement amount.
            animationFunction = elapsedTime =>
            {
                turbulence.TransformMatrix = Matrix3x2.CreateTranslation((float)Math.Cos(elapsedTime) * 50 - 50,
                                                                         (float)Math.Sin(elapsedTime) * 50 - 50);

                displacementEffect.Amount = (float)Math.Sin(elapsedTime * 0.7) * 50 + 75;

                currentEffectSize = bitmapTiger.Size.ToVector2() + new Vector2(displacementEffect.Amount / 2);
            };

            return displacementEffect;
        }

        private ICanvasImage CreateGaussianBlur()
        {
            var blurEffect = new GaussianBlurEffect
            {
                Source = bitmapTiger
            };

            // Animation changes the blur amount.
            animationFunction = elapsedTime =>
            {
                blurEffect.BlurAmount = ((float)Math.Sin(elapsedTime * 3) + 1) * 6;
            };

            return blurEffect;
        }

        private ICanvasImage CreateHueRotation()
        {
            var hueRotationEffect = new HueRotationEffect
            {
                Source = bitmapTiger
            };

            // Animation changes the hue.
            animationFunction = elapsedTime =>
            {
                hueRotationEffect.Angle = elapsedTime * 4;
            };

            return hueRotationEffect;
        }

        private ICanvasImage CreateLighting()
        {
            var heightMap = new LuminanceToAlphaEffect
            {
                Source = bitmapTiger
            };

            var distantDiffuseEffect = new DistantDiffuseEffect
            {
                Source = heightMap,
                HeightMapScale = 2
            };

            var distantSpecularEffect = new DistantSpecularEffect
            {
                Source = heightMap,
                SpecularExponent = 16
            };

            var pointDiffuseEffect = new PointDiffuseEffect
            {
                Source = heightMap,
                HeightMapScale = 2
            };

            var pointSpecularEffect = new PointSpecularEffect
            {
                Source = heightMap,
                SpecularExponent = 16
            };

            var spotDiffuseEffect = new SpotDiffuseEffect
            {
                Source = heightMap,
                HeightMapScale = 2,
                LimitingConeAngle = 0.25f,
                LightTarget = new Vector3(bitmapTiger.Size.ToVector2(), 0) / 2
            };

            var spotSpecularEffect = new SpotSpecularEffect
            {
                Source = heightMap,
                SpecularExponent = 16,
                LimitingConeAngle = 0.25f,
                LightTarget = new Vector3(bitmapTiger.Size.ToVector2(), 0) / 2
            };

            // Lay out all the different light types in a grid.
            var xgap = (float)bitmapTiger.Size.Width * 1.1f;
            var ygap = (float)bitmapTiger.Size.Height * 1.1f;

            var compositeEffect = new CompositeEffect
            {
                Inputs =
                {
                    AddTextOverlay(distantDiffuseEffect,  -xgap, -ygap),
                    AddTextOverlay(distantSpecularEffect, -xgap,  0),
                    AddTextOverlay(pointDiffuseEffect,    -0,    -ygap),
                    AddTextOverlay(pointSpecularEffect,   -0,     0),
                    AddTextOverlay(spotDiffuseEffect,      xgap, -ygap),
                    AddTextOverlay(spotSpecularEffect,     xgap,  0),

                    CombineDiffuseAndSpecular(distantDiffuseEffect, distantSpecularEffect, -xgap, ygap),
                    CombineDiffuseAndSpecular(pointDiffuseEffect,   pointSpecularEffect,    0,    ygap),
                    CombineDiffuseAndSpecular(spotDiffuseEffect,    spotSpecularEffect,     xgap, ygap),
                }
            };

            ICanvasImage finalEffect = compositeEffect;

            // Check the screen size, and scale down our output if the screen is too small to fit the whole thing as-is.
            var xScaleFactor = (float)canvas.ActualWidth / 750;
            var yScaleFactor = (float)canvas.ActualHeight / 500;

            var scaleFactor = Math.Min(xScaleFactor, yScaleFactor);

            if (scaleFactor < 1)
            {
                finalEffect = new Transform2DEffect
                {
                    Source = compositeEffect,
                    TransformMatrix = Matrix3x2.CreateScale(scaleFactor, bitmapTiger.Size.ToVector2() / 2)
                };
            }

            // Animation changes the light directions.
            animationFunction = elapsedTime =>
            {
                distantDiffuseEffect.Azimuth = 
                distantSpecularEffect.Azimuth = elapsedTime % ((float)Math.PI * 2);

                distantDiffuseEffect.Elevation =
                distantSpecularEffect.Elevation = (float)Math.PI / 4 + (float)Math.Sin(elapsedTime / 2) * (float)Math.PI / 8;

                pointDiffuseEffect.LightPosition = 
                pointSpecularEffect.LightPosition =
                spotDiffuseEffect.LightPosition =
                spotSpecularEffect.LightPosition = new Vector3((float)Math.Cos(elapsedTime), (float)Math.Sin(elapsedTime), 1) * 100;
            };

            return finalEffect;
        }

        private ICanvasImage CombineDiffuseAndSpecular(ICanvasImage diffuse, ICanvasImage specular, float x, float y)
        {
            var composite = new ArithmeticCompositeEffect
            {
                Source1 = new ArithmeticCompositeEffect
                {
                    Source1 = bitmapTiger,
                    Source2 = diffuse,
                    Source1Amount = 0,
                    Source2Amount = 0,
                    MultiplyAmount = 1,
                },
                Source2 = specular,
                Source1Amount = 1,
                Source2Amount = 1,
                MultiplyAmount = 0,
            };

            return AddTextOverlay(composite, x, y);
        }

        private ICanvasImage CreateLuminanceToAlpha()
        {
            var contrastAdjustedTiger = new LinearTransferEffect
            {
                Source = bitmapTiger,

                RedOffset = -3,
                GreenOffset = -3,
                BlueOffset = -3,
            };

            var tigerAlpha = new LuminanceToAlphaEffect
            {
                Source = contrastAdjustedTiger
            };

            var tigerAlphaWithWhiteRgb = new LinearTransferEffect
            {
                Source = tigerAlpha,
                RedOffset = 1,
                GreenOffset = 1,
                BlueOffset = 1,
                RedSlope = 0,
                GreenSlope = 0,
                BlueSlope = 0,
            };

            var recombinedRgbAndAlpha = new ArithmeticCompositeEffect
            {
                Source1 = tigerAlphaWithWhiteRgb,
                Source2 = bitmapTiger,
            };

            var movedTiger = new Transform2DEffect
            {
                Source = recombinedRgbAndAlpha
            };

            const float turbulenceSize = 128;

            var backgroundImage = new CropEffect
            {
                Source = new TileEffect
                {
                    Source = new TurbulenceEffect
                    {
                        Octaves = 8,
                        Size = new Vector2(turbulenceSize),
                        Tileable = true
                    },
                    SourceRectangle= new Rect(0, 0, turbulenceSize, turbulenceSize)
                },
                SourceRectangle = new Rect((bitmapTiger.Size.ToVector2() * -0.5f).ToPoint(),
                                           (bitmapTiger.Size.ToVector2() * 1.5f).ToPoint())
            };

            var tigerOnBackground = new BlendEffect
            {
                Foreground = movedTiger,
                Background = backgroundImage
            };

            // Animation moves the alpha bitmap around, and alters color transfer settings to change how solid it is.
            animationFunction = elapsedTime =>
            {
                contrastAdjustedTiger.RedSlope =
                contrastAdjustedTiger.GreenSlope =
                contrastAdjustedTiger.BlueSlope = ((float)Math.Sin(elapsedTime * 0.9) + 2) * 3;

                var dx = (float)Math.Cos(elapsedTime) * 50;
                var dy = (float)Math.Sin(elapsedTime) * 50;

                movedTiger.TransformMatrix = Matrix3x2.CreateTranslation(dx, dy);
            };

            return tigerOnBackground;
        }

        private ICanvasImage CreateMorphology()
        {
            var morphologyEffect = new MorphologyEffect
            {
                Source = bitmapTiger,
            };

            // Animation changes the morphology filter kernel size, and switches back and forth between the two modes.
            animationFunction = elapsedTime =>
            {
                var t = (int)((elapsedTime * 10) % 50);

                morphologyEffect.Mode = (t < 25) ? MorphologyEffectMode.Erode :
                                                   MorphologyEffectMode.Dilate;

                textLabel = "Mode: " + morphologyEffect.Mode;

                morphologyEffect.Width =
                morphologyEffect.Height = t % 25 + 1;
            };

            return morphologyEffect;
        }

        private ICanvasImage CreateSaturation()
        {
            var saturationEffect = new SaturationEffect
            {
                Source = bitmapTiger
            };

            // Animation changes the saturation amount.
            animationFunction = elapsedTime =>
            {
                saturationEffect.Saturation = ((float)Math.Sin(elapsedTime * 6) + 1) / 2;
            };

            return saturationEffect;
        }

        private ICanvasImage CreateShadow()
        {
            var renderTarget = new CanvasRenderTarget(canvas, 360, 150);

            using (var ds = renderTarget.CreateDrawingSession())
            {
                ds.Clear(Color.FromArgb(0, 0, 0, 0));

                ds.DrawText("This text is drawn onto a rendertarget", 10, 10, Colors.White);
                ds.DrawText("with a different color per line,", 10, 40, Colors.Red);
                ds.DrawText("after which a drop shadow is", 10, 70, Colors.Green);
                ds.DrawText("generated using image effects.", 10, 100, Colors.Blue);
            }

            var shadowEffect = new Transform2DEffect
            {
                Source = new ShadowEffect
                {
                    Source = renderTarget,
                    BlurAmount = 2
                },
                TransformMatrix = Matrix3x2.CreateTranslation(3, 3)
            };

            var whiteBackground = new CropEffect
            {
                Source = new ColorSourceEffect { Color = Colors.White },
                SourceRectangle = renderTarget.Bounds
            };

            var compositeEffect = new CompositeEffect
            {
                Inputs = { whiteBackground, shadowEffect, renderTarget }
            };

            animationFunction = elapsedTime => { };

            currentEffectSize = renderTarget.Size.ToVector2();

            return compositeEffect;
        }

        private ICanvasImage CreateTile()
        {
            var tileEffect = new TileEffect
            {
                Source = bitmapTiger,
                SourceRectangle = new Rect(105, 55, 40, 30)
            };

            animationFunction = elapsedTime => { };

            return AddSoftEdgedCrop(tileEffect);
        }

        private ICanvasImage CreateTransform3D()
        {
            var transformEffect = new Transform3DEffect
            {
                Source = bitmapTiger
            };

            // Animation rotates the bitmap in 3D space.
            animationFunction = elapsedTime =>
            {
                transformEffect.TransformMatrix = Matrix4x4.CreateFromYawPitchRoll(elapsedTime / 3,
                                                                                   elapsedTime / 7,
                                                                                   elapsedTime * 5);
            };

            currentEffectSize = Vector2.Zero;

            return transformEffect;
        }

        private ICanvasImage AddSoftEdgedCrop(ICanvasImage effect)
        {
            var size = bitmapTiger.Size;

            var softEdge = new GaussianBlurEffect
            {
                Source = new CropEffect
                {
                    Source = new ColorSourceEffect { Color = Colors.Black },
                    SourceRectangle = new Rect(-size.Width / 2, -size.Height / 2, size.Width * 2, size.Height * 2)
                },
                BlurAmount = 32
            };

            return new CompositeEffect
            {
                Inputs = { softEdge, effect },
                Mode = CanvasComposite.SourceIn 
            };
        }

        private ICanvasImage AddTextOverlay(ICanvasImage effect, float x, float y)
        {
            var textOverlay = new CanvasRenderTarget(canvas, 200, 30);

            using (var ds = textOverlay.CreateDrawingSession())
            {
                ds.Clear(Color.FromArgb(0, 0, 0, 0));
#if WINDOWS_UAP // TODO: fix reflection for UAPs
                ds.DrawText("<<EFFECT>>", 0, 0, Colors.White);
#else
                ds.DrawText(effect.GetType().Name.Replace("Effect", ""), 0, 0, Colors.White);
#endif
            }

            return new Transform2DEffect
            {
                Source = new BlendEffect
                {
                    Background = effect,
                    Foreground = textOverlay,
                    Mode = BlendEffectMode.Screen
                },
                TransformMatrix = Matrix3x2.CreateTranslation(x, y)
            };
        }

        private string GetActiveEffectNames()
        {
#if WINDOWS_UAP // TODO: fix reflection for UAPs
            return "<<TODO: metadata not working on UAP>>";
#else
            var typesSeen = new List<Type>();

            GetActiveEffects(effect, typesSeen);

            var names = from type in typesSeen
                        orderby type.Name
                        select type.Name;

            return "Effects used:\n    " + string.Join("\n    ", names.Distinct());
#endif
        }

        private static void GetActiveEffects(IEffectInput effectInput, List<Type> typesSeen)
        {
            IEffect effect = effectInput as IEffect;

            if (effect != null)
            {
                typesSeen.Add(effect.GetType());

                foreach (var input in effect.Inputs)
                {
                    GetActiveEffects(input, typesSeen);
                }
            }
        }
    }
}
