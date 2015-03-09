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
using Microsoft.Graphics.Canvas.DirectX;
using Microsoft.Graphics.Canvas.Effects;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ExampleGallery
{
    // How this works:
    //  - Instantiate each example in turn
    //  - Wait for it to initialize
    //  - Search the XAML visual tree for a CanvasControl or CanvasAnimatedControl
    //  - If found:
    //      - Use reflection to find the method that handles the Draw event
    //      - We guess which method this is by matching parameter signatures (a hack, but it works)
    //      - Invoke the Draw method, passing it a drawing session on our own rendertarget
    //  - If not found:
    //      - Use RenderTargetBitmap to capture other XAML controls (eg. CanvasImageSource)
    //  - Crop the resulting image
    //  - Choose a background color that matches whatever we captured, so each thumbnail gets a different hue
    //  - Add a shadow effect
    //  - Save out wide and narrow thumbnail .png for every example

    public sealed partial class ThumbnailGenerator : UserControl
    {
        // Constants.
        const int controlWidth = 768;
        const int controlHeight = 512;

        const float animationDelay = 1.333f;

        const int dilateAmount = 5;
        const float blurAmount = 5;

        const float minBrightness = 128;
        const float maxBrightness = 256;


        // Fields.
        StorageFolder outputFolder;


        // Some example implementations use this to tweak their rendering when thumbnails are being generated.
        public static bool IsDrawingThumbnail { get; private set; }


        public ThumbnailGenerator(StorageFolder outputFolder)
        {
            this.outputFolder = outputFolder;

            this.InitializeComponent();
        }


        public async void OnLoaded(object sender, RoutedEventArgs e)
        {
            try
            {
                IsDrawingThumbnail = true;

                foreach (var exampleDefinition in ExampleDefinitions.Definitions)
                {
                    // Skip the developer tools menu (bad things happen if we try to add that as a child of itself).
                    if (exampleDefinition.Control == typeof(DeveloperTools))
                        continue;

                    // Capture a thumbnail for this example.
                    var generator = new Generator(exampleDefinition, outputFolder);
                    await generator.GenerateThumbnail(panel);
                }

                var messageBox = new MessageDialog("Thumbnail generation complete.").ShowAsync();
            }
            catch (Exception exception)
            {
                var messageBox = new MessageDialog("Thumbnail generation failed: " + exception).ShowAsync();
            }
            finally
            {
                IsDrawingThumbnail = false;

                // Remove ourselves once all the thumbnails have been generated.
                ((Panel)Parent).Children.Remove(this);
            }
        }


        class Generator
        {
            ExampleDefinition exampleDefinition;
            StorageFolder outputFolder;
            UserControl exampleControl;


            public Generator(ExampleDefinition exampleDefinition, StorageFolder outputFolder)
            {
                this.exampleDefinition = exampleDefinition;
                this.outputFolder = outputFolder;
            }

            
            public async Task GenerateThumbnail(Panel panel)
            {
                // Instantiate the example.
                exampleControl = (UserControl)Activator.CreateInstance(exampleDefinition.Control);

                exampleControl.Width = controlWidth;
                exampleControl.Height = controlHeight;

                // Before doing anything else, we must wait for XAML to finish loading the control.
                // Therefore the thumbnail capture work is triggered by the example Loaded event.
                var completedSignal = new TaskCompletionSource<object>();

                exampleControl.Loaded += async (sender, e) =>
                {
                    try
                    {
                        // Ok! The control is loaded, so we can proceed to capture its contents.
                        await CaptureThumbnail(exampleControl, exampleDefinition);

                        completedSignal.SetResult(null);
                    }
                    catch (Exception exception)
                    {
                        completedSignal.SetException(exception);
                    }
                };

                // Activate the control.
                panel.Children.Add(exampleControl);

                // Wait to be signalled that it has finished loading and captured the thumbnail.
                try
                {
                    await completedSignal.Task;
                }
                finally
                {
                    panel.Children.Remove(exampleControl);
                }
            }


            async Task CaptureThumbnail(UserControl exampleControl, ExampleDefinition exampleDefinition)
            {
                CanvasControl canvasControl;
                ICanvasAnimatedControl animatedControl;
                MethodInfo drawMethod;

                if (FindControlAndDrawMethod<CanvasControl, CanvasDrawEventArgs>(exampleControl, out canvasControl, out drawMethod))
                {
                    // It's a CanvasControl!
                    await CaptureThumbnailFromCanvasControl(canvasControl, drawMethod);
                }
                else if (FindControlAndDrawMethod<ICanvasAnimatedControl, CanvasAnimatedDrawEventArgs>(exampleControl, out animatedControl, out drawMethod))
                {
                    // It's a CanvasAnimatedControl!
                    await CaptureThumbnailFromAnimatedControl(animatedControl, drawMethod);
                }
                else
                {
                    // This example does not use either of the Win2D controls, but we can still capture it via a XAML RenderTargetBitmap.
                    await CaptureThumbnailFromXaml();
                }
            }


            async Task CaptureThumbnailFromCanvasControl(CanvasControl canvasControl, MethodInfo drawMethod)
            {
                // Wait for the control to be ready.
                while (!canvasControl.ReadyToDraw)
                {
                    await Task.Delay(1);
                }

                // Capture a thumbnail from it.
                await CaptureThumbnailFromControl(canvasControl, canvasControl.Size, drawMethod, ds => new CanvasDrawEventArgs(ds));
            }


            async Task CaptureThumbnailFromAnimatedControl(ICanvasAnimatedControl animatedControl, MethodInfo drawMethod)
            {
                // Wait for the control to be ready.
                while (!animatedControl.ReadyToDraw)
                {
                    await Task.Delay(1);
                }

                // Wait a while for any animations to settle into a good looking state.
                await Task.Delay(TimeSpan.FromSeconds(animationDelay));

                // We will mess with the control device from somewhere other than its game loop thread,
                // so must first pause the control to stop the game loop. There's no good way to
                // synchronize this, so we just wait a moment to give the game loop a chance to exit.
                animatedControl.Paused = true;
                // TODO #3317: once we can RunAsync on the update/render thread, use that instead of this
                await Task.Delay(TimeSpan.FromSeconds(0.1f));

                // Capture a thumbnail from the control.
                var timing = new CanvasTimingInformation
                {
                    TotalTime = TimeSpan.FromSeconds(animationDelay),
                    UpdateCount = (int)(animationDelay * 60),
                };

                await CaptureThumbnailFromControl(animatedControl, animatedControl.Size, drawMethod, ds => new CanvasAnimatedDrawEventArgs(ds, timing));
            }


            async Task CaptureThumbnailFromXaml()
            {
                // Wait a while for any animations to settle into a good looking state.
                await Task.Delay(TimeSpan.FromSeconds(animationDelay));

                // Which UI element should we capture? We currently just grab the first Image control.
                // This works for the ImageSourceUpdateRegion example, but may need extending in future.
                UIElement elementToCapture = GetDescendantsOfType<Image>(exampleControl).First();

                // Tell XAML to render into a WriteableBitmap.
                var bitmap = new RenderTargetBitmap();
                await bitmap.RenderAsync(elementToCapture);
                var pixels = await bitmap.GetPixelsAsync();

                // Transfer the pixel data from XAML to Win2D for further processing.
                using (CanvasDevice canvasDevice = new CanvasDevice())
                using (CanvasBitmap canvasBitmap = CanvasBitmap.CreateFromBytes(canvasDevice, pixels.ToArray(), bitmap.PixelWidth, bitmap.PixelHeight, DirectXPixelFormat.B8G8R8A8UIntNormalized, CanvasAlphaMode.Premultiplied))
                {
                    await SaveThumbnails(canvasBitmap);
                }
            }


            async Task CaptureThumbnailFromControl(ICanvasResourceCreator canvasControl, Size controlSize, MethodInfo drawMethod, Func<CanvasDrawingSession, object> createDrawEventArgs)
            {
                // Use reflection to invoke the same method the example would normally use to handle the Draw
                // event. Because we are calling this directly rather than the control raising the event in
                // the normal way, we can cunningly redirect the drawing into a rendertarget of our choosing.
                var renderTarget = new CanvasRenderTarget(canvasControl, (float)controlSize.Width, (float)controlSize.Height, 96);

                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.Clear(Colors.Transparent);

                    object[] args = { canvasControl, createDrawEventArgs(ds) };

                    drawMethod.Invoke(exampleControl, args);
                }

                await SaveThumbnails(renderTarget);
            }


            async Task SaveThumbnails(CanvasBitmap capturedBitmap)
            {
                // Apply magic to make the thumbnail images visually attractive and appropriately sized.
                var wideThumbnail = MakeThumbnailPretty(capturedBitmap, 300, 150, new Rect(115, 15, 170, 120));
                var narrowThumbnail = MakeThumbnailPretty(capturedBitmap, 150, 150, new Rect(15, 15, 120, 100));

                // Write the output files.
                await SaveThumbnail(wideThumbnail, "Wide");
                await SaveThumbnail(narrowThumbnail, "Narrow");
            }


            async Task SaveThumbnail(CanvasBitmap thumbnail, string suffix)
            {
                using (var stream = await outputFolder.OpenStreamForWriteAsync(exampleDefinition.ThumbnailFilename(suffix), CreationCollisionOption.ReplaceExisting))
                {
                    await thumbnail.SaveAsync(stream.AsRandomAccessStream(), CanvasBitmapFileFormat.Png);
                }
            }

            
            static CanvasBitmap MakeThumbnailPretty(CanvasBitmap capturedBitmap, float thumbnailWidth, float thumbnailHeight, Rect targetRect)
            {
                var pixelColors = capturedBitmap.GetPixelColors();

                // Remove any unused space around the edge of the bitmap, so it will fill the thumbnail.
                Rect cropRect = CropCapturedBitmap(capturedBitmap, pixelColors);

                // Choose a (hopefully) aesthetically pleasing background color.
                Color backgroundColor = ChooseBackgroundColor(pixelColors);

                // Apply letterbox scaling to fit the image into the target thumbnail.
                Vector2 outputSize = new Vector2((float)targetRect.Width, (float)targetRect.Height);
                var sourceSize = new Vector2((float)cropRect.Width, (float)cropRect.Height);
                var letterbox = Utils.GetDisplayTransform(outputSize, sourceSize);
                var translate = Matrix3x2.CreateTranslation((float)targetRect.X, (float)targetRect.Y);

                // Position the image where we want it.
                var scaledImage = new Transform2DEffect
                {
                    Source = new AtlasEffect
                    {
                        Source = capturedBitmap,
                        SourceRectangle = cropRect,
                    },
                    InterpolationMode = CanvasImageInterpolation.HighQualityCubic,
                    TransformMatrix = letterbox * translate,
                };

                // Create the final thumbnail image.
                var finalImage = new CompositeEffect
                {
                    Inputs = 
                { 
                    // Blurred shadow.
                    new ShadowEffect
                    {
                        Source = new MorphologyEffect
                        {
                            Source = scaledImage,
                            Mode = MorphologyEffectMode.Dilate,
                            Width = dilateAmount,
                            Height = dilateAmount,
                        },
                        BlurAmount = blurAmount,
                    },

                    // Overlay the image itself.
                    scaledImage 
                }
                };

                // Rasterize the effect into a rendertarget.
                CanvasRenderTarget output = new CanvasRenderTarget(capturedBitmap.Device, thumbnailWidth, thumbnailHeight, 96);

                using (var ds = output.CreateDrawingSession())
                {
                    ds.Clear(backgroundColor);
                    ds.DrawImage(finalImage);
                }

                return output;
            }


            static Rect CropCapturedBitmap(CanvasBitmap capturedBitmap, Color[] pixelColors)
            {
                Debug.Assert(capturedBitmap.Dpi == 96, "The following code mixes up dips and pixels in ways that are only valid if these units are the same.");

                Rect rect = capturedBitmap.Bounds;
                uint stride = capturedBitmap.SizeInPixels.Width;

                // Crop transparent pixels from the left of the bitmap.
                while (rect.Width > 1 && ArePixelsTransparent(pixelColors, stride, (int)rect.X, (int)rect.Y, 1, (int)rect.Height))
                {
                    rect.X++;
                    rect.Width--;
                }

                // Crop transparent pixels from the top of the bitmap.
                while (rect.Height > 1 && ArePixelsTransparent(pixelColors, stride, (int)rect.X, (int)rect.Y, (int)rect.Width, 1))
                {
                    rect.Y++;
                    rect.Height--;
                }

                // Crop transparent pixels from the right of the bitmap.
                while (rect.Width > 1 && ArePixelsTransparent(pixelColors, stride, (int)rect.X + (int)rect.Width - 1, (int)rect.Y, 1, (int)rect.Height))
                {
                    rect.Width--;
                }

                // Crop transparent pixels from the bottom of the bitmap.
                while (rect.Height > 1 && ArePixelsTransparent(pixelColors, stride, (int)rect.X, (int)rect.Y + (int)rect.Height - 1, (int)rect.Width, 1))
                {
                    rect.Height--;
                }

                return rect;
            }


            // Checks whether all the pixels in the specified region are fully transparent.
            static bool ArePixelsTransparent(Color[] colors, uint stride, int x, int y, int w, int h)
            {
                for (int i = 0; i < w; i++)
                {
                    for (int j = 0; j < h; j++)
                    {
                        var color = colors[((y + j) * stride) + (x + i)];

                        if (color.A > 0)
                        {
                            return false;
                        }
                    }
                }

                return true;
            }


            static Color ChooseBackgroundColor(Color[] pixelColors)
            {
                // Thumbnail background color should reflect the color of the example itself.
                // We start by totalling all the rendered pixel values.
                Vector3 color = Vector3.Zero;
                float alpha = 0;

                foreach (var c in pixelColors)
                {
                    color.X += c.R;
                    color.Y += c.G;
                    color.Z += c.B;

                    alpha += c.A;
                }

                // Normalize to remove the contribution of zero alpha pixels.
                color /= (alpha / 255);

                // Clamp if the result is excessively dark or bright.
                float brightness = color.Length();

                if (brightness < minBrightness)
                {
                    color = Vector3.Normalize(color + new Vector3(0.01f)) * minBrightness;
                }
                else if (brightness > maxBrightness)
                {
                    color = Vector3.Normalize(color) * maxBrightness;
                }

                return Color.FromArgb(255, (byte)color.X, (byte)color.Y, (byte)color.Z);
            }


            // Looks for a control of the specified type, plus the method that handles its Draw event.
            static bool FindControlAndDrawMethod<TControl, TDrawEventArgs>(UserControl parent, out TControl control, out MethodInfo drawMethod)
                where TControl : class
            {
                // Look for a control of the specified type.
                control = GetDescendantsOfType<TControl>(parent).FirstOrDefault();

                if (control == null)
                {
                    drawMethod = null;
                    return false;
                }

                // Look for the method that handles the Draw event. This is identified by having two parameters,
                // the first being the control and the second matching the type of its draw event args.
                var drawMethods = from method in parent.GetType().GetRuntimeMethods()
                                  where method.GetParameters().Length == 2
                                  where method.GetParameters()[0].ParameterType == typeof(TControl)
                                  where method.GetParameters()[1].ParameterType == typeof(TDrawEventArgs)
                                  select method;

                drawMethod = drawMethods.FirstOrDefault();

                return drawMethod != null;
            }


            // Searches the XAML visual tree for objects of the specified type.
            static IEnumerable<T> GetDescendantsOfType<T>(DependencyObject parent)
                where T : class
            {
                for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parent); i++)
                {
                    var child = VisualTreeHelper.GetChild(parent, i);

                    if (child is T)
                    {
                        yield return (T)(object)child;
                    }

                    foreach (var grandChild in GetDescendantsOfType<T>(child))
                    {
                        yield return grandChild;
                    }
                }
            }
        }
    }
}
