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

using System;
using System.Numerics;

namespace ExampleGallery
{
    // Particles are the little bits that make up a particle system effect. Each particle
    // system is be comprised of many of these particles. They have basic physical properties,
    // such as position, velocity, acceleration, and rotation. They'll be drawn as sprites,
    // all layered on top of one another, and will be very pretty.
    public class Particle
    {
        // Position, Velocity, and Acceleration represent exactly what their names
        // indicate. They are public fields rather than properties so that users
        // can directly access their .X and .Y properties.
        public Vector2 Position;
        public Vector2 Velocity;
        public Vector2 Acceleration;

        // How long this particle will live.
        public float Lifetime;

        // How long it has been since initialize was called.
        public float TimeSinceStart;

        // The scale of this particle.
        public float Scale;

        // Its rotation, in radians.
        public float Rotation;

        // How fast does it rotate?
        public float RotationSpeed;


        // Initialize is called by ParticleSystem to set up the particle, and prepares the particle for use.
        public void Initialize(Vector2 position, Vector2 velocity, Vector2 acceleration, float lifetime, float scale, float rotationSpeed)
        {
            this.Position = position;
            this.Velocity = velocity;
            this.Acceleration = acceleration;
            this.Lifetime = lifetime;
            this.Scale = scale;
            this.RotationSpeed = rotationSpeed;
            
            // Reset TimeSinceStart - we have to do this because particles will be reused.
            this.TimeSinceStart = 0.0f;

            // Set rotation to some random value between 0 and 360 degrees.
            this.Rotation = Utils.RandomBetween(0, (float)Math.PI * 2);
        }


        // Update is called by the ParticleSystem on every frame.
        // This is where the particle's position and that kind of thing get updated.
        // Returns whether the particle is still alive.
        public bool Update(float elapsedTime)
        {
            Velocity += Acceleration * elapsedTime;
            Position += Velocity * elapsedTime;

            Rotation += RotationSpeed * elapsedTime;

            TimeSinceStart += elapsedTime;

            return TimeSinceStart < Lifetime;
        }
    }
}
