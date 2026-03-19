// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasRadialGradientBrush.h"
#include "Gradients.h"

using namespace ABI::Microsoft::Graphics::Canvas::Brushes;
using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::UI;

ComPtr<CanvasRadialGradientBrush> CanvasRadialGradientBrush::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    ID2D1GradientStopCollection1* stopCollection)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    ComPtr<ICanvasDeviceInternal> deviceInternal;
    ThrowIfFailed(device.As(&deviceInternal));

    auto d2dBrush = deviceInternal->CreateRadialGradientBrush(stopCollection);

    auto canvasRadialGradientBrush = Make<CanvasRadialGradientBrush>(
        device.Get(),
        d2dBrush.Get());
    CheckMakeResult(canvasRadialGradientBrush);

    return canvasRadialGradientBrush;
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateSimple(
    ICanvasResourceCreator* resourceCreator,
    ABI::Windows::UI::Color startColor,
    ABI::Windows::UI::Color endColor,
    ICanvasRadialGradientBrush** brush)
{
    return CreateSimpleGradientBrush<CanvasRadialGradientBrush>(resourceCreator, startColor, endColor, brush);
 }

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateWithStops(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStops, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateWithEdgeBehaviorAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStops, edgeBehavior, alphaMode, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateWithEdgeBehaviorAndInterpolationOptions(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStop* gradientStops,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    CanvasColorSpace preInterpolationSpace,
    CanvasColorSpace postInterpolationSpace,
    CanvasBufferPrecision bufferPrecision,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStops, edgeBehavior, alphaMode, preInterpolationSpace, postInterpolationSpace, bufferPrecision, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateHdrSimple(
    ICanvasResourceCreator* resourceCreator,
    Vector4 startColorHdr,
    Vector4 endColorHdr,
    ICanvasRadialGradientBrush** brush)
{
    return CreateSimpleGradientBrush<CanvasRadialGradientBrush>(resourceCreator, startColorHdr, endColorHdr, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateHdrWithStops(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStops,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStops, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateHdrWithEdgeBehaviorAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStopsHdr,
    CanvasEdgeBehavior extend,
    CanvasAlphaMode alphaMode,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStopsHdr, extend, alphaMode, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateHdrWithEdgeBehaviorAndInterpolationOptions(
    ICanvasResourceCreator* resourceCreator,
    UINT32 gradientStopCount,
    CanvasGradientStopHdr* gradientStops,
    CanvasEdgeBehavior edgeBehavior,
    CanvasAlphaMode alphaMode,
    CanvasColorSpace preInterpolationSpace,
    CanvasColorSpace postInterpolationSpace,
    CanvasBufferPrecision bufferPrecision,
    ICanvasRadialGradientBrush** brush)
{
    return CreateGradientBrush<CanvasRadialGradientBrush>(resourceCreator, gradientStopCount, gradientStops, edgeBehavior, alphaMode, preInterpolationSpace, postInterpolationSpace, bufferPrecision, brush);
}

IFACEMETHODIMP CanvasRadialGradientBrushFactory::CreateRainbow(
    ICanvasResourceCreator* resourceCreator,
    float eldritchness,
    ICanvasRadialGradientBrush** canvasRadialGradientBrush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasRadialGradientBrush);

            ComPtr<ICanvasDevice> canvasDevice;
            ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

            ComPtr<ID2D1GradientStopCollection1> stopCollection =
                CreateRainbowGradientStopCollection(canvasDevice.Get(), eldritchness);

            auto newRadialBrush = CanvasRadialGradientBrush::CreateNew(resourceCreator, stopCollection.Get());

            ThrowIfFailed(newRadialBrush.CopyTo(canvasRadialGradientBrush));
        });
}

CanvasRadialGradientBrush::CanvasRadialGradientBrush(
    ICanvasDevice* device,
    ID2D1RadialGradientBrush* brush)
    : CanvasBrush(device)
    , ResourceWrapper(brush)
{
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_Center(_Out_ Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DPoint(GetResource()->GetCenter());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::put_Center(_In_ Vector2 value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetCenter(ToD2DPoint(value));
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_OriginOffset(_Out_ Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DPoint(GetResource()->GetGradientOriginOffset());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::put_OriginOffset(_In_ Vector2 value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetGradientOriginOffset(ToD2DPoint(value));
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_RadiusX(_Out_ float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = GetResource()->GetRadiusX();
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::put_RadiusX(_In_ float value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetRadiusX(value);
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_RadiusY(_Out_ float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = GetResource()->GetRadiusY();
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::put_RadiusY(_In_ float value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetRadiusY(value);
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_Stops(UINT32* valueCount, CanvasGradientStop** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            CopyStops(GetResource(), valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_StopsHdr(UINT32* valueCount, CanvasGradientStopHdr** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            CopyStops(GetResource(), valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_EdgeBehavior(CanvasEdgeBehavior* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = StaticCastAs<CanvasEdgeBehavior>(GetGradientStopCollection()->GetExtendMode());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_AlphaMode(CanvasAlphaMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DColorInterpolation(GetGradientStopCollection()->GetColorInterpolationMode());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_PreInterpolationSpace(CanvasColorSpace* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = StaticCastAs<CanvasColorSpace>(GetGradientStopCollection()->GetPreInterpolationSpace());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_PostInterpolationSpace(CanvasColorSpace* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = StaticCastAs<CanvasColorSpace>(GetGradientStopCollection()->GetPostInterpolationSpace());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::get_BufferPrecision(CanvasBufferPrecision* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = FromD2DBufferPrecision(GetGradientStopCollection()->GetBufferPrecision());
        });
}

IFACEMETHODIMP CanvasRadialGradientBrush::Close()
{
    CanvasBrush::Close();
    return ResourceWrapper::Close();
}

ComPtr<ID2D1Brush> CanvasRadialGradientBrush::GetD2DBrush(ID2D1DeviceContext*, GetBrushFlags)
{
    return GetResource();
}

ComPtr<ID2D1GradientStopCollection1> CanvasRadialGradientBrush::GetGradientStopCollection()
{
    auto& resource = GetResource();
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    resource->GetGradientStopCollection(stopCollection.GetAddressOf());
    ComPtr<ID2D1GradientStopCollection1> stopCollection1;
    ThrowIfFailed(stopCollection.As(&stopCollection1));
    return stopCollection1;
}

ActivatableClassWithFactory(CanvasRadialGradientBrush, CanvasRadialGradientBrushFactory);
