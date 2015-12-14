// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
// represented as an HResultException.  Use ExceptionBoundary() to catch
// exceptions and translate them to HRESULTs.
//
// As the constructor is protected, only ThrowHR() can be used to throw an
// HResultException.  This provides future flexibility in case we some day
// want to throw more varied types of exception.
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
// Specialized HResultException for the various device lost errors.
//
class DeviceLostException : public HResultException
{
protected:
    explicit DeviceLostException(HRESULT hr)
        : HResultException(hr)
    {
        assert(IsDeviceLostHResult(hr));
    }

public:
    static bool IsDeviceLostHResult(HRESULT hr)
    {
        switch (hr)
        {
        case DXGI_ERROR_DEVICE_HUNG:
        case DXGI_ERROR_DEVICE_REMOVED:
        case DXGI_ERROR_DEVICE_RESET:
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
        case DXGI_ERROR_INVALID_CALL:
        case D2DERR_RECREATE_TARGET:
            return true;

        default:
            return false;
        }
    }

    __declspec(noreturn)
    friend void ThrowHR(HRESULT);
};

//
// Throws an exception for the given HRESULT.
//
__declspec(noreturn) __declspec(noinline)
inline void ThrowHR(HRESULT hr)
{
    if (DeviceLostException::IsDeviceLostHResult(hr))
        throw DeviceLostException(hr);
    else
        throw HResultException(hr);
}

//
// Throws the appropriate exception for the given HRESULT, attaching a custom error message
// string. To avoid leaks, the message string should be owned by an RAII wrapper such as
// WinString. We don't take this parameter directly as a WinString to break what would
// otherwise be a circular dependency (WinString uses ErrorHandling.h in its implementation).
//
__declspec(noreturn) __declspec(noinline)
inline void ThrowHR(HRESULT hr, HSTRING message)
{
    RoOriginateError(hr, message);
    
    ThrowHR(hr);
}

__declspec(noreturn) __declspec(noinline)
inline void ThrowHR(HRESULT hr, wchar_t const* message)
{
    using ::Microsoft::WRL::Wrappers::HStringReference;

    ThrowHR(hr, HStringReference(message).Get());
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

template<typename T>
inline void ThrowIfNegative(T value)
{
    if (value < 0)
        ThrowHR(E_INVALIDARG);
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
__declspec(noreturn) __declspec(noinline)
inline void ThrowBadAlloc()
{
    throw std::bad_alloc();
}

inline void CheckMakeResult(bool result)
{
    if (!result)
        ThrowBadAlloc();
}

//
// Converts exceptions in the callable code into HRESULTs.
//
__declspec(noinline)
inline HRESULT ThrownExceptionToHResult()
{
    try
    {
        throw;
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

template<typename CALLABLE>
HRESULT ExceptionBoundary(CALLABLE&& fn)
{
    try
    {
        fn();
        return S_OK;
    }
    catch (...)
    {
        return ThrownExceptionToHResult();
    }
}
