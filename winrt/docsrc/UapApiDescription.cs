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
    }

    /// <summary></summary>
    public partial class CanvasBitmap
    {
        /// <summary></summary>
        public void CreateFromSoftwareBitmap(ICanvasResourceCreator resourceCreator, Windows.Graphics.Imaging.SoftwareBitmap sourceBitmap) { throw new System.NotImplementedException(); }
    }
}