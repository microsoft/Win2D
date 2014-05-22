//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::Graphics::Canvas::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(QuaternionTest)
    {
    public:
        // A test for Dot (Quaternion, Quaternion)
        TEST_METHOD(QuaternionDotTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = QuaternionHelper::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(QuaternionLengthTest)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);

            float w = 4.0f;

            Quaternion target = QuaternionHelper::FromVector3(v, w);

            float expected = 5.477226f;
            float actual;

            actual = QuaternionHelper::Length(target);

            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(QuaternionLengthSquaredTest)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target = QuaternionHelper::FromVector3(v, w);

            float expected = 30.0f;
            float actual;

            actual = QuaternionHelper::LengthSquared(target);

            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::LengthSquared did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(20.0f));
            Quaternion actual;

            actual = QuaternionHelper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Lerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = QuaternionHelper::Lerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Lerp did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest1)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected = QuaternionHelper::FromValues(a.X, a.Y, a.Z, a.W);
            Quaternion actual = QuaternionHelper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Lerp did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest2)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected = QuaternionHelper::FromValues(b.X, b.Y, b.Z, b.W);
            Quaternion actual = QuaternionHelper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest3)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::Negate(a);

            float t = 1.0f;

            Quaternion actual = QuaternionHelper::Lerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == a, L"QuaternionHelper::Lerp did not return the expected value.");
        }

        // A test for Conjugate(Quaternion)
        TEST_METHOD(QuaternionConjugateTest1)
        {
            Quaternion a = QuaternionHelper::FromValues(1, 2, 3, 4);

            Quaternion expected = QuaternionHelper::FromValues(-1, -2, -3, 4);
            Quaternion actual;

            actual = QuaternionHelper::Conjugate(a);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Conjugate did not return the expected value.");
        }

        // A test for Normalize (Quaternion)
        TEST_METHOD(QuaternionNormalizeTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected = QuaternionHelper::FromValues(0.182574168f, 0.365148336f, 0.5477225f, 0.7302967f);
            Quaternion actual;

            actual = QuaternionHelper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Normalize did not return the expected value.");
        }
        
        // A test for Normalize (Quaternion)
        TEST_METHOD(QuaternionNormalizeTest1)
        {
            Quaternion a = QuaternionHelper::FromValues(0.0f, 0.0f, -0.0f, 0.0f);

            Quaternion actual = QuaternionHelper::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W)
                , L"QuaternionHelper::Normalize did not return the expected value.");
        }
        
        // A test for Concatenate(Quaternion, Quaternion)
        TEST_METHOD(QuaternionConcatenateTest1)
        {
            Quaternion b = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion a = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = QuaternionHelper::Concatenate(a, b);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Concatenate did not return the expected value.");
        }

        // A test for Quaternion (float, float, float, float)
        TEST_METHOD(QuaternionConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Quaternion target = QuaternionHelper::FromValues(x, y, z, w);

            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z) && Equal(target.W, w),
                L"QuaternionHelper::constructor (x,y,z,w) did not return the expected value.");
        }
        
        // A test for Quaternion (Vector3, float)
        TEST_METHOD(QuaternionConstructorTest1)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target = QuaternionHelper::FromVector3(v, w);
            Assert::IsTrue(Equal(target.X, v.X) && Equal(target.Y, v.Y) && Equal(target.Z, v.Z) && Equal(target.W, w),
                L"QuaternionHelper::constructor (Vector3,w) did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            float angle = ToRadians(30.0f);

            Quaternion expected = QuaternionHelper::FromValues(0.0691723f, 0.1383446f, 0.207516879f, 0.9659258f);
            Quaternion actual;

            actual = QuaternionHelper::CreateFromAxisAngle(axis, angle);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::CreateFromAxisAngle did not return the expected value.");
        }
        
        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest1)
        {
            Vector3 axis = Vector3Helper::FromValues(0, 0, 0);
            float angle = ToRadians(-30.0f);

            float cos = cosf(angle / 2.0f);
            Quaternion actual = QuaternionHelper::CreateFromAxisAngle(axis, angle);

            Assert::IsTrue(actual.X == 0.0f && actual.Y == 0.0f && actual.Z == 0.0f && Equal(cos, actual.W)
                , L"QuaternionHelper::CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest2)
        {
            Vector3 axis = Vector3Helper::FromValues(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(750.0f);

            Quaternion actual1 = QuaternionHelper::CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = QuaternionHelper::CreateFromAxisAngle(axis, angle2);
            Assert::IsTrue(Equal(actual1, actual2), L"QuaternionHelper::CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest3)
        {
            Vector3 axis = Vector3Helper::FromValues(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(390.0f);

            Quaternion actual1 = QuaternionHelper::CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = QuaternionHelper::CreateFromAxisAngle(axis, angle2);
            actual1.X = -actual1.X;
            actual1.W = -actual1.W;

            Assert::IsTrue(Equal(actual1, actual2), L"QuaternionHelper::CreateFromAxisAngle did not return the expected value.");
        }

        TEST_METHOD(QuaternionCreateFromYawPitchRollTest1)
        {
            float yawAngle = ToRadians(30.0f);
            float pitchAngle = ToRadians(40.0f);
            float rollAngle = ToRadians(50.0f);

            Quaternion yaw = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, yawAngle);
            Quaternion pitch = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, pitchAngle);
            Quaternion roll = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, rollAngle);

            Quaternion expected = QuaternionHelper::Multiply(QuaternionHelper::Multiply(yaw, pitch), roll);
            Quaternion actual = QuaternionHelper::CreateFromYawPitchRoll(yawAngle, pitchAngle, rollAngle);
            Assert::IsTrue(Equal(expected, actual));
        }

        TEST_METHOD(QuaternionCreateFromYawPitchRollTest2)
        {
            const float step = 123;

            for (float yawAngle = -720.0f; yawAngle <= 720.0f; yawAngle += step)
            {
                for (float pitchAngle = -720.0f; pitchAngle <= 720.0f; pitchAngle += step)
                {
                    for (float rollAngle = -720.0f; rollAngle <= 720.0f; rollAngle += step)
                    {
                        float yawRad = ToRadians(yawAngle);
                        float pitchRad = ToRadians(pitchAngle);
                        float rollRad = ToRadians(rollAngle);

                        Quaternion yaw = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, yawRad);
                        Quaternion pitch = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, pitchRad);
                        Quaternion roll = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, rollRad);

                        Quaternion expected = QuaternionHelper::Multiply(QuaternionHelper::Multiply(yaw, pitch), roll);
                        Quaternion actual = QuaternionHelper::CreateFromYawPitchRoll(yawRad, pitchRad, rollRad);
                        Assert::IsTrue(Equal(expected, actual));
                    }
                }
            }
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(20.0f));
            Quaternion actual;

            actual = QuaternionHelper::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Slerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = QuaternionHelper::Slerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest1)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected = QuaternionHelper::FromValues(a.X, a.Y, a.Z, a.W);
            Quaternion actual = QuaternionHelper::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest2)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected = QuaternionHelper::FromValues(b.X, b.Y, b.Z, b.W);
            Quaternion actual = QuaternionHelper::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest3)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::Negate(a);

            float t = 1.0f;

            Quaternion expected = a;
            Quaternion actual = QuaternionHelper::Slerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == expected, L"QuaternionHelper::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest4)
        {
            Vector3 axis = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f));
            Quaternion a = QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = QuaternionHelper::Negate(QuaternionHelper::CreateFromAxisAngle(axis, ToRadians(30.0f)));

            float t = 0.0f;

            Quaternion expected = QuaternionHelper::FromValues(a.X, a.Y, a.Z, a.W);
            Quaternion actual = QuaternionHelper::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Slerp did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        TEST_METHOD(QuaternionInverseTest)
        {
            Quaternion a = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(-0.0287356321f, -0.03448276f, -0.0402298868f, 0.04597701f);
            Quaternion actual;

            actual = QuaternionHelper::Inverse(a);
            Assert::AreEqual(expected, actual, L"QuaternionHelper::Inverse did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        TEST_METHOD(QuaternionInverseTest1)
        {
            Quaternion a = QuaternionHelper::FromValues(0, 0, 0, 0);
            Quaternion actual = QuaternionHelper::Inverse(a);

            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W)
                , L"QuaternionHelper::Inverse did not return the expected value.");
        }

        // A test for Add (Quaternion, Quaternion)
        TEST_METHOD(QuaternionAddTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(6.0f, 8.0f, 10.0f, 12.0f);
            Quaternion actual;

            actual = QuaternionHelper::Add(a, b);
            Assert::AreEqual(expected, actual, L"QuaternionHelper::Add did not return the expected value.");
        }

        // A test for Divide (Quaternion, Quaternion)
        TEST_METHOD(QuaternionDivideTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            Quaternion actual;

            actual = QuaternionHelper::Divide(a, b);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Divide did not return the expected value.");
        }

        // A test for Multiply (Quaternion, float)
        TEST_METHOD(QuaternionMultiplyTest2)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            Quaternion expected = QuaternionHelper::FromValues(0.5f, 1.0f, 1.5f, 2.0f);
            Quaternion actual;

            actual = QuaternionHelper::MultiplyByScalar(a, factor);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Multiply did not return the expected value.");
        }

        // A test for Multiply (Quaternion, Quaternion)
        TEST_METHOD(QuaternionMultiplyTest3)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b = QuaternionHelper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = QuaternionHelper::Multiply(a, b);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::Multiply did not return the expected value.");
        }

        // A test for Negate (Quaternion)
        TEST_METHOD(QuaternionNegateTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected = QuaternionHelper::FromValues(-1.0f, -2.0f, -3.0f, -4.0f);
            Quaternion actual;

            actual = QuaternionHelper::Negate(a);
            Assert::AreEqual(expected, actual, L"QuaternionHelper::Negate did not return the expected value.");
        }

        // A test for Subtract (Quaternion, Quaternion)
        TEST_METHOD(QuaternionSubtractTest)
        {
            Quaternion a = QuaternionHelper::FromValues(1.0f, 6.0f, 7.0f, 4.0f);
            Quaternion b = QuaternionHelper::FromValues(5.0f, 2.0f, 3.0f, 8.0f);

            Quaternion expected = QuaternionHelper::FromValues(-4.0f, 4.0f, 4.0f, -4.0f);
            Quaternion actual;

            actual = QuaternionHelper::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"QuaternionHelper::Subtract did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest1)
        {
            Matrix4x4 matrix = Matrix4x4Helper::Identity;

            Quaternion expected = QuaternionHelper::FromValues(0.0f, 0.0f, 0.0f, 1.0f);
            Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(Equal(expected, actual), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest2)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4Helper::CreateRotationX(angle);

                Quaternion expected = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, angle);
                Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest3)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4Helper::CreateRotationY(angle);

                Quaternion expected = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, angle);
                Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest4)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4Helper::CreateRotationZ(angle);

                Quaternion expected = QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, angle);
                Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest5)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix =
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::Multiply(
                            Matrix4x4Helper::CreateRotationX(angle),
                            Matrix4x4Helper::CreateRotationY(angle)
                        ),
                        Matrix4x4Helper::CreateRotationZ(angle)
                    );

                Quaternion expected =
                    QuaternionHelper::Multiply(
                        QuaternionHelper::Multiply(
                            QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, angle),
                            QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, angle)
                        ),
                        QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, angle)
                    );

                Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest1)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4Helper::Multiply(Matrix4x4Helper::CreateRotationY(angle), Matrix4x4Helper::CreateRotationZ(angle));

            Quaternion expected = QuaternionHelper::Multiply(QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, angle), 
                                                             QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, angle));

            Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest2)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4Helper::Multiply(Matrix4x4Helper::CreateRotationX(angle), Matrix4x4Helper::CreateRotationZ(angle));

            Quaternion expected = QuaternionHelper::Multiply(QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitZ, angle),
                                                             QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, angle));

            Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest3)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4Helper::Multiply(Matrix4x4Helper::CreateRotationX(angle), Matrix4x4Helper::CreateRotationY(angle));

            Quaternion expected = QuaternionHelper::Multiply(QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitY, angle),
                                                             QuaternionHelper::CreateFromAxisAngle(Vector3Helper::UnitX, angle));

            Quaternion actual = QuaternionHelper::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4Helper::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"QuaternionHelper::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for Identity
        TEST_METHOD(QuaternionIdentityTest)
        {
            Quaternion val = QuaternionHelper::FromValues(0, 0, 0, 1);
            Assert::AreEqual(val, QuaternionHelper::Identity, L"QuaternionHelper::Identity was not set correctly.");
        }

        // A test for IsIdentity()
        TEST_METHOD(QuaternionIsIdentityTest)
        {
            Assert::IsTrue(QuaternionHelper::IsIdentity(QuaternionHelper::Identity));
            Assert::IsTrue(QuaternionHelper::IsIdentity(QuaternionHelper::FromValues(0, 0, 0, 1)));
            Assert::IsFalse(QuaternionHelper::IsIdentity(QuaternionHelper::FromValues(1, 0, 0, 1)));
            Assert::IsFalse(QuaternionHelper::IsIdentity(QuaternionHelper::FromValues(0, 1, 0, 1)));
            Assert::IsFalse(QuaternionHelper::IsIdentity(QuaternionHelper::FromValues(0, 0, 1, 1)));
            Assert::IsFalse(QuaternionHelper::IsIdentity(QuaternionHelper::FromValues(0, 0, 0, 0)));
        }

        struct Quaternion_2x
        {
            Quaternion a;
            Quaternion b;
        };

        struct QuaternionPlusFloat
        {
            Quaternion v;
            float f;
        };

        struct QuaternionPlusFloat_2x
        {
            QuaternionPlusFloat a;
            QuaternionPlusFloat b;
        };
        
        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(QuaternionSizeofTest)
        {
            Assert::AreEqual(size_t(16), sizeof(Quaternion));
            Assert::AreEqual(size_t(32), sizeof(Quaternion_2x));
            Assert::AreEqual(size_t(20), sizeof(QuaternionPlusFloat));
            Assert::AreEqual(size_t(40), sizeof(QuaternionPlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(QuaternionFieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Quaternion, X));
            Assert::AreEqual(size_t(4), offsetof(Quaternion, Y));
            Assert::AreEqual(size_t(8), offsetof(Quaternion, Z));
            Assert::AreEqual(size_t(12), offsetof(Quaternion, W));
        }
    };
}
