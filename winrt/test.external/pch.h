// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "targetver.h"

#include <memory>
#include <functional>
#include <iterator>

#include <wrl.h>
#include <strsafe.h>
#pragma warning(push)
#pragma warning(disable: 4499) // TODO: Disable "explicit specialization cannot have a storage class" warning until we pick up fix to DevDiv.1116774
#include <CppUnitTest.h>
#pragma warning(pop)

#include <d3d11.h>
#include <dxgi1_3.h>
#pragma warning(push)
#pragma warning(disable: 4458)  // TODO: Disable "hides class member" warning until we pick up fix to MS.601961
#include <d2d1_2.h>
#pragma warning(pop)
#include <dwrite_2.h>
#include <DirectXMath.h>
#include <Combaseapi.h>
#include <wincodec.h>

#include <ErrorHandling.h>
#include <Utilities.h>
#include <Constants.h>
#include <ComArray.h>

#include <windowsnumerics.h>

#include <Microsoft.Graphics.Canvas.native.h>
#include <Microsoft.Graphics.Canvas.h>

// Pick up the inbox or local WinRT DirectX types as appropriate
#include <UapApis.h>

#pragma warning(disable: 4100)  // "unreferenced formal parameter"
#pragma warning(disable: 4351)  // "elements of {array} will be default initialized"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

#include "Helpers.h"
#include "MockDxgiDevice.h"
#include "MockDxgiSurface.h"
