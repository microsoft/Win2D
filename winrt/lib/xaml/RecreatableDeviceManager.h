// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    enum class RunWithDeviceFlags
    {
        None                = 0x00,
        NewlyCreatedDevice  = 0x01,
        ResourcesNotCreated = 0x02
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
        Size,
        DeviceLost,
        DeviceCreationOptions
    };

    struct DeviceCreationOptions
    {
        bool UseSharedDevice;
        bool ForceSoftwareRenderer;
        ComPtr<ICanvasDevice> CustomDevice;

        bool operator==(const DeviceCreationOptions& rhs)
        {
            return UseSharedDevice == rhs.UseSharedDevice &&
                ForceSoftwareRenderer == rhs.ForceSoftwareRenderer &&
                IsSameInstance(CustomDevice.Get(), rhs.CustomDevice.Get());
        }

        bool operator!=(const DeviceCreationOptions& rhs)
        { 
            return !(*this == rhs); 
        }
    };
    
    template<typename TRAITS>
    class IRecreatableDeviceManager
    {
    public:
        typedef typename TRAITS::control_t                     Sender;
        typedef typename TRAITS::createResourcesEventHandler_t CreateResourcesHandler;

        virtual ~IRecreatableDeviceManager() = default;

        virtual void SetChangedCallback(std::function<void(ChangeReason)> fn) = 0;
        virtual void RunWithDevice(Sender* sender, DeviceCreationOptions deviceCreationOptions, RunWithDeviceFunction fn) = 0;
        virtual ComPtr<ICanvasDevice> const& GetDevice() = 0;
        virtual bool IsReadyToDraw() = 0;
        virtual void SetDpiChanged() = 0;

        virtual EventRegistrationToken AddCreateResources(Sender* sender, CreateResourcesHandler* value) = 0;
        virtual void RemoveCreateResources(EventRegistrationToken token) = 0;
    };

} } } } } }
