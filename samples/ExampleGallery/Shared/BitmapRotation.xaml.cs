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
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class BitmapRotation : UserControl
    {
        public BitmapRotation()
        {
            this.InitializeComponent();
        }

        CanvasBitmap[] testBitmaps;

        // The suffix on the filename is the number contained in the EXIF metadata,
        // field 274 (for rotation).
        // Each of these orientations, when rendered, should produce the image (with
        // text) right side up. 
        //
        string[] fileNames =
            {
                "orientation_0.jpg",
                "orientation_1.jpg",
                "orientation_2.jpg",
                "orientation_3.jpg",
                "orientation_4.jpg",
                "orientation_5.jpg",
                "orientation_6.jpg",
                "orientation_7.jpg",
                "orientation_8.jpg",
                "orientation_9.jpg",
                "orientation_65535.jpg"
            };

        public enum BitmapSourceOption
        {
            FromFilename,
            FromStream
        }

        public List<BitmapSourceOption> BitmapSourceOptions { get { return Utils.GetEnumAsList<BitmapSourceOption>(); } }

        private BitmapSourceOption currentBitmapSourceOption;
        public BitmapSourceOption NextBitmapSourceOption { get; set; } // databinded

        async Task LoadBitmaps(CanvasControl sender)
        {
            testBitmaps = new CanvasBitmap[fileNames.Length];

            for (int i = 0; i < fileNames.Length; i++)
            {
                Package package = Package.Current;
                StorageFolder installedLocation = package.InstalledLocation;
                string pathName = installedLocation.Path + "\\" + "BitmapOrientation" + "\\" + fileNames[i];

                if (currentBitmapSourceOption == BitmapSourceOption.FromStream)
                {
                    StorageFile storageFile = await StorageFile.GetFileFromPathAsync(pathName);
                    using (IRandomAccessStreamWithContentType stream = await storageFile.OpenReadAsync())
                    {
                        testBitmaps[i] = await CanvasBitmap.LoadAsync(sender, stream);
                    }
                }
                else
                {
                    testBitmaps[i] = await CanvasBitmap.LoadAsync(sender, pathName);
                }
            }
        }

        async Task Canvas_CreateResourcesAsync(CanvasControl sender)
        {
            var folder = ApplicationData.Current.LocalFolder;

            await LoadBitmaps(sender);
        }

        void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            if (isRecreatingResources) return;

            var ds = args.DrawingSession;

            float senderSize = Math.Min((float)sender.Size.Width, (float)sender.Size.Height);
            float marginSize = senderSize / 50.0f;

            // There are 11 cells, so divide the target into a 4x3 grid (with one empty cell).
            float cellWidthInclMargin = (float)sender.Size.Width / 4.0f;
            float cellWidth = cellWidthInclMargin - marginSize;

            float cellHeightInclMargin = (float)sender.Size.Height / 3.0f;
            float cellHeight = cellHeightInclMargin - marginSize;

            const int cellsPerRow = 4;
            for(int cellY = 0; cellY < 3; ++cellY)
            {
                for (int cellX = 0; cellX < cellsPerRow; ++cellX)
                {
                    int bitmapIndex = (cellsPerRow * cellY) + cellX;
                    Rect cellRect = new Rect(cellX * cellWidthInclMargin, cellY * cellHeightInclMargin, cellWidth, cellHeight);
                    if(bitmapIndex < testBitmaps.Length)
                    {
                        ds.DrawImage(testBitmaps[bitmapIndex], cellRect);
                    }
                    else
                    {
                        // X out the cell, just to show it's not glitched out or anything.
                        ds.DrawLine((float)cellRect.Left, (float)cellRect.Top, (float)cellRect.Right, (float)cellRect.Bottom, Colors.White);
                        ds.DrawLine((float)cellRect.Right, (float)cellRect.Top, (float)cellRect.Left, (float)cellRect.Bottom, Colors.White);
                    }
                }
            }
        }

        bool isRecreatingResources = false;

        private async void BitmapSourceOption_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!canvas.ReadyToDraw) return; // CreateResources hasn't been called yet.

            if (currentBitmapSourceOption == NextBitmapSourceOption) return;
            currentBitmapSourceOption = NextBitmapSourceOption;

            //
            // If the window size changes during the execution of LoadBitmaps, the canvas control's 
            // draw handler will be called. This may seem uncommon but is especially likely in 
            // a debugging scenario. This bool keeps the draw handler from doing anything in
            // that case.
            //
            isRecreatingResources = true;

            await LoadBitmaps(canvas);

            isRecreatingResources = false;

            canvas.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected.
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
