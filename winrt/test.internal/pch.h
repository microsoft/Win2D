// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

#include "utils/Helpers.h"
#include "mocks/MockHelpers.h"
#include "mocks/MockAsyncAction.h"
#include "mocks/MockDxgiSwapChain.h"
#include "mocks/MockCanvasDevice.h"
#include "mocks/MockCanvasDrawingSession.h"
#include "mocks/MockCanvasImageSourceDrawingSessionFactory.h"
#include "mocks/MockCoreApplication.h"
#include "mocks/MockD2DBitmapBrush.h"
#include "mocks/MockD2DCommandList.h"
#include "mocks/MockD2DDeviceContext.h"
#include "mocks/MockD2DDevice.h"
#include "mocks/MockD2DFactory.h"
#include "mocks/MockD2DGradientStopCollection.h"
#include "mocks/MockD2DImageBrush.h"
#include "mocks/MockD2DLinearGradientBrush.h"
#include "mocks/MockD2DRadialGradientBrush.h"
#include "mocks/MockD2DSolidColorBrush.h"
#include "mocks/MockD2DStrokeStyle.h"
#include "mocks/MockD3D11Device.h"
#include "xaml/MockRecreatableDeviceManager.h"
#include "mocks/MockSurfaceImageSource.h"
#include "mocks/MockSurfaceImageSourceFactory.h"
#include "mocks/MockSuspendingEventArgs.h"
#include "mocks/MockWICFormatConverter.h"
#include "stubs/StubD2DResources.h"
#include "stubs/StubCanvasDevice.h"
#include "stubs/StubCanvasDrawingSessionAdapter.h"
#include "stubs/StubD2DDeviceContext.h"
#include "stubs/StubImageControl.h"
#include "stubs/StubSurfaceImageSource.h"
#include "stubs/StubSurfaceImageSourceFactory.h"
#include "stubs/StubUserControl.h"
#include "stubs/TestBitmapResourceCreationAdapter.h"
#include "stubs/TestDeviceResourceCreationAdapter.h"
#include "stubs/SwitchableTestBrushFixture.h"
#include "xaml/CanvasControlTestAdapter.h"
#include "xaml/ControlFixtures.h"
#include "xaml/BasicControlFixture.h"
#include "mocks/MockCanvasSwapChain.h"
