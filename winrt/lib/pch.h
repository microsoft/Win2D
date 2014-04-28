//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <sdkddkver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>
#include <wrl.h>
#include <strsafe.h>
#include <map>

#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>

#include <windows.foundation.h>
#include <windows.ui.h>
#include <windows.ui.xaml.media.dxinterop.h>

