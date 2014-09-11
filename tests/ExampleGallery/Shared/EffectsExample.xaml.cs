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
            None,
            GaussianBlur,
            Saturation,
            Transform3D,
            Blend,
            Composite
        }

        public EffectsExample()
        {
            this.InitializeComponent();
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.canvas.Invalidate();
        }


        public List<EffectType> EffectsList { get { return GetEnumAsList<EffectType>(); } }

        private List<T> GetEnumAsList<T>()
        {
            return new List<T>(Enum.GetValues(typeof(T)).Cast<T>());
        }

        public EffectType CurrentEffect { get; set; }

        private CanvasBitmap bitmapTiger;
        private bool isLoaded;

        CanvasStrokeStyle hairlineStrokeStyle = new CanvasStrokeStyle()
        {
            TransformBehavior = CanvasStrokeTransformBehavior.Hairline
        };

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;
            ds.Clear(Color.FromArgb(0, 0, 0, 0));

            if (!isLoaded)
                return;

            Vector2 center = new Vector2();
            center.X = (float)(sender.ActualWidth - bitmapTiger.Size.Width) / 2;
            center.Y = (float)(sender.ActualHeight - bitmapTiger.Size.Height) / 2;

            ICanvasImage effect = null;

            switch(CurrentEffect)
            {
                case EffectType.None:
                    {
                        effect = bitmapTiger;
                        break;
                    }
                case EffectType.GaussianBlur:
                    {
                        effect = new GaussianBlurEffect()
                        {
                            Source = bitmapTiger,
                            StandardDeviation = 2.0f
                        };
                        break;
                    }
                case EffectType.Saturation:
                    effect = new SaturationEffect()
                    {
                        Saturation = 0.2f,
                        Source = bitmapTiger
                    };
                    break;
                case EffectType.Transform3D:
                    {
                        effect = new Transform3DEffect()
                        {
                            Source = bitmapTiger,
                            TransformMatrix = Matrix4x4.CreateRotationZ(0.5f, new Vector3(0, 0, 0))
                        };
                        break;
                    }
                case EffectType.Blend:
                    {
                        Transform3DEffect transformEffect = new Transform3DEffect()
                        {
                            Source = bitmapTiger,
                            TransformMatrix = Matrix4x4.CreateRotationZ(0.5f, new Vector3(0, 0, 0))
                        };
                        effect = new BlendEffect()
                        {
                            Background = bitmapTiger,
                            Foreground = transformEffect,
                            Mode = BlendEffectMode.SoftLight
                        };
                        break;
                    }
                case EffectType.Composite:
                    {
                        CompositeEffect compositeEffect = new CompositeEffect();
                        compositeEffect.Mode = CompositeEffectMode.SourceOver;

                        float angle = 0.0f;
                        float angleDelta = 0.5f;
                        int imageNumber = (int)(2 * Math.PI / angleDelta) + 1;
                        foreach (var i in Enumerable.Range(0, imageNumber))
                        {
                            Transform3DEffect nextTransformEffect = new Transform3DEffect();
                            nextTransformEffect.Source = bitmapTiger;
                            nextTransformEffect.TransformMatrix = Matrix4x4.CreateRotationZ(angle, new Vector3(0, 0, 0));
                            angle += angleDelta;
                            compositeEffect.Inputs.Add(nextTransformEffect);
                        }
                        Matrix4x4 translationMatrix = Matrix4x4.CreateTranslation((float)bitmapTiger.Size.Width / 2,
                                                                                  (float)bitmapTiger.Size.Height / 2,
                                                                                  0.0f);
                        // Move composite effect so it appear on the screen center
                        effect = new Transform3DEffect()
                        {
                            Source = compositeEffect,
                            TransformMatrix = translationMatrix
                        };
                        break;
                    }
            }

            ds.DrawImage(effect, center);
        }

        async void Canvas_CreateResources(CanvasControl sender, object args)
        {
            bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");

            isLoaded = true;
            this.canvas.Invalidate();
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.canvas.Invalidate();
        }
    }
}
