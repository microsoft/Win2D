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

#include <sdkddkver.h>

#if WINVER <= 0x0603
# define USE_LOCAL_WINRT_DIRECTX
#endif


#ifdef USE_LOCAL_WINRT_DIRECTX
# include <Microsoft.Graphics.Canvas.DirectX.Direct3D11.interop.h>
#else
# include <windows.graphics.directx.h>
# include <windows.graphics.directx.direct3d11.interop.h>
#endif


#ifdef __cplusplus_winrt
# define WINRT_PREFIX
#else
# define WINRT_PREFIX ABI::
#endif

namespace WinRTDirectX
{
#ifdef USE_LOCAL_WINRT_DIRECTX

    using ::Microsoft::Graphics::Canvas::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess;
    using namespace WINRT_PREFIX Microsoft::Graphics::Canvas::DirectX::Direct3D11;
    using namespace WINRT_PREFIX Microsoft::Graphics::Canvas::DirectX;

#else

    using ::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess;
    using namespace WINRT_PREFIX Windows::Graphics::DirectX;
    using namespace WINRT_PREFIX Windows::Graphics::DirectX::Direct3D11;

#endif
}

#undef WINRT_PREFIX
