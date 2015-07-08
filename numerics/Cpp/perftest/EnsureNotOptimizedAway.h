// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once


// The optimizer is wont to remove calculations if their result is eventually discarded.
// To stop it deleting the code we are trying to measure, each test is written in such a
// way that they iteratively mutate a single value. At the end of the test, all fields of
// that value are added to this global, which is printed out at the end of the run.
// Thus the work is not discarded and cannot be optimized out.
//
// A sufficiently smart optimizer could still lift the entire calculation out to compile
// time, generating code that just prints a constant result. Luckily for us, the actual
// compiler is not quite that smart :-)

extern float valueTheOptimizerCannotRemove;


inline void EnsureNotOptimizedAway(float value)
{
    valueTheOptimizerCannotRemove += value;
}


inline void EnsureNotOptimizedAway(float2 const& value)
{
    EnsureNotOptimizedAway(value.x);
    EnsureNotOptimizedAway(value.y);
}


inline void EnsureNotOptimizedAway(float3 const& value)
{
    EnsureNotOptimizedAway(value.x);
    EnsureNotOptimizedAway(value.y);
    EnsureNotOptimizedAway(value.z);
}


inline void EnsureNotOptimizedAway(float4 const& value)
{
    EnsureNotOptimizedAway(value.x);
    EnsureNotOptimizedAway(value.y);
    EnsureNotOptimizedAway(value.z);
    EnsureNotOptimizedAway(value.w);
}


inline void EnsureNotOptimizedAway(float3x2 const& value)
{
    EnsureNotOptimizedAway(value.m11);
    EnsureNotOptimizedAway(value.m12);

    EnsureNotOptimizedAway(value.m21);
    EnsureNotOptimizedAway(value.m22);

    EnsureNotOptimizedAway(value.m31);
    EnsureNotOptimizedAway(value.m32);
}


inline void EnsureNotOptimizedAway(float4x4 const& value)
{
    EnsureNotOptimizedAway(value.m11);
    EnsureNotOptimizedAway(value.m12);
    EnsureNotOptimizedAway(value.m13);
    EnsureNotOptimizedAway(value.m14);

    EnsureNotOptimizedAway(value.m21);
    EnsureNotOptimizedAway(value.m22);
    EnsureNotOptimizedAway(value.m23);
    EnsureNotOptimizedAway(value.m24);

    EnsureNotOptimizedAway(value.m31);
    EnsureNotOptimizedAway(value.m32);
    EnsureNotOptimizedAway(value.m33);
    EnsureNotOptimizedAway(value.m34);

    EnsureNotOptimizedAway(value.m41);
    EnsureNotOptimizedAway(value.m42);
    EnsureNotOptimizedAway(value.m43);
    EnsureNotOptimizedAway(value.m44);
}


inline void EnsureNotOptimizedAway(plane const& value)
{
    EnsureNotOptimizedAway(value.normal);
    EnsureNotOptimizedAway(value.d);
}


inline void EnsureNotOptimizedAway(quaternion const& value)
{
    EnsureNotOptimizedAway(value.x);
    EnsureNotOptimizedAway(value.y);
    EnsureNotOptimizedAway(value.z);
    EnsureNotOptimizedAway(value.w);
}
