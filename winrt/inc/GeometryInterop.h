// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#ifdef NTDDI_WIN10_RS3 //NTDDI_WIN10_RS4
#if NTDDI_VERSION >= NTDDI_WIN10_RS3 //NTDDI_WIN10_RS4
#define ENABLE_GEOMETRY_INTEROP 1

// Until the RS4 SDK is available
#define NEED_TO_DEFINE_GEOMETRY_INTEROP_INTERFACES 1

#endif
#endif

#ifndef __midl
#ifdef ENABLE_GEOMETRY_INTEROP
#ifndef NEED_TO_DEFINE_GEOMETRY_INTEROP_INTERFACES

#include "Windows.Graphics.Interop.h"

#else

namespace ABI {
    namespace Windows {
        namespace Graphics
        {
#undef INTERFACE
#define INTERFACE IGeometrySource2DInterop
            DECLARE_INTERFACE_IID_(IGeometrySource2DInterop, IUnknown, "0657AF73-53FD-47CF-84FF-C8492D2A80A3")
            {
                IFACEMETHOD(GetGeometry)(
                    _COM_Outptr_ ID2D1Geometry** value
                    ) PURE;

                IFACEMETHOD(TryGetGeometryUsingFactory)(
                    _In_ ID2D1Factory* factory,
                    _COM_Outptr_result_maybenull_ ID2D1Geometry** value
                    ) PURE;
            };
        }
    }
}

#endif
#endif
#endif

