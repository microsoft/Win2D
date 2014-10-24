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

class RegisteredEvent
{
    std::function<void()> m_unregisterFunction;

public:
    template<typename DELEGATE, typename SOURCE, typename HANDLER>
    RegisteredEvent(
        SOURCE* eventSource,
        HRESULT (STDMETHODCALLTYPE SOURCE::* addMethod)(DELEGATE*, EventRegistrationToken*),
        HRESULT (STDMETHODCALLTYPE SOURCE::* removeMethod)(EventRegistrationToken),
        HANDLER handler)
    {
        EventRegistrationToken token;
        ThrowIfFailed((eventSource->*addMethod)(handler, &token));

        ComPtr<SOURCE> source(eventSource);
        m_unregisterFunction = 
            [source, removeMethod, token]()
            {
                ThrowIfFailed((source.Get()->*removeMethod)(token));
            };
    }

    RegisteredEvent(std::function<void()>&& unregisterFunction)
        : m_unregisterFunction(std::move(unregisterFunction))
    {
    }

    RegisteredEvent()
    {
    }

    RegisteredEvent(RegisteredEvent&& other)
    {
        std::swap(m_unregisterFunction, other.m_unregisterFunction);
    }

    RegisteredEvent& operator=(RegisteredEvent&& other)
    {
        m_unregisterFunction = other.m_unregisterFunction;
        other.m_unregisterFunction = nullptr;
        return *this;
    }

    RegisteredEvent(const RegisteredEvent&) = delete;
    RegisteredEvent& operator=(const RegisteredEvent&) = delete;

    ~RegisteredEvent()
    {
        Release();
    }

    void Detach()
    {
        m_unregisterFunction = nullptr;
    }

    void Release()
    {
        if (m_unregisterFunction)
        {
            m_unregisterFunction();
            m_unregisterFunction = nullptr;
        }
    }

    explicit operator bool()
    {
        return m_unregisterFunction != nullptr;
    }
};
