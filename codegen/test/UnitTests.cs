using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CodeGen.Test
{
    [TestClass]
    public class UnitTest1
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
    }
}
