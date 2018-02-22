// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Input;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class CustomFonts : UserControl
    {
        struct Entry
        {
            public int Code;
            public string Label;

            public Entry(int code, string label)
            {
                Code = code;
                Label = label;
            }
        }

        public CustomFonts()
        {
            this.InitializeComponent();
        }

        static CanvasTextFormat labelText = new CanvasTextFormat()
        {
            FontSize = 24,
            HorizontalAlignment = CanvasHorizontalAlignment.Left,
            VerticalAlignment = CanvasVerticalAlignment.Center
        };

        static CanvasTextFormat symbolText = new CanvasTextFormat()
        {
            FontSize = 24,
            FontFamily = "Symbols.ttf#Symbols",
            HorizontalAlignment = CanvasHorizontalAlignment.Right,
            VerticalAlignment = CanvasVerticalAlignment.Center
        };

        static float lineHeight = symbolText.FontSize * 1.5f;

        CanvasLinearGradientBrush textOpacityBrush;
        CanvasLinearGradientBrush blurOpacityBrush;

        CoreIndependentInputSource inputSource;
        GestureRecognizer gestureRecognizer;

        private void OnGameLoopStarting(ICanvasAnimatedControl sender, object args)
        {
            //
            // The GestureRecognizer needs to be created and accessed from the 
            // same thread -- in this case the game loop thread, so we use the GameLoopStarting event 
            // for this.
            //

            gestureRecognizer = new GestureRecognizer();
            gestureRecognizer.GestureSettings = GestureSettings.ManipulationTranslateInertia | GestureSettings.ManipulationTranslateY;
                
            gestureRecognizer.ManipulationStarted += gestureRecognizer_ManipulationStarted;
            gestureRecognizer.ManipulationUpdated += gestureRecognizer_ManipulationUpdated;
            gestureRecognizer.ManipulationCompleted += gestureRecognizer_ManipulationCompleted;

            gestureRecognizer.InertiaTranslationDeceleration = -0.05f;

            //
            // When the GestureRecognizer goes into intertia mode (ie after the pointer is released)
            // we want it to generate ManipulationUpdated events in sync with the game loop's Update.
            // We do this by disabling AutoProcessIntertia and explicitly calling ProcessInertia() 
            // from the Update.
            //
            gestureRecognizer.AutoProcessInertia = false;

            inputSource = animatedControl.CreateCoreIndependentInputSource(CoreInputDeviceTypes.Mouse | CoreInputDeviceTypes.Pen | CoreInputDeviceTypes.Touch);

            inputSource.PointerPressed += Input_PointerPressed;
            inputSource.PointerMoved += Input_PointerMoved;
            inputSource.PointerReleased += Input_PointerReleased;
        }

        private void OnGameLoopStopped(ICanvasAnimatedControl sender, object args)
        {
            // Unregister the various input / gesture events.  Since these have strong thread affinity
            // this needs to be done on the game loop thread.

            inputSource.PointerPressed -= Input_PointerPressed;
            inputSource.PointerMoved -= Input_PointerMoved;
            inputSource.PointerReleased -= Input_PointerReleased;

            gestureRecognizer.ManipulationStarted -= gestureRecognizer_ManipulationStarted;
            gestureRecognizer.ManipulationUpdated -= gestureRecognizer_ManipulationUpdated;
            gestureRecognizer.ManipulationCompleted -= gestureRecognizer_ManipulationCompleted;
        }

        private void OnCreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            var stops = new CanvasGradientStop[]
            {
                new CanvasGradientStop() { Color=Colors.Transparent, Position=0.1f },
                new CanvasGradientStop() { Color=Colors.White, Position = 0.3f },
                new CanvasGradientStop() { Color=Colors.White, Position = 0.7f },
                new CanvasGradientStop() { Color=Colors.Transparent, Position = 0.9f },
            };

            textOpacityBrush = new CanvasLinearGradientBrush(sender, stops, CanvasEdgeBehavior.Clamp, CanvasAlphaMode.Premultiplied);

            stops = new CanvasGradientStop[]
            {
                new CanvasGradientStop() { Color=Colors.White, Position=0.0f },
                new CanvasGradientStop() { Color=Colors.Transparent, Position = 0.3f },
                new CanvasGradientStop() { Color=Colors.Transparent, Position = 0.7f },
                new CanvasGradientStop() { Color=Colors.White, Position = 1.0f },
            };

            blurOpacityBrush = new CanvasLinearGradientBrush(sender, stops, CanvasEdgeBehavior.Clamp, CanvasAlphaMode.Premultiplied);
        }

        //
        // The various Pointer events just forward to the GestureRecognizer
        //

        private void Input_PointerPressed(object sender, PointerEventArgs args)
        {
            gestureRecognizer.ProcessDownEvent(args.CurrentPoint);
            args.Handled = true;
        }

        private void Input_PointerMoved(object sender, PointerEventArgs args)      
        {
            gestureRecognizer.ProcessMoveEvents(args.GetIntermediatePoints());
            args.Handled = true;
        }

        private void Input_PointerReleased(object sender, PointerEventArgs args)
        {
            gestureRecognizer.ProcessUpEvent(args.CurrentPoint);
            args.Handled = true;
        }

        float offset = 0;
        float velocity = 0;
        const float targetSpeed = 2;
        float targetVelocity = targetSpeed;        
        bool inManipulation;

        void gestureRecognizer_ManipulationStarted(GestureRecognizer sender, ManipulationStartedEventArgs args)
        {
            velocity = 0;
            inManipulation = true;
        }

        void gestureRecognizer_ManipulationUpdated(GestureRecognizer sender, ManipulationUpdatedEventArgs args)
        {
            offset = offset - (float)args.Delta.Translation.Y;
            targetVelocity = -Math.Sign(args.Velocities.Linear.Y) * targetSpeed;
        }

        void gestureRecognizer_ManipulationCompleted(GestureRecognizer sender, ManipulationCompletedEventArgs args)
        {
            inManipulation = false;
        }

        int firstLine;
        int lastLine;
                
        private void OnUpdate(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            float height = (float)sender.Size.Height;
            float totalHeight = characters.Length * lineHeight + height;

            if (inManipulation)
            {
                if (gestureRecognizer != null)
                    gestureRecognizer.ProcessInertia();
            }
            else
            {
                velocity = velocity * 0.90f + targetVelocity * 0.10f;

                offset = offset + velocity;                
            }

            offset = offset % totalHeight;
            while (offset < 0)
                offset += totalHeight;

            float top = height - offset;
            firstLine = Math.Max(0, (int)(-top / lineHeight));
            lastLine = Math.Min(characters.Length, (int)((height + lineHeight - top) / lineHeight));
        }

        private CanvasCommandList GenerateTextDisplay(ICanvasResourceCreator resourceCreator, float width, float height)
        {
            var cl = new CanvasCommandList(resourceCreator);

            using (var ds = cl.CreateDrawingSession())
            {
                float top = height - offset;

                float center = width / 4.0f;
                float symbolPos = center - 5.0f;
                float labelPos = center + 5.0f;

                for (int i = firstLine; i < lastLine; ++i)
                {
                    float y = top + lineHeight * i;
                    int index = i;

                    if (index < characters.Length)
                    {
                        ds.DrawText(string.Format("{0}", (char)characters[index].Code), symbolPos, y, Colors.White, symbolText);
                        ds.DrawText(string.Format("0x{0:X} - {1}", characters[index].Code, characters[index].Label), labelPos, y, Colors.White, labelText);
                    }
                }
            }

            return cl;
        }

        private void OnDraw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var textDisplay = GenerateTextDisplay(sender, (float)sender.Size.Width, (float)sender.Size.Height);

            var blurEffect = new GaussianBlurEffect()
            {
                Source = textDisplay,
                BlurAmount = 10
            };

            textOpacityBrush.StartPoint = blurOpacityBrush.StartPoint = new Vector2(0,0);
            textOpacityBrush.EndPoint = blurOpacityBrush.EndPoint = new Vector2(0, (float)sender.Size.Height);

            var ds = args.DrawingSession;

            using (ds.CreateLayer(blurOpacityBrush))
            {
                ds.DrawImage(blurEffect);
            }

            using (ds.CreateLayer(textOpacityBrush))
            {
                ds.DrawImage(textDisplay);
            }
        }

        private void UserControl_Unloaded(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get
            // garbage collected
            animatedControl.RemoveFromVisualTree();
            animatedControl = null;
        }

        static Entry[] characters = new Entry[]
        {
            new Entry(0xE018, "Scrollbar arrow"),
            new Entry(0xE019, "Scrollbar arrow"),
            new Entry(0xE081, "Check box"),
            new Entry(0xE082, "Ratings star"),
            new Entry(0xE094, "Magnifier"),
            new Entry(0xE0B8, "Used by magnifier tool"),
            new Entry(0xE0D5, "Back arrow"),
            new Entry(0xE0AE, "Mirrored version of U+E0D4"),
            new Entry(0xE0E2, "Arrow used in FlipView"),
            new Entry(0xE0E3, "Arrow used in FlipView"),
            new Entry(0xE0E4, "Arrow used in FlipView"),
            new Entry(0xE0E5, "Arrow used in FlipView"),
            new Entry(0xE0E7, "AppBar menu checkmark"),
            new Entry(0xE224, "Outline ratings star"),
            new Entry(0xE26B, "Arrow used in group headers"),
            new Entry(0xE26C, "Arrow used in group headers"),
            new Entry(0xE100, "Previous"),
            new Entry(0xE168, "Accounts"),
            new Entry(0xE101, "Next"),
            new Entry(0xE169, "Show bcc"),
            new Entry(0xE102, "Play"),
            new Entry(0xE16A, "Bcc"),
            new Entry(0xE103, "Pause"),
            new Entry(0xE16B, "Cut"),
            new Entry(0xE104, "Edit"),
            new Entry(0xE16C, "Attach"),
            new Entry(0xE105, "Save"),
            new Entry(0xE16D, "Paste"),
            new Entry(0xE106, "Clear"),
            new Entry(0xE16E, "Filter"),
            new Entry(0xE107, "Trash"),
            new Entry(0xE16F, "Copy"),
            new Entry(0xE108, "Remove"),
            new Entry(0xE170, "Emoji2"),
            new Entry(0xE109, "Add"),
            new Entry(0xE171, "High priority"),
            new Entry(0xE10A, "Cancel"),
            new Entry(0xE172, "Reply (mirrored at U+E1AF)"),
            new Entry(0xE10B, "Accept"),
            new Entry(0xE173, "Slideshow"),
            new Entry(0xE10C, "More"),
            new Entry(0xE174, "Sort"),
            new Entry(0xE10D, "Redo"),
            new Entry(0xE178, "Manage"),
            new Entry(0xE10E, "Undo"),
            new Entry(0xE179, "All applications (mirrored at U+E1EC)"),
            new Entry(0xE10F, "Home"),
            new Entry(0xE17A, "Disconnect network drive"),
            new Entry(0xE110, "Up"),
            new Entry(0xE17B, "Map network drive"),
            new Entry(0xE111, "Forward"),
            new Entry(0xE17C, "Open new windows"),
            new Entry(0xE112, "Back"),
            new Entry(0xE17D, "Open with (mirrored at U+E1ED)"),
            new Entry(0xE113, "Favorite"),
            new Entry(0xE181, "Contact presence"),
            new Entry(0xE114, "Camera"),
            new Entry(0xE182, "Options"),
            new Entry(0xE115, "Settings"),
            new Entry(0xE183, "Upload"),
            new Entry(0xE116, "Video"),
            new Entry(0xE184, "Calendar"),
            new Entry(0xE117, "Sync"),
            new Entry(0xE185, "Font"),
            new Entry(0xE118, "Download"),
            new Entry(0xE186, "Font color"),
            new Entry(0xE119, "Mail"),
            new Entry(0xE187, "Contact 2"),
            new Entry(0xE11A, "Find"),
            new Entry(0xE188, "Browse by album (mirrored at U+E1C1)"),
            new Entry(0xE11B, "Help (mirrored at U+E1F3)"),
            new Entry(0xE189, "Alternate audio track"),
            new Entry(0xE11C, "Upload"),
            new Entry(0xE18A, "Placeholder"),
            new Entry(0xE11D, "Emoticon"),
            new Entry(0xE18B, "View"),
            new Entry(0xE11E, "Layout"),
            new Entry(0xE18C, "Set as lock screen image"),
            new Entry(0xE11F, "Leave multiple party conversation"),
            new Entry(0xE18D, "Set as tile image"),
            new Entry(0xE120, "Forward (mirrored at U+E1A8)"),
            new Entry(0xE190, "Closed caption"),
            new Entry(0xE121, "Timer"),
            new Entry(0xE191, "Autoplay stop"),
            new Entry(0xE122, "Send/Send calendar event (mirrored at U+E1A9)"),
            new Entry(0xE192, "Permissions"),
            new Entry(0xE123, "Crop"),
            new Entry(0xE193, "Highlight"),
            new Entry(0xE124, "Rotate camera"),
            new Entry(0xE194, "Disable updates"),
            new Entry(0xE125, "People"),
            new Entry(0xE195, "Unfavorite"),
            new Entry(0xE126, "Close metadata (mirrored at U+E1BF)"),
            new Entry(0xE196, "Unpin"),
            new Entry(0xE127, "Open metadata (mirrored at U+E1C0)"),
            new Entry(0xE197, "Open file location"),
            new Entry(0xE128, "Open in web"),
            new Entry(0xE198, "Volume mute"),
            new Entry(0xE129, "View notifications"),
            new Entry(0xE199, "Italic"),
            new Entry(0xE12A, "Preview link"),
            new Entry(0xE19A, "Underline"),
            new Entry(0xE12B, "Category not in A-Z"),
            new Entry(0xE19B, "Bold"),
            new Entry(0xE12C, "Trim"),
            new Entry(0xE19C, "Move to folder"),
            new Entry(0xE12D, "Attach camera"),
            new Entry(0xE19D, "Choose like or dislike"),
            new Entry(0xE12E, "Zoom"),
            new Entry(0xE19E, "Dislike"),
            new Entry(0xE12F, "Bookmarks (mirrored at U+E1EE)"),
            new Entry(0xE19F, "Like"),
            new Entry(0xE130, "PDF"),
            new Entry(0xE1A0, "Align right"),
            new Entry(0xE131, "Password protected PDF"),
            new Entry(0xE1A1, "Align center"),
            new Entry(0xE132, "Page"),
            new Entry(0xE1A2, "Align left"),
            new Entry(0xE133, "More options (mirrored at U+E1EF)"),
            new Entry(0xE1A3, "Zoom neutral"),
            new Entry(0xE134, "Post"),
            new Entry(0xE1A4, "Zoom out"),
            new Entry(0xE135, "Mail2"),
            new Entry(0xE1A5, "Open file"),
            new Entry(0xE136, "Contact info"),
            new Entry(0xE1A6, "Other user"),
            new Entry(0xE137, "Hang up"),
            new Entry(0xE1A7, "Run as admin"),
            new Entry(0xE138, "View all albums"),
            new Entry(0xE1C3, "Street"),
            new Entry(0xE139, "Map address"),
            new Entry(0xE1C4, "Map"),
            new Entry(0xE13A, "Phone"),
            new Entry(0xE1C5, "Clear selection (mirrored at U+E1F4)"),
            new Entry(0xE13B, "Video chat"),
            new Entry(0xE1C6, "Font size decrease"),
            new Entry(0xE13C, "Switch"),
            new Entry(0xE1C7, "Font size increase"),
            new Entry(0xE13D, "Contact"),
            new Entry(0xE1C8, "Font size"),
            new Entry(0xE13E, "Rename"),
            new Entry(0xE1C9, "Cell phone"),
            new Entry(0xE141, "Pin"),
            new Entry(0xE1CA, "Retweet (full size)"),
            new Entry(0xE142, "View all info"),
            new Entry(0xE1CB, "Tag"),
            new Entry(0xE143, "Go (mirrored at U+E1AA)"),
            new Entry(0xE1CC, "Repeat once"),
            new Entry(0xE144, "Keyboard"),
            new Entry(0xE1CD, "Repeat/Loop"),
            new Entry(0xE145, "Dock left"),
            new Entry(0xE1CE, "Favorite star empty"),
            new Entry(0xE146, "Dock right"),
            new Entry(0xE1CF, "Add to favorite"),
            new Entry(0xE147, "Dock bottom"),
            new Entry(0xE1D0, "Calculator"),
            new Entry(0xE148, "Remote desktop home"),
            new Entry(0xE1D1, "Direction"),
            new Entry(0xE149, "Refresh"),
            new Entry(0xE1D2, "Current location/GPS"),
            new Entry(0xE14A, "Rotate"),
            new Entry(0xE1D3, "Library"),
            new Entry(0xE14B, "Shuffle"),
            new Entry(0xE1D4, "Address book"),
            new Entry(0xE14C, "Details (mirrored at U+E175)"),
            new Entry(0xE1D5, "Voicemail/Memo"),
            new Entry(0xE14D, "Shop"),
            new Entry(0xE1D6, "Microphone"),
            new Entry(0xE14E, "Select all"),
            new Entry(0xE1D7, "Post update"),
            new Entry(0xE14F, "Rotate"),
            new Entry(0xE1D8, "Back to window"),
            new Entry(0xE150, "Import (mirrored at U+E1AD)"),
            new Entry(0xE1D9, "Full screen"),
            new Entry(0xE151, "Import all files (mirrored at U+E1AE)"),
            new Entry(0xE1DA, "Add new folder"),
            new Entry(0xE155, "Browse photos"),
            new Entry(0xE1DB, "Calendar reply"),
            new Entry(0xE156, "Webcam"),
            new Entry(0xE1DD, "Unsync folder"),
            new Entry(0xE158, "Picture library"),
            new Entry(0xE1DE, "Report hacked"),
            new Entry(0xE159, "Save local"),
            new Entry(0xE1DF, "Sync folder"),
            new Entry(0xE15A, "Caption"),
            new Entry(0xE1E0, "Block contact"),
            new Entry(0xE15B, "Stop"),
            new Entry(0xE1E1, "RD switch apps"),
            new Entry(0xE15C, "Results (find) (mirrored at U+E1F1)"),
            new Entry(0xE1E2, "Add friend"),
            new Entry(0xE15D, "Volume"),
            new Entry(0xE1E3, "RD touch pointer"),
            new Entry(0xE15E, "Tools"),
            new Entry(0xE1E4, "RD go to start"),
            new Entry(0xE15F, "Start chat"),
            new Entry(0xE1E5, "RD zero bars"),
            new Entry(0xE160, "Document"),
            new Entry(0xE1E6, "RD one bar"),
            new Entry(0xE161, "Day"),
            new Entry(0xE1E7, "RD two bars"),
            new Entry(0xE162, "Week"),
            new Entry(0xE1E8, "RD three bars"),
            new Entry(0xE163, "Month (mirrored at U+E1DC)"),
            new Entry(0xE1E9, "RD four bars"),
            new Entry(0xE164, "Match options"),
            new Entry(0xE294, "Scan"),
            new Entry(0xE165, "Reply all"),
            new Entry(0xE295, "Preview"),
            new Entry(0xE166, "Read"),
            new Entry(0xE167, "Link"),
        };
    }
}
