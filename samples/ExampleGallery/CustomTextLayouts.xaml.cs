// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

#if !WINDOWS_UWP
using Vector2 = Microsoft.Graphics.Canvas.Numerics.Vector2;
#endif

namespace ExampleGallery
{
    public sealed partial class CustomTextLayouts : UserControl
    {
        Rect layoutBox;
        const float desiredFontSize = 20.0f;
        List<Rect> layoutRectangles;
        List<LayoutBox> layoutBoxes;
        bool needsLayout;
        Point pointerPressPosition;

        public CustomTextLayouts()
        {
            this.InitializeComponent();

            DataContext = this;

            ShowLayoutRectangles = false;
            ShowSelectionBox = false;
            CurrentTextOption = TextOption.English;
            CurrentTextDirection = TextDirection.LeftToRight;
            CurrentLayoutShapeOption = LayoutShapeOption.TextOutline;
            ShouldJustify = true;
        }
        public enum TextOption
        {
            English,
            حسنا
        }
        public List<TextOption> TextOptions { get { return Utils.GetEnumAsList<TextOption>(); } }
        public TextOption CurrentTextOption { get; set; }

        public enum LayoutShapeOption
        {
            Box,
            Ellipse,
            Triangle,
            TextOutline
        }
        public List<LayoutShapeOption> LayoutShapeOptions { get { return Utils.GetEnumAsList<LayoutShapeOption>(); } }
        public LayoutShapeOption CurrentLayoutShapeOption { get; set; }
        public bool ShouldJustify { get; set; }

        public enum TextDirection
        {
            LeftToRight,
            RightToLeft
        }
        public List<TextDirection> TextDirections { get { return Utils.GetEnumAsList<TextDirection>(); } }
        public TextDirection CurrentTextDirection { get; set; }
        public bool ShowLayoutRectangles { get; set; }
        public bool ShowSelectionBox { get; set; }

        class RectangleComparer : IComparer<Rect>
        {
            bool m_isRightToLeft;

            public RectangleComparer(bool isRightToLeft)
            {
                m_isRightToLeft = isRightToLeft;
            }

            public int Compare(Rect a, Rect b)
            {
                if (!m_isRightToLeft)
                {
                    return a.Left < b.Left ? -1 : 1;
                }
                else
                {
                    return a.Left > b.Left ? -1 : 1;
                }
            }
        }

        //
        // This is a helper class for turning any source geometry into a set of rectangles
        // we can use for text layout. For the purposes of this demo, and because it looks 
        // cool, we've added a way to layout based off of arbitrary path data.
        //
        class PathReader : ICanvasPathReceiver
        {
            Rect lineRegion;

            List<Rect> unsortedOutput;
            
            float[] figureTopEdge;
            float[] figureBottomEdge;

            public PathReader(Rect lr)
            {
                lineRegion = lr;
                unsortedOutput = new List<Rect>();
            }

            public void BeginFigure(Vector2 point, CanvasFigureFill fill)
            {
                figureTopEdge = null;
                figureBottomEdge = null;

                if (AreClose(point.Y, (float)lineRegion.Top))
                {
                    figureTopEdge = new float[2] { point.X, point.X };
                }
                else if (AreClose(point.Y, (float)lineRegion.Bottom))
                {
                    figureBottomEdge = new float[2] { point.X, point.X };
                }
            }
            public void AddArc(Vector2 point, float x, float y, float z, CanvasSweepDirection sweepDirection, CanvasArcSize arcSize)
            {
                throw new NotSupportedException("This is never called, since we use CanvasGeometrySimplification.Lines on the geometry.");
            }
            public void AddCubicBezier(Vector2 controlPoint1, Vector2 controlPoint2, Vector2 endPoint)
            {
                throw new NotSupportedException("This is never called, since we use CanvasGeometrySimplification.Lines on the geometry.");
            }

            bool AreClose(float v1, float v2, float thresh = 0.01f)
            {
                return (System.Math.Abs(v1 - v2) < thresh);
            }

            public void AddLine(Vector2 endPoint)
            {
                if (AreClose(endPoint.Y, (float)lineRegion.Top))
                {
                    if (figureTopEdge == null)
                    {
                        figureTopEdge = new float[2] { endPoint.X, endPoint.X };
                    }
                    else
                    {
                        figureTopEdge[0] = System.Math.Min(figureTopEdge[0], endPoint.X);
                        figureTopEdge[1] = System.Math.Max(figureTopEdge[1], endPoint.X);
                    }
                }
                else if (AreClose(endPoint.Y, (float)lineRegion.Bottom))
                {
                    if (figureBottomEdge == null)
                    {
                        figureBottomEdge = new float[2] { endPoint.X, endPoint.X };
                    }
                    else
                    {
                        figureBottomEdge[0] = System.Math.Min(figureBottomEdge[0], endPoint.X);
                        figureBottomEdge[1] = System.Math.Max(figureBottomEdge[1], endPoint.X);
                    }
                }

            }
            public void AddQuadraticBezier(Vector2 controlPoint, Vector2 endPoint)
            {
                throw new NotSupportedException("This is never called, since we use CanvasGeometrySimplification.Lines on the geometry.");
            }

