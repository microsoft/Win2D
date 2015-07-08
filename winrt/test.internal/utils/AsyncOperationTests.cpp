// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::WRL::Wrappers;


// Fake WinRT interface, used as an async operation result.
class MockAsyncResult
{
public:
    MockAsyncResult()
        : refCount(1)
    { }

    DWORD AddRef()  { return ++refCount; }
    DWORD Release() { return --refCount; };

    int refCount;
};


// Fake interface specializations to make WRL happy.
// In a real app, MIDL would generate a more complete version of this.
namespace ABI {
    namespace Windows {
        namespace Foundation {
            template<> struct __declspec(uuid("853707c5-fd26-499d-aefe-a7d986137939")) IAsyncOperation                <MockAsyncResult*> : IAsyncOperation_impl                <MockAsyncResult*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
            template<> struct __declspec(uuid("40bd86ee-66d9-4623-b144-0df7c81aa9f4")) IAsyncOperationCompletedHandler<MockAsyncResult*> : IAsyncOperationCompletedHandler_impl<MockAsyncResult*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
        }
    }
}


TEST_CLASS(AsyncOperationTests)
{
    // 5 second timeout in case something deadlocks.
    static const int waitTimeout = 5000;


    TEST_METHOD_EX(AsyncCompletesAfterAddingCallbackTest)
    {
        MockAsyncResult expectedResult;
        Event asyncCanFinishNow(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        auto async = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            // Block until the calling code says it's ready for the async operation to complete.
            Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncCanFinishNow.Get(), waitTimeout, false));

            return &expectedResult;
        });

        // Validate initial state of the async object.
        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Started, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async->GetResults(&result));

        Assert::AreEqual(1, expectedResult.refCount);
        
        // One reference from us, plus one from the active async operation.
        AssertExpectedRefCount(async, 2);

        // Subscribe to the completion callback.
        bool hasCallbackFired = false;

        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::IsFalse(hasCallbackFired);
            hasCallbackFired = true;

            Assert::AreEqual(AsyncStatus::Completed, status);
            Assert::IsTrue(IsSameInstance(async.Get(), asyncInfo));

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async->put_Completed(completedCallback.Get()));

        // Callback should not have fired yet!
        Assert::IsFalse(hasCallbackFired);

        // Tell the async operation to complete, then wait until it has.
        SetEvent(asyncCanFinishNow.Get());

        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Validate the completed state.
        Assert::IsTrue(hasCallbackFired);

        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Completed, status);

        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ThrowIfFailed(async->GetResults(&result));
        Assert::AreEqual<void*>(&expectedResult, result.Get());
        
        // Expect one original reference, plus one from the IAsyncOperation holding onto its result object.
        result = nullptr;
        Assert::AreEqual(2, expectedResult.refCount);

        // Expect just one reference now the IAsyncOperation has completed.
        AssertExpectedRefCount(async, 1);

        // Close it, then validate the closed state.
        async->Close();

        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async->get_Status(&status));
        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async->get_ErrorCode(&errorCode));
        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async->GetResults(&result));

        // Closing the IAsyncOperation should release the reference it was holding on the result object.
        Assert::AreEqual(1, expectedResult.refCount);
    }


    TEST_METHOD_EX(AsyncCompletesBeforeAddingCallbackTest)
    {
        MockAsyncResult expectedResult;

        auto async = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            return &expectedResult;
        });

        // Wait for the operation to complete.
        auto startTime = GetTickCount64();
        AsyncStatus status = AsyncStatus::Started;

        while (status != AsyncStatus::Completed)
        {
            Assert::IsTrue(GetTickCount64() < startTime + waitTimeout);
            ThrowIfFailed(async->get_Status(&status));
        }

        // Subscribe to the completion callback.
        bool hasCallbackFired = false;

        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::IsFalse(hasCallbackFired);
            hasCallbackFired = true;

            Assert::AreEqual(AsyncStatus::Completed, status);
            Assert::IsTrue(IsSameInstance(async.Get(), asyncInfo));

            return S_OK;
        });

        ThrowIfFailed(async->put_Completed(completedCallback.Get()));

        // Callback should fire immediately when we add it, since the async operation was already complete.
        Assert::IsTrue(hasCallbackFired);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        ThrowIfFailed(async->GetResults(&result));
        Assert::AreEqual<void*>(&expectedResult, result.Get());

        // Expect one original reference, plus one from the IAsyncOperation holding onto its result object.
        result = nullptr;
        Assert::AreEqual(2, expectedResult.refCount);

        // Expect just one reference now the IAsyncOperation has completed.
        AssertExpectedRefCount(async, 1);
    }


    TEST_METHOD_EX(AsyncReturnsNullTest)
    {
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        auto async = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            return nullptr;
        });

        // Subscribe to the completion callback.
        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::AreEqual(AsyncStatus::Completed, status);
            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async->put_Completed(completedCallback.Get()));

        // Wait until the operation completes.
        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Validate the completed state.
        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Completed, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        ThrowIfFailed(async->GetResults(&result));
        Assert::AreEqual<void*>(nullptr, result.Get());

        // Expect just one reference now the IAsyncOperation has completed.
        AssertExpectedRefCount(async, 1);
    }


    TEST_METHOD_EX(AsyncThrowsExceptionTest)
    {
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        auto async = Make<AsyncOperation<MockAsyncResult>>([&]() -> ComPtr<MockAsyncResult>
        {
            ThrowHR(E_INVALIDARG);
        });

        // Subscribe to the completion callback.
        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::AreEqual(AsyncStatus::Error, status);
            Assert::IsTrue(IsSameInstance(async.Get(), asyncInfo));

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async->put_Completed(completedCallback.Get()));

        // Wait until the operation completes.
        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Validate the error state.
        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Error, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(E_INVALIDARG, errorCode);

        ComPtr<MockAsyncResult> result;
        Assert::AreEqual(E_INVALIDARG, async->GetResults(&result));

        // Expect just one reference now the IAsyncOperation has completed.
        AssertExpectedRefCount(async, 1);
    }


    TEST_METHOD_EX(AsyncCanceledTest)
    {
        MockAsyncResult expectedResult;
        Event asyncCanFinishNow(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        auto async = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            // Block until the calling code says it's ready for the async operation to complete.
            Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncCanFinishNow.Get(), waitTimeout, false));

            return &expectedResult;
        });

        // Subscribe to the completion callback.
        bool hasCallbackFired = false;

        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::IsFalse(hasCallbackFired);
            hasCallbackFired = true;

            Assert::AreEqual(AsyncStatus::Canceled, status);

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async->put_Completed(completedCallback.Get()));

        // Cancelling the async operation should not eagerly fire the completed event, but should report canceled state.
        async->Cancel();
        
        Assert::IsFalse(hasCallbackFired);

        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Canceled, status);

        // Tell the async worker thread to complete, then wait until it has.
        SetEvent(asyncCanFinishNow.Get());

        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Now we should observe the canceled state.
        Assert::IsTrue(hasCallbackFired);

        ThrowIfFailed(async->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Canceled, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async->GetResults(&result));

        // Expect just one reference now the IAsyncOperation has completed.
        AssertExpectedRefCount(async, 1);
    }


    TEST_METHOD_EX(AsyncContinuationTest)
    {
        MockAsyncResult result1, result2;
        
        Event asyncCanFinishNow(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        // First async operation.
        auto async1 = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncCanFinishNow.Get(), waitTimeout, false));

            return &result1;
        });

        // Validate refcounts.
        AssertExpectedRefCount(async1, 2);      // One from us, plus one from the active async operation.
        Assert::AreEqual(1, result1.refCount);

        // Second async operation is registered as a continuation of the first.
        auto async2 = Make<AsyncOperation<MockAsyncResult>>(As<IAsyncOperation<MockAsyncResult*>>(async1), [&, async1]
        {
            // Validate the result of the first async operation. This is important to make sure the continuation
            // lambda has captured that previous async, which mimics how this will be used by real code.
            ComPtr<MockAsyncResult> result;
            ThrowIfFailed(async1->GetResults(&result));
            Assert::AreEqual<void*>(&result1, result.Get());

            return &result2;
        });

        // Validate refcounts.
        AssertExpectedRefCount(async1, 4);      // One from us, one from the active async operation, and two continuation backlinks.
        AssertExpectedRefCount(async2, 2);      // One from us, plus one from the active async operation.
        Assert::AreEqual(1, result1.refCount);
        Assert::AreEqual(1, result2.refCount);

        // Subscribe to the completion callback of the second async operation.
        bool hasCallbackFired = false;

        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::IsFalse(hasCallbackFired);
            hasCallbackFired = true;

            Assert::AreEqual(AsyncStatus::Completed, status);
            Assert::IsTrue(IsSameInstance(async2.Get(), asyncInfo));

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async2->put_Completed(completedCallback.Get()));

        // Callback should not have fired yet!
        Assert::IsFalse(hasCallbackFired);

        // Telling the first async operation to complete should trigger the second one to run as its continuation.
        SetEvent(asyncCanFinishNow.Get());

        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Validate the completed state.
        Assert::IsTrue(hasCallbackFired);

        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async1->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Completed, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async1->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        ThrowIfFailed(async1->GetResults(&result));
        Assert::AreEqual<void*>(&result1, result.Get());

        ThrowIfFailed(async2->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Completed, status);

        ThrowIfFailed(async2->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ThrowIfFailed(async2->GetResults(&result));
        Assert::AreEqual<void*>(&result2, result.Get());

        // Validate refcounts.
        result = nullptr;

        Assert::AreEqual(2, result1.refCount);
        Assert::AreEqual(2, result2.refCount);

        async1->Close();
        async2->Close();

        Assert::AreEqual(1, result1.refCount);
        Assert::AreEqual(1, result2.refCount);

        AssertExpectedRefCount(async1, 1);
        AssertExpectedRefCount(async2, 1);
    }


    TEST_METHOD_EX(AsyncContinuationThrowsExceptionTest)
    {
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        // First async operation.
        auto async1 = Make<AsyncOperation<MockAsyncResult>>([&]() -> ComPtr<MockAsyncResult>
        {
            ThrowHR(E_BOUNDS);
        });

        // Second async operation is registered as a continuation of the first.
        auto async2 = Make<AsyncOperation<MockAsyncResult>>(As<IAsyncOperation<MockAsyncResult*>>(async1), [&]
        {
            Assert::Fail(L"Should never execute since the first operation failed.");
            return nullptr;
        });

        // Subscribe to the completion callback of the second async operation.
        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::AreEqual(AsyncStatus::Error, status);
            Assert::IsTrue(IsSameInstance(async2.Get(), asyncInfo));

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async2->put_Completed(completedCallback.Get()));

        // Wait until the operation completes.
        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // Validate the completed state.
        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async1->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Error, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async1->get_ErrorCode(&errorCode));
        Assert::AreEqual(E_BOUNDS, errorCode);

        ComPtr<MockAsyncResult> result;
        Assert::AreEqual(E_BOUNDS, async1->GetResults(&result));

        // The continuation async should inherit the error state, even though its lambda never actually ran.
        ThrowIfFailed(async2->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Error, status);

        ThrowIfFailed(async2->get_ErrorCode(&errorCode));
        Assert::AreEqual(E_BOUNDS, errorCode);

        Assert::AreEqual(E_BOUNDS, async2->GetResults(&result));

        // Validate refcounts.
        AssertExpectedRefCount(async1, 1);
        AssertExpectedRefCount(async2, 1);
    }


    TEST_METHOD_EX(AsyncContinuationCancelledTest)
    {
        Event asyncCanFinishNow(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        Event asyncFinished(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));

        // First async operation.
        auto async1 = Make<AsyncOperation<MockAsyncResult>>([&]
        {
            Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncCanFinishNow.Get(), waitTimeout, false));

            return nullptr;
        });

        // Second async operation is registered as a continuation of the first.
        auto async2 = Make<AsyncOperation<MockAsyncResult>>(As<IAsyncOperation<MockAsyncResult*>>(async1), [&]() -> ComPtr<MockAsyncResult>
        {
            Assert::Fail(L"Should never execute since we were cancelled.");
            return nullptr;
        });

        // Subscribe to the completion callback of the second async operation.
        auto completedCallback = Callback<IAsyncOperationCompletedHandler<MockAsyncResult*>>([&](IAsyncOperation<MockAsyncResult*> *asyncInfo, AsyncStatus status)
        {
            Assert::AreEqual(AsyncStatus::Canceled, status);
            Assert::IsTrue(IsSameInstance(async2.Get(), asyncInfo));

            SetEvent(asyncFinished.Get());
            return S_OK;
        });

        ThrowIfFailed(async2->put_Completed(completedCallback.Get()));

        // Cancel the second async operation.
        async2->Cancel();

        // Tell the first async operation to complete.
        SetEvent(asyncCanFinishNow.Get());

        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(asyncFinished.Get(), waitTimeout, false));

        // First async should have completed ok.
        AsyncStatus status = (AsyncStatus)-1;
        ThrowIfFailed(async1->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Completed, status);

        HRESULT errorCode = E_UNEXPECTED;
        ThrowIfFailed(async1->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        ComPtr<MockAsyncResult> result;
        ThrowIfFailed(async1->GetResults(&result));
        Assert::IsNull(result.Get());

        // The continuation async should be in canceled state.
        ThrowIfFailed(async2->get_Status(&status));
        Assert::AreEqual(AsyncStatus::Canceled, status);

        ThrowIfFailed(async2->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);

        Assert::AreEqual(E_ILLEGAL_METHOD_CALL, async2->GetResults(&result));

        // Validate refcounts.
        AssertExpectedRefCount(async1, 1);
        AssertExpectedRefCount(async2, 1);
    }


    template<typename T>
    static void AssertExpectedRefCount(ComPtr<T> const& ptr, unsigned long expected)
    {
        // Race condition alert! We want to test that temporary refcounts on the async operation
        // are released once everything is complete. But the last such refcount is held by a lambda
        // capture deep within the task implementation, and we have no way to synchronize our test
        // code with that release. So instead we just busy-wait for the refcount to go to the
        // expected value. This normally exits on the first go around the loop - the retries are
        // defensive against possible timing variation.

        auto startTime = GetTickCount64();

        for (;;)
        {
            // Exploit side effect of AddRef+Release to read the current refcount.
            ptr.Get()->AddRef();
            auto refCount = ptr.Get()->Release();

            // Happy!
            if (refCount == expected)
            {
                return;
            }

            // If we have waited too long, assert and fail the test.
            if (GetTickCount64() > startTime + waitTimeout)
            {
                Assert::AreEqual(expected, refCount);
            }
        }
    }
};
