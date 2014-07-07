// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

#include "pch.h"

using namespace Windows::Foundation::Numerics;


namespace NumericsTests
{
    // Angle conversion helper.
    inline float ToRadians(float degrees)
    {
        return degrees * DirectX::XM_PI / 180.0f;
    }


    // Test for +ve and -ve infinity.
    inline bool IsPositiveInfinity(float value)
    {
        return isinf(value) && value > 0;
    }

    inline bool IsNegativeInfinity(float value)
    {
        return isinf(value) && value < 0;
    }


    // Comparison helpers with small tolerance to allow for floating point rounding during computations.
    inline bool Equal(float a, float b)
    {
        return (fabs(a - b) < 1e-5);
    }

    inline bool Equal(float2 const& a, float2 const& b)
    {
        return Equal(a.x, b.x) && Equal(a.y, b.y);
    }

    inline bool Equal(float3 const& a, float3 const& b)
    {
        return Equal(a.x, b.x) && Equal(a.y, b.y) && Equal(a.z, b.z);
    }

    inline bool Equal(float4 const& a, float4 const& b)
    {
        return Equal(a.x, b.x) && Equal(a.y, b.y) && Equal(a.z, b.z) && Equal(a.w, b.w);
    }

    inline bool Equal(float3x2 const& a, float3x2 const& b)
    {
        return Equal(a.m11, b.m11) && Equal(a.m12, b.m12) &&
               Equal(a.m21, b.m21) && Equal(a.m22, b.m22) &&
               Equal(a.m31, b.m31) && Equal(a.m32, b.m32);
    }

    inline bool Equal(float4x4 const& a, float4x4 const& b)
    {
        return Equal(a.m11, b.m11) && Equal(a.m12, b.m12) && Equal(a.m13, b.m13) && Equal(a.m14, b.m14) &&
               Equal(a.m21, b.m21) && Equal(a.m22, b.m22) && Equal(a.m23, b.m23) && Equal(a.m24, b.m24) &&
               Equal(a.m31, b.m31) && Equal(a.m32, b.m32) && Equal(a.m33, b.m33) && Equal(a.m34, b.m34) &&
               Equal(a.m41, b.m41) && Equal(a.m42, b.m42) && Equal(a.m43, b.m43) && Equal(a.m44, b.m44);
    }

    inline bool Equal(plane const& a, plane const& b)
    {
        return Equal(a.normal, b.normal) && Equal(a.d, b.d);
    }

    inline bool Equal(quaternion const& a, quaternion const& b)
    {
        return Equal(a.x, b.x) && Equal(a.y, b.y) && Equal(a.z, b.z) && Equal(a.w, b.w);
    }

    inline bool EqualRotation(quaternion a, quaternion b)
    {
        return Equal(a, b) || Equal(a, -b);
    }
}


// Specialize ToString templates so the test framework can format error messages containing these types.
namespace Microsoft 
{ 
    namespace VisualStudio 
    { 
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<float2>(float2 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{x:%f y:%f}", value.x, value.y);
                return tmp;
            }

            template<> static std::wstring ToString<float3>(float3 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{x:%f y:%f z:%f}", value.x, value.y, value.z);
                return tmp;
            }

            template<> static std::wstring ToString<float4>(float4 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{x:%f y:%f z:%f w:%f}", value.x, value.y, value.z, value.w);
                return tmp;
            }

            template<> static std::wstring ToString<float4x4>(float4x4 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{ {%f %f %f %f} {%f %f %f %f} {%f %f %f %f} {%f %f %f %f} }",
                                value.m11, value.m12, value.m13, value.m14,
                                value.m21, value.m22, value.m23, value.m24,
                                value.m31, value.m32, value.m33, value.m34,
                                value.m41, value.m42, value.m43, value.m44);
                return tmp;
            }

            template<> static std::wstring ToString<float3x2>(float3x2 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{ {%f %f} {%f %f} {%f %f} }",
                                value.m11, value.m12,
                                value.m21, value.m22,
                                value.m31, value.m32);
                return tmp;
            }

            template<> static std::wstring ToString<plane>(plane const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{Normal:{x:%f y:%f z:%f} d:%f}", value.normal.x, value.normal.y, value.normal.z, value.d);
                return tmp;
            }

            template<> static std::wstring ToString<quaternion>(quaternion const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{x:%f y:%f z:%f w:%f}", value.x, value.y, value.z, value.w);
                return tmp;
            }
        }
    }
}
