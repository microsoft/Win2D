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
    public class FontPickerComboBox : UserControl
    {
        ComboBox comboBox = new ComboBox();

        public FontPickerComboBox()
        {
            var fontFamilyNames = CanvasTextFormat.GetSystemFontFamilies(ApplicationLanguages.Languages).OrderBy(k => k);

            foreach (string fontFamilyName in fontFamilyNames)
            {
                ComboBoxItem item = new ComboBoxItem();
                item.Content = fontFamilyName;
                item.FontFamily = new FontFamily(fontFamilyName);
                comboBox.Items.Add(item);
            }

            comboBox.SelectionChanged += ComboBox_SelectionChanged;

            Content = comboBox;

            SelectDefaultFont();
        }

        public event SelectionChangedEventHandler SelectionChanged
        {
            add { comboBox.SelectionChanged += value;}
            remove { comboBox.SelectionChanged -= value; }
        }

        private void SelectDefaultFont()
        {
            SelectFont("Arial");
        }

        public void SelectFont(string name)
        {
            for (int i = 0; i < comboBox.Items.Count; ++i)
            {
                ComboBoxItem item = comboBox.Items[i] as ComboBoxItem;
                if ((item.Content as string) == name)
                {
                    comboBox.SelectedIndex = i;
                    return;
                }
            }
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Updates the style of the currently-displayed item to reflect the font.
            FontFamily = (comboBox.SelectedItem as ComboBoxItem).FontFamily;
        }

        public string CurrentFontFamily { get { return (comboBox.SelectedItem as ComboBoxItem).Content as string; } }
    }
}