            public void SetFilledRegionDetermination(CanvasFilledRegionDetermination filledRegionDetermination)
            {
                // Ignored
            }

            public void SetSegmentOptions(CanvasFigureSegmentOptions figureSegmentOptions)
            {
                // Ignored
            }

            public void EndFigure(CanvasFigureLoop figureLoop)
            {
                if (figureTopEdge == null || figureBottomEdge == null)
                    return; // The figure's area doesn't comprise a line.

                float left = System.Math.Max(figureTopEdge[0], figureBottomEdge[0]);
                float right = System.Math.Min(figureTopEdge[1], figureBottomEdge[1]);

                float width = right - left;

                if (width > 0)
                {
                    Rect rect = new Rect(left, lineRegion.Top, width, lineRegion.Height);
                    unsortedOutput.Add(rect);
                }
            }

            public List<Rect> GetRectangles()
            {
                return unsortedOutput;
            }
        }

        List<Rect> SplitGeometryIntoRectangles(CanvasGeometry geometry, float rectangleHeight, CanvasControl sender)
        {
            List<Rect> result = new List<Rect>();

            var geometryBounds = geometry.ComputeBounds();
            double left = geometryBounds.X;
            double top = geometryBounds.Y;
            double y = top;
            while (y < geometryBounds.Bottom)
            {
                var lineRegion = new Rect(left, y, geometryBounds.Width, rectangleHeight);
                var lineRegionGeometry = CanvasGeometry.CreateRectangle(sender, lineRegion);
                var compareResult = geometry.CompareWith(lineRegionGeometry);

                if (compareResult == CanvasGeometryRelation.Contains)
                {
                    // The whole line fits.
                    result.Add(lineRegion);
                }
                else if (compareResult == CanvasGeometryRelation.Disjoint)
                {
                    // The line doesn't fit, so skip it.
                }
                else if (compareResult == CanvasGeometryRelation.Overlap)
                {
                    var intersection = geometry.CombineWith(lineRegionGeometry, Matrix3x2.Identity, CanvasGeometryCombine.Intersect);

                    PathReader pathReader = new PathReader(lineRegion);
                    intersection.Simplify(CanvasGeometrySimplification.Lines).SendPathTo(pathReader);

                    var rectangles = pathReader.GetRectangles();
                    rectangles.Sort(new RectangleComparer(CurrentTextDirection == TextDirection.RightToLeft));
                    result.AddRange(rectangles);
                }

                y += rectangleHeight;

            }

            return result;
        }

        private CanvasGeometry GetLayoutGeometry(CanvasControl sender)
        {
            CanvasGeometry geometry = null;

            if (CurrentLayoutShapeOption == LayoutShapeOption.Box)
            {
                geometry = CanvasGeometry.CreateRectangle(sender, layoutBox);
            }
            else if (CurrentLayoutShapeOption == LayoutShapeOption.Ellipse)
            {
                float cx = (float)(layoutBox.Left + layoutBox.Right) / 2;
                float cy = (float)(layoutBox.Top + layoutBox.Bottom) / 2;
                float rx = cx - (float)layoutBox.Left;
                float ry = cy - (float)layoutBox.Top;
                geometry = CanvasGeometry.CreateEllipse(sender, cx, cy, rx, ry);
            }
            else if (CurrentLayoutShapeOption == LayoutShapeOption.Triangle)
            {
                CanvasPathBuilder pathBuilder = new CanvasPathBuilder(sender);
                pathBuilder.BeginFigure((float)layoutBox.Left, (float)layoutBox.Top);
                pathBuilder.AddLine((float)layoutBox.Left + (float)(layoutBox.Width), (float)layoutBox.Top);
                pathBuilder.AddLine((float)layoutBox.Left, (float)layoutBox.Top + (float)(layoutBox.Height));
                pathBuilder.EndFigure(CanvasFigureLoop.Closed);
                geometry = CanvasGeometry.CreatePath(pathBuilder);
            }
            else if (CurrentLayoutShapeOption == LayoutShapeOption.TextOutline)
            {
                CanvasTextFormat textFormat = new CanvasTextFormat();
                textFormat.FontWeight = Windows.UI.Text.FontWeights.ExtraBold;
                textFormat.WordWrapping = CanvasWordWrapping.NoWrap;
                textFormat.FontFamily = fontPicker.CurrentFontFamily;
                CanvasTextLayout textLayout = new CanvasTextLayout(sender, textLayoutBoxInput.Text, textFormat, 0, 0);

                geometry = CanvasGeometry.CreateText(textLayout);
                var bounds = geometry.ComputeBounds();
                float scaleX = (float)layoutBox.Width / (float)bounds.Width;
                float scaleY = (float)layoutBox.Height / (float)bounds.Height;
                geometry = geometry.Transform(
                    Matrix3x2.CreateTranslation(-(float)bounds.X, -(float)bounds.Y) *
                    Matrix3x2.CreateScale(scaleX, scaleY) *
                    Matrix3x2.CreateTranslation((float)layoutBox.X, (float)layoutBox.Y));
            }

            return geometry;
        }

