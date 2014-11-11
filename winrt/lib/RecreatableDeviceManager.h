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

#include "CanvasControl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    class IRecreatableDeviceManager
    {
    public:
        virtual bool RunWithDevice(CanvasControl* sender, std::function<void(ICanvasDevice*,bool)> fn) = 0;
        virtual ComPtr<ICanvasDevice> const& GetDevice() = 0;

        virtual bool AddSynchronousCreateResources(CanvasControl* sender, CreateResourcesEventHandler* value, EventRegistrationToken* outToken) = 0;
        virtual void RemoveSynchronousCreateResources(EventRegistrationToken token) = 0;
    };

    class RecreatableDeviceManager : public IRecreatableDeviceManager
    {
        ComPtr<IActivationFactory> m_canvasDeviceFactory;
        ComPtr<ICanvasDevice> m_device;
        EventSource<CreateResourcesEventHandler, InvokeModeOptions<StopOnFirstError>> m_createResourcesEventList;

    public:
        RecreatableDeviceManager(IActivationFactory* canvasDeviceFactory)
            : m_canvasDeviceFactory(canvasDeviceFactory)
        {
        }

        virtual bool RunWithDevice(CanvasControl* sender, std::function<void(ICanvasDevice*,bool)> fn)
        {
            try
            {
                bool wasDeviceJustCreated = false;

                if (!m_device)
                {
                    m_device = CreateDevice();
                    ThrowIfFailed(m_createResourcesEventList.InvokeAll(sender, static_cast<IInspectable*>(nullptr)));
                    wasDeviceJustCreated = true;
                }

                assert(m_device);

                fn(m_device.Get(), wasDeviceJustCreated);
                return false;
            }
            catch (DeviceLostException const&)
            {
                HandleDeviceLostException();
                return true;
            }
        }

        virtual ComPtr<ICanvasDevice> const& GetDevice() override
        {
            return m_device;
        }

        virtual bool AddSynchronousCreateResources(CanvasControl* sender, CreateResourcesEventHandler* value, EventRegistrationToken* outToken)
        {
            ThrowIfFailed(m_createResourcesEventList.Add(value, outToken));

            //
            // If the device hasn't been created yet then we know that when
            // it does get created CreateResources will be raised.
            //
            if (!m_device)
                return false;

            try
            {
                //
                // If the device has already been created then we need to call
                // the handler now.
                //
                ThrowIfFailed(value->Invoke(sender, nullptr));
            }
            catch (DeviceLostException const&)
            {
                HandleDeviceLostException();
                return true;
            }

            return false;
        }

        virtual void RemoveSynchronousCreateResources(EventRegistrationToken token) override
        {
            ThrowIfFailed(m_createResourcesEventList.Remove(token));
        }

    private:
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

            if (HasValidDevice())
            {
                // The device lost exception wasn't caused by the device managed
                // by this object.  Recreating therefore won't resolve this.  So
                // instead we propogate the error.
                throw;
            }

            m_device.Reset();
        }

        bool HasValidDevice()
        {
            if (!m_device)
                return false;

            auto d3dDevice = GetDXGIInterface<ID3D11Device>(m_device.Get());

            if (d3dDevice->GetDeviceRemovedReason() == S_OK)
                return true;

            return false;
        }
    };

} } } }
