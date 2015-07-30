// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <memory>
#include <wrl.h>
#include <WindowsNumerics.h>

// DirectX headers.
#include <d2d1_2.h>
#include <DirectXMath.h>

// DirectX Tool Kit headers.
#include <GeometricPrimitive.h>
#include <Effects.h>

// Win2D headers.
#include <Microsoft.Graphics.Canvas.native.h>

#if WINVER <= _WIN32_WINNT_WINBLUE
#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#else
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#endif
