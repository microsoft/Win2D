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

#include "../lib/pch.h"

// UnitTest
#pragma warning(push)
#pragma warning(disable: 4499) // TODO: Disable "explicit specialization cannot have a storage class" warning until we pick up fix to DevDiv.1116774
#include <CppUnitTest.h>
#pragma warning(pop)

// local headers
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

namespace canvas {}
using namespace canvas;
using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Effects;

// TODO #997/#1429: move these files
#include "../test.external/MockDxgiDevice.h"
#include "../test.external/MockDxgiSurface.h"

#include "Helpers.h"
#include "MockHelpers.h"
#include "MockAsyncAction.h"
#include "MockDxgiSwapChain.h"
#include "MockCanvasDevice.h"
#include "MockCanvasDrawingSession.h"
#include "MockCanvasImageSourceDrawingSessionFactory.h"
#include "MockCoreApplication.h"
#include "MockD2DBitmapBrush.h"
#include "MockD2DCommandList.h"
#include "MockD2DDeviceContext.h"
#include "MockD2DDevice.h"
#include "MockD2DFactory.h"
#include "MockD2DGradientStopCollection.h"
#include "MockD2DImageBrush.h"
#include "MockD2DLinearGradientBrush.h"
#include "MockD2DRadialGradientBrush.h"
#include "MockD2DSolidColorBrush.h"
#include "MockD2DStrokeStyle.h"
#include "MockD3D11Device.h"
#include "MockRecreatableDeviceManager.h"
#include "MockSurfaceImageSource.h"
#include "MockSurfaceImageSourceFactory.h"
#include "MockSuspendingEventArgs.h"
#include "MockWICFormatConverter.h"
#include "StubD2DResources.h"
#include "StubCanvasDevice.h"
#include "StubCanvasDrawingSessionAdapter.h"
#include "StubD2DDeviceContext.h"
#include "StubImageControl.h"
#include "StubSurfaceImageSource.h"
#include "StubSurfaceImageSourceFactory.h"
#include "StubUserControl.h"
#include "TestBitmapResourceCreationAdapter.h"
#include "TestDeviceResourceCreationAdapter.h"
#include "SwitchableTestBrushFixture.h"
#include "CanvasControlTestAdapter.h"
#include "ControlFixtures.h"
#include "BasicControlFixture.h"
#include "MockCanvasSwapChain.h"
