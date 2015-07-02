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

using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class CustomControlExample : UserControl
    {
        public CustomControlExample()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                Content = new GlowTextCustomControl()
                {
                    Text = "Glow",
                    GlowAmount = 30,
                    GlowColor = Colors.Yellow,
                    TextColor = Colors.White,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    VerticalAlignment = VerticalAlignment.Center
                };
            }
        }

        private void UserControl_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (e.NewSize.Width > e.NewSize.Height)
                demoPanel.Orientation = Orientation.Horizontal;
            else
                demoPanel.Orientation = Orientation.Vertical;
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (ThumbnailGenerator.IsDrawingThumbnail)
                return;

            Pulse.Begin();
        }
    }
}
