// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "brushes/CanvasSolidColorBrush.h"
#include "brushes/CanvasLinearGradientBrush.h"
#include "brushes/CanvasRadialGradientBrush.h"
#include "drawing/CanvasGradientMesh.h"
#include "effects/ColorManagementProfile.h"
#include "effects/EffectTransferTable3D.h"
#include "geometry/CanvasCachedGeometry.h"
#include "images/CanvasCommandList.h"
#include "images/CanvasVirtualBitmap.h"
#include "text/CanvasFontFace.h"
#include "text/CanvasFontSet.h"
#include "text/CanvasTextLayout.h"
#include "text/CanvasTextRenderingParameters.h"
#include "text/CanvasTypography.h"
#include "text/CanvasTextAnalyzer.h"
#include "text/CanvasNumberSubstitution.h"
#include "svg/CanvasSvgDocument.h"
#include "svg/CanvasSvgElement.h"
#include "svg/CanvasSvgPaintAttribute.h"
#include "svg/CanvasSvgPathAttribute.h"
#include "svg/CanvasSvgPointsAttribute.h"
#include "svg/CanvasSvgStrokeDashArrayAttribute.h"


std::unordered_map<IUnknown*, WeakRef> ResourceManager::m_resources;
std::recursive_mutex ResourceManager::m_mutex;

// When adding new types here, please also update the "Types that support interop" table in winrt\docsrc\Interop.aml.
std::vector<ResourceManager::TryCreateFunction> ResourceManager::tryCreateFunctions =
{
    TryCreate<ID2D1Device1,                CanvasDevice,                      MakeWrapper>,
    TryCreate<ID2D1DeviceContext1,         CanvasDrawingSession,              MakeWrapper>,
    TryCreate<ID2D1Bitmap1,                CanvasRenderTarget,                MakeWrapperWithDevice,  IsRenderTargetBitmap>,
    TryCreate<ID2D1Bitmap1,                CanvasBitmap,                      MakeWrapperWithDevice>,
    TryCreate<ID2D1CommandList,            CanvasCommandList,                 MakeWrapperWithDevice>,
    TryCreate<IDXGISwapChain1,             CanvasSwapChain,                   MakeWrapperWithDeviceAndDpi>,
    TryCreate<ID2D1Geometry,               CanvasGeometry,                    MakeWrapperWithDevice>,
    TryCreate<ID2D1GeometryRealization,    CanvasCachedGeometry,              MakeWrapperWithDevice>,
    TryCreate<DWriteTextLayoutType,        CanvasTextLayout,                  MakeWrapperWithDevice>,
    TryCreate<IDWriteTextFormat1,          CanvasTextFormat,                  MakeWrapper>,
    TryCreate<ID2D1StrokeStyle1,           CanvasStrokeStyle,                 MakeWrapper>,
    TryCreate<ID2D1SolidColorBrush,        CanvasSolidColorBrush,             MakeWrapperWithDevice>,
    TryCreate<ID2D1LinearGradientBrush,    CanvasLinearGradientBrush,         MakeWrapperWithDevice>,
    TryCreate<ID2D1RadialGradientBrush,    CanvasRadialGradientBrush,         MakeWrapperWithDevice>,
    TryCreate<ID2D1ImageBrush,             CanvasImageBrush,                  MakeWrapperWithDevice>,
    TryCreate<ID2D1BitmapBrush1,           CanvasImageBrush,                  MakeWrapperWithDevice>,
#if WINVER > _WIN32_WINNT_WINBLUE
    TryCreate<ID2D1GradientMesh,           CanvasGradientMesh,                MakeWrapperWithDevice>,
    TryCreate<ID2D1ImageSource,            CanvasVirtualBitmap,               MakeWrapperWithDevice>,
    TryCreate<ID2D1TransformedImageSource, CanvasVirtualBitmap,               MakeWrapperWithDevice>,
    TryCreate<ID2D1LookupTable3D,          EffectTransferTable3D,             MakeWrapperWithDevice>,
    TryCreate<IDWriteRenderingParams3,     CanvasTextRenderingParameters,     MakeWrapper>,
    TryCreate<IDWriteFontSet,              CanvasFontSet,                     MakeWrapper>,
    TryCreate<IDWriteFontFaceReference,    CanvasFontFace,                    MakeWrapper>,
    TryCreate<ID2D1SvgDocument,            CanvasSvgDocument,                 MakeWrapperWithDevice>,
    TryCreate<ID2D1SvgElement,             CanvasSvgTextElement,              MakeWrapperWithDevice,  IsSvgTextElement>,
    TryCreate<ID2D1SvgElement,             CanvasSvgNamedElement,             MakeWrapperWithDevice>,

    TryCreate<ID2D1SvgPaint,               CanvasSvgPaintAttribute,           MakeWrapperWithDevice>,
    TryCreate<ID2D1SvgPathData,            CanvasSvgPathAttribute,            MakeWrapperWithDevice>,
    TryCreate<ID2D1SvgPointCollection,     CanvasSvgPointsAttribute,          MakeWrapperWithDevice>,
    TryCreate<ID2D1SvgStrokeDashArray,     CanvasSvgStrokeDashArrayAttribute, MakeWrapperWithDevice>,
#else
    TryCreate<IDWriteRenderingParams2,     CanvasTextRenderingParameters, MakeWrapper>,
    TryCreate<IDWriteFontCollection,       CanvasFontSet,                 MakeWrapper>,
    TryCreate<IDWriteFont2,                CanvasFontFace,                MakeWrapper>,
#endif
    TryCreate<IDWriteTypography,           CanvasTypography,              MakeWrapper>,
    TryCreate<IDWriteNumberSubstitution,   CanvasNumberSubstitution,      MakeWrapper>,
    TryCreate<ID2D1ColorContext,           ColorManagementProfile,        MakeWrapperWithDevice>,

    // Effects get their very own try-create function. These are special because ID2D1Effect
    // can map to many different Win2D wrapper types depending on its D2D1_PROPERTY_CLSID.
    CanvasEffect::TryCreateEffect
};


