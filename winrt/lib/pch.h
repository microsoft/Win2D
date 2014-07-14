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

#pragma once

#include <sdkddkver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#ifndef NOMINMAX
#define NOMINMAX                        // Stop Windows from defining min() and max() macros that break STL
#endif

#include <windows.h>

// Standard C++
#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <iterator>

// Win32
#include <wrl.h>
#include <strsafe.h>
#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite_2.h>
#include <dxgi1_3.h>
#include <wincodec.h>

// WinRT
#include <windows.foundation.h>
#include <windows.ui.h>
#include <windows.ui.xaml.media.dxinterop.h>

// Public
#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

// Inc
#include <ErrorHandling.h>
#include <ScopeWarden.h>

// Generated from local IDLs
#include <Canvas.abi.h>

// local
#include "ClosablePtr.h"
#include "Conversion.h"
