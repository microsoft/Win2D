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


float valueTheOptimizerCannotRemove = 0;


// Measures operations that are common to all the vector, matrix and quaternion types.
template<typename T>
void RunCommonArithmeticTests(std::string const& typeName)
{
    RunPerfTest<T, T>(typeName + " operator+", [](T* value, T const& param)
    {
        *value += param;
    });

    RunPerfTest<T, T>(typeName + " operator-", [](T* value, T const& param)
    {
        *value -= param;
    });

    RunPerfTest<T, T>(typeName + " operator* (" + typeName + ")", [](T* value, T const& param)
    {
        *value *= param;
    });

    RunPerfTest<T, float>(typeName + " operator* (scalar)", [](T* value, float param)
    {
        *value *= param;
    });

    RunPerfTest<T, T>(typeName + " operator- (unary)", [](T* value, T& param)
    {
        auto t = param;
        param = *value;
        *value = -t;
    });

    RunPerfTest<T, T>(typeName + " lerp", [](T* value, T const& param)
    {
        *value = lerp(*value, param, 0.5f);
    });
}


// Measures operations that are common to all the vector and quaternion types.
template<typename T>
void RunCommonVectorOrQuaternionTests(std::string const& typeName)
{
    RunCommonArithmeticTests<T>(typeName);

    RunPerfTest<T, T>(typeName + " operator/", [](T* value, T const& param)
    {
        *value /= param;
    });

    RunPerfTest<float, T>(typeName + " length", [](float* value, T const& param)
    {
        *value += length(param);
    });

    RunPerfTest<float, T>(typeName + " length_squared", [](float* value, T const& param)
    {
        *value += length_squared(param);
    });

    RunPerfTest<T, T>(typeName + " dot", [](T* value, T const& param)
    {
        value->x = dot(*value, param);
    });

    RunPerfTest<T, T>(typeName + " normalize", [](T* value, T& param)
    {
        auto t = param;
        param = *value;
        *value = normalize(t);
    });
}


// Measures operations that are common to all the vector types.
template<typename T>
void RunCommonVectorTests(std::string const& typeName)
{
    RunCommonVectorOrQuaternionTests<T>(typeName);

    RunPerfTest<T, float>(typeName + " operator* (scalar lhs)", [](T* value, float param)
    {
        *value = param * *value;
    });

    RunPerfTest<T, float>(typeName + " operator/ (scalar)", [](T* value, float param)
    {
        *value /= param;
    });

    RunPerfTest<T, T>(typeName + " distance", [](T* value, T const& param)
    {
        value->x = distance(*value, param);
    });

    RunPerfTest<T, T>(typeName + " distance_squared", [](T* value, T const& param)
    {
        value->x = distance_squared(*value, param);
    });

    RunPerfTest<T, T>(typeName + " min", [](T* value, T const& param)
    {
        *value = min(*value, param);
    });

    RunPerfTest<T, T>(typeName + " max", [](T* value, T const& param)
    {
        *value = max(*value, param);
    });

    RunPerfTest<T, T>(typeName + " clamp", [](T* value, T const& param)
    {
        *value = clamp(*value, T::zero(), param);
    });

    RunPerfTest<T, float4x4>(typeName + " transform (float4x4)", [](T* value, float4x4 const& param)
    {
        *value = transform(*value, param);
    });

    RunPerfTest<T, quaternion>(typeName + " transform (quaternion)", [](T* value, quaternion const& param)
    {
        *value = transform(*value, param);
    });
}


// Measures operations that are common to float2 and float3 (but not float4).
template<typename T>
void RunFloat2Or3Tests(std::string const& typeName)
{
    RunCommonVectorTests<T>(typeName);

    RunPerfTest<T, T>(typeName + " reflect", [](T* value, T const& param)
    {
        *value = reflect(*value, param);
    });

    RunPerfTest<T, float4x4>(typeName + " transform_normal (float4x4)", [](T* value, float4x4 const& param)
    {
        *value = transform_normal(*value, param);
    });

    RunPerfTest<T, float4x4>(typeName + " transform4 (float4x4)", [](T* value, float4x4 const& param)
    {
        float4 t = transform4(*value, param);
        value->x = t.x + t.y + t.z + t.w;
    });

    RunPerfTest<T, quaternion>(typeName + " transform4 (quaternion)", [](T* value, quaternion const& param)
    {
        float4 t = transform4(*value, param);
        value->x = t.x + t.y + t.z + t.w;
    });
}


