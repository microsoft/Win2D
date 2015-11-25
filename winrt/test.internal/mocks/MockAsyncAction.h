// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

//
// This implements IAsyncAction enough that it can be passed to something that
// requires an IAsyncAction.  However, it does not provide any functionality itself
// and expects the caller (ie the test code) to poke directly at it to notify
// when it has completed or failed.
//
typedef AsyncBase<IAsyncActionCompletedHandler,
                  Microsoft::WRL::Details::Nil,
                  SingleResult,
                  AsyncOptions<ErrorPropagationPolicy::PropagateDelegateError>> MockAsyncBase;

class MockAsyncAction : public RuntimeClass<MockAsyncBase, IAsyncAction>
{
    bool m_fireCompletionOnCancel;;

    InspectableClass(InterfaceName_Windows_Foundation_IAsyncAction, BaseTrust);

public:
    MockAsyncAction()
        : m_fireCompletionOnCancel(false)
    {
        Start();
    }

    void SetResult(HRESULT hr, const wchar_t* errorText = nullptr)
    {
        SetResultWithoutFireCompletion(hr, errorText);
        ThrowIfFailed(FireCompletion());
    }

    void SetResultWithoutFireCompletion(HRESULT hr, const wchar_t* errorText = nullptr)
    {
        if (errorText)
            RoOriginateError(hr, WinString(errorText));

        if (SUCCEEDED(hr))
            (void)TryTransitionToCompleted();
        else
            (void)TryTransitionToError(hr);
    }

    void FireCompletionOnCancel()
    {
        m_fireCompletionOnCancel = true;
    }

    // Inherited: HRESULT FireCompletion();

    IFACEMETHODIMP put_Completed(IAsyncActionCompletedHandler* value) override
    {
        return PutOnComplete(value);
    }

    IFACEMETHODIMP get_Completed(IAsyncActionCompletedHandler** value) override
    {
        return GetOnComplete(value);
    }

    IFACEMETHODIMP GetResults() override
    {
        return S_OK;
    }

protected:
    virtual HRESULT OnStart() override
    {
        return S_OK;
    }
    
    virtual void OnClose() override
    {
    }

    virtual void OnCancel() override
    {
        if (m_fireCompletionOnCancel)
        {
            ThrowIfFailed(FireCompletion());
        }
    }
};
