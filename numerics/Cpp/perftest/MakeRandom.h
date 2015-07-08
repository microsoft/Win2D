// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once


// Generates an array of random values.
template<typename T, size_t Count>
std::array<T, Count> MakeRandom()
{
    std::array<T, Count> values;

    std::generate(values.begin(), values.end(), MakeRandom<T>);

    return values;
}


// Generates a single random value.
template<typename T>
T MakeRandom()
{
    static_assert(false, "Unknown MakeRandom type.");
}


template<>
inline float MakeRandom<float>()
{
    return static_cast<float>(rand()) / RAND_MAX;
}


template<>
inline float2 MakeRandom<float2>()
{
    auto v = MakeRandom<float, 2>();

    return float2(v[0], v[1]);
}


template<>
inline float3 MakeRandom<float3>()
{
    auto v = MakeRandom<float, 3>();

    return float3(v[0], v[1], v[2]);
}


template<>
inline float4 MakeRandom<float4>()
{
    auto v = MakeRandom<float, 4>();

    return float4(v[0], v[1], v[2], v[3]);
}


template<>
inline float3x2 MakeRandom<float3x2>()
{
    auto v = MakeRandom<float, 6>();

    return float3x2(v[0], v[1],
                    v[2], v[3],
                    v[4], v[5]);
}


template<>
inline float4x4 MakeRandom<float4x4>()
{
    auto v = MakeRandom<float, 16>();

    return float4x4(v[0],  v[1],  v[2],  v[3],
                    v[4],  v[5],  v[6],  v[7],
                    v[8],  v[9],  v[10], v[11],
                    v[12], v[13], v[14], v[15]);
}


template<>
inline plane MakeRandom<plane>()
{
    auto v = MakeRandom<float, 4>();

    return plane(v[0], v[1], v[2], v[3]);
}


template<>
inline quaternion MakeRandom<quaternion>()
{
    auto v = MakeRandom<float, 4>();

    return quaternion(v[0], v[1], v[2], v[3]);
}
