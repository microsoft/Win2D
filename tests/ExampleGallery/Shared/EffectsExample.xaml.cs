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
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    public sealed partial class EffectsExample : UserControl
    {
        public enum EffectType
        {
            Blend,
            Composite,
            GaussianBlur,
            Saturation,
            Transform3D,
        }

        public EffectsExample()
        {
            this.InitializeComponent();
        }

        public List<EffectType> EffectsList { get { return Utils.GetEnumAsList<EffectType>(); } }

        public EffectType CurrentEffect { get; set; }

        CanvasBitmap bitmapTiger;
        ICanvasImage effect;
        string textLabel;
        Action<float> animationFunction;
        Stopwatch timer;
        bool isLoaded;

        async void Canvas_CreateResources(CanvasControl sender, object args)
        {
            bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
            
            effect = CreateEffect();

            isLoaded = true;
            this.canvas.Invalidate();
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;
            ds.Clear(Color.FromArgb(0, 0, 0, 0));

            if (!isLoaded)
                return;

            var position = new Vector2((float)sender.ActualWidth, (float)sender.ActualHeight) / 2;
            position -= bitmapTiger.Size.ToVector2() / 2;

            animationFunction((float)timer.Elapsed.TotalSeconds);

            ds.DrawImage(effect, position);

            if (textLabel != null)
            {
                ds.DrawText(textLabel, position - new Vector2(100, 50), Colors.White);
            }

            sender.Invalidate();
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var reallyChanged = e.AddedItems.Count != 1 ||
                                e.RemovedItems.Count != 1 ||
                                (EffectType)e.AddedItems[0] != (EffectType)e.RemovedItems[0];

            if (reallyChanged && isLoaded)
            {
                effect = CreateEffect();
            }
        }

        private ICanvasImage CreateEffect()
        {
            timer = Stopwatch.StartNew();
            textLabel = null;

            switch (CurrentEffect)
            {
                case EffectType.Blend:
                    {
                        var rotatedTiger = new Transform3DEffect()
                        {
                            Source = bitmapTiger
                        };

                        var blendEffect = new BlendEffect()
                        {
                            Background = bitmapTiger,
                            Foreground = rotatedTiger
                        };

                        int enumValueCount = Utils.GetEnumAsList<BlendEffectMode>().Count;

                        animationFunction = elapsedTime =>
                        {
                            blendEffect.Mode = (BlendEffectMode)(elapsedTime / Math.PI % enumValueCount);
                            textLabel = blendEffect.Mode.ToString();
                            rotatedTiger.TransformMatrix = Matrix4x4.CreateRotationZ((float)Math.Sin(elapsedTime));
                        };

                        return blendEffect;
                    }

                case EffectType.Composite:
                    {
                        var compositeEffect = new CompositeEffect()
                        {
                            Mode = CompositeEffectMode.SourceOver
                        };

                        var transformEffects = new List<Transform3DEffect>();

                        const int petalCount = 12;

                        for (int i = 0; i < petalCount; i++)
                        {
                            var transformEffect = new Transform3DEffect()
                            {
                                Source = bitmapTiger
                            };

                            compositeEffect.Inputs.Add(transformEffect);
                            transformEffects.Add(transformEffect);
                        }

                        animationFunction = elapsedTime =>
                        {
                            for (int i = 0; i < petalCount; i++)
                            {
                                transformEffects[i].TransformMatrix = Matrix4x4.CreateRotationZ(elapsedTime * i / 3);
                            }
                        };

                        return compositeEffect;
                    }

                case EffectType.GaussianBlur:
                    {
                        var blurEffect = new GaussianBlurEffect()
                        {
                            Source = bitmapTiger
                        };

                        animationFunction = elapsedTime =>
                        {
                            blurEffect.StandardDeviation = ((float)Math.Sin(elapsedTime * 3) + 1) * 6;
                        };

                        return blurEffect;
                    }

                case EffectType.Saturation:
                    {
                        var saturationEffect = new SaturationEffect()
                        {
                            Source = bitmapTiger
                        };

                        animationFunction = elapsedTime =>
                        {
                            saturationEffect.Saturation = ((float)Math.Sin(elapsedTime * 6) + 1) / 2;
                        };

                        return saturationEffect;
                    }

                case EffectType.Transform3D:
                    {
                        var transformEffect = new Transform3DEffect()
                        {
                            Source = bitmapTiger
                        };

                        animationFunction = elapsedTime =>
                        {
                            transformEffect.TransformMatrix = Matrix4x4.CreateFromYawPitchRoll(elapsedTime / 3,
                                                                                               elapsedTime / 7,
                                                                                               elapsedTime * 5);
                        };

                        return transformEffect;
                    }

                default:
                    throw new NotSupportedException();
            }
        }
    }
}
