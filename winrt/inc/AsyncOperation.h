// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <wrl.h>
#include <wrl\async.h>
#include <Windows.System.Threading.h>
#include <functional>
#include "ErrorHandling.h"
#include "LifespanTracker.h"


// Helper for marking our callback delegates as agile, by mixing in FtmBase.
// Without this WinRT would marshal everything back to the UI thread.
template<typename T>
struct AddFtmBase
{
    typedef Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, T, Microsoft::WRL::FtmBase> Type;
};


// Traits helper for deducing the type of the completion handler delegate,
// when given either an IAsyncOperation<T> or an IAsyncAction.
template<typename T>
struct AsyncCompletedHandlerType { };

template<typename TResult>
struct AsyncCompletedHandlerType<ABI::Windows::Foundation::IAsyncOperation<TResult>>
{
    typedef ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TResult> Type;
};

template<>
struct AsyncCompletedHandlerType<ABI::Windows::Foundation::IAsyncAction>
{
    typedef ABI::Windows::Foundation::IAsyncActionCompletedHandler Type;
};


// Common implementation code shared between AsyncOperation and AsyncAction.
template<typename T>
class AsyncCommon : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::AsyncBase<typename AsyncCompletedHandlerType<T>::Type>, T>
{
protected:
    AsyncCommon()
    {
        ThrowIfFailed(Start());
    }


    // Runs an async operation on the threadpool.
    template<typename TFunction>
    void RunOnThreadPool(TFunction&& workerFunction)
    {
        auto threadPoolDelegate = MakeThreadPoolDelegate(std::move(workerFunction));

        StartThreadPoolDelegate(threadPoolDelegate);
    }


