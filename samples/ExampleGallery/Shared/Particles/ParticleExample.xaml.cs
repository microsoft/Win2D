// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace ExampleGallery
{
    public sealed partial class ParticleExample : UserControl
    {
        public enum ParticleMode
        {
            Explosions,
            SmokePlume,
        }

        public List<ParticleMode> ModeList { get { return Utils.GetEnumAsList<ParticleMode>(); } }

        public ParticleMode CurrentMode { get; set; }


        // This example uses three different particle systems.
        ParticleSystem smokePlume = new SmokePlumeParticleSystem();
        ParticleSystem smoke = new ExplosionSmokeParticleSystem();
        ParticleSystem explosion = new ExplosionParticleSystem();


        // A timer that will tell us when it's time to trigger another explosion.
        const float TimeBetweenExplosions = .8f;
        float timeTillExplosion = 0.0f;

        // A timer that will tell us when it's time to add more particles to the smoke plume.
        const float TimeBetweenSmokePlumePuffs = .5f;
        float timeTillPuff = 0.0f;


        public ParticleExample()
        {
            this.InitializeComponent();
        }


        void CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            if (args.Reason == CanvasCreateResourcesReason.DpiChanged)
                return;

            args.TrackAsyncAction(CreateResourcesAsync(sender).AsAsyncAction());
        }


        async Task CreateResourcesAsync(CanvasAnimatedControl sender)
        {
            await smokePlume.CreateResourcesAsync(sender);
            await smoke.CreateResourcesAsync(sender);
            await explosion.CreateResourcesAsync(sender);
        }


        void Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            var elapsedTime = (float)args.Timing.ElapsedTime.TotalSeconds;

            switch (CurrentMode)
            {
                case ParticleMode.Explosions:
                    CreateExplosions(elapsedTime);
                    break;

                case ParticleMode.SmokePlume:
                    CreateSmokePlume(elapsedTime);
                    break;
            }

            smokePlume.Update(elapsedTime);
            smoke.Update(elapsedTime);
            explosion.Update(elapsedTime);
        }


        // This function is called when we want to demo the explosion effect. It updates the
        // timeTillExplosion timer, and starts another explosion effect when the timer reaches zero.
        void CreateExplosions(float elapsedTime)
        {
            timeTillExplosion -= elapsedTime;

            while (timeTillExplosion < 0)
            {
                Vector2 where = canvas.Size.ToVector2();

                // Create the explosion at some random point on the screen.
                if (ThumbnailGenerator.IsDrawingThumbnail)
                {
                    where /= 2;
                }
                else
                {
                    where.X *= Utils.RandomBetween(0.25f, 0.75f);
                    where.Y *= Utils.RandomBetween(0.25f, 0.75f);
                }

                // The overall explosion effect is actually comprised of two particle systems:
                // the fiery bit, and the smoke behind it. Add particles to both of those systems.
                explosion.AddParticles(where);
                smoke.AddParticles(where);

                // Reset the timer.
                timeTillExplosion += TimeBetweenExplosions;
            }
        }


        // This function is called when we want to demo the smoke plume effect. It updates
        // the timeTillPuff timer, and adds more particles to the plume when necessary.
        void CreateSmokePlume(float elapsedTime)
        {
            timeTillPuff -= elapsedTime;

            while (timeTillPuff < 0)
            {
                // Add more particles at the bottom of the screen, one third of the way across.
                Vector2 where = canvas.Size.ToVector2();
                where.X /= 3;

                smokePlume.AddParticles(where);

                // And then reset the timer.
                timeTillPuff += TimeBetweenSmokePlumePuffs;
            }
        }

        
        void Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            smokePlume.Draw(ds);
            smoke.Draw(ds);
            explosion.Draw(ds);
        }

        public void SliderValueChanged(object sender, RangeBaseValueChangedEventArgs args)
        {
            if (canvas != null)
                canvas.DpiScale = (float)(args.NewValue);
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected.
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
