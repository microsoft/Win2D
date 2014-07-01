// Copyright (c) Microsoft Corporation.  All rights reserved

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
    explicit __declspec(nothrow)  ScopeWarden(FN* p) : m_p(p)
    {
    }

    void __declspec(nothrow) Dismiss()
    {
        m_p = nullptr;
    }

    __declspec(nothrow) ~ScopeWarden()
    {
        if (m_p)
        {
            try
            {
                (*m_p)();
            }
            catch (...)
            {
                std::terminate();
            }
        }
    }

    ScopeWarden(ScopeWarden&&) = delete;
    ScopeWarden(const ScopeWarden&) = delete;
    ScopeWarden& operator=(const ScopeWarden&) = delete;

private:
    FN* m_p;
};

template<typename FN>
ScopeWarden<FN> MakeScopeWarden(FN&& fn)
{
    return ScopeWarden<FN>(std::addressof(fn));
}