// Called by the ResourceWrapper constructor, to add itself to the interop mapping table.
void ResourceManager::Add(IUnknown* resource, IInspectable* wrapper)
{
    if (!TryAdd(resource, wrapper))
        ThrowHR(E_UNEXPECTED);
}

// Exposed through CanvasDeviceFactory::Add.
bool ResourceManager::TryAdd(IUnknown* resource, IInspectable* wrapper)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto result = m_resources.insert(std::make_pair(resourceIdentity.Get(), AsWeak(wrapper)));

    return result.second;
}

// Called by ResourceWrapper::Close, to remove itself from the interop mapping table.
void ResourceManager::Remove(IUnknown* resource)
{
    if (!TryRemove(resource))
        ThrowHR(E_UNEXPECTED);
}

// Exposed through CanvasDeviceFactory::Remove.
bool ResourceManager::TryRemove(IUnknown* resource)
{
    ComPtr<IUnknown> resourceIdentity = AsUnknown(resource);

    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    auto result = m_resources.erase(resourceIdentity.Get());

    return result == 1;
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
    // This method only checks that the wrapper does not have a fixed canvas device associated
    // with it that does not match the input one. If the input one is null, then the method will
    // always succeed. Therefore, in that case we can skip all remaining work and avoid QI calls.
    if (!device)
    {
        return;
    }

    // First check for ICanvasResourceWrapperWithDevice, implemented by all internal objects bound
    // to a creation device. This applies to eg. bitmap image types as well as all geometric shapes.
    auto wrapperWithDevice = MaybeAs<ICanvasResourceWrapperWithDevice>(wrapper);

    if (wrapperWithDevice)
    {
        ValidateDevice(wrapperWithDevice.Get(), device);
    }
    else
    {
        // Then, also check for ICanvasImageInterop to cover external effects too. Note that we can't just
        // check against this interface, as it doesn't cover all possible cases for internal wrappers. For
        // instance, this interface is not implemented by geometry shapes and SVG objects.
        auto canvasImageInterop = MaybeAs<ICanvasImageInterop>(wrapper);

        if (canvasImageInterop)
        {
            ValidateDevice(canvasImageInterop.Get(), device);
        }
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

void ResourceManager::ValidateDevice(ICanvasImageInterop* wrapper, ICanvasDevice* device)
{
    if (device)
    {
        ComPtr<ICanvasDevice> wrapperDevice;
        WIN2D_GET_DEVICE_ASSOCIATION_TYPE deviceInfo;
        ThrowIfFailed(wrapper->GetDevice(&wrapperDevice, &deviceInfo));

        // Only check and fail if WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE is used, as that would
        // have the same semantics as the internal ICanvasResourceWrapperWithDevice interface. That is, effects
        // returning this value from GetDevice are bound to a single device upon creation, which never changes.
        if (deviceInfo == WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE && device != wrapperDevice.Get())
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
