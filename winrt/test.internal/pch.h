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

#pragma warning(disable: 4100)  // "unreferenced formal parameter"
#pragma warning(disable: 4702)  // "unreachable code"

#include "targetver.h"

// Standard C++
#include <assert.h>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>

// Win32
#include <corerror.h>
#include <d2d1_2.h>
#include <d3d11.h>
#include <dwrite_2.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <strsafe.h>
#include <wincodec.h>
#include <winstringwrapper.h>

// WinRT
#include <windows.applicationmodel.core.h>
#include <windows.applicationmodel.h>
#include <windows.graphics.display.h>
#include <windows.ui.xaml.media.dxinterop.h>

// UnitTest
#include <CppUnitTest.h>

// Headers generated from IDL files
#include <Canvas.abi.h>

// Inc
#include <AsyncOperation.h>
#include <ClosablePtr.h>
#include <ComArray.h>
#include <Constants.h>
#include <ErrorHandling.h>
#include <Nullable.h>
#include <ScopeWarden.h>
#include <Utilities.h>
#include <Vector.h>
#include <WinStringBuilder.h>
#include <WinStringWrapper.h>

// Public
#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>

// winrt.lib
#include <CanvasBitmap.h>
#include <CanvasBrush.h>
#include <CanvasDevice.h>
#include <CanvasDrawingSession.h>
#include <CanvasImageBrush.h>
#include <CanvasImageSource.h>
#include <CanvasImageSourceDrawingSessionAdapter.h>
#include <CanvasLinearGradientBrush.h>
#include <CanvasRadialGradientBrush.h>
#include <CanvasRenderTarget.h>
#include <CanvasSolidColorBrush.h>
#include <CanvasStrokeStyle.h>
#include <CanvasSwapChain.h>
#include <controls\CanvasSwapChainPanel.h>
#include <CanvasTextFormat.h>
#include <Conversion.h>
#include <DxgiUtilities.h>
#include <controls\RecreatableDeviceManager.h>
#include <ResourceManager.h>
#include <ResourceTracker.h>
#include <ResourceWrapper.h>
#include <Strings.h>
#include <effects\CanvasEffect.h>


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
#include "MockD2DBitmapBrush.h"
#include "MockD2DImageBrush.h"
#include "MockD3D11Device.h"
#include "MockWICFormatConverter.h"
#include "MockD2DGradientStopCollection.h"
#include "MockD2DLinearGradientBrush.h"
#include "MockD2DRadialGradientBrush.h"
#include "MockSurfaceImageSource.h"
#include "MockSurfaceImageSourceFactory.h"
#include "StubCanvasBrush.h"
#include "StubCanvasDevice.h"
#include "StubCanvasDrawingSessionAdapter.h"
#include "StubD2DBrush.h"
#include "StubD2DDeviceContext.h"
#include "StubD2DResources.h"
#include "StubD2DStrokeStyle.h"
#include "StubImageControl.h"
#include "StubSurfaceImageSource.h"
#include "StubSurfaceImageSourceFactory.h"
#include "StubUserControl.h"

