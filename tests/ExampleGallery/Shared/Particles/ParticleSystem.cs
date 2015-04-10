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
using System.Collections.Generic;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;

namespace ExampleGallery
{
    // ParticleSystem is an abstract class that provides the basic functionality to
    // create a particle effect. Different subclasses will have different effects,
    // such as fire, explosions, and plumes of smoke. To use these subclasses, 
    // simply call AddParticles, and pass in where the particles should be created.
    public abstract class ParticleSystem
    {
        // The texture this particle system will use.
        protected CanvasBitmap bitmap;

        Vector2 bitmapCenter;
        Rect bitmapBounds;

        // The particles currently in use by this system. These are reused,
        // so calling AddParticles will not normally cause any allocations.
        List<Particle> activeParticles = new List<Particle>();

        // Keeps track of particles that are not curently being used by an effect.
        // When new particles are requested, particles are taken from here.
        // When particles are finished they are transferred back to here.
        static Stack<Particle> freeParticles = new Stack<Particle>();


        // This region of values control the "look" of the particle system, and should 
        // be set by derived particle systems in the InitializeConstants method. The
        // values are then used by the virtual function InitializeParticle. Subclasses
        // can override InitializeParticle for further customization.

        #region Constants to be set by subclasses

        // minNumParticles and maxNumParticles control the number of particles that are
        // added when AddParticles is called. The number of particles will be a random
        // number between minNumParticles and maxNumParticles.
        protected int minNumParticles;
        protected int maxNumParticles;
       
        // This controls the bitmap that the particle system uses.
        protected string bitmapFilename;

        // minInitialSpeed and maxInitialSpeed are used to control the initial velocity
        // of the particles. The particle's initial speed will be a random number between these two.
        // The direction is determined by the function PickRandomDirection, which can be overriden.
        protected float minInitialSpeed;
        protected float maxInitialSpeed;

        // minAcceleration and maxAcceleration are used to control the acceleration of the particles.
        // The particle's acceleration will be a random number between these two. By default, the
        // direction of acceleration is the same as the direction of the initial velocity.
        protected float minAcceleration;
        protected float maxAcceleration;

        // minRotationSpeed and maxRotationSpeed control the particles' angular velocity: the
        // speed at which particles will rotate. Each particle's rotation speed will be a random
        // number between minRotationSpeed and maxRotationSpeed. Use smaller numbers to make
        // particle systems look calm and wispy, and large numbers for more violent effects.
        protected float minRotationSpeed;
        protected float maxRotationSpeed;

        // minLifetime and maxLifetime are used to control the lifetime. Each particle's lifetime
        // will be a random number between these two. Lifetime is used to determine how long a
        // particle "lasts." Also, in the base implementation of Draw, lifetime is also used to
        // calculate alpha and scale values to avoid particles suddenly "popping" into view
        protected float minLifetime;
        protected float maxLifetime;

        // to get some additional variance in the appearance of the particles, we give them all
        // random scales. the scale is a value between minScale and maxScale, and is additionally
        // affected by the particle's lifetime to avoid particles "popping" into view.
        protected float minScale;
        protected float maxScale;

        // Different effects can use different blend states.
        // Fire and explosions work well with additive blending, for example.
        protected CanvasBlend blendState;

        #endregion
        

        // Constructs a new ParticleSystem.
        protected ParticleSystem()
        {
            InitializeConstants();
        }


        // This abstract function must be overriden by subclasses of ParticleSystem. It is
        // here that they should set all the constants marked in the region "constants to
        // be set by subclasses", which give each ParticleSystem its specific flavor.
        protected abstract void InitializeConstants();


        // Loads the bitmap that will be used to draw this particle system.
        public virtual async Task CreateResourcesAsync(ICanvasResourceCreator resourceCreator)
        {
            bitmap = await CanvasBitmap.LoadAsync(resourceCreator, bitmapFilename);

            bitmapCenter = bitmap.Size.ToVector2() / 2;
            bitmapBounds = bitmap.Bounds;
        }


        // AddParticles's job is to add an effect somewhere on the screen.
        public void AddParticles(Vector2 where)
        {
            // The number of particles we want for this effect is a random number
            // somewhere between the two constants specified by the subclasses.
            int numParticles = Utils.Random.Next(minNumParticles, maxNumParticles);

            // Activate that many particles.
            for (int i = 0; i < numParticles; i++)
            {
                // Grab a particle from the freeParticles store, or create a new one if freeParticles is empty.
                Particle particle = (freeParticles.Count > 0) ? freeParticles.Pop() : new Particle();

                InitializeParticle(particle, where);

                activeParticles.Add(particle);
            }
        }


