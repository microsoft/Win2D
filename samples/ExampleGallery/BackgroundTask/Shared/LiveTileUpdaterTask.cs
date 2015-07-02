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
using Microsoft.Graphics.Canvas.Text;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Notifications;

namespace ExampleGallery.BackgroundTask
{
    //
    // The actual background task.
    //
    public sealed class LiveTileUpdaterTask : IBackgroundTask
    {
        Dictionary<TileTemplateType, string> tiles = new Dictionary<TileTemplateType, string>();
        CanvasDevice device;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            using (device = new CanvasDevice())
            {
                try
                {
                    GenerateTileImage(TileTemplateType.TileSquare150x150Image, 150, 150);
                    GenerateTileImage(TileTemplateType.TileWide310x150Image, 310, 150);
                }
                catch (Exception e)
                {
                    if (device.IsDeviceLost(e.HResult))
                    {
                        // When the device is lost we don't attempt to retry, and instead just wait for
                        // the next time the task runs.
                        return;
                    }

                    // Any other errors we bubble up
                    throw;
                }
            }

            UpdateLiveTiles();
            DeleteOldLiveTileImageFiles();
            LiveTileUpdater.SetLatestTileImagesInSettings(tiles.Values);
        }

        void GenerateTileImage(TileTemplateType templateType, float width, float height)
        {
            tiles.Add(templateType, GenerateAndSaveTileImage(device, width, height).Result);
        }

        void UpdateLiveTiles()
        {
            var updater = TileUpdateManager.CreateTileUpdaterForApplication();

            XmlDocument tileXml = null;

            foreach (var tile in tiles)
            {
                var thisTileXml = GenerateTileXml(tile.Value, tile.Key);

                if (tileXml == null)
                {
                    tileXml = thisTileXml;
                }
                else
                {
                    // merge this tile in to tileXml
                    var node = tileXml.ImportNode(thisTileXml.GetElementsByTagName("binding").Item(0), true);
                    tileXml.GetElementsByTagName("visual").Item(0).AppendChild(node);
                }
            }

            updater.Update(new TileNotification(tileXml));
        }

        static XmlDocument GenerateTileXml(string imageFileName, TileTemplateType template)
        {
            var tileXml = TileUpdateManager.GetTemplateContent(template);

            var images = tileXml.GetElementsByTagName("image");

            var src = tileXml.CreateAttribute("src");
            src.Value = "ms-appdata:///local/" + imageFileName;
            images[0].Attributes.SetNamedItem(src);

            return tileXml;
        }

        void DeleteOldLiveTileImageFiles()
        {
            var folder = Windows.Storage.ApplicationData.Current.LocalFolder;

            var files = folder.GetFilesAsync().AsTask().Result;

            foreach (var file in files)
            {
                if (!ShouldKeep(file.Name))
                {
                    var ignoredTask = file.DeleteAsync();
                }
            }
        }

        bool ShouldKeep(string fileName)
        {
            if (!fileName.EndsWith(".png"))
                return true;

            return tiles.Values.Any(v => fileName.Contains(v));
        }

        static async Task<string> GenerateAndSaveTileImage(CanvasDevice device, float width, float height)
        {
            using (var renderTarget = new CanvasRenderTarget(device, width, height, 96))
            {
                using (var ds = renderTarget.CreateDrawingSession())
                {
                    DrawTile(ds, width, height);
                }

                var filename = string.Format("tile-{0}x{1}-{2}.png",
                                (int)width,
                                (int)height,
                                Guid.NewGuid().ToString());

                await renderTarget.SaveAsync(Path.Combine(ApplicationData.Current.LocalFolder.Path, filename));

                return filename;
            }
        }

        static void DrawTile(CanvasDrawingSession ds, float width, float height)
        {
            using (var cl = new CanvasCommandList(ds))
            {
                using (var clds = cl.CreateDrawingSession())
                {
                    var text = string.Format("{0}\n{1}", DateTime.Now.ToString("ddd"), DateTime.Now.ToString("HH:mm"));

                    var textFormat = new CanvasTextFormat()
                    {
                        FontFamily = "Segoe UI Black",
                        HorizontalAlignment = CanvasHorizontalAlignment.Right,
                        VerticalAlignment = CanvasVerticalAlignment.Center,
                        FontSize = 20,
                        LineSpacing = 20
                    };

                    clds.DrawText(text, 0, 0, Colors.White, textFormat);
                }

                var effect = new GaussianBlurEffect()
                {
                    Source = cl,
                    BlurAmount = 1,
                };

                ds.Clear(Colors.Orange);

                var bounds = effect.GetBounds(ds);
                var ratio = bounds.Height / bounds.Width;
                var destHeight = height * ratio;

                ds.DrawImage(effect, new Rect(0, height / 2 - destHeight / 2, width, destHeight), bounds);

                ds.DrawText(string.Format("Generated by Win2D\n{0}\n{1}", DateTime.Now.ToString("d"), DateTime.Now.ToString("t")),
                    12, 12, Colors.Black,
                    new CanvasTextFormat()
                    {
                        HorizontalAlignment = CanvasHorizontalAlignment.Left,
                        VerticalAlignment = CanvasVerticalAlignment.Top,
                        FontSize = 12
                    });
            }
        }
    }
}
