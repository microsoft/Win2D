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
using Windows.Media.Editing;
using Windows.Media.Effects;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class BasicVideoEffectExample : UserControl
    {
        public BasicVideoEffectExample()
        {
            this.InitializeComponent();
        }

        async void OnLoaded(object sender, RoutedEventArgs e)
        {
            var thumbnailFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Assets/Logo.scale-100.png"));
            var thumbnail = RandomAccessStreamReference.CreateFromFile(thumbnailFile);

            var url = "http://video.ch9.ms/ch9/4597/8db5a656-b173-4897-b2aa-e2075fb24597/windows10recap.mp4";
            var file = await StorageFile.CreateStreamedFileFromUriAsync(
                "windows10recap.mp4",
                new Uri(url),
                thumbnail);

            var clip = await MediaClip.CreateFromFileAsync(file);
            clip.VideoEffectDefinitions.Add(new VideoEffectDefinition(typeof(ExampleVideoEffect).FullName));

            var composition = new MediaComposition();
            composition.Clips.Add(clip);

            mediaElement.SetMediaStreamSource(composition.GenerateMediaStreamSource());
            mediaElement.IsLooping = true;
        }
    }
}
