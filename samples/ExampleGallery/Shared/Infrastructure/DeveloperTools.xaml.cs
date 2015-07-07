// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class DeveloperTools : UserControl
    {
        static DispatcherTimer timer;
        static List<KeyValuePair<string, WeakReference<UserControl>>> exampleControls = new List<KeyValuePair<string,WeakReference<UserControl>>>();
        static WeakReference<DeveloperTools> activePage;

        public DeveloperTools()
        {
            this.InitializeComponent();

            activePage = new WeakReference<DeveloperTools>(this);

            gcButton.IsChecked = timer != null && timer.IsEnabled;
        }

        async void thumbnailsButton_Click(object sender, RoutedEventArgs e)
        {
            var outputFolder = await PickOutputFolder("Generate Thumbnails");

            if (outputFolder != null)
            {
                panel.Children.Add(new ThumbnailGenerator(outputFolder));
            }
        }

        async void iconsButton_Click(object sender, RoutedEventArgs e)
        {
            var outputFolder = await PickOutputFolder("Generate App Icons");

            if (outputFolder != null)
            {
                var generator = new AppIconGenerator(outputFolder);

                await generator.GenerateIcons();
            }
        }

        async Task<StorageFolder> PickOutputFolder(string title)
        {
#if WINDOWS_PHONE_APP
            await new MessageDialog("Not supported on Windows Phone. Please run on a Windows PC instead.").ShowAsync();

            return null;
#else
            var folderPicker = new FolderPicker
            {
                CommitButtonText = title,
                SuggestedStartLocation = PickerLocationId.Desktop,
                FileTypeFilter = { ".png" },
            };

            return await folderPicker.PickSingleFolderAsync();
#endif
        }

        void gcButton_Checked(object sender, RoutedEventArgs e)
        {
            if (timer == null)
            {
                timer = new DispatcherTimer();
                timer.Interval = TimeSpan.FromSeconds(0.1);
                timer.Tick += timer_Tick;
            }

            timer.Start();
        }
        
        void gcButton_Unchecked(object sender, RoutedEventArgs e)
        {
            timer.Stop();
        }

        static void timer_Tick(object sender, object e)
        {
            GC.Collect();

            DeveloperTools thePage;
            if (activePage.TryGetTarget(out thePage))
            {
                thePage.GenerateLeakReport();
            }
        }

        void exitButton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Exit();
        }

        static public void ExampleControlCreated(string name, UserControl control)
        {
            if (control.GetType() == typeof(DeveloperTools))
                return;

            exampleControls.Add(new KeyValuePair<string, WeakReference<UserControl>>(name, new WeakReference<UserControl>(control)));
        }

        private void control_Loaded(object sender, RoutedEventArgs e)
        {
            GenerateLeakReport();
        }

        private void GenerateLeakReport()
        {
            var leakString = new StringBuilder();
            foreach (var entry in exampleControls)
            {
                UserControl theControl = null;

                bool referenceExists = entry.Value.TryGetTarget(out theControl);

                leakString.AppendFormat("{1}\t{0}\n", entry.Key, referenceExists ? "live    " : "collected");
            }
            leakText.Text = leakString.ToString();
        }

        private void resetLeakTracking_Click(object sender, RoutedEventArgs e)
        {
            exampleControls.Clear();
            GenerateLeakReport();
        }

        private async void checkLeaks_Click(object sender, RoutedEventArgs e)
        {
            var leakCheckFrame = new Frame();
            leakCheckFrame.HorizontalContentAlignment = Windows.UI.Xaml.HorizontalAlignment.Stretch;
            leakCheckFrame.Height = 500;

            panel.Children.Add(leakCheckFrame);

            leakCheckFrame.Visibility = Visibility.Visible;
            checkLeaksButton.IsEnabled = false;

            foreach (var example in ExampleDefinitions.Definitions)
            {
                if (example.Control == this.GetType())
                    continue;
                
                leakCheckFrame.Navigate(typeof(ExamplePage), example);
                await Task.Delay(1000);
                GenerateLeakReport();
            }

            panel.Children.Remove(leakCheckFrame);

            checkLeaksButton.IsEnabled = true;
        }
    }
}
