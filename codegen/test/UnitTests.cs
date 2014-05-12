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
        public void OutputIsInSync()
        {            
            //
            // This test sets codegen to generate its output to some temporary files, and then diffs the temporary files
            // with what's checked in. 
            //
            // This allows us to detect if the output files were manually edited, or someone changed codegen without
            // re-running it.
            //
            // Normally, Codegen outputs to winrt/lib.
            // For this test, Codegen outputs to codegen/test/tmp.
            //

            DirectoryInfo treeDirectoryInfo = null;
            DirectoryInfo testDirectoryInfo = null;

            try
            {            
                string treeOutputDir = CodeGen.Program.GetDefaultOutputLocation();
                string testOutputDir = Path.Combine(Path.GetTempPath(), "codegen");

                treeDirectoryInfo = new DirectoryInfo(treeOutputDir);
                testDirectoryInfo = new DirectoryInfo(testOutputDir);

                // Generate code to the test directory.
                CodeGen.Program.GenerateCode(testOutputDir);

                FileInfo[] treeGeneratedFiles = treeDirectoryInfo.GetFiles("*.codegen.*");
                FileInfo[] testGeneratedFiles = testDirectoryInfo.GetFiles(); // Used for .Length only

                // Ensure the correct number of files was generated.
                Assert.AreEqual(treeGeneratedFiles.Length, testGeneratedFiles.Length);

                // For each codegenned file in the tree, ensure it was output to the test folder.
                foreach (FileInfo treeFile in treeGeneratedFiles)
                {
                    FileInfo testFile = new FileInfo(Path.Combine(testOutputDir, treeFile.Name));
                    Assert.IsTrue(testFile.Exists);

                    string treeFileContent = File.ReadAllText(treeFile.FullName);
                    string testFileContent = File.ReadAllText(testFile.FullName);

                    Assert.AreEqual(treeFileContent, testFileContent);
                }            
            }
            finally
            {
                // Delete the temporary directory.
                if (testDirectoryInfo != null)
                {
                    testDirectoryInfo.Delete(true); // 'true' indicates recursive behavior, deletes contained files
                }
            }
        }
    }
}
