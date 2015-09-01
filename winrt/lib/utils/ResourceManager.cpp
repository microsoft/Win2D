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


// TODO interop: autogen this table
std::vector<ResourceManager::TryCreateFunction> ResourceManager::tryCreateFunctions =
{
    TryCreate<ID2D1Device1, CanvasDevice, CreateFromManager<ID2D1Device1, CanvasDevice, CanvasDeviceFactory>>,
    TryCreate<ID2D1DeviceContext1, CanvasDrawingSession, CreateFromManager<ID2D1DeviceContext1, CanvasDrawingSession, CanvasDrawingSessionFactory>>,
    TryCreate<ID2D1Bitmap1, CanvasRenderTarget, CreateFromManagerWithDevice<ID2D1Bitmap1, CanvasRenderTarget, CanvasRenderTargetFactory>, IsRenderTargetBitmap>,
    TryCreate<ID2D1Bitmap1, CanvasBitmap, CreateFromManagerWithDevice<ID2D1Bitmap1, CanvasBitmap, CanvasBitmapFactory>>,
    TryCreate<ID2D1SolidColorBrush, CanvasSolidColorBrush, MakeWrapperWithDevice<ID2D1SolidColorBrush, CanvasSolidColorBrush>>,
    TryCreate<ID2D1LinearGradientBrush, CanvasLinearGradientBrush, MakeWrapperWithDevice<ID2D1LinearGradientBrush, CanvasLinearGradientBrush>>,
    TryCreate<ID2D1RadialGradientBrush, CanvasRadialGradientBrush, MakeWrapperWithDevice<ID2D1RadialGradientBrush, CanvasRadialGradientBrush>>,
    TryCreate<ID2D1ImageBrush, CanvasImageBrush, MakeWrapperWithDevice<ID2D1ImageBrush, CanvasImageBrush>>,
    TryCreate<ID2D1BitmapBrush1, CanvasImageBrush, MakeWrapperWithDevice<ID2D1BitmapBrush1, CanvasImageBrush>>,
    TryCreate<ID2D1StrokeStyle1, CanvasStrokeStyle, MakeWrapper<ID2D1StrokeStyle1, CanvasStrokeStyle>>,
    TryCreate<IDXGISwapChain1, CanvasSwapChain, CreateFromManagerWithDeviceAndDpi<IDXGISwapChain1, CanvasSwapChain, CanvasSwapChainFactory>>,
    TryCreate<ID2D1CommandList, CanvasCommandList, MakeWrapperWithDevice<ID2D1CommandList, CanvasCommandList>>,
    TryCreate<IDWriteTextLayout2, CanvasTextLayout, MakeWrapperWithDevice<IDWriteTextLayout2, CanvasTextLayout>>,
    TryCreate<IDWriteTextFormat, CanvasTextFormat, MakeWrapper<IDWriteTextFormat, CanvasTextFormat>>,
    TryCreate<ID2D1Geometry, CanvasGeometry, MakeWrapperWithDevice<ID2D1Geometry, CanvasGeometry>>,
    TryCreate<ID2D1GeometryRealization, CanvasCachedGeometry, MakeWrapperWithDevice<ID2D1GeometryRealization, CanvasCachedGeometry>>,
#if WINVER > _WIN32_WINNT_WINBLUE
    TryCreate<ID2D1GradientMesh, CanvasGradientMesh, MakeWrapperWithDevice<ID2D1GradientMesh, CanvasGradientMesh>>,
#endif
};


// Called by the ResourceWrapper constructor, to add itself to the interop mapping table.
void ResourceManager::Add(IUnknown* resource, WeakRef const& wrapper)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto result = m_resources.insert(std::make_pair(resourceIdentity.Get(), wrapper));

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


static void ValidateDevice(IInspectable* wrapper, ICanvasDevice* device)
{
    // If the caller specified a device, and the wrapper has a device, these must match.
    // If the caller specified a device but the wrapper has no device, we'll allow that, ignoring the device parameter.
    // If the caller did not specify a device, we'll accept any existing wrapper instance regardless of its device.
    if (device)
    {
        auto wrapperWithDevice = MaybeAs<ICanvasResourceWrapperWithDevice>(wrapper);

        if (wrapperWithDevice)
        {
            ComPtr<ICanvasDevice> wrapperDevice;
            ThrowIfFailed(wrapperWithDevice->get_Device(&wrapperDevice));

            if (device != wrapperDevice.Get())
            {
                ThrowHR(E_INVALIDARG, HStringReference(Strings::ResourceTrackerWrongDevice).Get());
            }
        }
    }
}


static void ValidateDpi(IInspectable* wrapper, float dpi)
{
    // If the caller specified a dpi, and the wrapper has a dpi, these must match.
    // If the caller specified a dpi but the wrapper has no dpi, we'll allow that, ignoring the dpi parameter.
    // If the caller did not specify a dpi, we'll accept any existing wrapper instance regardless of its dpi.
    if (dpi != 0)
    {
        auto wrapperWithDpi = MaybeAs<ICanvasResourceWrapperWithDpi>(wrapper);

        if (wrapperWithDpi)
        {
            float wrapperDpi;
            ThrowIfFailed(wrapperWithDpi->get_Dpi(&wrapperDpi));

            if (dpi != wrapperDpi)
            {
                ThrowHR(E_INVALIDARG, HStringReference(Strings::ResourceTrackerWrongDpi).Get());
            }
        }
    }
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
            // TODO interop better error message on failure (+ test for that)
            ThrowHR(E_NOINTERFACE);
        }
    }

    // Validate that the object we got back reports the expected device and DPI.
    ValidateDevice(wrapper.Get(), device);
    ValidateDpi(wrapper.Get(), dpi);

    return wrapper;
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
