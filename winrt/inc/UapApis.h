// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <sdkddkver.h>


# include <windows.graphics.directx.h>
# include <windows.graphics.directx.direct3d11.interop.h>
# include <windows.graphics.effects.interop.h>


#ifdef __cplusplus_winrt
# define WINRT_PREFIX
#else
# define WINRT_PREFIX ABI::
#endif

namespace WinRTDirectX
{
    using ::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess;
    using namespace WINRT_PREFIX Windows::Graphics::DirectX;
    using namespace WINRT_PREFIX Windows::Graphics::DirectX::Direct3D11;
    using namespace WINRT_PREFIX Windows::Graphics::Effects;
}

#undef WINRT_PREFIX
