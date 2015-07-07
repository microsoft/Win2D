// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;

namespace ExampleGallery
{
    public sealed partial class About : UserControl
    {
        public About()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                // Thumbnail icons use a much simpler/shorter version of the text.
                textBlock.Inlines.Clear();
                textBlock.Inlines.Add(new Hyperlink { Inlines = { new Run { Text = "github.com/Microsoft/Win2D" } } });
            }
        }
    }
}
