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

#include <wrl.h>
#include <strsafe.h>
#include <CppUnitTest.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <DirectXMath.h>

#include <ErrorHandling.h>

#include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#include <Microsoft.Graphics.Canvas.native.h>
#include <Canvas.abi.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

#include "Helpers.h"
#include "MockDxgiDevice.h"
#include "MockDxgiSurface.h"
