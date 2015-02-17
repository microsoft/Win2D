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

using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class DebuggingOptions : UserControl
    {
        static DispatcherTimer timer;

        public DebuggingOptions()
        {
            this.InitializeComponent();

            gcButton.IsChecked = timer != null && timer.IsEnabled;
        }

        void gcButton_Checked(object sender, RoutedEventArgs e)
        {
            if (timer == null)
            {
                timer = new DispatcherTimer();
                timer.Interval = TimeSpan.FromSeconds(1);
                timer.Tick += timer_Tick;
            }

            timer.Start();
        }

        void gcButton_Unchecked(object sender, RoutedEventArgs e)
        {
            timer.Stop();
        }

        void timer_Tick(object sender, object e)
        {
            GC.Collect();
        }

        void exitButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Exit();
        }
    }
}
