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
#include <dwrite_2.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <strsafe.h>
#include <wincodec.h>
#include <winstringwrapper.h>

// WinRT
#include <windows.ui.xaml.media.dxinterop.h>
#include <windows.graphics.display.h>

// UnitTest
#include <CppUnitTest.h>

// Headers generated from IDL files
#include <Canvas.abi.h>

// Inc
#include <ErrorHandling.h>
#include <Constants.h>
#include <Vector.h>
#include <WinStringWrapper.h>

// Public
#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

// winrt.lib
#include <CanvasDevice.h>
#include <CanvasDrawingSession.h>
#include <CanvasImageSource.h>
#include <CanvasImageSourceDrawingSessionAdapter.h>
#include <CanvasBrush.h>
#include <CanvasStrokeStyle.h>
#include <CanvasControl.h>
#include <Conversion.h>
#include <CanvasTextFormat.h>
#include <ResourceManager.h>
#include <ResourceTracker.h>
#include <ResourceWrapper.h>


// local headers
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

namespace canvas {}
using namespace canvas;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Effects;

#include "Helpers.h"
#include "MockCanvasDevice.h"
#include "MockCanvasDrawingSession.h"
#include "MockCanvasImageSourceDrawingSessionFactory.h"
#include "MockCoreApplication.h"
#include "MockD2DDevice.h"
#include "MockD2DDeviceContext.h"
#include "MockD2DFactory.h"
#include "MockD2DSolidColorBrush.h"
#include "MockD2DStrokeStyle.h"
#include "MockD3D11Device.h"
#include "MockSurfaceImageSource.h"
#include "MockSurfaceImageSourceFactory.h"
#include "StubCanvasBrush.h"
#include "StubCanvasDevice.h"
#include "StubCanvasDrawingSessionAdapter.h"
#include "StubD2DBrush.h"
#include "StubD2DResources.h"
#include "StubD2DStrokeStyle.h"
#include "StubImageControl.h"
#include "StubSurfaceImageSource.h"
#include "StubSurfaceImageSourceFactory.h"
#include "StubUserControl.h"
#include "CanvasControlTestAdapter.h"
