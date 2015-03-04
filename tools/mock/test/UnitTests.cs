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

using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Text;
using System.Collections.Generic;

namespace Mock.Test
{
    [TestClass]
    public class UnitTests
    {
        [TestMethod]
        [DeploymentItem("Deployed Files/D2DEffectAuthor.xml", "mock/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes.xml", "mock/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes2.xml", "mock/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes3.xml", "mock/in/apiref")]
        [DeploymentItem("Deployed Files/Settings.xml", "mock/in")]
        [DeploymentItem("Deployed Files/MockD2DGeometrySink.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DGeometryRealization.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DPathGeometry.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DRectangleGeometry.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DRoundedRectangleGeometry.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DEllipseGeometry.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DTransformedGeometry.h", "mock/expected")]
        [DeploymentItem("Deployed Files/MockD2DGeometryGroup.h", "mock/expected")]
        public void OutputIsInSync()
        {
            //
            // This test sets the mock tool to generate its output to some temporary 
            // files, and then diffs the temporary files with what's checked in. 
            // It works similarly to the CodeGen.Test.OutputIsInSync.
            //
            // This validates that the mock tool generates something that is correct.
            //

            string inputDir = "mock/in";
            string expectedDir = "mock/expected";
            string actualDir = "mock/actual";
         
            Mock.Program.GenerateMocks(inputDir, actualDir);

            var expectedDirectoryInfo = new DirectoryInfo(expectedDir);
            var actualDirectoryInfo = new DirectoryInfo(actualDir);

            FileInfo[] expectedGeneratedFiles = expectedDirectoryInfo.GetFiles("*.h");
            FileInfo[] actualGeneratedFiles = actualDirectoryInfo.GetFiles(); // Used for .Length only

            Assert.AreEqual(8, expectedGeneratedFiles.Length);
            Assert.AreEqual(expectedGeneratedFiles.Length, actualGeneratedFiles.Length);

            CheckFilesMatch(expectedGeneratedFiles, actualDir);
        }

        private void CheckFilesMatch(FileInfo[] expectedGeneratedFiles, string actualDir)
        {
            foreach (FileInfo expectedFile in expectedGeneratedFiles)
            {
                FileInfo actualFile = new FileInfo(Path.Combine(actualDir, expectedFile.Name));
                Assert.IsTrue(actualFile.Exists);

                string expectedFileContent = File.ReadAllText(expectedFile.FullName);
                string actualFileContent = File.ReadAllText(actualFile.FullName);

                Assert.AreEqual(expectedFileContent, actualFileContent);
            }
        }
    }
}
