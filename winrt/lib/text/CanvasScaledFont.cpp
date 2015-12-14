// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasScaledFont.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

CanvasScaledFont::CanvasScaledFont(
    ICanvasFontFace* fontFace, 
    float scaleFactor)
    : m_fontFace(fontFace)
    , m_scaleFactor(scaleFactor)
{
}

IFACEMETHODIMP CanvasScaledFont::get_FontFace(ICanvasFontFace** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);

            m_fontFace.CopyTo(value);
        });
}

IFACEMETHODIMP CanvasScaledFont::get_ScaleFactor(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_scaleFactor;
        });
}