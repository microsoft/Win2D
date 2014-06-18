//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Platform::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(Float2Test)
    {
    public:
        // A test for distance (float2, float2)
        TEST_METHOD(Float2DistanceTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(3.0f, 4.0f);

            float expected = sqrtf(8);
            float actual;

            actual = distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance did not return the expected value.");
        }

        // A test for distance (float2, float2)
        TEST_METHOD(Float2DistanceTest2)
        {
            float2 a(1.051f, 2.05f);
            float2 b(1.051f, 2.05f);

            float actual = distance(a, b);
            Assert::AreEqual(0.0f, actual, L"distance did not return the expected value.");
        }

        // A test for distance_squared (float2, float2)
        TEST_METHOD(Float2DistanceSquaredTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(3.0f, 4.0f);

            float expected = 8.0f;
            float actual;

            actual = distance_squared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"distance_squared did not return the expected value.");
        }

        // A test for dot (float2, float2)
        TEST_METHOD(Float2DotTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(3.0f, 4.0f);

            float expected = 11.0f;
            float actual;

            actual = dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"dot did not return the expected value.");
        }

        // A test for dot (float2, float2)
        TEST_METHOD(Float2DotTest1)
        {
            float2 a(1.55f, 1.55f);
            float2 b(-1.55f, 1.55f);

            float expected = 0.0f;
            float actual = dot(a, b);
            Assert::AreEqual(expected, actual, L"dot did not return the expected value.");
        }

        // A test for dot (float2, float2)
        TEST_METHOD(Float2DotTest2)
        {
            float2 a(-FLT_MAX, -FLT_MAX);
            float2 b(FLT_MAX, FLT_MAX);

            float actual = dot(a, b);
            Assert::IsTrue(IsNegativeInfinity(actual), L"dot did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float2LengthTest)
        {
            float2 a(2.0f, 4.0f);

            float2 target = a;

            float expected = sqrtf(20);
            float actual;

            actual = length(target);

            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length ()
        TEST_METHOD(Float2LengthTest1)
        {
            float2 target;
            target.x = 0.0f;
            target.y = 0.0f;

            float expected = 0.0f;
            float actual;

            actual = length(target);

            Assert::IsTrue(Equal(expected, actual), L"length did not return the expected value.");
        }

        // A test for length_squared ()
        TEST_METHOD(Float2LengthSquaredTest)
        {
            float2 a(2.0f, 4.0f);

            float2 target = a;

            float expected = 20.0f;
            float actual;

            actual = length_squared(target);

            Assert::IsTrue(Equal(expected, actual), L"length_squared did not return the expected value.");
        }

        // A test for length_squared ()
        TEST_METHOD(Float2LengthSquaredTest1)
        {
            float2 a(0.0f, 0.0f);

            float expected = 0.0f;
            float actual = length_squared(a);

            Assert::AreEqual(expected, actual, L"length_squared did not return the expected value.");
        }

        // A test for min (float2, float2)
        TEST_METHOD(Float2MinTest)
        {
            float2 a(-1.0f, 4.0f);
            float2 b(2.0f, 1.0f);

            float2 expected(-1.0f, 1.0f);
            float2 actual;
            actual = min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"min did not return the expected value.");
        }

        TEST_METHOD(Float2MinMaxCodeCoverageTest)
        {
            float2 minV(0, 0);
            float2 maxV(1, 1);
            float2 actual;

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

        // A test for max (float2, float2)
        TEST_METHOD(Float2MaxTest)
        {
            float2 a(-1.0f, 4.0f);
            float2 b(2.0f, 1.0f);

            float2 expected(2.0f, 4.0f);
            float2 actual;
            actual = max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"max did not return the expected value.");
        }

        // A test for clamp (float2, float2, float2)
        TEST_METHOD(Float2ClampTest)
        {
            float2 a(0.5f, 0.3f);
            float2 min(0.0f, 0.1f);
            float2 max(1.0f, 1.1f);

            // Normal case.
            // Case N1: specfied value is in the range.
            float2 expected(0.5f, 0.3f);
            float2 actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = float2(2.0f, 3.0f);
            expected = max;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = float2(-1.0f, -2.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case N4: combination case.
            a = float2(-2.0f, 4.0f);
            expected = float2(min.x, max.y);
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = float2(0.0f, 0.1f);
            min = float2(1.0f, 1.1f);

            // Case W1: specfied value is in the range.
            a = float2(0.5f, 0.3f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = float2(2.0f, 3.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = float2(-1.0f, -2.0f);
            expected = min;
            actual = clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"clamp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(3.0f, 4.0f);

            float t = 0.5f;

            float2 expected(2.0f, 3.0f);
            float2 actual;
            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest1)
        {
            float2 a(0.0f, 0.0f);
            float2 b(3.18f, 4.25f);

            float t = 0.0f;
            float2 expected = float2::zero();
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest2)
        {
            float2 a(0.0f, 0.0f);
            float2 b(3.18f, 4.25f);

            float t = 1.0f;
            float2 expected(3.18f, 4.25f);
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest3)
        {
            float2 a(0.0f, 0.0f);
            float2 b(3.18f, 4.25f);

            float t = 2.0f;
            float2 expected = b * 2.0f;
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest4)
        {
            float2 a(0.0f, 0.0f);
            float2 b(3.18f, 4.25f);

            float t = -2.0f;
            float2 expected = -(b * 2.0f);
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest5)
        {
            float2 a(45.67f, 90.0f);
            float2 b(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            float t = 0.408f;
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(IsPositiveInfinity(actual.x), L"lerp did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.y), L"lerp did not return the expected value.");
        }

        // A test for lerp (float2, float2, float)
        TEST_METHOD(Float2LerpTest6)
        {
            float2 a(1.0f, 2.0f);
            float2 b(1.0f, 2.0f);

            float t = 0.5f;

            float2 expected(1.0f, 2.0f);
            float2 actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for transform(float2, float4x4)
        TEST_METHOD(Float2TransformTest)
        {
            float2 v(1.0f, 2.0f);
            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float2 expected(10.316987f, 22.183012f);
            float2 actual;

            actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform(float2, float3x2)
        TEST_METHOD(Float2Transform3x2Test)
        {
            float2 v(1.0f, 2.0f);
            float3x2 m = make_float3x2_rotation(ToRadians(30.0f));
            m.m31 = 10.0f;
            m.m32 = 20.0f;

            float2 expected(9.866025f, 22.23205f);
            float2 actual;

            actual = transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform_normal (float2, float4x4)
        TEST_METHOD(Float2TransformNormalTest)
        {
            float2 v(1.0f, 2.0f);
            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            float2 expected(0.3169873f, 2.18301272f);
            float2 actual;

            actual = transform_normal(v, m);
            Assert::AreEqual(expected, actual, L"transform_normal did not return the expected value.");
        }

        // A test for transform_normal (float2, float3x2)
        TEST_METHOD(Float2TransformNormal3x2Test)
        {
            float2 v(1.0f, 2.0f);
            float3x2 m = make_float3x2_rotation(ToRadians(30.0f));
            m.m31 = 10.0f;
            m.m32 = 20.0f;

            float2 expected(-0.133974612f, 2.232051f);
            float2 actual;

            actual = transform_normal(v, m);
            Assert::AreEqual(expected, actual, L"transform_normal did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float2TransformByQuaternionTest)
        {
            float2 v(1.0f, 2.0f);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            float2 expected = transform(v, m);
            float2 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float2TransformByQuaternionTest1)
        {
            float2 v(1.0f, 2.0f);
            quaternion q(0, 0, 0, 0);
            float2 expected = v;

            float2 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for transform (float2, quaternion)
        TEST_METHOD(Float2TransformByQuaternionTest2)
        {
            float2 v(1.0f, 2.0f);
            quaternion q = quaternion::identity();
            float2 expected = v;

            float2 actual = transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for normalize (float2)
        TEST_METHOD(Float2NormalizeTest)
        {
            float2 a(2.0f, 3.0f);
            float2 expected(0.554700196225229122018341733457f, 0.8320502943378436830275126001855f);
            float2 actual;

            actual = normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"normalize did not return the expected value.");
        }

        // A test for normalize (float2)
        TEST_METHOD(Float2NormalizeTest1)
        {
            float2 a(0);
            float2 actual = normalize(a);
            Assert::IsTrue(isnan(actual.x) && isnan(actual.y), L"normalize did not return the expected value.");
        }

        // A test for normalize (float2)
        TEST_METHOD(Float2NormalizeTest2)
        {
            float2 a(FLT_MAX, FLT_MAX);
            float2 actual = normalize(a);
            float2 expected(0, 0);
            Assert::AreEqual(expected, actual, L"normalize did not return the expected value.");
        }

        // A test for operator - (float2)
        TEST_METHOD(Float2UnaryNegationTest)
        {
            float2 a(1.0f, 2.0f);

            float2 expected(-1.0f, -2.0f);
            float2 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator - did not return the expected value.");
        }

        // A test for operator - (float2)
        TEST_METHOD(Float2UnaryNegationTest1)
        {
            float2 a(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            float2 actual = -a;

            Assert::IsTrue(IsNegativeInfinity(actual.x), L"lerp did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.y), L"lerp did not return the expected value.");
        }

        // A test for operator - (float2)
        TEST_METHOD(Float2UnaryNegationTest2)
        {
            float2 a(NAN, 0.0f);
            float2 actual = -a;

            Assert::IsTrue(isnan(actual.x), L"lerp did not return the expected value.");
            Assert::IsTrue(0.0f == actual.y, L"lerp did not return the expected value.");
        }

        // A test for operator - (float2, float2)
        TEST_METHOD(Float2SubtractionTest)
        {
            float2 a(1.0f, 3.0f);
            float2 b(2.0f, 1.5f);

            float2 expected(-1.0f, 1.5f);
            float2 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator - did not return the expected value.");
        }

        // A test for operator * (float2, float)
        TEST_METHOD(Float2MultiplyTest)
        {
            float2 a(2.0f, 3.0f);
            float factor = 2.0f;

            float2 expected(4.0f, 6.0f);
            float2 actual;

            actual = a * factor;
            Assert::IsTrue(Equal(expected, actual), L"float2::operator * did not return the expected value.");
        }

        // A test for operator * (float, float2)
        TEST_METHOD(Float2MultiplyTest4)
        {
            float2 a(2.0f, 3.0f);
            float factor = 2.0f;

            float2 expected(4.0f, 6.0f);
            float2 actual;

            actual = factor * a;
            Assert::IsTrue(Equal(expected, actual), L"float2::operator * did not return the expected value.");
        }

        // A test for operator * (float2, float2)
        TEST_METHOD(Float2MultiplyTest1)
        {
            float2 a(2.0f, 3.0f);
            float2 b(4.0f, 5.0f);

            float2 expected(8.0f, 15.0f);
            float2 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator * did not return the expected value.");
        }

        // A test for operator / (float2, float)
        TEST_METHOD(Float2DivisionTest)
        {
            float2 a(2.0f, 3.0f);

            float div = 2.0f;

            float2 expected(1.0f, 1.5f);
            float2 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator / did not return the expected value.");
        }

        // A test for operator / (float2, float2)
        TEST_METHOD(Float2DivisionTest1)
        {
            float2 a(2.0f, 3.0f);
            float2 b(4.0f, 5.0f);

            float2 expected(2.0f / 4.0f, 3.0f / 5.0f);
            float2 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator / did not return the expected value.");
        }

        // A test for operator / (float2, float)
        TEST_METHOD(Float2DivisionTest2)
        {
            float2 a(-2.0f, 3.0f);

            float div = 0.0f;

            float2 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.x), L"float2::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.y), L"float2::operator / did not return the expected value.");
        }

        // A test for operator / (float2, float2)
        TEST_METHOD(Float2DivisionTest3)
        {
            float2 a(0.047f, -3.0f);
            float2 b(0, 0);

            float2 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.x), L"float2::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.y), L"float2::operator / did not return the expected value.");
        }

        // A test for operator + (float2, float2)
        TEST_METHOD(Float2AdditionTest)
        {
            float2 a(1.0f, 2.0f);

            float2 b(3.0f, 4.0f);

            float2 expected(4.0f, 6.0f);
            float2 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"float2::operator + did not return the expected value.");
        }

        // A test for float2 (float, float)
        TEST_METHOD(Float2ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;

            float2 target(x, y);
            Assert::IsTrue(Equal(target.x, x) && Equal(target.y, y), L"float2(x,y) constructor did not return the expected value.");
        }

        // A test for float2 (float2)
        TEST_METHOD(Float2ConstructorTest1)
        {
            float2 a(1.0f, 2.0f);

            float2 target(a);
            Assert::IsTrue(Equal(target, a), L"float2( float2 ) constructor did not return the expected value.");
        }

        // A test for float2 ()
        TEST_METHOD(Float2ConstructorTest2)
        {
            float2 target;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            target.x = 0;    // avoid warning about unused variable
        }

        // A test for float2 (float, float)
        TEST_METHOD(Float2ConstructorTest3)
        {
            float2 target(NAN, FLT_MAX);
            Assert::IsTrue(isnan(target.x), L"float2(x,y) constructor did not return the expected value.");
            Assert::AreEqual(target.y, FLT_MAX, L"float2(x,y) constructor did not return the expected value.");
        }

        // A test for operator != (float2, float2)
        TEST_METHOD(Float2InequalityTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(1.0f, 2.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"float2::operator != did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"float2::operator != did not return the expected value.");
        }

        // A test for operator == (float2, float2)
        TEST_METHOD(Float2EqualityTest)
        {
            float2 a(1.0f, 2.0f);
            float2 b(1.0f, 2.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"float2::operator == did not return the expected value.");

            // case 2: compare between different values
            b.x = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"float2::operator == did not return the expected value.");
        }

        // A test for unit_x()
        TEST_METHOD(Float2UnitXTest)
        {
            float2 val(1.0f, 0.0f);
            Assert::AreEqual(val, float2::unit_x(), L"float2::unit_x() was not set correctly.");
        }

        // A test for unit_y()
        TEST_METHOD(Float2UnitYTest)
        {
            float2 val(0.0f, 1.0f);
            Assert::AreEqual(val, float2::unit_y(), L"float2::unit_y() was not set correctly.");
        }

        // A test for one
        TEST_METHOD(Float2OneTest)
        {
            float2 val(1.0f, 1.0f);
            Assert::AreEqual(val, float2::one(), L"float2::one was not set correctly.");
        }

        // A test for zero
        TEST_METHOD(Float2ZeroTest)
        {
            float2 val(0.0f, 0.0f);
            Assert::AreEqual(val, float2::zero(), L"float2::zero() was not set correctly.");
        }

        // A test for operator +=
        TEST_METHOD(Float2OperatorAddEqualsTest)
        {
            float2 a(1, 2);
            float2 b(3, 4);
            float2 expected = a + b;
            
            // In-place += operation.
            float2& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Float2OperatorSubtractEqualsTest)
        {
            float2 a(1, 2);
            float2 b(3, 4);
            float2 expected = a - b;
            
            // In-place -= operation.
            float2& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float2OperatorMultiplyEqualsTest)
        {
            float2 a(1, 2);
            float2 b(3, 4);
            float2 expected = a * b;
            
            // In-place *= operation.
            float2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float2OperatorMultiplyEqualsScalarTest)
        {
            float2 a(1, 2);
            float b = 3;
            float2 expected = a * b;
            
            // In-place *= operation.
            float2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(Float2OperatorDivideEqualsTest)
        {
            float2 a(1, 2);
            float2 b(3, 4);
            float2 expected = a / b;
            
            // In-place /= operation.
            float2& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b / b;
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator /=
        TEST_METHOD(Float2OperatorDivideEqualsScalarTest)
        {
            float2 a(1, 2);
            float b = 3;
            float2 expected = a / b;
            
            // In-place /= operation.
            float2& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for float2 (float)
        TEST_METHOD(Float2ConstructorTest4)
        {
            float value = 1.0f;
            float2 target(value);

            float2 expected(value, value);
            Assert::AreEqual(expected, target, L"float2::cstr did not return the expected value.");

            value = 2.0f;
            target = float2(value);
            expected = float2(value, value);
            Assert::AreEqual(expected, target, L"float2::cstr did not return the expected value.");
        }

        // A test for float2 comparison involving NaN values
        TEST_METHOD(Float2EqualsNanTest)
        {
            float2 a(NAN, 0);
            float2 b(0, NAN);

            Assert::IsFalse(a == float2::zero());
            Assert::IsFalse(b == float2::zero());

            Assert::IsTrue(a != float2::zero());
            Assert::IsTrue(b != float2::zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
        }

        // A test for reflect (float2, float2)
        TEST_METHOD(Float2ReflectTest)
        {
            float2 a = normalize(float2(1.0f, 1.0f));

            // Reflect on XZ plane.
            float2 n(0.0f, 1.0f);
            float2 expected(a.x, -a.y);
            float2 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");

            // Reflect on XY plane.
            n = float2(0.0f, 0.0f);
            expected = float2(a.x, a.y);
            actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = float2(1.0f, 0.0f);
            expected = float2(-a.x, a.y);
            actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for reflect (float2, float2)
        TEST_METHOD(Float2ReflectTest1)
        {
            float2 n(0.45f, 1.28f);
            n = normalize(n);
            float2 a = n;

            float2 expected = -n;
            float2 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for reflect (float2, float2)
        TEST_METHOD(Float2ReflectTest2)
        {
            float2 n(0.45f, 1.28f);
            n = normalize(n);
            float2 a = -n;

            float2 expected = n;
            float2 actual = reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"reflect did not return the expected value.");
        }

        // A test for float2 -> Size conversion
        TEST_METHOD(Float2ToSizeTest)
        {
            float2 vector(23, 42);

            Windows::Foundation::Size result(vector);

            Assert::AreEqual(23.0f, result.Width);
            Assert::AreEqual(42.0f, result.Height);

            result = float2(42, 23);

            Assert::AreEqual(42.0f, result.Width);
            Assert::AreEqual(23.0f, result.Height);
        }

        // A test for float2 -> Point conversion
        TEST_METHOD(Float2ToPointTest)
        {
            float2 vector(23, 42);

            Windows::Foundation::Point result(vector);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);

            result = float2(42, 23);

            Assert::AreEqual(42.0f, result.X);
            Assert::AreEqual(23.0f, result.Y);
        }

        // A test for Size -> float2 conversion
        TEST_METHOD(Float2FromSizeTest)
        {
            Windows::Foundation::Size size(23, 42);

            float2 result(size);

            Assert::AreEqual(23.0f, result.x);
            Assert::AreEqual(42.0f, result.y);

            result = Windows::Foundation::Size(42, 23);

            Assert::AreEqual(42.0f, result.x);
            Assert::AreEqual(23.0f, result.y);
        }

        // A test for Point -> float2 conversion
        TEST_METHOD(Float2FromPointTest)
        {
            Windows::Foundation::Point point(23, 42);

            float2 result(point);

            Assert::AreEqual(23.0f, result.x);
            Assert::AreEqual(42.0f, result.y);

            result = Windows::Foundation::Point(42, 23);

            Assert::AreEqual(42.0f, result.x);
            Assert::AreEqual(23.0f, result.y);
        }

        struct Vector2_2x
        {
            float2 a;
            float2 b;
        };

        struct Vector2PlusFloat
        {
            float2 v;
            float f;
        };

        struct Vector2PlusFloat_2x
        {
            Vector2PlusFloat a;
            Vector2PlusFloat b;
        };
        
        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Float2SizeofTest)
        {
            Assert::AreEqual(size_t(8), sizeof(float2));
            Assert::AreEqual(size_t(16), sizeof(Vector2_2x));
            Assert::AreEqual(size_t(12), sizeof(Vector2PlusFloat));
            Assert::AreEqual(size_t(24), sizeof(Vector2PlusFloat_2x));
            Assert::AreEqual(sizeof(float2), sizeof(DirectX::XMFLOAT2));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Float2FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(float2, x));
            Assert::AreEqual(size_t(4), offsetof(float2, y));
        }

        // A test of float2 -> DirectXMath interop
        TEST_METHOD(Float2LoadTest)
        {
            float2 a(23, 42);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat2(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
            Assert::AreEqual(0.0f, c.z);
            Assert::AreEqual(0.0f, c.w);
        }

        // A test of DirectXMath -> float2 interop
        TEST_METHOD(Float2StoreTest)
        {
            DirectX::XMFLOAT2 a(23, 42);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat2(&a);
            float2 c;
            DirectX::XMStoreFloat2(&c, b);

            Assert::AreEqual(a.x, c.x);
            Assert::AreEqual(a.y, c.y);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Float2TypeTraitsTest)
        {
            // We should be standard layout and trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_standard_layout<float2>::value);
            Assert::IsTrue(std::is_trivial<float2>::value);
            Assert::IsFalse(std::is_pod<float2>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<float2>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<float2>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<float2>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<float2>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<float2>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<float2>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<float2>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<float2>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<float2>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<float2>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<float2>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<float2>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<float2>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<float2>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<float2>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<float2>::value);
            Assert::IsTrue(std::is_trivially_destructible<float2>::value);
            Assert::IsTrue(std::is_nothrow_destructible<float2>::value);
        }
    };
}
