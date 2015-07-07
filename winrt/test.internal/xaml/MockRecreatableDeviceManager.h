// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

template<typename TRAITS>
class MockRecreatableDeviceManager : public IRecreatableDeviceManager<TRAITS>
{
    ComPtr<ICanvasDevice> m_device;

public:
    CALL_COUNTER_WITH_MOCK(SetChangedCallbackMethod, void(std::function<void(ChangeReason)>));
    CALL_COUNTER_WITH_MOCK(RunWithDeviceMethod, void(Sender*, DeviceCreationOptions, RunWithDeviceFunction));
    CALL_COUNTER_WITH_MOCK(IsReadyToDrawMethod, bool());
    CALL_COUNTER_WITH_MOCK(AddCreateResourcesMethod, EventRegistrationToken(Sender*, CreateResourcesHandler*));
    CALL_COUNTER_WITH_MOCK(RemoveCreateResourcesMethod, void(EventRegistrationToken));
    CALL_COUNTER_WITH_MOCK(SetDpiChangedMethod, void())

    virtual void SetChangedCallback(std::function<void(ChangeReason)> fn) override
    {
        return SetChangedCallbackMethod.WasCalled(fn);
    }

    virtual void RunWithDevice(Sender* sender, DeviceCreationOptions options, RunWithDeviceFunction fn) override
    {
        return RunWithDeviceMethod.WasCalled(sender, options, fn);
    }

    void SetRunWithDeviceFlags(RunWithDeviceFlags flags, int expectedCalls)
    {
        RunWithDeviceMethod.SetExpectedCalls(expectedCalls,
            [=](Sender*, DeviceCreationOptions, RunWithDeviceFunction fn)
            {
                fn(m_device.Get(), flags);
            });
    }

    void SetRunWithDeviceFlags(RunWithDeviceFlags flags)
    {
        RunWithDeviceMethod.AllowAnyCall(
            [=](Sender*, RunWithDeviceFunction fn)
            {
                fn(m_device.Get(), flags);
            });
    }

    void SetDevice(ComPtr<ICanvasDevice> device)
    {
        m_device = device;
    }

    virtual ComPtr<ICanvasDevice> const& GetDevice() override
    {
        return m_device;
    }

    virtual bool IsReadyToDraw() override
    {
        return IsReadyToDrawMethod.WasCalled();
    }
    
    virtual void SetDpiChanged() override
    {
        SetDpiChangedMethod.WasCalled();
    }

    virtual EventRegistrationToken AddCreateResources(Sender* sender, CreateResourcesHandler* value) override
    {
        return AddCreateResourcesMethod.WasCalled(sender, value);
    }

    virtual void RemoveCreateResources(EventRegistrationToken token) override
    {
        return RemoveCreateResourcesMethod.WasCalled(token);
    }
};
