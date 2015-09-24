// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.


// This shader has no input textures.
// It generates an animatable series of concentric circles.

#define D2D_INPUT_COUNT 0
#define D2D_REQUIRES_SCENE_POSITION

#include "d2d1effecthelpers.hlsli"


float2 center;
float frequency = 1;
float offset;
float dpi = 96;


D2D_PS_ENTRY(main)
{
    float2 positionInPixels = D2DGetScenePosition().xy;

    float2 positionInDips = positionInPixels * 96 / dpi;

    float d = distance(center, positionInDips) * frequency + offset;

    float v = sin(d) / 2 + 0.5;

    return float4(v, v, v, 1);
}
