// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Foundation::Numerics;

namespace NumericsTests
{
    NUMERICS_TEST_CLASS(QuaternionTest)
    {
        NUMERICS_TEST_CLASS_INNER(QuaternionTest)

    public:
        // A test for dot (quaternion, quaternion)
        TEST_METHOD(QuaternionDotTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"dot did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(QuaternionLengthTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float w = 4.0f;

            quaternion target(v, w);

            float expected = 5.477226f;
            float actual;

            actual = length(target);

            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length_squared ()
        TEST_METHOD(QuaternionLengthSquaredTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            quaternion target(v, w);

            float expected = 30.0f;
            float actual;

            actual = length_squared(target);

            Assert::IsTrue(Equal(expected, actual), L"length_squared did not return the expected value.");
        }
        
        // A test for lerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionLerpTest)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 0.5f;

            quaternion expected = make_quaternion_from_axis_angle(axis, ToRadians(20.0f));
            quaternion actual;

            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = lerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }
        
        // A test for lerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionLerpTest1)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 0.0f;

            quaternion expected(a.x, a.y, a.z, a.w);
            quaternion actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }
        
        // A test for lerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionLerpTest2)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 1.0f;

            quaternion expected(b.x, b.y, b.z, b.w);
            quaternion actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionLerpTest3)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = -a;

            float t = 1.0f;

            quaternion actual = lerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == a, L"lerp did not return the expected value.");
        }

        // A test for conjugate(quaternion)
        TEST_METHOD(QuaternionConjugateTest1)
        {
            quaternion a(1, 2, 3, 4);

            quaternion expected(-1, -2, -3, 4);
            quaternion actual;

            actual = conjugate(a);
            Assert::IsTrue(Equal(expected, actual), L"conjugate did not return the expected value.");
        }

        // A test for normalize (quaternion)
        TEST_METHOD(QuaternionNormalizeTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);

            quaternion expected(0.182574168f, 0.365148336f, 0.5477225f, 0.7302967f);
            quaternion actual;

            actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }
        
        // A test for normalize (quaternion)
        TEST_METHOD(QuaternionNormalizeTest1)
        {
            quaternion a(0.0f, 0.0f, -0.0f, 0.0f);

            quaternion actual = normalize(a);
            Assert::IsTrue(isnan(actual.x) && isnan(actual.y) && isnan(actual.z) && isnan(actual.w)
                , L"normalize did not return the expected value.");
        }
        
        // A test for concatenate(quaternion, quaternion)
        TEST_METHOD(QuaternionConcatenateTest1)
        {
            quaternion b(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion a(5.0f, 6.0f, 7.0f, 8.0f);

            quaternion expected(24.0f, 48.0f, 48.0f, -6.0f);
            quaternion actual;

            actual = concatenate(a, b);
            Assert::IsTrue(Equal(expected, actual), L"concatenate did not return the expected value.");
        }

        // A test for operator - (quaternion, quaternion)
        TEST_METHOD(QuaternionSubtractionTest)
        {
            quaternion a(1.0f, 6.0f, 7.0f, 4.0f);
            quaternion b(5.0f, 2.0f, 3.0f, 8.0f);

            quaternion expected(-4.0f, 4.0f, 4.0f, -4.0f);
            quaternion actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator - did not return the expected value.");
        }

        // A test for operator * (quaternion, float)
        TEST_METHOD(QuaternionMultiplyTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 0.5f;

            quaternion expected(0.5f, 1.0f, 1.5f, 2.0f);
            quaternion actual;

            actual = a * factor;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator * did not return the expected value.");
        }

        // A test for operator * (quaternion, quaternion)
        TEST_METHOD(QuaternionMultiplyTest1)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            quaternion expected(24.0f, 48.0f, 48.0f, -6.0f);
            quaternion actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator * did not return the expected value.");
        }

        // A test for operator / (quaternion, quaternion)
        TEST_METHOD(QuaternionDivisionTest1)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            quaternion expected(-0.045977015f, -0.09195402f, -7.450581E-9f, 0.402298868f);
            quaternion actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator / did not return the expected value.");
        }

        // A test for operator + (quaternion, quaternion)
        TEST_METHOD(QuaternionAdditionTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(5.0f, 6.0f, 7.0f, 8.0f);

            quaternion expected(6.0f, 8.0f, 10.0f, 12.0f);
            quaternion actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator + did not return the expected value.");
        }

        // A test for quaternion (float, float, float, float)
        TEST_METHOD(QuaternionConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            quaternion target(x, y, z, w);

            Assert::IsTrue(Equal(target.x, x) && Equal(target.y, y) && Equal(target.z, z) && Equal(target.w, w),
                L"quaternion::constructor (x,y,z,w) did not return the expected value.");
        }
        
        // A test for quaternion (float3, float)
        TEST_METHOD(QuaternionConstructorTest1)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            quaternion target(v, w);
            Assert::IsTrue(Equal(target.x, v.x) && Equal(target.y, v.y) && Equal(target.z, v.z) && Equal(target.w, w),
                L"quaternion::constructor (float3,w) did not return the expected value.");
        }

        // A test for quaternion ()
        TEST_METHOD(QuaternionConstructorTest2)
        {
            quaternion target;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            target.x = 0;    // avoid warning about unused variable
        }

        // A test for from_axis_angle (float3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            float angle = ToRadians(30.0f);

            quaternion expected(0.0691723f, 0.1383446f, 0.207516879f, 0.9659258f);
            quaternion actual;

            actual = make_quaternion_from_axis_angle(axis, angle);
            Assert::IsTrue(Equal(expected, actual), L"make_quaternion_from_axis_angle did not return the expected value.");
        }
        
        // A test for from_axis_angle (float3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest1)
        {
            float3 axis = float3::zero();
            float angle = ToRadians(-30.0f);

            float cos = cosf(angle / 2.0f);
            quaternion actual = make_quaternion_from_axis_angle(axis, angle);

            Assert::IsTrue(actual.x == 0.0f && actual.y == 0.0f && actual.z == 0.0f && Equal(cos, actual.w)
                , L"make_quaternion_from_axis_angle did not return the expected value.");
        }

        // A test for from_axis_angle (float3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest2)
        {
            float3 axis(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(750.0f);

            quaternion actual1 = make_quaternion_from_axis_angle(axis, angle1);
            quaternion actual2 = make_quaternion_from_axis_angle(axis, angle2);
            Assert::IsTrue(Equal(actual1, actual2), L"make_quaternion_from_axis_angle did not return the expected value.");
        }

        // A test for from_axis_angle (float3, float)
        TEST_METHOD(QuaternionCreateFromAxisAngleTest3)
        {
            float3 axis(1, 0, 0);
            float angle1 = ToRadians(30.0f);
            float angle2 = ToRadians(390.0f);

            quaternion actual1 = make_quaternion_from_axis_angle(axis, angle1);
            quaternion actual2 = make_quaternion_from_axis_angle(axis, angle2);
            actual1.x = -actual1.x;
            actual1.w = -actual1.w;

            Assert::IsTrue(Equal(actual1, actual2), L"make_quaternion_from_axis_angle did not return the expected value.");
        }

        TEST_METHOD(QuaternionCreateFromYawPitchRollTest1)
        {
            float yawAngle = ToRadians(30.0f);
            float pitchAngle = ToRadians(40.0f);
            float rollAngle = ToRadians(50.0f);

            quaternion yaw = make_quaternion_from_axis_angle(float3::unit_y(), yawAngle);
            quaternion pitch = make_quaternion_from_axis_angle(float3::unit_x(), pitchAngle);
            quaternion roll = make_quaternion_from_axis_angle(float3::unit_z(), rollAngle);

            quaternion expected = yaw * pitch * roll;
            quaternion actual = make_quaternion_from_yaw_pitch_roll(yawAngle, pitchAngle, rollAngle);
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

                        quaternion yaw = make_quaternion_from_axis_angle(float3::unit_y(), yawRad);
                        quaternion pitch = make_quaternion_from_axis_angle(float3::unit_x(), pitchRad);
                        quaternion roll = make_quaternion_from_axis_angle(float3::unit_z(), rollRad);

                        quaternion expected = yaw * pitch * roll;
                        quaternion actual = make_quaternion_from_yaw_pitch_roll(yawRad, pitchRad, rollRad);
                        Assert::IsTrue(Equal(expected, actual));
                    }
                }
            }
        }

        // A test for slerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionSlerpTest)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 0.5f;

            quaternion expected = make_quaternion_from_axis_angle(axis, ToRadians(20.0f));
            quaternion actual;

            actual = slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"slerp did not return the expected value.");

            // Case a and b are same.
            expected = a;
            actual = slerp(a, a, t);
            Assert::IsTrue(Equal(expected, actual), L"slerp did not return the expected value.");
        }

        // A test for slerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionSlerpTest1)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 0.0f;

            quaternion expected(a.x, a.y, a.z, a.w);
            quaternion actual = slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"slerp did not return the expected value.");
        }

        // A test for slerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionSlerpTest2)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 1.0f;

            quaternion expected(b.x, b.y, b.z, b.w);
            quaternion actual = slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"slerp did not return the expected value.");
        }

        // A test for slerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionSlerpTest3)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = -a;

            float t = 1.0f;

            quaternion expected = a;
            quaternion actual = slerp(a, b, t);
            // Note that in quaternion world, Q == -Q. In the case of quaternions dot product is zero, 
            // one of the quaternion will be flipped to compute the shortest distance. When t = 1, we
            // expect the result to be the same as quaternion b but flipped.
            Assert::IsTrue(actual == expected, L"slerp did not return the expected value.");
        }

        // A test for slerp (quaternion, quaternion, float)
        TEST_METHOD(QuaternionSlerpTest4)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion a = make_quaternion_from_axis_angle(axis, ToRadians(10.0f));
            quaternion b = -make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float t = 0.0f;

            quaternion expected(a.x, a.y, a.z, a.w);
            quaternion actual = slerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"slerp did not return the expected value.");
        }

        // A test for operator - (quaternion)
        TEST_METHOD(QuaternionUnaryNegationTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);

            quaternion expected(-1.0f, -2.0f, -3.0f, -4.0f);
            quaternion actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"quaternion::operator - did not return the expected value.");
        }

        // A test for inverse (quaternion)
        TEST_METHOD(QuaternionInverseTest)
        {
            quaternion a(5.0f, 6.0f, 7.0f, 8.0f);

            quaternion expected(-0.0287356321f, -0.03448276f, -0.0402298868f, 0.04597701f);
            quaternion actual;

            actual = inverse(a);
            Assert::AreEqual(expected, actual, L"inverse did not return the expected value.");
        }

        // A test for inverse (quaternion)
        TEST_METHOD(QuaternionInverseTest1)
        {
            quaternion a(0, 0, 0, 0);
            quaternion actual = inverse(a);

            Assert::IsTrue(isnan(actual.x) && isnan(actual.y) && isnan(actual.z) && isnan(actual.w)
                , L"inverse did not return the expected value.");
        }

        // A test for operator +=
        TEST_METHOD(QuaternionOperatorAddEqualsTest)
        {
            quaternion a(1, 2, 3, 4);
            quaternion b(5, 6, 7, 8);
            quaternion expected = a + b;
            
            // In-place += operation.
            quaternion& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(QuaternionOperatorSubtractEqualsTest)
        {
            quaternion a(1, 2, 3, 4);
            quaternion b(5, 6, 7, 8);
            quaternion expected = a - b;
            
            // In-place -= operation.
            quaternion& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(QuaternionOperatorMultiplyEqualsTest)
        {
            quaternion a(1, 2, 3, 4);
            quaternion b(5, 6, 7, 8);
            quaternion expected = a * b;
            
            // In-place *= operation.
            quaternion& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(QuaternionOperatorMultiplyEqualsScalarTest)
        {
            quaternion a(1, 2, 3, 4);
            float b = 5;
            quaternion expected = a * b;
            
            // In-place *= operation.
            quaternion& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(QuaternionOperatorDivideEqualsTest)
        {
            quaternion a(1, 2, 3, 4);
            quaternion b(5, 6, 7, 8);
            quaternion expected = a / b;
            
            // In-place /= operation.
            quaternion& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b / b;
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator != (quaternion, quaternion)
        TEST_METHOD(QuaternionInequalityTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"quaternion::operator != did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"quaternion::operator != did not return the expected value.");
        }

        // A test for operator == (quaternion, quaternion)
        TEST_METHOD(QuaternionEqualityTest)
        {
            quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
            quaternion b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"quaternion::operator == did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"quaternion::operator == did not return the expected value.");
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest1)
        {
            float4x4 matrix = float4x4::identity();

            quaternion expected(0.0f, 0.0f, 0.0f, 1.0f);
            quaternion actual = make_quaternion_from_rotation_matrix(matrix);
            Assert::IsTrue(Equal(expected, actual), L"make_quaternion_from_rotation_matrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            float4x4 m2 = make_float4x4_from_quaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"make_quaternion_from_rotation_matrix did not return the expected value.");
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest2)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                float4x4 matrix = make_float4x4_rotation_x(angle);

                quaternion expected = make_quaternion_from_axis_angle(float3::unit_x(), angle);
                quaternion actual = make_quaternion_from_rotation_matrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                float4x4 m2 = make_float4x4_from_quaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");
            }
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest3)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                float4x4 matrix = make_float4x4_rotation_y(angle);

                quaternion expected = make_quaternion_from_axis_angle(float3::unit_y(), angle);
                quaternion actual = make_quaternion_from_rotation_matrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                float4x4 m2 = make_float4x4_from_quaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");
            }
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest4)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                float4x4 matrix = make_float4x4_rotation_z(angle);

                quaternion expected = make_quaternion_from_axis_angle(float3::unit_z(), angle);
                quaternion actual = make_quaternion_from_rotation_matrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                float4x4 m2 = make_float4x4_from_quaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");
            }
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixTest5)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                float4x4 matrix = make_float4x4_rotation_x(angle) * make_float4x4_rotation_y(angle) * make_float4x4_rotation_z(angle);

                quaternion expected =
                    make_quaternion_from_axis_angle(float3::unit_z(), angle) *
                    make_quaternion_from_axis_angle(float3::unit_y(), angle) *
                    make_quaternion_from_axis_angle(float3::unit_x(), angle);

                quaternion actual = make_quaternion_from_rotation_matrix(matrix);
                Assert::IsTrue(EqualRotation(expected, actual),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");

                // make sure convert back to matrix is same as we passed matrix.
                float4x4 m2 = make_float4x4_from_quaternion(actual);
                Assert::IsTrue(Equal(matrix, m2),
                    L"make_quaternion_from_rotation_matrix did not return the expected value.");
            }
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest1)
        {
            float angle = ToRadians(180.0f);
            float4x4 matrix = make_float4x4_rotation_y(angle) * make_float4x4_rotation_z(angle);

            quaternion expected = make_quaternion_from_axis_angle(float3::unit_z(), angle) * make_quaternion_from_axis_angle(float3::unit_y(), angle);
            quaternion actual = make_quaternion_from_rotation_matrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"make_quaternion_from_rotation_matrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            float4x4 m2 = make_float4x4_from_quaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"make_quaternion_from_rotation_matrix did not return the expected value.");
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest2)
        {
            float angle = ToRadians(180.0f);
            float4x4 matrix = make_float4x4_rotation_x(angle) * make_float4x4_rotation_z(angle);

            quaternion expected = make_quaternion_from_axis_angle(float3::unit_z(), angle) * make_quaternion_from_axis_angle(float3::unit_x(), angle);
            quaternion actual = make_quaternion_from_rotation_matrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"make_quaternion_from_rotation_matrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            float4x4 m2 = make_float4x4_from_quaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"make_quaternion_from_rotation_matrix did not return the expected value.");
        }

        // A test for from_rotation_matrix (float4x4)
        TEST_METHOD(QuaternionFromRotationMatrixWithScaledMatrixTest3)
        {
            float angle = ToRadians(180.0f);
            float4x4 matrix = make_float4x4_rotation_x(angle) * make_float4x4_rotation_y(angle);

            quaternion expected = make_quaternion_from_axis_angle(float3::unit_y(), angle) * make_quaternion_from_axis_angle(float3::unit_x(), angle);
            quaternion actual = make_quaternion_from_rotation_matrix(matrix);
            Assert::IsTrue(EqualRotation(expected, actual), L"make_quaternion_from_rotation_matrix did not return the expected value.");

            // make sure convert back to matrix is same as we passed matrix.
            float4x4 m2 = make_float4x4_from_quaternion(actual);
            Assert::IsTrue(Equal(matrix, m2), L"make_quaternion_from_rotation_matrix did not return the expected value.");
        }

        // A test for identity
        TEST_METHOD(QuaternionIdentityTest)
        {
            quaternion val(0, 0, 0, 1);
            Assert::AreEqual(val, quaternion::identity(), L"quaternion::identity was not set correctly.");
        }

        // A test for is_identity()
        TEST_METHOD(QuaternionIsIdentityTest)
        {
            Assert::IsTrue(is_identity(quaternion::identity()));
            Assert::IsTrue(is_identity(quaternion(0, 0, 0, 1)));
            Assert::IsFalse(is_identity(quaternion(1, 0, 0, 1)));
            Assert::IsFalse(is_identity(quaternion(0, 1, 0, 1)));
            Assert::IsFalse(is_identity(quaternion(0, 0, 1, 1)));
            Assert::IsFalse(is_identity(quaternion(0, 0, 0, 0)));
        }

        // A test for quaternion comparison involving NaN values
        TEST_METHOD(QuaternionEqualsNanTest)
        {
            quaternion a(NAN, 0, 0, 0);
            quaternion b(0, NAN, 0, 0);
            quaternion c(0, 0, NAN, 0);
            quaternion d(0, 0, 0, NAN);

            Assert::IsFalse(a == quaternion(0, 0, 0, 0));
            Assert::IsFalse(b == quaternion(0, 0, 0, 0));
            Assert::IsFalse(c == quaternion(0, 0, 0, 0));
            Assert::IsFalse(d == quaternion(0, 0, 0, 0));

            Assert::IsTrue(a != quaternion(0, 0, 0, 0));
            Assert::IsTrue(b != quaternion(0, 0, 0, 0));
            Assert::IsTrue(c != quaternion(0, 0, 0, 0));
            Assert::IsTrue(d != quaternion(0, 0, 0, 0));

            Assert::IsFalse(is_identity(a));
            Assert::IsFalse(is_identity(b));
            Assert::IsFalse(is_identity(c));
            Assert::IsFalse(is_identity(d));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
        }

        struct Quaternion_2x
        {
            quaternion a;
            quaternion b;
        };

        struct QuaternionPlusFloat
        {
            quaternion v;
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
            Assert::AreEqual(size_t(16), sizeof(quaternion));
            Assert::AreEqual(size_t(32), sizeof(Quaternion_2x));
            Assert::AreEqual(size_t(20), sizeof(QuaternionPlusFloat));
            Assert::AreEqual(size_t(40), sizeof(QuaternionPlusFloat_2x));
            Assert::AreEqual(sizeof(quaternion), sizeof(DirectX::XMFLOAT4));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(QuaternionFieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(quaternion, x));
            Assert::AreEqual(size_t(4), offsetof(quaternion, y));
            Assert::AreEqual(size_t(8), offsetof(quaternion, z));
            Assert::AreEqual(size_t(12), offsetof(quaternion, w));
        }

        // A test of quaternion -> DirectXMath interop
        TEST_METHOD(QuaternionLoadTest)
        {
            quaternion a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadQuaternion(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
            Assert::AreEqual(a.w, c.w);
        }

        // A test of DirectXMath -> quaternion interop
        TEST_METHOD(QuaternionStoreTest)
        {
            DirectX::XMFLOAT4 a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&a);
            quaternion c;
            DirectX::XMStoreQuaternion(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
            Assert::AreEqual(a.w, c.w);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(QuaternionTypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<quaternion>::value);
            Assert::IsFalse(std::is_pod<quaternion>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<quaternion>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<quaternion>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<quaternion>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<quaternion>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<quaternion>::value);

            // Move constructor is present.
            Assert::IsTrue(std::is_move_constructible<quaternion>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<quaternion>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<quaternion>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<quaternion>::value);

            // Move assignment is present.
            Assert::IsTrue(std::is_move_assignable<quaternion>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<quaternion>::value);
            Assert::IsTrue(std::is_trivially_destructible<quaternion>::value);
        }


#ifndef DISABLE_NUMERICS_INTEROP_TESTS

        // A test to validate interop between WindowsNumerics.h (Windows::Foundation::Numerics) and the WinRT struct types
        TEST_METHOD(QuaternionWinRTInteropTest)
        {
            quaternion a(23, 42, 100, -1);

            NUMERICS_ABI_NAMESPACE::Quaternion b = a;

            Assert::AreEqual(a.x, b.X);
            Assert::AreEqual(a.y, b.Y);
            Assert::AreEqual(a.z, b.Z);
            Assert::AreEqual(a.w, b.W);

            quaternion c = b;

            Assert::AreEqual(a, c);
        }

#endif
    };
}
