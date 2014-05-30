// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Standard C++
#include <functional>
#include <memory>

// Win32
#include <d2d1_2.h>
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

// winrt.lib
#include <CanvasDevice.h>
#include <CanvasDrawingSession.h>
#include <CanvasImageSource.h>
#include <CanvasImageSourceDrawingSessionAdapter.h>
#include <Conversion.h>
#include <ErrorHandling.h>

// local headers
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;
using namespace canvas;

#include "Helpers.h"
#include "MockCanvasDevice.h"
#include "MockCanvasDrawingSession.h"
#include "MockCanvasImageSourceDrawingSessionFactory.h"
#include "MockD2DDevice.h"
#include "MockD2DDeviceContext.h"
#include "MockSurfaceImageSourceFactory.h"
#include "MockSurfaceImageSource.h"
#include "StubCanvasDevice.h"
#include "StubCanvasDrawingSessionAdapter.h"
#include "StubSurfaceImageSourceFactory.h"
#include "StubSurfaceImageSource.h"