        struct FormattingSpan
        {
            public CanvasGlyph[] Glyphs;
            public CanvasFontFace FontFace;
            public float FontSize;
            public CanvasAnalyzedScript Script;
            public int[] ClusterMap;
            public CanvasGlyphShaping[] GlyphShaping;
            public CanvasCharacterRange Range;
            public bool NeedsAdditionalJustificationCharacters;
            public uint BidiLevel;
        }

        class FormattingSpanHelper
        {
            int fontRunIndex = 0;
            int scriptRunIndex = 0;
            int bidiRunIndex = 0;

            CanvasCharacterRange characterRange;

            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasScaledFont>> fontRuns;
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>> scriptRuns;
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>> bidiRuns;

            public FormattingSpanHelper(
                IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasScaledFont>> f,
                IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>> s,
                IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>> b)
            {
                fontRuns = f;
                scriptRuns = s;
                bidiRuns = b;

                characterRange.CharacterCount = Min(fontRuns[0].Key.CharacterCount, scriptRuns[0].Key.CharacterCount, bidiRuns[0].Key.CharacterCount);
            }

            public void MoveNext()
            {
                int fontRunBoundary = GetBoundary(fontRuns[fontRunIndex].Key);
                int scriptRunBoundary = GetBoundary(scriptRuns[scriptRunIndex].Key);
                int bidiRunBoundary = GetBoundary(bidiRuns[bidiRunIndex].Key);

                int soonestBoundary = characterRange.CharacterIndex + characterRange.CharacterCount;

                if (soonestBoundary == fontRunBoundary)
                {
                    fontRunIndex++;

                    if (fontRunIndex < fontRuns.Count)
                        fontRunBoundary = GetBoundary(fontRuns[fontRunIndex].Key);
                }

                if (soonestBoundary == scriptRunBoundary)
                {
                    scriptRunIndex++;

                    if (scriptRunIndex < scriptRuns.Count)
                        scriptRunBoundary = GetBoundary(scriptRuns[scriptRunIndex].Key);
                }

                if (soonestBoundary == bidiRunBoundary)
                {
                    bidiRunIndex++;

                    if (bidiRunIndex < bidiRuns.Count)
                        bidiRunBoundary = GetBoundary(bidiRuns[bidiRunIndex].Key);
                }

                int nextBoundary = Min(fontRunBoundary, scriptRunBoundary, bidiRunBoundary);

                characterRange.CharacterIndex += characterRange.CharacterCount;
                characterRange.CharacterCount = nextBoundary - characterRange.CharacterIndex;
            }

            public bool IsDone()
            {
                return !(fontRunIndex < fontRuns.Count &&
                    scriptRunIndex < scriptRuns.Count &&
                    bidiRunIndex < bidiRuns.Count);
            }

            public CanvasScaledFont ScaledFont { get { return fontRuns[fontRunIndex].Value; } }
            public CanvasAnalyzedScript Script { get { return scriptRuns[scriptRunIndex].Value; } }
            public CanvasAnalyzedBidi Bidi { get { return bidiRuns[bidiRunIndex].Value; } }

            public CanvasCharacterRange Range { get { return characterRange; } }
            

            int GetBoundary(CanvasCharacterRange range)
            {
                return range.CharacterIndex + range.CharacterCount;
            }

            int Min(int a, int b, int c)
            {
                return Math.Min(Math.Min(a, b), c);
            }
            
        }

        List<FormattingSpan> EvaluateFormattingSpans(
            CanvasTextAnalyzer textAnalyzer,
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasScaledFont>> fontRuns,
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>> scriptRuns,
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>> bidiRuns,
            out float maxLineSpacing)
        {
            maxLineSpacing = 0;

            //
            // Divide up our text space into spans of uniform font face, script and bidi.
            //

            List<FormattingSpan> formattingSpans = new List<FormattingSpan>();

            FormattingSpanHelper formattingSpanHelper = new FormattingSpanHelper(fontRuns, scriptRuns, bidiRuns);

            while (!formattingSpanHelper.IsDone())
            {
                var scriptProperties = textAnalyzer.GetScriptProperties(formattingSpanHelper.Script);

                float fontSize = desiredFontSize * formattingSpanHelper.ScaledFont.ScaleFactor;

                FormattingSpan formattingSpan = new FormattingSpan();
                
                int[] clusterMap;
                bool[] isShapedAlone;
                CanvasGlyphShaping[] glyphShaping;
                
                // Evaluate which glyphs comprise the text.
                formattingSpan.Glyphs = textAnalyzer.GetGlyphs(
                    formattingSpanHelper.Range,
                    formattingSpanHelper.ScaledFont.FontFace,
                    fontSize,
                    false, // isSideways
                    formattingSpanHelper.Bidi.ResolvedLevel % 2 == 1,
                    formattingSpanHelper.Script,
                    "",
                    null,
                    null,
                    out clusterMap,
                    out isShapedAlone,
                    out glyphShaping);

                formattingSpan.FontFace = formattingSpanHelper.ScaledFont.FontFace;
                formattingSpan.FontSize = fontSize;
                formattingSpan.Script = formattingSpanHelper.Script;
                formattingSpan.ClusterMap = clusterMap;
                formattingSpan.GlyphShaping = glyphShaping;
                formattingSpan.Range = formattingSpanHelper.Range;
                formattingSpan.BidiLevel = formattingSpanHelper.Bidi.ResolvedLevel;
                formattingSpan.NeedsAdditionalJustificationCharacters = scriptProperties.JustificationCharacter != null;

                formattingSpans.Add(formattingSpan);

                //
                // For text which contains non-uniform font faces, CanvasTextLayout takes the maximum of
                // all of line spacings and applies it as the overall line spacing. We do the same thing, here.
                //
                maxLineSpacing = System.Math.Max(maxLineSpacing, GetLineSpacing(formattingSpan.FontFace, fontSize));

                formattingSpanHelper.MoveNext();
            }

            return formattingSpans;
        }

