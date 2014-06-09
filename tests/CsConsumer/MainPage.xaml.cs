using Microsoft.Graphics.Canvas;
using System;
using System.Collections.Generic;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CsConsumer
{
    public sealed partial class MainPage : Page
    {
        Random m_random = new Random();
        CanvasDevice m_device = new CanvasDevice();
        CanvasImageSource m_imageSource;

        public MainPage()
        {
            this.InitializeComponent();

            //
            // Set up the width/height combo boxes.
            //
            var sizes = new List<int>();
            for (int i = 0; i < 16; ++i)
            {
                sizes.Add(1 << i);
            }

            m_widthCombo.ItemsSource = sizes;
            m_heightCombo.ItemsSource = sizes;

            m_widthCombo.SelectedIndex = sizes.Count / 2;
            m_heightCombo.SelectedIndex = sizes.Count / 2;

            m_widthCombo.SelectionChanged += ImageSizeChangeRequested;
            m_heightCombo.SelectionChanged += ImageSizeChangeRequested;

            //
            // Create the initial image source
            //
            RecreateImageSource();
            DrawStuff();
        }

        private void OnRedrawClicked(object sender, RoutedEventArgs e)
        {
            DrawStuff();
        }

        private void ImageSizeChangeRequested(object sender, SelectionChangedEventArgs e)
        {
            RecreateImageSource();
            DrawStuff();
        }

        private void RecreateImageSource()
        {
            try
            {
                //
                // Wipe current state -- so if this fails for some reason we're 
                // left with no image source rather than the previous image source.
                //
                m_imageSource = null;
                m_theImage.Source = null;

                //
                // Create a new image source for the specified width/height
                //
                int width = (int)m_widthCombo.SelectedValue;
                int height = (int)m_heightCombo.SelectedValue;

                m_imageSource = new CanvasImageSource(m_device, width, height);
                m_theImage.Source = m_imageSource;

                m_statusMessage.Text = String.Format("{0}x{1} image source", width, height);
            }
            catch (Exception e)
            {
                m_statusMessage.Text = e.Message;
            }
        }

        void DrawStuff()
        {
            using (var ds = m_imageSource.CreateDrawingSession())
            {
                ds.Clear(NextRandomColor());
            }
        }

        private Color NextRandomColor()
        {
            return Color.FromArgb(
                                (byte)255,
                                (byte)m_random.Next(256),
                                (byte)m_random.Next(256),
                                (byte)m_random.Next(256));
        }
    }
}
