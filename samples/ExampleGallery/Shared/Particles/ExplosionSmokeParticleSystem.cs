// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using System;

namespace ExampleGallery
{
    // ExplosionSmokeParticleSystem is a specialization of ParticleSystem which creates a circular
    // pattern of smoke. It should be combined with ExplosionParticleSystem for best effect.
    public class ExplosionSmokeParticleSystem : ParticleSystem
    {
        // Set up the constants that will give this particle system its behavior and properties.
        protected override void InitializeConstants()
        {
            bitmapFilename = "Particles/smoke.dds";

            // Less initial speed than the explosion itself.
            minInitialSpeed = 15;
            maxInitialSpeed = 150;

            // Acceleration is negative, so particles will accelerate away from the initial velocity.
            // This will make them slow down, as if from wind resistance. We want the smoke to linger
            // a bit and feel wispy, though, so we don't stop them completely like we do
            // ExplosionParticleSystem particles.
            minAcceleration = -10;
            maxAcceleration = -50;

            // Explosion smoke lasts for longer than the explosion itself, but not as long as the plumes do.
            minLifetime = 1.0f;
            maxLifetime = 2.5f;

            minScale = 1.0f;
            maxScale = 2.0f;

            minNumParticles = 10;
            maxNumParticles = 20;

            minRotationSpeed = -(float)Math.PI / 4;
            maxRotationSpeed = (float)Math.PI / 4;

            blendState = CanvasBlend.SourceOver;
        }
    }
}