        class GlyphRun
        {
            public FormattingSpan FormattingSpan;
            public List<CanvasGlyph> Glyphs;

            private int firstGlyphIndex; // Indices within the formatting span.
            private int lastGlyphIndex;
            
            public CanvasCharacterRange GetRange()
            {
                int formattingSpanStartIndex = FormattingSpan.Range.CharacterIndex;
                int start = GetCharacterIndex(firstGlyphIndex, FormattingSpan.ClusterMap);
                int end = GetCharacterIndex(lastGlyphIndex, FormattingSpan.ClusterMap);
                int length = end - start + 1;

                return new CanvasCharacterRange {
                    CharacterIndex = formattingSpanStartIndex + start,
                    CharacterCount = length };
            }

            public void AddGlyph(int glyphIndex)
            {
                if (Glyphs.Count == 0)
                {
                    firstGlyphIndex = glyphIndex;
                }
                lastGlyphIndex = glyphIndex;

                Glyphs.Add(FormattingSpan.Glyphs[glyphIndex]);
            }

            public int[] GetClusterMap(CanvasCharacterRange range)
            {
                //
                // Create a cluster map for this character range. Because the cluster map
                // should reflect only the text positions in the range, we need to re-normalize
                // it (so that it starts at 0).
                //
                int[] clusterMap = new int[range.CharacterCount];

                int formattingSpanStartIndex = FormattingSpan.Range.CharacterIndex;

                int firstClusterMapValue = FormattingSpan.ClusterMap[range.CharacterIndex - formattingSpanStartIndex];

                for (int i=0; i < range.CharacterCount; ++i)
                {                    
                    int indexWithinFormattingSpan = range.CharacterIndex - formattingSpanStartIndex + i; // Cluster maps are per formatting span.

                    clusterMap[i] = FormattingSpan.ClusterMap[indexWithinFormattingSpan] - firstClusterMapValue;
                }
                return clusterMap;
            }

            public CanvasGlyphShaping[] GetShaping()
            {
                //
                // The shaping array is in terms of glyph indices. The formatting span contains all the shaping info for this glyph run.
                //
                CanvasGlyphShaping[] shaping = new CanvasGlyphShaping[Glyphs.Count];
                                
                for (int i = 0; i < Glyphs.Count; ++i)
                {
                    shaping[i] = FormattingSpan.GlyphShaping[firstGlyphIndex + i];
                }
                return shaping;
            }

            public float GetAdvance()
            {
                float advance = 0;
                foreach (var g in Glyphs)
                {
                    advance += g.Advance;
                }
                return advance;
            }
        }

        class LayoutBox
        {
            List<GlyphRun> glyphRuns;
            Rect rectangle;
            uint highestBidiLevel = 0;
            uint lowestOddBidiLevel = uint.MaxValue;

            public LayoutBox(Rect r)
            {
                glyphRuns = new List<GlyphRun>();
                rectangle = r;
            }

            public void AddGlyphRun(GlyphRun run)
            {
                glyphRuns.Add(run);

                uint bidiLevel = run.FormattingSpan.BidiLevel;
                highestBidiLevel = Math.Max(highestBidiLevel, bidiLevel);

                if (bidiLevel % 2 == 1)
                    lowestOddBidiLevel = Math.Min(lowestOddBidiLevel, run.FormattingSpan.BidiLevel);
            }

            public List<GlyphRun> GlyphRuns { get { return glyphRuns; } }
            public Rect Rectangle { get { return rectangle; } }

            public int GetGlyphCount()
            {
                int count = 0;
                foreach (var g in glyphRuns)
                {
                    count += g.Glyphs.Count;
                }
                return count;
            }

