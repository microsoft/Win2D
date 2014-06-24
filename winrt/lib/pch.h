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
#include <wrl.h>
#include <strsafe.h>
#include <assert.h>

#include <algorithm>
#include <memory>
#include <map>
#include <cstdint>
#include <mutex>

#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_3.h>
#include <wincodec.h>

#include <windows.foundation.h>
#include <windows.ui.h>
#include <windows.ui.xaml.media.dxinterop.h>


#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

#include <CanvasImageSource.abi.h>
#include <WinRTDirect3D11.h>
#include <WinRTDirectXCommon.h>
#include <WinRTNumerics.h>

#include <Conversion.h>
#include <ErrorHandling.h>

#include "ClosablePtr.h"
