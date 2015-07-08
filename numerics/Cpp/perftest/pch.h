// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "../WindowsNumerics.h"

using namespace Windows::Foundation::Numerics;

#include <algorithm>
#include <array>
#include <numeric>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "EnsureNotOptimizedAway.h"
#include "MakeRandom.h"
#include "PerfTest.h"