            public int[] ProduceBidiOrdering()
            {
                int spanStart = 0;

                int spanCount = GlyphRuns.Count;

                //
                // Produces an index mapping from sequential order to visual bidi order.
                // The function progresses forward, checking the bidi level of each
                // pair of spans, reversing when needed.
                //
                // See the Unicode technical report 9 for an explanation.
                // http://www.unicode.org/reports/tr9/tr9-17.html 
                //
                int[] spanIndices = new int[spanCount];

                for (int i = 0; i < spanCount; ++i)
                    spanIndices[i] = spanStart + i;

                if (spanCount <= 1)
                    return spanIndices;

                int runStart = 0;
                uint currentLevel = glyphRuns[spanStart].FormattingSpan.BidiLevel;

                // Rearrange each run to produced reordered spans.
                for (int i=0; i< spanCount; ++i)
                {
                    int runEnd = i + 1;
                    uint nextLevel = (runEnd < spanCount) ? glyphRuns[spanIndices[runEnd]].FormattingSpan.BidiLevel : 0;

                    if (currentLevel <= nextLevel)
                    {
                        if (currentLevel < nextLevel)
                        {
                            currentLevel = nextLevel;
                            runStart = i + 1;
                        }
                        continue; // Skip past equal levels, or increasing stairsteps.
                    }

                    do
                    {
                        // Recede to find start of the run and previous level.
                        uint previousLevel;
                        for (;;)
                        {
                            if (runStart <= 0)
                            {
                                previousLevel = 0; // position before string has bidi level of 0
                                break;
                            }
                            if (glyphRuns[spanIndices[--runStart]].FormattingSpan.BidiLevel < currentLevel)
                            {
                                previousLevel = glyphRuns[spanIndices[runStart]].FormattingSpan.BidiLevel;
                                ++runStart; // compensate for going one element past
                                break;
                            }
                        }

                        // Reverse the indices, if the difference between the current and
                        // next/previous levels is odd. Otherwise, it would be a no-op, so
                        // don't bother.
                        if (Math.Min(currentLevel - nextLevel, currentLevel - previousLevel) % 2 != 0)
                        {
                            ReverseArrayElements(spanIndices, runStart, runEnd);
                        }

                        // Descend to the next lower level, the greater of previous and next
                        currentLevel = Math.Max(previousLevel, nextLevel);
                    } while (currentLevel > nextLevel);
                }

                return spanIndices;
            }

            void ReverseArrayElements(int[] indices, int start, int end)
            {
                int length = end - start;
                for (int i=0; i<length / 2; i++)
                {
                    int temp = indices[start + i];
                    indices[start + i] = indices[end - i - 1];
                    indices[end - i - 1] = temp;
                }
            }   
        }

        // This method returns the current glyph run.
        GlyphRun BeginGlyphRun(Rect rectangle, float advance, LayoutBox currentLayoutBox, FormattingSpan formattingSpan, int lineNumber)
        {
            GlyphRun glyphRun = new GlyphRun();

            glyphRun.FormattingSpan = formattingSpan;
            
            glyphRun.Glyphs = new List<CanvasGlyph>();
            currentLayoutBox.AddGlyphRun(glyphRun);

            return glyphRun;
        }

        //
        // Returns the current glyph run, or null if there's no more layout boxes.
        //
        GlyphRun BeginNewLayoutBox(ref int rectangleIndex, List<Rect> rectangles, ref float glyphRunAdvance, ref int wordsPerLine, FormattingSpan formattingSpan, List<LayoutBox> layoutBoxes)
        {
            rectangleIndex++;
            if (rectangleIndex >= rectangles.Count)
                return null;

            LayoutBox layoutBox = new LayoutBox(rectangles[rectangleIndex]);
            layoutBoxes.Add(layoutBox);

            glyphRunAdvance = 0;
            wordsPerLine = 0;

            GlyphRun newGlyphRun = BeginGlyphRun(rectangles[rectangleIndex], glyphRunAdvance, layoutBox, formattingSpan, rectangleIndex);

            return newGlyphRun;
        }

        static int GetCharacterIndex(int glyphIndex, int[] clusterMap)
        {
            int correspondingTextPosition = 0;
            for (int k = 0; k < clusterMap.Length; ++k)
            {
                if (clusterMap[k] == glyphIndex)
                    correspondingTextPosition = k;
            }
            return correspondingTextPosition;
        }

