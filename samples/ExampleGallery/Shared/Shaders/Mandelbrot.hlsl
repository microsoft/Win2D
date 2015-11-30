// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.


// This shader has no input textures.
// It generates a mandelbrot fractal.

#define D2D_INPUT_COUNT 0
#define D2D_REQUIRES_SCENE_POSITION

#include "d2d1effecthelpers.hlsli"


float scale;
float2 translate;

static const float4 tapOffsetsX = float4(-0.25,  0.25, -0.25, 0.25);
static const float4 tapOffsetsY = float4(-0.25, -0.25,  0.25, 0.25);

static const int iterations = 100;


D2D_PS_ENTRY(main)
{
    float2 pos = D2DGetScenePosition().xy;

    // Improve visual quality by supersampling inside the pixel shader, evaluating four separate
    // versions of the fractal in parallel, each at a slightly different position offset.
    // The x, y, z, and w components of these float4s contain the four simultaneous computations.
    float4 c_r = (pos.x + tapOffsetsX) * scale + translate.x;
    float4 c_i = (pos.y + tapOffsetsY) * scale + translate.y;

    float4 value_r = 0;
    float4 value_i = 0;

    // Evalulate the Mandelbrot fractal.
    for (int i = 0; i < iterations; i++)
    {
        float4 new_r = value_r * value_r - value_i * value_i + c_r;
        float4 new_i = value_r * value_i * 2 + c_i;

        value_r = new_r;
        value_i = new_i;
    }

    // Adjust our four parallel results to range 0:1.
    float4 distanceSquared = value_r * value_r + value_i * value_i;

    float4 vectorResult = isfinite(distanceSquared) ? saturate(1 - distanceSquared) : 0;

    // Resolve the supersampling to produce a single scalar result.
    float result = dot(vectorResult, 0.25);

    if (result < 1.0 / 256)
        return 0;
    else
        return float4(result, result, result, 1);
}
