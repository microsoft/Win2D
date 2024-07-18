// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <sdkddkver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif

#ifndef NOMINMAX
#define NOMINMAX                // Stop Windows from defining min() and max() macros that break STL
#endif

#ifndef WIN2D_DLL_EXPORT
#define WIN2D_DLL_EXPORT		// Mark public C APIs as being exported (whereas external consumers will import them)
#endif

#if defined(_M_IX86) && defined(_MSC_VER)
#ifndef ARCH_X86
#define ARCH_X86				// Used to detect the x86 architecture so fixups for C exports can be added
#endif
#endif

#include <windows.h>

// Undef GetCurrentTime because the Win32 API in windows.h collides with Storyboard.GetCurrentTime
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

// Standard C++
#include <algorithm>
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Win32
#include <initguid.h>
#include <wrl.h>
#include <wrl\async.h>
#include <strsafe.h>
#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite_2.h>
#include <dxgi1_3.h>
#include <d2d1effectauthor.h>  
#include <d2d1effecthelpers.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wincodec.h>
#include <shcore.h>
#include <robuffer.h>

#ifndef WINDOWS_PHONE
#include <DocumentSource.h>
#include <PrintPreview.h>
#endif

#if WINVER > _WIN32_WINNT_WINBLUE
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <inkrenderer.h>
#include <MemoryBuffer.h>
#endif

// WinRT
#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.security.cryptography.h>
#include <windows.security.cryptography.core.h>
#include <windows.storage.h>

#ifndef WINUI3
#include <microsoft.graphics.h>
#include <microsoft.system.h>
#else
#include <microsoft.ui.h>
#include <microsoft.ui.dispatching.h>
#include <microsoft.UI.Xaml.Controls.h>
#include <microsoft.ui.xaml.media.h>
#include <microsoft.ui.xaml.media.dxinterop.h>
#include <microsoft.ui.xaml.shapes.h>
#endif

#include <windows.graphics.display.h>
#include <windows.graphics.interop.h>

#include <Microsoft.UI.Interop.h>

#if WINVER > _WIN32_WINNT_WINBLUE
#include <windows.foundation.metadata.h>
#include <microsoft.ui.composition.h>
#include <microsoft.ui.composition.interop.h>
#endif

#pragma warning(default: 4265)  // "class has virtual functions, but destructor is not virtual"

// Public
#include <Microsoft.Graphics.Canvas.native.h>

// Generated from local IDLs
#include <Microsoft.Graphics.Canvas.h>

// Inc
#include <AsyncOperation.h>
#include <CanvasUtilities.h>
#include <ClosablePtr.h>
#include <ComArray.h>
#include <Constants.h>
#include <ErrorHandling.h>
#include <LifespanTracker.h>
#include <Map.h>
#include <MicrosoftTelemetry.h>
#include <Nullable.h>
#include <ReferenceArray.h>
#include <RegisteredEvent.h>
#include <ScopeWarden.h>
#include <Singleton.h>
#include <Utilities.h>
#include <Vector.h>
#include <Win2DTelemetry.h>
#include <WinStringWrapper.h>
#include <WinStringBuilder.h>

#pragma warning(push)
#pragma warning(disable:4459)   // declaration hides global declaration
#include <win2d.etw.h>
#pragma warning(pop)

// Pick up the inbox or local WinRT DirectX types as appropriate
#include "UapApis.h"

// local
#include "utils/Conversion.h"
#include "utils/DxgiUtilities.h"
#include "utils/MathUtilities.h"
#include "utils/ResourceManager.h"
#include "utils/ResourceWrapper.h"
#include "utils/CachedResourceReference.h"
#include "utils/Strings.h"
#include "images/CanvasImage.h"
#include "images/CanvasBitmap.h"
#include "images/CanvasRenderTarget.h"
#include "effects/CanvasEffect.h"
#include "brushes/CanvasBrush.h"
#include "brushes/CanvasImageBrush.h"
#include "drawing/CanvasDevice.h"
#include "drawing/CanvasGradientMesh.h"
#include "drawing/CanvasDrawingSession.h"
#include "drawing/CanvasStrokeStyle.h"
#include "drawing/CanvasSwapChain.h"
#include "geometry/CanvasGeometry.h"
#include "text/CanvasTextFormat.h"
#include "xaml/RecreatableDeviceManager.h"
#include "xaml/CanvasAnimatedControl.h"
#include "xaml/CanvasImageSource.h"
#include "xaml/CanvasImageSourceDrawingSessionAdapter.h"
