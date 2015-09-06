// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using Windows.Foundation.Collections;
using Windows.Graphics.DirectX.Direct3D11;
using Windows.Media.Effects;
using Windows.Media.MediaProperties;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;

namespace ExampleGallery.Effects
{
    /// <summary>
    /// Win2D Gaussian Blur - http://microsoft.github.io/Win2D/html/T_Microsoft_Graphics_Canvas_Effects_GaussianBlurEffect.htm
    /// BlurAmount (in DIP) - 0 is no blur, default is 3, max for this demo is 12
    /// </summary>
    public sealed class DynamicBlurVideoEffect : IBasicVideoEffect
    {
        private CanvasDevice _canvasDevice;
        private IPropertySet _configuration;

        /// <summary>
        /// Value used for BlurAmount property
        /// </summary>
        public double BlurAmount
        {
            get
            {
                if (_configuration != null && _configuration.ContainsKey("BlurAmount"))
                    return (double)_configuration["BlurAmount"];

                return 3;
            }
            set
            {
                _configuration["BlurAmount"] = value;
            }
        }
        public bool IsReadOnly { get { return true; } }

        public IReadOnlyList<VideoEncodingProperties> SupportedEncodingProperties { get { return new List<VideoEncodingProperties>(); } }

        public MediaMemoryTypes SupportedMemoryTypes { get { return MediaMemoryTypes.Gpu; } }

        public bool TimeIndependent { get { return true; } }

        public void Close(MediaEffectClosedReason reason)
        {
            if(_canvasDevice != null) _canvasDevice.Dispose();
        }

        public void DiscardQueuedFrames()
        {
        }

        public void SetProperties(IPropertySet configuration)
        {
            _configuration = configuration;
        }

        public void SetEncodingProperties(VideoEncodingProperties encodingProperties, IDirect3DDevice device)
        {
            _canvasDevice = CanvasDevice.CreateFromDirect3D11Device(device);
        }

        public void ProcessFrame(ProcessVideoFrameContext context)
        {
            using (CanvasBitmap inputBitmap = CanvasBitmap.CreateFromDirect3D11Surface(_canvasDevice, context.InputFrame.Direct3DSurface))
            using (CanvasRenderTarget renderTarget = CanvasRenderTarget.CreateFromDirect3D11Surface(_canvasDevice, context.OutputFrame.Direct3DSurface))
            using (CanvasDrawingSession ds = renderTarget.CreateDrawingSession())
            {

                var gaussianBlurEffect = new GaussianBlurEffect
                {
                    Source = inputBitmap,
                    BlurAmount = (float)BlurAmount,
                    Optimization = EffectOptimization.Speed
                };

                ds.DrawImage(gaussianBlurEffect);
            }
        }
    }
}
