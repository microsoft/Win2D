// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "EffectTransferTable3D.h"


static D2D1_BUFFER_PRECISION ConvertTableFormatToBufferPrecision(DirectXPixelFormat format)
{
    switch (format)
    {
        case PIXEL_FORMAT(R8G8B8A8UIntNormalized):      return D2D1_BUFFER_PRECISION_8BPC_UNORM;
        case PIXEL_FORMAT(R8G8B8A8UIntNormalizedSrgb):  return D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB;
        case PIXEL_FORMAT(R16G16B16A16UIntNormalized):  return D2D1_BUFFER_PRECISION_16BPC_UNORM;
        case PIXEL_FORMAT(R16G16B16A16Float):           return D2D1_BUFFER_PRECISION_16BPC_FLOAT;
        case PIXEL_FORMAT(R32G32B32A32Float):           return D2D1_BUFFER_PRECISION_32BPC_FLOAT;

        default:
            ThrowHR(WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT);
    }
}


ComPtr<EffectTransferTable3D> EffectTransferTable3D::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    uint32_t colorCount,
    Color* colors,
    int32_t sizeB,
    int32_t sizeG,
    int32_t sizeR)
{
    CheckInPointer(colors);

    auto convertedBytes = ConvertColorsToRgba(colorCount, colors);

    return CreateNew(
        resourceCreator,
        static_cast<uint32_t>(convertedBytes.size()),
        convertedBytes.data(),
        sizeB,
        sizeG,
        sizeR,
        PIXEL_FORMAT(R8G8B8A8UIntNormalized));
}


ComPtr<EffectTransferTable3D> EffectTransferTable3D::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    uint32_t byteCount,
    BYTE* bytes,
    int32_t sizeB,
    int32_t sizeG,
    int32_t sizeR,
    DirectXPixelFormat format)
{
    CheckInPointer(resourceCreator);
    CheckInPointer(bytes);

    if (sizeB < 2 || sizeB > 256 ||
        sizeG < 2 || sizeG > 256 ||
        sizeR < 2 || sizeR > 256)
    {
        ThrowHR(E_INVALIDARG);
    }

    D2D1_BUFFER_PRECISION precision = ConvertTableFormatToBufferPrecision(format);

    auto bytesPerPixel = GetBytesPerBlock(static_cast<DXGI_FORMAT>(format));

    auto bytesNeeded = bytesPerPixel * sizeB * sizeG * sizeR;

    if (byteCount < bytesNeeded)
    {
        ThrowHR(E_INVALIDARG);
    }

    // Get a resource creation device context.
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
    auto deviceContext = As<ID2D1DeviceContext2>(lease.Get());

    // Create the D2D lookup table resource.
    uint32_t extents[3] =
    {
        static_cast<uint32_t>(sizeB),
        static_cast<uint32_t>(sizeG),
        static_cast<uint32_t>(sizeR),
    };

    uint32_t strides[2] =
    {
        bytesPerPixel * sizeB,
        bytesPerPixel * sizeB * sizeG,
    };

    ComPtr<ID2D1LookupTable3D> lookupTable;
    ThrowIfFailed(deviceContext->CreateLookupTable3D(precision, extents, bytes, byteCount, strides, &lookupTable));

    // Create the Win2D wrapper.
    auto transferTable = Make<EffectTransferTable3D>(device.Get(), lookupTable.Get());
    CheckMakeResult(transferTable);

    return transferTable;
}


EffectTransferTable3D::EffectTransferTable3D(ICanvasDevice* device, ID2D1LookupTable3D* lookupTable)
    : ResourceWrapper(lookupTable)
    , m_device(device)
{
}


IFACEMETHODIMP EffectTransferTable3D::Close()
{
    m_device.Close();

    return ResourceWrapper::Close();
}


IFACEMETHODIMP EffectTransferTable3D::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(device);

        ThrowIfFailed(m_device.EnsureNotClosed().CopyTo(device));
    });
}


IFACEMETHODIMP EffectTransferTable3DFactory::CreateFromColors(
    ICanvasResourceCreator* resourceCreator,
    uint32_t colorCount,
    Color* colors,
    int32_t sizeB,
    int32_t sizeG,
    int32_t sizeR,
    IEffectTransferTable3D** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto transferTable = EffectTransferTable3D::CreateNew(resourceCreator, colorCount, colors, sizeB, sizeG, sizeR);

        ThrowIfFailed(transferTable.CopyTo(result));
    });
}


IFACEMETHODIMP EffectTransferTable3DFactory::CreateFromBytes(
    ICanvasResourceCreator* resourceCreator,
    uint32_t byteCount,
    BYTE* bytes,
    int32_t sizeB,
    int32_t sizeG,
    int32_t sizeR,
    DirectXPixelFormat format,
    IEffectTransferTable3D** result)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(result);

        auto transferTable = EffectTransferTable3D::CreateNew(resourceCreator, byteCount, bytes, sizeB, sizeG, sizeR, format);

        ThrowIfFailed(transferTable.CopyTo(result));
    });
}


ActivatableStaticOnlyFactory(EffectTransferTable3DFactory);
