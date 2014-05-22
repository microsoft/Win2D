//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma once

#include "pch.h"

using namespace Microsoft::Graphics::Canvas::Math;


namespace MathTests
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

    inline bool Equal(Vector2 const& a, Vector2 const& b)
    {
        return Equal(a.X, b.X) && Equal(a.Y, b.Y);
    }

    inline bool Equal(Vector3 const& a, Vector3 const& b)
    {
        return Equal(a.X, b.X) && Equal(a.Y, b.Y) && Equal(a.Z, b.Z);
    }

    inline bool Equal(Vector4 const& a, Vector4 const& b)
    {
        return Equal(a.X, b.X) && Equal(a.Y, b.Y) && Equal(a.Z, b.Z) && Equal(a.W, b.W);
    }

    inline bool Equal(Matrix3x2 const& a, Matrix3x2 const& b)
    {
        return Equal(a.M11, b.M11) && Equal(a.M12, b.M12) &&
               Equal(a.M21, b.M21) && Equal(a.M22, b.M22) &&
               Equal(a.M31, b.M31) && Equal(a.M32, b.M32);
    }

    inline bool Equal(Matrix4x4 const& a, Matrix4x4 const& b)
    {
        return Equal(a.M11, b.M11) && Equal(a.M12, b.M12) && Equal(a.M13, b.M13) && Equal(a.M14, b.M14) &&
               Equal(a.M21, b.M21) && Equal(a.M22, b.M22) && Equal(a.M23, b.M23) && Equal(a.M24, b.M24) &&
               Equal(a.M31, b.M31) && Equal(a.M32, b.M32) && Equal(a.M33, b.M33) && Equal(a.M34, b.M34) &&
               Equal(a.M41, b.M41) && Equal(a.M42, b.M42) && Equal(a.M43, b.M43) && Equal(a.M44, b.M44);
    }

    inline bool Equal(Plane const& a, Plane const& b)
    {
        return Equal(a.Normal, b.Normal) && Equal(a.D, b.D);
    }

    inline bool Equal(Quaternion const& a, Quaternion const& b)
    {
        return Equal(a.X, b.X) && Equal(a.Y, b.Y) && Equal(a.Z, b.Z) && Equal(a.W, b.W);
    }

    inline bool EqualRotation(Quaternion const& a, Quaternion const& b)
    {
        return Equal(a, b) || Equal(a, QuaternionHelper::Negate(b));
    }
}


// Exact equality comparisons.
namespace Microsoft
{
    namespace Graphics
    {
        namespace Canvas
        {
            namespace Math
            {
                inline bool operator ==(Vector2 const& a, Vector2 const& b)
                {
                    return (a.X == b.X) && (a.Y == b.Y);
                }

                inline bool operator ==(Vector3 const& a, Vector3 const& b)
                {
                    return (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z);
                }

                inline bool operator ==(Vector4 const& a, Vector4 const& b)
                {
                    return (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z) && (a.W == b.W);
                }

                inline bool operator ==(Matrix3x2 const& a, Matrix3x2 const& b)
                {
                    return (a.M11 == b.M11) && (a.M12 == b.M12) &&
                        (a.M21 == b.M21) && (a.M22 == b.M22) &&
                        (a.M31 == b.M31) && (a.M32 == b.M32);
                }

                inline bool operator ==(Matrix4x4 const& a, Matrix4x4 const& b)
                {
                    return (a.M11 == b.M11) && (a.M12 == b.M12) && (a.M13 == b.M13) && (a.M14 == b.M14) &&
                        (a.M21 == b.M21) && (a.M22 == b.M22) && (a.M23 == b.M23) && (a.M24 == b.M24) &&
                        (a.M31 == b.M31) && (a.M32 == b.M32) && (a.M33 == b.M33) && (a.M34 == b.M34) &&
                        (a.M41 == b.M41) && (a.M42 == b.M42) && (a.M43 == b.M43) && (a.M44 == b.M44);
                }

                inline bool operator ==(Plane const& a, Plane const& b)
                {
                    return (a.Normal == b.Normal) && (a.D == b.D);
                }

                inline bool operator ==(Quaternion const& a, Quaternion const& b)
                {
                    return (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z) && (a.W == b.W);
                }
            }
        }
    }
}


// Specialize ToString templates so the test framework can format error messages containing these types.
namespace Microsoft 
{ 
    namespace VisualStudio 
    { 
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<Vector2>(Vector2 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{X:%f Y:%f}", value.X, value.Y);
                return tmp;
            }

            template<> static std::wstring ToString<Vector3>(Vector3 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{X:%f Y:%f Z:%f}", value.X, value.Y, value.Z);
                return tmp;
            }

            template<> static std::wstring ToString<Vector4>(Vector4 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{X:%f Y:%f Z:%f W:%f}", value.X, value.Y, value.Z, value.W);
                return tmp;
            }

            template<> static std::wstring ToString<Matrix4x4>(Matrix4x4 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{ {%f %f %f %f} {%f %f %f %f} {%f %f %f %f} {%f %f %f %f} }",
                                value.M11, value.M12, value.M13, value.M14,
                                value.M21, value.M22, value.M23, value.M24,
                                value.M31, value.M32, value.M33, value.M34,
                                value.M41, value.M42, value.M43, value.M44);
                return tmp;
            }

            template<> static std::wstring ToString<Matrix3x2>(Matrix3x2 const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{ {%f %f} {%f %f} {%f %f} }",
                                value.M11, value.M12,
                                value.M21, value.M22,
                                value.M31, value.M32);
                return tmp;
            }

            template<> static std::wstring ToString<Plane>(Plane const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{Normal:{X:%f Y:%f Z:%f} D:%f}", value.Normal.X, value.Normal.Y, value.Normal.Z, value.D);
                return tmp;
            }

            template<> static std::wstring ToString<Quaternion>(Quaternion const& value)
            {
                wchar_t tmp[256];
                swprintf_s(tmp, L"{X:%f Y:%f Z:%f W:%f}", value.X, value.Y, value.Z, value.W);
                return tmp;
            }
        }
    }
}
