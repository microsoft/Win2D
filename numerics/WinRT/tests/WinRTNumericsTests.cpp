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

#include "pch.h"

#ifdef USE_LOCALLY_EMULATED_UAP_APIS

using namespace Microsoft::Graphics::Canvas::Numerics;

// Because these WinRT types are just structures without any methods, there is no actual 
// executable code to validate here. But we do want to make sure the structure size and 
// layout is what we expect. This is important because the C++/CX and .NET projections 
// will be remapping to their own language specific implementations, which must match the 
// layout of the WinRT structs. Structure layout can be statically validated at compile 
// time, so there is no need to actually execute this test: if it compiles, it passed.

namespace WinRTNumericsTests
{
    // Embed Vector2 in other structures to make sure there's no unexpected padding.
    struct Vector2_2x
    {
        Vector2 a;
        Vector2 b;
    };

    struct Vector2PlusFloat
    {
        Vector2 v;
        float f;
    };

    struct Vector2PlusFloat_2x
    {
        Vector2PlusFloat a;
        Vector2PlusFloat b;
    };

    // Make sure Vector2 has the expected size and padding.
    static_assert(sizeof(Vector2) == 8, "wrong sizeof(Vector2)");
    static_assert(sizeof(Vector2_2x) == 16, "wrong sizeof(Vector2_2x)");
    static_assert(sizeof(Vector2PlusFloat) == 12, "wrong sizeof(Vector2PlusFloat)");
    static_assert(sizeof(Vector2PlusFloat_2x) == 24, "wrong sizeof(Vector2PlusFloat_2x)");

    // Make sure Vector2 has the expected field offsets.
    static_assert(offsetof(Vector2, X) == 0, "wrong offsetof(Vector2, X)");
    static_assert(offsetof(Vector2, Y) == 4, "wrong offsetof(Vector2, Y)");


    // Embed Vector3 in other structures to make sure there's no unexpected padding.
    struct Vector3_2x
    {
        Vector3 a;
        Vector3 b;
    };

    struct Vector3PlusFloat
    {
        Vector3 v;
        float f;
    };

    struct Vector3PlusFloat_2x
    {
        Vector3PlusFloat a;
        Vector3PlusFloat b;
    };

    // Make sure Vector3 has the expected size and padding.
    static_assert(sizeof(Vector3) == 12, "wrong sizeof(Vector3)");
    static_assert(sizeof(Vector3_2x) == 24, "wrong sizeof(Vector3_2x)");
    static_assert(sizeof(Vector3PlusFloat) == 16, "wrong sizeof(Vector3PlusFloat)");
    static_assert(sizeof(Vector3PlusFloat_2x) == 32, "wrong sizeof(Vector3PlusFloat_2x)");

    // Make sure Vector3 has the expected field offsets.
    static_assert(offsetof(Vector3, X) == 0, "wrong offsetof(Vector3, X)");
    static_assert(offsetof(Vector3, Y) == 4, "wrong offsetof(Vector3, Y)");
    static_assert(offsetof(Vector3, Z) == 8, "wrong offsetof(Vector3, Z)");


    // Embed Vector4 in other structures to make sure there's no unexpected padding.
    struct Vector4_2x
    {
        Vector4 a;
        Vector4 b;
    };

    struct Vector4PlusFloat
    {
        Vector4 v;
        float f;
    };

    struct Vector4PlusFloat_2x
    {
        Vector4PlusFloat a;
        Vector4PlusFloat b;
    };

    // Make sure Vector4 has the expected size and padding.
    static_assert(sizeof(Vector4) == 16, "wrong sizeof(Vector4)");
    static_assert(sizeof(Vector4_2x) == 32, "wrong sizeof(Vector4_2x)");
    static_assert(sizeof(Vector4PlusFloat) == 20, "wrong sizeof(Vector4PlusFloat)");
    static_assert(sizeof(Vector4PlusFloat_2x) == 40, "wrong sizeof(Vector4PlusFloat_2x)");

