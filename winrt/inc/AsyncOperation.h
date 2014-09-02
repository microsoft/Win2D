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

#include <wrl.h>
#include <wrl\async.h>
#include <Windows.System.Threading.h>
#include <functional>
#include "ErrorHandling.h"


// Implements the WinRT IAsyncOperation interface.
template <typename T>
class AsyncOperation : public Microsoft::WRL::RuntimeClass<
                                  Microsoft::WRL::AsyncBase<ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T*>>,
                                  ABI::Windows::Foundation::IAsyncOperation<T*>>
{
    InspectableClass(L"Windows.Foundation.IAsyncOperation`1", BaseTrust)

    // T_abi is often the same as T*, but if T is a runtime class, T_abi will be the corresponding interface.
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IAsyncOperation::TResult_complex>::type T_abi;

    // Stores the async operation result, once available.
    Microsoft::WRL::ComPtr<T> m_result;


public:
    // Constructor starts the async operation.
    AsyncOperation(std::function<Microsoft::WRL::ComPtr<T>()> const& workerFunction)
    {
        using namespace Microsoft::WRL;
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::System::Threading;;

        Start();
        
        ComPtr<AsyncOperation> keepThisAliveUntilTaskCompletion(this);

        // Crazy typedef marks our threadpool callback as agile, by mixing in FtmBase.
        // Without this WinRT would marshal everything back to the UI thread.
        typedef Implements<RuntimeClassFlags<ClassicCom>, IWorkItemHandler, FtmBase> CallbackType;

        auto threadPoolFunction = Callback<CallbackType>([=](IAsyncAction*)
        {
            // Run the worker function.
            HRESULT hr = ExceptionBoundary([&]()
            {
                m_result = workerFunction();
            });

            // Capture the success or failure state.
            if (SUCCEEDED(hr))
            {
                (void)TryTransitionToCompleted();
            }
            else
            {
                (void)TryTransitionToError(hr);
            }

            // Notify listeners that the task is complete.
            keepThisAliveUntilTaskCompletion->FireCompletion();

            return S_OK;
        });

        CheckMakeResult(threadPoolFunction);

        // Start our task running on the system threadpool.
        ComPtr<IThreadPoolStatics> threadPool;
        ThrowIfFailed(GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(), &threadPool));

        ComPtr<IAsyncAction> threadPoolTask;
        ThrowIfFailed(threadPool->RunAsync(threadPoolFunction.Get(), &threadPoolTask));
    }


    // Gets the result of the async operation.
    virtual HRESULT STDMETHODCALLTYPE GetResults(T_abi* results)
    {
        HRESULT hr = CheckValidStateForResultsCall();

        if (FAILED(hr))
        {
            return hr;
        }

        return m_result.CopyTo(results);
    }


    // Sets the completion callback. If the async operation has already completed, the handler will be called straight away.
    virtual HRESULT STDMETHODCALLTYPE put_Completed(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T*>* handler)
    {
        return PutOnComplete(handler);
    }


    // Gets the completion callback.
    virtual HRESULT STDMETHODCALLTYPE get_Completed(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<T*>** handler)
    {
        return GetOnComplete(handler);
    }


protected:
    // Start notification (unused).
    virtual HRESULT OnStart()
    { 
        return S_OK; 
    }


    // Cancel notification (unused).
    virtual void OnCancel()
    {
    }


    // Close notification.
    virtual void OnClose()
    {
        m_result = nullptr;
    }
};
