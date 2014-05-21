using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
        [DeploymentItem("Deployed Files/DirectX.codegen.cpp", "codegen/expected")]
        [DeploymentItem("Deployed Files/DirectX.codegen.idl", "codegen/expected")]
        [DeploymentItem("Deployed Files/D2DEffectAuthor.xml", "codegen/in")]
        [DeploymentItem("Deployed Files/D2DTypes.xml", "codegen/in")]
        [DeploymentItem("Deployed Files/D2DTypes2.xml", "codegen/in")]
        [DeploymentItem("Deployed Files/D2DTypes3.xml", "codegen/in")]
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
            Assert.AreEqual(expectedGeneratedFiles.Length, actualGeneratedFiles.Length);

            // For each codegenned file in the tree, ensure it was output to the test folder.
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
