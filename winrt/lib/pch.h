//
// pch.h
// Header for standard system include files.
//

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

// Win32
#include <wrl.h>
#include <strsafe.h>
#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
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
#include <CanvasImageSource.abi.h>
#include <WinRTDirect3D11.h>
#include <WinRTDirectXCommon.h>
#include <WinRTNumerics.h>

// local
#include "ClosablePtr.h"
#include "Conversion.h"
