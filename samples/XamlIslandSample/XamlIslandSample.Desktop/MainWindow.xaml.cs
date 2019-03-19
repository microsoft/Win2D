// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using ExampleGallery;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using System.Threading.Tasks;
using System.Windows;
using Windows.UI;
using Win2D = Microsoft.Graphics.Canvas;

namespace XamlIslandSample.Desktop
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // Test the deployment
            // This only works when the .deps.json file is published
            if (string.IsNullOrEmpty(AppDomain.CurrentDomain.GetData("FX_PRODUCT_VERSION") as string))
            {
                MessageBox.Show("No .deps.json file found.");
            }

            canvasAnimatedControl.UwpControl.ClearColor = Colors.Black;
            ParticleSystem.RootDirectory = AppDomain.CurrentDomain.BaseDirectory;
        }

        private bool _useSpriteBatch;
        private ParticleSystem _smoke = new ExplosionSmokeParticleSystem();
        private ParticleSystem _explosion = new ExplosionParticleSystem();
        private float _timeTillExplosion;
        private const float TimeBetweenExplosions = .8f;

        private void CanvasAnimatedControl_CreateResources(Win2D.UI.Xaml.CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            if (args.Reason == CanvasCreateResourcesReason.DpiChanged)
                return;

            if (args.Reason == CanvasCreateResourcesReason.FirstTime)
                _useSpriteBatch = CanvasSpriteBatch.IsSupported(sender.Device);

            args.TrackAsyncAction(CreateResourcesAsync(sender).AsAsyncAction());

            async Task CreateResourcesAsync(Win2D.UI.Xaml.CanvasAnimatedControl sender)
            {
                await _smoke.CreateResourcesAsync(sender);
                await _explosion.CreateResourcesAsync(sender);
            }
        }

        private void CanvasAnimatedControl_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            _smoke.Draw(ds, _useSpriteBatch);
            _explosion.Draw(ds, _useSpriteBatch);
        }

        private void CanvasAnimatedControl_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            var elapsedTime = (float)args.Timing.ElapsedTime.TotalSeconds;

            CreateExplosions(elapsedTime);

            _smoke.Update(elapsedTime);
            _explosion.Update(elapsedTime);

            void CreateExplosions(float elapsedTime)
            {
                _timeTillExplosion -= elapsedTime;

                while (_timeTillExplosion < 0)
                {
                    Vector2 where = canvasAnimatedControl.UwpControl.Size.ToVector2();

                    // Create the explosion at some random point on the screen.
                    where.X *= Utils.RandomBetween(0.25f, 0.75f);
                    where.Y *= Utils.RandomBetween(0.25f, 0.75f);

                    // The overall explosion effect is actually comprised of two particle systems:
                    // the fiery bit, and the smoke behind it. Add particles to both of those systems.
                    _explosion.AddParticles(where);
                    _smoke.AddParticles(where);

                    // Reset the timer.
                    _timeTillExplosion += TimeBetweenExplosions;
                }
            }

        }

    }
}
