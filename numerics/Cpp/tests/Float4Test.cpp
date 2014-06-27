//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Foundation::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(Float4Test)
    {
    public:
        // A test for distance_squared (float4, float4)
        TEST_METHOD(Float4DistanceSquaredTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 64.0f;
            float actual;

            actual = distance_squared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance_squared did not return the expected value.");
        }

        // A test for distance (float4, float4)
        TEST_METHOD(Float4DistanceTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 8.0f;
            float actual;

            actual = distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance did not return the expected value.");
        }

        // A test for distance (float4, float4)
        TEST_METHOD(Float4DistanceTest1)
        {
            float4 a(float2(1.051f, 2.05f), 3.478f, 1.0f);
            float4 b(float3(1.051f, 2.05f, 3.478f), 0.0f);
            b.w = 1.0f;

            float actual = distance(a, b);
            Assert::AreEqual(0.0f, actual, L"distance did not return the expected value.");
        }

        // A test for dot (float4, float4)
        TEST_METHOD(Float4DotTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"dot did not return the expected value.");
        }

        // A test for dot (float4, float4)
        TEST_METHOD(Float4DotTest1)
        {
            float3 a(1.55f, 1.55f, 1);
            float3 b(2.5f, 3, 1.5f);
            float3 c = cross(a, b);

            float4 d(a, 0);
            float4 e(c, 0);

            float actual = dot(d, e);
            Assert::IsTrue(Equal(0.0f, actual), L"dot did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float4LengthTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            float4 target(a, w);

            float expected = sqrtf(30.0f);
            float actual;

            actual = length(target);

            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float4LengthTest1)
        {
            float4 target(0);

            float expected = 0.0f;
            float actual = length(target);

            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length_squared ()
        TEST_METHOD(Float4LengthSquaredTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            float4 target(a, w);

            float expected = 30;
            float actual;

            actual = length_squared(target);

            Assert::IsTrue(Equal(expected, actual), L"length_squared did not return the expected value.");
        }

        // A test for min (float4, float4)
        TEST_METHOD(Float4MinTest)
        {
            float4 a(-1.0f, 4.0f, -3.0f, 1000.0f);
            float4 b(2.0f, 1.0f, -1.0f, 0.0f);

            float4 expected(-1.0f, 1.0f, -3.0f, 0.0f);
            float4 actual;
            actual = min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"min did not return the expected value.");
        }

        // A test for max (float4, float4)
        TEST_METHOD(Float4MaxTest)
        {
            float4 a(-1.0f, 4.0f, -3.0f, 1000.0f);
            float4 b(2.0f, 1.0f, -1.0f, 0.0f);

            float4 expected(2.0f, 4.0f, -1.0f, 1000.0f);
            float4 actual;
            actual = max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"max did not return the expected value.");
        }
        
        TEST_METHOD(Float4MinMaxCodeCoverageTest)
        {
            float4 minV = float4::zero();
            float4 maxV = float4::one();
            float4 actual;

            // Min.
            actual = min(minV, maxV);
            Assert::AreEqual(actual, minV);

            actual = min(maxV, minV);
            Assert::AreEqual(actual, minV);

            // Max.
            actual = max(minV, maxV);
            Assert::AreEqual(actual, maxV);

            actual = max(maxV, minV);
            Assert::AreEqual(actual, maxV);
        }

        // A test for clamp (float4, float4, float4)
        TEST_METHOD(Float4ClampTest)
        {
            float4 a(0.5f, 0.3f, 0.33f, 0.44f);
            float4 min(0.0f, 0.1f, 0.13f, 0.14f);
            float4 max(1.0f, 1.1f, 1.13f, 1.14f);

            // Normal case.
            // Case N1: specfied value is in the range.
            float4 expected(0.5f, 0.3f, 0.33f, 0.44f);
            float4 actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = float4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = max;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = float4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N4: combination case.
            a = float4(-2.0f, 0.5f, 4.0f, -5.0f);
            expected = float4(min.x, a.y, max.z, min.w);
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = float4(0.0f, 0.1f, 0.13f, 0.14f);
            min = float4(1.0f, 1.1f, 1.13f, 1.14f);

            // Case W1: specfied value is in the range.
            a = float4(0.5f, 0.3f, 0.33f, 0.44f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = float4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = float4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float t = 0.5f;

            float4 expected(3.0f, 4.0f, 5.0f, 6.0f);
            float4 actual;

            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest1)
        {
            float4 a(float3(1.0f, 2.0f, 3.0f), 4.0f);
            float4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.0f;
            float4 expected(1.0f, 2.0f, 3.0f, 4.0f);
            float4 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest2)
        {
            float4 a(float3(1.0f, 2.0f, 3.0f), 4.0f);
            float4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 1.0f;
            float4 expected(4.0f, 5.0f, 6.0f, 7.0f);
            float4 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest3)
        {
            float4 a(float3(0.0f, 0.0f, 0.0f), 0.0f);
            float4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 2.0f;
            float4 expected(8.0f, 10.0f, 12.0f, 14.0f);
            float4 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest4)
        {
            float4 a(float3(0.0f, 0.0f, 0.0f), 0.0f);
            float4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = -2.0f;
            float4 expected = -(b * 2);
            float4 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float4, float4, float)
        TEST_METHOD(Float4LerpTest5)
        {
            float4 a(4.0f, 5.0f, 6.0f, 7.0f);
            float4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.85f;
            float4 expected = a;
            float4 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for transform (float2, float4x4)
        TEST_METHOD(Float4TransformTest1)
        {
            float2 v(1.0f, 2.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float4 expected(10.316987f, 22.183012f, 30.3660259f, 1.0f);
            float4 actual;

            actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, float4x4)
        TEST_METHOD(Float4TransformTest2)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float4 expected(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            float4 actual;

            actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for transform (float4, float4x4)
        TEST_METHOD(Float4TransformVector4Test)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float4 expected(2.19198728f, 1.53349376f, 2.61602545f, 0.0f);
            float4 actual;

            actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");

            // 
            v.w = 1.0f;

            expected = float4(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float4, float4x4)
        TEST_METHOD(Float4TransformVector4Test1)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);
            float4x4 m(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4 expected(0, 0, 0, 0);

            float4 actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float4, float4x4)
        TEST_METHOD(Float4TransformVector4Test2)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);
            float4x4 m = float4x4::identity();
            float4 expected(1.0f, 2.0f, 3.0f, 0.0f);

            float4 actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, float4x4)
        TEST_METHOD(Float4TransformVector3Test)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float4 expected = transform(float4(v, 1.0f), m);
            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, float4x4)
        TEST_METHOD(Float4TransformVector3Test1)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float4x4 m(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4 expected(0, 0, 0, 0);

            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, float4x4)
        TEST_METHOD(Float4TransformVector3Test2)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float4x4 m = float4x4::identity();
            float4 expected(1.0f, 2.0f, 3.0f, 1.0f);

            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, float4x4)
        TEST_METHOD(Float4TransformVector2Test)
        {
            float2 v(1.0f, 2.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float4 expected = transform(float4(v, 0.0f, 1.0f), m);
            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, float4x4)
        TEST_METHOD(Float4TransformVector2Test1)
        {
            float2 v(1.0f, 2.0f);
            float4x4 m(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4 expected(0, 0, 0, 0);

            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, float4x4)
        TEST_METHOD(Float4TransformVector2Test2)
        {
            float2 v(1.0f, 2.0f);
            float4x4 m = float4x4::identity();
            float4 expected(1.0f, 2.0f, 0, 1.0f);

            float4 actual = transform4(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float4TransformVector2QuatanionTest)
        {
            float2 v(1.0f, 2.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));

            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4 expected = transform4(v, m);
            float4 actual;

            actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float4TransformVector3Quaternion)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4 expected = transform4(v, m);
            float4 actual;

            actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for transform (float4, quaternion)
        TEST_METHOD(Float4TransformVector4QuaternionTest)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4 expected = transform(v, m);
            float4 actual;

            actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");

            // 
            v.w = 1.0f;
            expected.w = 1.0f;
            actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float4, quaternion)
        TEST_METHOD(Float4TransformVector4QuaternionTest1)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);
            quaternion q(0, 0, 0, 0);
            float4 expected = v;

            float4 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float4, quaternion)
        TEST_METHOD(Float4TransformVector4QuaternionTest2)
        {
            float4 v(1.0f, 2.0f, 3.0f, 0.0f);
            quaternion q = quaternion::identity();
            float4 expected(1.0f, 2.0f, 3.0f, 0.0f);

            float4 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float4TransformVector3QuaternionTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4 expected = transform4(v, m);
            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float4TransformVector3QuaternionTest1)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            quaternion q(0, 0, 0, 0);
            float4 expected(v, 1.0f);

            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float4TransformVector3QuaternionTest2)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            quaternion q = quaternion::identity();
            float4 expected(1.0f, 2.0f, 3.0f, 1.0f);

            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float4TransformVector2QuaternionTest)
        {
            float2 v(1.0f, 2.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4 expected = transform4(v, m);
            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float4TransformVector2QuaternionTest1)
        {
            float2 v(1.0f, 2.0f);
            quaternion q(0, 0, 0, 0);
            float4 expected(1.0f, 2.0f, 0, 1.0f);

            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, float4x4)
        TEST_METHOD(Float4TransformVector2QuaternionTest2)
        {
            float2 v(1.0f, 2.0f);
            quaternion q = quaternion::identity();
            float4 expected(1.0f, 2.0f, 0, 1.0f);

            float4 actual = transform4(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for normalize (float4)
        TEST_METHOD(Float4NormalizeTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float4 expected(
                0.1825741858350553711523232609336f,
                0.3651483716701107423046465218672f,
                0.5477225575051661134569697828008f,
                0.7302967433402214846092930437344f);
            float4 actual;

            actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }

        // A test for normalize (float4)
        TEST_METHOD(Float4NormalizeTest1)
        {
            float4 a(1.0f, 0.0f, 0.0f, 0.0f);

            float4 expected(1.0f, 0.0f, 0.0f, 0.0f);
            float4 actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }

        // A test for normalize (float4)
        TEST_METHOD(Float4NormalizeTest2)
        {
            float4 a(0.0f, 0.0f, 0.0f, 0.0f);

            float4 expected(0.0f, 0.0f, 0.0f, 0.0f);
            float4 actual = normalize(a);
            Assert::IsTrue(isnan(actual.x) && isnan(actual.y) && isnan(actual.z) && isnan(actual.w), L"normalize did not return the expected value.");
        }

        // A test for operator - (float4)
        TEST_METHOD(Float4UnaryNegationTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float4 expected(-1.0f, -2.0f, -3.0f, -4.0f);
            float4 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator - did not return the expected value.");
        }

        // A test for operator - (float4, float4)
        TEST_METHOD(Float4SubtractionTest)
        {
            float4 a(1.0f, 6.0f, 3.0f, 4.0f);
            float4 b(5.0f, 2.0f, 3.0f, 9.0f);

            float4 expected(-4.0f, 4.0f, 0.0f, -5.0f);
            float4 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator - did not return the expected value.");
        }

        // A test for operator * (float4, float)
        TEST_METHOD(Float4MultiplyTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;

            float4 expected(2.0f, 4.0f, 6.0f, 8.0f);
            float4 actual;

            actual = a * factor;
            Assert::IsTrue(Equal(expected, actual), L"float4::operator * did not return the expected value.");
        }

        // A test for operator * (float, float4)
        TEST_METHOD(Float4MultiplyTest4)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;
            float4 expected(2.0f, 4.0f, 6.0f, 8.0f);
            float4 actual;

            actual = factor * a;
            Assert::IsTrue(Equal(expected, actual), L"float4::operator * did not return the expected value.");
        }

        // A test for operator * (float4, float4)
        TEST_METHOD(Float4MultiplyTest1)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float4 expected(5.0f, 12.0f, 21.0f, 32.0f);
            float4 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator * did not return the expected value.");
        }

        // A test for operator / (float4, float)
        TEST_METHOD(Float4DivisionTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float div = 2.0f;

            float4 expected(0.5f, 1.0f, 1.5f, 2.0f);
            float4 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator / did not return the expected value.");
        }

        // A test for operator / (float4, float4)
        TEST_METHOD(Float4DivisionTest1)
        {
            float4 a(1.0f, 6.0f, 7.0f, 4.0f);
            float4 b(5.0f, 2.0f, 3.0f, 8.0f);

            float4 expected(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            float4 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator / did not return the expected value.");
        }

        // A test for operator / (float4, float4)
        TEST_METHOD(Float4DivisionTest2)
        {
            float4 a(-2.0f, 3.0f, FLT_MAX, NAN);

            float div = 0.0f;

            float4 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.x), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.y), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.z), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(isnan(actual.w), L"float4::operator / did not return the expected value.");
        }

        // A test for operator / (float4, float4)
        TEST_METHOD(Float4DivisionTest3)
        {
            float4 a(0.047f, -3.0f, -std::numeric_limits<float>::infinity(), -FLT_MAX);
            float4 b(0);

            float4 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.x), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.y), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.z), L"float4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.w), L"float4::operator / did not return the expected value.");
        }

        // A test for operator + (float4, float4)
        TEST_METHOD(Float4AdditionTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float4 expected(6.0f, 8.0f, 10.0f, 12.0f);
            float4 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"float4::operator + did not return the expected value.");
        }

        // A test for float4 (float, float, float, float)
        TEST_METHOD(Float4ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            float4 target(x, y, z, w);

            Assert::IsTrue(Equal(target.x, x) && Equal(target.y, y) && Equal(target.z, z) && Equal(target.w, w),
                L"float4 constructor(x,y,z,w) did not return the expected value.");
        }

        // A test for float4 (float2, float, float)
        TEST_METHOD(Float4ConstructorTest1)
        {
            float2 a(1.0f, 2.0f);
            float z = 3.0f;
            float w = 4.0f;

            float4 target(a, z, w);
            Assert::IsTrue(Equal(target.x, a.x) && Equal(target.y, a.y) && Equal(target.z, z) && Equal(target.w, w),
                L"float4 constructor(float2,z,w) did not return the expected value.");
        }

        // A test for float4 (float3, float)
        TEST_METHOD(Float4ConstructorTest2)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            float4 target(a, w);

            Assert::IsTrue(Equal(target.x, a.x) && Equal(target.y, a.y) && Equal(target.z, a.z) && Equal(target.w, w),
                L"float4 constructor(float3,w) did not return the expected value.");
        }

        // A test for float4 ()
        TEST_METHOD(Float4ConstructorTest4)
        {
            float4 a;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            a.x = 0;    // avoid warning about unused variable
        }

        // A test for float4 ()
        TEST_METHOD(Float4ConstructorTest5)
        {
            float4 target(NAN, FLT_MAX, std::numeric_limits<float>::infinity(), FLT_EPSILON);

            Assert::IsTrue(isnan(target.x), L"float4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.y, L"float4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.z), L"float4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_EPSILON == target.w, L"float4::constructor (float, float, float, float) did not return the expected value.");
        }

        // A test for operator != (float4, float4)
        TEST_METHOD(Float4InequalityTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"float4::operator != did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"float4::operator != did not return the expected value.");
        }

        // A test for operator == (float4, float4)
        TEST_METHOD(Float4EqualityTest)
        {
            float4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float4 b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"float4::operator == did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"float4::operator == did not return the expected value.");
        }

        // A test for unit_w
        TEST_METHOD(Float4UnitWTest)
        {
            float4 val(0.0f, 0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, float4::unit_w(), L"float4::unit_w was not set correctly.");
        }

        // A test for unit_x()
        TEST_METHOD(Float4UnitXTest)
        {
            float4 val(1.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, float4::unit_x(), L"float4::unit_x() was not set correctly.");
        }

        // A test for unit_y()
        TEST_METHOD(Float4UnitYTest)
        {
            float4 val(0.0f, 1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, float4::unit_y(), L"float4::unit_y() was not set correctly.");
        }

        // A test for unit_z
        TEST_METHOD(Float4UnitZTest)
        {
            float4 val(0.0f, 0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, float4::unit_z(), L"float4::unit_z was not set correctly.");
        }

        // A test for one
        TEST_METHOD(Float4OneTest)
        {
            float4 val(1.0f, 1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, float4::one(), L"float4::one was not set correctly.");
        }

        // A test for zero
        TEST_METHOD(Float4ZeroTest)
        {
            float4 val(0.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, float4::zero(), L"float4::zero was not set correctly.");
        }

        // A test for operator +=
        TEST_METHOD(Float4OperatorAddEqualsTest)
        {
            float4 a(1, 2, 3, 4);
            float4 b(5, 6, 7, 8);
            float4 expected = a + b;
            
            // In-place += operation.
            float4& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Float4OperatorSubtractEqualsTest)
        {
            float4 a(1, 2, 3, 4);
            float4 b(5, 6, 7, 8);
            float4 expected = a - b;
            
            // In-place -= operation.
            float4& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float4OperatorMultiplyEqualsTest)
        {
            float4 a(1, 2, 3, 4);
            float4 b(5, 6, 7, 8);
            float4 expected = a * b;
            
            // In-place *= operation.
            float4& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float4OperatorMultiplyEqualsScalarTest)
        {
            float4 a(1, 2, 3, 4);
            float b = 5;
            float4 expected = a * b;
            
            // In-place *= operation.
            float4& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(Float4OperatorDivideEqualsTest)
        {
            float4 a(1, 2, 3, 4);
            float4 b(5, 6, 7, 8);
            float4 expected = a / b;
            
            // In-place /= operation.
            float4& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b / b;
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator /=
        TEST_METHOD(Float4OperatorDivideEqualsScalarTest)
        {
            float4 a(1, 2, 3, 4);
            float b = 5;
            float4 expected = a / b;
            
            // In-place /= operation.
            float4& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for float4 (float)
        TEST_METHOD(Float4ConstructorTest6)
        {
            float value = 1.0f;
            float4 target(value);

            float4 expected(value, value, value, value);
            Assert::AreEqual(expected, target, L"float4::cstr did not return the expected value.");

            value = 2.0f;
            target = float4(value);
            expected = float4(value, value, value, value);
            Assert::AreEqual(expected, target, L"float4::cstr did not return the expected value.");
        }

        // A test for float4 comparison involving NaN values
        TEST_METHOD(Float4EqualsNanTest)
        {
            float4 a(NAN, 0, 0, 0);
            float4 b(0, NAN, 0, 0);
            float4 c(0, 0, NAN, 0);
            float4 d(0, 0, 0, NAN);

            Assert::IsFalse(a == float4::zero());
            Assert::IsFalse(b == float4::zero());
            Assert::IsFalse(c == float4::zero());
            Assert::IsFalse(d == float4::zero());

            Assert::IsTrue(a != float4::zero());
            Assert::IsTrue(b != float4::zero());
            Assert::IsTrue(c != float4::zero());
            Assert::IsTrue(d != float4::zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
        }

        struct Vector4_2x
        {
            float4 a;
            float4 b;
        };

        struct Vector4PlusFloat
        {
            float4 v;
            float f;
        };

        struct Vector4PlusFloat_2x
        {
            Vector4PlusFloat a;
            Vector4PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Float4SizeofTest)
        {
            Assert::AreEqual(size_t(16), sizeof(float4));
            Assert::AreEqual(size_t(32), sizeof(Vector4_2x));
            Assert::AreEqual(size_t(20), sizeof(Vector4PlusFloat));
            Assert::AreEqual(size_t(40), sizeof(Vector4PlusFloat_2x));
            Assert::AreEqual(sizeof(float4), sizeof(DirectX::XMFLOAT4));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Float4FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(float4, x));
            Assert::AreEqual(size_t(4), offsetof(float4, y));
            Assert::AreEqual(size_t(8), offsetof(float4, z));
            Assert::AreEqual(size_t(12), offsetof(float4, w));
        }

        // A test of float4 -> DirectXMath interop
        TEST_METHOD(Float4LoadTest)
        {
            float4 a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
            Assert::AreEqual(a.w, c.w);
        }

        // A test of DirectXMath -> float4 interop
        TEST_METHOD(Float4StoreTest)
        {
            DirectX::XMFLOAT4 a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&a);
            float4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
            Assert::AreEqual(a.w, c.w);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Float4TypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<float4>::value);
            Assert::IsFalse(std::is_pod<float4>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<float4>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<float4>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<float4>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<float4>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<float4>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<float4>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<float4>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<float4>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<float4>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<float4>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<float4>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<float4>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<float4>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<float4>::value);
            Assert::IsTrue(std::is_trivially_destructible<float4>::value);
        }

        // A test to validate interop between WindowsNumerics.h (Windows::Foundation::Numerics) and WinRT (Microsoft::Graphics::Canvas::Numerics)
        TEST_METHOD(Float4WinRTInteropTest)
        {
            float4 a(23, 42, 100, -1);

            Microsoft::Graphics::Canvas::Numerics::Vector4 b = a;

            Assert::AreEqual(a.x, b.X);
            Assert::AreEqual(a.y, b.Y);
            Assert::AreEqual(a.z, b.Z);
            Assert::AreEqual(a.w, b.W);

            float4 c = b;

            Assert::AreEqual(a, c);
        }
    };
}
