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
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif

#ifndef NOMINMAX
#define NOMINMAX                // Stop Windows from defining min() and max() macros that break STL
#endif

#include <windows.h>

// Standard C++
#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <type_traits>
#include <vector>

// Win32
#include <initguid.h>
#include <wrl.h>
#include <wrl\async.h>
#include <strsafe.h>

#pragma warning(push)
#pragma warning(disable: 4458)  // TODO: Disable "hides class member" warning until we pick up fix to MS.601961
#include <d2d1_2.h>
#pragma warning(pop)

#include <d3d11.h>
#include <dwrite_2.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <wincodec.h>
#include <shcore.h>

// WinRT
#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.storage.h>
#include <windows.ui.h>
#include <windows.ui.xaml.controls.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include <windows.graphics.display.h>

#pragma warning(default: 4265)  // "class has virtual functions, but destructor is not virtual"

// Public
#include <Microsoft.Graphics.Canvas.Effects.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

// Inc
#include <AsyncOperation.h>
#include <ClosablePtr.h>
#include <ComArray.h>
#include <Constants.h>
#include <ErrorHandling.h>
#include <LifespanTracker.h>
#include <Nullable.h>
#include <RegisteredEvent.h>
#include <ScopeWarden.h>
#include <Utilities.h>
#include <Vector.h>
#include <WinStringWrapper.h>
#include <WinStringBuilder.h>

// Generated from local IDLs
#include <Microsoft.Graphics.Canvas.h>

// Pick up the inbox or local WinRT DirectX types as appropriate
#include "UapApis.h"

// local
#include "utils/Conversion.h"
#include "utils/DxgiUtilities.h"
#include "utils/ResourceManager.h"
#include "utils/Strings.h"
#include "images/CanvasImage.h"
#include "images/CanvasBitmap.h"
#include "images/CanvasRenderTarget.h"
#include "effects/CanvasEffect.h"
#include "brushes/CanvasBrush.h"
#include "brushes/CanvasImageBrush.h"
#include "brushes/Gradients.h"
#include "drawing/CanvasDevice.h"
#include "drawing/CanvasDrawingSession.h"
#include "drawing/CanvasStrokeStyle.h"
#include "drawing/CanvasSwapChain.h"
#include "geometry/CanvasGeometry.h"
#include "text/CanvasTextFormat.h"
#include "xaml/RecreatableDeviceManager.h"
#include "xaml/CanvasAnimatedControl.h"
#include "xaml/CanvasImageSource.h"
#include "xaml/CanvasImageSourceDrawingSessionAdapter.h"
