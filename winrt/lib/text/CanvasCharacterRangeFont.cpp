// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasCharacterRangeFont.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

CanvasCharacterRangeFont::CanvasCharacterRangeFont(
    int characterIndex,
    int characterCount,
    ICanvasFontFace* fontFace, 
    float scaleFactor)
    : m_characterIndex(characterIndex)
    , m_characterCount(characterCount)
    , m_fontFace(fontFace)
    , m_scaleFactor(scaleFactor)
{
}        

IFACEMETHODIMP CanvasCharacterRangeFont::get_CharacterIndex(int* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_characterIndex;
        });
}

IFACEMETHODIMP CanvasCharacterRangeFont::get_CharacterCount(int* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_characterCount;
        });
}

IFACEMETHODIMP CanvasCharacterRangeFont::get_FontFace(ICanvasFontFace** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);

            m_fontFace.CopyTo(value);
        });
}

IFACEMETHODIMP CanvasCharacterRangeFont::get_ScaleFactor(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_scaleFactor;
        });
}