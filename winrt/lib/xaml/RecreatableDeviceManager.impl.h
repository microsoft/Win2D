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

#include "RecreatableDeviceManager.h"
#include "CanvasCreateResourcesEventArgs.h"
#include "CanvasControl.h"
#include "CanvasAnimatedControl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CommittedDevice
    {
        enum class State
        {
            NeedCreateResources,
            ResourcesCreated,
            Lost
        };

        ComPtr<ICanvasDevice> m_committedDevice;

        State m_state;
        CanvasCreateResourcesReason m_createResourcesReason;

    public:
        CommittedDevice(ComPtr<ICanvasDevice> const& device, CanvasCreateResourcesReason createResourcesReason)
            : m_committedDevice(device)
            , m_state(State::NeedCreateResources)
            , m_createResourcesReason(createResourcesReason)
        {
        }

        ComPtr<ICanvasDevice> const& GetDevice()
        { 
            return m_committedDevice; 
        }

        void SetResourcesCreated()
        {
            switch (m_state)
            {
            case State::NeedCreateResources:
                m_state = State::ResourcesCreated;
                break;

            case State::Lost:
                // Once lost this device is lost forever
                break;

            case State::ResourcesCreated:
                // SetResourcesCreated() can be called when already in this
                // state when processing a CreateResources handler that was
                // added after entering this state.
                break;

            default:
                assert(false);
                break;
            }
        }

        void SetDpiChanged()
        {
            if (m_state == State::ResourcesCreated)
            {
                m_state = State::NeedCreateResources;
                m_createResourcesReason = CanvasCreateResourcesReason::DpiChanged;
            }
        }

        bool NeedsCreateResources()
        {
            return (m_state == State::NeedCreateResources);
        }

        CanvasCreateResourcesReason GetCreateResourcesReason()
        {
            assert(m_state == State::NeedCreateResources);
            return m_createResourcesReason;
        }

        bool IsLost()
        {
            return (m_state == State::Lost);
        }

        void CheckForDeviceLost()
        {
            if (m_state == State::Lost)
                return;

            auto d3dDevice = GetDXGIInterface<ID3D11Device>(m_committedDevice.Get());
            
            if (d3dDevice->GetDeviceRemovedReason() != S_OK)
                m_state = State::Lost;
        }
    };

    template<typename TRAITS>
    class RecreatableDeviceManager : public IRecreatableDeviceManager<TRAITS>
    {
        std::function<void(ChangeReason)> m_changedCallback;
        ComPtr<IActivationFactory> m_canvasDeviceFactory;
        EventSource<CreateResourcesHandler, InvokeModeOptions<StopOnFirstError>> m_createResourcesEventSource;

        ComPtr<ICanvasDevice> m_device;

        std::recursive_mutex m_currentOperationMutex;
        ComPtr<IAsyncInfo> m_currentOperation;
        bool m_currentOperationIsPending;

        std::unique_ptr<CommittedDevice> m_committedDevice;

        bool m_dpiChanged;

    public:
        RecreatableDeviceManager(IActivationFactory* canvasDeviceFactory)
            : m_canvasDeviceFactory(canvasDeviceFactory)
            , m_currentOperationIsPending(false)
            , m_dpiChanged(false)
        {
        }

        virtual void SetChangedCallback(std::function<void(ChangeReason)> fn)
        {
            m_changedCallback = fn;
        }

        virtual void RunWithDevice(Sender* sender, RunWithDeviceFunction runWithDeviceFunction)
        {
            try
            {
                RunWithDeviceFlags flags = EnsureDeviceCreated(sender);
                assert(m_device);

                // m_device is passed to the function.  This means that the
                // function has a device it can use for rendering, even if we
                // haven't committed to using the new one yet.  This allows
                // controls to perform rendering while asynchronous create
                // resources actions are still executing against the previous
                // device.

                runWithDeviceFunction(m_device.Get(), flags);
            }
            catch (DeviceLostException const&)
            {
                HandleDeviceLostException();
            }
        }

        virtual ComPtr<ICanvasDevice> const& GetDevice() override
        {
            // This device may have been lost and we may have created a new
            // m_device, but we still report the old one until we're sure that
            // any pending asynchronous CreateResources actions have completed.
            
            if (m_committedDevice)
            {
                return m_committedDevice->GetDevice();
            }
            else
            {
                static ComPtr<ICanvasDevice> nullCanvasDevice;
                return nullCanvasDevice;
            }
        }

        virtual bool IsReadyToDraw() override
        {
            if (!m_committedDevice || m_committedDevice->IsLost())
                return false;

            std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);
            return !m_currentOperationIsPending;
        }

        virtual void SetDpiChanged() override
        {
            std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);
         
            m_dpiChanged = true;

            if (!m_currentOperationIsPending && m_changedCallback)
            {
                m_changedCallback(ChangeReason::Other);
            }
        }

        virtual EventRegistrationToken AddCreateResources(Sender* sender, CreateResourcesHandler* value)
        {
            //
            // If the device has already been created and committed then this
            // means that we've already called any existing handlers, so we need
            // to explicitly call the new one now.
            //
            // This might fail, so we do it before we actually add the handler
            // so we're not left in a state where the handler has been added but
            // this method throws.
            //
            if (m_committedDevice && !m_committedDevice->IsLost())
            {
                auto eventArgs = MakeEventArgs(CanvasCreateResourcesReason::FirstTime);

                try
                {
                    ThrowIfFailed(value->Invoke(sender, eventArgs.Get()));
                }
                catch (DeviceLostException const&)
                {
                    HandleDeviceLostException();
                }
            }

            EventRegistrationToken token;
            ThrowIfFailed(m_createResourcesEventSource.Add(value, &token));

            return token;
        }

        virtual void RemoveCreateResources(EventRegistrationToken token) override
        {
            ThrowIfFailed(m_createResourcesEventSource.Remove(token));
        }

    private:
        RunWithDeviceFlags EnsureDeviceCreated(Sender* sender)
        {
            std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);

            RunWithDeviceFlags flags{};

            //
            // If we've never created a device, or we've seen a device lost
            // error for the one we have, then we'll need to create a new
            // device.
            //            
            if (!m_device)
            {
                m_device = CreateDevice();
                flags = RunWithDeviceFlags::NewlyCreatedDevice;
            }

            //
            // While there's an operation pending we can't use any new device we
            // created.
            //
            if (m_currentOperationIsPending)
                return flags | RunWithDeviceFlags::ResourcesNotCreated;

            if (m_currentOperation)
            {
                try
                {
                    ProcessCurrentOperationResult();
                }
                catch (DeviceLostException const&)
                {
                    HandleDeviceLostException();

                    // If it was just the committed device that was lost,
                    // m_device might still be valid, but if it isn't we can't
                    // continue.
                    if (!m_device)
                        throw;
                }
            }

            //
            // From this point on we know that there are no outstanding
            // CreateResources operations.
            //
            // If we haven't already, we can commit to the new device.
            //
            if (!m_committedDevice || m_committedDevice->IsLost())
            {
                auto reason = !m_committedDevice ? CanvasCreateResourcesReason::FirstTime : 
                                                   CanvasCreateResourcesReason::NewDevice;

                m_committedDevice = std::make_unique<CommittedDevice>(m_device, reason);
            }

            assert(m_committedDevice->GetDevice() == m_device);

            //
            // If the DPI has changed, tell the device that it needs to reload resources.
            //
            if (m_dpiChanged)
            {
                m_committedDevice->SetDpiChanged();
                m_dpiChanged = false;
            }

            //
            // Raise the CreateResources event if we haven't managed to
            // successfully CreateResources since we committed to the new
            // device, or if the DPI has changed.
            //
            if (m_committedDevice->NeedsCreateResources())
            {
                auto eventArgs = MakeEventArgs(m_committedDevice->GetCreateResourcesReason());

                ThrowIfFailed(m_createResourcesEventSource.InvokeAll(sender, eventArgs.Get()));

                //
                // One of the CreateResources handlers might have registered an
                // operation for us to track.
                //
                if (m_currentOperationIsPending)
                    flags = flags | RunWithDeviceFlags::ResourcesNotCreated;
                else
                    m_committedDevice->SetResourcesCreated();
            }

            return flags;
        }

        ComPtr<ICanvasDevice> CreateDevice()
        {
            ComPtr<IInspectable> inspectableDevice;
            ThrowIfFailed(m_canvasDeviceFactory->ActivateInstance(&inspectableDevice));

            return As<ICanvasDevice>(inspectableDevice);
        }

        void HandleDeviceLostException()
        {
            // This function must be called from inside a catch block.
            assert(std::current_exception());

            // No m_device means that this exception has already been handled.
            if (!m_device)
                return;

            if (m_committedDevice)
            {
                m_committedDevice->CheckForDeviceLost();

                if (!m_committedDevice->IsLost())
                {
                    // If m_committedDevice hasn't been lost then m_device also
                    // cannot have been lost so the exception can't be related
                    // to either of them.
                    throw;
                }
            }

            // At this point, the committed device is definitely lost
            assert(!m_committedDevice || m_committedDevice->IsLost());

            // ...any pending create resources against it are definitely going
            // to fail (or be useless) so we cancel them now.
            CancelAnyPendingOperation();

            auto d3dDevice = GetDXGIInterface<ID3D11Device>(m_device.Get());
            if (d3dDevice->GetDeviceRemovedReason() != S_OK)
            {
                // We need to create a new device
                m_device.Reset();
                
                // Notify the control that it needs to try rendering again
                if (m_changedCallback)
                    m_changedCallback(ChangeReason::DeviceLost);
            }
        }

        bool CommittedDeviceIsStillValid()
        {
            return m_committedDevice && m_committedDevice->IsValid();
        }

        void CancelAnyPendingOperation()
        {
            std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);
            if (m_currentOperationIsPending)
            {
                ThrowIfFailed(m_currentOperation->Cancel());
            }            
        }

        void ProcessCurrentOperationResult()
        {
            auto operation = m_currentOperation;
            m_currentOperation.Reset();
  
            AsyncStatus status;
            ThrowIfFailed(operation->get_Status(&status));

            switch (status)
            {
            case AsyncStatus::Completed:
                assert(m_committedDevice);
                m_committedDevice->SetResourcesCreated();
                break;
                
            case AsyncStatus::Canceled:
                break;

            case AsyncStatus::Error: 
            {
                HRESULT hr;
                ThrowIfFailed(operation->get_ErrorCode(&hr));
                ThrowHR(hr);
                // (can't get here, we threw!)
            }
            
            default:
                assert(false);
                ThrowHR(E_UNEXPECTED);
            }
        }

        ComPtr<ICanvasCreateResourcesEventArgs> MakeEventArgs(CanvasCreateResourcesReason reason)
        {
            auto eventArgs = Make<CanvasCreateResourcesEventArgs>(reason,
                [=](IAsyncAction* action)
                {
                    TrackAsyncAction(action);
                });

            CheckMakeResult(eventArgs);
            return eventArgs;
        }

        void TrackAsyncAction(IAsyncAction* action)
        {
            using ::Microsoft::WRL::Wrappers::HStringReference;

            std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);

            if (m_currentOperation)
                ThrowHR(COR_E_NOTSUPPORTED, HStringReference(Strings::MultipleAsyncCreateResourcesNotSupported).Get());

            auto onCompleted = Callback<IAsyncActionCompletedHandler>(this, &RecreatableDeviceManager::OnAsynchronousCreateResourcesCompleted);
            CheckMakeResult(onCompleted);
            ThrowIfFailed(action->put_Completed(onCompleted.Get()));
            m_currentOperation = As<IAsyncInfo>(action);                
            m_currentOperationIsPending = true;
        }

        HRESULT OnAsynchronousCreateResourcesCompleted(IAsyncAction*, AsyncStatus)
        {
            return ExceptionBoundary(
                [&]
                {
                    std::unique_lock<std::recursive_mutex> lock(m_currentOperationMutex);
                    m_currentOperationIsPending = false;
                    lock.unlock();
                    
                    if (m_changedCallback)
                        m_changedCallback(ChangeReason::Other);
                });
        }
    };
} } } }