        List<LayoutBox> CreateGlyphRuns(List<Rect> rectangles, List<FormattingSpan> formattingSpans, CanvasAnalyzedBreakpoint[] analyzedBreakpoints)
        {
            List<LayoutBox> layoutBoxes = new List<LayoutBox>();

            if (rectangles.Count == 0) return layoutBoxes;

            int rectangleIndex = -1;
            float glyphRunAdvance = 0;
            int wordsPerLine = 0;            

            for (int formattingSpanIndex = 0; formattingSpanIndex < formattingSpans.Count; formattingSpanIndex++)
            {
                var formattingSpan = formattingSpans[formattingSpanIndex];

                GlyphRun currentGlyphRun;
                if (layoutBoxes.Count == 0)
                    currentGlyphRun = BeginNewLayoutBox(ref rectangleIndex, rectangles, ref glyphRunAdvance, ref wordsPerLine, formattingSpan, layoutBoxes);
                else
                    currentGlyphRun = BeginGlyphRun(rectangles[rectangleIndex], glyphRunAdvance, layoutBoxes[layoutBoxes.Count - 1], formattingSpan, rectangleIndex);

                if (currentGlyphRun == null)
                    return layoutBoxes;

                var glyphs = formattingSpan.Glyphs;

                for (int i = 0; i < glyphs.Length; ++i)
                {
                    //
                    // Will the next word fit in the box?
                    //
                    float wordAdvance = 0.0f;
                    int wordBoundary;
                    for (wordBoundary = i; wordBoundary < glyphs.Length; wordBoundary++)
                    {
                        int correspondingTextPosition = formattingSpan.Range.CharacterIndex + GetCharacterIndex(wordBoundary, formattingSpan.ClusterMap);

                        var afterThisCharacter = analyzedBreakpoints[correspondingTextPosition].BreakAfter;
                        var beforeNextCharacter = (correspondingTextPosition < analyzedBreakpoints.Length - 1) ? analyzedBreakpoints[correspondingTextPosition + 1].BreakBefore : CanvasLineBreakCondition.Neutral;
                        
                        // 
                        // The text for this demo doesn't have any hard line breaks.
                        //
                        System.Diagnostics.Debug.Assert(afterThisCharacter != CanvasLineBreakCondition.MustBreak);

                        if (afterThisCharacter == CanvasLineBreakCondition.CanBreak && beforeNextCharacter != CanvasLineBreakCondition.CannotBreak)
                            break;

                        wordAdvance += glyphs[wordBoundary].Advance;
                    }

                    if (glyphRunAdvance + wordAdvance < rectangles[rectangleIndex].Width) // It fits
                    {
                        for (int j = i; j <= wordBoundary; j++)
                        {
                            if (j < glyphs.Length)
                            {
                                currentGlyphRun.AddGlyph(j);

                                glyphRunAdvance += glyphs[j].Advance;
                            }
                        }
                        i = wordBoundary;
                        wordsPerLine++;
                    }
                    else // Doesn't fit
                    {
                        if (wordsPerLine == 0) // Need an emegency break?
                        {
                            int breakBoundary = i;
                            while (breakBoundary < glyphs.Length && glyphRunAdvance + glyphs[breakBoundary].Advance < rectangles[rectangleIndex].Width)
                            {
                                currentGlyphRun.AddGlyph(breakBoundary);

                                glyphRunAdvance += glyphs[breakBoundary].Advance;

                                breakBoundary++;
                            }
                            i = breakBoundary - 1;
                        }
                        else
                        {
                            i--; // Retry the glyph against the next rectangle.
                        }

                        currentGlyphRun = BeginNewLayoutBox(ref rectangleIndex, rectangles, ref glyphRunAdvance, ref wordsPerLine, formattingSpan, layoutBoxes);

                        if (currentGlyphRun == null)
                            return layoutBoxes;
                    }
                }
            }

            return layoutBoxes;
        }

        CanvasJustificationOpportunity[] GetJustificationOpportunities(CanvasTextAnalyzer textAnalyzer, LayoutBox layoutBox, out CanvasGlyph[] allGlyphs)
        {
            int layoutBoxGlyphCount = layoutBox.GetGlyphCount();

            CanvasJustificationOpportunity[] justificationOpportunities = new CanvasJustificationOpportunity[layoutBoxGlyphCount];
            allGlyphs = new CanvasGlyph[layoutBoxGlyphCount];

            int glyphIndex = 0;

            for (int i=0; i<layoutBox.GlyphRuns.Count; i++)
            {
                if (layoutBox.GlyphRuns[i].Glyphs.Count == 0)
                    continue;

                CanvasCharacterRange range = layoutBox.GlyphRuns[i].GetRange();

                var glyphRunClusterMap = layoutBox.GlyphRuns[i].GetClusterMap(range);
                var glyphRunShaping = layoutBox.GlyphRuns[i].GetShaping();

                var justificationOpportunitiesThisGlyphRun = textAnalyzer.GetJustificationOpportunities(
                    range,
                    layoutBox.GlyphRuns[i].FormattingSpan.FontFace,
                    layoutBox.GlyphRuns[i].FormattingSpan.FontSize,
                    layoutBox.GlyphRuns[i].FormattingSpan.Script,
                    glyphRunClusterMap,
                    glyphRunShaping);

                for (int j = 0; j < layoutBox.GlyphRuns[i].Glyphs.Count; ++j)
                {
                    justificationOpportunities[glyphIndex + j] = justificationOpportunitiesThisGlyphRun[j];
                    allGlyphs[glyphIndex + j] = layoutBox.GlyphRuns[i].Glyphs[j];
                }
                glyphIndex += layoutBox.GlyphRuns[i].Glyphs.Count;
            }

            return justificationOpportunities;
        }

        void SplitJustifiedGlyphsIntoRuns(CanvasTextAnalyzer textAnalyzer, LayoutBox layoutBox, CanvasGlyph[] justifiedGlyphs, bool needsAdditionalJustificationCharacters)
        {
            int glyphIndex = 0;

            float xPosition = (float)layoutBox.Rectangle.Right;
            for (int i=0; i<layoutBox.GlyphRuns.Count; i++)
            {
                if (layoutBox.GlyphRuns[i].Glyphs.Count == 0)
                    continue;
                
                int originalGlyphCountForThisRun = layoutBox.GlyphRuns[i].Glyphs.Count;

                if (needsAdditionalJustificationCharacters)
                {
                    // Replace the glyph data, since justification can modify glyphs                
                    CanvasGlyph[] justifiedGlyphsForThisGlyphRun = new CanvasGlyph[layoutBox.GlyphRuns[i].Glyphs.Count];
                    for (int j = 0; j < layoutBox.GlyphRuns[i].Glyphs.Count; j++)
                    {
                        justifiedGlyphsForThisGlyphRun[j] = justifiedGlyphs[glyphIndex + j];
                    }

                    CanvasCharacterRange range = layoutBox.GlyphRuns[i].GetRange();

                    var glyphRunClusterMap = layoutBox.GlyphRuns[i].GetClusterMap(range);
                    var glyphRunShaping = layoutBox.GlyphRuns[i].GetShaping();
                    
                    CanvasGlyph[] newSetOfGlyphs = textAnalyzer.AddGlyphsAfterJustification(
                        layoutBox.GlyphRuns[i].FormattingSpan.FontFace,
                        layoutBox.GlyphRuns[i].FormattingSpan.FontSize,
                        layoutBox.GlyphRuns[i].FormattingSpan.Script,
                        glyphRunClusterMap,
                        layoutBox.GlyphRuns[i].Glyphs.ToArray(),
                        justifiedGlyphsForThisGlyphRun,
                        glyphRunShaping);

                    layoutBox.GlyphRuns[i].Glyphs = new List<CanvasGlyph>(newSetOfGlyphs);
                }
                else
                {
                    for (int j = 0; j < layoutBox.GlyphRuns[i].Glyphs.Count; j++)
                    {
                        layoutBox.GlyphRuns[i].Glyphs[j] = justifiedGlyphs[glyphIndex + j];
                    }
                }

                glyphIndex += originalGlyphCountForThisRun;
            }
        }

