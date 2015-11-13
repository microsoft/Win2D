// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas.Text;
using System.Linq;
using Windows.Globalization;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public class FontPickerComboBox : ComboBox
    {
        public FontPickerComboBox()
        {
            var fontFamilyNames = CanvasTextFormat.GetSystemFontFamilies(ApplicationLanguages.Languages).OrderBy(k => k);

            foreach (string fontFamilyName in fontFamilyNames)
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Content = fontFamilyName;
                item.FontFamily = new FontFamily(fontFamilyName);
                this.Items.Add(item);
            }
            this.SelectedIndex = 0;

            this.SelectionChanged += FontPickerComboBox_SelectionChanged;
        }

        public void SelectDefaultFont()
        {
            SelectFont("Arial");
        }

        public void SelectFont(string name)
        {
            for (int i = 0; i < this.Items.Count; ++i)
            {
                ComboBoxItem item = this.Items[i] as ComboBoxItem;
                if ((item.Content as string) == name)
                {
                    this.SelectedIndex = i;
                    return;
                }
            }
        }

        void FontPickerComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Updates the style of the currently-displayed item to reflect the font.
            this.FontFamily = (this.SelectedItem as ComboBoxItem).FontFamily;
        }

        public string CurrentFontFamily { get { return (this.SelectedItem as ComboBoxItem).Content as string; } }
    }
}
