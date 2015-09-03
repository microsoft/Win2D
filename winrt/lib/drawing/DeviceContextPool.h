// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "utils/LockUtilities.h"

using namespace Microsoft::WRL;
using namespace ABI::Microsoft::Graphics::Canvas;

class DeviceContextLease;

class DeviceContextPool
{
    ID2D1Device1* m_d2dDevice;

    std::mutex m_mutex;
    std::vector<ComPtr<ID2D1DeviceContext1>> m_deviceContexts;
    
public:
    DeviceContextPool(ID2D1Device1* d2dDevice);

    DeviceContextPool(DeviceContextPool const&) = delete;
    DeviceContextPool& operator=(DeviceContextPool const&) = delete;

    DeviceContextLease TakeLease();

    void Close();

private:
    void ReturnLease(ComPtr<ID2D1DeviceContext1>&& deviceContext);

    friend class DeviceContextLease;
};


class DeviceContextLease
{
    DeviceContextPool* m_owner;
    ComPtr<ID2D1DeviceContext1> m_deviceContext;
    
public:
    DeviceContextLease()
        : m_owner(nullptr)
    {
    }
    
    explicit DeviceContextLease(ComPtr<ID2D1DeviceContext1>&& deviceContext)
        : m_owner(nullptr)
        , m_deviceContext(std::move(deviceContext))
    {
    }

    DeviceContextLease(DeviceContextLease&& other)
        : m_owner(other.m_owner)
        , m_deviceContext(std::move(other.m_deviceContext))
    {
    }
    
    DeviceContextLease& operator=(DeviceContextLease&& other)
    {
        ReturnLease();
        m_owner = other.m_owner;
        m_deviceContext = std::move(other.m_deviceContext);
        return *this;
    }

    DeviceContextLease(DeviceContextLease const&) = delete;
    DeviceContextLease& operator=(DeviceContextLease const&) = delete;

    ~DeviceContextLease()
    {
        ReturnLease();
    }

    ID2D1DeviceContext1* Get()
    {
        return m_deviceContext.Get();
    }
    
    ID2D1DeviceContext1* operator->()
    {
        return m_deviceContext.Get();
    }

private:
    DeviceContextLease(DeviceContextPool* owner, ComPtr<ID2D1DeviceContext1>&& deviceContext)
        : m_owner(owner)
        , m_deviceContext(std::move(deviceContext))
    {
        assert(m_owner);
    }

    void ReturnLease()
    {
        if (m_owner)
        {
            m_owner->ReturnLease(std::move(m_deviceContext));
            m_owner = nullptr;
        }
        else
        {
            m_deviceContext.Reset();
        }
    }

    friend class DeviceContextPool;
};
