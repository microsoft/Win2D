// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "ColorManagementProfile.h"


ColorManagementProfile::ColorManagementProfile(CanvasColorSpace colorSpace)
    : ResourceWrapper(nullptr)
    , m_closed(false)
    , m_colorSpace(colorSpace)
{
    if (colorSpace == CanvasColorSpace::Custom)
    {
        ThrowHR(E_INVALIDARG);
    }
}


ColorManagementProfile::ColorManagementProfile(uint32_t iccProfileCount, uint8_t* iccProfile)
    : ResourceWrapper(nullptr)
    , m_closed(false)
    , m_colorSpace(CanvasColorSpace::Custom)
{
    CheckInPointer(iccProfile);

    m_iccProfile.assign(iccProfile, iccProfile + iccProfileCount);
}


ColorManagementProfile::ColorManagementProfile(ICanvasDevice* device, ID2D1ColorContext* d2dColorContext)
    : ResourceWrapper(d2dColorContext)
    , m_closed(false)
    , m_device(device)
{
    m_colorSpace = StaticCastAs<CanvasColorSpace>(d2dColorContext->GetColorSpace());
}


IFACEMETHODIMP ColorManagementProfile::get_ColorSpace(CanvasColorSpace* value)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(value);
        ThrowIfClosed();

        *value = m_colorSpace;
    });
}


IFACEMETHODIMP ColorManagementProfile::get_IccProfile(uint32_t* valueCount, uint8_t** valueElements)
{
    return ExceptionBoundary([&]
    {
        Lock lock(m_mutex);

        CheckInPointer(valueCount);
        CheckAndClearOutPointer(valueElements);
        ThrowIfClosed();

        ComArray<uint8_t> array;
        auto& d2dColorContext = MaybeGetResource();

        if (d2dColorContext && m_colorSpace == CanvasColorSpace::Custom)
        {
            // Read the profile bytes back from a realized D2D resource.
            array = ComArray<uint8_t>(d2dColorContext->GetProfileSize());

            ThrowIfFailed(d2dColorContext->GetProfile(array.GetData(), array.GetSize()));
        }
        else
        {
            // Return a copy of our local profile bytes.
            array = ComArray<uint8_t>(m_iccProfile.begin(), m_iccProfile.end());
        }

        array.Detach(valueCount, valueElements);
    });
}


IFACEMETHODIMP ColorManagementProfile::Close()
{
    return ExceptionBoundary([&]
    {
        Lock lock(m_mutex);

        ReleaseResource();
        m_closed = true;
        m_iccProfile.clear();
        m_device.Reset();
    });
}


IFACEMETHODIMP ColorManagementProfile::GetNativeResource(ICanvasDevice* device, float dpi, REFIID iid, void** resource)
{
    UNREFERENCED_PARAMETER(dpi);

    return ExceptionBoundary([&]
    {
        Lock lock(m_mutex);

        CheckAndClearOutPointer(resource);
        ThrowIfClosed();

        if (!device)
            ThrowHR(E_INVALIDARG, Strings::GetResourceNoDevice);

        if (!IsSameInstance(device, m_device.Get()))
        {
            Realize(device);
        }

        ThrowIfFailed(GetResource().CopyTo(iid, resource));
    });
}


IFACEMETHODIMP ColorManagementProfile::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(device);

        ThrowIfFailed(m_device.CopyTo(device));
    });
}


void ColorManagementProfile::Realize(ICanvasDevice* device)
{
    // If we have an existing realization, read back its profile bytes.
    auto& existingColorContext = MaybeGetResource();

    if (existingColorContext && m_colorSpace == CanvasColorSpace::Custom)
    {
        auto size = existingColorContext->GetProfileSize();
        m_iccProfile.resize(size);
        ThrowIfFailed(existingColorContext->GetProfile(m_iccProfile.data(), size));
    }

    // Create the D2D color context.
    auto deviceContext = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
    
    ComPtr<ID2D1ColorContext> newColorContext;

    ThrowIfFailed(deviceContext->CreateColorContext(
        StaticCastAs<D2D1_COLOR_SPACE>(m_colorSpace),
        m_iccProfile.data(),
        static_cast<uint32_t>(m_iccProfile.size()),
        &newColorContext));

    // Store our new realization.
    SetResource(newColorContext.Get());

    m_device = device;

    // Save some memory - no need to keep a duplicate of this array.
    m_iccProfile.clear();
}


void ColorManagementProfile::ThrowIfClosed()
{
    if (m_closed)
        ThrowHR(RO_E_CLOSED);
}


IFACEMETHODIMP ColorManagementProfileFactory::Create(CanvasColorSpace colorSpace, IColorManagementProfile** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto newProfile = Make<ColorManagementProfile>(colorSpace);
        CheckMakeResult(newProfile);

        ThrowIfFailed(newProfile.CopyTo(result));
    });
}


IFACEMETHODIMP ColorManagementProfileFactory::CreateCustom(uint32_t iccProfileCount, uint8_t* iccProfile, IColorManagementProfile** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto newProfile = Make<ColorManagementProfile>(iccProfileCount, iccProfile);
        CheckMakeResult(newProfile);

        ThrowIfFailed(newProfile.CopyTo(result));
    });
}


ActivatableClassWithFactory(ColorManagementProfile, ColorManagementProfileFactory);
