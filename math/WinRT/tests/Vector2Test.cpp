//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::Graphics::Canvas::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector2Test)
    {
    public:
        // A test for Distance (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(3.0f, 4.0f);

            float expected = sqrtf(8);
            float actual;

            actual = Vector2Helper::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Distance did not return the expected value.");
        }

        // A test for Distance (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceTest2)
        {
            Vector2 a = Vector2Helper::FromValues(1.051f, 2.05f);
            Vector2 b = Vector2Helper::FromValues(1.051f, 2.05f);

            float actual = Vector2Helper::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector2Helper::Distance did not return the expected value.");
        }

        // A test for DistanceSquared (Vector2, Vector2)
        TEST_METHOD(Vector2DistanceSquaredTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(3.0f, 4.0f);

            float expected = 8.0f;
            float actual;

            actual = Vector2Helper::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::DistanceSquared did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(3.0f, 4.0f);

            float expected = 11.0f;
            float actual;

            actual = Vector2Helper::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Dot did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest1)
        {
            Vector2 a = Vector2Helper::FromValues(1.55f, 1.55f);
            Vector2 b = Vector2Helper::FromValues(-1.55f, 1.55f);

            float expected = 0.0f;
            float actual = Vector2Helper::Dot(a, b);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Dot did not return the expected value.");
        }

        // A test for Dot (Vector2, Vector2)
        TEST_METHOD(Vector2DotTest2)
        {
            Vector2 a = Vector2Helper::FromValues(-FLT_MAX, -FLT_MAX);
            Vector2 b = Vector2Helper::FromValues(FLT_MAX, FLT_MAX);

            float actual = Vector2Helper::Dot(a, b);
            Assert::IsTrue(IsNegativeInfinity(actual), L"Vector2Helper::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector2LengthTest)
        {
            Vector2 a = Vector2Helper::FromValues(2.0f, 4.0f);

            Vector2 target = a;

            float expected = sqrtf(20);
            float actual;

            actual = Vector2Helper::Length(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector2LengthTest1)
        {
            Vector2 target;
            target.X = 0.0f;
            target.Y = 0.0f;

            float expected = 0.0f;
            float actual;

            actual = Vector2Helper::Length(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector2LengthSquaredTest)
        {
            Vector2 a = Vector2Helper::FromValues(2.0f, 4.0f);

            Vector2 target = a;

            float expected = 20.0f;
            float actual;

            actual = Vector2Helper::LengthSquared(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::LengthSquared did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector2LengthSquaredTest1)
        {
            Vector2 a = Vector2Helper::FromValues(0.0f, 0.0f);

            float expected = 0.0f;
            float actual = Vector2Helper::LengthSquared(a);

            Assert::AreEqual(expected, actual, L"Vector2Helper::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector2, Vector2)
        TEST_METHOD(Vector2MinTest)
        {
            Vector2 a = Vector2Helper::FromValues(-1.0f, 4.0f);
            Vector2 b = Vector2Helper::FromValues(2.0f, 1.0f);

            Vector2 expected = Vector2Helper::FromValues(-1.0f, 1.0f);
            Vector2 actual;
            actual = Vector2Helper::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Min did not return the expected value.");
        }

        TEST_METHOD(Vector2MinMaxCodeCoverageTest)
        {
            Vector2 min = Vector2Helper::FromValues(0, 0);
            Vector2 max = Vector2Helper::FromValues(1, 1);
            Vector2 actual;

            // Min.
            actual = Vector2Helper::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector2Helper::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector2Helper::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector2Helper::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Max (Vector2, Vector2)
        TEST_METHOD(Vector2MaxTest)
        {
            Vector2 a = Vector2Helper::FromValues(-1.0f, 4.0f);
            Vector2 b = Vector2Helper::FromValues(2.0f, 1.0f);

            Vector2 expected = Vector2Helper::FromValues(2.0f, 4.0f);
            Vector2 actual;
            actual = Vector2Helper::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Max did not return the expected value.");
        }

        // A test for Clamp (Vector2, Vector2, Vector2)
        TEST_METHOD(Vector2ClampTest)
        {
            Vector2 a = Vector2Helper::FromValues(0.5f, 0.3f);
            Vector2 min = Vector2Helper::FromValues(0.0f, 0.1f);
            Vector2 max = Vector2Helper::FromValues(1.0f, 1.1f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector2 expected = Vector2Helper::FromValues(0.5f, 0.3f);
            Vector2 actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector2Helper::FromValues(2.0f, 3.0f);
            expected = max;
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector2Helper::FromValues(-1.0f, -2.0f);
            expected = min;
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector2Helper::FromValues(-2.0f, 4.0f);
            expected = Vector2Helper::FromValues(min.X, max.Y);
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector2Helper::FromValues(0.0f, 0.1f);
            min = Vector2Helper::FromValues(1.0f, 1.1f);

            // Case W1: specfied value is in the range.
            a = Vector2Helper::FromValues(0.5f, 0.3f);
            expected = min;
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector2Helper::FromValues(2.0f, 3.0f);
            expected = min;
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector2Helper::FromValues(-1.0f, -2.0f);
            expected = min;
            actual = Vector2Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Clamp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(3.0f, 4.0f);

            float t = 0.5f;

            Vector2 expected = Vector2Helper::FromValues(2.0f, 3.0f);
            Vector2 actual;
            actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest1)
        {
            Vector2 a = Vector2Helper::FromValues(0.0f, 0.0f);
            Vector2 b = Vector2Helper::FromValues(3.18f, 4.25f);

            float t = 0.0f;
            Vector2 expected = Vector2Helper::Zero;
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest2)
        {
            Vector2 a = Vector2Helper::FromValues(0.0f, 0.0f);
            Vector2 b = Vector2Helper::FromValues(3.18f, 4.25f);

            float t = 1.0f;
            Vector2 expected = Vector2Helper::FromValues(3.18f, 4.25f);
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest3)
        {
            Vector2 a = Vector2Helper::FromValues(0.0f, 0.0f);
            Vector2 b = Vector2Helper::FromValues(3.18f, 4.25f);

            float t = 2.0f;
            Vector2 expected = Vector2Helper::MultiplyByScalar(b, 2.0f);
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest4)
        {
            Vector2 a = Vector2Helper::FromValues(0.0f, 0.0f);
            Vector2 b = Vector2Helper::FromValues(3.18f, 4.25f);

            float t = -2.0f;
            Vector2 expected = Vector2Helper ::Negate(Vector2Helper::MultiplyByScalar(b, 2.0f));
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest5)
        {
            Vector2 a = Vector2Helper::FromValues(45.67f, 90.0f);
            Vector2 b = Vector2Helper::FromValues(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

            float t = 0.408f;
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(IsPositiveInfinity(actual.X), L"Vector2Helper::Lerp did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Y), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector2, Vector2, float)
        TEST_METHOD(Vector2LerpTest6)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(1.0f, 2.0f);

            float t = 0.5f;

            Vector2 expected = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 actual = Vector2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Lerp did not return the expected value.");
        }

        // A test for Transform(Vector2, Matrix4x4)
        TEST_METHOD(Vector2TransformTest)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector2 expected = Vector2Helper::FromValues(10.316987f, 22.183012f);
            Vector2 actual;

            actual = Vector2Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Transform did not return the expected value.");
        }

        // A test for Transform(Vector2, Matrix3x2)
        TEST_METHOD(Vector2Transform3x2Test)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix3x2 m = Matrix3x2Helper::CreateRotation(ToRadians(30.0f));
            m.M31 = 10.0f;
            m.M32 = 20.0f;

            Vector2 expected = Vector2Helper::FromValues(9.866025f, 22.23205f);
            Vector2 actual;

            actual = Vector2Helper::TransformByMatrix3x2(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Transform did not return the expected value.");
        }

        // A test for TransformNormal (Vector2, Matrix4x4)
        TEST_METHOD(Vector2TransformNormalTest)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector2 expected = Vector2Helper::FromValues(0.3169873f, 2.18301272f);
            Vector2 actual;

            actual = Vector2Helper::TransformNormal(v, m);
            Assert::AreEqual(expected, actual, L"Vector2Helper::TransformNormal did not return the expected value.");
        }

        // A test for TransformNormal (Vector2, Matrix3x2)
        TEST_METHOD(Vector2TransformNormal3x2Test)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix3x2 m = Matrix3x2Helper::CreateRotation(ToRadians(30.0f));
            m.M31 = 10.0f;
            m.M32 = 20.0f;

            Vector2 expected = Vector2Helper::FromValues(-0.133974612f, 2.232051f);
            Vector2 actual;

            actual = Vector2Helper::TransformNormalByMatrix3x2(v, m);
            Assert::AreEqual(expected, actual, L"Vector2Helper::TransformNormal did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            Quaternion q = QuaternionHelper::CreateFromRotationMatrix(m);

            Vector2 expected = Vector2Helper::Transform(v, m);
            Vector2 actual = Vector2Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest1)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Quaternion q = QuaternionHelper::FromValues(0, 0, 0, 0);
            Vector2 expected = v;

            Vector2 actual = Vector2Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector2TransformByQuaternionTest2)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Quaternion q = QuaternionHelper::Identity;
            Vector2 expected = v;

            Vector2 actual = Vector2Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest)
        {
            Vector2 a = Vector2Helper::FromValues(2.0f, 3.0f);
            Vector2 expected = Vector2Helper::FromValues(0.554700196225229122018341733457f, 0.8320502943378436830275126001855f);
            Vector2 actual;

            actual = Vector2Helper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest1)
        {
            Vector2 a = Vector2Helper::FromValue(0);
            Vector2 actual = Vector2Helper::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y), L"Vector2Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector2)
        TEST_METHOD(Vector2NormalizeTest2)
        {
            Vector2 a = Vector2Helper::FromValues(FLT_MAX, FLT_MAX);
            Vector2 actual = Vector2Helper::Normalize(a);
            Vector2 expected = Vector2Helper::FromValues(0, 0);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Normalize did not return the expected value.");
        }

        // A test for Vector2 (float, float)
        TEST_METHOD(Vector2ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;

            Vector2 target = Vector2Helper::FromValues(x, y);
            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y), L"Vector2(x,y) constructor did not return the expected value.");
        }

        // A test for Vector2 (float, float)
        TEST_METHOD(Vector2ConstructorTest3)
        {
            Vector2 target = Vector2Helper::FromValues(NAN, FLT_MAX);
            Assert::IsTrue(isnan(target.X), L"Vector2(x,y) constructor did not return the expected value.");
            Assert::AreEqual(target.Y, FLT_MAX, L"Vector2(x,y) constructor did not return the expected value.");
        }

        // A test for Add (Vector2, Vector2)
        TEST_METHOD(Vector2AddTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(5.0f, 6.0f);

            Vector2 expected = Vector2Helper::FromValues(6.0f, 8.0f);
            Vector2 actual;

            actual = Vector2Helper::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Add did not return the expected value.");
        }

        // A test for Divide (Vector2, float)
        TEST_METHOD(Vector2DivideTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            float div = 2.0f;
            Vector2 expected = Vector2Helper::FromValues(0.5f, 1.0f);
            Vector2 actual;
            actual = Vector2Helper::DivideByScalar(a, div);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Divide did not return the expected value.");
        }

        // A test for Divide (Vector2, Vector2)
        TEST_METHOD(Vector2DivideTest1)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 6.0f);
            Vector2 b = Vector2Helper::FromValues(5.0f, 2.0f);

            Vector2 expected = Vector2Helper::FromValues(1.0f / 5.0f, 6.0f / 2.0f);
            Vector2 actual;

            actual = Vector2Helper::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector2, float)
        TEST_METHOD(Vector2MultiplyTest2)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            float factor = 2.0f;
            Vector2 expected = Vector2Helper::FromValues(2.0f, 4.0f);
            Vector2 actual = Vector2Helper::MultiplyByScalar(a, factor);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector2, Vector2)
        TEST_METHOD(Vector2MultiplyTest3)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            Vector2 b = Vector2Helper::FromValues(5.0f, 6.0f);

            Vector2 expected = Vector2Helper::FromValues(5.0f, 12.0f);
            Vector2 actual;

            actual = Vector2Helper::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector2)
        TEST_METHOD(Vector2NegateTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);

            Vector2 expected = Vector2Helper::FromValues(-1.0f, -2.0f);
            Vector2 actual;

            actual = Vector2Helper::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Negate did not return the expected value.");
        }

        // A test for Subtract (Vector2, Vector2)
        TEST_METHOD(Vector2SubtractTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 6.0f);
            Vector2 b = Vector2Helper::FromValues(5.0f, 2.0f);

            Vector2 expected = Vector2Helper::FromValues(-4.0f, 4.0f);
            Vector2 actual;

            actual = Vector2Helper::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Vector2Helper::Subtract did not return the expected value.");
        }

        // A test for UnitX
        TEST_METHOD(Vector2UnitXTest)
        {
            Vector2 val = Vector2Helper::FromValues(1.0f, 0.0f);
            Assert::AreEqual(val, Vector2Helper::UnitX, L"Vector2Helper::UnitX was not set correctly.");
        }

        // A test for UnitY
        TEST_METHOD(Vector2UnitYTest)
        {
            Vector2 val = Vector2Helper::FromValues(0.0f, 1.0f);
            Assert::AreEqual(val, Vector2Helper::UnitY, L"Vector2Helper::UnitY was not set correctly.");
        }

        // A test for One
        TEST_METHOD(Vector2OneTest)
        {
            Vector2 val = Vector2Helper::FromValues(1.0f, 1.0f);
            Assert::AreEqual(val, Vector2Helper::One, L"Vector2Helper::One was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector2ZeroTest)
        {
            Vector2 val = Vector2Helper::FromValues(0.0f, 0.0f);
            Assert::AreEqual(val, Vector2Helper::Zero, L"Vector2Helper::Zero was not set correctly.");
        }

        // A test for Vector2 (float)
        TEST_METHOD(Vector2ConstructorTest4)
        {
            float value = 1.0f;
            Vector2 target = Vector2Helper::FromValue(value);

            Vector2 expected = Vector2Helper::FromValues(value, value);
            Assert::AreEqual(expected, target, L"Vector2Helper::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector2Helper::FromValue(value);
            expected = Vector2Helper::FromValues(value, value);
            Assert::AreEqual(expected, target, L"Vector2Helper::cstr did not return the expected value.");
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest)
        {
            Vector2 a = Vector2Helper::Normalize(Vector2Helper::FromValues(1.0f, 1.0f));

            // Reflect on XZ plane.
            Vector2 n = Vector2Helper::FromValues(0.0f, 1.0f);
            Vector2 expected = Vector2Helper::FromValues(a.X, -a.Y);
            Vector2 actual = Vector2Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Reflect did not return the expected value.");

            // Reflect on XY plane.
            n = Vector2Helper::FromValues(0.0f, 0.0f);
            expected = Vector2Helper::FromValues(a.X, a.Y);
            actual = Vector2Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = Vector2Helper::FromValues(1.0f, 0.0f);
            expected = Vector2Helper::FromValues(-a.X, a.Y);
            actual = Vector2Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest1)
        {
            Vector2 n = Vector2Helper::FromValues(0.45f, 1.28f);
            n = Vector2Helper::Normalize(n);
            Vector2 a = n;

            Vector2 expected = Vector2Helper::Negate(n);
            Vector2 actual = Vector2Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector2, Vector2)
        TEST_METHOD(Vector2ReflectTest2)
        {
            Vector2 n = Vector2Helper::FromValues(0.45f, 1.28f);
            n = Vector2Helper::Normalize(n);
            Vector2 a = Vector2Helper::Negate(n);

            Vector2 expected = n;
            Vector2 actual = Vector2Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector2Helper::Reflect did not return the expected value.");
        }

        // A test for Vector2 -> Size conversion
        TEST_METHOD(Vector2ToSizeTest)
        {
            Vector2 vector = Vector2Helper::FromValues(23, 42);

            Windows::Foundation::Size result = Vector2Helper::ToSize(vector);

            Assert::AreEqual(23.0f, result.Width);
            Assert::AreEqual(42.0f, result.Height);
        }

        // A test for Vector2 -> Point conversion
        TEST_METHOD(Vector2ToPointTest)
        {
            Vector2 vector = Vector2Helper::FromValues(23, 42);

            Windows::Foundation::Point result = Vector2Helper::ToPoint(vector);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);
        }

        // A test for Size -> Vector2 conversion
        TEST_METHOD(Vector2FromSizeTest)
        {
            Windows::Foundation::Size size(23, 42);

            Vector2 result = Vector2Helper::FromSize(size);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);
        }

        // A test for Point -> Vector2 conversion
        TEST_METHOD(Vector2FromPointTest)
        {
            Windows::Foundation::Point point(23, 42);

            Vector2 result = Vector2Helper::FromPoint(point);

            Assert::AreEqual(23.0f, result.X);
            Assert::AreEqual(42.0f, result.Y);
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
            Assert::AreEqual(size_t(8), sizeof(Vector2));
            Assert::AreEqual(size_t(16), sizeof(Vector2_2x));
            Assert::AreEqual(size_t(12), sizeof(Vector2PlusFloat));
            Assert::AreEqual(size_t(24), sizeof(Vector2PlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Vector2FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Vector2, X));
            Assert::AreEqual(size_t(4), offsetof(Vector2, Y));
        }
    };
}
