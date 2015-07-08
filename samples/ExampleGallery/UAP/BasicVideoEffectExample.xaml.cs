// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using ExampleGallery.Effects;
using System;
using System.Net.Http;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Media.Editing;
using Windows.Media.Effects;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class BasicVideoEffectExample : UserControl, ICustomThumbnailSource
    {
        public BasicVideoEffectExample()
        {
            this.InitializeComponent();
        }

        async void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.mediaElement.Visibility = Visibility.Collapsed;
            this.progressInfo.Visibility = Visibility.Visible;
            this.progressRing.IsActive = true;
            this.progressText.Text = "Downloading video...";

            var thumbnailFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/Logo.scale-100.png"));
            var thumbnail = RandomAccessStreamReference.CreateFromFile(thumbnailFile);

            var url = "http://video.ch9.ms/ch9/4597/8db5a656-b173-4897-b2aa-e2075fb24597/windows10recap.mp4";

            // TODO: replace TemporaryFolder and HttpClient with StorageFile.CreateStreamedFileFromUriAsync once TH:2458060 is fixed
            //var file = await StorageFile.CreateStreamedFileFromUriAsync(
            //    "windows10recap.mp4",
            //    new Uri(url),
            //    thumbnail);

            var file = await ApplicationData.Current.TemporaryFolder.CreateFileAsync("windows10recap.mp4", CreationCollisionOption.ReplaceExisting);

            using (var httpClient = new HttpClient())
            {
                byte[] videoData = await httpClient.GetByteArrayAsync(url);

                using (var writer = await file.OpenAsync(FileAccessMode.ReadWrite))
                {
                    await writer.WriteAsync(videoData.AsBuffer());
                }
            }

            this.progressText.Text = "Creating clip...";
            var clip = await MediaClip.CreateFromFileAsync(file);
            clip.VideoEffectDefinitions.Add(new VideoEffectDefinition(typeof(ExampleVideoEffect).FullName));

            var composition = new MediaComposition();
            composition.Clips.Add(clip);

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                customThumbnail = await composition.GetThumbnailAsync(TimeSpan.FromSeconds(10), 1280, 720, VideoFramePrecision.NearestFrame);
            }

            mediaElement.SetMediaStreamSource(composition.GenerateMediaStreamSource());
            mediaElement.IsLooping = true;
        
            this.mediaElement.Visibility = Visibility.Visible;
            this.progressInfo.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = false;
        }

        // This example generates a custom thumbnail image (not just a rendering capture like most examples).
        IRandomAccessStream ICustomThumbnailSource.Thumbnail { get { return customThumbnail; } }
        IRandomAccessStream customThumbnail;
    }
}
