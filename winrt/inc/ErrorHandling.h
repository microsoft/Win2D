// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

//
// Helpers for error handling.  General strategy is that errors are always
// reported as exceptions (so we don't need to special case any STL usage).
//
// Methods that need to return HRESULTs wrap their body inside an
// ExceptionBoundary.
//
class ComException
{
    HRESULT m_Hr;

public:
    explicit ComException(HRESULT hr)
        : m_Hr(hr)
    {}

    HRESULT GetHr() const
    {
        return m_Hr;
    }
};

template<typename CALLABLE>
HRESULT ExceptionBoundary(CALLABLE&& fn)
{
    try
    {
        fn();
        return S_OK;
    }
    catch (const ComException& e)
    {
        return e.GetHr();
    }
    catch (const std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_UNEXPECTED;
    }
}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
        throw ComException(hr);
}
