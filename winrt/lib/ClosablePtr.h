// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace dxrt
{
    //
    // This wraps up a ComPtr to make sure that it cannot be accessed without
    // first checking to see if it has been 'closed'.  This is to help enforce
    // Windows.Foundation.IClosable semantics.
    //
    // Usage:
    //
    //   ClosablePtr<IInspectable> foo = original;
    //
    //   auto& definitelyNotClosedFoo = foo.EnsureNotClosed();
    //
    // Note the "auto&".  This prevents an unnecessary AddRef/Release call.
    //   
    //
    template<class T>
    class ClosablePtr
    {
        Microsoft::WRL::ComPtr<T> m_Ptr;

    public:
        ClosablePtr()
        {
        }

        ClosablePtr(const ClosablePtr& other)
            : m_Ptr(other.m_Ptr)
        {
        }

        ClosablePtr(T* other)
            : m_Ptr(other)
        {
        }

        ClosablePtr& operator=(const ClosablePtr& other)
        {
            m_Ptr = other.m_Ptr;
            return *this;
        }

        ClosablePtr& operator=(const Microsoft::WRL::ComPtr<T>& other)
        {
            m_Ptr = other;
            return *this;
        }

        ClosablePtr& operator=(T* other)
        {
            m_Ptr = other;
            return *this;
        }

        void Close() throw()
        {
            m_Ptr.Reset();
        }

        const Microsoft::WRL::ComPtr<T>& EnsureNotClosed() const
        {
            if (!m_Ptr)
                throw ComException(RO_E_CLOSED);
            
            //
            // There's a potential race condition here -- if another thread
            // calls Close() before we manage to return (and the caller uses
            // m_Ptr) then we're likely to AV.  
            //
            // This would be an application bug -- it's the moral equivalent of
            // calling 'delete foo' on one thread while trying to use 'foo' on
            // another.
            //
            // We do not try and detect or handle this case.
            //

            return m_Ptr;
        }        
    };
}
