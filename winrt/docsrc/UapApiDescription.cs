// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// TODO #3356 - delete this file when we switch docs to have UAP as primary focus.
// It is needed because our docs are currently generated against the 8.1 flavor of Win2D, 
// yet we want docs to include the new effects that are only available in UAP. Therefore 
// this file explicitly declares the API surface of the new effect types, in the same 
// format as normally output by the ExtractAPISurface tool.

namespace Windows.Graphics.Imaging
{
    public class SoftwareBitmap { internal SoftwareBitmap() { } }
}

namespace Windows.UI.Input.Inking
{
    public class InkStroke { internal InkStroke() { } }
}

namespace Microsoft.Graphics.Canvas
{
    /// <summary></summary>
    public sealed partial class CanvasDrawingSession : global::System.IDisposable, global::Microsoft.Graphics.Canvas.ICanvasResourceCreatorWithDpi, global::Microsoft.Graphics.Canvas.ICanvasResourceCreator
    {
        /// <summary></summary>
        public void DrawInk(System.Collections.Generic.IEnumerable<Windows.UI.Input.Inking.InkStroke> inkStrokes) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public void DrawInk(System.Collections.Generic.IEnumerable<Windows.UI.Input.Inking.InkStroke> inkStrokes, bool highContrast) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public void DrawGradientMesh(Microsoft.Graphics.Canvas.Geometry.CanvasGradientMesh gradientMesh) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public void DrawGradientMesh(Microsoft.Graphics.Canvas.Geometry.CanvasGradientMesh gradientMesh, global::System.Single x, global::System.Single y) { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public void DrawGradientMesh(Microsoft.Graphics.Canvas.Geometry.CanvasGradientMesh gradientMesh, Microsoft.Graphics.Canvas.Numerics.Vector2 offset) { throw new System.NotImplementedException(); }
    }

    /// <summary></summary>
    public partial class CanvasBitmap
    {
        /// <summary></summary>
        public void CreateFromSoftwareBitmap(ICanvasResourceCreator resourceCreator, Windows.Graphics.Imaging.SoftwareBitmap sourceBitmap) { throw new System.NotImplementedException(); }
    }
}


namespace Microsoft.Graphics.Canvas.Geometry
{
    /// <summary></summary>
    public enum CanvasGradientMeshPatchEdge : int
    {
        /// <summary></summary>
        Aliased = 0,
        /// <summary></summary>
        Antialiased = 1,
        /// <summary></summary>
        AliasedAndInflated = 2,
    }

    /// <summary></summary>
    public struct CanvasGradientMeshPatch
    {
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point00;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point01;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point02;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point03;

        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point10;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point11;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point12;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point13;

        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point20;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point21;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point22;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point23;

        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point30;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point31;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point32;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Numerics.Vector2 Point33;

        /// <summary></summary>
        public global::Windows.UI.Color Color00;
        /// <summary></summary>
        public global::Windows.UI.Color Color03;
        /// <summary></summary>
        public global::Windows.UI.Color Color30;
        /// <summary></summary>
        public global::Windows.UI.Color Color33;

        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Geometry.CanvasGradientMeshPatchEdge Edge00To03;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Geometry.CanvasGradientMeshPatchEdge Edge03To33;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Geometry.CanvasGradientMeshPatchEdge Edge33To30;
        /// <summary></summary>
        public Microsoft.Graphics.Canvas.Geometry.CanvasGradientMeshPatchEdge Edge30To00;
    }

    /// <summary></summary>
    public sealed class CanvasGradientMesh : global::System.IDisposable
    {
        internal CanvasGradientMesh() { }

        /// <summary>Releases all resources used by the CanvasGradientMesh.</summary>
        public void Dispose() { throw new System.NotImplementedException(); }

        /// <summary></summary>
        public CanvasGradientMeshPatch[] Patches
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public global::Windows.Foundation.Rect GetBounds(CanvasDrawingSession drawingSession)
        {
            throw new System.NotImplementedException();
        }

        /// <summary></summary>
        public global::Microsoft.Graphics.Canvas.CanvasDevice Device
        {
            get { throw new System.NotImplementedException(); }
        }

        /// <summary></summary>
        public static CanvasGradientMeshPatch CreateCoonsPatch(
                    global::Microsoft.Graphics.Canvas.Numerics.Vector2[] points,
                    global::Windows.UI.Color[] colors,
                    CanvasGradientMeshPatchEdge[] edges)
        {
            throw new System.NotImplementedException();
        }

        /// <summary></summary>
        public static CanvasGradientMeshPatch CreateTensorPatch(
                    global::Microsoft.Graphics.Canvas.Numerics.Vector2[] points,
                    global::Windows.UI.Color[] colors,
                    CanvasGradientMeshPatchEdge[] edges)
        {
            throw new System.NotImplementedException();
        }
    }
}