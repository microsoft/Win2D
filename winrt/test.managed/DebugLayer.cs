// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Runtime.InteropServices;

namespace test.managed
{
    // In order to get test coverage both with and without the Direct2D debug layer,
    // we turn it on in Debug configuration but leave it off when testing Release.
#if DEBUG

    [TestClass]
    public static class EnableDebugLayer
    {
        [AssemblyInitialize]
        static public void AssemblyInitialize(TestContext context)
        {
            CanvasDevice.DebugLevel = CanvasDebugLevel.Warning;

            // But the debug layer might not be installed on the current machine!
            // When it's not available, attempting to create a device using it will fail.
            // We don't want to require installing the debug layer in order to run tests,
            // so we'll just turn it off again if that happens.
            try
            {
                new CanvasDevice().Dispose();
            }
            catch (COMException)
            {
                CanvasDevice.DebugLevel = CanvasDebugLevel.None;
            }
        }
    }

#endif


    // Put one of these inside a using block to temporarily disable the D2D debug
    // layer while running a test that is expected to hit error conditions.
    class DisableDebugLayer : IDisposable
    {
        CanvasDebugLevel previousDebugLevel;

        public DisableDebugLayer()
        {
            previousDebugLevel = CanvasDevice.DebugLevel;

            CanvasDevice.DebugLevel = CanvasDebugLevel.None;
        }

        public void Dispose()
        {
            CanvasDevice.DebugLevel = previousDebugLevel;
        }
    }
}
