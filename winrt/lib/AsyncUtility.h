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

// Code extracted from http://cxxreflect.codeplex.com/SourceControl/latest#cxxreflect/windows_runtime/utility.hpp

#pragma once

template <typename T>
class async_operation_base
    : public ::Microsoft::WRL::RuntimeClass<
    ::Microsoft::WRL::AsyncBase< ::ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T*>>,
    ::ABI::Windows::Foundation::IAsyncOperation<T*>
    >
{
    InspectableClass(L"Windows.Foundation.IAsyncInfo", BaseTrust)

public:

    typedef ::ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T*> HandlerType;

    async_operation_base()
    {
        this->Start();
    }

    virtual auto STDMETHODCALLTYPE put_Completed(HandlerType* handler) -> HRESULT override
    {
        return this->PutOnComplete(handler);
    }

    virtual auto STDMETHODCALLTYPE get_Completed(HandlerType** handler) -> HRESULT override
    {
        return this->GetOnComplete(handler);
    }

protected:

    virtual auto OnStart()  -> HRESULT override { return S_OK; }
    virtual auto OnClose()  -> void    override { };
    virtual auto OnCancel() -> void    override { };
};

/// An IAsyncOperation<T> implementation that returns an already-realized value
template <typename T>
class already_completed_async_operation
    : public async_operation_base<T>
{
public:

    already_completed_async_operation(T* const value)
        : _value(value)
    {
        this->FireCompletion();
    }

    virtual auto STDMETHODCALLTYPE GetResults(T** results)  -> HRESULT override
    {
        if (results == nullptr)
            return E_INVALIDARG;

        *results = _value.Get();
        return S_OK;
    }

private:

    ::Microsoft::WRL::ComPtr<T> _value;
};

/// An IAsyncOperation<T> implementation that waits on a std::future using PPL
template <typename T>
class task_based_async_operation
    : public async_operation_base<T>
{
public:

    task_based_async_operation(std::future<T*>&& f)
        : _future(std::move(f)), _task([&]() -> ::Microsoft::WRL::ComPtr<T> { return _future.get(); })
    {
        _task.then([&](::Microsoft::WRL::ComPtr<T>)
        {
            this->FireCompletion();
        });
    }

    virtual auto STDMETHODCALLTYPE GetResults(T** results)  -> HRESULT override
    {
        if (results == nullptr)
            return E_INVALIDARG;

        *results = clone_for_return(_task.get());
        return S_OK;
    }

private:

    std::future<T*>                                  _future;
    ::Concurrency::task<::Microsoft::WRL::ComPtr<T>> _task;
};

/// A helper to AddRef an interface pointer for return to a caller.
template <typename T>
auto clone_for_return(::Microsoft::WRL::ComPtr<T> p) -> T*
{
    return p.Detach();
}