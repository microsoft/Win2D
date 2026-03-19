// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <Microsoft.Graphics.Canvas.h>

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> GetWrappedResource(U&& wrapper, ABI::Microsoft::Graphics::Canvas::ICanvasDevice* device = nullptr, float dpi = 0)
{
    Microsoft::WRL::ComPtr<T> resource;
    ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(wrapper)->GetNativeResource(device, dpi, IID_PPV_ARGS(&resource)));
    return resource;
}
