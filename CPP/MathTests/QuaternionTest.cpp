//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(QuaternionTest)
    {
    public:
        // A test for Dot (Quaternion, Quaternion)
        TEST_METHOD(QuaternionDotTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = Quaternion::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(QuaternionLengthTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            float w = 4.0f;

            Quaternion target(v, w);

            float expected = 5.477226f;
            float actual;

            actual = target.Length();

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(QuaternionLengthSquaredTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target(v, w);

            float expected = 30.0f;
            float actual;

            actual = target.LengthSquared();

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::LengthSquared did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = Quaternion::CreateFromAxisAngle(axis, ToRadians(20.0f));
            Quaternion actual;

            actual = Quaternion::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Lerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = Quaternion::Lerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Lerp did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest1)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Lerp did not return the expected value.");
        }
        
        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest2)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected(b.X, b.Y, b.Z, b.W);
            Quaternion actual = Quaternion::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Lerp did not return the expected value.");
        }

        // A test for Lerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionLerpTest3)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::Negate(a);

            float t = 1.0f;

            Quaternion actual = Quaternion::Lerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == a, L"Quaternion::Lerp did not return the expected value.");
        }

        // A test for Conjugate(Quaternion)
        TEST_METHOD(QuaternionConjugateTest1)
        {
            Quaternion a(1, 2, 3, 4);

            Quaternion expected(-1, -2, -3, 4);
            Quaternion actual;

            actual = Quaternion::Conjugate(a);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Conjugate did not return the expected value.");
        }

        // A test for Normalize (Quaternion)
        TEST_METHOD(QuaternionNormalizeTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected(0.182574168f, 0.365148336f, 0.5477225f, 0.7302967f);
            Quaternion actual;

            actual = Quaternion::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Normalize did not return the expected value.");
        }
        
        // A test for Normalize (Quaternion)
        TEST_METHOD(QuaternionNormalizeTest1)
        {
            Quaternion a(0.0f, 0.0f, -0.0f, 0.0f);

            Quaternion actual = Quaternion::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W)
                , L"Quaternion::Normalize did not return the expected value.");
        }
        
        // A test for Concatenate(Quaternion, Quaternion)
        TEST_METHOD(QuaternionConcatenateTest1)
        {
            Quaternion b(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion a(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = Quaternion::Concatenate(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Concatenate did not return the expected value.");
        }

        // A test for operator - (Quaternion, Quaternion)
        TEST_METHOD(QuaternionSubtractionTest)
        {
            Quaternion a(1.0f, 6.0f, 7.0f, 4.0f);
            Quaternion b(5.0f, 2.0f, 3.0f, 8.0f);

            Quaternion expected(-4.0f, 4.0f, 4.0f, -4.0f);
            Quaternion actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator - did not return the expected value.");
        }

        // A test for operator * (Quaternion, float)
        TEST_METHOD(QuaternionMultiplyTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            Quaternion expected(0.5f, 1.0f, 1.5f, 2.0f);
            Quaternion actual;

            actual = a * factor;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator * did not return the expected value.");
        }

        // A test for operator * (Quaternion, Quaternion)
        TEST_METHOD(QuaternionMultiplyTest1)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            //            Quaternion expected(32.0f, 32.0f, 56.0f, -6.0f);
            Quaternion expected(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator * did not return the expected value.");
        }

        // A test for operator / (Quaternion, Quaternion)
        TEST_METHOD(QuaternionDivisionTest1)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            Quaternion actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator / did not return the expected value.");
        }

        // A test for operator + (Quaternion, Quaternion)
        TEST_METHOD(QuaternionAdditionTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(6.0f, 8.0f, 10.0f, 12.0f);
            Quaternion actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator + did not return the expected value.");
        }

        // A test for Quaternion (float, float, float, float)
        TEST_METHOD(QuaternionConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Quaternion target(x, y, z, w);

            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z) && Equal(target.W, w),
                L"Quaternion::constructor (x,y,z,w) did not return the expected value.");
        }
        
        // A test for Quaternion (Vector3, float)
        TEST_METHOD(QuaternionConstructorTest1)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Quaternion target(v, w);
            Assert::IsTrue(Equal(target.X, v.X) && Equal(target.Y, v.Y) && Equal(target.Z, v.Z) && Equal(target.W, w),
                L"Quaternion::constructor (Vector3,w) did not return the expected value.");
        }

        // A test for Quaternion ()
        TEST_METHOD(QuaternionConstructorTest2)
        {
            Quaternion target;
            Assert::IsTrue(target.X == 0 && target.Y == 0 && target.Z == 0 && target.W == 0,
                L"Quaternion::constructor () did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            float angle = ToRadians(30.0f);

            Quaternion expected(0.0691723f, 0.1383446f, 0.207516879f, 0.9659258f);
            Quaternion actual;

            actual = Quaternion::CreateFromAxisAngle(axis, angle);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::CreateFromAxisAngle did not return the expected value.");
        }
        
        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest1)
        {
            Vector3 axis;
            float angle = ToRadians(-30.0f);

            float cos = cosf(angle / 2.0f);
            Quaternion actual = Quaternion::CreateFromAxisAngle(axis, angle);

            Assert::IsTrue(actual.X == 0.0f && actual.Y == 0.0f && actual.Z == 0.0f && Equal(cos, actual.W)
                , L"Quaternion::CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest2)
        {
            Vector3 axis(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(750.0f);

            Quaternion actual1 = Quaternion::CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = Quaternion::CreateFromAxisAngle(axis, angle2);
            Assert::IsTrue(Equal(actual1, actual2), L"Quaternion::CreateFromAxisAngle did not return the expected value.");
        }

        // A test for CreateFromAxisAngle (Vector3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest3)
        {
            Vector3 axis(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(390.0f);

            Quaternion actual1 = Quaternion::CreateFromAxisAngle(axis, angle1);
            Quaternion actual2 = Quaternion::CreateFromAxisAngle(axis, angle2);
            actual1.X = -actual1.X;
            actual1.W = -actual1.W;

            Assert::IsTrue(Equal(actual1, actual2), L"Quaternion::CreateFromAxisAngle did not return the expected value.");
        }

        TEST_METHOD(QuaternionCreateFromYawPitchRollTest1)
        {
            float yawAngle = ToRadians(30.0f);
            float pitchAngle = ToRadians(40.0f);
            float rollAngle = ToRadians(50.0f);

            Quaternion yaw = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), yawAngle);
            Quaternion pitch = Quaternion::CreateFromAxisAngle(Vector3::UnitX(), pitchAngle);
            Quaternion roll = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), rollAngle);

            Quaternion expected = yaw * pitch * roll;
            Quaternion actual = Quaternion::CreateFromYawPitchRoll(yawAngle, pitchAngle, rollAngle);
            Assert::IsTrue(Equal(expected, actual));
        }

        TEST_METHOD(QuaternionCreateFromYawPitchRollTest2)
        {
            const float step = 35.0f;

            for (float yawAngle = -720.0f; yawAngle <= 720.0f; yawAngle += step)
            {
                for (float pitchAngle = -720.0f; pitchAngle <= 720.0f; pitchAngle += step)
                {
                    for (float rollAngle = -720.0f; rollAngle <= 720.0f; rollAngle += step)
                    {
                        float yawRad = ToRadians(yawAngle);
                        float pitchRad = ToRadians(pitchAngle);
                        float rollRad = ToRadians(rollAngle);

                        Quaternion yaw = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), yawRad);
                        Quaternion pitch = Quaternion::CreateFromAxisAngle(Vector3::UnitX(), pitchRad);
                        Quaternion roll = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), rollRad);

                        Quaternion expected = yaw * pitch * roll;
                        Quaternion actual = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, rollRad);
                        Assert::IsTrue(Equal(expected, actual));
                    }
                }
            }
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.5f;

            Quaternion expected = Quaternion::CreateFromAxisAngle(axis, ToRadians(20.0f));
            Quaternion actual;

            actual = Quaternion::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Slerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = Quaternion::Slerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest1)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest2)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 1.0f;

            Quaternion expected(b.X, b.Y, b.Z, b.W);
            Quaternion actual = Quaternion::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest3)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = -a;

            float t = 1.0f;

            Quaternion expected = a;
            Quaternion actual = Quaternion::Slerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == expected, L"Quaternion::Slerp did not return the expected value.");
        }

        // A test for Slerp (Quaternion, Quaternion, float)
        TEST_METHOD(QuaternionSlerpTest4)
        {
            Vector3 axis = Vector3::Normalize(Vector3(1.0f, 2.0f, 3.0f));
            Quaternion a = Quaternion::CreateFromAxisAngle(axis, ToRadians(10.0f));
            Quaternion b = -Quaternion::CreateFromAxisAngle(axis, ToRadians(30.0f));

            float t = 0.0f;

            Quaternion expected(a.X, a.Y, a.Z, a.W);
            Quaternion actual = Quaternion::Slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Slerp did not return the expected value.");
        }

        // A test for operator - (Quaternion)
        TEST_METHOD(QuaternionUnaryNegationTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected(-1.0f, -2.0f, -3.0f, -4.0f);
            Quaternion actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"Quaternion::operator - did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        TEST_METHOD(QuaternionInverseTest)
        {
            Quaternion a(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(-0.0287356321f, -0.03448276f, -0.0402298868f, 0.04597701f);
            Quaternion actual;

            actual = Quaternion::Inverse(a);
            Assert::AreEqual(expected, actual, L"Quaternion::Inverse did not return the expected value.");
        }

        // A test for Inverse (Quaternion)
        TEST_METHOD(QuaternionInverseTest1)
        {
            Quaternion a;
            Quaternion actual = Quaternion::Inverse(a);

            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W)
                , L"Quaternion::Inverse did not return the expected value.");
        }

        // A test for Add (Quaternion, Quaternion)
        TEST_METHOD(QuaternionAddTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(6.0f, 8.0f, 10.0f, 12.0f);
            Quaternion actual;

            actual = Quaternion::Add(a, b);
            Assert::AreEqual(expected, actual, L"Quaternion::Add did not return the expected value.");
        }

        // A test for Divide (Quaternion, Quaternion)
        TEST_METHOD(QuaternionDivideTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            Quaternion actual;

            actual = Quaternion::Divide(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Divide did not return the expected value.");
        }

        // A test for Multiply (Quaternion, float)
        TEST_METHOD(QuaternionMultiplyTest2)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            Quaternion expected(0.5f, 1.0f, 1.5f, 2.0f);
            Quaternion actual;

            actual = Quaternion::Multiply(a, factor);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Multiply did not return the expected value.");
        }

        // A test for Multiply (Quaternion, Quaternion)
        TEST_METHOD(QuaternionMultiplyTest3)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            Quaternion expected(24.0f, 48.0f, 48.0f, -6.0f);
            Quaternion actual;

            actual = Quaternion::Multiply(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::Multiply did not return the expected value.");
        }

        // A test for Negate (Quaternion)
        TEST_METHOD(QuaternionNegateTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);

            Quaternion expected(-1.0f, -2.0f, -3.0f, -4.0f);
            Quaternion actual;

            actual = Quaternion::Negate(a);
            Assert::AreEqual(expected, actual, L"Quaternion::Negate did not return the expected value.");
        }

        // A test for Subtract (Quaternion, Quaternion)
        TEST_METHOD(QuaternionSubtractTest)
        {
            Quaternion a(1.0f, 6.0f, 7.0f, 4.0f);
            Quaternion b(5.0f, 2.0f, 3.0f, 8.0f);

            Quaternion expected(-4.0f, 4.0f, 4.0f, -4.0f);
            Quaternion actual;

            actual = Quaternion::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Quaternion::Subtract did not return the expected value.");
        }

        // A test for operator +=
        TEST_METHOD(QuaternionOperatorAddEqualsTest)
        {
            Quaternion a(1, 2, 3, 4);
            Quaternion b(5, 6, 7, 8);
            Quaternion expected = Quaternion::Add(a, b);
            
            // In-place += operation.
            Quaternion& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Quaternion::Add(b, b);
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(QuaternionOperatorSubtractEqualsTest)
        {
            Quaternion a(1, 2, 3, 4);
            Quaternion b(5, 6, 7, 8);
            Quaternion expected = Quaternion::Subtract(a, b);
            
            // In-place -= operation.
            Quaternion& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Quaternion::Subtract(b, b);
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(QuaternionOperatorMultiplyEqualsTest)
        {
            Quaternion a(1, 2, 3, 4);
            Quaternion b(5, 6, 7, 8);
            Quaternion expected = Quaternion::Multiply(a, b);
            
            // In-place *= operation.
            Quaternion& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Quaternion::Multiply(b, b);
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(QuaternionOperatorMultiplyEqualsScalerTest)
        {
            Quaternion a(1, 2, 3, 4);
            float b = 5;
            Quaternion expected = Quaternion::Multiply(a, b);
            
            // In-place *= operation.
            Quaternion& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(QuaternionOperatorDivideEqualsTest)
        {
            Quaternion a(1, 2, 3, 4);
            Quaternion b(5, 6, 7, 8);
            Quaternion expected = Quaternion::Divide(a, b);
            
            // In-place /= operation.
            Quaternion& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Quaternion::Divide(b, b);
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator != (Quaternion, Quaternion)
        TEST_METHOD(QuaternionInequalityTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Quaternion::operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Quaternion::operator != did not return the expected value.");
        }

        // A test for operator == (Quaternion, Quaternion)
        TEST_METHOD(QuaternionEqualityTest)
        {
            Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            Quaternion b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Quaternion::operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Quaternion::operator == did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest1)
        {
            Matrix4x4 matrix = Matrix4x4::Identity();

            Quaternion expected(0.0f, 0.0f, 0.0f, 1.0f);
            Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(Equal(expected, actual), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest2)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4::CreateRotationX(angle);

                Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitX(), angle);
                Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest3)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4::CreateRotationY(angle);

                Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), angle);
                Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest4)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4::CreateRotationZ(angle);

                Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), angle);
                Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest5)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                Matrix4x4 matrix = Matrix4x4::CreateRotationX(angle) * Matrix4x4::CreateRotationY(angle) * Matrix4x4::CreateRotationZ(angle);

                Quaternion expected =
                    Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), angle) *
                    Quaternion::CreateFromAxisAngle(Vector3::UnitY(), angle) *
                    Quaternion::CreateFromAxisAngle(Vector3::UnitX(), angle);

                Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
            }
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest1)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4::CreateRotationY(angle) * Matrix4x4::CreateRotationZ(angle);

            Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), angle) * Quaternion::CreateFromAxisAngle(Vector3::UnitY(), angle);
            Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest2)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4::CreateRotationX(angle) * Matrix4x4::CreateRotationZ(angle);

            Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), angle) * Quaternion::CreateFromAxisAngle(Vector3::UnitX(), angle);
            Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for CreateFromRotationMatrix (Matrix4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest3)
        {
            float angle = ToRadians(180.0f);
            Matrix4x4 matrix = Matrix4x4::CreateRotationX(angle) * Matrix4x4::CreateRotationY(angle);

            Quaternion expected = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), angle) * Quaternion::CreateFromAxisAngle(Vector3::UnitX(), angle);
            Quaternion actual = Quaternion::CreateFromRotationMatrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            Matrix4x4 m2 = Matrix4x4::CreateFromQuaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"Quaternion::CreateFromRotationMatrix did not return the expected value.");
        }

        // A test for Identity
        TEST_METHOD(QuaternionIdentityTest)
        {
            Quaternion val(0, 0, 0, 1);
            Assert::AreEqual(val, Quaternion::Identity(), L"Quaternion::Identity was not set correctly.");
        }

        // A test for IsIdentity()
        TEST_METHOD(QuaternionIsIdentityTest)
        {
            Assert::IsTrue(Quaternion::Identity().IsIdentity());
            Assert::IsTrue(Quaternion(0, 0, 0, 1).IsIdentity());
            Assert::IsFalse(Quaternion(1, 0, 0, 1).IsIdentity());
            Assert::IsFalse(Quaternion(0, 1, 0, 1).IsIdentity());
            Assert::IsFalse(Quaternion(0, 0, 1, 1).IsIdentity());
            Assert::IsFalse(Quaternion(0, 0, 0, 0).IsIdentity());
        }

        // A test for Quaternion comparison involving NaN values
        TEST_METHOD(QuaternionEqualsNanTest)
        {
            Quaternion a(NAN, 0, 0, 0);
            Quaternion b(0, NAN, 0, 0);
            Quaternion c(0, 0, NAN, 0);
            Quaternion d(0, 0, 0, NAN);

            Assert::IsFalse(a == Quaternion(0, 0, 0, 0));
            Assert::IsFalse(b == Quaternion(0, 0, 0, 0));
            Assert::IsFalse(c == Quaternion(0, 0, 0, 0));
            Assert::IsFalse(d == Quaternion(0, 0, 0, 0));

            Assert::IsTrue(a != Quaternion(0, 0, 0, 0));
            Assert::IsTrue(b != Quaternion(0, 0, 0, 0));
            Assert::IsTrue(c != Quaternion(0, 0, 0, 0));
            Assert::IsTrue(d != Quaternion(0, 0, 0, 0));

            Assert::IsFalse(a.IsIdentity());
            Assert::IsFalse(b.IsIdentity());
            Assert::IsFalse(c.IsIdentity());
            Assert::IsFalse(d.IsIdentity());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
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

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(QuaternionTypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Quaternion>::value);
            Assert::IsFalse(std::is_pod<Quaternion>::value);
            Assert::IsFalse(std::is_trivial<Quaternion>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Quaternion>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Quaternion>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Quaternion>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Quaternion>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Quaternion>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Quaternion>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Quaternion>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Quaternion>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Quaternion>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Quaternion>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Quaternion>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Quaternion>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Quaternion>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Quaternion>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Quaternion>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Quaternion>::value);
            Assert::IsTrue(std::is_trivially_destructible<Quaternion>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Quaternion>::value);
        }
    };
}
