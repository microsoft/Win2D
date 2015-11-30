// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "TextUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{

    static bool IsInRange(uint32_t n, uint32_t lowerBound, uint32_t upperBound)
    {
        return lowerBound <= n && n <= upperBound;
    }

    static bool AreValidUtf16Surrogates(uint32_t leading, uint32_t trailing)
    {
        const bool isHighSurrogate = IsInRange(leading, 0xD800, 0xDBFF);

        const bool isLowSurrogate = IsInRange(trailing, 0xDC00, 0xDFFF);

        return isHighSurrogate && isLowSurrogate;
    }

    static void ConvertUtf32CodepointToUtf16(uint32_t value, wchar_t* buffer)
    {
        const uint32_t leadOffset = 0xD800 - (0x10000 >> 10u);

        const uint32_t leading = leadOffset + (value >> 10u);
        const uint32_t trailing = 0xDC00 + (value & 0x3FF);
        
        //
        // It's impossible to get invalid values here; DWrite fails if
        // it was passed an invalid UTF32 code point.
        //
        assert(AreValidUtf16Surrogates(leading, trailing));

        buffer[0] = static_cast<wchar_t>(leading);
        buffer[1] = static_cast<wchar_t>(trailing);
    }

    static uint32_t ConvertUtf16ToUtf32Codepoint(wchar_t const* sourceString)
    {
        uint32_t leading = sourceString[0];
        uint32_t trailing = sourceString[1];

        if (AreValidUtf16Surrogates(leading, trailing))
        {
            return ((leading & 0x03FF) << 10 | (trailing & 0x03FF)) + 0x10000;
        }
        else
        {
            ThrowHR(E_INVALIDARG);
        }
    }

    WinString ToCanvasTrimmingDelimiter(uint32_t value)
    {
        WinStringBuilder builder;

        if (value == 0)
        {
            return WinString();
        }
        else if (value <= 0x0000FFFF)
        {
            auto buffer = builder.Allocate(1);
            buffer[0] = static_cast<wchar_t>(value);
        }
        else
        {
            auto buffer = builder.Allocate(2);
            ConvertUtf32CodepointToUtf16(value, buffer);
        }

        return builder.Get();
    }

    uint32_t ToTrimmingDelimiter(WinString const& value)
    {
        uint32_t sourceStringLength = 0;
        auto sourceString = WindowsGetStringRawBuffer(value, &sourceStringLength);

        if (sourceStringLength == 0)
        {
            return 0;
        }
        else if (sourceStringLength == 1)
        {
            return sourceString[0];
        }
        else
        {
            if (sourceStringLength > 2)
                ThrowHR(E_INVALIDARG);

            return ConvertUtf16ToUtf32Codepoint(sourceString);
        }
    }
    
}}}}}
