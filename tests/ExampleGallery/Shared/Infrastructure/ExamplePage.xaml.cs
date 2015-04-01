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
using Windows.UI.Xaml.Navigation;

namespace ExampleGallery
{
    public sealed partial class ExamplePage : Page
    {
        private NavigationHelper navigationHelper;

        public ExamplePage()
        {
            this.InitializeComponent();         

            this.navigationHelper = new NavigationHelper(this);

            if (Windows.ApplicationModel.DesignMode.DesignModeEnabled)
            {
                this.DataContext = new ExampleDefinition("An Example", null);
            }

            if (this.navigationHelper.HasHardwareButtons)
            {
                this.backButton.Visibility = Visibility.Collapsed;
            }
        }

        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {            
            this.navigationHelper.OnNavigatedTo(e);

            var example = e.Parameter as ExampleDefinition;
            if (example != null)
            {
                this.DataContext = example;
                if (example.Control != null)
                {
                    var control = Activator.CreateInstance(example.Control);
                    this.exampleContent.Children.Add((UIElement)control);
                    DeveloperTools.ExampleControlCreated(example.Name, (UserControl)control);
                }                
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }
    }
}
