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

using ExampleGallery.Effects;
using System;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.Effects;
using Windows.Media.MediaProperties;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class CameraEffectExample : UserControl, ICustomThumbnailSource
    {
        private IPropertySet effectConfiguration;
        private MediaCapture mediaCapture;
        private IRandomAccessStream thumbnail;

        public CameraEffectExample()
        {
            this.InitializeComponent();
        }

        private async void OnUnloaded(object sender, RoutedEventArgs e)
        {
            try
            { 
                await mediaCapture.StopPreviewAsync();
            }
            catch (Exception)
            {
                // Any errors indicate that camera capture was already not active.
            }
        }

        private void DisableButtons()
        {
            ResetButton.IsEnabled = false;
            DisplacementButton.IsEnabled = false;
            TilesButton.IsEnabled = false;
        }

        private void EnableButtons()
        {
            ResetButton.IsEnabled = true;
            DisplacementButton.IsEnabled = true;
            TilesButton.IsEnabled = true;
        }

        private async void mediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DisableButtons();
            });
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            DisableButtons();

            this.captureElement.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = true;

            effectConfiguration = new PropertySet();
            mediaCapture = new MediaCapture();
            mediaCapture.Failed += mediaCapture_Failed;

            var settings = new MediaCaptureInitializationSettings()
            {
                StreamingCaptureMode = StreamingCaptureMode.Video
            };

            try
            {
                await mediaCapture.InitializeAsync(settings);
            }
            catch (Exception)
            {
                this.progressRing.IsActive = false;
                this.progressRing.Visibility = Visibility.Collapsed;
                this.progressText.Text = "No camera is available.";

                return;
            }

            captureElement.Source = mediaCapture;
            await mediaCapture.StartPreviewAsync();

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                await StartTilesEffectAsync();
                var stream = new InMemoryRandomAccessStream();
                await mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream);
                thumbnail = stream;
            }

            EnableButtons();

            this.captureElement.Visibility = Visibility.Visible;
            this.progressRing.IsActive = false;
            this.progressRing.Visibility = Visibility.Collapsed;
        }

        private async Task ResetEffectsAsync()
        {
            if (mediaCapture.CameraStreamState == CameraStreamState.Streaming)
            {
                await mediaCapture.ClearEffectsAsync(MediaStreamType.VideoPreview);
            }
        }

        IRandomAccessStream ICustomThumbnailSource.Thumbnail
        {
            get { return thumbnail; }
        }

        private async void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            DisableButtons();

            await ResetEffectsAsync();

            EnableButtons();
        }

        private async void DisplacementButton_Click(object sender, RoutedEventArgs e)
        {
            DisableButtons();

            await ResetEffectsAsync();

            await mediaCapture.AddVideoEffectAsync(
                new VideoEffectDefinition(
                    typeof(DisplacementEffect).FullName,
                    effectConfiguration
                    ),
                MediaStreamType.VideoPreview
                );

            EnableButtons();
        }

        private async void TilesButton_Click(object sender, RoutedEventArgs e)
        {
            DisableButtons();
            await StartTilesEffectAsync();
            EnableButtons();
        }

        // This is a separate method to allow it to be called by the thumbnail generator.
        private async Task StartTilesEffectAsync()
        {
            await ResetEffectsAsync();

            await mediaCapture.AddVideoEffectAsync(
                new VideoEffectDefinition(
                    typeof(RotatedTilesEffect).FullName,
                    effectConfiguration
                    ),
                MediaStreamType.VideoPreview
                );
        }
    }
}
