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
#include <Microsoft.Graphics.Canvas.Effects.interop.h>

#include <Microsoft.Graphics.Canvas.h>

// Pick up the inbox or local WinRT DirectX types as appropriate
#include <WinRTDirectX.h>

#pragma warning(disable: 4100)  // "unreferenced formal parameter"
#pragma warning(disable: 4351)  // "elements of {array} will be default initialized"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

#include "Helpers.h"
#include "MockDxgiDevice.h"
#include "MockDxgiSurface.h"
