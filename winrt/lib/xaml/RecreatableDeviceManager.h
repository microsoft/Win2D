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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    enum class RunWithDeviceFlags
    {
        None                = 0x00,
        NewlyCreatedDevice  = 0x01,
        ResourcesNotCreated = 0x02,
    };

    DEFINE_ENUM_FLAG_OPERATORS(RunWithDeviceFlags);

    inline bool IsSet(RunWithDeviceFlags flags, RunWithDeviceFlags toTest)
    { 
        return (flags & toTest) == toTest; 
    }

    typedef std::function<void(ICanvasDevice*, RunWithDeviceFlags)> RunWithDeviceFunction;

    enum class ChangeReason
    {
        Other,
        ClearColor,
        DeviceLost
    };
    
    template<typename TRAITS>
    class IRecreatableDeviceManager
    {
    public:
        typedef typename TRAITS::control_t                     Sender;
        typedef typename TRAITS::createResourcesEventHandler_t CreateResourcesHandler;

        virtual void SetChangedCallback(std::function<void(ChangeReason)> fn) = 0;
        virtual void RunWithDevice(Sender* sender, RunWithDeviceFunction fn) = 0;
        virtual ComPtr<ICanvasDevice> const& GetDevice() = 0;
        virtual bool IsReadyToDraw() = 0;
        virtual void SetDpiChanged() = 0;

        virtual EventRegistrationToken AddCreateResources(Sender* sender, CreateResourcesHandler* value) = 0;
        virtual void RemoveCreateResources(EventRegistrationToken token) = 0;
    };
} } } }
