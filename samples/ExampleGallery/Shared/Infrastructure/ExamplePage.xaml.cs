// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
