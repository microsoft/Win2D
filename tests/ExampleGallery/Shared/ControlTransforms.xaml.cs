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

using Microsoft.Graphics.Canvas;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class ControlTransforms : UserControl
    {
        CanvasImageSource imageSource;
        CanvasSwapChain swapChain;


        public ControlTransforms()
        {
            this.InitializeComponent();
        }


        void Canvas_CreateResources(CanvasControl sender, object args)
        {
            DrawToImageSource(imageControl.DesiredSize);
            DrawToSwapChain(swapChainPanel.DesiredSize);
        }


        // Draw to the CanvasControl.
        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawTextLabel(args.DrawingSession, "Canvas\nControl", sender.Size);
        }


        // Draw to the CanvasAnimatedControl.
        void Animated_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            DrawTextLabel(args.DrawingSession, "Canvas\nAnimated\nControl", sender.Size);
        }
        
        
        // Draw to the CanvasImageSource.
        void DrawToImageSource(Size size)
        {
            if (size.Width <= 0 || size.Height <= 0)
            {
                imageControl.Source = null;
                return;
            }

            imageSource = new CanvasImageSource(canvasControl, (float)size.Width, (float)size.Height);

            using (var ds = imageSource.CreateDrawingSession(Colors.Yellow))
            {
                DrawTextLabel(ds, "Canvas\nImage\nSource", size);
            }

            imageControl.Source = imageSource;
        }


        // Draw to the CanvasSwapChain.
        void DrawToSwapChain(Size size)
        {
            if (size.Width <= 0 || size.Height <= 0)
            {
                swapChain = null;
                swapChainPanel.SwapChain = null;
                return;
            }
            else if (swapChain == null)
            {
                swapChain = new CanvasSwapChain(canvasControl, (float)size.Width, (float)size.Height);
                swapChainPanel.SwapChain = swapChain;
            }
            else
            {
                swapChain.ResizeBuffers((float)size.Width, (float)size.Height);
            }

            using (var ds = swapChain.CreateDrawingSession(Colors.Yellow))
            {
                DrawTextLabel(ds, "Canvas\nSwap\nChain", size);
            }

            swapChain.Present();
        }


        void DrawTextLabel(CanvasDrawingSession drawingSession, string text, Size size)
        {
            var label = string.Format("{0}\n{1:0} x {2:0}", text, size.Width, size.Height);

            drawingSession.DrawText(label, size.ToVector2() / 2, Colors.Black, textLabelFormat);
        }


        readonly CanvasTextFormat textLabelFormat = new CanvasTextFormat
        {
            FontSize = 16,
            ParagraphAlignment = ParagraphAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center,
        };


        void imageControl_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (canvasControl.ReadyToDraw)
            {
                // Only redraw if the size really has changed. This avoids layout cycles where assigning
                // a new CanvasImageSource to imageControl.Source causes a no-op SizeChanged notification.
                if (e.NewSize != imageSource.Size)
                {
                    DrawToImageSource(e.NewSize);
                }
            }
        }


        void swapChainPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (canvasControl.ReadyToDraw)
            {
                // Unlike imageControl_SizeChanged, this does not need to bother checking
                // for no-op because swapChain.ResizeBuffers does not trigger XAML layout.

                DrawToSwapChain(e.NewSize);
            }
        }


        void AnimateTransform_Click(object sender, RoutedEventArgs e)
        {
            sizeAnimation.Stop();
            transformAnimation.Begin();
        }


        void AnimateSize_Click(object sender, RoutedEventArgs e)
        {
            transformAnimation.Stop();
            sizeAnimation.Begin();
        }


        void ResetAnimation_Click(object sender, RoutedEventArgs e)
        {
            transformAnimation.Stop();
            sizeAnimation.Stop();
        }
    }
}
