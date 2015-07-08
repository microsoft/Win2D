// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "CanvasBrush.h"

using namespace ABI::Microsoft::Graphics::Canvas::Brushes;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;

IFACEMETHODIMP CanvasBrush::get_Opacity(float *value)
{
    return ExceptionBoundary(
        [&]()
        {
            CheckInPointer(value);
            *value = GetD2DBrush(nullptr, GetBrushFlags::NoValidation)->GetOpacity();
        });
}

IFACEMETHODIMP CanvasBrush::put_Opacity(float value)
{
    return ExceptionBoundary(
        [&]()
        {
            GetD2DBrush(nullptr, GetBrushFlags::NoValidation)->SetOpacity(value);
        });
}


IFACEMETHODIMP CanvasBrush::get_Transform(Numerics::Matrix3x2 *value)
{
    return ExceptionBoundary(
        [&]()
        {
            CheckInPointer(value);

            GetD2DBrush(nullptr, GetBrushFlags::NoValidation)->GetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(value));
        });
}

IFACEMETHODIMP CanvasBrush::put_Transform(Numerics::Matrix3x2 value)
{
    return ExceptionBoundary(
        [&]()
        {
            GetD2DBrush(nullptr, GetBrushFlags::NoValidation)->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&value));
        });
}

IFACEMETHODIMP CanvasBrush::get_Device(ICanvasDevice** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            ThrowIfFailed(m_device.EnsureNotClosed().CopyTo(value));
        });
}

void CanvasBrush::Close()
{
    m_device.Close();
}
