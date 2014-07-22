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

namespace canvas
{
    class MockEvent
    {
    public:
        MockEvent()
          : refCount(0),
            callCount(0),
            lastSender(nullptr),
            lastArgs(0)
        { }

        ULONG AddRef() { return ++refCount; }
        ULONG Release() { return --refCount; }

        HRESULT Invoke(void* sender, int* args)
        {
            callCount++;
            lastSender = sender;
            lastArgs = *args;

            return S_OK;
        }

        ULONG refCount;
        int callCount;
        void* lastSender;
        int lastArgs;
    };


    TEST_CLASS(RegisteredEventListTests)
    {
        TEST_METHOD(RegisteredEventList_AddRemove)
        {
            RegisteredEventList<MockEvent> eventList;

            // List starts empty.
            Assert::IsTrue(eventList.IsEmpty());
            Assert::AreEqual(0, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(0), eventList.m_registeredEvents.size());

            // Add one element.
            MockEvent a;
            EventRegistrationToken aToken = eventList.Add(&a);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(1, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(1), eventList.m_registeredEvents.size());
            Assert::AreEqual(__int64(0), aToken.value);
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[0].Get());

            // Add another element.
            MockEvent b;
            EventRegistrationToken bToken = eventList.Add(&b);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(2, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(2), eventList.m_registeredEvents.size());
            Assert::AreEqual(__int64(1), bToken.value);
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());

            // Add the same handler twice. Should yield different tokens.
            EventRegistrationToken aToken2 = eventList.Add(&a);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(3, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(3), eventList.m_registeredEvents.size());
            Assert::AreEqual(__int64(2), aToken2.value);
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[2].Get());

            // Remove non-existent events. Should be a no-op, and not throw any errors.
            EventRegistrationToken madeUpToken;

            madeUpToken.value = 1234;
            eventList.Remove(madeUpToken);

            madeUpToken.value = 3;
            eventList.Remove(madeUpToken);

            madeUpToken.value = -1;
            eventList.Remove(madeUpToken);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(3, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(3), eventList.m_registeredEvents.size());
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[2].Get());

            // Remove the first event. Should leave a null space in the array.
            eventList.Remove(aToken);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(2, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(3), eventList.m_registeredEvents.size());
            Assert::AreEqual<void*>(nullptr, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[2].Get());

            // Remove the first event again. Should no-op and not throw.
            eventList.Remove(aToken);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(2, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(3), eventList.m_registeredEvents.size());
            Assert::AreEqual<void*>(nullptr, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());
            Assert::AreEqual<void*>(&a, eventList.m_registeredEvents[2].Get());

            // Remove the second copy of the first event. Should shrink the array.
            eventList.Remove(aToken2);

            Assert::IsFalse(eventList.IsEmpty());
            Assert::AreEqual(1, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(2), eventList.m_registeredEvents.size());
            Assert::AreEqual<void*>(nullptr, eventList.m_registeredEvents[0].Get());
            Assert::AreEqual<void*>(&b, eventList.m_registeredEvents[1].Get());

            // Remove the second event. Should empty the array entirely.
            eventList.Remove(bToken);

            Assert::IsTrue(eventList.IsEmpty());
            Assert::AreEqual(0, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(0), eventList.m_registeredEvents.size());

            // Remove non-existent events from the now empty array.
            madeUpToken.value = -1;
            eventList.Remove(madeUpToken);

            madeUpToken.value = 0;
            eventList.Remove(madeUpToken);

            madeUpToken.value = 1;
            eventList.Remove(madeUpToken);

            Assert::IsTrue(eventList.IsEmpty());
            Assert::AreEqual(0, eventList.m_registeredEventCount);
            Assert::AreEqual(size_t(0), eventList.m_registeredEvents.size());
        }

        TEST_METHOD(RegisteredEventList_FireAll)
        {
            RegisteredEventList<MockEvent> eventList;
            MockEvent a, b, c;
            int eventArgs;

            // Fire on an empty list does nothing.
            eventArgs = 1;
            eventList.FireAll(this, &eventArgs);

            Assert::AreEqual(0, a.callCount);
            Assert::AreEqual(0, b.callCount);
            Assert::AreEqual(0, c.callCount);

            // Fire on a list of one item.
            EventRegistrationToken aToken = eventList.Add(&a);

            eventArgs = 2;
            eventList.FireAll(this, &eventArgs);

            Assert::AreEqual(1, a.callCount);
            Assert::AreEqual<void*>(this, a.lastSender);
            Assert::AreEqual(2, a.lastArgs);

            Assert::AreEqual(0, b.callCount);
            Assert::AreEqual(0, c.callCount);

            // Fire on a list of three items, with a added twice (so should be called twice).
            EventRegistrationToken aToken2 = eventList.Add(&a);
            EventRegistrationToken bToken = eventList.Add(&b);
            EventRegistrationToken cToken = eventList.Add(&c);

            eventArgs = 3;
            eventList.FireAll(this, &eventArgs);

            Assert::AreEqual(3, a.callCount);
            Assert::AreEqual<void*>(this, a.lastSender);
            Assert::AreEqual(3, a.lastArgs);

            Assert::AreEqual(1, b.callCount);
            Assert::AreEqual<void*>(this, b.lastSender);
            Assert::AreEqual(3, b.lastArgs);

            Assert::AreEqual(1, c.callCount);
            Assert::AreEqual<void*>(this, c.lastSender);
            Assert::AreEqual(3, c.lastArgs);

            // Fire on a list containing just one copy of a and c, now with holes in the layout (b should not be called).
            eventList.Remove(aToken);
            eventList.Remove(bToken);

            eventArgs = 4;
            eventList.FireAll(this, &eventArgs);

            Assert::AreEqual(4, a.callCount);
            Assert::AreEqual(4, a.lastArgs);

            Assert::AreEqual(1, b.callCount);
            Assert::AreEqual(3, b.lastArgs);

            Assert::AreEqual(2, c.callCount);
            Assert::AreEqual(4, c.lastArgs);
        }
    };
}
