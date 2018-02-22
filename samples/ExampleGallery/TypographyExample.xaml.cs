// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class TypographyExample : UserControl, INotifyPropertyChanged
    {           
        CanvasTextLayout textLayout;

        bool needsResourceRecreation;
        Size resourceRealizationSize;

        bool typographyChanged;
        bool needRefreshTypographyList;
        
        static CanvasDrawingSession currentDrawingSession;

        string testString;
        public List<TypographyFeatureInfo> TypographyOptions { get; set; }
        public TypographyFeatureInfo CurrentTypographyOption { get; set; }
        public bool HighlightTypographyAffectedGlyphs { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        class TypographyHandler : ICanvasTextRenderer
        {
            IReadOnlyList<KeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>> analyzedScript;

            public List<TypographyFeatureInfo> TypographyOptions;
            public Mode CurrentMode;
            public CanvasTypographyFeatureName FeatureToHighlight;

            public enum Mode { BuildTypographyList, Highlight }

            public TypographyHandler(string text)
            {
                var textAnalyzer = new CanvasTextAnalyzer(text, CanvasTextDirection.TopToBottomThenLeftToRight);
                analyzedScript = textAnalyzer.GetScript();

                TypographyOptions = new List<TypographyFeatureInfo>();
                TypographyOptions.Add(new TypographyFeatureInfo(CanvasTypographyFeatureName.None));

                CurrentMode = Mode.BuildTypographyList;
                FeatureToHighlight = CanvasTypographyFeatureName.None;
            }

            private CanvasAnalyzedScript GetScript(uint textPosition)
            {
                foreach (KeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript> range in analyzedScript)
                {
                    if (textPosition >= range.Key.CharacterIndex && textPosition < range.Key.CharacterIndex + range.Key.CharacterCount)
                    {
                        return range.Value;
                    }
                }

                System.Diagnostics.Debug.Assert(false);
                return analyzedScript[analyzedScript.Count - 1].Value;
            }

            public void DrawGlyphRun(
                Vector2 position,
                CanvasFontFace fontFace,
                float fontSize,
                CanvasGlyph[] glyphs,
                bool isSideways,
                uint bidiLevel,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                string textString,
                int[] clusterMapIndices,
                uint textPosition,
                CanvasGlyphOrientation glyphOrientation)
            {
                var script = GetScript(textPosition);

                if (CurrentMode == Mode.BuildTypographyList)
                {
                    CanvasTypographyFeatureName[] features = fontFace.GetSupportedTypographicFeatureNames(script);

                    foreach (var featureName in features)
                    {
                        TypographyFeatureInfo featureInfo = new TypographyFeatureInfo(featureName);

                        if (!TypographyOptions.Contains(featureInfo))
                        {
                            TypographyOptions.Add(featureInfo);
                        }
                    }
                }
                else
                {
                    if (glyphs == null || glyphs.Length == 0)
                        return;
                    //
                    // This demo handles only simple Latin text with no diacritical
                    // markers or ligatures, so we can make assumptions about the 
                    // mapping of text positions to glyph indices. This works fine for
                    // the sake of this example.
                    //
                    // In general, apps should use the cluster map to map text 
                    // positions to glyphs while knowing that glyph substitution can happen 
                    // for reasons besides typography.
                    //
                    uint[] codePoints = new uint[glyphs.Length];
                    for (int i = 0; i < glyphs.Length; i++)
                    {
                        int glyphTextPosition = 0;
                        for (int j=0; j<clusterMapIndices.Length; j++)
                        {
                            if (clusterMapIndices[j] == i)
                            {
                                glyphTextPosition = j;
                                break;
                            }
                        }
                        codePoints[i] = textString[glyphTextPosition];
                    }
                    int[] nominalGlyphIndices = fontFace.GetGlyphIndices(codePoints);

                    CanvasGlyph[] unsubstitutedGlyphs = new CanvasGlyph[glyphs.Length];
                    for (int i = 0; i < glyphs.Length; i++)
                    {
                        unsubstitutedGlyphs[i] = glyphs[i];
                        unsubstitutedGlyphs[i].Index = nominalGlyphIndices[i];
                    }

                    bool[] eligible = fontFace.GetTypographicFeatureGlyphSupport(script, FeatureToHighlight, unsubstitutedGlyphs);

                    var highlightBrush = new CanvasSolidColorBrush(currentDrawingSession, Colors.Yellow);

                    for (int i = 0; i < glyphs.Length; ++i)
                    {
                        if (eligible[i])
                        {
                            CanvasGlyph[] singleGlyph = new CanvasGlyph[1];
                            singleGlyph[0] = glyphs[i];

                            currentDrawingSession.DrawGlyphRun(
                                position,
                                fontFace,
                                fontSize,
                                singleGlyph,
                                isSideways,
                                bidiLevel,
                                highlightBrush);
                        }

                        position.X += glyphs[i].Advance;
                    }
                }
            }

            public void DrawStrikethrough(
                Vector2 position,
                float strikethroughWidth,
                float strikethroughThickness,
                float strikethroughOffset,
                CanvasTextDirection textDirection,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                CanvasGlyphOrientation glyphOrientation)
            {
            }

            public void DrawUnderline(
                Vector2 position,
                float underlineWidth,
                float underlineThickness,
                float underlineOffset,
                float runHeight,
                CanvasTextDirection textDirection,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                CanvasGlyphOrientation glyphOrientation)
            {
            }

            public void DrawInlineObject(
                Vector2 baselineOrigin,
                ICanvasTextInlineObject inlineObject,
                bool isSideways,
                bool isRightToLeft,
                object brush,
                CanvasGlyphOrientation glyphOrientation)
            {
            }

            public float Dpi { get { return 96; } }
            public bool PixelSnappingDisabled { get { return false; } }
            public Matrix3x2 Transform { get { return System.Numerics.Matrix3x2.Identity; } }
        }

        public class TypographyFeatureInfo
        {
            private CanvasTypographyFeatureName name;

            private string stringName;

            public TypographyFeatureInfo(CanvasTypographyFeatureName n)
            {
                name = n;

                if (IsNamedFeature(name))
                {
                    stringName = name.ToString();
                }
                else
                {
                    //
                    // For custom font features, we can produce the OpenType feature tag
                    // using the feature name.
                    //
                    uint id = (uint)(name);
                    stringName = 
                        ((char)((id >> 24) & 0xFF)).ToString() +
                        ((char)((id >> 16) & 0xFF)).ToString() +
                        ((char)((id >> 8) & 0xFF)).ToString() +
                        ((char)((id >> 0) & 0xFF)).ToString() + " (Custom)";
                }
            }

            public CanvasTypographyFeatureName Name { get {return name; } }

            public override string ToString()
            {
                return stringName;
            }

            public override bool Equals(object obj)
            {
                var other = obj as TypographyFeatureInfo;
                if (other != null)
                    return name == other.Name;
                else
                    return false;
            }

            public override int GetHashCode()
            {
                return base.GetHashCode();
            }
            
            bool IsNamedFeature(CanvasTypographyFeatureName name)
            {
                //
                // DWrite and Win2D support a pre-defined list of typographic features.
                // However, fonts are free to expose features outside of that list.
                // In fact, many built-in fonts have such custom features. 
                // 
                // These custom features are also accessible through Win2D, and 
                // are reported by GetSupportedTypographicFeatureNames.
                //
                var allValues = Enum.GetValues(typeof(CanvasTypographyFeatureName));
                foreach (var value in allValues)
                {
                    if ((CanvasTypographyFeatureName)(value) == name)
                        return true;
                }

                return false;
            }
        }

        public TypographyExample()
        {
            DataContext = this;

            this.InitializeComponent();    

            TypographyOptions = new List<TypographyFeatureInfo>();

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                fontPicker.SelectFont("Gabriola");
                CurrentTypographyOption = new TypographyFeatureInfo(CanvasTypographyFeatureName.StylisticSet2);
            }
            else
            {
                CurrentTypographyOption = new TypographyFeatureInfo(CanvasTypographyFeatureName.Kerning);
            }
            HighlightTypographyAffectedGlyphs = true;
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (resourceRealizationSize == targetSize && !needsResourceRecreation)
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;

            if (textLayout != null) 
                textLayout.Dispose();
            textLayout = CreateTextLayout(resourceCreator, canvasWidth, canvasHeight);
            
            typographyChanged = true;
            needRefreshTypographyList = true;

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            float sizeDim = Math.Min(canvasWidth, canvasHeight);
            
            float fontSize;
            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                testString = "Win2D supports the use of typography options with text. Typography affects how glyphs are positioned, how they appear, and may at times substitute glyphs with other glyphs. Typography features can be queried for font compatibility, or applied to any part of a text layout.";
                fontSize = sizeDim * 0.075f;
            }
            else
            {
                testString = "Typography";
                fontSize = sizeDim * 0.1f;
            }

            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontSize = fontSize,
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                VerticalAlignment = CanvasVerticalAlignment.Center
            };

            textFormat.FontFamily = fontPicker.CurrentFontFamily;

            return textLayout = new CanvasTextLayout(resourceCreator, testString, textFormat, canvasWidth, canvasHeight);
        }

        private void EnsureTypographyList(TypographyHandler typographyHandler)
        {
            if (!needRefreshTypographyList)
                return;

            textLayout.DrawToTextRenderer(typographyHandler, 0, 0);

            TypographyOptions = typographyHandler.TypographyOptions;

            TypographyFeatureInfo previousOption = CurrentTypographyOption;

            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("TypographyOptions"));

            if (TypographyOptions.Contains(previousOption))
                CurrentTypographyOption = previousOption;
            else
                CurrentTypographyOption = new TypographyFeatureInfo(CanvasTypographyFeatureName.None);

            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("CurrentTypographyOption"));

            needRefreshTypographyList = false;
        }

        void EnsureTypography()
        {
            if (!typographyChanged)
                return;

            CanvasTypography typography = null;

            if (CurrentTypographyOption.Name != CanvasTypographyFeatureName.None)
            {
                typography = new CanvasTypography();
                typography.AddFeature(CurrentTypographyOption.Name, 1u);
            }

            textLayout.SetTypography(0, testString.Length, typography);

            typographyChanged = false;
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);

            currentDrawingSession = args.DrawingSession;

            bool needsTypographyHandler = needRefreshTypographyList || HighlightTypographyAffectedGlyphs;
            TypographyHandler typographyHandler = needsTypographyHandler ? new TypographyHandler(testString) : null;

            EnsureTypographyList(typographyHandler);

            EnsureTypography();

            args.DrawingSession.DrawTextLayout(textLayout, 0, 0, Colors.LightBlue);

            if (HighlightTypographyAffectedGlyphs && CurrentTypographyOption.Name != CanvasTypographyFeatureName.None)
            {
                typographyHandler.CurrentMode = TypographyHandler.Mode.Highlight;
                typographyHandler.FeatureToHighlight = CurrentTypographyOption.Name;
                textLayout.DrawToTextRenderer(typographyHandler, 0, 0);
            }
        }

        private void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            needsResourceRecreation = true;
        }

        private void TypographyChanged(object sender, RoutedEventArgs e)
        {
            typographyChanged = true;

            canvas.Invalidate();
        }

        private void InvalidateCanvas(object sender, RoutedEventArgs e)
        {
            canvas.Invalidate();
        }

        private void fontPicker_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            needsResourceRecreation = true;
            needRefreshTypographyList = true;
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
