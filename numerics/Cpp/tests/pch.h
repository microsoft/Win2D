// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "../WindowsNumerics.h"

#pragma warning(disable: 4505)  // "unreferenced local function"

#include <SDKDDKVer.h>
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define NUMERICS_TEST_CLASS(ClassName) TEST_CLASS(ClassName)
#define NUMERICS_TEST_CLASS_INNER(ClassName)

#define NUMERICS_ABI_NAMESPACE Microsoft::Graphics::Canvas::Numerics

#include "Helpers.h"
