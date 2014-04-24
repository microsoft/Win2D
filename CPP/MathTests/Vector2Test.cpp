//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector2Test)
    {
    public:
        // A test for Distance (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(3.0f, 4.0f);

            float expected = sqrtf(8);
            float actual;

            actual = Vector2::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Distance did not return the expected value.");
        }

        // A test for Distance (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceTest2)
        {
            Vector2 a(1.051f, 2.05f);
            Vector2 b(1.051f, 2.05f);

            float actual = Vector2::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector2::Distance did not return the expected value.");
        }

        // A test for DistanceSquared (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceSquaredTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(3.0f, 4.0f);

            float expected = 8.0f;
            float actual;

            actual = Vector2::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::DistanceSquared did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(3.0f, 4.0f);

            float expected = 11.0f;
            float actual;

            actual = Vector2::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Dot did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest1)
        {
            Vector2 a(1.55f, 1.55f);
            Vector2 b(-1.55f, 1.55f);

            float expected = 0.0f;
            float actual = Vector2::Dot(a, b);
            Assert::AreEqual(expected, actual, L"Vector2::Dot did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest2)
        {
            Vector2 a(-FLT_MAX, -FLT_MAX);
            Vector2 b(FLT_MAX, FLT_MAX);

            float actual = Vector2::Dot(a, b);
            Assert::IsTrue(IsNegativeInfinity(actual), L"Vector2::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector2LengthTest)
        {
            Vector2 a(2.0f, 4.0f);

            Vector2 target = a;

            float expected = sqrtf(20);
            float actual;

            actual = target.Length();

            Assert::IsTrue(Equal(expected, actual), L"Vector2::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector2LengthTest1)
        {
            Vector2 target;
            target.X = 0.0f;
            target.Y = 0.0f;

            float expected = 0.0f;
            float actual;

            actual = target.Length();

            Assert::IsTrue(Equal(expected, actual), L"Vector2::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector2LengthSquaredTest)
        {
            Vector2 a(2.0f, 4.0f);

            Vector2 target = a;

            float expected = 20.0f;
            float actual;

            actual = target.LengthSquared();

            Assert::IsTrue(Equal(expected, actual), L"Vector2::LengthSquared did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector2LengthSquaredTest1)
        {
            Vector2 a(0.0f, 0.0f);

            float expected = 0.0f;
            float actual = a.LengthSquared();

            Assert::AreEqual(expected, actual, L"Vector2::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector2, Vector2)
        TEST_METHOD(Vector2MinTest)
        {
            Vector2 a(-1.0f, 4.0f);
            Vector2 b(2.0f, 1.0f);

            Vector2 expected(-1.0f, 1.0f);
            Vector2 actual;
            actual = Vector2::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Min did not return the expected value.");
        }

        TEST_METHOD(Vector2MinMaxCodeCoverageTest)
        {
            Vector2 min(0, 0);
            Vector2 max(1, 1);
            Vector2 actual;

            // Min.
            actual = Vector2::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector2::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector2::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector2::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Max (Vector2, Vector2)
        TEST_METHOD(Vector2MaxTest)
        {
            Vector2 a(-1.0f, 4.0f);
            Vector2 b(2.0f, 1.0f);

            Vector2 expected(2.0f, 4.0f);
            Vector2 actual;
            actual = Vector2::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Max did not return the expected value.");
        }

        // A test for Clamp (Vector2, Vector2, Vector2)
        TEST_METHOD(Vector2ClampTest)
        {
            Vector2 a(0.5f, 0.3f);
            Vector2 min(0.0f, 0.1f);
            Vector2 max(1.0f, 1.1f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector2 expected(0.5f, 0.3f);
            Vector2 actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector2(2.0f, 3.0f);
            expected = max;
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector2(-1.0f, -2.0f);
            expected = min;
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector2(-2.0f, 4.0f);
            expected = Vector2(min.X, max.Y);
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector2(0.0f, 0.1f);
            min = Vector2(1.0f, 1.1f);

            // Case W1: specfied value is in the range.
            a = Vector2(0.5f, 0.3f);
            expected = min;
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector2(2.0f, 3.0f);
            expected = min;
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector2(-1.0f, -2.0f);
            expected = min;
            actual = Vector2::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Clamp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(3.0f, 4.0f);

            float t = 0.5f;

            Vector2 expected(2.0f, 3.0f);
            Vector2 actual;
            actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest1)
        {
            Vector2 a(0.0f, 0.0f);
            Vector2 b(3.18f, 4.25f);

            float t = 0.0f;
            Vector2 expected = Vector2::Zero();
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest2)
        {
            Vector2 a(0.0f, 0.0f);
            Vector2 b(3.18f, 4.25f);

            float t = 1.0f;
            Vector2 expected(3.18f, 4.25f);
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest3)
        {
            Vector2 a(0.0f, 0.0f);
            Vector2 b(3.18f, 4.25f);

            float t = 2.0f;
            Vector2 expected = b * 2.0f;
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest4)
        {
            Vector2 a(0.0f, 0.0f);
            Vector2 b(3.18f, 4.25f);

            float t = -2.0f;
            Vector2 expected = -(b * 2.0f);
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest5)
        {
            Vector2 a(45.67f, 90.0f);
            Vector2 b(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            float t = 0.408f;
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(IsPositiveInfinity(actual.X), L"Vector2::Lerp did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Y), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest6)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(1.0f, 2.0f);

            float t = 0.5f;

            Vector2 expected(1.0f, 2.0f);
            Vector2 actual = Vector2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for Transform(Vector2, Matrix4x4)
        TEST_METHOD(Vector2TransformTest)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector2 expected(10.316987f, 22.183012f);
            Vector2 actual;

            actual = Vector2::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Transform did not return the expected value.");
        }

        // A test for Transform(Vector2, Matrix3x2)
        TEST_METHOD(Vector2Transform3x2Test)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix3x2 m = Matrix3x2::CreateRotation(ToRadians(30.0f));
            m.M31 = 10.0f;
            m.M32 = 20.0f;

            Vector2 expected(9.866025f, 22.23205f);
            Vector2 actual;

            actual = Vector2::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Transform did not return the expected value.");
        }

        // A test for TransformNormal (Vector2, Matrix4x4)
        TEST_METHOD(Vector2TransformNormalTest)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector2 expected(0.3169873f, 2.18301272f);
            Vector2 actual;

            actual = Vector2::TransformNormal(v, m);
            Assert::AreEqual(expected, actual, L"Vector2::TransformNormal did not return the expected value.");
        }

        // A test for TransformNormal (Vector2, Matrix3x2)
        TEST_METHOD(Vector2TransformNormal3x2Test)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix3x2 m = Matrix3x2::CreateRotation(ToRadians(30.0f));
            m.M31 = 10.0f;
            m.M32 = 20.0f;

            Vector2 expected(-0.133974612f, 2.232051f);
            Vector2 actual;

            actual = Vector2::TransformNormal(v, m);
            Assert::AreEqual(expected, actual, L"Vector2::TransformNormal did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest)
        {
            Vector2 v(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector2 expected = Vector2::Transform(v, m);
            Vector2 actual = Vector2::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest1)
        {
            Vector2 v(1.0f, 2.0f);
            Quaternion q;
            Vector2 expected = v;

            Vector2 actual = Vector2::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest2)
        {
            Vector2 v(1.0f, 2.0f);
            Quaternion q = Quaternion::Identity();
            Vector2 expected = v;

            Vector2 actual = Vector2::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest)
        {
            Vector2 a(2.0f, 3.0f);
            Vector2 expected(0.554700196225229122018341733457f, 0.8320502943378436830275126001855f);
            Vector2 actual;

            actual = Vector2::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest1)
        {
            Vector2 a; // no parameter, default to 0.0f
            Vector2 actual = Vector2::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y), L"Vector2::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest2)
        {
            Vector2 a(FLT_MAX, FLT_MAX);
            Vector2 actual = Vector2::Normalize(a);
            Vector2 expected(0, 0);
            Assert::AreEqual(expected, actual, L"Vector2::Normalize did not return the expected value.");
        }

        // A test for operator - (Vector2)
        TEST_METHOD(Vector2UnaryNegationTest)
        {
            Vector2 a(1.0f, 2.0f);

            Vector2 expected(-1.0f, -2.0f);
            Vector2 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator - did not return the expected value.");
        }

        // A test for operator - (Vector2)
        TEST_METHOD(Vector2UnaryNegationTest1)
        {
            Vector2 a(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            Vector2 actual = -a;

            Assert::IsTrue(IsNegativeInfinity(actual.X), L"Vector2::Lerp did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Y), L"Vector2::Lerp did not return the expected value.");
        }

        // A test for operator - (Vector2)
        TEST_METHOD(Vector2UnaryNegationTest2)
        {
            Vector2 a(NAN, 0.0f);
            Vector2 actual = -a;

            Assert::IsTrue(isnan(actual.X), L"Vector2::Lerp did not return the expected value.");
            Assert::IsTrue(0.0f == actual.Y, L"Vector2::Lerp did not return the expected value.");
        }

        // A test for operator - (Vector2, Vector2)
        TEST_METHOD(Vector2SubtractionTest)
        {
            Vector2 a(1.0f, 3.0f);
            Vector2 b(2.0f, 1.5f);

            Vector2 expected(-1.0f, 1.5f);
            Vector2 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator - did not return the expected value.");
        }

        // A test for operator * (Vector2, float)
        TEST_METHOD(Vector2MultiplyTest)
        {
            Vector2 a(2.0f, 3.0f);
            float factor = 2.0f;

            Vector2 expected(4.0f, 6.0f);
            Vector2 actual;

            actual = a * factor;
            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator * did not return the expected value.");
        }

        // A test for operator * (float, Vector2)
        TEST_METHOD(Vector2MultiplyTest4)
        {
            Vector2 a(2.0f, 3.0f);
            float factor = 2.0f;

            Vector2 expected(4.0f, 6.0f);
            Vector2 actual;

            actual = factor * a;
            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator * did not return the expected value.");
        }

        // A test for operator * (Vector2, Vector2)
        TEST_METHOD(Vector2MultiplyTest1)
        {
            Vector2 a(2.0f, 3.0f);
            Vector2 b(4.0f, 5.0f);

            Vector2 expected(8.0f, 15.0f);
            Vector2 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator * did not return the expected value.");
        }

        // A test for operator / (Vector2, float)
        TEST_METHOD(Vector2DivisionTest)
        {
            Vector2 a(2.0f, 3.0f);

            float div = 2.0f;

            Vector2 expected(1.0f, 1.5f);
            Vector2 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator / did not return the expected value.");
        }

        // A test for operator / (Vector2, Vector2)
        TEST_METHOD(Vector2DivisionTest1)
        {
            Vector2 a(2.0f, 3.0f);
            Vector2 b(4.0f, 5.0f);

            Vector2 expected(2.0f / 4.0f, 3.0f / 5.0f);
            Vector2 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator / did not return the expected value.");
        }

        // A test for operator / (Vector2, float)
        TEST_METHOD(Vector2DivisionTest2)
        {
            Vector2 a(-2.0f, 3.0f);

            float div = 0.0f;

            Vector2 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.X), L"Vector2::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Y), L"Vector2::operator / did not return the expected value.");
        }

        // A test for operator / (Vector2, Vector2)
        TEST_METHOD(Vector2DivisionTest3)
        {
            Vector2 a(0.047f, -3.0f);
            Vector2 b;

            Vector2 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.X), L"Vector2::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Y), L"Vector2::operator / did not return the expected value.");
        }

        // A test for operator + (Vector2, Vector2)
        TEST_METHOD(Vector2AdditionTest)
        {
            Vector2 a(1.0f, 2.0f);

            Vector2 b(3.0f, 4.0f);

            Vector2 expected(4.0f, 6.0f);
            Vector2 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"Vector2::operator + did not return the expected value.");
        }

        // A test for Vector2 (float, float)
        TEST_METHOD(Vector2ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;

            Vector2 target(x, y);
            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y), L"Vector2(x,y) constructor did not return the expected value.");
        }

        // A test for Vector2 (Vector2)
        TEST_METHOD(Vector2ConstructorTest1)
        {
            Vector2 a(1.0f, 2.0f);

            Vector2 target(a);
            Assert::IsTrue(Equal(target, a), L"Vector2( Vector2 ) constructor did not return the expected value.");
        }


        // A test for Vector2 ()
        TEST_METHOD(Vector2ConstructorTest2)
        {
            Vector2 target;
            Assert::AreEqual(target.X, 0.0f, L"Vector2() constructor did not return the expected value.");
            Assert::AreEqual(target.Y, 0.0f, L"Vector2() constructor did not return the expected value.");
        }

        // A test for Vector2 (float, float)
        TEST_METHOD(Vector2ConstructorTest3)
        {
            Vector2 target(NAN, FLT_MAX);
            Assert::IsTrue(isnan(target.X), L"Vector2(x,y) constructor did not return the expected value.");
            Assert::AreEqual(target.Y, FLT_MAX, L"Vector2(x,y) constructor did not return the expected value.");
        }

        // A test for Add (Vector2, Vector2)
        TEST_METHOD(Vector2AddTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(5.0f, 6.0f);

            Vector2 expected(6.0f, 8.0f);
            Vector2 actual;

            actual = Vector2::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector2::Add did not return the expected value.");
        }

        // A test for Divide (Vector2, float)
        TEST_METHOD(Vector2DivideTest)
        {
            Vector2 a(1.0f, 2.0f);
            float div = 2.0f;
            Vector2 expected(0.5f, 1.0f);
            Vector2 actual;
            actual = Vector2::Divide(a, div);
            Assert::AreEqual(expected, actual, L"Vector2::Divide did not return the expected value.");
        }

        // A test for Divide (Vector2, Vector2)
        TEST_METHOD(Vector2DivideTest1)
        {
            Vector2 a(1.0f, 6.0f);
            Vector2 b(5.0f, 2.0f);

            Vector2 expected(1.0f / 5.0f, 6.0f / 2.0f);
            Vector2 actual;

            actual = Vector2::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector2::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector2, float)
        TEST_METHOD(Vector2MultiplyTest2)
        {
            Vector2 a(1.0f, 2.0f);
            float factor = 2.0f;
            Vector2 expected(2.0f, 4.0f);
            Vector2 actual = Vector2::Multiply(a, factor);
            Assert::AreEqual(expected, actual, L"Vector2::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector2, Vector2)
        TEST_METHOD(Vector2MultiplyTest3)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(5.0f, 6.0f);

            Vector2 expected(5.0f, 12.0f);
            Vector2 actual;

            actual = Vector2::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector2::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector2)
        TEST_METHOD(Vector2NegateTest)
        {
            Vector2 a(1.0f, 2.0f);

            Vector2 expected(-1.0f, -2.0f);
            Vector2 actual;

            actual = Vector2::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector2::Negate did not return the expected value.");
        }

        // A test for operator != (Vector2, Vector2)
        TEST_METHOD(Vector2InequalityTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(1.0f, 2.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector2::operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector2::operator != did not return the expected value.");
        }

        // A test for operator == (Vector2, Vector2)
        TEST_METHOD(Vector2EqualityTest)
        {
            Vector2 a(1.0f, 2.0f);
            Vector2 b(1.0f, 2.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector2::operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector2::operator == did not return the expected value.");
        }

        // A test for Subtract (Vector2, Vector2)
        TEST_METHOD(Vector2SubtractTest)
        {
            Vector2 a(1.0f, 6.0f);
            Vector2 b(5.0f, 2.0f);

            Vector2 expected(-4.0f, 4.0f);
            Vector2 actual;

            actual = Vector2::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Vector2::Subtract did not return the expected value.");
        }

        // A test for UnitX()
        TEST_METHOD(Vector2UnitXTest)
        {
            Vector2 val(1.0f, 0.0f);
            Assert::AreEqual(val, Vector2::UnitX(), L"Vector2::UnitX() was not set correctly.");
        }

        // A test for UnitY()
        TEST_METHOD(Vector2UnitYTest)
        {
            Vector2 val(0.0f, 1.0f);
            Assert::AreEqual(val, Vector2::UnitY(), L"Vector2::UnitY() was not set correctly.");
        }

        // A test for One
        TEST_METHOD(Vector2OneTest)
        {
            Vector2 val(1.0f, 1.0f);
            Assert::AreEqual(val, Vector2::One(), L"Vector2::One was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector2ZeroTest)
        {
            Vector2 val(0.0f, 0.0f);
            Assert::AreEqual(val, Vector2::Zero(), L"Vector2::Zero() was not set correctly.");
        }

        // A test for operator +=
        TEST_METHOD(Vector2OperatorAddEqualsTest)
        {
            Vector2 a(1, 2);
            Vector2 b(3, 4);
            Vector2 expected = Vector2::Add(a, b);
            
            // In-place += operation.
            Vector2& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector2::Add(b, b);
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Vector2OperatorSubtractEqualsTest)
        {
            Vector2 a(1, 2);
            Vector2 b(3, 4);
            Vector2 expected = Vector2::Subtract(a, b);
            
            // In-place -= operation.
            Vector2& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector2::Subtract(b, b);
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Vector2OperatorMultiplyEqualsTest)
        {
            Vector2 a(1, 2);
            Vector2 b(3, 4);
            Vector2 expected = Vector2::Multiply(a, b);
            
            // In-place *= operation.
            Vector2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector2::Multiply(b, b);
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Vector2OperatorMultiplyEqualsScalerTest)
        {
            Vector2 a(1, 2);
            float b = 3;
            Vector2 expected = Vector2::Multiply(a, b);
            
            // In-place *= operation.
            Vector2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(Vector2OperatorDivideEqualsTest)
        {
            Vector2 a(1, 2);
            Vector2 b(3, 4);
            Vector2 expected = Vector2::Divide(a, b);
            
            // In-place /= operation.
            Vector2& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector2::Divide(b, b);
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator /=
        TEST_METHOD(Vector2OperatorDivideEqualsScalerTest)
        {
            Vector2 a(1, 2);
            float b = 3;
            Vector2 expected = Vector2::Divide(a, b);
            
            // In-place /= operation.
            Vector2& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for Vector2 (float)
        TEST_METHOD(Vector2ConstructorTest4)
        {
            float value = 1.0f;
            Vector2 target(value);

            Vector2 expected(value, value);
            Assert::AreEqual(expected, target, L"Vector2::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector2(value);
            expected = Vector2(value, value);
            Assert::AreEqual(expected, target, L"Vector2::cstr did not return the expected value.");
        }

        // A test for Vector2 comparison involving NaN values
        TEST_METHOD(Vector2EqualsNanTest)
        {
            Vector2 a(NAN, 0);
            Vector2 b(0, NAN);

            Assert::IsFalse(a == Vector2::Zero());
            Assert::IsFalse(b == Vector2::Zero());

            Assert::IsTrue(a != Vector2::Zero());
            Assert::IsTrue(b != Vector2::Zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest)
        {
            Vector2 a = Vector2::Normalize(Vector2(1.0f, 1.0f));

            // Reflect on XZ plane.
            Vector2 n(0.0f, 1.0f);
            Vector2 expected(a.X, -a.Y);
            Vector2 actual = Vector2::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Reflect did not return the expected value.");

            // Reflect on XY plane.
            n = Vector2(0.0f, 0.0f);
            expected = Vector2(a.X, a.Y);
            actual = Vector2::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = Vector2(1.0f, 0.0f);
            expected = Vector2(-a.X, a.Y);
            actual = Vector2::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest1)
        {
            Vector2 n(0.45f, 1.28f);
            n = Vector2::Normalize(n);
            Vector2 a = n;

            Vector2 expected = -n;
            Vector2 actual = Vector2::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest2)
        {
            Vector2 n(0.45f, 1.28f);
            n = Vector2::Normalize(n);
            Vector2 a = -n;

            Vector2 expected = n;
            Vector2 actual = Vector2::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2::Reflect did not return the expected value.");
        }

        // A test for Vector2 -> Size conversion
        TEST_METHOD(Vector2ToSizeTest)
        {
            Vector2 vector(23, 42);

            Windows::Foundation::Size result(vector);

            Assert::AreEqual(23.0f, result.Width);
            Assert::AreEqual(42.0f, result.Height);

            result = Vector2(42, 23);

            Assert::AreEqual(42.0f, result.Width);
            Assert::AreEqual(23.0f, result.Height);
        }

        // A test for Vector2 -> Point conversion
        TEST_METHOD(Vector2ToPointTest)
        {
            Vector2 vector(23, 42);

            Windows::Foundation::Point result(vector);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);

            result = Vector2(42, 23);

            Assert::AreEqual(42.0f, result.X);
            Assert::AreEqual(23.0f, result.Y);
        }

        // A test for Size -> Vector2 conversion
        TEST_METHOD(Vector2FromSizeTest)
        {
            Windows::Foundation::Size size(23, 42);

            Vector2 result(size);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);

            result = Windows::Foundation::Size(42, 23);

            Assert::AreEqual(42.0f, result.X);
            Assert::AreEqual(23.0f, result.Y);
        }

        // A test for Point -> Vector2 conversion
        TEST_METHOD(Vector2FromPointTest)
        {
            Windows::Foundation::Point point(23, 42);

            Vector2 result(point);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);

            result = Windows::Foundation::Point(42, 23);

            Assert::AreEqual(42.0f, result.X);
            Assert::AreEqual(23.0f, result.Y);
        }

        struct Vector2_2x
        {
            Vector2 a;
            Vector2 b;
        };

        struct Vector2PlusFloat
        {
            Vector2 v;
            float f;
        };

        struct Vector2PlusFloat_2x
        {
            Vector2PlusFloat a;
            Vector2PlusFloat b;
        };
        
        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Vector2SizeofTest)
        {
            Assert::AreEqual(8u, sizeof(Vector2));
            Assert::AreEqual(16u, sizeof(Vector2_2x));
            Assert::AreEqual(12u, sizeof(Vector2PlusFloat));
            Assert::AreEqual(24u, sizeof(Vector2PlusFloat_2x));
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Vector2TypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Vector2>::value);
            Assert::IsFalse(std::is_pod<Vector2>::value);
            Assert::IsFalse(std::is_trivial<Vector2>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Vector2>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Vector2>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Vector2>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Vector2>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Vector2>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Vector2>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Vector2>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Vector2>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Vector2>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Vector2>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Vector2>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Vector2>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Vector2>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Vector2>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Vector2>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Vector2>::value);
            Assert::IsTrue(std::is_trivially_destructible<Vector2>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Vector2>::value);
        }
    };
}
