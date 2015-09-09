// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "brushes/CanvasSolidColorBrush.h"
#include "brushes/CanvasLinearGradientBrush.h"
#include "brushes/CanvasRadialGradientBrush.h"
#include "drawing/CanvasGradientMesh.h"
#include "geometry/CanvasCachedGeometry.h"
#include "images/CanvasCommandList.h"
#include "text/CanvasTextLayout.h"


std::unordered_map<IUnknown*, WeakRef> ResourceManager::m_resources;
std::recursive_mutex ResourceManager::m_mutex;


std::vector<ResourceManager::TryCreateFunction> ResourceManager::tryCreateFunctions =
{
    TryCreate<ID2D1Device1,             CanvasDevice,               MakeWrapper>,
    TryCreate<ID2D1DeviceContext1,      CanvasDrawingSession,       MakeWrapper>,
    TryCreate<ID2D1Bitmap1,             CanvasRenderTarget,         MakeWrapperWithDevice,  IsRenderTargetBitmap>,
    TryCreate<ID2D1Bitmap1,             CanvasBitmap,               MakeWrapperWithDevice>,
    TryCreate<ID2D1CommandList,         CanvasCommandList,          MakeWrapperWithDevice>,
    TryCreate<IDXGISwapChain1,          CanvasSwapChain,            MakeWrapperWithDeviceAndDpi>,
    TryCreate<ID2D1Geometry,            CanvasGeometry,             MakeWrapperWithDevice>,
    TryCreate<ID2D1GeometryRealization, CanvasCachedGeometry,       MakeWrapperWithDevice>,
    TryCreate<IDWriteTextLayout2,       CanvasTextLayout,           MakeWrapperWithDevice>,
    TryCreate<IDWriteTextFormat,        CanvasTextFormat,           MakeWrapper>,
    TryCreate<ID2D1StrokeStyle1,        CanvasStrokeStyle,          MakeWrapper>,
    TryCreate<ID2D1SolidColorBrush,     CanvasSolidColorBrush,      MakeWrapperWithDevice>,
    TryCreate<ID2D1LinearGradientBrush, CanvasLinearGradientBrush,  MakeWrapperWithDevice>,
    TryCreate<ID2D1RadialGradientBrush, CanvasRadialGradientBrush,  MakeWrapperWithDevice>,
    TryCreate<ID2D1ImageBrush,          CanvasImageBrush,           MakeWrapperWithDevice>,
    TryCreate<ID2D1BitmapBrush1,        CanvasImageBrush,           MakeWrapperWithDevice>,
#if WINVER > _WIN32_WINNT_WINBLUE
    TryCreate<ID2D1GradientMesh,        CanvasGradientMesh,         MakeWrapperWithDevice>,
#endif
};


// Called by the ResourceWrapper constructor, to add itself to the interop mapping table.
void ResourceManager::Add(IUnknown* resource, IInspectable* wrapper)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto result = m_resources.insert(std::make_pair(resourceIdentity.Get(), AsWeak(wrapper)));

    if (!result.second)
        ThrowHR(E_UNEXPECTED);
}


// Called by ResourceWrapper::Close, to remove itself from the interop mapping table.
void ResourceManager::Remove(IUnknown* resource)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto result = m_resources.erase(resourceIdentity.Get());

    if (result != 1)
        ThrowHR(E_UNEXPECTED);
}


ComPtr<IInspectable> ResourceManager::GetOrCreate(ICanvasDevice* device, IUnknown* resource, float dpi)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);
    ComPtr<IInspectable> wrapper;

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    // Do we already have a wrapper around this resource?
    auto it = m_resources.find(resourceIdentity.Get());

    if (it != m_resources.end())
    {
        wrapper = LockWeakRef<IInspectable>(it->second);
    }

    // Create a new wrapper instance?
    if (!wrapper)
    {
        for (auto& tryCreateFunction : tryCreateFunctions)
        {
            if (tryCreateFunction(device, resource, dpi, &wrapper))
            {
                break;
            }
        }

        // Fail if we did not find a way to wrap this type.
        if (!wrapper)
        {
            ThrowHR(E_NOINTERFACE, Strings::ResourceManagerUnknownType);
        }
    }

    // Validate that the object we got back reports the expected device and DPI.
    ValidateDevice(wrapper.Get(), device);
    ValidateDpi(wrapper.Get(), dpi);

    return wrapper;
}


// Validation rules:
//  - If the caller specified a device or dpi, and the wrapper has device/dpi, these must match.
//  - If the caller specified device or dpi but the wrapper has no device/dpi, we'll allow that, ignoring the parameter.
//  - If the caller did not specify device or dpi, we'll accept any existing wrapper instance regardless of its device/dpi.


void ResourceManager::ValidateDevice(IInspectable* wrapper, ICanvasDevice* device)
{
    auto wrapperWithDevice = MaybeAs<ICanvasResourceWrapperWithDevice>(wrapper);

    if (wrapperWithDevice)
    {
        ValidateDevice(wrapperWithDevice.Get(), device);
    }
}


void ResourceManager::ValidateDevice(ICanvasResourceWrapperWithDevice* wrapper, ICanvasDevice* device)
{
    if (device)
    {
        ComPtr<ICanvasDevice> wrapperDevice;
        ThrowIfFailed(wrapper->get_Device(&wrapperDevice));

        if (device != wrapperDevice.Get())
        {
            ThrowHR(E_INVALIDARG, Strings::ResourceManagerWrongDevice);
        }
    }
}


void ResourceManager::ValidateDpi(IInspectable* wrapper, float dpi)
{
    auto wrapperWithDpi = MaybeAs<ICanvasResourceWrapperWithDpi>(wrapper);

    if (wrapperWithDpi)
    {
        ValidateDpi(wrapperWithDpi.Get(), dpi);
    }
}


void ResourceManager::ValidateDpi(ICanvasResourceWrapperWithDpi* wrapper, float dpi)
{
    if (dpi != 0)
    {
        float wrapperDpi;
        ThrowIfFailed(wrapper->get_Dpi(&wrapperDpi));

        if (dpi != wrapperDpi)
        {
            ThrowHR(E_INVALIDARG, Strings::ResourceManagerWrongDpi);
        }
    }
}


void ResourceManager::RegisterType(TryCreateFunction tryCreate)
{
    assert(std::find(tryCreateFunctions.begin(), tryCreateFunctions.end(), tryCreate) == tryCreateFunctions.end());

    tryCreateFunctions.push_back(tryCreate);
}


void ResourceManager::UnregisterType(TryCreateFunction tryCreate)
{
    auto it = std::find(tryCreateFunctions.begin(), tryCreateFunctions.end(), tryCreate);

    assert(it != tryCreateFunctions.end());

    tryCreateFunctions.erase(it);
}
