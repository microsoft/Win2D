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
// Helpers for error handling.  General strategy is that errors are always
// reported as exceptions (so we don't need to special case any STL usage).
//
// Methods that need to return HRESULTs wrap their body inside an
// ExceptionBoundary.
//


//
// Generally all errors are reported with an associated HRESULT.  These are
// represented as an HResultException, or a subclass of it.  Use
// ExceptionBoundary() to catch exceptions and translate them to HRESULTs.
//
// As the constructor is protected, only ThrowHR() can be used to throw an
// HResultException.  This allows ThrowHR() to pick the right exception for an
// HRESULT.
//
// This is useful because sometimes (eg test code) it is useful to be able to
// catch a specific HRESULT.  For example, ThrowHR(E_INVALIDARG) will throw
// InvalidArgException rather than HResultException.  This allows test code to
// use Assert::ExpectException<InvalidArgException>.
//
class HResultException
{
    HRESULT m_Hr;

protected:
    explicit HResultException(HRESULT hr)
        : m_Hr(hr)
    {}

public:
    HRESULT GetHr() const
    {
        return m_Hr;
    }

    __declspec(noreturn)
    friend void ThrowHR(HRESULT);
};

//
// Specialized exception type for E_INVALIDARG.  Allows tests to write
// Assert::ExpectException<InvalidArgException>.
//
class InvalidArgException : public HResultException
{
protected:
    InvalidArgException()
        : HResultException(E_INVALIDARG)
    {}

    __declspec(noreturn)
    friend void ThrowHR(HRESULT);
};

class NoSuchInterfaceException : public HResultException
{
public:
    NoSuchInterfaceException()
        : HResultException(E_NOINTERFACE)
    {}

    __declspec(noreturn)
    friend void ThrowHR(HRESULT);
};

class ObjectDisposedException : public HResultException
{
public:
    ObjectDisposedException()
        : HResultException(RO_E_CLOSED)
    {}

    __declspec(noreturn)
    friend void ThrowHR(HRESULT);
};

//
// Throws the appropriate exception for the given HRESULT.
//
__declspec(noreturn)
inline void ThrowHR(HRESULT hr)
{
    switch (hr)
    {
    case E_INVALIDARG:  throw InvalidArgException();
    case E_NOINTERFACE: throw NoSuchInterfaceException();
    case RO_E_CLOSED:   throw ObjectDisposedException();
    default:            throw HResultException(hr);
    }
}

//
// Throws if the HR fails.  This is the workhorse helper for converting calls to
// functions that return HRESULTs to exceptions. eg:
//
//    ThrowIfFailed(myObj->MethodThatReturnsHRESULT());
//
inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
        ThrowHR(hr);
}

//
// Throws if the given pointer is null.
//
template<typename T>
inline void ThrowIfNullPointer(T* ptr, HRESULT hrToThrow)
{
    if (ptr == nullptr)
        ThrowHR(hrToThrow);
}

//
// Checks that a given pointer argument is valid (ie non-null).  This is
// expected to be used at the beginning of methods to validate pointer
// parameters that are marked as [in].
//
template<typename T>
inline void CheckInPointer(T* ptr)
{
    ThrowIfNullPointer(ptr, E_INVALIDARG);
}

//
// Checks that a given output pointer parameter is valid (ie non-null), and sets
// it to null.  This is expected to be used at the beginning of methods to
// validate out pointer parameters that are marked as [out].
//
template<typename T>
inline void CheckAndClearOutPointer(T** ptr)
{
    CheckInPointer(ptr);
    *ptr = nullptr;
}

//
// WRL's Make<>() function returns an empty ComPtr on failure rather than
// throwing an exception.  This checks the result and throws bad_alloc.
//
// Note: generally we use exceptions inside constructors to report errors.
// Therefore the only way that Make() will return an error is if an allocation
// fails.
//
inline void CheckMakeResult(bool result)
{
    if (!result)
        throw std::bad_alloc();
}


//
// Converts exceptions in the callable code into HRESULTs.
//
template<typename CALLABLE>
HRESULT ExceptionBoundary(CALLABLE&& fn)
{
    try
    {
        fn();
        return S_OK;
    }
    catch (HResultException const& e)
    {
        return e.GetHr();
    }
    catch (std::bad_alloc const&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_UNEXPECTED;
    }
}

