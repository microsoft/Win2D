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

namespace ExampleGallery
{
    // SmokePlumeParticleSystem is a specialization of ParticleSystem which sends up a
    // plume of smoke. The smoke is blown to the right by the wind.
    public class SmokePlumeParticleSystem : ParticleSystem
    {
        // Set up the constants that will give this particle system its behavior and properties.
        protected override void InitializeConstants()
        {
            bitmapFilename = "Particles/smoke.png";

            minInitialSpeed = 20;
            maxInitialSpeed = 100;

            // We don't want the particles to accelerate at all, aside from what we do in our overriden InitializeParticle.
            minAcceleration = 0;
            maxAcceleration = 0;

            // Long lifetime, this can be changed to create thinner or thicker smoke.
            // Tweak minNumParticles and maxNumParticles to complement the effect.
            minLifetime = 5.0f;
            maxLifetime = 7.0f;

            minScale = .5f;
            maxScale = 1.0f;

            minNumParticles = 10;
            maxNumParticles = 50;

            // Rotate slowly; we want a fairly relaxed effect
            minRotationSpeed = -(float)Math.PI / 8;
            maxRotationSpeed = (float)Math.PI / 8;

            blendState = CanvasBlend.SourceOver;
        }


        // PickRandomDirection is overriden so we can make the particles always have an initial velocity pointing up.
        protected override Vector2 PickRandomDirection()
        {
            // Point the particles somewhere between 80 and 100 degrees.
            // Tweak this to make the smoke have more or less spread.
            float angle = Utils.RandomBetween(Utils.DegreesToRadians(80), Utils.DegreesToRadians(100));

            // From the unit circle, cosine is the x coordinate and sine is the y coordinate.
            // We're negating y because on the screen increasing y moves down the display.
            return new Vector2( (float)Math.Cos(angle),
                               -(float)Math.Sin(angle));
        }


        // InitializeParticle is overridden to add the appearance of wind.
        protected override void InitializeParticle(Particle particle, Vector2 where)
        {
            base.InitializeParticle(particle, where);

            // The base is mostly good, but we want to simulate a little bit of wind heading to the right.
            particle.Acceleration.X += Utils.RandomBetween(10, 50);
        }
    }
}
