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
using System.Collections.Generic;
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
        private TaskCompletionSource<object> hasLoaded = new TaskCompletionSource<object>();
        private Task changeEffectTask = null;

        const string noEffect = "No effect";
        const string displacementEffect = "Displacement effect";
        const string rotatingTilesEffect = "Rotating tiles effect";

        public List<string> PossibleEffects
        {
            get
            {
                return new List<string> { noEffect, displacementEffect, rotatingTilesEffect };
            }
        }

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

        private async void mediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                progressText.Text = "MediaCapture failed: " + errorEventArgs.Message;
                progressText.Visibility = Visibility.Visible;
            });
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.effectCombo.SelectedItem = noEffect;
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
                await SetEffectWorker(rotatingTilesEffect);
                var stream = new InMemoryRandomAccessStream();
                await mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream);
                thumbnail = stream;
            }

            this.captureElement.Visibility = Visibility.Visible;
            this.progressRing.IsActive = false;
            this.progressRing.Visibility = Visibility.Collapsed;

            hasLoaded.SetResult(null);
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

        private async void EffectCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string effect = noEffect;

            if (e.AddedItems.Count != 0)
                effect = (string)e.AddedItems[0];

            await SetEffect(effect);
        }

        private async Task SetEffect(string effect)
        {
            // wait for OnLoaded to complete
            await hasLoaded.Task;

            // wait for any previous SetEffect to finish
            if (changeEffectTask != null)
            {
                await changeEffectTask;
            }

            await (changeEffectTask = SetEffectWorker(effect));
        }

        private async Task SetEffectWorker(string effect)
        {
            await ResetEffectsAsync();

            string typeName = null;

            switch (effect)
            {
                case displacementEffect: typeName = typeof(DisplacementEffect).FullName; break;
                case rotatingTilesEffect: typeName = typeof(RotatedTilesEffect).FullName; break;
            }

            if (typeName == null)
                return;

            await mediaCapture.AddVideoEffectAsync(
                new VideoEffectDefinition(typeName, effectConfiguration),
                MediaStreamType.VideoPreview);
        }
    }
}
