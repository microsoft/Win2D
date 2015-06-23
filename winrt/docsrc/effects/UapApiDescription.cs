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

// TODO #3356 - delete this file when we switch docs to have UAP as primary focus.
// It is needed because our docs are currently generated against the 8.1 flavor of Win2D, 
// yet we want docs to include the new effects that are only available in UAP. Therefore 
// this file explicitly declares the API surface of the new effect types, in the same 
// format as normally output by the ExtractAPISurface tool.

namespace Microsoft.Graphics.Canvas.Effects
{
    /// <summary></summary>
    public sealed class ChromaKeyEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the ChromaKeyEffect class.</summary>
        public ChromaKeyEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the ChromaKeyEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Tolerance
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean InvertAlpha
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean Feather
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Windows.UI.Color Color
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class ContrastEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the ContrastEffect class.</summary>
        public ContrastEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the ContrastEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Contrast
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean ClampSource
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class EdgeDetectionEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the EdgeDetectionEffect class.</summary>
        public EdgeDetectionEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the EdgeDetectionEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean OverlayEdges
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.EdgeDetectionEffectMode Mode
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single BlurAmount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Amount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.CanvasAlphaMode AlphaMode
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public enum EdgeDetectionEffectMode : int
    {
        /// <summary></summary>
        Sobel = 0,
        /// <summary></summary>
        Prewitt = 1,
    }

    /// <summary></summary>
    public enum EffectHueColorSpace : int
    {
        /// <summary></summary>
        Hsv = 0,
        /// <summary></summary>
        Hsl = 1,
    }

    /// <summary></summary>
    public sealed class EmbossEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the EmbossEffect class.</summary>
        public EmbossEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the EmbossEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Angle
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Amount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class ExposureEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the ExposureEffect class.</summary>
        public ExposureEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the ExposureEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Exposure
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class GrayscaleEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the GrayscaleEffect class.</summary>
        public GrayscaleEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the GrayscaleEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class HighlightsAndShadowsEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the HighlightsAndShadowsEffect class.</summary>
        public HighlightsAndShadowsEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the HighlightsAndShadowsEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean SourceIsLinearGamma
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Shadows
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single MaskBlurAmount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Highlights
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Clarity
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class HueToRgbEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the HueToRgbEffect class.</summary>
        public HueToRgbEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the HueToRgbEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.EffectHueColorSpace SourceColorSpace
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class InvertEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the InvertEffect class.</summary>
        public InvertEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the InvertEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class PosterizeEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the PosterizeEffect class.</summary>
        public PosterizeEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the PosterizeEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Int32 RedValueCount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Int32 GreenValueCount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Int32 BlueValueCount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class RgbToHueEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the RgbToHueEffect class.</summary>
        public RgbToHueEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the RgbToHueEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.EffectHueColorSpace OutputColorSpace
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class SepiaEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the SepiaEffect class.</summary>
        public SepiaEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the SepiaEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Intensity
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.CanvasAlphaMode AlphaMode
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class SharpenEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the SharpenEffect class.</summary>
        public SharpenEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the SharpenEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.Single Threshold
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Amount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class StraightenEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the StraightenEffect class.</summary>
        public StraightenEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the StraightenEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Boolean MaintainSize
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.CanvasImageInterpolation InterpolationMode
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Angle
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class TemperatureAndTintEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the TemperatureAndTintEffect class.</summary>
        public TemperatureAndTintEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the TemperatureAndTintEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::System.Single Tint
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Temperature
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }

    /// <summary></summary>
    public sealed class VignetteEffect : global::Microsoft.Graphics.Canvas.ICanvasImage, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource, global::System.IDisposable, global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffect
    {
        /// <summary>Initializes a new instance of the VignetteEffect class.</summary>
        public VignetteEffect() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(global::Microsoft.Graphics.Canvas.CanvasDrawingSession drawingSession, global::Microsoft.Graphics.Canvas.Numerics.Matrix3x2 transform) { throw new System.NotImplementedException(); }

        /// <summary>Releases all resources used by the VignetteEffect.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.Effects.IGraphicsEffectSource Source
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Curve
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Windows.UI.Color Color
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.Single Amount
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::System.String Name
        {
            get { throw new System.NotImplementedException(); }
            set { throw new System.NotImplementedException(); }
        }
    }
}
