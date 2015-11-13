// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasLinearGradientBrush.h"
#include "Gradients.h"

using namespace ABI::Microsoft::Graphics::Canvas::Brushes;
using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Windows::UI;

ComPtr<CanvasLinearGradientBrush> CanvasLinearGradientBrush::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    ID2D1GradientStopCollection1* stopCollection)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    ComPtr<ICanvasDeviceInternal> deviceInternal;
    ThrowIfFailed(device.As(&deviceInternal));

    auto d2dBrush = deviceInternal->CreateLinearGradientBrush(stopCollection);

    auto canvasLinearGradientBrush = Make<CanvasLinearGradientBrush>(
        device.Get(),
        d2dBrush.Get());
    CheckMakeResult(canvasLinearGradientBrush);

    return canvasLinearGradientBrush;
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateSimple(
    ICanvasResourceCreator* resourceCreator,
    ABI::Windows::UI::Color startColor,
    ABI::Windows::UI::Color endColor,
    ICanvasLinearGradientBrush** brush)
{
    return CreateSimpleGradientBrush<CanvasLinearGradientBrush>(resourceCreator, startColor, endColor, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateWithStops(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStops, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateWithEdgeBehaviorAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStops, edgeBehavior, alphaMode, brush);
}    

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateWithEdgeBehaviorAndInterpolationOptions(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    CanvasColorSpace preInterpolationSpace,
    CanvasColorSpace postInterpolationSpace,
    CanvasBufferPrecision bufferPrecision,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStops, edgeBehavior, alphaMode, preInterpolationSpace, postInterpolationSpace, bufferPrecision, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateHdrSimple(
    ICanvasResourceCreator* resourceCreator,
    Vector4 startColorHdr,
    Vector4 endColorHdr,
    ICanvasLinearGradientBrush** brush)
{
    return CreateSimpleGradientBrush<CanvasLinearGradientBrush>(resourceCreator, startColorHdr, endColorHdr, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateHdrWithStops(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStopsHdr,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStopsHdr, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateHdrWithEdgeBehaviorAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStopsHdr,
    CanvasEdgeBehavior extend,
    CanvasAlphaMode alphaMode,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStopsHdr, extend, alphaMode, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateHdrWithEdgeBehaviorAndInterpolationOptions(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStopsHdr,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    CanvasColorSpace preInterpolationSpace,
    CanvasColorSpace postInterpolationSpace,
    CanvasBufferPrecision bufferPrecision,
    ICanvasLinearGradientBrush** brush)
{
    return CreateGradientBrush<CanvasLinearGradientBrush>(resourceCreator, gradientStopCount, gradientStopsHdr, edgeBehavior, alphaMode, preInterpolationSpace, postInterpolationSpace, bufferPrecision, brush);
}

IFACEMETHODIMP CanvasLinearGradientBrushFactory::CreateRainbow(
    ICanvasResourceCreator* resourceCreator,
    float eldritchness,
    ICanvasLinearGradientBrush** canvasLinearGradientBrush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasLinearGradientBrush);

            ComPtr<ICanvasDevice> canvasDevice;
            ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

            ComPtr<ID2D1GradientStopCollection1> stopCollection =
                CreateRainbowGradientStopCollection(canvasDevice.Get(), eldritchness);

            auto newLinearBrush = CanvasLinearGradientBrush::CreateNew(resourceCreator, stopCollection.Get());

            ThrowIfFailed(newLinearBrush.CopyTo(canvasLinearGradientBrush));
        });
}

CanvasLinearGradientBrush::CanvasLinearGradientBrush(
    ICanvasDevice* device,
    ID2D1LinearGradientBrush* brush)
    : CanvasBrush(device)
    , ResourceWrapper(brush)
{
    assert(brush != nullptr);
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_StartPoint(_Out_ Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DPoint(GetResource()->GetStartPoint());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::put_StartPoint(_In_ Vector2 value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetStartPoint(ToD2DPoint(value));
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_EndPoint(_Out_ Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DPoint(GetResource()->GetEndPoint());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::put_EndPoint(_In_ Vector2 value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetEndPoint(ToD2DPoint(value));
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_Stops(UINT32* valueCount, CanvasGradientStop** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            CopyStops(GetResource(), valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_StopsHdr(UINT32* valueCount, CanvasGradientStopHdr** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            CopyStops(GetResource(), valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_EdgeBehavior(CanvasEdgeBehavior* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = static_cast<CanvasEdgeBehavior>(GetGradientStopCollection()->GetExtendMode());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_AlphaMode(CanvasAlphaMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DColorInterpolation(GetGradientStopCollection()->GetColorInterpolationMode());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_PreInterpolationSpace(CanvasColorSpace* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = static_cast<CanvasColorSpace>(GetGradientStopCollection()->GetPreInterpolationSpace());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_PostInterpolationSpace(CanvasColorSpace* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = static_cast<CanvasColorSpace>(GetGradientStopCollection()->GetPostInterpolationSpace());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::get_BufferPrecision(CanvasBufferPrecision* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DBufferPrecision(GetGradientStopCollection()->GetBufferPrecision());
        });
}

IFACEMETHODIMP CanvasLinearGradientBrush::Close()
{
    CanvasBrush::Close();
    return ResourceWrapper::Close();
}

ComPtr<ID2D1Brush> CanvasLinearGradientBrush::GetD2DBrush(ID2D1DeviceContext*, GetBrushFlags)
{
    return GetResource();
}

ComPtr<ID2D1GradientStopCollection1> CanvasLinearGradientBrush::GetGradientStopCollection()
{
    auto& resource = GetResource();
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    resource->GetGradientStopCollection(stopCollection.GetAddressOf());
    ComPtr<ID2D1GradientStopCollection1> stopCollection1;
    ThrowIfFailed(stopCollection.As(&stopCollection1));
    return stopCollection1;
}

ActivatableClassWithFactory(CanvasLinearGradientBrush, CanvasLinearGradientBrushFactory);