        // Randomizes some properties for a particle, then calls Initialize on it.
        // This can be overriden by subclasses if they  want to modify the way particles
        // are created. For example, SmokePlumeParticleSystem overrides this function
        // make all particles accelerate to the right, simulating wind.
        protected virtual void InitializeParticle(Particle particle, Vector2 where)
        {
            // First, call PickRandomDirection to figure out which way the particle
            // will be moving. Velocity and acceleration values will come from this.
            Vector2 direction = PickRandomDirection();

            // Pick some random values for our particle.
            float velocity = Utils.RandomBetween(minInitialSpeed, maxInitialSpeed);
            float acceleration = Utils.RandomBetween(minAcceleration, maxAcceleration);
            float lifetime = Utils.RandomBetween(minLifetime, maxLifetime);
            float scale = Utils.RandomBetween(minScale, maxScale);
            float rotationSpeed = Utils.RandomBetween(minRotationSpeed, maxRotationSpeed);

            // Then initialize the particle with these random values.
            particle.Initialize(where, velocity * direction, acceleration * direction, lifetime, scale, rotationSpeed);
        }


        // PickRandomDirection is used by InitializeParticles to decide which direction
        // particles will move. The default implementation is a random vector on a circle.
        protected virtual Vector2 PickRandomDirection()
        {
            float angle = Utils.RandomBetween(0, (float)Math.PI * 2);

            return new Vector2((float)Math.Cos(angle),
                               (float)Math.Sin(angle));
        }


        // Updates all of the active particles.
        public void Update(float elapsedTime)
        {
            // Go through the active particles in reverse order, so our loop
            // index stays valid even when we decide to remove some of them.
            for (int i = activeParticles.Count - 1; i >= 0; i--)
            {
                Particle particle = activeParticles[i];

                if (!particle.Update(elapsedTime))
                {
                    // If the particle is no longer active, move it from activeParticles to freeParticles.
                    activeParticles.RemoveAt(i);
                    freeParticles.Push(particle);
                }
            }
        }


        // Draws all of the active particles.
        public void Draw(CanvasDrawingSession drawingSession)
        {
            var previousBlend = drawingSession.Blend;

            drawingSession.Blend = blendState;

            // Go through the particles in reverse order, so new ones are drawn underneath
            // older ones. This improves visual appearance of effects like smoke plume
            // where many particles are created at the same position over a period of time.
            for (int i = activeParticles.Count - 1; i >= 0; i--)
            {
                Particle particle = activeParticles[i];

                // Normalized lifetime is a value from 0 to 1 and represents how far a particle
                // is through its life. 0 means it just started, .5 is half way through, and
                // 1.0 means it's just about to be finished. This value will be used to calculate
                // alpha and scale, to avoid  having particles suddenly appear or disappear.
                float normalizedLifetime = particle.TimeSinceStart / particle.Lifetime;

                // We want particles to fade in and fade out, so we'll calculate alpha to be
                // (normalizedLifetime) * (1 - normalizedLifetime). This way, when normalizedLifetime
                // is 0 or 1, alpha is 0. The maximum value is at normalizedLifetime = .5, and is:
                //
                //      (normalizedLifetime) * (1-normalizedLifetime)
                //      (.5)                 * (1-.5)
                //      .25
                //
                // Since we want the maximum alpha to be 1, not .25, we'll scale the entire equation by 4.
                float alpha = 4 * normalizedLifetime * (1 - normalizedLifetime);

                // Make particles grow as they age.
                // They'll start at 75% of their size, and increase to 100% once they're finished.
                float scale = particle.Scale * (.75f + .25f * normalizedLifetime);

                // Compute a transform matrix for this particle.
                var transform = Matrix3x2.CreateRotation(particle.Rotation, bitmapCenter) *
                                Matrix3x2.CreateScale(scale, bitmapCenter) *
                                Matrix3x2.CreateTranslation(particle.Position - bitmapCenter);

                // Draw the particle.
                drawingSession.DrawImage(bitmap, 0, 0, bitmapBounds, alpha, CanvasImageInterpolation.Linear, new Matrix4x4(transform));
            }

            drawingSession.Blend = previousBlend;
        }
    }
}
