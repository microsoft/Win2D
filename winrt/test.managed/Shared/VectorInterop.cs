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
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;

namespace test.managed
{
    [TestClass]
    public class VectorInterop
    {
        [TestMethod]
        public void UseVectorFromManagedCode()
        {
            var v = NativeComponent.VectorCreator.CreateVectorOfInts(0, false);

            Assert.AreEqual(0, v.Count);

            // Add some values.
            v.Add(23);
            v.Add(42);

            // Read them back.
            Assert.AreEqual(2, v.Count);

            Assert.AreEqual(23, v[0]);
            Assert.AreEqual(42, v[1]);

            // Test GetEnumerator.
            var e = v.GetEnumerator();

            Assert.IsTrue(e.MoveNext());
            Assert.AreEqual(23, e.Current);

            Assert.IsTrue(e.MoveNext());
            Assert.AreEqual(42, e.Current);

            Assert.IsFalse(e.MoveNext());
            Assert.IsFalse(e.MoveNext());

            // Change them.
            v[0] = 1;
            v[1] = 2;

            Assert.AreEqual(1, v[0]);
            Assert.AreEqual(2, v[1]);

            // Test Contains.
            Assert.IsTrue(v.Contains(1));
            Assert.IsTrue(v.Contains(2));
            Assert.IsFalse(v.Contains(3));

            // Test IndexOf.
            Assert.AreEqual(0, v.IndexOf(1));
            Assert.AreEqual(1, v.IndexOf(2));
            Assert.AreEqual(-1, v.IndexOf(3));

            // Test Insert.
            v.Insert(1, 23);

            Assert.AreEqual(3, v.Count);

            Assert.AreEqual(1, v[0]);
            Assert.AreEqual(23, v[1]);
            Assert.AreEqual(2, v[2]);

            // Test RemoveAt.
            v.RemoveAt(1);

            Assert.AreEqual(2, v.Count);

            Assert.AreEqual(1, v[0]);
            Assert.AreEqual(2, v[1]);

            // Out-of-range accesses.
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { var tmp = v[2]; });
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { var tmp = v[-1]; });

            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v[2] = 123; });
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v[-1] = 123; });

            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v.Insert(3, 123); });
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v.Insert(-1, 123); });

            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v.RemoveAt(2); });
            Assert.ThrowsException<ArgumentOutOfRangeException>(() => { v.RemoveAt(-1); });

            Assert.AreEqual(2, v.Count);

            Assert.AreEqual(1, v[0]);
            Assert.AreEqual(2, v[1]);

            // Test Clear.
            v.Clear();
            
            Assert.AreEqual(0, v.Count);
        }


        [TestMethod]
        public void UseFixedSizeVectorFromManagedCode()
        {
            var v = NativeComponent.VectorCreator.CreateVectorOfInts(2, true);

            // Collection should start out the size we specified, and initialized to zero.
            Assert.AreEqual(2, v.Count);

            Assert.AreEqual(0, v[0]);
            Assert.AreEqual(0, v[1]);

            // We can change its contents.
            v[0] = 23;
            v[1] = 42;

            Assert.AreEqual(23, v[0]);
            Assert.AreEqual(42, v[1]);

            // But we can't do anything that changes the collection size.
            Assert.ThrowsException<NotImplementedException>(() => { v.Add(123); });
            Assert.ThrowsException<NotImplementedException>(() => { v.Insert(0, 123); });
            Assert.ThrowsException<NotImplementedException>(() => { v.RemoveAt(0); });
            Assert.ThrowsException<NotImplementedException>(() => { v.Clear(); });

            Assert.AreEqual(2, v.Count);

            Assert.AreEqual(23, v[0]);
            Assert.AreEqual(42, v[1]);
        }
    }
}
