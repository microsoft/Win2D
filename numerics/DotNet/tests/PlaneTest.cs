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
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System.Numerics;

namespace NumericsTests
{
    [TestClass()]
    public class PlaneTest
    {
        // A test for Equals (Plane)
        [TestMethod]
        public void PlaneEqualsTest1()
        {
            Plane a = new Plane(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b = new Plane(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            expected = false;
            actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");
        }

        // A test for Equals (object)
        [TestMethod]
        public void PlaneEqualsTest()
        {
            Plane a = new Plane(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b = new Plane(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            object obj = b;

            bool expected = true;
            bool actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            obj = b;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");

            // case 3: compare between different types.
            obj = new Quaternion();
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");

            // case 3: compare against null.
            obj = null;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Plane.Equals did not return the expected value.");
        }

        // A test for operator != (Plane, Plane)
        [TestMethod]
        public void PlaneInequalityTest()
        {
            Plane a = new Plane(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b = new Plane(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert.AreEqual(expected, actual, "Plane.operator != did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert.AreEqual(expected, actual, "Plane.operator != did not return the expected value.");
        }

        // A test for operator == (Plane, Plane)
        [TestMethod]
        public void PlaneEqualityTest()
        {
            Plane a = new Plane(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b = new Plane(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert.AreEqual(expected, actual, "Plane.operator != did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert.AreEqual(expected, actual, "Plane.operator != did not return the expected value.");
        }

        // A test for GetHashCode ()
        [TestMethod]
        public void PlaneGetHashCodeTest()
        {
            Plane target = new Plane(1.0f, 2.0f, 3.0f, 4.0f);

            int expected = target.Normal.GetHashCode() + target.D.GetHashCode();
            int actual = target.GetHashCode();
            Assert.AreEqual(expected, actual, "Plane.GetHashCode did not return the expected value.");
        }

        // A test for ToString ()
        [TestMethod]
        public void PlaneToStringTest()
        {
            Plane target = new Plane(1.0f, 2.0f, 3.0f, 4.1f);
            string expected = "{Normal:{X:1 Y:2 Z:3} D:4.1}";
            string actual = target.ToString();
            Assert.AreEqual(expected, actual, "Plane.ToString did not return the expected value.");
        }

        // A test for Plane (float, float, float, float)
        [TestMethod]
        public void PlaneConstructorTest1()
        {
            float a = 1.0f, b = 2.0f, c = 3.0f, d = 4.0f;
            Plane target = new Plane(a, b, c, d);

            Assert.IsTrue(
                target.Normal.X == a && target.Normal.Y == b && target.Normal.Z == c && target.D == d,
                "Plane.cstor did not return the expected value.");
        }

        // A test for Plane.CreateFromVertices
        [TestMethod]
        public void PlaneCreateFromVerticesTest()
        {
            Vector3 point1 = new Vector3(0.0f, 1.0f, 1.0f);
            Vector3 point2 = new Vector3(0.0f, 0.0f, 1.0f);
            Vector3 point3 = new Vector3(1.0f, 0.0f, 1.0f);

            Plane target = Plane.CreateFromVertices(point1, point2, point3);
            Plane expected = new Plane(new Vector3(0, 0, 1), -1.0f);
            Assert.AreEqual(target, expected, "Plane.cstor did not return the expected value.");
        }

        // A test for Plane.CreateFromVertices
        [TestMethod]
        public void PlaneCreateFromVerticesTest2()
        {
            Vector3 point1 = new Vector3(0.0f, 0.0f, 1.0f);
            Vector3 point2 = new Vector3(1.0f, 0.0f, 0.0f);
            Vector3 point3 = new Vector3(1.0f, 1.0f, 0.0f);

            Plane target = Plane.CreateFromVertices(point1, point2, point3);
            float invRoot2 = (float)(1 / Math.Sqrt(2));

            Plane expected = new Plane(new Vector3(invRoot2, 0, invRoot2), -invRoot2);
            Assert.IsTrue(MathHelper.Equal(target, expected), "Plane.cstor did not return the expected value.");
        }

        // A test for Plane (Vector3, float)
        [TestMethod]
        public void PlaneConstructorTest3()
        {
            Vector3 normal = new Vector3(1, 2, 3);
            float d = 4;

            Plane target = new Plane(normal, d);
            Assert.IsTrue(
                target.Normal == normal && target.D == d,
                "Plane.cstor did not return the expected value.");
        }

        // A test for Plane (Vector4)
        [TestMethod]
        public void PlaneConstructorTest()
        {
            Vector4 value = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Plane target = new Plane(value);

            Assert.IsTrue(
                target.Normal.X == value.X && target.Normal.Y == value.Y && target.Normal.Z == value.Z && target.D == value.W,
                "Plane.cstor did not return the expected value.");
        }

        [TestMethod]
        public void PlaneDotTest()
        {
            Plane target = new Plane(2, 3, 4, 5);
            Vector4 value = new Vector4(5, 4, 3, 2);

            float expected = 10 + 12 + 12 + 10;
            float actual = Plane.Dot(target, value);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.Dot returns unexpected value.");
        }

        [TestMethod]
        public void PlaneDotCoordinateTest()
        {
            Plane target = new Plane(2, 3, 4, 5);
            Vector3 value = new Vector3(5, 4, 3);

            float expected = 10 + 12 + 12 + 5;
            float actual = Plane.DotCoordinate(target, value);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.DotCoordinate returns unexpected value.");
        }

        [TestMethod]
        public void PlaneDotNormalTest()
        {
            Plane target = new Plane(2, 3, 4, 5);
            Vector3 value = new Vector3(5, 4, 3);

            float expected = 10 + 12 + 12;
            float actual = Plane.DotNormal(target, value);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.DotCoordinate returns unexpected value.");
        }

        [TestMethod]
        public void PlaneNormalizeTest()
        {
            Plane target = new Plane(1, 2, 3, 4);

            float f = target.Normal.LengthSquared();
            float invF = 1.0f / (float)Math.Sqrt(f);
            Plane expected = new Plane(target.Normal * invF, target.D * invF);

            Plane actual = Plane.Normalize(target);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.Normalize returns unexpected value.");

            // normalize, normalized normal.
            actual = Plane.Normalize(actual);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.Normalize returns unexpected value.");
        }

        [TestMethod]
        // Transform by matrix
        public void PlaneTransformTest1()
        {
            Plane target = new Plane(1, 2, 3, 4);
            target = Plane.Normalize(target);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Plane expected = new Plane();
            Matrix4x4 inv;
            Matrix4x4.Invert(m, out inv);
            Matrix4x4 itm = Matrix4x4.Transpose(inv);
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * itm.M11 + y * itm.M21 + z * itm.M31 + w * itm.M41;
            expected.Normal.Y = x * itm.M12 + y * itm.M22 + z * itm.M32 + w * itm.M42;
            expected.Normal.Z = x * itm.M13 + y * itm.M23 + z * itm.M33 + w * itm.M43;
            expected.D = x * itm.M14 + y * itm.M24 + z * itm.M34 + w * itm.M44;

            Plane actual;
            actual = Plane.Transform(target, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.Transform did not return the expected value.");
        }

        [TestMethod]
        // Transform by quaternion
        public void PlaneTransformTest2()
        {
            Plane target = new Plane(1, 2, 3, 4);
            target = Plane.Normalize(target);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Plane expected = new Plane();
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * m.M11 + y * m.M21 + z * m.M31 + w * m.M41;
            expected.Normal.Y = x * m.M12 + y * m.M22 + z * m.M32 + w * m.M42;
            expected.Normal.Z = x * m.M13 + y * m.M23 + z * m.M33 + w * m.M43;
            expected.D = x * m.M14 + y * m.M24 + z * m.M34 + w * m.M44;

            Plane actual;
            actual = Plane.Transform(target, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Plane.Transform did not return the expected value.");
        }

        // A test for Plane comparison involving NaN values
        [TestMethod]
        public void PlaneEqualsNanTest()
        {
            Plane a = new Plane(float.NaN, 0, 0, 0);
            Plane b = new Plane(0, float.NaN, 0, 0);
            Plane c = new Plane(0, 0, float.NaN, 0);
            Plane d = new Plane(0, 0, 0, float.NaN);

            Assert.IsFalse(a == new Plane(0, 0, 0, 0));
            Assert.IsFalse(b == new Plane(0, 0, 0, 0));
            Assert.IsFalse(c == new Plane(0, 0, 0, 0));
            Assert.IsFalse(d == new Plane(0, 0, 0, 0));

            Assert.IsTrue(a != new Plane(0, 0, 0, 0));
            Assert.IsTrue(b != new Plane(0, 0, 0, 0));
            Assert.IsTrue(c != new Plane(0, 0, 0, 0));
            Assert.IsTrue(d != new Plane(0, 0, 0, 0));

            Assert.IsFalse(a.Equals(new Plane(0, 0, 0, 0)));
            Assert.IsFalse(b.Equals(new Plane(0, 0, 0, 0)));
            Assert.IsFalse(c.Equals(new Plane(0, 0, 0, 0)));
            Assert.IsFalse(d.Equals(new Plane(0, 0, 0, 0)));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert.IsFalse(a.Equals(a));
            Assert.IsFalse(b.Equals(b));
            Assert.IsFalse(c.Equals(c));
            Assert.IsFalse(d.Equals(d));
        }

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        [TestMethod]
        public unsafe void PlaneSizeofTest()
        {
            Assert.AreEqual(16, sizeof(Plane));
            Assert.AreEqual(32, sizeof(Plane_2x));
            Assert.AreEqual(20, sizeof(PlanePlusFloat));
            Assert.AreEqual(40, sizeof(PlanePlusFloat_2x));
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Plane_2x
        {
            Plane a;
            Plane b;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct PlanePlusFloat
        {
            Plane v;
            float f;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct PlanePlusFloat_2x
        {
            PlanePlusFloat a;
            PlanePlusFloat b;
        }

        // A test to make sure the fields are laid out how we expect
        [TestMethod]
        public unsafe void PlaneFieldOffsetTest()
        {
            Plane value;

            Assert.AreEqual(0 + new IntPtr(&value).ToInt64(), new IntPtr(&value.Normal).ToInt64());
            Assert.AreEqual(12 + new IntPtr(&value).ToInt64(), new IntPtr(&value.D).ToInt64());
        }

        // A test to validate interop between .NET (System.Numerics) and WinRT (Microsoft.Graphics.Canvas.Numerics)
        [TestMethod]
        public void PlaneWinRTInteropTest()
        {
            Plane a = new Plane(23, 42, 100, -1);

            Microsoft.Graphics.Canvas.Numerics.Plane b = a;

            Assert.AreEqual(a.Normal.X, b.Normal.X);
            Assert.AreEqual(a.Normal.Y, b.Normal.Y);
            Assert.AreEqual(a.Normal.Z, b.Normal.Z);
            Assert.AreEqual(a.D, b.D);

            Plane c = b;

            Assert.AreEqual(a, c);
        }
    }
}
