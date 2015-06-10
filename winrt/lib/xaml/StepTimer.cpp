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

#include "pch.h"
#include "StepTimer.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;

StepTimer::StepTimer(std::shared_ptr<ICanvasTimingAdapter> adapter)
    : m_adapter(adapter)
    , m_elapsedTicks(0)
    , m_totalTicks(0)
    , m_leftOverTicks(0)
    , m_frameCount(0)
    , m_framesPerSecond(0)
    , m_framesThisSecond(0)
    , m_secondCounter(0)
    , m_isFixedTimeStep(true)
    , m_targetElapsedTicks(DefaultTargetElapsedTime)
{
    m_frequency = m_adapter->GetPerformanceFrequency();

    m_lastTime = m_adapter->GetPerformanceCounter();

    // Initialize max delta to 1/10 of a second.
    m_maxDelta = m_frequency / 10;
    assert(m_maxDelta > 0);
}

void StepTimer::ResetElapsedTime()
{
    m_lastTime = m_adapter->GetPerformanceCounter();

    m_leftOverTicks = 0;
    m_framesPerSecond = 0;
    m_framesThisSecond = 0;
    m_secondCounter = 0;
}