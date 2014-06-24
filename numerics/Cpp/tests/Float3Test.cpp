//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Foundation::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(Float3Test)
    {
    public:
        // A test for cross (float3, float3)
        TEST_METHOD(Float3CrossTest)
        {
            float3 a(1.0f, 0.0f, 0.0f);
            float3 b(0.0f, 1.0f, 0.0f);

            float3 expected(0.0f, 0.0f, 1.0f);
            float3 actual;

            actual = cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"cross did not return the expected value.");
        }

        // A test for cross (float3, float3)
        TEST_METHOD(Float3CrossTest1)
        {
            float3 a(0.0f, 1.0f, 0.0f);
            float3 b(0.0f, 1.0f, 0.0f);

            float3 expected(0.0f, 0.0f, 0.0f);
            float3 actual = cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"cross did not return the expected value.");
        }

        // A test for distance (float3, float3)
        TEST_METHOD(Float3DistanceTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float expected = sqrtf(27);
            float actual;

            actual = distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance did not return the expected value.");
        }

        // A test for distance (float3, float3)
        TEST_METHOD(Float3DistanceTest1)
        {
            float3 a(1.051f, 2.05f, 3.478f);
            float3 b(float2(1.051f, 0.0f), 1);
            b.y = 2.05f;
            b.z = 3.478f;

            float actual = distance(a, b);
            Assert::AreEqual(0.0f, actual, L"distance did not return the expected value.");
        }

        // A test for distance_squared (float3, float3)
        TEST_METHOD(Float3DistanceSquaredTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float expected = 27.0f;
            float actual;

            actual = distance_squared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance_squared did not return the expected value.");
        }

        // A test for dot (float3, float3)
        TEST_METHOD(Float3DotTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float expected = 32.0f;
            float actual;

            actual = dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"dot did not return the expected value.");
        }

        // A test for dot (float3, float3)
        TEST_METHOD(Float3DotTest1)
        {
            float3 a(1.55f, 1.55f, 1);
            float3 b(2.5f, 3, 1.5f);
            float3 c = cross(a, b);

            float expected = 0.0f;
            float actual1 = dot(a, c);
            float actual2 = dot(b, c);
            Assert::IsTrue(Equal(expected, actual1), L"dot did not return the expected value.");
            Assert::IsTrue(Equal(expected, actual2), L"dot did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float3LengthTest)
        {
            float2 a(1.0f, 2.0f);

            float z = 3.0f;

            float3 target(a, z);

            float expected = sqrtf(14.0f);
            float actual;

            actual = length(target);
            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float3LengthTest1)
        {
            float3 target(0);

            float expected = 0.0f;
            float actual = length(target);
            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length_squared ()
        TEST_METHOD(Float3LengthSquaredTest)
        {
            float2 a(1.0f, 2.0f);

            float z = 3.0f;

            float3 target(a, z);

            float expected = 14.0f;
            float actual;

            actual = length_squared(target);
            Assert::IsTrue(Equal(expected, actual), L"length_squared did not return the expected value.");
        }

        // A test for min (float3, float3)
        TEST_METHOD(Float3MinTest)
        {
            float3 a(-1.0f, 4.0f, -3.0f);
            float3 b(2.0f, 1.0f, -1.0f);

            float3 expected(-1.0f, 1.0f, -3.0f);
            float3 actual;
            actual = min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"min did not return the expected value.");
        }

        // A test for max (float3, float3)
        TEST_METHOD(Float3MaxTest)
        {
            float3 a(-1.0f, 4.0f, -3.0f);
            float3 b(2.0f, 1.0f, -1.0f);

            float3 expected(2.0f, 4.0f, -1.0f);
            float3 actual;
            actual = max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"max did not return the expected value.");
        }

        TEST_METHOD(Float3MinMaxCodeCoverageTest)
        {
            float3 minV = float3::zero();
            float3 maxV = float3::one();
            float3 actual;

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

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float t = 0.5f;

            float3 expected(2.5f, 3.5f, 4.5f);
            float3 actual;

            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest1)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float t = 0.0f;
            float3 expected(1.0f, 2.0f, 3.0f);
            float3 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest2)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float t = 1.0f;
            float3 expected(4.0f, 5.0f, 6.0f);
            float3 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest3)
        {
            float3 a(0.0f, 0.0f, 0.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float t = 2.0f;
            float3 expected(8.0f, 10.0f, 12.0f);
            float3 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest4)
        {
            float3 a(0.0f, 0.0f, 0.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float t = -2.0f;
            float3 expected(-8.0f, -10.0f, -12.0f);
            float3 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float3, float3, float)
        TEST_METHOD(Float3LerpTest5)
        {
            float3 a(1.68f, 2.34f, 5.43f);
            float3 b = a;

            float t = 0.18f;
            float3 expected(1.68f, 2.34f, 5.43f);
            float3 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for reflect (float3, float3)
        TEST_METHOD(Float3ReflectTest)
        {
            float3 a = normalize(float3(1.0f, 1.0f, 1.0f));

            // Reflect on XZ plane.
            float3 n(0.0f, 1.0f, 0.0f);
            float3 expected(a.x, -a.y, a.z);
            float3 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");

            // Reflect on XY plane.
            n = float3(0.0f, 0.0f, 1.0f);
            expected = float3(a.x, a.y, -a.z);
            actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = float3(1.0f, 0.0f, 0.0f);
            expected = float3(-a.x, a.y, a.z);
            actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for reflect (float3, float3)
        TEST_METHOD(Float3ReflectTest1)
        {
            float3 n(0.45f, 1.28f, 0.86f);
            n = normalize(n);
            float3 a = n;

            float3 expected = -n;
            float3 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for reflect (float3, float3)
        TEST_METHOD(Float3ReflectTest2)
        {
            float3 n(0.45f, 1.28f, 0.86f);
            n = normalize(n);
            float3 a = -n;

            float3 expected = n;
            float3 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for reflect (float3, float3)
        TEST_METHOD(Float3ReflectTest3)
        {
            float3 n(0.45f, 1.28f, 0.86f);
            float3 temp(1.28f, 0.45f, 0.01f);
            // find a perpendicular vector of n
            float3 a = cross(temp, n);

            float3 expected = a;
            float3 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for transform(float3, float4x4)
        TEST_METHOD(Float3TransformTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float3 expected(12.191987f, 21.533493f, 32.616024f);
            float3 actual;

            actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for clamp (float3, float3, float3)
        TEST_METHOD(Float3ClampTest)
        {
            float3 a(0.5f, 0.3f, 0.33f);
            float3 min(0.0f, 0.1f, 0.13f);
            float3 max(1.0f, 1.1f, 1.13f);

            // Normal case.
            // Case N1: specfied value is in the range.
            float3 expected(0.5f, 0.3f, 0.33f);
            float3 actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = float3(2.0f, 3.0f, 4.0f);
            expected = max;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = float3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N4: combination case.
            a = float3(-2.0f, 0.5f, 4.0f);
            expected = float3(min.x, a.y, max.z);
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = float3(0.0f, 0.1f, 0.13f);
            min = float3(1.0f, 1.1f, 1.13f);

            // Case W1: specfied value is in the range.
            a = float3(0.5f, 0.3f, 0.33f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = float3(2.0f, 3.0f, 4.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = float3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");
        }

        // A test for transform_normal (float3, float4x4)
        TEST_METHOD(Float3TransformNormalTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float3 expected(2.19198728f, 1.53349364f, 2.61602545f);
            float3 actual;

            actual = transform_normal(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform_normal did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float3TransformByQuaternionTest)
        {
            float3 v(1.0f, 2.0f, 3.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float3 expected = transform(v, m);
            float3 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float3TransformByQuaternionTest1)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            quaternion q(0, 0, 0, 0);
            float3 expected = v;

            float3 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float3, quaternion)
        TEST_METHOD(Float3TransformByQuaternionTest2)
        {
            float3 v(1.0f, 2.0f, 3.0f);
            quaternion q = quaternion::identity();
            float3 expected = v;

            float3 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for normalize (float3)
        TEST_METHOD(Float3NormalizeTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float3 expected(
                0.26726124191242438468455348087975f,
                0.53452248382484876936910696175951f,
                0.80178372573727315405366044263926f);
            float3 actual;

            actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }

        // A test for normalize (float3)
        TEST_METHOD(Float3NormalizeTest1)
        {
            float3 a(1.0f, 0.0f, 0.0f);

            float3 expected(1.0f, 0.0f, 0.0f);
            float3 actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }

        // A test for normalize (float3)
        TEST_METHOD(Float3NormalizeTest2)
        {
            float3 a(0.0f, 0.0f, 0.0f);

            float3 expected(0.0f, 0.0f, 0.0f);
            float3 actual = normalize(a);
            Assert::IsTrue(isnan(actual.x) && isnan(actual.y) && isnan(actual.z), L"normalize did not return the expected value.");
        }

        // A test for operator - (float3)
        TEST_METHOD(Float3UnaryNegationTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float3 expected(-1.0f, -2.0f, -3.0f);
            float3 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator - did not return the expected value.");
        }

        // A test for operator - (float3, float3)
        TEST_METHOD(Float3SubtractionTest)
        {
            float3 a(4.0f, 2.0f, 3.0f);

            float3 b(1.0f, 5.0f, 7.0f);

            float3 expected(3.0f, -3.0f, -4.0f);
            float3 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator - did not return the expected value.");
        }

        // A test for operator * (float3, float)
        TEST_METHOD(Float3MultiplyTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float factor = 2.0f;

            float3 expected(2.0f, 4.0f, 6.0f);
            float3 actual;

            actual = a * factor;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator * did not return the expected value.");
        }

        // A test for operator * (float3, float3)
        TEST_METHOD(Float3MultiplyTest1)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float3 b(4.0f, 5.0f, 6.0f);

            float3 expected(4.0f, 10.0f, 18.0f);
            float3 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator * did not return the expected value.");
        }

        // A test for operator / (float3, float)
        TEST_METHOD(Float3DivisionTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float div = 2.0f;

            float3 expected(0.5f, 1.0f, 1.5f);
            float3 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator / did not return the expected value.");
        }

        // A test for operator / (float3, float3)
        TEST_METHOD(Float3DivisionTest1)
        {
            float3 a(4.0f, 2.0f, 3.0f);

            float3 b(1.0f, 5.0f, 6.0f);

            float3 expected(4.0f, 0.4f, 0.5f);
            float3 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator / did not return the expected value.");
        }

        // A test for operator / (float3, float3)
        TEST_METHOD(Float3DivisionTest2)
        {
            float3 a(-2.0f, 3.0f, FLT_MAX);

            float div = 0.0f;

            float3 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.x), L"float3::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.y), L"float3::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.z), L"float3::operator / did not return the expected value.");

        }

        // A test for operator / (float2, float2)
        TEST_METHOD(Float3DivisionTest3)
        {
            float3 a(0.047f, -3.0f, -std::numeric_limits<float>::infinity());
            float3 b(0);

            float3 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.x), L"float3::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.y), L"float3::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.z), L"float3::operator / did not return the expected value.");
        }

        // A test for operator + (float3, float3)
        TEST_METHOD(Float3AdditionTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(4.0f, 5.0f, 6.0f);

            float3 expected(5.0f, 7.0f, 9.0f);
            float3 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"float3::operator + did not return the expected value.");
        }

        // A test for float3 (float, float, float)
        TEST_METHOD(Float3ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;

            float3 target(x, y, z);
            Assert::IsTrue(Equal(target.x, x) && Equal(target.y, y) && Equal(target.z, z), L"float3::constructor (x,y,z) did not return the expected value.");
        }

        // A test for float3 (float2, float)
        TEST_METHOD(Float3ConstructorTest1)
        {
            float2 a(1.0f, 2.0f);

            float z = 3.0f;

            float3 target(a, z);
            Assert::IsTrue(Equal(target.x, a.x) && Equal(target.y, a.y) && Equal(target.z, z), L"float3::constructor (float2,z) did not return the expected value.");
        }

        // A test for float3 (float3)
        TEST_METHOD(Float3ConstructorTest2)
        {
            float3 a(1.0f, 2.0f, 3.0f);

            float3 target(a);

            Assert::IsTrue(Equal(target.x, 1.0f) && Equal(target.y, 2.0f) && Equal(target.z, 3.0f), L"float3::constructor (float3) did not return the expected value.");
        }

        // A test for float3 ()
        TEST_METHOD(Float3ConstructorTest3)
        {
            float3 a;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            a.x = 0;    // avoid warning about unused variable
        }

        // A test for float3 (float, float, float)
        TEST_METHOD(Float3ConstructorTest4)
        {
            float3 target(NAN, FLT_MAX, std::numeric_limits<float>::infinity());

            Assert::IsTrue(isnan(target.x), L"float3::constructor (float3) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.y, L"float3::constructor (float3) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.z), L"float3::constructor (float3) did not return the expected value.");
        }

        // A test for operator != (float3, float3)
        TEST_METHOD(Float3InequalityTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"float3::operator != did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"float3::operator != did not return the expected value.");
        }

        // A test for operator == (float3, float3)
        TEST_METHOD(Float3EqualityTest)
        {
            float3 a(1.0f, 2.0f, 3.0f);
            float3 b(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"float3::operator == did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"float3::operator == did not return the expected value.");
        }

        // A test for one
        TEST_METHOD(Float3OneTest)
        {
            float3 val(1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, float3::one(), L"float3::one was not set correctly.");
        }

        // A test for unit_x()
        TEST_METHOD(Float3UnitXTest)
        {
            float3 val(1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, float3::unit_x(), L"float3::unit_x() was not set correctly.");
        }

        // A test for unit_y()
        TEST_METHOD(Float3UnitYTest)
        {
            float3 val(0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, float3::unit_y(), L"float3::unit_y() was not set correctly.");
        }

        // A test for unit_z
        TEST_METHOD(Float3UnitZTest)
        {
            float3 val(0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, float3::unit_z(), L"float3::unit_z() was not set correctly.");
        }

        // A test for zero
        TEST_METHOD(Float3ZeroTest)
        {
            float3 val(0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, float3::zero(), L"float3::zero() was not set correctly.");
        }

        // A test for operator +=
        TEST_METHOD(Float3OperatorAddEqualsTest)
        {
            float3 a(1, 2, 3);
            float3 b(4, 5, 6);
            float3 expected = a + b;
            
            // In-place += operation.
            float3& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Float3OperatorSubtractEqualsTest)
        {
            float3 a(1, 2, 3);
            float3 b(4, 5, 6);
            float3 expected = a - b;
            
            // In-place -= operation.
            float3& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float3OperatorMultiplyEqualsTest)
        {
            float3 a(1, 2, 3);
            float3 b(4, 5, 6);
            float3 expected = a * b;
            
            // In-place *= operation.
            float3& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float3OperatorMultiplyEqualsScalarTest)
        {
            float3 a(1, 2, 3);
            float b = 5;
            float3 expected = a * b;
            
            // In-place *= operation.
            float3& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(Float3OperatorDivideEqualsTest)
        {
            float3 a(1, 2, 3);
            float3 b(4, 5, 6);
            float3 expected = a / b;
            
            // In-place /= operation.
            float3& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b / b;
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator /=
        TEST_METHOD(Float3OperatorDivideEqualsScalarTest)
        {
            float3 a(1, 2, 3);
            float b = 5;
            float3 expected = a / b;
            
            // In-place /= operation.
            float3& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for float3 (float)
        TEST_METHOD(Float3ConstructorTest5)
        {
            float value = 1.0f;
            float3 target(value);

            float3 expected(value, value, value);
            Assert::AreEqual(expected, target, L"float3::cstr did not return the expected value.");

            value = 2.0f;
            target = float3(value);
            expected = float3(value, value, value);
            Assert::AreEqual(expected, target, L"float3::cstr did not return the expected value.");
        }

        // A test for float3 comparison involving NaN values
        TEST_METHOD(Float3EqualsNanTest)
        {
            float3 a(NAN, 0, 0);
            float3 b(0, NAN, 0);
            float3 c(0, 0, NAN);

            Assert::IsFalse(a == float3::zero());
            Assert::IsFalse(b == float3::zero());
            Assert::IsFalse(c == float3::zero());

            Assert::IsTrue(a != float3::zero());
            Assert::IsTrue(b != float3::zero());
            Assert::IsTrue(c != float3::zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
        }

        struct Vector3_2x
        {
            float3 a;
            float3 b;
        };

        struct Vector3PlusFloat
        {
            float3 v;
            float f;
        };

        struct Vector3PlusFloat_2x
        {
            Vector3PlusFloat a;
            Vector3PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Float3SizeofTest)
        {
            Assert::AreEqual(size_t(12), sizeof(float3));
            Assert::AreEqual(size_t(24), sizeof(Vector3_2x));
            Assert::AreEqual(size_t(16), sizeof(Vector3PlusFloat));
            Assert::AreEqual(size_t(32), sizeof(Vector3PlusFloat_2x));
            Assert::AreEqual(sizeof(float3), sizeof(DirectX::XMFLOAT3));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Float3FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(float3, x));
            Assert::AreEqual(size_t(4), offsetof(float3, y));
            Assert::AreEqual(size_t(8), offsetof(float3, z));
        }

        // A test of float3 -> DirectXMath interop
        TEST_METHOD(Float3LoadTest)
        {
            float3 a(23, 42, 666);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
            Assert::AreEqual(0.0f, c.w);
        }

        // A test of DirectXMath -> float3 interop
        TEST_METHOD(Float3StoreTest)
        {
            DirectX::XMFLOAT3 a(23, 42, 666);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&a);
            float3 c;
            DirectX::XMStoreFloat3(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(a.z, c.z);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Float3TypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<float3>::value);
            Assert::IsFalse(std::is_pod<float3>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<float3>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<float3>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<float3>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<float3>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<float3>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<float3>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<float3>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<float3>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<float3>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<float3>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<float3>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<float3>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<float3>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<float3>::value);
            Assert::IsTrue(std::is_trivially_destructible<float3>::value);
        }
    };
}
