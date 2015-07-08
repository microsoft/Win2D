// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Windows::ApplicationModel;


    class MockSuspendingEventArgs : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISuspendingEventArgs>
    {
    public:
        CALL_COUNTER_WITH_MOCK(get_SuspendingOperationMethod, HRESULT(ISuspendingOperation**));

        IFACEMETHODIMP get_SuspendingOperation(ISuspendingOperation** value) override
        {
            return get_SuspendingOperationMethod.WasCalled(value);
        }
    };


    class MockSuspendingOperation : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISuspendingOperation>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetDeferralMethod, HRESULT(ISuspendingDeferral**));
        CALL_COUNTER_WITH_MOCK(get_DeadlineMethod, HRESULT(DateTime*));

        IFACEMETHODIMP GetDeferral(ISuspendingDeferral** deferral) override
        {
            return GetDeferralMethod.WasCalled(deferral);
        }

        IFACEMETHODIMP get_Deadline(DateTime* value) override
        {
            return get_DeadlineMethod.WasCalled(value);
        }
    };


    class MockSuspendingDeferral : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISuspendingDeferral>
    {
    public:
        CALL_COUNTER_WITH_MOCK(CompleteMethod, HRESULT());

        IFACEMETHODIMP Complete() override
        {
            return CompleteMethod.WasCalled();
        }
    };
}
