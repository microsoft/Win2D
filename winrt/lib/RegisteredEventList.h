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

namespace canvas
{
    template<class EVENT_TYPE>
    class RegisteredEventList
    {
        std::vector<ComPtr<EVENT_TYPE>> m_registeredEvents;
        int m_registeredEventCount; // Separate, because there may be empty spots in the array.

        friend class RegisteredEventListTests;

    public:

        RegisteredEventList()
            : m_registeredEventCount(0) 
            {}

        EventRegistrationToken Add(EVENT_TYPE* eventHandler)
        {
            // Convention: the token is that event's index into the array.
            EventRegistrationToken token;

            // Find the first empty index in the array, or add it to the end.
            for (unsigned int i = 0; i < m_registeredEvents.size(); i++)
            {
                if (!m_registeredEvents[i])
                {
                    m_registeredEvents[i] = eventHandler;
                    m_registeredEventCount++;
                    token.value = static_cast<__int64>(i);
                    return token;
                }
            }

            m_registeredEvents.push_back(eventHandler);
            m_registeredEventCount++;
            token.value = static_cast<__int64>(m_registeredEvents.size() - 1);
            return token;
        }

        void Remove(const EventRegistrationToken& token)
        {
            // Check for non-registered events. These cause a no-op.
            if (token.value < 0 || token.value >= (__int64)m_registeredEvents.size())
            {
                return; 
            }
            unsigned int index = static_cast<unsigned int>(token.value);
            if (!m_registeredEvents[index])
            {
                return;
            }

            assert(m_registeredEventCount > 0);
            m_registeredEventCount--;

            m_registeredEvents[index].Reset();

            while (!m_registeredEvents.empty() && !m_registeredEvents.back())
            {
                m_registeredEvents.pop_back();
            }
        }

        template<class SENDER_TYPE, class EVENT_ARGS>
        void FireAll(SENDER_TYPE* sender, EVENT_ARGS* eventArgs)  const
        {
            if (m_registeredEventCount == 0) return;

            //
            // TODO #1756. It may not be sufficient to simply iterate over the collection and invoke all the 
            // events, because of a race condition where an event handler could itself register new 
            // events. Investigate whether this is actually possible or worth being concerned about, and
            // whether it's worth copying out the array.
            //
            for (size_t i = 0; i < m_registeredEvents.size(); i++)
            {
                if (m_registeredEvents[i])
                {
                    ThrowIfFailed(m_registeredEvents[i]->Invoke(sender, eventArgs));
                }
            }
        }

        bool IsEmpty() const
        {
            return m_registeredEventCount == 0;
        }
    };
}
