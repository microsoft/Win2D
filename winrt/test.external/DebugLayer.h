// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once


// Put one of these on the stack to temporarily disable the D2D debug
// layer while running a test that is expected to hit error conditions.
// We can't just let the debug layer trap these because it asserts
// on failure in a way that takes down the entire test harness :-(
class DisableDebugLayer
{
    CanvasDebugLevel m_previousDebugLevel;

public:
    DisableDebugLayer()
    {
        m_previousDebugLevel = CanvasDevice::DebugLevel;

        CanvasDevice::DebugLevel = CanvasDebugLevel::None;
    }

    ~DisableDebugLayer()
    {
        CanvasDevice::DebugLevel = m_previousDebugLevel;
    }

    DisableDebugLayer(DisableDebugLayer const&) = delete;
    DisableDebugLayer& operator=(DisableDebugLayer const&) = delete;
};
