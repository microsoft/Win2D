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

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Microsoft.Graphics.Canvas;

namespace test.managed
{
    [TestClass]
    public class DeviceRemovedTests_WithoutControl
    {
        bool m_deviceLostRaised;

        [TestMethod]
        public void DeviceRaisesDeviceLostEvent()
        {
            CanvasDevice device = new CanvasDevice();

            // Verifies that the event is raised, even if the native
            // underlying device was not lost.
            Assert.IsFalse(device.IsDeviceLost(0));

            device.DeviceLost += device_DeviceLost;

            device.RaiseDeviceLost();

            Assert.IsTrue(m_deviceLostRaised);

            device.DeviceLost -= device_DeviceLost;

            // If the event was actually removed, shout not hit the
            // assert at the top of device_DeviceLost.
            device.RaiseDeviceLost(); 
        }

        void device_DeviceLost(CanvasDevice sender, object args)
        {
            Assert.IsFalse(m_deviceLostRaised);

            Assert.IsNull(args);

            m_deviceLostRaised = true;
        }
    }
}
