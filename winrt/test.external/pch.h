// pch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <memory>
#include <functional>

#include <wrl.h>
#include <strsafe.h>
#include <CppUnitTest.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

#include <ErrorHandling.h>

#include <WinRTGraphicsInterop.h>

#include <Microsoft.Canvas.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

#include "Helpers.h"
#include "MockDxgiDevice.h"
#include "MockDxgiSurface.h"
