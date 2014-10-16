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
using System.Xml.Serialization;
using System.Collections.Generic;

namespace CodeGen.Test
{
    [TestClass]
    public class UnitTests
    {
        [TestMethod]
        public void FormattingHelpers()
        {
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter(null), null);
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter(""), "");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("a"), "A");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("A"), "A");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("_"), "_");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("cat"), "Cat");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("Cat"), "Cat");
            Assert.AreEqual(Formatter.StylizeWithCapitalLeadingLetter("...cat"), "...cat");

            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators(null), null);
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators(""), "");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("d"), "D");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("D"), "D");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("_"), "");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("__"), "");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("a__"), "A");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("_a_"), "A");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("__a"), "A");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("a_b_c"), "ABC");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("CAT_DOG"), "CatDog");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("CATDOG"), "Catdog");
            Assert.AreEqual(Formatter.StylizeNameFromUnderscoreSeparators("TYPE_1X1_suffix"), "Type1x1Suffix");
        }

        [TestMethod]
        [DeploymentItem("../../../../winrt/lib/effects/generated", "codegen/effects/expected")]
        [DeploymentItem("../../../../tools/codegen/exe/apiref/effects", "codegen/effects/in/apiref/effects")]
        [DeploymentItem("Deployed Files/Settings.xml", "codegen/effects/in")]
        public void OutputEffectsIsSync()
        {
            //
            // This test sets codegen to generate its output to some temporary 
            // files, and then diffs the temporary files with what's checked in. 
            //
            // This allows us to detect if the output files were manually 
            // edited, or someone changed codegen without re-running it.
            //
            // The tests are executed from a deployed directory that contains 
            // the test executable / dependent assemblies and the 
            // DeploymentItems listed above.  The codegen outputs under this 
            // directory as well.  The test runner cleans up the entire deploy 
            // directory on exit.
            //
            string inputDir = "codegen/effects/in";
            string expectedDir = "codegen/effects/expected";
            string actualDir = "codegen/effects/actual";

            // Create directory for actual output
            Directory.CreateDirectory(actualDir);

            // Run codegen
            CodeGen.Program.GenerateEffectsCode(inputDir, actualDir);

            // Verify the output from the codegen matches what was expected
            var expectedDirectoryInfo = new DirectoryInfo(expectedDir);
            var actualDirectoryInfo = new DirectoryInfo(actualDir);

            FileInfo[] expectedGeneratedFiles = expectedDirectoryInfo.GetFiles();
            FileInfo[] actualGeneratedFiles = actualDirectoryInfo.GetFiles();

            // Ensure the correct number of files was generated.
            const int expectedEffectCount = 34;
            Assert.AreEqual(expectedEffectCount * 3 + 1, expectedGeneratedFiles.Length);
            Assert.AreEqual(expectedGeneratedFiles.Length, actualGeneratedFiles.Length);

            // For each codegenned file in the tree, ensure it was output to the test folder.
            CheckFilesMatch(expectedGeneratedFiles, actualDir);
        }

        [TestMethod]
        [DeploymentItem("Deployed Files/Canvas.codegen.cpp", "codegen/expected")]
        [DeploymentItem("Deployed Files/Canvas.codegen.idl", "codegen/expected")]
        [DeploymentItem("Deployed Files/D2DEffectAuthor.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes2.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes3.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/Settings.xml", "codegen/in")]
        public void OutputIsInSync()
        {
            //
            // This test sets codegen to generate its output to some temporary 
            // files, and then diffs the temporary files with what's checked in. 
            //
            // This allows us to detect if the output files were manually 
            // edited, or someone changed codegen without re-running it.
            //
            // The tests are executed from a deployed directory that contains 
            // the test executable / dependent assemblies and the 
            // DeploymentItems listed above.  The codegen outputs under this 
            // directory as well.  The test runner cleans up the entire deploy 
            // directory on exit.
            //

            string inputDir = "codegen/in";
            string expectedDir = "codegen/expected";
            string actualDir = "codegen/actual";

            // Run the codegen            
            CodeGen.Program.GenerateCode(inputDir, actualDir);

            // Verify the output from the codegen matches what was expected
            var expectedDirectoryInfo = new DirectoryInfo(expectedDir);
            var actualDirectoryInfo = new DirectoryInfo(actualDir);

            FileInfo[] expectedGeneratedFiles = expectedDirectoryInfo.GetFiles("*.codegen.*");
            FileInfo[] actualGeneratedFiles = actualDirectoryInfo.GetFiles(); // Used for .Length only

            // Ensure the correct number of files was generated.
            Assert.AreEqual(2, expectedGeneratedFiles.Length);
            Assert.AreEqual(expectedGeneratedFiles.Length, actualGeneratedFiles.Length);

            // For each codegenned file in the tree, ensure it was output to the test folder.
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

        [TestMethod]
        [DeploymentItem("Deployed Files/D2DEffectAuthor.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes2.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/D2DTypes3.xml", "codegen/in/apiref")]
        [DeploymentItem("Deployed Files/Settings.xml", "codegen/in")]
        public void OverridesAreWellFormed()
        {
            List<string> files = CodeGen.Program.GetInputFileList();

            string inputDir = "codegen/in";

            Overrides.XmlBindings.Settings overridesXmlData = XmlBindings.Utilities.LoadXmlData<Overrides.XmlBindings.Settings>(inputDir, "Settings.xml");
            Formatter.Prefix = overridesXmlData.Prefix.Value;
            Formatter.Subnamespace = overridesXmlData.Subnamespace.Value;

            List<D2DTypes> typeDocuments = new List<D2DTypes>();
            Dictionary<string, QualifiableType> typeDictionary = new Dictionary<string, QualifiableType>();
            OutputDataTypes outputDataTypes = new OutputDataTypes();
            foreach (string fileName in files)
            {
                XmlBindings.D2DTypes xmlDocument = XmlBindings.Utilities.LoadXmlData<XmlBindings.D2DTypes>(inputDir, fileName);
                typeDocuments.Add(new D2DTypes(xmlDocument, overridesXmlData, typeDictionary, outputDataTypes));
            }

            foreach (Overrides.XmlBindings.Primitive overridePrimitive in overridesXmlData.Primitives)
            {
                Assert.IsTrue(typeDictionary.ContainsKey(overridePrimitive.Name), "Unexpected override primitive: " + overridePrimitive.Name);
            }

            foreach (Overrides.XmlBindings.Namespace overrideNamespace in overridesXmlData.Namespaces)
            {
                // Skip Effects namespace because it have different logic
                if (overrideNamespace.Name == "Effects")
                    continue;

                Assert.IsTrue(overrideNamespace.Name == "D2D" || overrideNamespace.Name == "D2D1", "Unexpected override namespace: " + overrideNamespace.Name);

                foreach (Overrides.XmlBindings.Struct overrideStruct in overrideNamespace.Structs)
                {
                    string nameKey = overrideNamespace.Name + "::" + overrideStruct.Name;
                    Assert.IsTrue(typeDictionary.ContainsKey(nameKey), "Unexpected override struct: " + overrideStruct.Name);
                }

                foreach (Overrides.XmlBindings.Enum overrideEnum in overrideNamespace.Enums)
                {
                    string nameKey = overrideNamespace.Name + "::" + overrideEnum.Name;
                    Assert.IsTrue(typeDictionary.ContainsKey(nameKey), "Unexpected override enum: " + overrideEnum.Name);

                    CodeGen.Enum e = (CodeGen.Enum)(typeDictionary[nameKey]);
                    foreach (Overrides.XmlBindings.EnumValue overrideEnumValue in overrideEnum.Values)
                    {
                        CodeGen.EnumValue match = e.Values.Find(x => x.NativeName == overrideEnumValue.Name);
                        Assert.IsNotNull(match, "Unexpected override enum value: " + overrideEnum + "::" + overrideEnumValue.Name);
                    }
                }
            }
        }
    }
}