        void JustifyLine(CanvasTextAnalyzer textAnalyzer, LayoutBox layoutBox)
        {
            CanvasGlyph[] allGlyphs;
            var justificationOpportunities = GetJustificationOpportunities(textAnalyzer, layoutBox, out allGlyphs);

            CanvasGlyph[] justifiedGlyphs = textAnalyzer.ApplyJustificationOpportunities(
                (float)layoutBox.Rectangle.Width,
                justificationOpportunities,
                allGlyphs);

            bool needsJustificationCharacters = layoutBox.GlyphRuns[0].FormattingSpan.NeedsAdditionalJustificationCharacters;

            SplitJustifiedGlyphsIntoRuns(textAnalyzer, layoutBox, justifiedGlyphs, needsJustificationCharacters);
        }

        void Justify(CanvasTextAnalyzer textAnalyzer, List<LayoutBox> layoutBoxes)
        {
            if (layoutBoxes.Count == 0)
                return;            

            for (int i=0; i<layoutBoxes.Count; i++)
            {
                if (layoutBoxes[i].GlyphRuns.Count == 0)
                    return;

                JustifyLine(textAnalyzer, layoutBoxes[i]);
            }            
        }

        private void EnsureLayout(CanvasControl sender)
        {
            if (!needsLayout)
                return;

            var geometry = GetLayoutGeometry(sender);

            string text;
            if (CurrentTextOption == TextOption.English)
            {
                text =
                    "Win2D is an easy-to-use Windows Runtime API for immediate mode 2D graphics rendering with GPU acceleration. " +
                    "It is available to C# and C++ developers writing Windows apps for Windows 8.1, Windows Phone 8.1 and Windows " +
                    "10. It utilizes the power of Direct2D, and integrates seamlessly with XAML and CoreWindow. The CanvasTextAnalyzer " +
                    "class identifies what glyphs are needed to render a piece of text, including the font fallbacks to properly " +
                    "handle different languages.  Example Gallery uses it to create this Custom Text Layouts example, which wordwraps text " +
                    "to fill an arbitrary geometric shape.";
            }
            else
            {
                //
                // Storing bi-directional text as a contiguous literal causes problems in VS; it will attempt to reorder the bi-di levels 
                // in the script, and it reorders them wrongly (doesn't match Word, Outlook, other programs). This works around it
                // by separating some of them out.
                //
                // This text string contains control characters around 'C#' and C++' to ensure they are drawn with the correct
                // directionality. Higher-level editors like Word use this same mechanism, since a full, correct Bidi ordering 
                // isn't sufficient for them.
                //
                text =
                    "Win2D هو API  لنظام التشغيل ويندوز سهل الاستخدام لتقديم الرسومات الثنائية الابعاد " + "(2D)" +
                    "مع تسارع المعالج الجرافيك. متاح للمطورين \u202aC#\u202c و \u202aC+ +\u202c لتطوير تطبيقات الويندوز لإصدارات " +
                    "8.1،   10 و هاتف الويندوز إصدار 8.1. فإنه يستخدم قوة Direct2D، ويدمج بسهولة مع XAML وCoreWindows ." +
                    "الفئة CanvasTextAnalyzer يحدد ما هي الرموز المطلوبة لتقديم قطعة من " +
                    "النص، بما في ذلك أساسات الخط إلى التعامل مع لغات مختلفة. Example Gallery يستخدم هذه الطقنيه لعرض كيفية التعامل مع النصوص.";
            }

            // Duplicate the text, so that it fills more reading space.
            text = text + text;

            var textFormat = new CanvasTextFormat();
            textFormat.FontFamily = fontPicker.CurrentFontFamily;

            var textDirection = CurrentTextDirection == TextDirection.LeftToRight ?
                CanvasTextDirection.LeftToRightThenTopToBottom : CanvasTextDirection.RightToLeftThenTopToBottom;

            CanvasTextAnalyzer textAnalyzer = new CanvasTextAnalyzer(text, textDirection);

            //
            // Figure out what fonts to use.
            //
            var fontResult = textAnalyzer.GetFonts(textFormat);

            //
            // Perform a script analysis on the text.
            //
            var scriptAnalysis = textAnalyzer.GetScript();

            //
            // Perform bidi analysis.
            //
            var bidiAnalysis = textAnalyzer.GetBidi();

            float maxLineSpacing = 0;
            List<FormattingSpan> formattingSpans = EvaluateFormattingSpans(textAnalyzer, fontResult, scriptAnalysis, bidiAnalysis, out maxLineSpacing);
            
            //
            // Perform line break analysis.
            //
            var breakpoints = textAnalyzer.GetBreakpoints();

            //
            // Get the rectangles to layout text into.
            //
            layoutRectangles = SplitGeometryIntoRectangles(geometry, maxLineSpacing, sender);
            
            //
            // Insert glyph runs into the layout boxes.
            //
            layoutBoxes = CreateGlyphRuns(layoutRectangles, formattingSpans, breakpoints);

            if (ShouldJustify)
            {
                Justify(textAnalyzer, layoutBoxes);
            }

            needsLayout = false;

        }

