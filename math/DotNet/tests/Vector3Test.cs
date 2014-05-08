//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Runtime.InteropServices;
using Windows.Math;

#if NO_WINRT
using Microsoft.VisualStudio.TestTools.UnitTesting;
#else
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
#endif

namespace MathTests
{
    [TestClass()]
    public class Vector3Test
    {
        /// <summary>
        ///A test for Cross (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3CrossTest()
        {
            Vector3 a = new Vector3(1.0f, 0.0f, 0.0f);
            Vector3 b = new Vector3(0.0f, 1.0f, 0.0f);

            Vector3 expected = new Vector3(0.0f, 0.0f, 1.0f);
            Vector3 actual;

            actual = Vector3.Cross(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Cross did not return the expected value.");
        }

        /// <summary>
        ///A test for Cross (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Cross test of the same vector")]
        public void Vector3CrossTest1()
        {
            Vector3 a = new Vector3(0.0f, 1.0f, 0.0f);
            Vector3 b = new Vector3(0.0f, 1.0f, 0.0f);

            Vector3 expected = new Vector3(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3.Cross(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Cross did not return the expected value.");
        }

        /// <summary>
        ///A test for Distance (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3DistanceTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float expected = (float)System.Math.Sqrt(27);
            float actual;

            actual = Vector3.Distance(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Distance did not return the expected value.");
        }

        /// <summary>
        ///A test for Distance (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Distance from the same point")]
        public void Vector3DistanceTest1()
        {
            Vector3 a = new Vector3(1.051f, 2.05f, 3.478f);
            Vector3 b = new Vector3(new Vector2(1.051f, 0.0f), 1);
            b.Y = 2.05f;
            b.Z = 3.478f;

            float actual = Vector3.Distance(a, b);
            Assert.AreEqual(0.0f, actual, "Vector3.Distance did not return the expected value.");
        }

        /// <summary>
        ///A test for DistanceSquared (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("A test for DistanceSquared (Vector3, Vector3)")]
        public void Vector3DistanceSquaredTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float expected = 27.0f;
            float actual;

            actual = Vector3.DistanceSquared(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.DistanceSquared did not return the expected value.");
        }

        /// <summary>
        ///A test for Dot (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3DotTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float expected = 32.0f;
            float actual;

            actual = Vector3.Dot(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Dot did not return the expected value.");
        }

        /// <summary>
        ///A test for Dot (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Dot test for perpendicular vector")]
        public void Vector3DotTest1()
        {
            Vector3 a = new Vector3(1.55f, 1.55f, 1);
            Vector3 b = new Vector3(2.5f, 3, 1.5f);
            Vector3 c = Vector3.Cross(a, b);

            float expected = 0.0f;
            float actual1 = Vector3.Dot(a, c);
            float actual2 = Vector3.Dot(b, c);
            Assert.IsTrue(MathHelper.Equal(expected, actual1), "Vector3.Dot did not return the expected value.");
            Assert.IsTrue(MathHelper.Equal(expected, actual2), "Vector3.Dot did not return the expected value.");
        }

        /// <summary>
        ///A test for Length ()
        ///</summary>
        [TestMethod]
        public void Vector3LengthTest()
        {
            Vector2 a = new Vector2(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = new Vector3(a, z);

            float expected = (float)System.Math.Sqrt(14.0f);
            float actual;

            actual = target.Length();
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Length did not return the expected value.");
        }

        /// <summary>
        ///A test for Length ()
        ///</summary>
        [TestMethod]
        [Description("Length test where length is zero")]
        public void Vector3LengthTest1()
        {
            Vector3 target = new Vector3();

            float expected = 0.0f;
            float actual = target.Length();
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Length did not return the expected value.");
        }

        /// <summary>
        ///A test for LengthSquared ()
        ///</summary>
        [TestMethod]
        public void Vector3LengthSquaredTest()
        {
            Vector2 a = new Vector2(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = new Vector3(a, z);

            float expected = 14.0f;
            float actual;

            actual = target.LengthSquared();
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.LengthSquared did not return the expected value.");
        }

        /// <summary>
        ///A test for Min (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3MinTest()
        {
            Vector3 a = new Vector3(-1.0f, 4.0f, -3.0f);
            Vector3 b = new Vector3(2.0f, 1.0f, -1.0f);

            Vector3 expected = new Vector3(-1.0f, 1.0f, -3.0f);
            Vector3 actual;
            actual = Vector3.Min(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Min did not return the expected value.");
        }

        /// <summary>
        ///A test for Max (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3MaxTest()
        {
            Vector3 a = new Vector3(-1.0f, 4.0f, -3.0f);
            Vector3 b = new Vector3(2.0f, 1.0f, -1.0f);

            Vector3 expected = new Vector3(2.0f, 4.0f, -1.0f);
            Vector3 actual;
            actual = Vector3.Max(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "vector3.Max did not return the expected value.");
        }

        [TestMethod]
        public void Vector3MinMaxCodeCoverageTest()
        {
            Vector3 min = Vector3.Zero;
            Vector3 max = Vector3.One;
            Vector3 actual;

            // Min.
            actual = Vector3.Min(min, max);
            Assert.AreEqual(actual, min);

            actual = Vector3.Min(max, min);
            Assert.AreEqual(actual, min);

            // Max.
            actual = Vector3.Max(min, max);
            Assert.AreEqual(actual, max);

            actual = Vector3.Max(max, min);
            Assert.AreEqual(actual, max);
        }

        #region Lerp Tests

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        public void Vector3LerpTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float t = 0.5f;

            Vector3 expected = new Vector3(2.5f, 3.5f, 4.5f);
            Vector3 actual;

            actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        [Description("Lerp test with factor zero")]
        public void Vector3LerpTest1()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float t = 0.0f;
            Vector3 expected = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        [Description("Lerp test with factor one")]
        public void Vector3LerpTest2()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float t = 1.0f;
            Vector3 expected = new Vector3(4.0f, 5.0f, 6.0f);
            Vector3 actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        [Description("Lerp test with factor > 1")]
        public void Vector3LerpTest3()
        {
            Vector3 a = new Vector3(0.0f, 0.0f, 0.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float t = 2.0f;
            Vector3 expected = new Vector3(8.0f, 10.0f, 12.0f);
            Vector3 actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        [Description("Lerp test with factor < 0")]
        public void Vector3LerpTest4()
        {
            Vector3 a = new Vector3(0.0f, 0.0f, 0.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            float t = -2.0f;
            Vector3 expected = new Vector3(-8.0f, -10.0f, -12.0f);
            Vector3 actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        /// <summary>
        ///A test for Lerp (Vector3, Vector3, float)
        ///</summary>
        [TestMethod]
        [Description("Lerp test from the same point")]
        public void Vector3LerpTest5()
        {
            Vector3 a = new Vector3(1.68f, 2.34f, 5.43f);
            Vector3 b = a;

            float t = 0.18f;
            Vector3 expected = new Vector3(1.68f, 2.34f, 5.43f);
            Vector3 actual = Vector3.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Lerp did not return the expected value.");
        }

        #endregion

        #region Reflect Tests

        /// <summary>
        ///A test for Reflect (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3ReflectTest()
        {
            Vector3 a = Vector3.Normalize(new Vector3(1.0f, 1.0f, 1.0f));

            // Reflect on XZ plane.
            Vector3 n = new Vector3(0.0f, 1.0f, 0.0f);
            Vector3 expected = new Vector3(a.X, -a.Y, a.Z);
            Vector3 actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");

            // Reflect on XY plane.
            n = new Vector3(0.0f, 0.0f, 1.0f);
            expected = new Vector3(a.X, a.Y, -a.Z);
            actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = new Vector3(1.0f, 0.0f, 0.0f);
            expected = new Vector3(-a.X, a.Y, a.Z);
            actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");
        }

        /// <summary>
        ///A test for Reflect (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Reflection when normal and source are the same")]
        public void Vector3ReflectTest1()
        {
            Vector3 n = new Vector3(0.45f, 1.28f, 0.86f);
            n = Vector3.Normalize(n);
            Vector3 a = n;

            Vector3 expected = -n;
            Vector3 actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");
        }

        /// <summary>
        ///A test for Reflect (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Reflection when normal and source are negation")]
        public void Vector3ReflectTest2()
        {
            Vector3 n = new Vector3(0.45f, 1.28f, 0.86f);
            n = Vector3.Normalize(n);
            Vector3 a = -n;

            Vector3 expected = n;
            Vector3 actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");
        }

        /// <summary>
        ///A test for Reflect (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Reflection when normal and source are perpendicular (a dot n = 0)")]
        public void Vector3ReflectTest3()
        {
            Vector3 n = new Vector3(0.45f, 1.28f, 0.86f);
            Vector3 temp = new Vector3(1.28f, 0.45f, 0.01f);
            // find a perpendicular vector of n
            Vector3 a = Vector3.Cross(temp, n);

            Vector3 expected = a;
            Vector3 actual = Vector3.Reflect(a, n);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Reflect did not return the expected value.");
        }

        #endregion

        /// <summary>
        ///A test for Transform(Vector3, Matrix4x4)
        ///</summary>
        [TestMethod]
        public void Vector3TransformTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector3 expected = new Vector3(12.191987f, 21.533493f, 32.616024f);
            Vector3 actual;

            actual = Vector3.Transform(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Transform did not return the expected value.");
        }

        /// <summary>
        ///A test for Clamp (Vector3, Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3ClampTest()
        {
            Vector3 a = new Vector3(0.5f, 0.3f, 0.33f);
            Vector3 min = new Vector3(0.0f, 0.1f, 0.13f);
            Vector3 max = new Vector3(1.0f, 1.1f, 1.13f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector3 expected = new Vector3(0.5f, 0.3f, 0.33f);
            Vector3 actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = new Vector3(2.0f, 3.0f, 4.0f);
            expected = max;
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = new Vector3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // Case N4: combination case.
            a = new Vector3(-2.0f, 0.5f, 4.0f);
            expected = new Vector3(min.X, a.Y, max.Z);
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = new Vector3(0.0f, 0.1f, 0.13f);
            min = new Vector3(1.0f, 1.1f, 1.13f);

            // Case W1: specfied value is in the range.
            a = new Vector3(0.5f, 0.3f, 0.33f);
            expected = min;
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = new Vector3(2.0f, 3.0f, 4.0f);
            expected = min;
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = new Vector3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3.Clamp(a, min, max);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Clamp did not return the expected value.");
        }

        /// <summary>
        ///A test for TransformNormal (Vector3, Matrix4x4)
        ///</summary>
        [TestMethod]
        public void Vector3TransformNormalTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector3 expected = new Vector3(2.19198728f, 1.53349364f, 2.61602545f);
            Vector3 actual;

            actual = Vector3.TransformNormal(v, m);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.TransformNormal did not return the expected value.");
        }

        #region Transform vector by Quaternion

        /// <summary>
        ///A test for Transform (Vector3, Quaternion)
        ///</summary>
        [TestMethod]
        [Description("Transform Vector3 test")]
        public void Vector3TransformByQuaternionTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4.CreateRotationX(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationY(MathHelper.ToRadians(30.0f)) *
                Matrix4x4.CreateRotationZ(MathHelper.ToRadians(30.0f));
            Quaternion q = Quaternion.CreateFromRotationMatrix(m);

            Vector3 expected = Vector3.Transform(v, m);
            Vector3 actual = Vector3.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Transform did not return the expected value.");
        }

        /// <summary>
        ///A test for Transform (Vector3, Quaternion)
        ///</summary>
        [TestMethod]
        [Description("Transform vector3 with zero quaternion")]
        public void Vector3TransformByQuaternionTest1()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Quaternion q = new Quaternion();
            Vector3 expected = v;

            Vector3 actual = Vector3.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Transform did not return the expected value.");
        }

        /// <summary>
        ///A test for Transform (Vector3, Quaternion)
        ///</summary>
        [TestMethod]
        [Description("Transform vector3 with identity quaternion")]
        public void Vector3TransformByQuaternionTest2()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            Quaternion q = Quaternion.Identity;
            Vector3 expected = v;

            Vector3 actual = Vector3.Transform(v, q);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Transform did not return the expected value.");
        }

        #endregion

        /// <summary>
        ///A test for Normalize (Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3NormalizeTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            Vector3 expected = new Vector3(
                0.26726124191242438468455348087975f,
                0.53452248382484876936910696175951f,
                0.80178372573727315405366044263926f);
            Vector3 actual;

            actual = Vector3.Normalize(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Normalize did not return the expected value.");
        }

        /// <summary>
        ///A test for Normalize (Vector3)
        ///</summary>
        [TestMethod]
        [Description("Normalize vector of length one")]
        public void Vector3NormalizeTest1()
        {
            Vector3 a = new Vector3(1.0f, 0.0f, 0.0f);

            Vector3 expected = new Vector3(1.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3.Normalize(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.Normalize did not return the expected value.");
        }

        /// <summary>
        ///A test for Normalize (Vector3)
        ///</summary>
        [TestMethod]
        [Description("Normalize vector of length zero")]
        public void Vector3NormalizeTest2()
        {
            Vector3 a = new Vector3(0.0f, 0.0f, 0.0f);

            Vector3 expected = new Vector3(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3.Normalize(a);
            Assert.IsTrue(float.IsNaN(actual.X) && float.IsNaN(actual.Y) && float.IsNaN(actual.Z), "Vector3.Normalize did not return the expected value.");
        }

        /// <summary>
        ///A test for operator - (Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3UnaryNegationTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            Vector3 expected = new Vector3(-1.0f, -2.0f, -3.0f);
            Vector3 actual;

            actual = -a;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator - did not return the expected value.");
        }

        /// <summary>
        ///A test for operator - (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3SubtractionTest()
        {
            Vector3 a = new Vector3(4.0f, 2.0f, 3.0f);

            Vector3 b = new Vector3(1.0f, 5.0f, 7.0f);

            Vector3 expected = new Vector3(3.0f, -3.0f, -4.0f);
            Vector3 actual;

            actual = a - b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator - did not return the expected value.");
        }

        /// <summary>
        ///A test for operator * (Vector3, float)
        ///</summary>
        [TestMethod]
        public void Vector3MultiplyTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            float factor = 2.0f;

            Vector3 expected = new Vector3(2.0f, 4.0f, 6.0f);
            Vector3 actual;

            actual = a * factor;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator * did not return the expected value.");
        }

        /// <summary>
        ///A test for operator * (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3MultiplyTest1()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            Vector3 expected = new Vector3(4.0f, 10.0f, 18.0f);
            Vector3 actual;

            actual = a * b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator * did not return the expected value.");
        }

        /// <summary>
        ///A test for operator / (Vector3, float)
        ///</summary>
        [TestMethod]
        public void Vector3DivisionTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            float div = 2.0f;

            Vector3 expected = new Vector3(0.5f, 1.0f, 1.5f);
            Vector3 actual;

            actual = a / div;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator / did not return the expected value.");
        }

        /// <summary>
        ///A test for operator / (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3DivisionTest1()
        {
            Vector3 a = new Vector3(4.0f, 2.0f, 3.0f);

            Vector3 b = new Vector3(1.0f, 5.0f, 6.0f);

            Vector3 expected = new Vector3(4.0f, 0.4f, 0.5f);
            Vector3 actual;

            actual = a / b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator / did not return the expected value.");
        }

        /// <summary>
        ///A test for operator / (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        [Description("Divide by zero")]
        public void Vector3DivisionTest2()
        {
            Vector3 a = new Vector3(-2.0f, 3.0f, float.MaxValue);

            float div = 0.0f;

            Vector3 actual = a / div;

            Assert.IsTrue(float.IsNegativeInfinity(actual.X), "Vector3.operator / did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(actual.Y), "Vector3.operator / did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(actual.Z), "Vector3.operator / did not return the expected value.");

        }

        /// <summary>
        ///A test for operator / (Vector2, Vector2)
        ///</summary>
        [TestMethod]
        [Description("Divide by zero")]
        public void Vector3DivisionTest3()
        {
            Vector3 a = new Vector3(0.047f, -3.0f, float.NegativeInfinity);
            Vector3 b = new Vector3();

            Vector3 actual = a / b;

            Assert.IsTrue(float.IsPositiveInfinity(actual.X), "Vector3.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNegativeInfinity(actual.Y), "Vector3.operator / did not return the expected value.");
            Assert.IsTrue(float.IsNegativeInfinity(actual.Z), "Vector3.operator / did not return the expected value.");
        }

        /// <summary>
        ///A test for operator + (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3AdditionTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(4.0f, 5.0f, 6.0f);

            Vector3 expected = new Vector3(5.0f, 7.0f, 9.0f);
            Vector3 actual;

            actual = a + b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Vector3.operator + did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 (float, float, float)
        ///</summary>
        [TestMethod]
        public void Vector3ConstructorTest()
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;

            Vector3 target = new Vector3(x, y, z);
            Assert.IsTrue(MathHelper.Equal(target.X, x) && MathHelper.Equal(target.Y, y) && MathHelper.Equal(target.Z, z), "Vector3.constructor (x,y,z) did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 (Vector2, float)
        ///</summary>
        [TestMethod]
        public void Vector3ConstructorTest1()
        {
            Vector2 a = new Vector2(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = new Vector3(a, z);
            Assert.IsTrue(MathHelper.Equal(target.X, a.X) && MathHelper.Equal(target.Y, a.Y) && MathHelper.Equal(target.Z, z), "Vector3.constructor (Vector2,z) did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 (Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3ConstructorTest2()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            Vector3 target = a;

            Assert.IsTrue(MathHelper.Equal(target.X, 1.0f) && MathHelper.Equal(target.Y, 2.0f) && MathHelper.Equal(target.Z, 3.0f), "Vector3.constructor (Vector3) did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 ()
        ///</summary>
        [TestMethod]
        [Description("Constructor with no parameter")]
        public void Vector3ConstructorTest3()
        {
            Vector3 a = new Vector3();

            Assert.AreEqual(a.X, 0.0f, "Vector3.constructor () did not return the expected value.");
            Assert.AreEqual(a.Y, 0.0f, "Vector3.constructor () did not return the expected value.");
            Assert.AreEqual(a.Z, 0.0f, "Vector3.constructor () did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector2 (float, float)
        ///</summary>
        [TestMethod]
        [Description("Constructor with special floating values")]
        public void Vector3ConstructorTest4()
        {
            Vector3 target = new Vector3(float.NaN, float.MaxValue, float.PositiveInfinity);

            Assert.IsTrue(float.IsNaN(target.X), "Vector3.constructor (Vector3) did not return the expected value.");
            Assert.IsTrue(float.Equals(float.MaxValue, target.Y), "Vector3.constructor (Vector3) did not return the expected value.");
            Assert.IsTrue(float.IsPositiveInfinity(target.Z), "Vector3.constructor (Vector3) did not return the expected value.");
        }

        /// <summary>
        ///A test for ToString ()
        ///</summary>
        [TestMethod]
        [Description("ToString test for vector3")]
        public void Vector3ToStringTest()
        {
            Vector3 target = new Vector3(1.0f, 2.2f, 3.3f);

            string expected = "{X:1 Y:2.2 Z:3.3}";
            string actual;
            actual = target.ToString();
            Assert.AreEqual(expected, actual, "Vector3.ToString did not return the expected value.");
        }

        /// <summary>
        ///A test for Add (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3AddTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(5.0f, 6.0f, 7.0f);

            Vector3 expected = new Vector3(6.0f, 8.0f, 10.0f);
            Vector3 actual;

            actual = Vector3.Add(a, b);
            Assert.AreEqual(expected, actual, "Vector3.Add did not return the expected value.");
        }

        /// <summary>
        ///A test for Divide (Vector3, float)
        ///</summary>
        [TestMethod]
        public void Vector3DivideTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            float div = 2.0f;
            Vector3 expected = new Vector3(0.5f, 1.0f, 1.5f);
            Vector3 actual;
            actual = Vector3.Divide(a, div);
            Assert.AreEqual(expected, actual, "Vector3.Divide did not return the expected value.");
        }

        /// <summary>
        ///A test for Divide (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3DivideTest1()
        {
            Vector3 a = new Vector3(1.0f, 6.0f, 7.0f);
            Vector3 b = new Vector3(5.0f, 2.0f, 3.0f);

            Vector3 expected = new Vector3(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f);
            Vector3 actual;

            actual = Vector3.Divide(a, b);
            Assert.AreEqual(expected, actual, "Vector3.Divide did not return the expected value.");
        }

        /// <summary>
        ///A test for Equals (object)
        ///</summary>
        [TestMethod]
        public void Vector3EqualsTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            object obj = b;

            bool expected = true;
            bool actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            obj = b;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");

            // case 3: compare between different types.
            obj = new Quaternion();
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");

            // case 3: compare against null.
            obj = null;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");
        }

        /// <summary>
        ///A test for GetHashCode ()
        ///</summary>
        [TestMethod]
        public void Vector3GetHashCodeTest()
        {
            Vector3 target = new Vector3(1.0f, 2.0f, 3.0f);

            int expected = target.X.GetHashCode() + target.Y.GetHashCode() + target.Z.GetHashCode();
            int actual;

            actual = target.GetHashCode();

            Assert.AreEqual(expected, actual, "Vector3.GetHashCode did not return the expected value.");
        }

        /// <summary>
        ///A test for Multiply (Vector3, float)
        ///</summary>
        [TestMethod]
        public void Vector3MultiplyTest2()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            float factor = 2.0f;
            Vector3 expected = new Vector3(2.0f, 4.0f, 6.0f);
            Vector3 actual = Vector3.Multiply(a, factor);
            Assert.AreEqual(expected, actual, "Vector3.Multiply did not return the expected value.");
        }

        /// <summary>
        ///A test for Multiply (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3MultiplyTest3()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(5.0f, 6.0f, 7.0f);

            Vector3 expected = new Vector3(5.0f, 12.0f, 21.0f);
            Vector3 actual;

            actual = Vector3.Multiply(a, b);
            Assert.AreEqual(expected, actual, "Vector3.Multiply did not return the expected value.");
        }

        /// <summary>
        ///A test for Negate (Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3NegateTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);

            Vector3 expected = new Vector3(-1.0f, -2.0f, -3.0f);
            Vector3 actual;

            actual = Vector3.Negate(a);
            Assert.AreEqual(expected, actual, "Vector3.Negate did not return the expected value.");
        }

        /// <summary>
        ///A test for operator != (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3InequalityTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert.AreEqual(expected, actual, "Vector3.operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert.AreEqual(expected, actual, "Vector3.operator != did not return the expected value.");
        }

        /// <summary>
        ///A test for operator == (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3EqualityTest()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert.AreEqual(expected, actual, "Vector3.operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert.AreEqual(expected, actual, "Vector3.operator == did not return the expected value.");
        }

        /// <summary>
        ///A test for Subtract (Vector3, Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3SubtractTest()
        {
            Vector3 a = new Vector3(1.0f, 6.0f, 3.0f);
            Vector3 b = new Vector3(5.0f, 2.0f, 3.0f);

            Vector3 expected = new Vector3(-4.0f, 4.0f, 0.0f);
            Vector3 actual;

            actual = Vector3.Subtract(a, b);
            Assert.AreEqual(expected, actual, "Vector3.Subtract did not return the expected value.");
        }

        /// <summary>
        ///A test for One
        ///</summary>
        [TestMethod]
        public void Vector3OneTest()
        {
            Vector3 val = new Vector3(1.0f, 1.0f, 1.0f);
            Assert.AreEqual(val, Vector3.One, "Vector3.One was not set correctly.");
        }

        /// <summary>
        ///A test for UnitX
        ///</summary>
        [TestMethod]
        public void Vector3UnitXTest()
        {
            Vector3 val = new Vector3(1.0f, 0.0f, 0.0f);
            Assert.AreEqual(val, Vector3.UnitX, "Vector3.UnitX was not set correctly.");
        }

        /// <summary>
        ///A test for UnitY
        ///</summary>
        [TestMethod]
        public void Vector3UnitYTest()
        {
            Vector3 val = new Vector3(0.0f, 1.0f, 0.0f);
            Assert.AreEqual(val, Vector3.UnitY, "Vector3.UnitY was not set correctly.");
        }

        /// <summary>
        ///A test for UnitZ
        ///</summary>
        [TestMethod]
        public void Vector3UnitZTest()
        {
            Vector3 val = new Vector3(0.0f, 0.0f, 1.0f);
            Assert.AreEqual(val, Vector3.UnitZ, "Vector3.UnitZ was not set correctly.");
        }

        /// <summary>
        ///A test for Zero
        ///</summary>
        [TestMethod]
        public void Vector3ZeroTest()
        {
            Vector3 val = new Vector3(0.0f, 0.0f, 0.0f);
            Assert.AreEqual(val, Vector3.Zero, "Vector3.Zero was not set correctly.");
        }

        /// <summary>
        ///A test for Equals (Vector3)
        ///</summary>
        [TestMethod]
        public void Vector3EqualsTest1()
        {
            Vector3 a = new Vector3(1.0f, 2.0f, 3.0f);
            Vector3 b = new Vector3(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Vector3.Equals did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 (float)
        ///</summary>
        [TestMethod]
        public void Vector3ConstructorTest5()
        {
            float value = 1.0f;
            Vector3 target = new Vector3(value);

            Vector3 expected = new Vector3(value, value, value);
            Assert.AreEqual(expected, target, "Vector3.cstr did not return the expected value.");

            value = 2.0f;
            target = new Vector3(value);
            expected = new Vector3(value, value, value);
            Assert.AreEqual(expected, target, "Vector3.cstr did not return the expected value.");
        }

        /// <summary>
        ///A test for Vector3 comparison involving NaN values
        ///</summary>
        [TestMethod]
        public void Vector3EqualsNanTest()
        {
            Vector3 a = new Vector3(float.NaN, 0, 0);
            Vector3 b = new Vector3(0, float.NaN, 0);
            Vector3 c = new Vector3(0, 0, float.NaN);

            Assert.IsFalse(a == Vector3.Zero);
            Assert.IsFalse(b == Vector3.Zero);
            Assert.IsFalse(c == Vector3.Zero);

            Assert.IsTrue(a != Vector3.Zero);
            Assert.IsTrue(b != Vector3.Zero);
            Assert.IsTrue(c != Vector3.Zero);

            Assert.IsFalse(a.Equals(Vector3.Zero));
            Assert.IsFalse(b.Equals(Vector3.Zero));
            Assert.IsFalse(c.Equals(Vector3.Zero));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert.IsFalse(a.Equals(a));
            Assert.IsFalse(b.Equals(b));
            Assert.IsFalse(c.Equals(c));
        }

        /// <summary>
        ///A test to make sure these types are blittable directly into GPU buffer memory layouts
        ///</summary>
        [TestMethod]
        public unsafe void Vector3SizeofTest()
        {
            Assert.AreEqual(12, sizeof(Vector3));
            Assert.AreEqual(24, sizeof(Vector3_2x));
            Assert.AreEqual(16, sizeof(Vector3PlusFloat));
            Assert.AreEqual(32, sizeof(Vector3PlusFloat_2x));
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector3_2x
        {
            Vector3 a;
            Vector3 b;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector3PlusFloat
        {
            Vector3 v;
            float f;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Vector3PlusFloat_2x
        {
            Vector3PlusFloat a;
            Vector3PlusFloat b;
        }
    }
}
