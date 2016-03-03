// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"


// In order to get test coverage both with and without the Direct2D debug layer,
// we turn it on in Debug configuration but leave it off when testing Release.
#ifndef NDEBUG

TEST_MODULE_INITIALIZE(ModuleInitialize)
{
    CanvasDevice::DebugLevel = CanvasDebugLevel::Warning;

    // But the debug layer might not be installed on the current machine!
    // When it's not available, attempting to create a device using it will fail.
    // We don't want to require installing the debug layer in order to run tests,
    // so we'll just turn it off again if that happens.
    try
    {
        ref new CanvasDevice();
    }
    catch (Platform::FailureException^)
    {
        CanvasDevice::DebugLevel = CanvasDebugLevel::None;
    }
}

#endif
