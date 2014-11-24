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
// This implements IAsyncAction enough that it can be passed to something that
// requires an IAsyncAction.  However, it does provide any functionality itself
// and expects the caller (ie the test code) to poke directly at it to notify
// when it has completed or failed.
//
class MockAsyncAction : public RuntimeClass<AsyncBase<IAsyncActionCompletedHandler>, IAsyncAction>
{
public:
    MockAsyncAction()
    {
        Start();
    }

    void SetResult(HRESULT hr, const wchar_t* errorText = nullptr)
    {
        SetResultWithoutFireCompletion(hr, errorText);
        FireCompletion();
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
    }
};
