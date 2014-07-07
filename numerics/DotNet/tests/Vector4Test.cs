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
    public class Vector4Test
    {
        // A test for DistanceSquared (Vector4, Vector4)
        [TestMethod]
        public void Vector4DistanceSquaredTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 64.0f;
            float actual;

            actual = Vector4.DistanceSquared(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.DistanceSquared did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        [TestMethod]
        public void Vector4DistanceTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 8.0f;
            float actual;

            actual = Vector4.Distance(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Distance did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        // Distance from the same point
        [TestMethod]
        public void Vector4DistanceTest1()
        {
            Vector4 a = new Vector4(new Vector2(1.051f, 2.05f), 3.478f, 1.0f);
            Vector4 b = new Vector4(new Vector3(1.051f, 2.05f, 3.478f), 0.0f);
            b.W = 1.0f;

            float actual = Vector4.Distance(a, b);
            Assert.AreEqual(0.0f, actual, "Vector4.Distance did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        [TestMethod]
        public void Vector4DotTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = Vector4.Dot(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Dot did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        // Dot test for perpendicular vector
        [TestMethod]
        public void Vector4DotTest1()
        {
            Vector3 a = new Vector3(1.55f, 1.55f, 1);
            Vector3 b = new Vector3(2.5f, 3, 1.5f);
            Vector3 c = Vector3.Cross(a, b);

            Vector4 d = new Vector4(a, 0);
            Vector4 e = new Vector4(c, 0);

            float actual = Vector4.Dot(d, e);
            Assert.IsTrue(MathHelper.Equal(0.0f, actual), "Vector4.Dot did not return the expected value.");
        }

        // A test for Length ()
        [TestMethod]
        public void Vector4LengthTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = new Vector4(a, w);

            float expected = (float)System.Math.Sqrt(30.0f);
            float actual;

            actual = target.Length();

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Length did not return the expected value.");
        }

        // A test for Length ()
        // Length test where length is zero
        [TestMethod]
        public void Vector4LengthTest1()
        {
            Vector4 target = new Vector4();

            float expected = 0.0f;
            float actual = target.Length();

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        [TestMethod]
        public void Vector4LengthSquaredTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = new Vector4(a, w);

            float expected = 30;
            float actual;

            actual = target.LengthSquared();

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector4, Vector4)
        [TestMethod]
        public void Vector4MinTest()
        {
            Vector4 a = new Vector4(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b = new Vector4(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected = new Vector4(-1.0f, 1.0f, -3.0f, 0.0f);
            Vector4 actual;
            actual = Vector4.Min(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Min did not return the expected value.");
        }

        // A test for Max (Vector4, Vector4)
        [TestMethod]
        public void Vector4MaxTest()
        {
            Vector4 a = new Vector4(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b = new Vector4(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected = new Vector4(2.0f, 4.0f, -1.0f, 1000.0f);
            Vector4 actual;
            actual = Vector4.Max(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Max did not return the expected value.");
        }

        [TestMethod]
        public void Vector4MinMaxCodeCoverageTest()
        {
            Vector4 min = Vector4.Zero;
            Vector4 max = Vector4.One;
            Vector4 actual;

            // Min.
            actual = Vector4.Min(min, max);
            Assert.AreEqual(actual, min);

            actual = Vector4.Min(max, min);
            Assert.AreEqual(actual, min);

            // Max.
            actual = Vector4.Max(min, max);
            Assert.AreEqual(actual, max);

            actual = Vector4.Max(max, min);
            Assert.AreEqual(actual, max);
        }

        // A test for Clamp (Vector4, Vector4, Vector4)
        [TestMethod]
        public void Vector4ClampTest()
        {
            Vector4 a = new Vector4(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 min = new Vector4(0.0f, 0.1f, 0.13f, 0.14f);
            Vector4 max = new Vector4(1.0f, 1.1f, 1.13f, 1.14f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector4 expected = new Vector4(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = new Vector4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = max;
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = new Vector4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // Case N4: combination case.
            a = new Vector4(-2.0f, 0.5f, 4.0f, -5.0f);
            expected = new Vector4(min.X, a.Y, max.Z, min.W);
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = new Vector4(0.0f, 0.1f, 0.13f, 0.14f);
            min = new Vector4(1.0f, 1.1f, 1.13f, 1.14f);

            // Case W1: specfied value is in the range.
            a = new Vector4(0.5f, 0.3f, 0.33f, 0.44f);
            expected = min;
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = new Vector4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = min;
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = new Vector4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Clamp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        [TestMethod]
        public void Vector4LerpTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            float t = 0.5f;

            Vector4 expected = new Vector4(3.0f, 4.0f, 5.0f, 6.0f);
            Vector4 actual;

            actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        // Lerp test with factor zero
        [TestMethod]
        public void Vector4LerpTest1()
        {
            Vector4 a = new Vector4(new Vector3(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.0f;
            Vector4 expected = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        // Lerp test with factor one
        [TestMethod]
        public void Vector4LerpTest2()
        {
            Vector4 a = new Vector4(new Vector3(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 1.0f;
            Vector4 expected = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        // Lerp test with factor > 1
        [TestMethod]
        public void Vector4LerpTest3()
        {
            Vector4 a = new Vector4(new Vector3(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 2.0f;
            Vector4 expected = new Vector4(8.0f, 10.0f, 12.0f, 14.0f);
            Vector4 actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        // Lerp test with factor < 0
        [TestMethod]
        public void Vector4LerpTest4()
        {
            Vector4 a = new Vector4(new Vector3(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);

            float t = -2.0f;
            Vector4 expected = -(b * 2);
            Vector4 actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        // Lerp test from the same point
        [TestMethod]
        public void Vector4LerpTest5()
        {
            Vector4 a = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 b = new Vector4(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.85f;
            Vector4 expected = a;
            Vector4 actual = Vector4.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Lerp did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        [TestMethod]
        public void Vector4TransformTest1()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = new Vector4(10.316987f, 22.183012f, 30.3660259f, 1.0f);
            Vector4 actual;

            actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        [TestMethod]
        public void Vector4TransformTest2()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = new Vector4(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            Vector4 actual;

            actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        [TestMethod]
        public void Vector4TransformVector4Test()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = new Vector4(2.19198728f, 1.53349376f, 2.61602545f, 0.0f);
            Vector4 actual;

            actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");

            // 
            v.W = 1.0f;

            expected = new Vector4(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        // Transform vector4 with zero matrix
        [TestMethod]
        public void Vector4TransformVector4Test1()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m = new Matrix4x4();
            Vector4 expected = new Vector4(0, 0, 0, 0);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        // Transform vector4 with identity matrix
        [TestMethod]
        public void Vector4TransformVector4Test2()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m = Matrix4x4.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        // Transform Vector3 test
        [TestMethod]
        public void Vector4TransformVector3Test()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = Vector4.Transform(new Vector4(v, 1.0f), m);
            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        // Transform vector3 with zero matrix
        [TestMethod]
        public void Vector4TransformVector3Test1()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Matrix4x4 m = new Matrix4x4();
            Vector4 expected = new Vector4(0, 0, 0, 0);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        // Transform vector3 with identity matrix
        [TestMethod]
        public void Vector4TransformVector3Test2()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Matrix4x4 m = Matrix4x4.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        // Transform Vector2 test
        [TestMethod]
        public void Vector4TransformVector2Test()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = Vector4.Transform(new Vector4(v, 0.0f, 1.0f), m);
            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        // Transform Vector2 with zero matrix
        [TestMethod]
        public void Vector4TransformVector2Test1()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);
            Matrix4x4 m = new Matrix4x4();
            Vector4 expected = new Vector4(0, 0, 0, 0);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        // Transform vector2 with identity matrix
        [TestMethod]
        public void Vector4TransformVector2Test2()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);
            Matrix4x4 m = Matrix4x4.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        [TestMethod]
        public void Vector4TransformVector2QuatanionTest()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));

            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector4 expected = Vector4.Transform(v, m);
            Vector4 actual;

            actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        [TestMethod]
        public void Vector4TransformVector3Quaternion()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector4 expected = Vector4.Transform(v, m);
            Vector4 actual;

            actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        [TestMethod]
        public void Vector4TransformVector4QuaternionTest()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector4 expected = Vector4.Transform(v, m);
            Vector4 actual;

            actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");

            // 
            v.W = 1.0f;
            expected.W = 1.0f;
            actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        // Transform vector4 with zero quaternion
        [TestMethod]
        public void Vector4TransformVector4QuaternionTest1()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q = new Quaternion();
            Vector4 expected = v;

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        // Transform vector4 with identity matrix
        [TestMethod]
        public void Vector4TransformVector4QuaternionTest2()
        {
            Vector4 v = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q = Quaternion.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        // Transform Vector3 test
        [TestMethod]
        public void Vector4TransformVector3QuaternionTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector4 expected = Vector4.Transform(v, m);
            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        // Transform vector3 with zero quaternion
        [TestMethod]
        public void Vector4TransformVector3QuaternionTest1()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Quaternion q = new Quaternion();
            Vector4 expected = new Vector4(v, 1.0f);

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        // Transform vector3 with identity quaternion
        [TestMethod]
        public void Vector4TransformVector3QuaternionTest2()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Quaternion q = Quaternion.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        // Transform Vector2 by quaternion test
        [TestMethod]
        public void Vector4TransformVector2QuaternionTest()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector4 expected = Vector4.Transform(v, m);
            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        // Transform Vector2 with zero quaternion
        [TestMethod]
        public void Vector4TransformVector2QuaternionTest1()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);
            Quaternion q = new Quaternion();
            Vector4 expected = new Vector4(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        // Transform vector2 with identity Quaternion
        [TestMethod]
        public void Vector4TransformVector2QuaternionTest2()
        {
            Vector2 v = new Vector2(1.0f, 2.0f);
            Quaternion q = Quaternion.Identity;
            Vector4 expected = new Vector4(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Transform did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        [TestMethod]
        public void Vector4NormalizeTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected = new Vector4(
                0.1825741858350553711523232609336f,
                0.3651483716701107423046465218672f,
                0.5477225575051661134569697828008f,
                0.7302967433402214846092930437344f);
            Vector4 actual;

            actual = Vector4.Normalize(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        // Normalize vector of length one
        [TestMethod]
        public void Vector4NormalizeTest1()
        {
            Vector4 a = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4.Normalize(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        // Normalize vector of length zero
        [TestMethod]
        public void Vector4NormalizeTest2()
        {
            Vector4 a = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4.Normalize(a);
            Assert.IsTrue(float.IsNaN(actual.X) && float.IsNaN(actual.Y) && float.IsNaN(actual.Z) && float.IsNaN(actual.W), "Vector4.Normalize did not return the expected value.");
        }

        // A test for operator - (Vector4)
        [TestMethod]
        public void Vector4UnaryNegationTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected = new Vector4(-1.0f, -2.0f, -3.0f, -4.0f);
            Vector4 actual;

            actual = -a;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator - did not return the expected value.");
        }

        // A test for operator - (Vector4, Vector4)
        [TestMethod]
        public void Vector4SubtractionTest()
        {
            Vector4 a = new Vector4(1.0f, 6.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 2.0f, 3.0f, 9.0f);

            Vector4 expected = new Vector4(-4.0f, 4.0f, 0.0f, -5.0f);
            Vector4 actual;

            actual = a - b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator - did not return the expected value.");
        }

        // A test for operator * (Vector4, float)
        [TestMethod]
        public void Vector4MultiplyTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;

            Vector4 expected = new Vector4(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual;

            actual = a * factor;
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator * did not return the expected value.");
        }

        // A test for operator * (float, Vector4)
        [TestMethod]
        public void Vector4MultiplyTest4()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;
            Vector4 expected = new Vector4(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual;

            actual = factor * a;
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator * did not return the expected value.");
        }

        // A test for operator * (Vector4, Vector4)
        [TestMethod]
        public void Vector4MultiplyTest1()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = new Vector4(5.0f, 12.0f, 21.0f, 32.0f);
            Vector4 actual;

            actual = a * b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator * did not return the expected value.");
        }

        // A test for operator / (Vector4, float)
        [TestMethod]
        public void Vector4DivisionTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            float div = 2.0f;

            Vector4 expected = new Vector4(0.5f, 1.0f, 1.5f, 2.0f);
            Vector4 actual;

            actual = a / div;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        [TestMethod]
        public void Vector4DivisionTest1()
        {
            Vector4 a = new Vector4(1.0f, 6.0f, 7.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 2.0f, 3.0f, 8.0f);

            Vector4 expected = new Vector4(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            Vector4 actual;

            actual = a / b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        // Divide by zero
        [TestMethod]
        public void Vector4DivisionTest2()
        {
            Vector4 a = new Vector4(-2.0f, 3.0f, float.MaxValue, float.NaN);

            float div = 0.0f;

            Vector4 actual = a / div;

            Assert.IsTrue(float.IsNegativeInfinity(actual.X), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(actual.Y), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(actual.Z), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNaN(actual.W), "Vector4.operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        // Divide by zero
        [TestMethod]
        public void Vector4DivisionTest3()
        {
            Vector4 a = new Vector4(0.047f, -3.0f, float.NegativeInfinity, float.MinValue);
            Vector4 b = new Vector4();

            Vector4 actual = a / b;

            Assert.IsTrue(float.IsPositiveInfinity(actual.X), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNegativeInfinity(actual.Y), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNegativeInfinity(actual.Z), "Vector4.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNegativeInfinity(actual.W), "Vector4.operator / did not return the expected value.");
        }

        // A test for operator + (Vector4, Vector4)
        [TestMethod]
        public void Vector4AdditionTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = new Vector4(6.0f, 8.0f, 10.0f, 12.0f);
            Vector4 actual;

            actual = a + b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector4.operator + did not return the expected value.");
        }

        // A test for Vector4 (float, float, float, float)
        [TestMethod]
        public void Vector4ConstructorTest()
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target = new Vector4(x, y, z, w);

            Assert.IsTrue(MathHelper.Equal(target.X, x) && MathHelper.Equal(target.Y, y) && MathHelper.Equal(target.Z, z) && MathHelper.Equal(target.W, w),
                "Vector4 constructor(x,y,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector2, float, float)
        [TestMethod]
        public void Vector4ConstructorTest1()
        {
            Vector2 a = new Vector2(1.0f, 2.0f);
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target = new Vector4(a, z, w);
            Assert.IsTrue(MathHelper.Equal(target.X, a.X) && MathHelper.Equal(target.Y, a.Y) && MathHelper.Equal(target.Z, z) && MathHelper.Equal(target.W, w),
                "Vector4 constructor(Vector2,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector3, float)
        [TestMethod]
        public void Vector4ConstructorTest2()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = new Vector4(a, w);

            Assert.IsTrue(MathHelper.Equal(target.X, a.X) && MathHelper.Equal(target.Y, a.Y) && MathHelper.Equal(target.Z, a.Z) && MathHelper.Equal(target.W, w),
                "Vector4 constructor(Vector3,w) did not return the expected value.");
        }

        // A test for Vector4 ()
        // Constructor with no parameter
        [TestMethod]
        public void Vector4ConstructorTest4()
        {
            Vector4 a = new Vector4();

            Assert.AreEqual(a.X, 0.0f, "Vector4.constructor () did not return the expected value.");
            Assert.AreEqual(a.Y, 0.0f, "Vector4.constructor () did not return the expected value.");
            Assert.AreEqual(a.Z, 0.0f, "Vector4.constructor () did not return the expected value.");
            Assert.AreEqual(a.W, 0.0f, "Vector4.constructor () did not return the expected value.");
        }

        // A test for Vector4 ()
        // Constructor with special floating values
        [TestMethod]
        public void Vector4ConstructorTest5()
        {
            Vector4 target = new Vector4(float.NaN, float.MaxValue, float.PositiveInfinity, float.Epsilon);

            Assert.IsTrue(float.IsNaN(target.X), "Vector4.constructor (float, float, float, float) did not return the expected value.");
            Assert.IsTrue(float.Equals(float.MaxValue, target.Y), "Vector4.constructor (float, float, float, float) did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(target.Z), "Vector4.constructor (float, float, float, float) did not return the expected value.");
            Assert.IsTrue(float.Equals(float.Epsilon, target.W), "Vector4.constructor (float, float, float, float) did not return the expected value.");
        }

        // A test for ToString ()
        // ToString test for Vector4
        [TestMethod]
        public void Vector4ToStringTest()
        {
            Vector4 target = new Vector4(-1.0f, 2.2f, 3.3f, -4.4f);

            string expected = "{X:-1 Y:2.2 Z:3.3 W:-4.4}";
            string actual;
            actual = target.ToString();
            Assert.AreEqual(expected, actual, "Vector4.ToString did not return the expected value.");
        }

        // A test for Add (Vector4, Vector4)
        [TestMethod]
        public void Vector4AddTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = new Vector4(6.0f, 8.0f, 10.0f, 12.0f);
            Vector4 actual;

            actual = Vector4.Add(a, b);
            Assert.AreEqual(expected, actual, "Vector4.Add did not return the expected value.");
        }

        // A test for Divide (Vector4, float)
        [TestMethod]
        public void Vector4DivideTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            float div = 2.0f;
            Vector4 expected = new Vector4(0.5f, 1.0f, 1.5f, 2.0f);
            Vector4 actual;
            actual = Vector4.Divide(a, div);
            Assert.AreEqual(expected, actual, "Vector4.Divide did not return the expected value.");
        }

        // A test for Divide (Vector4, Vector4)
        [TestMethod]
        public void Vector4DivideTest1()
        {
            Vector4 a = new Vector4(1.0f, 6.0f, 7.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 2.0f, 3.0f, 8.0f);

            Vector4 expected = new Vector4(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            Vector4 actual;

            actual = Vector4.Divide(a, b);
            Assert.AreEqual(expected, actual, "Vector4.Divide did not return the expected value.");
        }

        // A test for Equals (object)
        [TestMethod]
        public void Vector4EqualsTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            object obj = b;

            bool expected = true;
            bool actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            obj = b;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");

            // case 3: compare between different types.
            obj = new Quaternion();
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");

            // case 3: compare against null.
            obj = null;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");
        }

        // A test for GetHashCode ()
        [TestMethod]
        public void Vector4GetHashCodeTest()
        {
            Vector4 target = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            int expected = target.X.GetHashCode() + target.Y.GetHashCode() + target.Z.GetHashCode() + target.W.GetHashCode();
            int actual;

            actual = target.GetHashCode();

            Assert.AreEqual(expected, actual, "Vector4.GetHashCode did not return the expected value.");
        }

        // A test for Multiply (Vector4, float)
        [TestMethod]
        public void Vector4MultiplyTest2()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 2.0f;
            Vector4 expected = new Vector4(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual = Vector4.Multiply(a, factor);
            Assert.AreEqual(expected, actual, "Vector4.Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector4, Vector4)
        [TestMethod]
        public void Vector4MultiplyTest3()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = new Vector4(5.0f, 12.0f, 21.0f, 32.0f);
            Vector4 actual;

            actual = Vector4.Multiply(a, b);
            Assert.AreEqual(expected, actual, "Vector4.Multiply did not return the expected value.");
        }

        // A test for Negate (Vector4)
        [TestMethod]
        public void Vector4NegateTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected = new Vector4(-1.0f, -2.0f, -3.0f, -4.0f);
            Vector4 actual;

            actual = Vector4.Negate(a);
            Assert.AreEqual(expected, actual, "Vector4.Negate did not return the expected value.");
        }

        // A test for operator != (Vector4, Vector4)
        [TestMethod]
        public void Vector4InequalityTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert.AreEqual(expected, actual, "Vector4.operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert.AreEqual(expected, actual, "Vector4.operator != did not return the expected value.");
        }

        // A test for operator == (Vector4, Vector4)
        [TestMethod]
        public void Vector4EqualityTest()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert.AreEqual(expected, actual, "Vector4.operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert.AreEqual(expected, actual, "Vector4.operator == did not return the expected value.");
        }

        // A test for Subtract (Vector4, Vector4)
        [TestMethod]
        public void Vector4SubtractTest()
        {
            Vector4 a = new Vector4(1.0f, 6.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(5.0f, 2.0f, 3.0f, 9.0f);

            Vector4 expected = new Vector4(-4.0f, 4.0f, 0.0f, -5.0f);
            Vector4 actual;

            actual = Vector4.Subtract(a, b);

            Assert.AreEqual(expected, actual, "Vector4.Subtract did not return the expected value.");
        }

        // A test for UnitW
        [TestMethod]
        public void Vector4UnitWTest()
        {
            Vector4 val = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            Assert.AreEqual(val, Vector4.UnitW, "Vector4.UnitW was not set correctly.");
        }

        // A test for UnitX
        [TestMethod]
        public void Vector4UnitXTest()
        {
            Vector4 val = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
            Assert.AreEqual(val, Vector4.UnitX, "Vector4.UnitX was not set correctly.");
        }

        // A test for UnitY
        [TestMethod]
        public void Vector4UnitYTest()
        {
            Vector4 val = new Vector4(0.0f, 1.0f, 0.0f, 0.0f);
            Assert.AreEqual(val, Vector4.UnitY, "Vector4.UnitY was not set correctly.");
        }

        // A test for UnitZ
        [TestMethod]
        public void Vector4UnitZTest()
        {
            Vector4 val = new Vector4(0.0f, 0.0f, 1.0f, 0.0f);
            Assert.AreEqual(val, Vector4.UnitZ, "Vector4.UnitZ was not set correctly.");
        }

        // A test for One
        [TestMethod]
        public void Vector4OneTest()
        {
            Vector4 val = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            Assert.AreEqual(val, Vector4.One, "Vector4.One was not set correctly.");
        }

        // A test for Zero
        [TestMethod]
        public void Vector4ZeroTest()
        {
            Vector4 val = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            Assert.AreEqual(val, Vector4.Zero, "Vector4.Zero was not set correctly.");
        }

        // A test for Equals (Vector4)
        [TestMethod]
        public void Vector4EqualsTest1()
        {
            Vector4 a = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = new Vector4(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Vector4.Equals did not return the expected value.");
        }

        // A test for Vector4 (float)
        [TestMethod]
        public void Vector4ConstructorTest6()
        {
            float value = 1.0f;
            Vector4 target = new Vector4(value);

            Vector4 expected = new Vector4(value, value, value, value);
            Assert.AreEqual(expected, target, "Vector4.cstr did not return the expected value.");

            value = 2.0f;
            target = new Vector4(value);
            expected = new Vector4(value, value, value, value);
            Assert.AreEqual(expected, target, "Vector4.cstr did not return the expected value.");
        }

        // A test for Vector4 comparison involving NaN values
        [TestMethod]
        public void Vector4EqualsNanTest()
        {
            Vector4 a = new Vector4(float.NaN, 0, 0, 0);
            Vector4 b = new Vector4(0, float.NaN, 0, 0);
            Vector4 c = new Vector4(0, 0, float.NaN, 0);
            Vector4 d = new Vector4(0, 0, 0, float.NaN);

            Assert.IsFalse(a == Vector4.Zero);
            Assert.IsFalse(b == Vector4.Zero);
            Assert.IsFalse(c == Vector4.Zero);
            Assert.IsFalse(d == Vector4.Zero);

            Assert.IsTrue(a != Vector4.Zero);
            Assert.IsTrue(b != Vector4.Zero);
            Assert.IsTrue(c != Vector4.Zero);
            Assert.IsTrue(d != Vector4.Zero);

            Assert.IsFalse(a.Equals(Vector4.Zero));
            Assert.IsFalse(b.Equals(Vector4.Zero));
            Assert.IsFalse(c.Equals(Vector4.Zero));
            Assert.IsFalse(d.Equals(Vector4.Zero));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert.IsFalse(a.Equals(a));
            Assert.IsFalse(b.Equals(b));
            Assert.IsFalse(c.Equals(c));
            Assert.IsFalse(d.Equals(d));
        }

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        [TestMethod]
        public unsafe void Vector4SizeofTest()
        {
            Assert.AreEqual(16, sizeof(Vector4));
            Assert.AreEqual(32, sizeof(Vector4_2x));
            Assert.AreEqual(20, sizeof(Vector4PlusFloat));
            Assert.AreEqual(40, sizeof(Vector4PlusFloat_2x));
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector4_2x
        {
            Vector4 a;
            Vector4 b;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector4PlusFloat
        {
            Vector4 v;
            float f;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector4PlusFloat_2x
        {
            Vector4PlusFloat a;
            Vector4PlusFloat b;
        }

        // A test to make sure the fields are laid out how we expect
        [TestMethod]
        public unsafe void Vector4FieldOffsetTest()
        {
            Vector4* ptr = (Vector4*)0;

            Assert.AreEqual(new IntPtr(0), new IntPtr(&ptr->X));
            Assert.AreEqual(new IntPtr(4), new IntPtr(&ptr->Y));
            Assert.AreEqual(new IntPtr(8), new IntPtr(&ptr->Z));
            Assert.AreEqual(new IntPtr(12), new IntPtr(&ptr->W));
        }

        // A test to validate interop between .NET (System.Numerics) and WinRT (Microsoft.Graphics.Canvas.Numerics)
        [TestMethod]
        public void Vector4WinRTInteropTest()
        {
            Vector4 a = new Vector4(23, 42, 100, -1);

            Microsoft.Graphics.Canvas.Numerics.Vector4 b = a;

            Assert.AreEqual(a.X, b.X);
            Assert.AreEqual(a.Y, b.Y);
            Assert.AreEqual(a.Z, b.Z);
            Assert.AreEqual(a.W, b.W);

            Vector4 c = b;

            Assert.AreEqual(a, c);
        }
    }
}
