// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(CanvasCreateResourcesEventArgsTests)
{
    TEST_METHOD(CanvasCreateResourcesEventArgs_ShouldBeActivatable)
    {
        CanvasCreateResourcesReason createResourceReasons[] =
        {
            CanvasCreateResourcesReason::FirstTime,
            CanvasCreateResourcesReason::NewDevice,
            CanvasCreateResourcesReason::DpiChanged
        };

        for (auto testCase : createResourceReasons)
        {
            CanvasCreateResourcesEventArgs^ args = ref new CanvasCreateResourcesEventArgs(testCase);
            Assert::AreEqual(testCase, args->Reason);
        }
    }
};
