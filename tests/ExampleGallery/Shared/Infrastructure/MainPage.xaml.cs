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

#if WINDOWS_UAP
            //
            // TODO: remove this once 1584010 is resolved "Infinite layout cycle with unconstrained ScrollViewer
            //
            this.scrollViewer.VerticalScrollBarVisibility = ScrollBarVisibility.Hidden;
#endif

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
