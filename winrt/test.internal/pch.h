// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Standard C++
#include <assert.h>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

// Win32
#include <d2d1_2.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <strsafe.h>

// WinRT
#include <windows.ui.xaml.media.dxinterop.h>

// UnitTest
#include <CppUnitTest.h>

// Headers generated from IDL files
#include <CanvasDevice.abi.h>
#include <CanvasDrawingSession.abi.h>
#include <CanvasImageSource.abi.h>

// Inc
#include <ErrorHandling.h>

// Public
#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

// winrt.lib
#include <CanvasDevice.h>
#include <CanvasDrawingSession.h>
#include <CanvasImageSource.h>
#include <CanvasImageSourceDrawingSessionAdapter.h>
#include <CanvasBrush.h>
#include <Conversion.h>
#include <ResourceManager.h>
#include <ResourceTracker.h>
#include <ResourceWrapper.h>


// local headers
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace canvas;

#include "Helpers.h"
#include "MockCanvasDevice.h"
#include "MockCanvasDrawingSession.h"
#include "MockCanvasImageSourceDrawingSessionFactory.h"
#include "MockCoreApplication.h"
#include "MockD2DDevice.h"
#include "MockD2DDeviceContext.h"
#include "MockD2DFactory.h"
#include "MockD2DSolidColorBrush.h"
#include "MockD3D11Device.h"
#include "MockSurfaceImageSource.h"
#include "MockSurfaceImageSourceFactory.h"
#include "StubCanvasBrush.h"
#include "StubCanvasDevice.h"
#include "StubCanvasDrawingSessionAdapter.h"
#include "StubD2DBrush.h"
#include "StubSurfaceImageSource.h"
#include "StubSurfaceImageSourceFactory.h"
