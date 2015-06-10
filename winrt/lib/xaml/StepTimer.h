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

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    class ICanvasTimingAdapter
    {
    public:
        virtual ~ICanvasTimingAdapter() = default;

        virtual int64_t GetPerformanceCounter() = 0;
        virtual int64_t GetPerformanceFrequency() = 0;
    };

    // Helper class for animation and simulation timing.
    class StepTimer
    {
        std::shared_ptr<ICanvasTimingAdapter> m_adapter;

        // Source timing data uses QPC units.
        int64_t m_frequency;
        int64_t m_lastTime;
        uint64_t m_maxDelta;

        // Derived timing data uses a canonical tick format.
        uint64_t m_elapsedTicks;
        uint64_t m_totalTicks;
        uint64_t m_leftOverTicks;

        // Members for tracking the framerate.
        uint32_t m_frameCount;
        uint32_t m_framesPerSecond;
        uint32_t m_framesThisSecond;
        uint64_t m_secondCounter;

        // Members for configuring fixed timestep mode.
        bool m_isFixedTimeStep;
        uint64_t m_targetElapsedTicks;

    public:
        StepTimer(std::shared_ptr<ICanvasTimingAdapter> adapter);

        // Get elapsed time since the previous Update call.
        uint64_t GetElapsedTicks() const                        
        { 
            return m_elapsedTicks; 
        }

        double GetElapsedSeconds() const                    
        { 
            return TicksToSeconds(m_elapsedTicks); 
        }

        // Get total time since the start of the program.
        uint64_t GetTotalTicks() const                        
        { 
            return m_totalTicks; 
        }

        double GetTotalSeconds() const                        
        { 
            return TicksToSeconds(m_totalTicks); 
        }

        // Get total number of updates since start of the program.
        uint32_t GetFrameCount() const                        
        { 
            return m_frameCount; 
        }

        // Get the current framerate.
        uint32_t GetFramesPerSecond() const                    
        { 
            return m_framesPerSecond; 
        }

        // Set whether to use fixed or variable timestep mode.
        void SetFixedTimeStep(bool isFixedTimestep)            
        { 
            m_isFixedTimeStep = isFixedTimestep; 
        }

        bool IsFixedTimeStep()                              
        { 
            return m_isFixedTimeStep; 
        }

        // Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTicks(uint64_t targetElapsed)    
        {
            m_targetElapsedTicks = targetElapsed; 
        }

        void SetTargetElapsedSeconds(double targetElapsed)    
        { 
            m_targetElapsedTicks = SecondsToTicks(targetElapsed); 
        }

        uint64_t GetTargetElapsedTicks() 
        { 
            return m_targetElapsedTicks; 
        }

        // Integer format represents time using 10,000,000 ticks per second.
        static const uint64_t TicksPerSecond = 10000000;

        static const uint64_t DefaultTargetElapsedTime = TicksPerSecond / 60;

        static double TicksToSeconds(uint64_t ticks)            
        { 
            return static_cast<double>(ticks) / TicksPerSecond; 
        }

        static double TicksToMilliseconds(uint64_t ticks)
        {
            return TicksToSeconds(ticks) * 1000;
        }

        static uint64_t SecondsToTicks(double seconds)        
        { 
            return static_cast<uint64_t>(seconds * TicksPerSecond); 
        }

        // After an intentional timing discontinuity (for instance a blocking IO operation)
        // call this to avoid having the fixed timestep logic attempt a set of catch-up 
        // Update calls.

        void ResetElapsedTime();

        // Update timer state, calling the specified Update function the appropriate number of times.
        template<typename CALLABLE>
        void Tick(bool forceUpdate, CALLABLE&& fn)
        {
            // Query the current time.
            auto currentTime = m_adapter->GetPerformanceCounter();

            uint64_t timeDelta = currentTime - m_lastTime;

            m_lastTime = currentTime;
            m_secondCounter += timeDelta;

            // Clamp excessively large time deltas (e.g. after paused in the debugger).
            if (timeDelta > m_maxDelta)
            {
                timeDelta = m_maxDelta;
            }

            // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
            timeDelta *= TicksPerSecond;
            timeDelta /= m_frequency;

            uint32_t lastFrameCount = m_frameCount;

            bool isRunningSlowly = false;

            if (m_isFixedTimeStep)
            {
                // Fixed timestep update logic

                // If the app is running very close to the target elapsed time
                // (within 1/4 of a millisecond) just clamp the clock to exactly
                // match the target value. This prevents tiny and irrelevant
                // errors from accumulating over time. Without this clamping, a
                // game that requested a 60 fps fixed update, running with vsync
                // enabled on a 59.94 NTSC display, would eventually accumulate
                // enough tiny errors that it would drop a frame. It is better
                // to just round small deviations down to zero to leave things
                // running smoothly.

                if (llabs(static_cast<uint64_t>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
                {
                    timeDelta = m_targetElapsedTicks;
                }

                m_leftOverTicks += timeDelta;

                //
                // If there would be at least one additional update fired, this
                // indicates the game loop is running slowly.
                //
                isRunningSlowly = m_leftOverTicks >= m_targetElapsedTicks * 2;

                while (m_leftOverTicks >= m_targetElapsedTicks)
                {
                    forceUpdate = false;
                    m_elapsedTicks = m_targetElapsedTicks;
                    m_totalTicks += m_targetElapsedTicks;
                    m_leftOverTicks -= m_targetElapsedTicks;
                    m_frameCount++;

                    fn(isRunningSlowly);
                }

                if (forceUpdate)
                {
                    m_frameCount++;
                    assert(!isRunningSlowly);
                    fn(false);
                }
            }
            else
            {
                // Variable timestep update logic.
                m_elapsedTicks = timeDelta;
                m_totalTicks += timeDelta;
                m_leftOverTicks = 0;
                m_frameCount++;

                fn(false);
            }

            // Track the current framerate.
            if (m_frameCount != lastFrameCount)
            {
                m_framesThisSecond++;
            }

            if (m_secondCounter >= static_cast<uint64_t>(m_frequency))
            {
                m_framesPerSecond = m_framesThisSecond;
                m_framesThisSecond = 0;
                m_secondCounter %= m_frequency;
            }
        }
    };
}}}}}}