    // Runs one async operation as a continuation of another. The specified
    // worker function will execute after the previous operation has completed.
    template<typename TPrevious, typename TFunction>
    void RunAsContinuation(Microsoft::WRL::ComPtr<TPrevious> const& previousOperation, TFunction&& workerFunction)
    {
        using namespace Microsoft::WRL;

        // We aren't ready to run this yet, but go ahead and create a callback which will 
        // later be executed on the threadpool. This captures and hangs onto necessary 
        // state such as workerFunction and a keepalive reference to 'this'.
        auto threadPoolDelegate = MakeThreadPoolDelegate(std::move(workerFunction));

        // Create a continuation callback to tell us when previousOperation has finished.
        typedef AddFtmBase<AsyncCompletedHandlerType<TPrevious>::Type>::Type CallbackType;

        auto continuation = Callback<CallbackType>([=](TPrevious*, AsyncStatus status)
        {
            // Early out if we are already in an inactive state (eg. cancelled).
            if (!ContinueAsyncOperation())
            {
                FireCompletion();
                return S_OK;
            }

            HRESULT hr;

            if (status == AsyncStatus::Completed)
            {
                // If previousOperation succeeded, start our continuation on the threadpool.
                hr = ExceptionBoundary([&]
                {
                    StartThreadPoolDelegate(threadPoolDelegate);
                });
            }
            else
            {
                // Look up why previousOperation failed.
                ComPtr<IAsyncInfo> asyncInfo;

                if (SUCCEEDED(hr = previousOperation.As(&asyncInfo)))
                {
                    HRESULT errorCode;

                    if (SUCCEEDED(hr = asyncInfo->get_ErrorCode(&errorCode)))
                    {
                        hr = FAILED(errorCode) ? errorCode : E_UNEXPECTED;
                    }
                }
            }

            // Either we are now running on the threadpool, or we must report that something went wrong.
            if (FAILED(hr))
            {
                (void)TryTransitionToError(hr);
                FireCompletion();
            }

            return S_OK;
        });

        CheckMakeResult(continuation);
        
        // Register our continuation to execute after previousOperation.
        ThrowIfFailed(previousOperation->put_Completed(continuation.Get()));
    }


private:
    // Creates a callback delegate which will later execute the specified worker function.
    template<typename TFunction>
    Microsoft::WRL::ComPtr<ABI::Windows::System::Threading::IWorkItemHandler> MakeThreadPoolDelegate(TFunction const& workerFunction)
    {
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::System::Threading;
        using namespace Microsoft::WRL;

        ComPtr<AsyncCommon> keepThisAliveUntilTaskCompletion(this);

        typedef AddFtmBase<IWorkItemHandler>::Type CallbackType;
        
        auto threadPoolDelegate = Callback<CallbackType>([=](IAsyncAction*)
        {
            // Run the worker function.
            HRESULT hr = ExceptionBoundary([&]
            {
                workerFunction();
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

        CheckMakeResult(threadPoolDelegate);

        return threadPoolDelegate;
    }


    // Starts a task running on the system threadpool.
    void StartThreadPoolDelegate(Microsoft::WRL::ComPtr<ABI::Windows::System::Threading::IWorkItemHandler> const& threadPoolDelegate)
    {
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::System::Threading;
        using namespace Microsoft::WRL;

        ComPtr<IThreadPoolStatics> threadPool;
        ThrowIfFailed(GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(), &threadPool));

        ComPtr<IAsyncAction> threadPoolTask;
        ThrowIfFailed(threadPool->RunAsync(threadPoolDelegate.Get(), &threadPoolTask));
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


public:
    // Sets the completion callback. If the async operation has already completed, the handler will be called straight away.
    virtual HRESULT STDMETHODCALLTYPE put_Completed(typename AsyncCompletedHandlerType<T>::Type* handler)
    {
        return PutOnComplete(handler);
    }


    // Gets the completion callback.
    virtual HRESULT STDMETHODCALLTYPE get_Completed(typename AsyncCompletedHandlerType<T>::Type** handler)
    {
        return GetOnComplete(handler);
    }
};


// Implements the WinRT IAsyncOperation interface.
template<typename T>
class AsyncOperation : public AsyncCommon<ABI::Windows::Foundation::IAsyncOperation<T*>>,
                       private LifespanTracker<AsyncOperation<T>>
{
    InspectableClass(IAsyncOperation<T*>::z_get_rc_name_impl(), BaseTrust);

    // T_abi is often the same as T*, but if T is a runtime class, T_abi will be the corresponding interface.
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename AsyncCommon::IAsyncOperation::TResult_complex>::type T_abi;

    // Stores the async operation result, once available.
    Microsoft::WRL::ComPtr<T> m_result;


public:
    // Runs an async operation on the threadpool.
    AsyncOperation(std::function<Microsoft::WRL::ComPtr<T>()>&& workerFunction)
    {
        RunOnThreadPool([=]
        {
            m_result = workerFunction();
        });
    }


    // Runs one async operation as a continuation of another. The specified
    // worker function will execute after the previous operation has completed.
    template<typename TPrevious>
    AsyncOperation(Microsoft::WRL::ComPtr<TPrevious> const& previousOperation, std::function<Microsoft::WRL::ComPtr<T>()>&& workerFunction)
    {
        RunAsContinuation(previousOperation, [=]
        {
            m_result = workerFunction();
        });
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


protected:
    // Close notification.
    virtual void OnClose()
    {
        m_result = nullptr;
    }
};


// Implements the WinRT IAsyncAction interface.
class AsyncAction : public AsyncCommon<ABI::Windows::Foundation::IAsyncAction>,
                    private LifespanTracker<AsyncAction>
{
    InspectableClass(InterfaceName_Windows_Foundation_IAsyncAction, BaseTrust);


public:
    // Runs an async action on the threadpool.
    AsyncAction(std::function<void()>&& workerFunction)
    {
        RunOnThreadPool(std::move(workerFunction));
    }


    // Gets the result of the async action.
    virtual HRESULT STDMETHODCALLTYPE GetResults()
    {
        return CheckValidStateForResultsCall();
    }


protected:
    // Close notification (unused).
    virtual void OnClose()
    {
    }
};
