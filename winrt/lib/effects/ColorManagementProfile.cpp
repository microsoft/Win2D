// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "ColorManagementProfile.h"


static ExtendedColorSpace CanvasColorSpaceToExtended(CanvasColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case CanvasColorSpace::Srgb:
        return ExtendedColorSpace::FullG22P709;

    case CanvasColorSpace::ScRgb:
        return ExtendedColorSpace::FullG10P709;

    default:
        return ExtendedColorSpace::Custom;
    }
}


static CanvasColorSpace ExtendedColorSpaceToCanvas(ExtendedColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case ExtendedColorSpace::FullG22P709:
        return CanvasColorSpace::Srgb;

    case ExtendedColorSpace::FullG10P709:
        return CanvasColorSpace::ScRgb;

    default:
        return CanvasColorSpace::Custom;
    }
}


ColorManagementProfile::ColorManagementProfile(ColorManagementSimpleProfile const& simpleProfile)
    : ResourceWrapper(nullptr)
    , m_closed(false)
    , m_type(ColorManagementProfileType::Simple)
    , m_colorSpace(CanvasColorSpace::Custom)
    , m_extendedColorSpace(ExtendedColorSpace::Custom)
    , m_simpleProfile(simpleProfile)
{
}


ColorManagementProfile::ColorManagementProfile(ExtendedColorSpace colorSpace)
    : ResourceWrapper(nullptr)
    , m_closed(false)
    , m_type(ColorManagementProfileType::Extended)
    , m_colorSpace(ExtendedColorSpaceToCanvas(colorSpace))
    , m_extendedColorSpace(colorSpace)
    , m_simpleProfile{}
{
    if (colorSpace == ExtendedColorSpace::Custom)
    {
        ThrowHR(E_INVALIDARG);
    }
}


ColorManagementProfile::ColorManagementProfile(CanvasColorSpace colorSpace)
    : ResourceWrapper(nullptr)
    , m_closed(false)
    , m_colorSpace(colorSpace)
    , m_type(ColorManagementProfileType::Icc)
    , m_extendedColorSpace(CanvasColorSpaceToExtended(colorSpace))
    , m_simpleProfile{}
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
    , m_type(ColorManagementProfileType::Icc)
    , m_extendedColorSpace(ExtendedColorSpace::Custom)
    , m_simpleProfile{}
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

    // Prior to RS2, we will have ID2D1ColorContext which is always an ICC profile.
    m_type = ColorManagementProfileType::Icc;
    m_extendedColorSpace = CanvasColorSpaceToExtended(m_colorSpace);
    m_simpleProfile = {};

    // ... but if we have an ID2D1ColorContext1, that could also be Simple or Extended type.
    if (auto d2dContext1 = MaybeAs<ID2D1ColorContext1>(d2dColorContext))
    {
        m_type = StaticCastAs<ColorManagementProfileType>(d2dContext1->GetColorContextType());
        m_extendedColorSpace = StaticCastAs<ExtendedColorSpace>(d2dContext1->GetDXGIColorSpace());

        if (m_type == ColorManagementProfileType::Simple)
        {
            ThrowIfFailed(d2dContext1->GetSimpleColorProfile(ReinterpretAs<D2D1_SIMPLE_COLOR_PROFILE*>(&m_simpleProfile)));

            // Work around MSFT:12401920 (ID2D1ColorContext::GetColorSpace returning wrong value for simple color profiles).
            m_colorSpace = CanvasColorSpace::Custom;
        }
    }
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

        if (d2dColorContext
            && m_colorSpace == CanvasColorSpace::Custom
            && m_type == ColorManagementProfileType::Icc
            )
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


IFACEMETHODIMP ColorManagementProfile::get_Type(ColorManagementProfileType* value)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(value);
        ThrowIfClosed();

        *value = m_type;
    });
}


IFACEMETHODIMP ColorManagementProfile::get_SimpleProfile(IReference<ColorManagementSimpleProfile>** value)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(value);
        ThrowIfClosed();

        if (m_type == ColorManagementProfileType::Simple)
        {
            auto boxedProfile = Make<Nullable<ColorManagementSimpleProfile>>(m_simpleProfile);
            CheckMakeResult(boxedProfile);
            ThrowIfFailed(boxedProfile.CopyTo(value));
        }
    });
}


IFACEMETHODIMP ColorManagementProfile::get_ExtendedColorSpace(ExtendedColorSpace* value)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(value);
        ThrowIfClosed();

        *value = m_extendedColorSpace;
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

    if (m_type == ColorManagementProfileType::Icc)
    {
        // Create an old style ICC color context.
        ThrowIfFailed(deviceContext->CreateColorContext(
            StaticCastAs<D2D1_COLOR_SPACE>(m_colorSpace),
            m_iccProfile.data(),
            static_cast<uint32_t>(m_iccProfile.size()),
            &newColorContext));
    }
    else
    {
        // Create one of the new RS2 color contexts.
        auto deviceContext5 = MaybeAs<ID2D1DeviceContext5>(deviceContext.Get());

        if (!deviceContext5)
            ThrowHR(E_NOTIMPL, Strings::ColorManagementProfileTypeNotSupported);

        ComPtr<ID2D1ColorContext1> colorContext1;

        switch (m_type)
        {
        case ColorManagementProfileType::Simple:
            ThrowIfFailed(deviceContext5->CreateColorContextFromSimpleColorProfile(
                ReinterpretAs<D2D1_SIMPLE_COLOR_PROFILE*>(&m_simpleProfile),
                &colorContext1));
            break;

        case ColorManagementProfileType::Extended:
            ThrowIfFailed(deviceContext5->CreateColorContextFromDxgiColorSpace(
                StaticCastAs<DXGI_COLOR_SPACE_TYPE>(m_extendedColorSpace),
                &colorContext1));
            break;

        default:
            assert(false);
            break;
        }

        newColorContext = colorContext1;
    }

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


IFACEMETHODIMP ColorManagementProfileFactory::CreateSimple(ColorManagementSimpleProfile simpleProfile, IColorManagementProfile** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto newProfile = Make<ColorManagementProfile>(simpleProfile);
        CheckMakeResult(newProfile);

        ThrowIfFailed(newProfile.CopyTo(result));
    });
}


IFACEMETHODIMP ColorManagementProfileFactory::CreateExtended(ExtendedColorSpace colorSpace, IColorManagementProfile** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto newProfile = Make<ColorManagementProfile>(colorSpace);
        CheckMakeResult(newProfile);

        ThrowIfFailed(newProfile.CopyTo(result));
    });
}


IFACEMETHODIMP ColorManagementProfileFactory::IsSupported(ColorManagementProfileType type, ICanvasDevice* device, boolean* result)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(device);
        CheckInPointer(result);

        switch (type)
        {
        case ColorManagementProfileType::Icc:
            // ICC profiles have always been supported.
            *result = true;
            break;

        case ColorManagementProfileType::Simple:
        case ColorManagementProfileType::Extended:
            // Simple (D2D1_SIMPLE_COLOR_PROFILE) and extended (DXGI_COLOR_SPACE_TYPE) profile types are new in RS2.
            {
                auto lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
                auto deviceContext5 = MaybeAs<ID2D1DeviceContext5>(lease.Get());

                *result = static_cast<bool>(deviceContext5);
            }
            break;

        default:
            *result = false;
            break;
        }
    });
}


ActivatableClassWithFactory(ColorManagementProfile, ColorManagementProfileFactory);
