// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once


// The WinRT methods for manipulating these math types are just a COM wrapper on
// top of the optimized inline C++ version. So we start by including that header.
#include "../Cpp/WindowsMath.h"


// Namespace shortcuts make the wrapper methods more readable. All this code requires explicit namespace qualification,
// since we have two versions of every type. Impl = the optimized inline C++ version, Midl = the WinRT struct type.
namespace Impl = Windows::Math;
namespace Midl = ABI::Microsoft::Graphics::Canvas::Math;


// Helper methods make reinterpret_cast between WinRT and Impl types more readable.
#define DECLARE_ASIMPL_FUNCTION(Type)                                                       \
                                                                                            \
    __forceinline Impl::Type* asImpl(Midl::Type* value)                                     \
    {                                                                                       \
        static_assert(sizeof(Impl::Type) == sizeof(Midl::Type), "type sizes don't match");  \
                                                                                            \
        return reinterpret_cast<Impl::Type*>(value);                                        \
    }                                                                                       \
                                                                                            \
    __forceinline Impl::Type& asImpl(Midl::Type& value)                                     \
    {                                                                                       \
        return *asImpl(&value);                                                             \
    }


DECLARE_ASIMPL_FUNCTION(Vector2)
DECLARE_ASIMPL_FUNCTION(Vector3)
DECLARE_ASIMPL_FUNCTION(Vector4)
DECLARE_ASIMPL_FUNCTION(Matrix3x2)
DECLARE_ASIMPL_FUNCTION(Matrix4x4)
DECLARE_ASIMPL_FUNCTION(Plane)
DECLARE_ASIMPL_FUNCTION(Quaternion)


#undef DECLARE_ASIMPL_FUNCTION
