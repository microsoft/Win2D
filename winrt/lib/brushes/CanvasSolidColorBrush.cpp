// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasSolidColorBrush.h"

using namespace ABI::Microsoft::Graphics::Canvas::Brushes;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;


static ComPtr<CanvasSolidColorBrush> CreateSolidColorBrush(
    ICanvasResourceCreator* resourceCreator,
    D2D1_COLOR_F d2dColor)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
    ThrowIfFailed(device.As(&canvasDeviceInternal));

    auto d2dBrush = canvasDeviceInternal->CreateSolidColorBrush(d2dColor);

    auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
        device.Get(),
        d2dBrush.Get());
    CheckMakeResult(canvasSolidColorBrush);

    return canvasSolidColorBrush;
};


ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrush::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    Color color)
{
    return CreateSolidColorBrush(resourceCreator, ToD2DColor(color));
};


ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrush::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    Vector4 colorHdr)
{
    return CreateSolidColorBrush(resourceCreator, *ReinterpretAs<D2D1_COLOR_F*>(&colorHdr));
}



IFACEMETHODIMP CanvasSolidColorBrushFactory::Create(
    ICanvasResourceCreator* resourceCreator,
    ABI::Windows::UI::Color color,
    ICanvasSolidColorBrush** canvasSolidColorBrush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasSolidColorBrush);

            auto newSolidColorBrush = CanvasSolidColorBrush::CreateNew(
                resourceCreator,
                color);

            ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
        });
}


IFACEMETHODIMP CanvasSolidColorBrushFactory::CreateHdr(
    ICanvasResourceCreator* resourceCreator,
    Numerics::Vector4 colorHdr,
    ICanvasSolidColorBrush** canvasSolidColorBrush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasSolidColorBrush);

            auto newSolidColorBrush = CanvasSolidColorBrush::CreateNew(
                resourceCreator,
                colorHdr);

            ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
        });
}


CanvasSolidColorBrush::CanvasSolidColorBrush(
    ICanvasDevice *device,
    ID2D1SolidColorBrush* brush)
    : CanvasBrush(device)
    , ResourceWrapper(brush)
{
}

IFACEMETHODIMP CanvasSolidColorBrush::get_Color(Color* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = ToWindowsColor(GetResource()->GetColor());
        });
}

IFACEMETHODIMP CanvasSolidColorBrush::put_Color(Color value)
{
    return ExceptionBoundary(
        [&]
        {                
            GetResource()->SetColor(ToD2DColor(value));
        });
}

IFACEMETHODIMP CanvasSolidColorBrush::get_ColorHdr(Numerics::Vector4* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto d2dColor = GetResource()->GetColor();
            *value = *ReinterpretAs<Vector4*>(&d2dColor);
        });
}

IFACEMETHODIMP CanvasSolidColorBrush::put_ColorHdr(Numerics::Vector4 value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource()->SetColor(*ReinterpretAs<D2D1_COLOR_F*>(&value));
        });

}

IFACEMETHODIMP CanvasSolidColorBrush::Close()
{
    CanvasBrush::Close();
    return ResourceWrapper::Close();
}

ComPtr<ID2D1Brush> CanvasSolidColorBrush::GetD2DBrush(ID2D1DeviceContext*, GetBrushFlags)
{
    return GetResource();
}

ActivatableClassWithFactory(CanvasSolidColorBrush, CanvasSolidColorBrushFactory);
