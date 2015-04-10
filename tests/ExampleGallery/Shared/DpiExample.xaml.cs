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
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
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
            CanvasAnimatedControl,
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


        // We need two copies of all these graphics resources, one for the main CanvasDevice used on the
        // UI thread, plus a second set for use by the CanvasAnimatedControl which runs on a separate thread.
        class PerDeviceResources
        {
            public readonly ICanvasResourceCreatorWithDpi ResourceCreator;

            public readonly CanvasBitmap DefaultDpiBitmap;
            public readonly CanvasBitmap HighDpiBitmap;
            public readonly CanvasBitmap LowDpiBitmap;

            public readonly CanvasRenderTarget AutoDpiRenderTarget;
            public readonly CanvasRenderTarget HighDpiRenderTarget;
            public readonly CanvasRenderTarget LowDpiRenderTarget;

            public readonly SaturationEffect SaturationEffect;

            public readonly CanvasTextFormat TextFormat = new CanvasTextFormat
            {
                VerticalAlignment = CanvasVerticalAlignment.Center,
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
            };

            string message;
            int drawCount;


            public PerDeviceResources(ICanvasResourceCreatorWithDpi resourceCreator)
            {
                ResourceCreator = resourceCreator;

                DefaultDpiBitmap = CreateTestBitmap(resourceCreator, defaultDpi);
                HighDpiBitmap = CreateTestBitmap(resourceCreator, highDpi);
                LowDpiBitmap = CreateTestBitmap(resourceCreator, lowDpi);

                AutoDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize);
                HighDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize, highDpi);
                LowDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize, lowDpi);

                SaturationEffect = new SaturationEffect { Saturation = 0 };
            }


            public void InitMessage()
            {
                message = "The test graphic should fit the yellow guide markers, regardless of display DPI or what drawing options are selected.\n\n";
            }

            public void AddMessage(string format, params object[] args)
            {
                message += string.Format(format, args);
            }

            public string GetFinalMessage()
            {
                AddMessage("\n\nRedraw count: {0}", ++drawCount);

                return message;
            }
        }
        

        PerDeviceResources mainDeviceResources;
        PerDeviceResources animatedDeviceResources;

        CanvasImageBrush imageBrush;
        CanvasImageSource imageSource;
        CanvasSwapChain swapChain;

        DispatcherTimer cycleTimer = new DispatcherTimer
        {
            Interval = TimeSpan.FromSeconds(0.25)
        };

        public event PropertyChangedEventHandler PropertyChanged;


        public DpiExample()
        {
            this.InitializeComponent();

            cycleTimer.Tick += cycleTimer_Tick;
        }


        void control_Unloaded(object sender, RoutedEventArgs e)
        {
            cycleTimer.Stop();

            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
            mainDeviceResources = null;

            animatedControl.RemoveFromVisualTree();
            animatedControl = null;
            animatedDeviceResources = null;

            swapChainPanel.RemoveFromVisualTree();
            swapChainPanel = null;
        }


        void Canvas_CreateResources(CanvasControl sender, object args)
        {
            mainDeviceResources = new PerDeviceResources(sender);

            imageBrush = new CanvasImageBrush(sender);

            imageSource = new CanvasImageSource(sender, controlSize, controlSize);
            imageControl.Source = imageSource;

            swapChain = new CanvasSwapChain(sender, controlSize, controlSize);
            swapChainPanel.SwapChain = swapChain;
        }


        void AnimatedCanvas_CreateResources(ICanvasAnimatedControl sender, object args)
        {
            animatedDeviceResources = new PerDeviceResources(sender);
        }


        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            mainDeviceResources.InitMessage();

            switch (CurrentOutput)
            {
                case OutputMode.CanvasControl:
                    DrawToOutput(mainDeviceResources, args.DrawingSession);
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
            imageControl.Visibility    = (CurrentOutput == OutputMode.CanvasImageSource)     ? Visibility.Visible : Visibility.Collapsed;
            swapChainPanel.Visibility  = (CurrentOutput == OutputMode.CanvasSwapChain)       ? Visibility.Visible : Visibility.Collapsed;
            animatedControl.Visibility = (CurrentOutput == OutputMode.CanvasAnimatedControl) ? Visibility.Visible : Visibility.Collapsed;
            animatedControl.Paused     = (CurrentOutput != OutputMode.CanvasAnimatedControl);

            // Update the info text.
            textBlock.Text = mainDeviceResources.GetFinalMessage();
        }


        void AnimatedCanvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            animatedDeviceResources.InitMessage();

            DrawToOutput(animatedDeviceResources, args.DrawingSession);

            // Update the info text.
            var message = animatedDeviceResources.GetFinalMessage();

            var task = textBlock.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                textBlock.Text = message;
            });
        }


        void DrawToOutput(PerDeviceResources resources, CanvasDrawingSession ds)
        {
            if (CurrentIntermediate == IntermediateMode.None)
            {
                // We can either draw directly to the output...
                DrawSourceGraphic(resources, ds, testOffset);
            }
            else
            {
                // Or go via an intermediate such as a rendertarget or image effect.
                var intermediateImage = WrapSourceWithIntermediateImage(resources, CurrentIntermediate);

                ds.DrawImage(intermediateImage, testOffset, testOffset);
            }

            resources.AddMessage("{0} (dpi: {1})", CurrentOutput, ds.Dpi);
        }


        void DrawViaImageBrush(CanvasDrawingSession ds)
        {
            imageBrush.Image = WrapSourceWithIntermediateImage(mainDeviceResources, CurrentIntermediate);
            imageBrush.SourceRectangle = new Rect(0, 0, testSize, testSize);
            imageBrush.Transform = Matrix3x2.CreateTranslation(testOffset, testOffset);

            ds.FillRectangle(testOffset, testOffset, testSize, testSize, imageBrush);

            mainDeviceResources.AddMessage("CanvasImageBrush ->\nCanvasControl (dpi: {0})", ds.Dpi);
        }


        void DrawToImageSource(CanvasDrawingSession canvasControlDrawingSession)
        {
            // XAML doesn't support nested image source drawing sessions, so we must close
            // the main CanvasControl one before drawing to a different CanvasImageSource.
            canvasControlDrawingSession.Dispose();

            using (var ds = imageSource.CreateDrawingSession(Colors.Transparent))
            {
                DrawToOutput(mainDeviceResources, ds);
            }
        }


        void DrawToSwapChain()
        {
            using (var ds = swapChain.CreateDrawingSession(Colors.Transparent))
            {
                DrawToOutput(mainDeviceResources, ds);
            }

            swapChain.Present();
        }


        ICanvasImage WrapSourceWithIntermediateImage(PerDeviceResources resources, IntermediateMode mode)
        {
            switch (mode)
            {
                case IntermediateMode.ImageEffect:
                    // We can either feed our graphics through an image effect...
                    resources.SaturationEffect.Source = GetSourceBitmap(resources) ??
                                                        WrapSourceWithIntermediateImage(resources, IntermediateMode.None);

                    resources.AddMessage("SaturationEffect ->\n");

                    return resources.SaturationEffect;

                case IntermediateMode.CommandList:
                    var cl = new CanvasCommandList(resources.ResourceCreator);
                    using (var ds = cl.CreateDrawingSession())
                    {
                        DrawSourceGraphic(resources, ds, 0);
                    }
                    resources.AddMessage("CommandList ->\n");
                    return cl;

                case IntermediateMode.ImageEffectInCommandList:
                    var cl2 = new CanvasCommandList(resources.ResourceCreator);
                    using (var ds = cl2.CreateDrawingSession())
                    {
                        ds.DrawImage(WrapSourceWithIntermediateImage(resources, IntermediateMode.ImageEffect));
                    }
                    resources.AddMessage("CommandList ->\n");
                    return cl2;

                default:
                    // ... or draw them into a rendertarget.
                    var renderTarget = GetIntermediateRenderTarget(resources, mode);

                    using (var ds = renderTarget.CreateDrawingSession())
                    {
                        DrawSourceGraphic(resources, ds, 0);
                    }

                    resources.AddMessage("RenderTarget (dpi: {0}, size: {1}, pixels: {2}) ->\n", renderTarget.Dpi, renderTarget.Size, renderTarget.SizeInPixels);

                    return renderTarget;
            }
        }


        void DrawSourceGraphic(PerDeviceResources resources, CanvasDrawingSession ds, float offset)
        {
            var source = GetSourceBitmap(resources);

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

                ds.DrawText("DPI test", new Vector2(offset + testSize / 2), Colors.Blue, resources.TextFormat);

                resources.AddMessage("DrawingSession ->\n");
            }
        }


        CanvasBitmap GetSourceBitmap(PerDeviceResources resources)
        {
            CanvasBitmap bitmap;

            switch (CurrentSource)
            {
                case SourceMode.DefaultDpiBitmap: bitmap = resources.DefaultDpiBitmap; break;
                case SourceMode.HighDpiBitmap:    bitmap = resources.HighDpiBitmap;    break;
                case SourceMode.LowDpiBitmap:     bitmap = resources.LowDpiBitmap;     break;
                default:                          bitmap = null;                       break;
            }

            if (bitmap != null)
            {
                resources.AddMessage("Bitmap (dpi: {0}, size: {1}, pixels: {2}) ->\n", bitmap.Dpi, bitmap.Size, bitmap.SizeInPixels);
            }

            return bitmap;
        }


        static CanvasRenderTarget GetIntermediateRenderTarget(PerDeviceResources resources, IntermediateMode mode)
        {
            switch (mode)
            {
                case IntermediateMode.HighDpiRenderTarget: return resources.HighDpiRenderTarget;
                case IntermediateMode.LowDpiRenderTarget:  return resources.LowDpiRenderTarget;
                default:                                   return resources.AutoDpiRenderTarget;
            }
        }
        

        static CanvasBitmap CreateTestBitmap(ICanvasResourceCreatorWithDpi resourceCreator, float dpi)
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
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize, CanvasAlphaMode.Premultiplied);
            }
            else
            {
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize, CanvasAlphaMode.Premultiplied, dpi);
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
            if (!cycleTimer.IsEnabled)
                return;

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
