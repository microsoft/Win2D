// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#ifdef NTDDI_WIN10_RS4
#if NTDDI_VERSION >= NTDDI_WIN10_RS4
#define ENABLE_GEOMETRY_INTEROP 1
#endif
#endif

#ifndef __midl
#ifdef ENABLE_GEOMETRY_INTEROP
#include "Windows.Graphics.Interop.h"
#endif
#endif