void RunFloat2Tests()
{
    RunFloat2Or3Tests<float2>("float2");

    RunPerfTest<float2, float3x2>("float2 transform (float3x2)", [](float2* value, float3x2 const& param)
    {
        *value = transform(*value, param);
    });

    RunPerfTest<float2, float3x2>("float2 transform_normal (float3x2)", [](float2* value, float3x2 const& param)
    {
        *value = transform_normal(*value, param);
    });
}


void RunFloat3Tests()
{
    RunFloat2Or3Tests<float3>("float3");

    RunPerfTest<float3, float3>("float3 cross", [](float3* value, float3 const& param)
    {
        *value = cross(*value, param);
    });
}


void RunFloat4Tests()
{
    RunCommonVectorTests<float4>("float4");
}


void RunFloat3x2Tests()
{
    RunCommonArithmeticTests<float3x2>("float3x2");
}


void RunFloat4x4Tests()
{
    RunCommonArithmeticTests<float4x4>("float4x4");

    RunPerfTest<float4x4, quaternion>("float4x4 transform(quaternion)", [](float4x4* value, quaternion const& param)
    {
        *value = transform(*value, param);
    });

    RunPerfTest<float4x4, float4x4>("float4x4 transpose", [](float4x4* value, float4x4& param)
    {
        auto t = param;
        param = *value;
        *value = transpose(t);
    });
}


void RunPlaneTests()
{
    RunPerfTest<plane, plane>("plane normalize", [](plane* value, plane& param)
    {
        auto t = param;
        param = *value;
        *value = normalize(t);
    });

    RunPerfTest<plane, float4x4>("plane transform (float4x4)", [](plane* value, float4x4 const& param)
    {
        *value = transform(*value, param);
    });

    RunPerfTest<plane, quaternion>("plane transform (quaternion)", [](plane* value, quaternion const& param)
    {
        *value = transform(*value, param);
    });

    RunPerfTest<plane, float4>("plane dot", [](plane* value, float4 const& param)
    {
        value->d = dot(*value, param);
    });

    RunPerfTest<plane, float3>("plane dot_coordinate", [](plane* value, float3 const& param)
    {
        value->d = dot_coordinate(*value, param);
    });

    RunPerfTest<plane, float3>("plane dot_normal", [](plane* value, float3 const& param)
    {
        value->d = dot_normal(*value, param);
    });
}


void RunQuaternionTests()
{
    RunCommonVectorOrQuaternionTests<quaternion>("quaternion");

    RunPerfTest<quaternion, quaternion>("quaternion conjugate", [](quaternion* value, quaternion& param)
    {
        auto t = param;
        param = *value;
        *value = conjugate(t);
    });

    RunPerfTest<quaternion, quaternion>("quaternion inverse", [](quaternion* value, quaternion& param)
    {
        auto t = param;
        param = *value;
        *value = inverse(t);
    });

    RunPerfTest<quaternion, quaternion>("quaternion slerp", [](quaternion* value, quaternion const& param)
    {
        *value = slerp(*value, param, 0.5f);
    });

    RunPerfTest<quaternion, quaternion>("quaternion concatenate", [](quaternion* value, quaternion const& param)
    {
        *value = concatenate(*value, param);
    });
}


int __cdecl main()
{
    printf("name, time, deviation\n");

    RunFloat2Tests();
    RunFloat3Tests();
    RunFloat4Tests();
    RunFloat3x2Tests();
    RunFloat4x4Tests();
    RunPlaneTests();
    RunQuaternionTests();

    printf("\nEnsureNotOptimizedAway: %f\n", valueTheOptimizerCannotRemove);

    return 0;
}
