// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class VirtualBitmapExample : UserControl, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public VirtualBitmapExample()
        {
            this.InitializeComponent();

            if (!DesignMode.DesignModeEnabled)
                DataContext = this;
        }

        public bool IsImageLoaded { get { return virtualBitmap != null; } }
        public bool AreCacheMethodsAvailable { get { return IsImageLoaded && virtualBitmapOptions == CanvasVirtualBitmapOptions.CacheOnDemand; } }
        public bool IsEnsureCachedInProgress { get; private set; }

        public string LoadedImageInfo { get; private set; }

        bool smallView;
        public bool SmallView
        {
            get
            {
                return smallView;
            }
            set
            {
                if (smallView != value)
                {
                    smallView = value;
                    Control_SizeChanged(null, null);
                }
            }
        }

        ByteCounterStreamProxy imageStream;
        CanvasVirtualBitmap virtualBitmap;
        CanvasVirtualBitmapOptions virtualBitmapOptions;

        List<int> bytesRead = new List<int>();
        float flash = 0;
        int drawCount;

        private void Control_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (smallView)
            {
                ImageScrollViewer.MaxWidth = ActualWidth / 4;
                ImageScrollViewer.MaxHeight = ActualHeight / 4;
            }
            else
            {
                ImageScrollViewer.MaxWidth = double.MaxValue;
                ImageScrollViewer.MaxHeight = double.MaxValue;
            }
        }


        private void ImageVirtualControl_RegionsInvalidated(CanvasVirtualControl sender, CanvasRegionsInvalidatedEventArgs args)
        {
            if (IsEnsureCachedInProgress)
                return;

            foreach (var region in args.InvalidatedRegions)
            {
                using (var ds = ImageVirtualControl.CreateDrawingSession(region))
                {
                    if (virtualBitmap != null)
                        ds.DrawImage(virtualBitmap, region, region);
                }
            }
        }

        
        private async void OnOpenClicked(object sender, RoutedEventArgs e)
        {
            await Open(CanvasVirtualBitmapOptions.None);
        }


        private async void OnOpenCacheOnDemandClicked(object sender, RoutedEventArgs e)
        {
            await Open(CanvasVirtualBitmapOptions.CacheOnDemand);
        }


        private async Task Open(CanvasVirtualBitmapOptions options)
        {
            var filePicker = new FileOpenPicker();
            filePicker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            filePicker.FileTypeFilter.Add("*");

            var file = await filePicker.PickSingleFileAsync();

            if (file == null)
                return;

            if (imageStream != null)
            {
                imageStream.Dispose();
                imageStream = null;
            }

            imageStream = new ByteCounterStreamProxy(await file.OpenReadAsync());
            virtualBitmapOptions = options;

            IOGraph.Invalidate();
            await LoadVirtualBitmap();
        }


        private void ImageVirtualControl_CreateResources(CanvasVirtualControl sender, Microsoft.Graphics.Canvas.UI.CanvasCreateResourcesEventArgs args)
        {
            if (imageStream != null)
            {
                args.TrackAsyncAction(LoadVirtualBitmap().AsAsyncAction());
            }
        }


        private async Task LoadVirtualBitmap()
        {
            if (virtualBitmap != null)
            {
                virtualBitmap.Dispose();
                virtualBitmap = null;
            }

            LoadedImageInfo = "";

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("LoadedImageInfo"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsImageLoaded"));
            }

            virtualBitmap = await CanvasVirtualBitmap.LoadAsync(ImageVirtualControl.Device, imageStream, virtualBitmapOptions);

            if (ImageVirtualControl == null)
            {
                // This can happen if the page is unloaded while LoadAsync is running
                return;
            }

            var size = virtualBitmap.Size;
            ImageVirtualControl.Width = size.Width;
            ImageVirtualControl.Height = size.Height;
            ImageVirtualControl.Invalidate();

            LoadedImageInfo = string.Format("{0}x{1} image, is {2}CachedOnDemand",
                size.Width, size.Height, virtualBitmap.IsCachedOnDemand ? "" : "not ");

            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("LoadedImageInfo"));
                PropertyChanged(this, new PropertyChangedEventArgs("IsImageLoaded"));
                PropertyChanged(this, new PropertyChangedEventArgs("AreCacheMethodsAvailable"));
            }
        }


        private async void OnEnsureCachedClicked(object sender, RoutedEventArgs e)
        {
            if (IsEnsureCachedInProgress)
                return;

            if (virtualBitmap == null)
                return;

            IsEnsureCachedInProgress = true;
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("IsEnsureCachedInProgress"));

            await virtualBitmap.EnsureCachedAsync();

            IsEnsureCachedInProgress = false;
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs("IsEnsureCachedInProgress"));
        }


        private void OnTrimCacheClicked(object sender, RoutedEventArgs e)
        {
            if (virtualBitmap == null)
                return;

            virtualBitmap.TrimCache();
        }


        private void OnIOGraphDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var mostRecentBytesRead = imageStream == null ? 0 : imageStream.GetBytesRead();

            bytesRead.Add(mostRecentBytesRead);

            // Flash the control red when there's some IO
            if (mostRecentBytesRead != 0)
                flash = 1;
            else
                flash *= 0.95f;

            ds.Clear(new Vector4(flash, 0, 0, 1));

            ds.DrawText("Bytes read", new Rect(0, 0, sender.ActualWidth, sender.ActualHeight), Colors.DarkRed,
                new CanvasTextFormat()
                {
                    VerticalAlignment = CanvasVerticalAlignment.Center,
                    HorizontalAlignment = CanvasHorizontalAlignment.Left,
                    FontSize = 12
                });


            int maxBytesRead = bytesRead.Max();
            maxBytesRead = Math.Max(1, maxBytesRead);

            int displayCount = 120;

            // Draw a moving vertical tick to allow us to see that the graph is
            // updating even if nothing is being read.
            drawCount = (drawCount + 1) % displayCount;
            var vx = (1.0f - (float)drawCount / (float)displayCount) * (float)sender.ActualWidth;
            ds.DrawLine(vx, (float)sender.ActualHeight - 5, vx, (float)sender.ActualHeight, Colors.Gray);


            // Draw the graph
            if (bytesRead.Count > 2)
            {
                var p = new CanvasPathBuilder(sender);
                for (int i = 0; i < Math.Min(displayCount, bytesRead.Count); ++i)
                {
                    var x = ((float)i / (float)displayCount) * (float)sender.ActualWidth;
                    var y = (float)sender.ActualHeight - ((float)bytesRead[i] / (float)maxBytesRead) * (float)sender.ActualHeight;

                    if (i == 0)
                        p.BeginFigure(x, y);
                    else
                        p.AddLine(x, y);
                }
                p.EndFigure(CanvasFigureLoop.Open);

                using (var g = CanvasGeometry.CreatePath(p))
                {
                    ds.DrawGeometry(g, Colors.White, 3, new CanvasStrokeStyle()
                    {
                        LineJoin = CanvasLineJoin.Round
                    });
                }

                int toRemove = bytesRead.Count - displayCount;
                if (toRemove > 0)
                    bytesRead.RemoveRange(0, toRemove);
            }

            sender.Invalidate();
        }


        private void Control_Unloaded(object sender, RoutedEventArgs e)
        {
            IOGraph.RemoveFromVisualTree();
            ImageVirtualControl.RemoveFromVisualTree();

            IOGraph = null;
            ImageVirtualControl = null;
        }


        //
        // This passes everything on to an underlying stream, but tracks how many bytes
        // were read.
        //
        // NOTE: this is not necessary in order to use CanvasVirtualBitmap.
        // It is only used here so that the sample is able to display when IO is taking place.
        //
        class ByteCounterStreamProxy : IRandomAccessStream
        {
            IRandomAccessStream stream;
            int bytesRead = 0;

            public ByteCounterStreamProxy(IRandomAccessStream s)
            {
                stream = s;
            }

            public int GetBytesRead()
            {
                lock (stream)
                {
                    var result = bytesRead;
                    bytesRead = 0;
                    return result;
                }
            }

            public IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
            {
                lock (stream)
                {
                    bytesRead += (int)count;
                }
                return stream.ReadAsync(buffer, count, options);
            }

            public bool CanRead { get { return stream.CanRead; } }
            public bool CanWrite { get { return stream.CanWrite; } }
            public ulong Position { get { return stream.Position; } }
            public ulong Size { get { return stream.Size; } set { stream.Size = value; } }
            public IInputStream GetInputStreamAt(ulong position) { return stream.GetInputStreamAt(position); }
            public IOutputStream GetOutputStreamAt(ulong position) { return stream.GetOutputStreamAt(position); }
            public void Seek(ulong position) { stream.Seek(position); }
            public IRandomAccessStream CloneStream() { return stream.CloneStream(); }
            public IAsyncOperationWithProgress<uint, uint> WriteAsync(IBuffer buffer) { return stream.WriteAsync(buffer); }
            public IAsyncOperation<bool> FlushAsync() { return stream.FlushAsync(); }
            public void Dispose() { stream.Dispose(); }
        }
    }
}