        private static float GetLineSpacing(CanvasFontFace fontFace, float fontSize)
        {
            return (fontFace.LineGap + fontFace.Ascent + fontFace.Descent) * fontSize;
        }

        CanvasStrokeStyle dashedStroke = new CanvasStrokeStyle() { DashStyle = CanvasDashStyle.Dash };

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureLayout(sender);

            if (ShowLayoutRectangles)
            {
                foreach (var layoutRectangle in layoutRectangles)
                {
                    args.DrawingSession.DrawRectangle(layoutRectangle, Colors.Gray);
                }
            }

            if (ShowSelectionBox)
            {
                args.DrawingSession.DrawRectangle(layoutBox, Colors.LightGray, 2.0f, dashedStroke);
            }

            var brush = new CanvasSolidColorBrush(sender, Colors.LightSkyBlue);

            foreach (LayoutBox l in layoutBoxes)
            {
                if (l.GlyphRuns.Count > 0)
                {
                    float layoutAdvance = 0;
                    foreach (var g in l.GlyphRuns)
                    {
                        layoutAdvance += g.GetAdvance();
                    }

                    float x = (float)l.Rectangle.Left;


                    if (CurrentTextDirection == TextDirection.RightToLeft)
                        x = (float)l.Rectangle.Right - layoutAdvance;

                    int[] bidiOrdering = l.ProduceBidiOrdering();

                    foreach (int glyphRunIndex in bidiOrdering)
                    {
                        GlyphRun g = l.GlyphRuns[glyphRunIndex];

                        //
                        // The Arabic test string contains control characters. A typical text renderer will just not draw these.
                        //
                        if (g.FormattingSpan.Script.Shape == CanvasScriptShape.NoVisual)
                            continue;

                        if (g.Glyphs.Count > 0)
                        {
                            float advance = g.GetAdvance();

                            Vector2 position;
                            position.X = x;
                            position.Y = (float)l.Rectangle.Bottom;

                            if (g.FormattingSpan.BidiLevel % 2 != 0)
                                position.X += advance;

                            args.DrawingSession.DrawGlyphRun(
                                position,
                                g.FormattingSpan.FontFace,
                                g.FormattingSpan.FontSize,
                                g.Glyphs.ToArray(),
                                false, // isSideways
                                g.FormattingSpan.BidiLevel,
                                brush);

                            x += advance;
                        }
                    }
                }
            }
        }

        private void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            needsLayout = true;
        }

        private void Canvas_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            pointerPressPosition = e.GetCurrentPoint(canvas).Position;
            canvas.Invalidate();
            e.Handled = true;
        }

        private void Canvas_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (e.Pointer.IsInContact)
            {
                layoutBox = new Rect(pointerPressPosition, e.GetCurrentPoint(canvas).Position);
                needsLayout = true;

                canvas.Invalidate();
                e.Handled = true;
            }
        }

        private void LayoutShapeOptions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            needsLayout = true;
            canvas.Invalidate();

            if (CurrentLayoutShapeOption == LayoutShapeOption.TextOutline)
            {
                textOutlineInputLabel.Visibility = Visibility.Visible;
                textLayoutBoxInput.Visibility = Visibility.Visible;
            }
            else
            {
                textOutlineInputLabel.Visibility = Visibility.Collapsed;
                textLayoutBoxInput.Visibility = Visibility.Collapsed;
            }
        }

        private void textLayoutBoxInput_TextChanged(object sender, TextChangedEventArgs e)
        {
            needsLayout = true;
            canvas.Invalidate();
        }

        private void SelectionChanged_InvalidateLayout(object sender, SelectionChangedEventArgs e)
        {
            needsLayout = true;
            canvas.Invalidate();
        }        

        private void InvalidateCanvas(object sender, RoutedEventArgs e)
        {
            canvas.Invalidate();
        }

        private void canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            layoutBox.X = (float)canvas.Size.Width / 8;
            layoutBox.Width = (float)canvas.Size.Width * 6 / 8;

            layoutBox.Y = (float)canvas.Size.Height / 8;
            layoutBox.Height = (float)canvas.Size.Height * 6 / 8;

            needsLayout = true;
        }

        private void Justify_ValueChanged(object sender, RoutedEventArgs e)
        {
            needsLayout = true;
            canvas.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
