// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    // Declare string constants.
    namespace Strings
    {
        #define STRING(name, value)     extern wchar_t const* const name;
        #define STRING_A(name, value)   extern char const* const name;

        #include "Strings.inl"
  
        #undef STRING
        #undef STRING_A
    }
}}}}