    // Make sure Vector4 has the expected field offsets.
    static_assert(offsetof(Vector4, X) == 0, "wrong offsetof(Vector4, X)");
    static_assert(offsetof(Vector4, Y) == 4, "wrong offsetof(Vector4, Y)");
    static_assert(offsetof(Vector4, Z) == 8, "wrong offsetof(Vector4, Z)");
    static_assert(offsetof(Vector4, W) == 12, "wrong offsetof(Vector4, W)");


    // Embed Matrix3x2 in other structures to make sure there's no unexpected padding.
    struct Matrix3x2_2x
    {
        Matrix3x2 a;
        Matrix3x2 b;
    };

    struct Matrix3x2PlusFloat
    {
        Matrix3x2 v;
        float f;
    };

    struct Matrix3x2PlusFloat_2x
    {
        Matrix3x2PlusFloat a;
        Matrix3x2PlusFloat b;
    };

    // Make sure Matrix3x2 has the expected size and padding.
    static_assert(sizeof(Matrix3x2) == 24, "wrong sizeof(Matrix3x2)");
    static_assert(sizeof(Matrix3x2_2x) == 48, "wrong sizeof(Matrix3x2_2x)");
    static_assert(sizeof(Matrix3x2PlusFloat) == 28, "wrong sizeof(Matrix3x2PlusFloat)");
    static_assert(sizeof(Matrix3x2PlusFloat_2x) == 56, "wrong sizeof(Matrix3x2PlusFloat_2x)");

    // Make sure Matrix3x2 has the expected field offsets.
    static_assert(offsetof(Matrix3x2, M11) == 0, "wrong offsetof(Matrix3x2, M11)");
    static_assert(offsetof(Matrix3x2, M12) == 4, "wrong offsetof(Matrix3x2, M12)");

    static_assert(offsetof(Matrix3x2, M21) == 8, "wrong offsetof(Matrix3x2, M21)");
    static_assert(offsetof(Matrix3x2, M22) == 12, "wrong offsetof(Matrix3x2, M22)");

    static_assert(offsetof(Matrix3x2, M31) == 16, "wrong offsetof(Matrix3x2, M31)");
    static_assert(offsetof(Matrix3x2, M32) == 20, "wrong offsetof(Matrix3x2, M32)");


    // Embed Matrix4x4 in other structures to make sure there's no unexpected padding.
    struct Matrix4x4_2x
    {
        Matrix4x4 a;
        Matrix4x4 b;
    };

    struct Matrix4x4PlusFloat
    {
        Matrix4x4 v;
        float f;
    };

    struct Matrix4x4PlusFloat_2x
    {
        Matrix4x4PlusFloat a;
        Matrix4x4PlusFloat b;
    };

    // Make sure Matrix4x4 has the expected size and padding.
    static_assert(sizeof(Matrix4x4) == 64, "wrong sizeof(Matrix4x4)");
    static_assert(sizeof(Matrix4x4_2x) == 128, "wrong sizeof(Matrix4x4_2x)");
    static_assert(sizeof(Matrix4x4PlusFloat) == 68, "wrong sizeof(Matrix4x4PlusFloat)");
    static_assert(sizeof(Matrix4x4PlusFloat_2x) == 136, "wrong sizeof(Matrix4x4PlusFloat_2x)");

    // Make sure Matrix4x4 has the expected field offsets.
    static_assert(offsetof(Matrix4x4, M11) == 0, "wrong offsetof(Matrix4x4, M11)");
    static_assert(offsetof(Matrix4x4, M12) == 4, "wrong offsetof(Matrix4x4, M12)");
    static_assert(offsetof(Matrix4x4, M13) == 8, "wrong offsetof(Matrix4x4, M13)");
    static_assert(offsetof(Matrix4x4, M14) == 12, "wrong offsetof(Matrix4x4, M14)");

