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
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation;
using Windows.System;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    //
    // This example is a simple typing game that demonstrates marshaling
    // keyboard events to the game loop thread.
    //
    public sealed partial class KeyboardInputExample : UserControl
    {
        LetterAttack.Game game;

        public KeyboardInputExample()
        {
            this.InitializeComponent();
            game = new LetterAttack.Game();
        }

        private void control_Loaded(object sender, RoutedEventArgs e)
        {
            // Register for keyboard events
            Window.Current.CoreWindow.KeyDown += KeyDown_UIThread;

#if WINDOWS_PHONE_APP || WINDOWS_UAP
            var keyboardCaps = new Windows.Devices.Input.KeyboardCapabilities();
            if (keyboardCaps.KeyboardPresent == 0)
            {
                // If we don't have a keyboard show the input pane (aka the on-screen keyboard).
                var inputPane = Windows.UI.ViewManagement.InputPane.GetForCurrentView();
                inputPane.TryShow();
                inputPane.Showing += inputPane_Showing;
                inputPane.Hiding += inputPane_Hiding;
            }
#endif
        }

        private void animatedControl_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
#if WINDOWS_PHONE_APP || WINDOWS_UAP
            // Bring the on-screen keyboard back up when the user taps on the screen.
            var keyboardCaps = new Windows.Devices.Input.KeyboardCapabilities();
            if (keyboardCaps.KeyboardPresent == 0)
            {
                // There's no keyboard present, so show the input pane
                var inputPane = Windows.UI.ViewManagement.InputPane.GetForCurrentView();
                inputPane.TryShow();
            }
#endif
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Unregister keyboard events
            Window.Current.CoreWindow.KeyDown -= KeyDown_UIThread;

            // Explicitly remove references to allow the Win2D controls to get garbage collected
            animatedControl.RemoveFromVisualTree();
            animatedControl = null;
        }

        private void KeyDown_UIThread(CoreWindow sender, KeyEventArgs args)
        {
            // This event runs on the UI thread.  If we want to process data
            // structures that are accessed on the game loop thread then we need
            // to use some kind of thread synchronization to ensure that the UI
            // thread and game loop thread are not accessing the same data at
            // the same time.
            //
            // The two main options here would be to use locks / critical
            // sections or to use RunOnGameLoopThreadAsync to cause code to
            // execute on the game loop thread.  This example uses
            // RunOnGameLoopThreadAsync.
            //
            // Since KeyEventArgs is not agile we cannot simply pass args to the
            // game loop thread.  Although doing this may appear to work, it may
            // have some surprising behavior.  For example, calling a method on
            // args may result in the method executing on the UI thread (and so
            // blocking the game loop thread).  There will also be surprising
            // races about which thread gets to set Handled first.
            //
            // Instead, we do as much processing as possible on the UI thread
            // before passing control to the game loop thread.

            char pressedLetter = GetPressedLetter(args);

            if (pressedLetter == 0)
            {
                // It wasn't a letter that was pressed, so we don't handle this event
                return;
            }

            // Mark that we've handled this event.  It is important to do this
            // synchronously inside this event handler since the framework uses
            // the Handled flag to determine whether or not to send this event
            // to other handlers.
            args.Handled = true;

            // Now schedule code to run on the game loop thread to handle the
            // pressed letter.  The animated control will execute this before
            // the next Update.
            var action = animatedControl.RunOnGameLoopThreadAsync(() => game.ProcessPressedLetterOnGameLoopThread(pressedLetter));
        }

        // Convert a KeyEventArgs to the corresponding A-Z letter.
        // Returns 0 if no letter was pressed.
        private static char GetPressedLetter(KeyEventArgs args)
        {
            var key = args.VirtualKey;
            char pressed = (char)0;

            switch (key)
            {
                case VirtualKey.A: pressed = 'A'; break;
                case VirtualKey.B: pressed = 'B'; break;
                case VirtualKey.C: pressed = 'C'; break;
                case VirtualKey.D: pressed = 'D'; break;
                case VirtualKey.E: pressed = 'E'; break;
                case VirtualKey.F: pressed = 'F'; break;
                case VirtualKey.G: pressed = 'G'; break;
                case VirtualKey.H: pressed = 'H'; break;
                case VirtualKey.I: pressed = 'I'; break;
                case VirtualKey.J: pressed = 'J'; break;
                case VirtualKey.K: pressed = 'K'; break;
                case VirtualKey.L: pressed = 'L'; break;
                case VirtualKey.M: pressed = 'M'; break;
                case VirtualKey.N: pressed = 'N'; break;
                case VirtualKey.O: pressed = 'O'; break;
                case VirtualKey.P: pressed = 'P'; break;
                case VirtualKey.Q: pressed = 'Q'; break;
                case VirtualKey.R: pressed = 'R'; break;
                case VirtualKey.S: pressed = 'S'; break;
                case VirtualKey.T: pressed = 'T'; break;
                case VirtualKey.U: pressed = 'U'; break;
                case VirtualKey.V: pressed = 'V'; break;
                case VirtualKey.W: pressed = 'W'; break;
                case VirtualKey.X: pressed = 'X'; break;
                case VirtualKey.Y: pressed = 'Y'; break;
                case VirtualKey.Z: pressed = 'Z'; break;
            }

            return pressed;
        }

        // When the InputPane (ie on-screen keyboard) is shown then we arrange
        // so that the animated control is not obscured.
        void inputPane_Showing(Windows.UI.ViewManagement.InputPane sender, Windows.UI.ViewManagement.InputPaneVisibilityEventArgs args)
        {
            RowObscuredByInputPane.Height = new GridLength(args.OccludedRect.Height);
        }

        void inputPane_Hiding(Windows.UI.ViewManagement.InputPane sender, Windows.UI.ViewManagement.InputPaneVisibilityEventArgs args)
        {
            // When the input pane rescale the game to fit
            RowObscuredByInputPane.Height = new GridLength(0);
        }

        private void animatedControl_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            game.Update();
        }

        private void animatedControl_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            game.Draw(args.DrawingSession, sender.Size);
        }
    }

    namespace LetterAttack
    {
        class Game
        {
            static CanvasTextFormat scoreFormat = new CanvasTextFormat
            {
                HorizontalAlignment = CanvasHorizontalAlignment.Left,
                VerticalAlignment = CanvasVerticalAlignment.Top
            };

            static CanvasTextFormat levelUpFormat = new CanvasTextFormat
            {
                FontSize = 40,
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                VerticalAlignment = CanvasVerticalAlignment.Top
            };

            const int initialFramesBetweenNewLetters = 60;
            const float initialVerticalRegion = 0.1f;

            List<Letter> letters = new List<Letter>();
            int framesSinceLastNewLetter = initialFramesBetweenNewLetters;

            int framesBetweenNewLetters;
            float verticalRegion;

            int leveledUpTimer;

            int score;
            int highScore = -1;

            bool gameOver;

            public Game()
            {
                ResetLevel();

                if (ThumbnailGenerator.IsDrawingThumbnail)
                {
                    var rng = new Random();

                    for (int i = 0; i < 30; ++i)
                    {
                        var l = new Letter();
                        l.Pos = new Vector2((float)(0.2 + rng.NextDouble() * 0.8), l.Pos.Y);
                        letters.Add(l);
                    }
                }
            }

            // This is called in response to a KeyDown event being received on
            // the UI thread (see KeyDown_UIThread).  It is invoked via
            // RunOnGameLoopThreadAsync.
            public void ProcessPressedLetterOnGameLoopThread(char pressedLetter)
            {
                foreach (var letter in letters)
                {
                    if (!letter.IsDead && letter.Value == pressedLetter)
                    {
                        letter.Die();
                        ++score;

                        if (score % 10 == 0)
                        {
                            // level up!
                            framesBetweenNewLetters = Math.Max(10, framesBetweenNewLetters * 5 / 6);
                            verticalRegion = Math.Min(0.8f, verticalRegion + 0.01f);
                            leveledUpTimer = 0;
                        }
                        break;
                    }
                }
            }

            public void Update()
            {
                if (ThumbnailGenerator.IsDrawingThumbnail)
                    return;

                ++framesSinceLastNewLetter;

                if (!gameOver && framesSinceLastNewLetter > framesBetweenNewLetters)
                {
                    letters.Add(new Letter());
                    framesSinceLastNewLetter = 0;
                }

                foreach (var letter in letters)
                {
                    letter.Update();

                    if (!letter.IsDead && letter.Pos.X < 0)
                    {
                        gameOver = true;
                    }
                }

                letters.RemoveAll(l => l.ShouldRemove);

                if (gameOver)
                {
                    foreach (var letter in letters)
                    {
                        if (!letter.IsDead)
                        {
                            letter.Die();
                        }
                    }

                    if (letters.Count == 0)
                    {
                        highScore = Math.Max(highScore, score);
                        ResetLevel();
                    }
                }

                ++leveledUpTimer;
            }

            void ResetLevel()
            {
                letters.Clear();
                leveledUpTimer = 1000;
                framesSinceLastNewLetter = initialFramesBetweenNewLetters;
                framesBetweenNewLetters = initialFramesBetweenNewLetters;
                verticalRegion = initialVerticalRegion;
                score = 0;
                gameOver = false;
            }

            public void Draw(CanvasDrawingSession ds, Size screenSize)
            {
                var transform = CalculateGameToScreenTransform(screenSize);

                DrawBackground(ds, transform);

                foreach (var letter in letters)
                {
                    letter.Draw(ds, transform);
                }

                ds.Transform = Matrix3x2.Identity;

                if (!ThumbnailGenerator.IsDrawingThumbnail)
                {
                    string scoreText = string.Format("Score: {0}", score);
                    if (highScore != -1)
                    {
                        scoreText += string.Format(" ({0})", highScore);
                    }
                    ds.DrawText(scoreText, 0, 0, Colors.White, scoreFormat);
                }
            }

            private Matrix3x2 CalculateGameToScreenTransform(Size screenSize)
            {
                var scaleToScreen = Matrix3x2.CreateScale(screenSize.ToVector2());

                var scaleVerticalRegion = Matrix3x2.CreateScale(1, verticalRegion);
                var offsetVerticalRegion = Matrix3x2.CreateTranslation(0, 0.5f - (verticalRegion / 2.0f));

                var transform = scaleVerticalRegion * offsetVerticalRegion * scaleToScreen;
                return transform;
            }

            private void DrawBackground(CanvasDrawingSession ds, Matrix3x2 transform)
            {
                const int levelUpTime = 60;

                // After levelling up we flash the screen white for a bit
                Color normalColor = Colors.Blue;
                Color levelUpFlashColor = Colors.White;

                var flashAmount = Math.Min(1, (leveledUpTimer / (float)levelUpTime));

                ds.Clear(InterpolateColors(normalColor, levelUpFlashColor, flashAmount));

                var topLeft = Vector2.Transform(new Vector2(0, 0), transform);
                var bottomRight = Vector2.Transform(new Vector2(1, 1), transform);

                var middle = (bottomRight.X - topLeft.X) / 2 + topLeft.X;

                // and display some text to let the player know what happened
                if (leveledUpTimer < levelUpTime * 2)
                {
                    ds.DrawText("Level Up!", middle, 0, Colors.White, levelUpFormat);
                }

                // Draw some lines to show where the top / bottom of the play area is.
                var topLine = topLeft.Y - Letter.TextFormat.FontSize;
                var bottomLine = bottomRight.Y + Letter.TextFormat.FontSize;

                Color lineColor = levelUpFlashColor;
                lineColor.A = 128;

                ds.DrawLine(0, topLine, bottomRight.X, topLine, lineColor, 3);
                ds.DrawLine(0, bottomLine, bottomRight.X, bottomLine, lineColor, 3);
            }

            private static Color InterpolateColors(Color color1, Color color2, float a)
            {
                var b = 1 - a;

                return Color.FromArgb(
                    255,
                    (byte)(color1.R * a + color2.R * b),
                    (byte)(color1.G * a + color2.G * b),
                    (byte)(color1.B * a + color2.B * b));
            }
        }

        // Repesents a single letter on the screen
        class Letter
        {
            static string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            static Random rng = new Random();
            const float moveSpeed = 0.002f;
            const int deathFrames = 30;
            int deathTimer = 0;

            public char Value { get; private set; }
            public Vector2 Pos { get; set; } // position is normalized 0-1

            public bool IsDead { get { return deathTimer > 0; } }

            public bool ShouldRemove { get { return deathTimer > deathFrames; } }

            public float DeadMu { get { return Math.Min(1.0f, (float)deathTimer / (float)deathFrames); } }

            public static CanvasTextFormat TextFormat = new CanvasTextFormat
            {
                FontSize = 36,
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                VerticalAlignment = CanvasVerticalAlignment.Center
            };

            public Letter()
            {
                Value = chars[rng.Next(0, chars.Length)];
                Pos = new Vector2(1, (float)rng.NextDouble());
                deathTimer = 0;
            }

            public void Update()
            {
                if (deathTimer == 0)
                {
                    Pos = new Vector2(Pos.X - moveSpeed, Pos.Y);
                }
                else
                {
                    deathTimer++;
                }
            }

            public void Draw(CanvasDrawingSession ds, Matrix3x2 transform)
            {
                var pos = Vector2.Transform(Pos, transform);

                var mu = (float)Math.Sin(DeadMu * Math.PI * 0.5);

                var scale = Matrix3x2.CreateScale(1.0f + mu * 10.0f);

                var center = new Vector2(pos.X, pos.Y);
                ds.Transform = Matrix3x2.CreateTranslation(-center) * scale * Matrix3x2.CreateTranslation(center);
                Color c = Color.FromArgb((byte)((1.0f - mu) * 255.0f), 255, 255, 255);

                ds.DrawText(Value.ToString(), pos, c, TextFormat);
            }

            public void Die()
            {
                if (deathTimer == 0)
                    deathTimer = 1;
            }
        }
    }
}
