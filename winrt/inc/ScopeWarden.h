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

//
// Adapted from:
//   http://channel9.msdn.com/Series/C9-Lectures-Stephan-T-Lavavej-Advanced-STL/C9-Lectures-Stephan-T-Lavavej-Advanced-STL-6-of-n#c634477472460000000
//
// which is similar to ScopeGuard described here:
//   http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C.
//
// The point of it is to provide a way to ensure that an action is executed,
// even if an exception if thrown.  The Dismiss() method can be used to say
// prevent the action executing.
//
template<typename FN>
class ScopeWarden
{
public:
    explicit __declspec(nothrow) ScopeWarden(FN&& fn)
        : m_fn(fn)
        , m_dismissed(false)
    {
    }

    void __declspec(nothrow) Dismiss()
    {
        m_dismissed = true;
    }

    __declspec(nothrow) ~ScopeWarden()
    {
        if (!m_dismissed)
        {
            try
            {
                m_fn();
            }
            catch (...)
            {
                std::terminate();
            }
        }
    }

    // Copy constructor is public, but not implemented - return-value
    // optimization will mean that any attempt to actually copy one will fail at
    // link time.
    ScopeWarden(ScopeWarden const& other);

    ScopeWarden& operator=(ScopeWarden const&) = delete;

private:
    FN m_fn;
    bool m_dismissed;
};

template<typename FN>
ScopeWarden<FN> MakeScopeWarden(FN&& fn)
{
    return ScopeWarden<FN>(std::move(fn));
}
