// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    // Define string constants.
    namespace Strings
    {
        #define STRING(name, value)     wchar_t const* const name = value;
        #define STRING_A(name, value)   char const* const name = value;

        #include "Strings.inl"

        #undef STRING
        #undef STRING_A
    }
}}}}
