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

#include <algorithm>
#include <assert.h>
#include <iterator>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include "WinStringBuilder.h"


// By default, object lifespan tracking is only enabled in debug builds.
// Define ENABLE_LIFESPAN_TRACKER or DISABLE_LIFESPAN_TRACKER to force it on or off.
#if defined _DEBUG && !defined DISABLE_LIFESPAN_TRACKER
#define ENABLE_LIFESPAN_TRACKER
#endif


#ifdef ENABLE_LIFESPAN_TRACKER


// Specialize these to (temporarily!) enable more intrusive lifespan reporting for specific types.
template<typename T> bool TraceAllocations()  { return false; }
template<typename T> bool BreakOnAllocation() { return false; }


// Derive from this to make a type trackable.
template<typename T>
class LifespanTracker
{
protected:
    LifespanTracker()
    {
        LifespanInfo::AddObject(typeid(T), TraceAllocations<T>(), BreakOnAllocation<T>());
    }

    ~LifespanTracker()
    {
        LifespanInfo::RemoveObject(typeid(T), TraceAllocations<T>(), BreakOnAllocation<T>());
    }
};


class LifespanInfo
{
    struct State
    {
        std::mutex mutex;
        std::unordered_map<std::type_index, size_t> objectCounts;
    };

    static State m_state;

    LifespanInfo() = delete;


public:
    // Records that a new object is being allocated.
    static void AddObject(type_info const& type, bool traceAllocations, bool breakOnAllocation)
    {
        std::lock_guard<std::mutex> lock(m_state.mutex);

        size_t objectCount = ++m_state.objectCounts[type];

        TraceAllocation(type, traceAllocations, breakOnAllocation, objectCount, true);
    }


    // Records that an object is being freed.
    static void RemoveObject(type_info const& type, bool traceAllocations, bool breakOnAllocation)
    {
        std::lock_guard<std::mutex> lock(m_state.mutex);

        size_t objectCount = --m_state.objectCounts[type];

        TraceAllocation(type, traceAllocations, breakOnAllocation, objectCount, false);
    }


    // Debug spews the counts of all live objects, and returns the total count.
    static size_t ReportLiveObjects()
    {
        std::lock_guard<std::mutex> lock(m_state.mutex);

        return ReportLiveObjectsNoLock();
    }


    // Debug spews the counts of all live objects, without taking out the usual lock.
    // For use during DLL unload where synchronization primitives are no longer available.
    static size_t ReportLiveObjectsNoLock()
    {
        typedef std::pair<std::type_index, size_t> pair_t;

        // Filter out only types that have remaining live instances.
        std::vector<pair_t> liveObjects;

        std::copy_if(m_state.objectCounts.begin(), m_state.objectCounts.end(),
                     back_inserter(liveObjects),
                     [](pair_t const& typeAndCount) { return typeAndCount.second > 0; });

        // Sort by live instance count (highest first).
        std::sort(liveObjects.begin(), liveObjects.end(),
                  [](pair_t const& a, pair_t const& b)
                  {
                      return (a.second != b.second) ? (a.second > b.second)
                                                    : (strcmp(a.first.name(), b.first.name()) < 0);
                  });

        // Output the results.
        size_t totalLiveObjects = 0;

        if (!liveObjects.empty())
        {
            OutputDebugString(L"Live Win2D objects (may indicate leaks):\n");

            for (auto& typeAndCount : liveObjects)
            {
                WinStringBuilder string;
                string.Format(L"    %hs (%Iu)\n", typeAndCount.first.name(), typeAndCount.second);
                OutputDebugString((wchar_t const*)string.Get());
             
                totalLiveObjects += typeAndCount.second;
            }
        }

        return totalLiveObjects;
    }


    // Wipes all lifespan data. This is used by unit tests to start each test invocation with
    // a clean state, so leaks in one test don't also cause failure reports from subsequent ones.
    static void Reset()
    {
        std::lock_guard<std::mutex> lock(m_state.mutex);

        m_state.objectCounts.clear();
    }


private:
    static void TraceAllocation(type_info const& type, bool traceAllocations, bool breakOnAllocation, size_t objectCount, bool isNew)
    {
        // Optional verbose tracing for alloc and free of selected types.
        if (traceAllocations)
        {
            WinStringBuilder string;
            string.Format(L"LifespanTracker: %s %hs (%Iu)\n", isNew ? L"new" : L"delete", type.name(), objectCount);
            OutputDebugString((wchar_t const*)string.Get());
        }

        // Optional debug break for alloc and free of selected types.
        if (breakOnAllocation)
        {
            __debugbreak();
        }
    }
};


__declspec(selectany) LifespanInfo::State LifespanInfo::m_state;


#else   // ENABLE_LIFESPAN_TRACKER


// No-op implementation for when lifespan tracking is disabled.
template<typename T>
struct LifespanTracker { };

struct LifespanInfo
{
    static size_t ReportLiveObjects()       { return 0; }
    static size_t ReportLiveObjectsNoLock() { return 0; }

    static void Reset() { }
};


#endif  // ENABLE_LIFESPAN_TRACKER
