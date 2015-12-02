// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage.Streams;
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
#if WINDOWS_UWP
            VirtualBitmap,
#endif
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
            CanvasVirtualControl,
            CanvasVirtualImageSource,
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


#if !WINDOWS_UWP
        // dummy CanvasVirtualBitmap type to simplify some of the ifdefs
        class CanvasVirtualBitmap { }
#endif


        // We need two copies of all these graphics resources, one for the main CanvasDevice used on the
        // UI thread, plus a second set for use by the CanvasAnimatedControl which runs on a separate thread.
        class PerDeviceResources
        {
            public readonly ICanvasResourceCreatorWithDpi ResourceCreator;

            public readonly CanvasBitmap DefaultDpiBitmap;
            public readonly CanvasBitmap HighDpiBitmap;
            public readonly CanvasBitmap LowDpiBitmap;        
            public readonly CanvasVirtualBitmap VirtualBitmap;

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

            PerDeviceResources(ICanvasResourceCreatorWithDpi resourceCreator, CanvasVirtualBitmap virtualBitmap)
            {
                ResourceCreator = resourceCreator;

                DefaultDpiBitmap = CreateTestBitmap(resourceCreator, defaultDpi);
                HighDpiBitmap = CreateTestBitmap(resourceCreator, highDpi);
                LowDpiBitmap = CreateTestBitmap(resourceCreator, lowDpi);
                VirtualBitmap = virtualBitmap;

                AutoDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize);
                HighDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize, highDpi);
                LowDpiRenderTarget = new CanvasRenderTarget(resourceCreator, testSize, testSize, lowDpi);

                SaturationEffect = new SaturationEffect { Saturation = 0 };
            }


            static async public Task<PerDeviceResources> Create(ICanvasResourceCreatorWithDpi resourceCreator)
            {                
                var virtualBitmap = await CreateTestVirtualBitmap(resourceCreator);
                return new PerDeviceResources(resourceCreator, virtualBitmap);
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
        CanvasVirtualImageSource virtualImageSource;

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

            virtualControl.RemoveFromVisualTree();
            virtualControl = null;
        }


        void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(LoadCanvasResources(sender).AsAsyncAction());

            imageBrush = new CanvasImageBrush(sender);

            imageSource = new CanvasImageSource(sender, controlSize, controlSize);
            imageControl.Source = imageSource;

            virtualImageSource = new CanvasVirtualImageSource(sender, controlSize, controlSize);
            virtualImageControl.Source = virtualImageSource.Source;

            swapChain = new CanvasSwapChain(sender, controlSize, controlSize);
            swapChainPanel.SwapChain = swapChain;
        }


        async Task LoadCanvasResources(CanvasControl sender)
        {
            mainDeviceResources = await PerDeviceResources.Create(sender);
        }


        void AnimatedCanvas_CreateResources(ICanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(LoadAnimatedCanvasResources(sender).AsAsyncAction());
        }


        async Task LoadAnimatedCanvasResources(ICanvasAnimatedControl sender)
        {
            animatedDeviceResources = await PerDeviceResources.Create(sender);
        }


        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            mainDeviceResources.InitMessage();

            IAsyncAction pendingDrawTask = null;

            switch (CurrentOutput)
            {
                case OutputMode.CanvasControl:
                    DrawToOutput(mainDeviceResources, args.DrawingSession);
                    break;

                case OutputMode.CanvasImageBrush:
                    DrawViaImageBrush(args.DrawingSession);
                    break;

                case OutputMode.CanvasImageSource:
                    pendingDrawTask = RunOnUIThread(DrawToImageSource);
                    break;

                case OutputMode.CanvasSwapChain:
                    DrawToSwapChain();
                    break;

                case OutputMode.CanvasVirtualControl:
                    pendingDrawTask = RunOnUIThread(DrawToVirtualControl);
                    break;

                case OutputMode.CanvasVirtualImageSource:
                    pendingDrawTask = RunOnUIThread(DrawToVirtualImageSource);
                    break;
            }

            // Show or hide overlay controls to fit the current mode.
            imageControl.Visibility        = (CurrentOutput == OutputMode.CanvasImageSource)        ? Visibility.Visible : Visibility.Collapsed;
            swapChainPanel.Visibility      = (CurrentOutput == OutputMode.CanvasSwapChain)          ? Visibility.Visible : Visibility.Collapsed;
            virtualControl.Visibility      = (CurrentOutput == OutputMode.CanvasVirtualControl)     ? Visibility.Visible : Visibility.Collapsed;
            virtualImageControl.Visibility = (CurrentOutput == OutputMode.CanvasVirtualImageSource) ? Visibility.Visible : Visibility.Collapsed;
            animatedControl.Visibility     = (CurrentOutput == OutputMode.CanvasAnimatedControl)    ? Visibility.Visible : Visibility.Collapsed;
            animatedControl.Paused         = (CurrentOutput != OutputMode.CanvasAnimatedControl);

            // Update the info text.
            DispatchedHandler updateText = () => textBlock.Text = mainDeviceResources.GetFinalMessage();

            if (pendingDrawTask == null)
            {
                updateText();
            }
            else
            {
                pendingDrawTask.AsTask().ContinueWith(task => RunOnUIThread(updateText));
            }
        }


        void AnimatedCanvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            animatedDeviceResources.InitMessage();

            DrawToOutput(animatedDeviceResources, args.DrawingSession);

            // Update the info text.
            var message = animatedDeviceResources.GetFinalMessage();

            var task = RunOnUIThread(() => textBlock.Text = message);
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


        void DrawToImageSource()
        {
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


        void DrawToVirtualControl()
        {
            var bounds = new Rect(new Point(), virtualControl.Size);

            using (var ds = virtualControl.CreateDrawingSession(bounds))
            {
                DrawToOutput(mainDeviceResources, ds);
            }
        }


        void DrawToVirtualImageSource()
        {
            var bounds = new Rect(new Point(), virtualImageSource.Size);

            using (var ds = virtualImageSource.CreateDrawingSession(Colors.Transparent, bounds))
            {
                DrawToOutput(mainDeviceResources, ds);
            }
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

                    var pixels = renderTarget.SizeInPixels;
                    resources.AddMessage("RenderTarget (dpi: {0}, size: {1}, pixels: {2},{3}) ->\n", renderTarget.Dpi, renderTarget.Size, pixels.Width, pixels.Height);

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


        ICanvasImage GetSourceBitmap(PerDeviceResources resources)
        {
#if WINDOWS_UWP
            if (CurrentSource == SourceMode.VirtualBitmap)
            {
                var virtualBitmap = resources.VirtualBitmap;
                var pixels = virtualBitmap.SizeInPixels;
                resources.AddMessage("VirtualBitmap (dpi: 96, size: {0}, pixels: {1},{2} ->\n", virtualBitmap.Size, pixels.Width, pixels.Height);
                return resources.VirtualBitmap;
            }
#endif

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
                var pixels = bitmap.SizeInPixels;
                resources.AddMessage("Bitmap (dpi: {0}, size: {1}, pixels: {2},{3}) ->\n", bitmap.Dpi, bitmap.Size, pixels.Width, pixels.Height);
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
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize);
            }
            else
            {
                return CanvasBitmap.CreateFromColors(resourceCreator, colors, pixelSize, pixelSize, dpi);
            }
        }


        static async Task<CanvasVirtualBitmap> CreateTestVirtualBitmap(ICanvasResourceCreatorWithDpi resourceCreator)
        {
#if WINDOWS_UWP
            var canvasBitmap = CreateTestBitmap(resourceCreator, 96);

            var stream = new InMemoryRandomAccessStream();
            await canvasBitmap.SaveAsync(stream, CanvasBitmapFileFormat.Png);

            return await CanvasVirtualBitmap.LoadAsync(resourceCreator, stream);
#else
            await Task.Yield(); // prevent warning about no await in async method
            return new CanvasVirtualBitmap();
#endif
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


        IAsyncAction RunOnUIThread(DispatchedHandler action)
        {
            return control.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action);
        }
    }
}
