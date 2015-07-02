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
using Microsoft.Graphics.Canvas.Effects;
using System;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Popups;

namespace ExampleGallery
{
    class AppIconGenerator
    {
        // Describe the apps for which icons will be generated.
        class AppInfo
        {
            public string Name;
            public Action<CanvasDrawingSession, IconInfo> DrawIconImage;
            public Color BackgroundColor;
            public float ImageScale;
            public bool AddShadow;

            public AppInfo(string name, Action<CanvasDrawingSession, IconInfo> drawIconImage, Color backgroundColor)
            {
                Name = name;
                DrawIconImage = drawIconImage;
                BackgroundColor = backgroundColor;

                ImageScale = 1.0f;
                AddShadow = false;
            }
        };

        static AppInfo[] apps =
        {
            new AppInfo("ExampleGallery",    DrawExampleGalleryIcon, Color.FromArgb(255, 0xE7, 0x59, 0x34)) { AddShadow = true },
            new AppInfo("CoreWindowExample", DrawCoreWindowIcon,     Colors.CornflowerBlue)                 { ImageScale = 0.6f },
            new AppInfo("SimpleSample",      DrawSimpleSampleIcon,   Colors.CornflowerBlue)                 { ImageScale = 0.6f },
        };


        // Describe the different platforms and resolutions for which icons will be generated.
        class IconInfo
        {
            public string Platform;
            public string Filename;
            public int Width;
            public int Height;
            public float BottomPadding;
            public bool TransparentBackground;

            public IconInfo(string platform, string filename, int width, int height)
            {
                Platform = platform;
                Filename = filename;
                Width = width;
                Height = height;

                BottomPadding = 0;
                TransparentBackground = false;
            }
        };

        static IconInfo[] requiredIcons =
        {
            new IconInfo("Windows",      "Logo.scale-100.png",            150,  150)  { BottomPadding = 0.25f },
            new IconInfo("Windows",      "Wide310x150Logo.scale-100.png", 310,  150)  { BottomPadding = 0.25f },
            new IconInfo("Windows",      "SmallLogo.scale-100.png",       30,   30),
            new IconInfo("Windows",      "SplashScreen.scale-100.png",    620,  300)  { BottomPadding = 0.1f, TransparentBackground = true },
            new IconInfo("Windows",      "StoreLogo.scale-100.png",       50,   50),
            new IconInfo("Windows",      "BadgeLogo.scale-100.png",       24,   24),

            new IconInfo("WindowsPhone", "Logo.scale-240.png",            360,  360)  { BottomPadding = 0.25f },
            new IconInfo("WindowsPhone", "WideLogo.scale-240.png",        744,  360)  { BottomPadding = 0.25f },
            new IconInfo("WindowsPhone", "SmallLogo.scale-240.png",       106,  106),
            new IconInfo("WindowsPhone", "Square71x71Logo.scale-240.png", 170,  170),
            new IconInfo("WindowsPhone", "SplashScreen.scale-240.png",    1152, 1920) { TransparentBackground = true},
            new IconInfo("WindowsPhone", "StoreLogo.scale-240.png",       120,  120),

            new IconInfo("UAP",          "Logo.scale-100.png",            150,  150)  { BottomPadding = 0.25f },
            new IconInfo("UAP",          "WideLogo.scale-100.png",        310,  150)  { BottomPadding = 0.25f },
            new IconInfo("UAP",          "SmallLogo.scale-100.png",       44,   44),
            new IconInfo("UAP",          "Square71x71Logo.scale-100.png", 71,   71),
            new IconInfo("UAP",          "SplashScreen.scale-100.png",    620,  300)  { BottomPadding = 0.1f, TransparentBackground = true },
            new IconInfo("UAP",          "StoreLogo.scale-100.png",       50,   50),

            new IconInfo("Store",        "300x300.png",                   300,  300),
            new IconInfo("Store",        "358x173.png",                   358,  173),
            new IconInfo("Store",        "358x358.png",                   358,  358),
            new IconInfo("Store",        "414x180.png",                   414,  180),
            new IconInfo("Store",        "414x468.png",                   414,  468),
            new IconInfo("Store",        "558x558.png",                   558,  558),
            new IconInfo("Store",        "558x756.png",                   558,  756),
            new IconInfo("Store",        "846x468.png",                   846,  468),
        };


        // Fields.
        StorageFolder outputFolder;

        CanvasDevice device;


        public AppIconGenerator(Windows.Storage.StorageFolder outputFolder)
        {
            this.outputFolder = outputFolder;

            device = new CanvasDevice();
        }


