// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation.Collections;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Effects;
using Windows.Media.MediaProperties;
using Windows.UI.Text;

namespace ExampleGallery.Effects
{
    public sealed class ExampleVideoEffect : IBasicVideoEffect
    {
        public bool IsReadOnly { get { return false; } }

        public IReadOnlyList<VideoEncodingProperties> SupportedEncodingProperties { get { return new List<VideoEncodingProperties>(); } }

        public MediaMemoryTypes SupportedMemoryTypes { get { return MediaMemoryTypes.Gpu; } }

        public bool TimeIndependent { get { return true; } }

        public void Close(MediaEffectClosedReason reason)
        {
        }

        public void DiscardQueuedFrames()
        {
        }

        public void SetProperties(IPropertySet configuration)
        {
        }

        CanvasDevice canvasDevice;

        public void SetEncodingProperties(VideoEncodingProperties encodingProperties, IDirect3DDevice device)
        {
            canvasDevice = CanvasDevice.CreateFromDirect3D11Device(device);
        }


        public void ProcessFrame(ProcessVideoFrameContext context)
        {
            var inputSurface = context.InputFrame.Direct3DSurface;
            var outputSurface = context.OutputFrame.Direct3DSurface;

            using (var inputBitmap = CanvasBitmap.CreateFromDirect3D11Surface(canvasDevice, inputSurface))
            using (var renderTarget = CanvasRenderTarget.CreateFromDirect3D11Surface(canvasDevice, outputSurface))
            using (var ds = renderTarget.CreateDrawingSession())
            using (var brush = new CanvasImageBrush(canvasDevice, inputBitmap))
            using (var textCommandList = new CanvasCommandList(canvasDevice))
            {
                using (var clds = textCommandList.CreateDrawingSession())
                {
                    clds.DrawText(
                        "Win2D\nMediaClip",
                        (float)inputBitmap.Size.Width / 2,
                        (float)inputBitmap.Size.Height / 2,
                        brush,
                        new CanvasTextFormat()
                        {
                            FontSize = (float)inputBitmap.Size.Width / 5,
                            FontWeight = new FontWeight() { Weight = 999 },
                            HorizontalAlignment = CanvasHorizontalAlignment.Center,
                            VerticalAlignment = CanvasVerticalAlignment.Center
                        });
                }

                var background = new GaussianBlurEffect()
                {
                    BlurAmount = 10,
                    BorderMode = EffectBorderMode.Hard,
                    Source = new BrightnessEffect()
                    {
                        BlackPoint = new Vector2(0.5f, 0.7f),
                        Source = new SaturationEffect()
                        {
                            Saturation = 0,
                            Source = inputBitmap
                        }
                    }
                };

                var shadow = new ShadowEffect()
                {
                    Source = textCommandList,
                    BlurAmount = 10
                };

                var composite = new CompositeEffect()
                {
                    Sources = { background, shadow, textCommandList }
                };

                ds.DrawImage(composite);
            }
        }
    }
}
