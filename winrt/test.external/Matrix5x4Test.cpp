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

using namespace Microsoft::Graphics::Canvas::Effects;

// Because this WinRT type is just a structure without any methods, there is no actual 
// executable code to validate here. But we do want to make sure the structure size and 
// layout is what we expect. Structure layout can be statically validated at compile 
// time, so there is no need to actually execute this test: if it compiles, it passed.

namespace Matrix5x4Test
{
    // Embed Matrix5x4 in other structures to make sure there's no unexpected padding.
    struct Matrix5x4_2x
    {
        Matrix5x4 a;
        Matrix5x4 b;
    };

    struct Matrix5x4PlusFloat
    {
        Matrix5x4 v;
        float f;
    };

    struct Matrix5x4PlusFloat_2x
    {
        Matrix5x4PlusFloat a;
        Matrix5x4PlusFloat b;
    };

    // Make sure Matrix5x4 has the expected size and padding.
    static_assert(sizeof(Matrix5x4) == 80, "wrong sizeof(Matrix5x4)");
    static_assert(sizeof(Matrix5x4_2x) == 160, "wrong sizeof(Matrix5x4_2x)");
    static_assert(sizeof(Matrix5x4PlusFloat) == 84, "wrong sizeof(Matrix5x4PlusFloat)");
    static_assert(sizeof(Matrix5x4PlusFloat_2x) == 168, "wrong sizeof(Matrix5x4PlusFloat_2x)");

    // Make sure Matrix5x4 has the expected field offsets.
    static_assert(offsetof(Matrix5x4, M11) == 0, "wrong offsetof(Matrix5x4, M11)");
    static_assert(offsetof(Matrix5x4, M12) == 4, "wrong offsetof(Matrix5x4, M12)");
    static_assert(offsetof(Matrix5x4, M13) == 8, "wrong offsetof(Matrix5x4, M13)");
    static_assert(offsetof(Matrix5x4, M14) == 12, "wrong offsetof(Matrix5x4, M14)");

    static_assert(offsetof(Matrix5x4, M21) == 16, "wrong offsetof(Matrix5x4, M21)");
    static_assert(offsetof(Matrix5x4, M22) == 20, "wrong offsetof(Matrix5x4, M22)");
    static_assert(offsetof(Matrix5x4, M23) == 24, "wrong offsetof(Matrix5x4, M23)");
    static_assert(offsetof(Matrix5x4, M24) == 28, "wrong offsetof(Matrix5x4, M24)");

    static_assert(offsetof(Matrix5x4, M31) == 32, "wrong offsetof(Matrix5x4, M31)");
    static_assert(offsetof(Matrix5x4, M32) == 36, "wrong offsetof(Matrix5x4, M32)");
    static_assert(offsetof(Matrix5x4, M33) == 40, "wrong offsetof(Matrix5x4, M33)");
    static_assert(offsetof(Matrix5x4, M34) == 44, "wrong offsetof(Matrix5x4, M34)");

    static_assert(offsetof(Matrix5x4, M41) == 48, "wrong offsetof(Matrix5x4, M41)");
    static_assert(offsetof(Matrix5x4, M42) == 52, "wrong offsetof(Matrix5x4, M42)");
    static_assert(offsetof(Matrix5x4, M43) == 56, "wrong offsetof(Matrix5x4, M43)");
    static_assert(offsetof(Matrix5x4, M44) == 60, "wrong offsetof(Matrix5x4, M44)");

    static_assert(offsetof(Matrix5x4, M51) == 64, "wrong offsetof(Matrix5x4, M51)");
    static_assert(offsetof(Matrix5x4, M52) == 68, "wrong offsetof(Matrix5x4, M52)");
    static_assert(offsetof(Matrix5x4, M53) == 72, "wrong offsetof(Matrix5x4, M53)");
    static_assert(offsetof(Matrix5x4, M54) == 76, "wrong offsetof(Matrix5x4, M54)");
}