    static_assert(offsetof(Matrix4x4, M21) == 16, "wrong offsetof(Matrix4x4, M21)");
    static_assert(offsetof(Matrix4x4, M22) == 20, "wrong offsetof(Matrix4x4, M22)");
    static_assert(offsetof(Matrix4x4, M23) == 24, "wrong offsetof(Matrix4x4, M23)");
    static_assert(offsetof(Matrix4x4, M24) == 28, "wrong offsetof(Matrix4x4, M24)");

    static_assert(offsetof(Matrix4x4, M31) == 32, "wrong offsetof(Matrix4x4, M31)");
    static_assert(offsetof(Matrix4x4, M32) == 36, "wrong offsetof(Matrix4x4, M32)");
    static_assert(offsetof(Matrix4x4, M33) == 40, "wrong offsetof(Matrix4x4, M33)");
    static_assert(offsetof(Matrix4x4, M34) == 44, "wrong offsetof(Matrix4x4, M34)");

    static_assert(offsetof(Matrix4x4, M41) == 48, "wrong offsetof(Matrix4x4, M41)");
    static_assert(offsetof(Matrix4x4, M42) == 52, "wrong offsetof(Matrix4x4, M42)");
    static_assert(offsetof(Matrix4x4, M43) == 56, "wrong offsetof(Matrix4x4, M43)");
    static_assert(offsetof(Matrix4x4, M44) == 60, "wrong offsetof(Matrix4x4, M44)");


    // Embed Plane in other structures to make sure there's no unexpected padding.
    struct Plane_2x
    {
        Plane a;
        Plane b;
    };

    struct PlanePlusFloat
    {
        Plane v;
        float f;
    };

    struct PlanePlusFloat_2x
    {
        PlanePlusFloat a;
        PlanePlusFloat b;
    };
    
    // Make sure Plane has the expected size and padding.
    static_assert(sizeof(Plane) == 16, "wrong sizeof(Plane)");
    static_assert(sizeof(Plane_2x) == 32, "wrong sizeof(Plane_2x)");
    static_assert(sizeof(PlanePlusFloat) == 20, "wrong sizeof(PlanePlusFloat)");
    static_assert(sizeof(PlanePlusFloat_2x) == 40, "wrong sizeof(PlanePlusFloat_2x)");

    // Make sure Plane has the expected field offsets.
    static_assert(offsetof(Plane, Normal) == 0, "wrong offsetof(Plane, Normal)");
    static_assert(offsetof(Plane, D) == 12, "wrong offsetof(Plane, D)");


    // Embed Quaternion in other structures to make sure there's no unexpected padding.
    struct Quaternion_2x
    {
        Quaternion a;
        Quaternion b;
    };

    struct QuaternionPlusFloat
    {
        Quaternion v;
        float f;
    };

    struct QuaternionPlusFloat_2x
    {
        QuaternionPlusFloat a;
        QuaternionPlusFloat b;
    };
    
    // Make sure Quaternion has the expected size and padding.
    static_assert(sizeof(Quaternion) == 16, "wrong sizeof(Quaternion)");
    static_assert(sizeof(Quaternion_2x) == 32, "wrong sizeof(Quaternion_2x)");
    static_assert(sizeof(QuaternionPlusFloat) == 20, "wrong sizeof(QuaternionPlusFloat)");
    static_assert(sizeof(QuaternionPlusFloat_2x) == 40, "wrong sizeof(QuaternionPlusFloat_2x)");

    // Make sure Quaternion has the expected field offsets.
    static_assert(offsetof(Quaternion, X) == 0, "wrong offsetof(Quaternion, X)");
    static_assert(offsetof(Quaternion, Y) == 4, "wrong offsetof(Quaternion, Y)");
    static_assert(offsetof(Quaternion, Z) == 8, "wrong offsetof(Quaternion, Z)");
    static_assert(offsetof(Quaternion, W) == 12, "wrong offsetof(Quaternion, W)");
}

#endif