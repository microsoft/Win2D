// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ExampleGallery
{
    public sealed partial class MainPage : Page
    {
        private NavigationHelper navigationHelper;

        // We track the last visual state we set to avoid redundant GoToState calls
        // (these cause flickering on 8.1 apps running on Win10).
        private enum VisualState { Unknown, Big, Small };
        private VisualState currentVisualState = VisualState.Unknown;

        public MainPage()
        {
            this.InitializeComponent();

            this.navigationHelper = new NavigationHelper(this);
        }

        private void GridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var example = (ExampleDefinition)e.ClickedItem;
            this.Frame.Navigate(typeof(ExamplePage), example);
        }

        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            // How much room do we have?
            double gridMargin = 20;
            double gridWidth = e.NewSize.Width - gridMargin;

            // How much do we allow for each item?
            const double itemWidth = 330;

            // We want at least 3 across
            if (itemWidth * 3 <= gridWidth)
            {
                if (currentVisualState != VisualState.Big)
                {
                    VisualStateManager.GoToState(this, "BigScreen", false);
                    currentVisualState = VisualState.Big;
                }
            }
            else
            {
                if (currentVisualState != VisualState.Small)
                {
                    VisualStateManager.GoToState(this, "SmallScreen", false);
                    currentVisualState = VisualState.Small;
                }
            }
        }
    }
}