        public async Task GenerateIcons()
        {
            try
            {
                var iconsByPlatform = from icon in requiredIcons
                                      group icon by icon.Platform into icons
                                      select icons;

                // Per app...
                foreach (var appInfo in apps)
                {
                    var appFolder = await outputFolder.CreateFolderAsync(appInfo.Name, CreationCollisionOption.OpenIfExists);

                    // Per platform...
                    foreach (var iconGroup in iconsByPlatform)
                    {
                        var platformFolder = await appFolder.CreateFolderAsync(iconGroup.Key, CreationCollisionOption.OpenIfExists);
                        var assetsFolder = await platformFolder.CreateFolderAsync("Assets", CreationCollisionOption.OpenIfExists);

                        // Per icon resolution...
                        foreach (var iconInfo in iconGroup)
                        {
                            await GenerateIcon(appInfo, iconInfo, assetsFolder);
                        }
                    }
                }

                var messageBox = new MessageDialog("Icon generation complete.").ShowAsync();
            }
            catch (Exception exception)
            {
                var messageBox = new MessageDialog("Icon generation failed: " + exception).ShowAsync();
            }
        }


        async Task GenerateIcon(AppInfo appInfo, IconInfo iconInfo, StorageFolder folder)
        {
            // Draw the icon image into a command list.
            var iconImage = new CanvasCommandList(device);

            using (var ds = iconImage.CreateDrawingSession())
            {
                appInfo.DrawIconImage(ds, iconInfo);
            }

            // Rasterize into a rendertarget.
            var renderTarget = new CanvasRenderTarget(device, iconInfo.Width, iconInfo.Height, 96);

            using (var ds = renderTarget.CreateDrawingSession())
            {
                // Initialize with the appropriate background color.
                ds.Clear(iconInfo.TransparentBackground ? Colors.Transparent : appInfo.BackgroundColor);

                // Work out where to position the icon image.
                var imageBounds = iconImage.GetBounds(ds);

                imageBounds.Height *= 1 + iconInfo.BottomPadding;

                float scaleUpTheSmallerIcons = Math.Max(1, 1 + (60f - iconInfo.Width) / 50f);

                float imageScale = appInfo.ImageScale * scaleUpTheSmallerIcons;

                ds.Transform = Matrix3x2.CreateTranslation((float)-imageBounds.X, (float)-imageBounds.Y) *
                               Utils.GetDisplayTransform(renderTarget.Size.ToVector2(), new Vector2((float)imageBounds.Width, (float)imageBounds.Height)) *
                               Matrix3x2.CreateScale(imageScale, renderTarget.Size.ToVector2() / 2);

                // Optional shadow effet.
                if (appInfo.AddShadow)
                {
                    var shadow = new ShadowEffect
                    {
                        Source = iconImage,
                        BlurAmount = 12,
                    };

                    ds.DrawImage(shadow);
                }

                // draw the main icon image.
                ds.DrawImage(iconImage);
            }

            // Save to a file.
            using (var stream = await folder.OpenStreamForWriteAsync(iconInfo.Filename, CreationCollisionOption.ReplaceExisting))
            {
                await renderTarget.SaveAsync(stream.AsRandomAccessStream(), CanvasBitmapFileFormat.Png);
            }
        }


        // Example Gallery reuses the existing drawing code in BurningTextExample to create its icon.
        static void DrawExampleGalleryIcon(CanvasDrawingSession ds, IconInfo iconInfo)
        {
            string text = (iconInfo.Width < 42) ? "W" : "Win2D";

            var burningText = new BurningTextExample();

            burningText.DrawIcon(ds, text);
        }


        // For CoreWindowExample, we use some interestingly shaped Unicode glyphs.
        static void DrawCoreWindowIcon(CanvasDrawingSession ds, IconInfo iconInfo)
        {
            // Note how this doesn't bother to specify a meaningful size or position. That's ok
            // because GenerateIcon will later examine the bounds of whatever was drawn, and
            // adjust it to properly fill the output icon. As we are drawing into a command list,
            // this scaling preserves full vector fidelity regardless of resolution. Neato!

            ds.DrawText("ಠ⌣ಠ", 0, 0, Colors.White);
        }


        // For CoreWindowExample, we draw the same simple graphics as the sample itself.
        static void DrawSimpleSampleIcon(CanvasDrawingSession ds, IconInfo iconInfo)
        {
            ds.DrawEllipse(155, 115, 80, 30, Colors.Black, 3);
            ds.DrawText("Hello, world!", 100, 100, Colors.Yellow);
        }
    }
}
