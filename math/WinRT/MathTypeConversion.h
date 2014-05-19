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
#define DECLARE_ASIMPL_FUNCTION(MidlType, ImplType)                                         \
                                                                                            \
    __forceinline ImplType* asImpl(MidlType* value)                                         \
    {                                                                                       \
        static_assert(sizeof(ImplType) == sizeof(MidlType), "type sizes don't match");      \
                                                                                            \
        return reinterpret_cast<ImplType*>(value);                                          \
    }                                                                                       \
                                                                                            \
    __forceinline ImplType& asImpl(MidlType& value)                                         \
    {                                                                                       \
        return *asImpl(&value);                                                             \
    }


DECLARE_ASIMPL_FUNCTION(Midl::Vector2,    Impl::float2)
DECLARE_ASIMPL_FUNCTION(Midl::Vector3,    Impl::float3)
DECLARE_ASIMPL_FUNCTION(Midl::Vector4,    Impl::float4)
DECLARE_ASIMPL_FUNCTION(Midl::Matrix3x2,  Impl::float3x2)
DECLARE_ASIMPL_FUNCTION(Midl::Matrix4x4,  Impl::float4x4)
DECLARE_ASIMPL_FUNCTION(Midl::Plane,      Impl::plane)
DECLARE_ASIMPL_FUNCTION(Midl::Quaternion, Impl::quaternion)


#undef DECLARE_ASIMPL_FUNCTION
