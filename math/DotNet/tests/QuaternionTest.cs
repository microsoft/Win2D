//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Windows.Math;

namespace MathTests
{
    [TestClass()]
    public class QuaternionTest
    {
        // A test for Dot (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionDotTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = Quaternion.Dot(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Dot did not return the expected value.");
        }

        // A test for Length ()
        [TestMethod]
        public void QuaternionLengthTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);

            float w = 4.0f;

            Quaternion target = new Quaternion(v, w);

            float expected = 5.477226f;
            float actual;

            actual = target.Length();

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        [TestMethod]
        public void QuaternionLengthSquaredTest()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target = new Quaternion(v, w);

            float expected = 30.0f;
            float actual;

            actual = target.LengthSquared();

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.LengthSquared did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        [TestMethod]
        public void QuaternionLerpTest()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(20.0f));
            Quaternion actual;

            actual = Quaternion.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Lerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = Quaternion.Lerp(a, a, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Lerp did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        // Lerp test when t = 0
        [TestMethod]
        public void QuaternionLerpTest1()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected = new Quaternion(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Lerp did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        // Lerp test when t = 1
        [TestMethod]
        public void QuaternionLerpTest2()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected = new Quaternion(b.X, b.Y, b.Z, b.W);
            Quaternion actual = Quaternion.Lerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Lerp did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        // Lerp test when the two quaternions are more than 90 degree (dot product <0)
        [TestMethod]
        public void QuaternionLerpTest3()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.Negate(a);

            float t = 1.0f;

            Quaternion actual = Quaternion.Lerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert.IsTrue(actual == a, "Quaternion.Lerp did not return the expected value.");
        }

        // A test for Conjugate(Quaternion)
        [TestMethod]
        public void QuaternionConjugateTest1()
        {
            Quaternion a = new Quaternion(1, 2, 3, 4);

            Quaternion expected = new Quaternion(-1, -2, -3, 4);
            Quaternion actual;

            actual = Quaternion.Conjugate(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Conjugate did not return the expected value.");
        }

        // A test for Normalize (Quaternion)
        [TestMethod]
        public void QuaternionNormalizeTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected = new Quaternion(0.182574168f, 0.365148336f, 0.5477225f, 0.7302967f);
            Quaternion actual;

            actual = Quaternion.Normalize(a);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Normalize did not return the expected value.");
        }

        // A test for Normalize (Quaternion)
        // Normalize zero length quaternion
        [TestMethod]
        public void QuaternionNormalizeTest1()
        {
            Quaternion a = new Quaternion(0.0f, 0.0f, -0.0f, 0.0f);

            Quaternion actual = Quaternion.Normalize(a);
            Assert.IsTrue(float.IsNaN(actual.X) && float.IsNaN(actual.Y) && float.IsNaN(actual.Z) && float.IsNaN(actual.W)
                , "Quaternion.Normalize did not return the expected value.");
        }

        // A test for Concatenate(Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionConcatenateTest1()
        {
            Quaternion b = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion a = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = Quaternion.Concatenate(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Concatenate did not return the expected value.");
        }

        // A test for operator - (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionSubtractionTest()
        {
            Quaternion a = new Quaternion(1.0f, 6.0f, 7.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 2.0f, 3.0f, 8.0f);

            Quaternion expected = new Quaternion(-4.0f, 4.0f, 4.0f, -4.0f);
            Quaternion actual;

            actual = a - b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator - did not return the expected value.");
        }

        // A test for operator * (Quaternion, float)
        [TestMethod]
        public void QuaternionMultiplyTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            Quaternion expected = new Quaternion(0.5f, 1.0f, 1.5f, 2.0f);
            Quaternion actual;

            actual = a * factor;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator * did not return the expected value.");
        }

        // A test for operator * (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionMultiplyTest1()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = a * b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator * did not return the expected value.");
        }

        // A test for operator / (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionDivisionTest1()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            Quaternion actual;

            actual = a / b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator / did not return the expected value.");
        }

        // A test for operator + (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionAdditionTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(6.0f, 8.0f, 10.0f, 12.0f);
            Quaternion actual;

            actual = a + b;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator + did not return the expected value.");
        }

        // A test for Quaternion (float, float, float, float)
        [TestMethod]
        public void QuaternionConstructorTest()
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Quaternion target = new Quaternion(x, y, z, w);

            Assert.IsTrue(MathHelper.Equal(target.X, x) && MathHelper.Equal(target.Y, y) && MathHelper.Equal(target.Z, z) && MathHelper.Equal(target.W, w),
                "Quaternion.constructor (x,y,z,w) did not return the expected value.");
        }

        // A test for Quaternion (Vector3, float)
        [TestMethod]
        public void QuaternionConstructorTest1()
        {
            Vector3 v = new Vector3(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target = new Quaternion(v, w);
            Assert.IsTrue(MathHelper.Equal(target.X, v.X) && MathHelper.Equal(target.Y, v.Y) && MathHelper.Equal(target.Z, v.Z) && MathHelper.Equal(target.W, w),
                "Quaternion.constructor (Vector3,w) did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        [TestMethod]
        public void QuaternionCreateFromAxisAngleTest()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            float angle = MathHelper.ToRadians(30.0f);

            Quaternion expected = new Quaternion(0.0691723f, 0.1383446f, 0.207516879f, 0.9659258f);
            Quaternion actual;

            actual = Quaternion.CreateFromAxisAngle(axis, angle);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        // CreateFromAxisAngle of zero vector
        [TestMethod]
        public void QuaternionCreateFromAxisAngleTest1()
        {
            Vector3 axis = new Vector3();
            float angle = MathHelper.ToRadians(-30.0f);

            float cos = (float)System.Math.Cos(angle / 2.0f);
            Quaternion actual = Quaternion.CreateFromAxisAngle(axis, angle);

            Assert.IsTrue(actual.X == 0.0f && actual.Y == 0.0f && actual.Z == 0.0f && MathHelper.Equal(cos, actual.W)
                , "Quaternion.CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        // CreateFromAxisAngle of angle = 30 && 750
        [TestMethod]
        public void QuaternionCreateFromAxisAngleTest2()
        {
            Vector3 axis = new Vector3(1, 0, 0);
            float angle1 = MathHelper.ToRadians(30.0f);
            float angle2 = MathHelper.ToRadians(750.0f);

            Quaternion actual1 = Quaternion.CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = Quaternion.CreateFromAxisAngle(axis, angle2);
            Assert.IsTrue(MathHelper.Equal(actual1, actual2), "Quaternion.CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        // CreateFromAxisAngle of angle = 30 && 390
        [TestMethod]
        public void QuaternionCreateFromAxisAngleTest3()
        {
            Vector3 axis = new Vector3(1, 0, 0);
            float angle1 = MathHelper.ToRadians(30.0f);
            float angle2 = MathHelper.ToRadians(390.0f);

            Quaternion actual1 = Quaternion.CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = Quaternion.CreateFromAxisAngle(axis, angle2);
            actual1.X = -actual1.X;
            actual1.W = -actual1.W;

            Assert.IsTrue(MathHelper.Equal(actual1, actual2), "Quaternion.CreateFromAxisAngle did not return the expected value.");
        }

        [TestMethod]
        public void QuaternionCreateFromYawPitchRollTest1()
        {
            float yawAngle = MathHelper.ToRadians(30.0f);
            float pitchAngle = MathHelper.ToRadians(40.0f);
            float rollAngle = MathHelper.ToRadians(50.0f);

            Quaternion yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, yawAngle);
            Quaternion pitch = Quaternion.CreateFromAxisAngle(Vector3.UnitX, pitchAngle);
            Quaternion roll = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, rollAngle);

            Quaternion expected = yaw * pitch * roll;
            Quaternion actual = Quaternion.CreateFromYawPitchRoll(yawAngle, pitchAngle, rollAngle);
            Assert.IsTrue(MathHelper.Equal(expected, actual));
        }

        // Covers more numeric rigions
        [TestMethod]
        public void QuaternionCreateFromYawPitchRollTest2()
        {
            const float step = 35.0f;

            for (float yawAngle = -720.0f; yawAngle <= 720.0f; yawAngle += step)
            {
                for (float pitchAngle = -720.0f; pitchAngle <= 720.0f; pitchAngle += step)
                {
                    for (float rollAngle = -720.0f; rollAngle <= 720.0f; rollAngle += step)
                    {
                        float yawRad = MathHelper.ToRadians(yawAngle);
                        float pitchRad = MathHelper.ToRadians(pitchAngle);
                        float rollRad = MathHelper.ToRadians(rollAngle);

                        Quaternion yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, yawRad);
                        Quaternion pitch = Quaternion.CreateFromAxisAngle(Vector3.UnitX, pitchRad);
                        Quaternion roll = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, rollRad);

                        Quaternion expected = yaw * pitch * roll;
                        Quaternion actual = Quaternion.CreateFromYawPitchRoll(yawRad, pitchRad, rollRad);
                        Assert.IsTrue(MathHelper.Equal(expected, actual), String.Format("Yaw:{0} Pitch:{1} Roll:{2}", yawAngle, pitchAngle, rollAngle));
                    }
                }
            }
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        [TestMethod]
        public void QuaternionSlerpTest()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(20.0f));
            Quaternion actual;

            actual = Quaternion.Slerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Slerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = Quaternion.Slerp(a, a, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        // Slerp test where t = 0
        [TestMethod]
        public void QuaternionSlerpTest1()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected = new Quaternion(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion.Slerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        // Slerp test where t = 1
        [TestMethod]
        public void QuaternionSlerpTest2()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected = new Quaternion(b.X, b.Y, b.Z, b.W);
            Quaternion actual = Quaternion.Slerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        // Slerp test where dot product is < 0
        [TestMethod]
        public void QuaternionSlerpTest3()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = -a;

            float t = 1.0f;

            Quaternion expected = a;
            Quaternion actual = Quaternion.Slerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert.IsTrue(actual == expected, "Quaternion.Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        // Slerp test where the quaternion is flipped
        [TestMethod]
        public void QuaternionSlerpTest4()
        {
            Vector3 axis = Vector3.Normalize(new Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(10.0f));
            Quaternion b = -Quaternion.CreateFromAxisAngle(axis, MathHelper.ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected = new Quaternion(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion.Slerp(a, b, t);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Slerp did not return the expected value.");
        }

        // A test for operator - (Quaternion)
        [TestMethod]
        public void QuaternionUnaryNegationTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected = new Quaternion(-1.0f, -2.0f, -3.0f, -4.0f);
            Quaternion actual;

            actual = -a;

            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.operator - did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        [TestMethod]
        public void QuaternionInverseTest()
        {
            Quaternion a = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(-0.0287356321f, -0.03448276f, -0.0402298868f, 0.04597701f);
            Quaternion actual;

            actual = Quaternion.Inverse(a);
            Assert.AreEqual(expected, actual, "Quaternion.Inverse did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        // Invert zero length quaternion
        [TestMethod]
        public void QuaternionInverseTest1()
        {
            Quaternion a = new Quaternion();
            Quaternion actual = Quaternion.Inverse(a);

            Assert.IsTrue(float.IsNaN(actual.X) && float.IsNaN(actual.Y) && float.IsNaN(actual.Z) && float.IsNaN(actual.W)
                , "Quaternion.Inverse did not return the expected value.");
        }

        // A test for ToString ()
        [TestMethod]
        public void QuaternionToStringTest()
        {
            Quaternion target = new Quaternion(-1.0f, 2.2f, 3.3f, -4.4f);
            string expected = "{X:-1 Y:2.2 Z:3.3 W:-4.4}";
            string actual;

            actual = target.ToString();
            Assert.AreEqual(expected, actual, "Quaternion.ToString did not return the expected value.");
        }

        // A test for Add (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionAddTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(6.0f, 8.0f, 10.0f, 12.0f);
            Quaternion actual;

            actual = Quaternion.Add(a, b);
            Assert.AreEqual(expected, actual, "Quaternion.Add did not return the expected value.");
        }

        // A test for Divide (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionDivideTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            Quaternion actual;

            actual = Quaternion.Divide(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Divide did not return the expected value.");
        }

        // A test for Equals (object)
        [TestMethod]
        public void QuaternionEqualsTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            object obj = b;

            bool expected = true;
            bool actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            obj = b;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");

            // case 3: compare between different types.
            obj = new Vector4();
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");

            // case 3: compare against null.
            obj = null;
            expected = false;
            actual = a.Equals(obj);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");
        }

        // A test for GetHashCode ()
        [TestMethod]
        public void QuaternionGetHashCodeTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            int expected = a.X.GetHashCode() + a.Y.GetHashCode() + a.Z.GetHashCode() + a.W.GetHashCode();
            int actual = a.GetHashCode();
            Assert.AreEqual(expected, actual, "Quaternion.GetHashCode did not return the expected value.");
        }

        // A test for Multiply (Quaternion, float)
        [TestMethod]
        public void QuaternionMultiplyTest2()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            Quaternion expected = new Quaternion(0.5f, 1.0f, 1.5f, 2.0f);
            Quaternion actual;

            actual = Quaternion.Multiply(a, factor);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Multiply did not return the expected value.");
        }

        // A test for Multiply (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionMultiplyTest3()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = new Quaternion(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = Quaternion.Multiply(a, b);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.Multiply did not return the expected value.");
        }

        // A test for Negate (Quaternion)
        [TestMethod]
        public void QuaternionNegateTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected = new Quaternion(-1.0f, -2.0f, -3.0f, -4.0f);
            Quaternion actual;

            actual = Quaternion.Negate(a);
            Assert.AreEqual(expected, actual, "Quaternion.Negate did not return the expected value.");
        }

        // A test for Subtract (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionSubtractTest()
        {
            Quaternion a = new Quaternion(1.0f, 6.0f, 7.0f, 4.0f);
            Quaternion b = new Quaternion(5.0f, 2.0f, 3.0f, 8.0f);

            Quaternion expected = new Quaternion(-4.0f, 4.0f, 4.0f, -4.0f);
            Quaternion actual;

            actual = Quaternion.Subtract(a, b);
            Assert.AreEqual(expected, actual, "Quaternion.Subtract did not return the expected value.");
        }

        // A test for operator != (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionInequalityTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert.AreEqual(expected, actual, "Quaternion.operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert.AreEqual(expected, actual, "Quaternion.operator != did not return the expected value.");
        }

        // A test for operator == (Quaternion, Quaternion)
        [TestMethod]
        public void QuaternionEqualityTest()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert.AreEqual(expected, actual, "Quaternion.operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert.AreEqual(expected, actual, "Quaternion.operator == did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Convert Identity matrix test
        [TestMethod]
        public void QuaternionFromRotationMatrixTest1()
        {
            Matrix4x4 matrix = Matrix4x4.Identity;

            Quaternion expected = new Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
            Assert.IsTrue(MathHelper.Equal(expected, actual), "Quaternion.CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
            Assert.IsTrue(MathHelper.Equal(matrix, m2), "Quaternion.CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Convert X axis rotation matrix
        [TestMethod]
        public void QuaternionFromRotationMatrixTest2()
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4.CreateRotationX(angle);

                Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitX, angle);
                Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
                Assert.IsTrue(MathHelper.EqualRotation(expected, actual),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), expected.ToString(), actual.ToString());

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
                Assert.IsTrue(MathHelper.Equal(matrix, m2),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), matrix.ToString(), m2.ToString());
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Convert Y axis rotation matrix
        [TestMethod]
        public void QuaternionFromRotationMatrixTest3()
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4.CreateRotationY(angle);

                Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
                Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
                Assert.IsTrue(MathHelper.EqualRotation(expected, actual),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0}",
                    angle.ToString());

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
                Assert.IsTrue(MathHelper.Equal(matrix, m2),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0}",
                    angle.ToString());
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Convert Z axis rotation matrix
        [TestMethod]
        public void QuaternionFromRotationMatrixTest4()
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4.CreateRotationZ(angle);

                Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, angle);
                Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
                Assert.IsTrue(MathHelper.EqualRotation(expected, actual),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), expected.ToString(), actual.ToString());

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
                Assert.IsTrue(MathHelper.Equal(matrix, m2),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), matrix.ToString(), m2.ToString());
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Convert XYZ axis rotation matrix
        [TestMethod]
        public void QuaternionFromRotationMatrixTest5()
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4.CreateRotationX(angle) * Matrix4x4.CreateRotationY(angle) * Matrix4x4.CreateRotationZ(angle);

                Quaternion expected =
                    Quaternion.CreateFromAxisAngle(Vector3.UnitZ, angle) *
                    Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle) *
                    Quaternion.CreateFromAxisAngle(Vector3.UnitX, angle);

                Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
                Assert.IsTrue(MathHelper.EqualRotation(expected, actual),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), expected.ToString(), actual.ToString());

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
                Assert.IsTrue(MathHelper.Equal(matrix, m2),
                    "Quaternion.CreateFromRotationMatrix did not return the expected value. angle:{0} expected:{1} actual:{2}",
                    angle.ToString(), matrix.ToString(), m2.ToString());
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // X axis is most large axis case
        [TestMethod]
        public void QuaternionFromRotationMatrixWithScaledMatrixTest1()
        {
            float angle = MathHelper.ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4.CreateRotationY(angle) * Matrix4x4.CreateRotationZ(angle);

            Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, angle) * Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);
            Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
            Assert.IsTrue(MathHelper.EqualRotation(expected, actual), "Quaternion.CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
            Assert.IsTrue(MathHelper.Equal(matrix, m2), "Quaternion.CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Y axis is most large axis case
        [TestMethod]
        public void QuaternionFromRotationMatrixWithScaledMatrixTest2()
        {
            float angle = MathHelper.ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4.CreateRotationX(angle) * Matrix4x4.CreateRotationZ(angle);

            Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, angle) * Quaternion.CreateFromAxisAngle(Vector3.UnitX, angle);
            Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
            Assert.IsTrue(MathHelper.EqualRotation(expected, actual), "Quaternion.CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
            Assert.IsTrue(MathHelper.Equal(matrix, m2), "Quaternion.CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        // Z axis is most large axis case
        [TestMethod]
        public void QuaternionFromRotationMatrixWithScaledMatrixTest3()
        {
            float angle = MathHelper.ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4.CreateRotationX(angle) * Matrix4x4.CreateRotationY(angle);

            Quaternion expected = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle) * Quaternion.CreateFromAxisAngle(Vector3.UnitX, angle);
            Quaternion actual = Quaternion.CreateFromRotationMatrix(matrix);
            Assert.IsTrue(MathHelper.EqualRotation(expected, actual), "Quaternion.CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4.CreateFromQuaternion(actual);
            Assert.IsTrue(MathHelper.Equal(matrix, m2), "Quaternion.CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for Equals (Quaternion)
        [TestMethod]
        public void QuaternionEqualsTest1()
        {
            Quaternion a = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = new Quaternion(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a.Equals(b);
            Assert.AreEqual(expected, actual, "Quaternion.Equals did not return the expected value.");
        }

        // A test for Identity
        [TestMethod]
        public void QuaternionIdentityTest()
        {
            Quaternion val = new Quaternion(0, 0, 0, 1);
            Assert.AreEqual(val, Quaternion.Identity, "Quaternion.Identity was not set correctly.");
        }

        // A test for IsIdentity
        [TestMethod]
        public void QuaternionIsIdentityTest()
        {
            Assert.IsTrue(Quaternion.Identity.IsIdentity);
            Assert.IsTrue(new Quaternion(0, 0, 0, 1).IsIdentity);
            Assert.IsFalse(new Quaternion(1, 0, 0, 1).IsIdentity);
            Assert.IsFalse(new Quaternion(0, 1, 0, 1).IsIdentity);
            Assert.IsFalse(new Quaternion(0, 0, 1, 1).IsIdentity);
            Assert.IsFalse(new Quaternion(0, 0, 0, 0).IsIdentity);
        }

        // A test for Quaternion comparison involving NaN values
        [TestMethod]
        public void QuaternionEqualsNanTest()
        {
            Quaternion a = new Quaternion(float.NaN, 0, 0, 0);
            Quaternion b = new Quaternion(0, float.NaN, 0, 0);
            Quaternion c = new Quaternion(0, 0, float.NaN, 0);
            Quaternion d = new Quaternion(0, 0, 0, float.NaN);

            Assert.IsFalse(a == new Quaternion(0, 0, 0, 0));
            Assert.IsFalse(b == new Quaternion(0, 0, 0, 0));
            Assert.IsFalse(c == new Quaternion(0, 0, 0, 0));
            Assert.IsFalse(d == new Quaternion(0, 0, 0, 0));

            Assert.IsTrue(a != new Quaternion(0, 0, 0, 0));
            Assert.IsTrue(b != new Quaternion(0, 0, 0, 0));
            Assert.IsTrue(c != new Quaternion(0, 0, 0, 0));
            Assert.IsTrue(d != new Quaternion(0, 0, 0, 0));

            Assert.IsFalse(a.Equals(new Quaternion(0, 0, 0, 0)));
            Assert.IsFalse(b.Equals(new Quaternion(0, 0, 0, 0)));
            Assert.IsFalse(c.Equals(new Quaternion(0, 0, 0, 0)));
            Assert.IsFalse(d.Equals(new Quaternion(0, 0, 0, 0)));

            Assert.IsFalse(a.IsIdentity);
            Assert.IsFalse(b.IsIdentity);
            Assert.IsFalse(c.IsIdentity);
            Assert.IsFalse(d.IsIdentity);

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert.IsFalse(a.Equals(a));
            Assert.IsFalse(b.Equals(b));
            Assert.IsFalse(c.Equals(c));
            Assert.IsFalse(d.Equals(d));
        }

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        [TestMethod]
        public unsafe void QuaternionSizeofTest()
        {
            Assert.AreEqual(16, sizeof(Quaternion));
            Assert.AreEqual(32, sizeof(Quaternion_2x));
            Assert.AreEqual(20, sizeof(QuaternionPlusFloat));
            Assert.AreEqual(40, sizeof(QuaternionPlusFloat_2x));
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Quaternion_2x
        {
            Quaternion a;
            Quaternion b;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct QuaternionPlusFloat
        {
            Quaternion v;
            float f;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct QuaternionPlusFloat_2x
        {
            QuaternionPlusFloat a;
            QuaternionPlusFloat b;
        }

        // A test to make sure the fields are laid out how we expect
        [TestMethod]
        public unsafe void QuaternionFieldOffsetTest()
        {
            Quaternion* ptr = (Quaternion*)0;

            Assert.AreEqual(new IntPtr(0), new IntPtr(&ptr->X));
            Assert.AreEqual(new IntPtr(4), new IntPtr(&ptr->Y));
            Assert.AreEqual(new IntPtr(8), new IntPtr(&ptr->Z));
            Assert.AreEqual(new IntPtr(12), new IntPtr(&ptr->W));
        }
    }
}
