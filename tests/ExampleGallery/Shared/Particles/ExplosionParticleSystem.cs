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
using System;
using System.Numerics;
using System.Threading.Tasks;

namespace ExampleGallery
{
    // ExplosionParticleSystem is a specialization of ParticleSystem which creates a fiery explosion.
    // It should be combined with ExplosionSmokeParticleSystem for best effect.
    public class ExplosionParticleSystem : ParticleSystem
    {
        // Set up the constants that will give this particle system its behavior and properties.
        protected override void InitializeConstants()
        {
            bitmapFilename = "Particles/explosion.png";

            // High initial speed with lots of variance. Make the values closer
            // together to have more consistently circular explosions.
            minInitialSpeed = 30;
            maxInitialSpeed = 300;

            // Doesn't matter what these values are set to, acceleration is tweaked in InitializeParticle.
            minAcceleration = 0;
            maxAcceleration = 0;

            // Explosions should be relatively short lived.
            minLifetime = .5f;
            maxLifetime = 1.0f;

            minScale = .3f;
            maxScale = 1.0f;

            minNumParticles = 20;
            maxNumParticles = 25;

            minRotationSpeed = -(float)Math.PI / 4;
            maxRotationSpeed = (float)Math.PI / 4;

            // Additive blending is very good at creating fiery effects.
            blendState = CanvasBlend.Add;
        }


        public override async Task CreateResourcesAsync(ICanvasResourceCreator resourceCreator)
        {
            await base.CreateResourcesAsync(resourceCreator);

            // This particle system uses additive blending, which has a side effect of leaving 1.0
            // in the framebuffer alpha channel for black texels (which are visually transparent when
            // using additive blending). This doesn't normally affect anything (it is irrelevant what
            // the framebuffer alpha ends up containing, as long as the color channels look the way we
            // want) but thumbnail generation relies on alpha to blend the icon over its background color.
            // To avoid ugly black borders around the thumbnail, we edit the bitmap to have zero alpha,
            // leaving only additive RGB data.

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                var colors = bitmap.GetPixelColors();

                for (int i = 0; i < colors.Length; i++)
                {
                    colors[i].A = 0;
                }

                bitmap.SetPixelColors(colors);
            }
        }


        protected override void InitializeParticle(Particle particle, Vector2 where)
        {
            base.InitializeParticle(particle, where);
            
            // The base works fine except for acceleration. Explosions move outwards,
            // then slow down and stop because of air resistance. Let's change acceleration
            // so that when the particle is at max lifetime, the velocity will be zero.
            //
            // We'll use the equation vt = v0 + (a0 * t). If you're not familar with this,
            // it's one of the basic kinematics equations for constant acceleration, and
            // basically says: velocity at time t = initial velocity + acceleration * t.
            //
            // We'll solve the equation for a0, using t = particle.Lifetime and vt = 0.

            particle.Acceleration = -particle.Velocity / particle.Lifetime;
        }
    }
}
