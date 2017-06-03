// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml;
using Windows.UI;

namespace ExampleGallery
{
    class ColorPicker : UserControl
    {
        Grid grid = new Grid();

        Button[] colorButtons = new Button[8];

        List<ColorChangedHandler> handlers = new List<ColorChangedHandler>();

        Color currentColor;

        public ColorPicker()
        {
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition());

            grid.ColumnDefinitions.Add(new ColumnDefinition());
            grid.ColumnDefinitions.Add(new ColumnDefinition());

            grid.Height = 80;

            Color[] colors = new Color[] {
                Colors.White,
                Colors.Green,
                Colors.Yellow,
                Colors.DarkRed,
                Colors.CornflowerBlue,
                Colors.OrangeRed,
                Colors.Silver,
                Colors.Navy
            };

            for (int i=0; i<8; ++i)
            {
                int row = i / 2;
                int column = i % 2;

                colorButtons[i] = new Button();
                colorButtons[i].SetValue(Grid.RowProperty, row);
                colorButtons[i].SetValue(Grid.ColumnProperty, column);

                colorButtons[i].Background = new SolidColorBrush(colors[i]);
                colorButtons[i].VerticalAlignment = Windows.UI.Xaml.VerticalAlignment.Stretch;
                colorButtons[i].HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Stretch;
                colorButtons[i].Click += ColorPicker_Click;

                grid.Children.Add(colorButtons[i]);
            }

            currentColor = colors[0];

            Content = grid;
        }

        void ResetColorSelectors()
        {
            foreach (Button button in colorButtons)
            {
                button.BorderBrush = null;
                button.BorderThickness = new Thickness(0);
            }
        }

        private void ColorPicker_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            ResetColorSelectors();

            var button = sender as Button;
            button.BorderBrush = new SolidColorBrush(Colors.Red);
            button.BorderThickness = new Thickness(3);

            currentColor = (button.Background as SolidColorBrush).Color;

            foreach (var handler in handlers)
            {
                handler.Invoke();
            }
        }

        public Color CurrentColor { get { return currentColor; } }

        public delegate void ColorChangedHandler();

        public event ColorChangedHandler SelectionChanged
        {
            add { handlers.Add(value); }
            remove { handlers.Remove(value); }
        }
    }

}
