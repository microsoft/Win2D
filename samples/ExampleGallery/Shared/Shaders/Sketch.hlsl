// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.


// This shader has two input textures:
//
//  - First is the image that will be processed by the sketch effect.
//
//  - Second is an overlay containing a pencil sketch texture. This combines
//    three different patterns of strokes in the red, green and blue channels.

#define D2D_INPUT_COUNT 2
#define D2D_INPUT0_COMPLEX
#define D2D_INPUT1_COMPLEX

#include "d2d1effecthelpers.hlsli"


// Is the sketch pattern enabled?
bool SketchEnabled = true;

// How should the sketch effect respond to changes of brightness in the input color?
float SketchThreshold = 0.2;
float SketchBrightness = 0.35;
float SketchColorLevel = 3;

// Settings controlling the edge detection filter.
float EdgeThreshold = 1;
float EdgeIntensity = 0.5;
float EdgeOffset = 1;

// Randomly offsets the sketch overlay pattern to create a hand-drawn animation effect.
float JitterX;
float JitterY;


D2D_PS_ENTRY(main)
{
    // Look up the original color from the source image.
    float3 color = D2DGetInput(0).rgb;

    // Apply the sketch effect?
    if (SketchEnabled)
    {
        // Adjust the color to remove very dark values and increase the contrast.
        float3 saturatedColor = saturate((color - SketchThreshold) * 2);

        // Look up into the sketch pattern overlay texture.
        float3 sketchPattern = D2DSampleInputAtOffset(1, float2(JitterX, JitterY)).rgb;

        // Convert into negative color space, and combine our color with the
        // sketch pattern. We need to do this multiply in negative space to get good
        // looking results, because pencil sketching works by drawing black ink
        // over an initially white page, rather than adding light to an initially
        // black background as would be more common in computer graphics.
        float3 negativeSketch = (1 - saturatedColor) * (1 - sketchPattern);

        // Convert the result into a positive color space greyscale value.
        float sketchResult = dot(1 - negativeSketch, SketchBrightness);

        // Combine the sketch result with the original color.
        color = saturate(color * SketchColorLevel) * sketchResult;
    }

    // Apply the edge detection filter?
    if (EdgeIntensity > 0)
    {
        // Look up four values from the source image, offset along the
        // four diagonals from the pixel we are currently shading.
        float4 edge1 = D2DSampleInputAtOffset(0, float2(-1, -1) * EdgeOffset);
        float4 edge2 = D2DSampleInputAtOffset(0, float2( 1,  1) * EdgeOffset);
        float4 edge3 = D2DSampleInputAtOffset(0, float2(-1,  1) * EdgeOffset);
        float4 edge4 = D2DSampleInputAtOffset(0, float2( 1, -1) * EdgeOffset);

        // Work out how much the values are changing.
        float4 diagonalDelta = abs(edge1 - edge2) + abs(edge3 - edge4);

        // Filter out very small changes, in order to produce nice clean results.
        float delta = saturate((dot(diagonalDelta, 1) - EdgeThreshold) * EdgeIntensity);

        // Apply edge detection result to the main color.
        color *= (1 - delta);
    }

    return float4(color, 1);
}
