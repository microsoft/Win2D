// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace CodeGen.Test
{
    [TestClass]
    public class UuidTests
    {
        [TestMethod]
        public void Version5UuidTest()
        {
            string name1 = "Hello";
            string name2 = "World";
            
            Guid salt1 = new Guid("A911588C-DB0A-41D2-AF64-EBEC037294D0");
            Guid salt2 = new Guid("88B56025-60BC-4FE9-AA3F-1148BD0C653E");

            Guid result11 = UuidHelper.GetVersion5Uuid(salt1, name1);
            Guid result12 = UuidHelper.GetVersion5Uuid(salt1, name2);
            Guid result21 = UuidHelper.GetVersion5Uuid(salt2, name1);
            Guid result22 = UuidHelper.GetVersion5Uuid(salt2, name2);

            // Validate against a hard-coded expected result.
            Assert.AreEqual(new Guid("C3091B3B-5718-5928-8CBF-41298EF7C614"), result11);

            // All results should be different (changing either name or salt alters the UUID).
            Assert.AreNotEqual(result11, result12);
            Assert.AreNotEqual(result11, result21);
            Assert.AreNotEqual(result11, result22);
            Assert.AreNotEqual(result12, result21);
            Assert.AreNotEqual(result12, result22);
            Assert.AreNotEqual(result21, result22);

            // Hashing the same inputs a second time should give the same results as before.
            Assert.AreEqual(result11, UuidHelper.GetVersion5Uuid(salt1, name1));
            Assert.AreEqual(result12, UuidHelper.GetVersion5Uuid(salt1, name2));
            Assert.AreEqual(result21, UuidHelper.GetVersion5Uuid(salt2, name1));
            Assert.AreEqual(result22, UuidHelper.GetVersion5Uuid(salt2, name2));

            // All should be marked as standard RFC 4122 UUID variant.
            Assert.AreEqual(2, GetUuidVariant(result11));
            Assert.AreEqual(2, GetUuidVariant(result12));
            Assert.AreEqual(2, GetUuidVariant(result21));
            Assert.AreEqual(2, GetUuidVariant(result22));

            // All should be marked as version 5 (SHA-1 name-based) UUID.
            Assert.AreEqual(5, GetUuidVersion(result11));
            Assert.AreEqual(5, GetUuidVersion(result12));
            Assert.AreEqual(5, GetUuidVersion(result21));
            Assert.AreEqual(5, GetUuidVersion(result22));

            // Sanity check our GetUuidVariant and GetUUidVersion methods against standard (version 4) UUIDs.
            Assert.AreEqual(2, GetUuidVariant(salt1));
            Assert.AreEqual(2, GetUuidVariant(salt2));

            Assert.AreEqual(4, GetUuidVersion(salt1));
            Assert.AreEqual(4, GetUuidVersion(salt2));
        }


        static int GetUuidVariant(Guid uuid)
        {
            return (uuid.ToByteArray()[8] & 0xC0) >> 6;
        }


        static int GetUuidVersion(Guid uuid)
        {
            return (uuid.ToByteArray()[7] & 0xF0) >> 4;
        }
    }
}
