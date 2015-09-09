// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
using Windows.UI.Xaml.Controls.Primitives;

namespace ExampleGallery
{
    public sealed partial class CameraEffectExample : UserControl, ICustomThumbnailSource
    {
        private MediaCapture mediaCapture;
        private TaskCompletionSource<object> hasLoaded = new TaskCompletionSource<object>();
        private Task changeEffectTask = null;
        private IPropertySet effectPropertySet = null; //defined in class scope so that Slider_ValueChanged can modify values

        const string noEffect = "No effect";
        const string displacementEffect = "Displacement effect";
        const string rotatingTilesEffect = "Rotating tiles effect";
        const string gaussianBlurEffect = "Gaussian Blur effect";
        
        public List<string> PossibleEffects
        {
            get
            {
                return new List<string> { noEffect, displacementEffect, rotatingTilesEffect, gaussianBlurEffect };
            }
        }

        public CameraEffectExample()
        {
            this.InitializeComponent();
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            var action = mediaCapture.StopPreviewAsync();
            mediaCapture.Failed -= mediaCapture_Failed;
        }

        private void mediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            var action = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                progressText.Text = "MediaCapture failed: " + errorEventArgs.Message;
            });
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.effectCombo.SelectedItem = noEffect;
            this.captureElement.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = true;

            await CreateMediaCapture();

            if (ThumbnailGenerator.IsDrawingThumbnail && captureElement.Source != null)
            {
                await SetEffectWorker(rotatingTilesEffect);
                customThumbnail = new InMemoryRandomAccessStream();
                await mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), customThumbnail);
            }

            this.captureElement.Visibility = Visibility.Visible;
            this.progressRing.IsActive = false;
            this.progressRing.Visibility = Visibility.Collapsed;

            hasLoaded.SetResult(null);
        }

        private async Task CreateMediaCapture()
        {
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
                this.progressText.Text = "No camera is available.";
                return;
            }

            captureElement.Source = mediaCapture;
            await mediaCapture.StartPreviewAsync();
        }

        private async Task ResetEffectsAsync()
        {
            if (mediaCapture.CameraStreamState == CameraStreamState.Streaming)
            {
                await mediaCapture.ClearEffectsAsync(MediaStreamType.VideoPreview);
            }
        }

        private void EffectCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string effect = noEffect;

            if (e.AddedItems.Count != 0)
                effect = (string)e.AddedItems[0];

            var task = SetEffect(effect);
        }

        private void BlurAmountSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (effectPropertySet == null) return;
            effectPropertySet["BlurAmount"] = e.NewValue;
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

            blurAmountSlider.Visibility = Visibility.Collapsed;
            effectPropertySet = new PropertySet();
            string typeName = null;

            switch (effect)
            {
                case displacementEffect:
                    typeName = typeof(DisplacementEffect).FullName;
                    break;
                case rotatingTilesEffect:
                    typeName = typeof(RotatedTilesEffect).FullName;
                    break;
                case gaussianBlurEffect:
                    typeName = typeof(DynamicBlurVideoEffect).FullName;
                    effectPropertySet["BlurAmount"] = blurAmountSlider.Value;
                    blurAmountSlider.Visibility = Visibility.Visible;
                    break;
            }

            if (typeName == null)
                return;

            await mediaCapture.AddVideoEffectAsync(new VideoEffectDefinition(typeName, effectPropertySet),
                MediaStreamType.VideoPreview);
        }

        // This example generates a custom thumbnail image (not just a rendering capture like most examples).
        IRandomAccessStream ICustomThumbnailSource.Thumbnail { get { return customThumbnail; } }
        IRandomAccessStream customThumbnail;

    }
}
