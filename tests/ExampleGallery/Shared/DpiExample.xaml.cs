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

using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Numerics;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class DpiExample : UserControl, INotifyPropertyChanged
    {
        public enum SourceMode
        {
            DrawingSession,
            DefaultDpiBitmap,
            HighDpiBitmap,
            LowDpiBitmap,
        }

        public enum IntermediateMode
        {
            None,
            AutoDpiRenderTarget,
            HighDpiRenderTarget,
            LowDpiRenderTarget,
            ImageEffect,
            CommandList,
            ImageEffectInCommandList
        }

        public enum OutputMode
        {
            CanvasControl,
            CanvasImageBrush,
            CanvasImageSource,
            CanvasSwapChain,
        }

        public SourceMode       CurrentSource       { get; set; }
        public IntermediateMode CurrentIntermediate { get; set; }
        public OutputMode       CurrentOutput       { get; set; }

        public List<SourceMode>       SourceModeList       { get { return Utils.GetEnumAsList<SourceMode>();       } }
        public List<IntermediateMode> IntermediateModeList { get { return Utils.GetEnumAsList<IntermediateMode>(); } }
        public List<OutputMode>       OutputModeList       { get { return Utils.GetEnumAsList<OutputMode>();       } }

        const float testSize = 128;
        const float testOffset = 20;
        const float controlSize = testSize + testOffset * 2;

        const float defaultDpi = 96;
        const float highDpi = 192;
        const float lowDpi = 48;

        CanvasBitmap defaultDpiBitmap;
        CanvasBitmap highDpiBitmap;
        CanvasBitmap lowDpiBitmap;

        CanvasRenderTarget autoDpiRenderTarget;
        CanvasRenderTarget highDpiRenderTarget;
        CanvasRenderTarget lowDpiRenderTarget;

        SaturationEffect saturationEffect;
        CanvasImageBrush imageBrush;
        CanvasImageSource imageSource;
        CanvasSwapChain swapChain;

        CanvasTextFormat textFormat = new CanvasTextFormat
        {
            VerticalAlignment = CanvasVerticalAlignment.Center,
            ParagraphAlignment = ParagraphAlignment.Center,
        };

        DispatcherTimer cycleTimer = new DispatcherTimer
        {
            Interval = TimeSpan.FromSeconds(0.25)
        };

        string message;
        int drawCount;

        public event PropertyChangedEventHandler PropertyChanged;


        public DpiExample()
        {
            this.InitializeComponent();

            cycleTimer.Tick += cycleTimer_Tick;
        }


        void Canvas_CreateResources(CanvasControl sender, object args)
        {
            defaultDpiBitmap = CreateTestBitmap(sender, defaultDpi);
            highDpiBitmap    = CreateTestBitmap(sender, highDpi);
            lowDpiBitmap     = CreateTestBitmap(sender, lowDpi);

            autoDpiRenderTarget = new CanvasRenderTarget(sender, testSize, testSize);
            highDpiRenderTarget = new CanvasRenderTarget(sender, testSize, testSize, highDpi);
            lowDpiRenderTarget  = new CanvasRenderTarget(sender, testSize, testSize, lowDpi);

            saturationEffect = new SaturationEffect { Saturation = 0 };
            
            imageBrush = new CanvasImageBrush(sender);

            imageSource = new CanvasImageSource(sender, controlSize, controlSize);
            imageControl.Source = imageSource;

            swapChain = new CanvasSwapChain(sender, controlSize, controlSize);
            swapChainPanel.SwapChain = swapChain;
        }


        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            message = "The test graphic should fit the yellow guide markers, regardless of display DPI or what drawing options are selected.\n\n";

            switch (CurrentOutput)
            {
                case OutputMode.CanvasControl:
                    DrawToOutput(args.DrawingSession);
                    break;

                case OutputMode.CanvasImageBrush:
                    DrawViaImageBrush(args.DrawingSession);
                    break;

                case OutputMode.CanvasImageSource:
                    DrawToImageSource(args.DrawingSession);
                    break;

                case OutputMode.CanvasSwapChain:
                    DrawToSwapChain();
                    break;
            }

            // Show or hide overlay controls to fit the current mode.
            imageControl.Visibility = (CurrentOutput == OutputMode.CanvasImageSource) ? Visibility.Visible : Visibility.Collapsed;
            swapChainPanel.Visibility = (CurrentOutput == OutputMode.CanvasSwapChain) ? Visibility.Visible : Visibility.Collapsed;

            // Update the info text.
            message += string.Format("\n\nRedraw count: {0}", ++drawCount);

            textBlock.Text = message;
        }

        
        void DrawToOutput(CanvasDrawingSession ds)
        {
            if (CurrentIntermediate == IntermediateMode.None)
            {
                // We can either draw directly to the output...
                DrawSourceGraphic(ds, testOffset);
            }
            else
            {
                // Or go via an intermediate such as a rendertarget or image effect.
                var intermediateImage = WrapSourceWithIntermediateImage(CurrentIntermediate);

                ds.DrawImage(intermediateImage, testOffset, testOffset);
            }

            message += string.Format("{0} (dpi: {1})", CurrentOutput, ds.Dpi);
        }


        void DrawViaImageBrush(CanvasDrawingSession ds)
        {
            imageBrush.Image = WrapSourceWithIntermediateImage(CurrentIntermediate);
            imageBrush.SourceRectangle = new Rect(0, 0, testSize, testSize);
            imageBrush.Transform = Matrix3x2.CreateTranslation(testOffset, testOffset);

            ds.FillRectangle(testOffset, testOffset, testSize, testSize, imageBrush);

            message += string.Format("CanvasImageBrush ->\nCanvasControl (dpi: {0})", ds.Dpi);
        }


        void DrawToImageSource(CanvasDrawingSession canvasControlDrawingSession)
        {
            // XAML doesn't support nested image source drawing sessions, so we must close
            // the main CanvasControl one before drawing to a different CanvasImageSource.
            canvasControlDrawingSession.Dispose();

            using (var ds = imageSource.CreateDrawingSession(Colors.Transparent))
            {
                DrawToOutput(ds);
            }
        }


        void DrawToSwapChain()
        {
            using (var ds = swapChain.CreateDrawingSession(Colors.Transparent))
            {
                DrawToOutput(ds);
            }

            swapChain.Present();
        }


        ICanvasImage WrapSourceWithIntermediateImage(IntermediateMode mode)
        {
            switch (mode)
            {
                case IntermediateMode.ImageEffect:
                    // We can either feed our graphics through an image effect...
                    saturationEffect.Source = GetSourceBitmap() ??
                                              WrapSourceWithIntermediateImage(IntermediateMode.None);

                    message += "SaturationEffect ->\n";

                    return saturationEffect;

                case IntermediateMode.CommandList:
                    var cl = new CanvasCommandList(canvasControl);
                    using (var ds = cl.CreateDrawingSession())
                    {
                        DrawSourceGraphic(ds, 0);
                    }
                    message += "CommandList ->\n";
                    return cl;

                case IntermediateMode.ImageEffectInCommandList:
                    var cl2 = new CanvasCommandList(canvasControl);
                    using (var ds = cl2.CreateDrawingSession())
                    {
                        ds.DrawImage(WrapSourceWithIntermediateImage(IntermediateMode.ImageEffect));
                    }
                    message += "CommandList ->\n";
                    return cl2;

                default:
                    // ... or draw them into a rendertarget.
                    var renderTarget = GetIntermediateRenderTarget(mode);

                    using (var ds = renderTarget.CreateDrawingSession())
                    {
                        DrawSourceGraphic(ds, 0);
                    }

                    message += string.Format("RenderTarget (dpi: {0}, size: {1}, pixels: {2}) ->\n", renderTarget.Dpi, renderTarget.Size, renderTarget.SizeInPixels);

                    return renderTarget;
            }
        }


        void DrawSourceGraphic(CanvasDrawingSession ds, float offset)
        {
            var source = GetSourceBitmap();

            if (source != null)
            {
                // We can either draw a precreated bitmap...
                ds.DrawImage(source, offset, offset);
            }
            else
            {
                // ... or directly draw some shapes.
                ds.FillRectangle(offset, offset, testSize, testSize, Colors.Gray);

                ds.DrawLine(offset, offset, offset + testSize, offset + testSize, Colors.Red);
                ds.DrawLine(offset + testSize, offset, offset, offset + testSize, Colors.Red);

                ds.DrawRectangle(offset + 0.5f, offset + 0.5f, testSize - 1, testSize - 1, Colors.Blue);

                ds.DrawText("DPI test", new Vector2(offset + testSize / 2), Colors.Blue, textFormat);

                message += "DrawingSession ->\n";
            }
        }


        CanvasBitmap GetSourceBitmap()
        {
            CanvasBitmap bitmap;

            switch (CurrentSource)
            {
                case SourceMode.DefaultDpiBitmap: bitmap = defaultDpiBitmap; break;
                case SourceMode.HighDpiBitmap:    bitmap = highDpiBitmap;    break;
                case SourceMode.LowDpiBitmap:     bitmap = lowDpiBitmap;     break;
                default:                          bitmap = null;             break;
            }

            if (bitmap != null)
            {
                message += string.Format("Bitmap (dpi: {0}, size: {1}, pixels: {2}) ->\n", bitmap.Dpi, bitmap.Size, bitmap.SizeInPixels);
            }

            return bitmap;
        }


        CanvasRenderTarget GetIntermediateRenderTarget(IntermediateMode mode)
        {
            switch (mode)
            {
                case IntermediateMode.HighDpiRenderTarget: return highDpiRenderTarget;
                case IntermediateMode.LowDpiRenderTarget:  return lowDpiRenderTarget;
                default:                                   return autoDpiRenderTarget;
            }
        }
        

        static CanvasBitmap CreateTestBitmap(CanvasControl resourceCreator, float dpi)
        {
            int pixelSize = (int)Math.Round(testSize * dpi / defaultDpi);

            // Initialize to solid gray.
            Color[] colors = Enumerable.Repeat(Colors.Gray, pixelSize * pixelSize).ToArray();

            // Diagonal lines.
            for (int i = 0; i < pixelSize; i++)
            {
                colors[i * pixelSize + i] = Colors.Red;
                colors[i * pixelSize + (pixelSize - i - 1)] = Colors.Red;
            }

            // Single pixel border.
            for (int i = 0; i < pixelSize; i++)
            {
                colors[i] = Colors.Blue;
                colors[i * pixelSize] = Colors.Blue;
                colors[i * pixelSize + pixelSize - 1] = Colors.Blue;
                colors[pixelSize * (pixelSize - 1) + i] = Colors.Blue;
            }

            if (dpi == defaultDpi)
            {
                // We could always just use the "else" path, but want to test this default overload as well.
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize, CanvasAlphaBehavior.Premultiplied);
            }
            else
            {
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize, CanvasAlphaBehavior.Premultiplied, dpi);
            }
        }


        void AutoCycle_Checked(object sender, RoutedEventArgs e)
        {
            cycleTimer.Start();
        }


        void AutoCycle_Unchecked(object sender, RoutedEventArgs e)
        {
            cycleTimer.Stop();
        }


        void cycleTimer_Tick(object sender, object e)
        {
            // Increment the source mode.
            CurrentSource++;

            if (CurrentSource >= (SourceMode)Enum.GetValues(typeof(SourceMode)).Length)
            {
                CurrentSource = 0;

                CurrentIntermediate++;

                // Increment the intermediate mode.
                if (CurrentIntermediate >= (IntermediateMode)Enum.GetValues(typeof(IntermediateMode)).Length)
                {
                    CurrentIntermediate = 0;

                    // Increment the output mode.
                    CurrentOutput++;

                    if (CurrentOutput >= (OutputMode)Enum.GetValues(typeof(OutputMode)).Length)
                    {
                        CurrentOutput = 0;
                    }

                    PropertyChanged(this, new PropertyChangedEventArgs("CurrentOutput"));
                }

                PropertyChanged(this, new PropertyChangedEventArgs("CurrentIntermediate"));
            }

            PropertyChanged(this, new PropertyChangedEventArgs("CurrentSource"));
        }


        void SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canvasControl.Invalidate();
        }
    }
}
