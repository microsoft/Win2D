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
using System.Numerics;
using Windows.Foundation.Collections;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Effects;
using Windows.Media.MediaProperties;
using Windows.UI;

namespace ExampleGallery.Effects
{
    public sealed class DisplacementEffect : IBasicVideoEffect
    {
        CanvasDevice canvasDevice;

        public bool IsReadOnly { get { return true; } }

        public IReadOnlyList<VideoEncodingProperties> SupportedEncodingProperties { get { return new List<VideoEncodingProperties>(); } }

        public MediaMemoryTypes SupportedMemoryTypes { get { return MediaMemoryTypes.Gpu; } }

        public bool TimeIndependent { get { return false; } }

        public void Close(MediaEffectClosedReason reason)
        {
            if (canvasDevice != null) { canvasDevice.Dispose(); }
        }

        public void DiscardQueuedFrames() { }

        public void ProcessFrame(ProcessVideoFrameContext context)
        {
            using (CanvasBitmap input = CanvasBitmap.CreateFromDirect3D11Surface(canvasDevice, context.InputFrame.Direct3DSurface))
            using (CanvasRenderTarget output = CanvasRenderTarget.CreateFromDirect3D11Surface(canvasDevice, context.OutputFrame.Direct3DSurface))
            using (CanvasDrawingSession ds = output.CreateDrawingSession())
            {
                TimeSpan time = context.InputFrame.RelativeTime.HasValue ? context.InputFrame.RelativeTime.Value : new TimeSpan();

                float dispX = (float)Math.Cos(time.TotalSeconds) * 75f;
                float dispY = (float)Math.Sin(time.TotalSeconds) * 75f;

                ds.Clear(Colors.Black);

                var dispMap = new DisplacementMapEffect()
                {
                    Source = input,
                    XChannelSelect = EffectChannelSelect.Red,
                    YChannelSelect = EffectChannelSelect.Green,
                    Amount = 100f,
                    Displacement = new Transform2DEffect()
                    {
                        TransformMatrix = Matrix3x2.CreateTranslation(dispX, dispY),
                        Source = new BorderEffect()
                        {
                            ExtendX = CanvasEdgeBehavior.Mirror,
                            ExtendY = CanvasEdgeBehavior.Mirror,
                            Source = new TurbulenceEffect()
                            {
                                Octaves = 3
                            }
                        }
                    }
                };

                ds.DrawImage(dispMap, -25f, -25f);
            }
        }

        public void SetEncodingProperties(VideoEncodingProperties encodingProperties, IDirect3DDevice device)
        {
            canvasDevice = CanvasDevice.CreateFromDirect3D11Device(device);
        }

        public void SetProperties(IPropertySet configuration) { }
    }
}
