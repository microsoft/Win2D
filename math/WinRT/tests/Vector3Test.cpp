//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::Graphics::Canvas::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector3Test)
    {
    public:
        // A test for Cross (Vector3, Vector3)
        TEST_METHOD(Vector3CrossTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);
            Vector3 b = Vector3Helper::FromValues(0.0f, 1.0f, 0.0f);

            Vector3 expected = Vector3Helper::FromValues(0.0f, 0.0f, 1.0f);
            Vector3 actual;

            actual = Vector3Helper::Cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Cross did not return the expected value.");
        }

        // A test for Cross (Vector3, Vector3)
        TEST_METHOD(Vector3CrossTest1)
        {
            Vector3 a = Vector3Helper::FromValues(0.0f, 1.0f, 0.0f);
            Vector3 b = Vector3Helper::FromValues(0.0f, 1.0f, 0.0f);

            Vector3 expected = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3Helper::Cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Cross did not return the expected value.");
        }

        // A test for Distance (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float expected = sqrtf(27);
            float actual;

            actual = Vector3Helper::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Distance did not return the expected value.");
        }

        // A test for Distance (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.051f, 2.05f, 3.478f);
            Vector3 b = Vector3Helper::FromVector2(Vector2Helper::FromValues(1.051f, 0.0f), 1);
            b.Y = 2.05f;
            b.Z = 3.478f;

            float actual = Vector3Helper::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector3Helper::Distance did not return the expected value.");
        }

        // A test for DistanceSquared (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceSquaredTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float expected = 27.0f;
            float actual;

            actual = Vector3Helper::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::DistanceSquared did not return the expected value.");
        }

        // A test for Dot (Vector3, Vector3)
        TEST_METHOD(Vector3DotTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float expected = 32.0f;
            float actual;

            actual = Vector3Helper::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Dot did not return the expected value.");
        }

        // A test for Dot (Vector3, Vector3)
        TEST_METHOD(Vector3DotTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.55f, 1.55f, 1);
            Vector3 b = Vector3Helper::FromValues(2.5f, 3, 1.5f);
            Vector3 c = Vector3Helper::Cross(a, b);

            float expected = 0.0f;
            float actual1 = Vector3Helper::Dot(a, c);
            float actual2 = Vector3Helper::Dot(b, c);
            Assert::IsTrue(Equal(expected, actual1), L"Vector3Helper::Dot did not return the expected value.");
            Assert::IsTrue(Equal(expected, actual2), L"Vector3Helper::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector3LengthTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = Vector3Helper::FromVector2(a, z);

            float expected = sqrtf(14.0f);
            float actual;

            actual = Vector3Helper::Length(target);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector3LengthTest1)
        {
            Vector3 target = Vector3Helper::Zero;

            float expected = 0.0f;
            float actual = Vector3Helper::Length(target);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector3LengthSquaredTest)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = Vector3Helper::FromVector2(a, z);

            float expected = 14.0f;
            float actual;

            actual = Vector3Helper::LengthSquared(target);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector3, Vector3)
        TEST_METHOD(Vector3MinTest)
        {
            Vector3 a = Vector3Helper::FromValues(-1.0f, 4.0f, -3.0f);
            Vector3 b = Vector3Helper::FromValues(2.0f, 1.0f, -1.0f);

            Vector3 expected = Vector3Helper::FromValues(-1.0f, 1.0f, -3.0f);
            Vector3 actual;
            actual = Vector3Helper::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Min did not return the expected value.");
        }

        // A test for Max (Vector3, Vector3)
        TEST_METHOD(Vector3MaxTest)
        {
            Vector3 a = Vector3Helper::FromValues(-1.0f, 4.0f, -3.0f);
            Vector3 b = Vector3Helper::FromValues(2.0f, 1.0f, -1.0f);

            Vector3 expected = Vector3Helper::FromValues(2.0f, 4.0f, -1.0f);
            Vector3 actual;
            actual = Vector3Helper::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Max did not return the expected value.");
        }

        TEST_METHOD(Vector3MinMaxCodeCoverageTest)
        {
            Vector3 min = Vector3Helper::Zero;
            Vector3 max = Vector3Helper::One;
            Vector3 actual;

            // Min.
            actual = Vector3Helper::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector3Helper::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector3Helper::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector3Helper::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float t = 0.5f;

            Vector3 expected = Vector3Helper::FromValues(2.5f, 3.5f, 4.5f);
            Vector3 actual;

            actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float t = 0.0f;
            Vector3 expected = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest2)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float t = 1.0f;
            Vector3 expected = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);
            Vector3 actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest3)
        {
            Vector3 a = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float t = 2.0f;
            Vector3 expected = Vector3Helper::FromValues(8.0f, 10.0f, 12.0f);
            Vector3 actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest4)
        {
            Vector3 a = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);
            Vector3 b = Vector3Helper::FromValues(4.0f, 5.0f, 6.0f);

            float t = -2.0f;
            Vector3 expected = Vector3Helper::FromValues(-8.0f, -10.0f, -12.0f);
            Vector3 actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest5)
        {
            Vector3 a = Vector3Helper::FromValues(1.68f, 2.34f, 5.43f);
            Vector3 b = a;

            float t = 0.18f;
            Vector3 expected = Vector3Helper::FromValues(1.68f, 2.34f, 5.43f);
            Vector3 actual = Vector3Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Lerp did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest)
        {
            Vector3 a = Vector3Helper::Normalize(Vector3Helper::FromValues(1.0f, 1.0f, 1.0f));

            // Reflect on XZ plane.
            Vector3 n = Vector3Helper::FromValues(0.0f, 1.0f, 0.0f);
            Vector3 expected = Vector3Helper::FromValues(a.X, -a.Y, a.Z);
            Vector3 actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");

            // Reflect on XY plane.
            n = Vector3Helper::FromValues(0.0f, 0.0f, 1.0f);
            expected = Vector3Helper::FromValues(a.X, a.Y, -a.Z);
            actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);
            expected = Vector3Helper::FromValues(-a.X, a.Y, a.Z);
            actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest1)
        {
            Vector3 n = Vector3Helper::FromValues(0.45f, 1.28f, 0.86f);
            n = Vector3Helper::Normalize(n);
            Vector3 a = n;

            Vector3 expected = Vector3Helper::Negate(n);
            Vector3 actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest2)
        {
            Vector3 n = Vector3Helper::FromValues(0.45f, 1.28f, 0.86f);
            n = Vector3Helper::Normalize(n);
            Vector3 a = Vector3Helper::Negate(n);

            Vector3 expected = n;
            Vector3 actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest3)
        {
            Vector3 n = Vector3Helper::FromValues(0.45f, 1.28f, 0.86f);
            Vector3 temp = Vector3Helper::FromValues(1.28f, 0.45f, 0.01f);
            // find a perpendicular vector of n
            Vector3 a = Vector3Helper::Cross(temp, n);

            Vector3 expected = a;
            Vector3 actual = Vector3Helper::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Reflect did not return the expected value.");
        }

        // A test for Transform(Vector3, Matrix4x4)
        TEST_METHOD(Vector3TransformTest)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
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

            Vector3 expected = Vector3Helper::FromValues(12.191987f, 21.533493f, 32.616024f);
            Vector3 actual;

            actual = Vector3Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Transform did not return the expected value.");
        }

        // A test for Clamp (Vector3, Vector3, Vector3)
        TEST_METHOD(Vector3ClampTest)
        {
            Vector3 a = Vector3Helper::FromValues(0.5f, 0.3f, 0.33f);
            Vector3 min = Vector3Helper::FromValues(0.0f, 0.1f, 0.13f);
            Vector3 max = Vector3Helper::FromValues(1.0f, 1.1f, 1.13f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector3 expected = Vector3Helper::FromValues(0.5f, 0.3f, 0.33f);
            Vector3 actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector3Helper::FromValues(2.0f, 3.0f, 4.0f);
            expected = max;
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector3Helper::FromValues(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector3Helper::FromValues(-2.0f, 0.5f, 4.0f);
            expected = Vector3Helper::FromValues(min.X, a.Y, max.Z);
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector3Helper::FromValues(0.0f, 0.1f, 0.13f);
            min = Vector3Helper::FromValues(1.0f, 1.1f, 1.13f);

            // Case W1: specfied value is in the range.
            a = Vector3Helper::FromValues(0.5f, 0.3f, 0.33f);
            expected = min;
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector3Helper::FromValues(2.0f, 3.0f, 4.0f);
            expected = min;
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector3Helper::FromValues(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Clamp did not return the expected value.");
        }

        // A test for TransformNormal (Vector3, Matrix4x4)
        TEST_METHOD(Vector3TransformNormalTest)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
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

            Vector3 expected = Vector3Helper::FromValues(2.19198728f, 1.53349364f, 2.61602545f);
            Vector3 actual;

            actual = Vector3Helper::TransformNormal(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::TransformNormal did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            Quaternion q = QuaternionHelper::CreateFromRotationMatrix(m);

            Vector3 expected = Vector3Helper::Transform(v, m);
            Vector3 actual = Vector3Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest1)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Quaternion q = QuaternionHelper::FromValues(0, 0, 0, 0);
            Vector3 expected = v;

            Vector3 actual = Vector3Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest2)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Quaternion q = QuaternionHelper::Identity;
            Vector3 expected = v;

            Vector3 actual = Vector3Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);

            Vector3 expected = Vector3Helper::FromValues(
                0.26726124191242438468455348087975f,
                0.53452248382484876936910696175951f,
                0.80178372573727315405366044263926f);
            Vector3 actual;

            actual = Vector3Helper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);

            Vector3 expected = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3Helper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector3Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest2)
        {
            Vector3 a = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);

            Vector3 expected = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3Helper::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z), L"Vector3Helper::Normalize did not return the expected value.");
        }

        // A test for Vector3 (float, float, float)
        TEST_METHOD(Vector3ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;

            Vector3 target = Vector3Helper::FromValues(x, y, z);
            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z), L"Vector3Helper::constructor (x,y,z) did not return the expected value.");
        }

        // A test for Vector3 (Vector2, float)
        TEST_METHOD(Vector3ConstructorTest1)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target = Vector3Helper::FromVector2(a, z);
            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, z), L"Vector3Helper::constructor (Vector2,z) did not return the expected value.");
        }

        // A test for Vector3 (float, float, float)
        TEST_METHOD(Vector3ConstructorTest4)
        {
            Vector3 target = Vector3Helper::FromValues(NAN, FLT_MAX, std::numeric_limits<float>::infinity());

            Assert::IsTrue(isnan(target.X), L"Vector3Helper::constructor (Vector3) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.Y, L"Vector3Helper::constructor (Vector3) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.Z), L"Vector3Helper::constructor (Vector3) did not return the expected value.");
        }

        // A test for Add (Vector3, Vector3)
        TEST_METHOD(Vector3AddTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(5.0f, 6.0f, 7.0f);

            Vector3 expected = Vector3Helper::FromValues(6.0f, 8.0f, 10.0f);
            Vector3 actual;

            actual = Vector3Helper::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Add did not return the expected value.");
        }

        // A test for Divide (Vector3, float)
        TEST_METHOD(Vector3DivideTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float div = 2.0f;
            Vector3 expected = Vector3Helper::FromValues(0.5f, 1.0f, 1.5f);
            Vector3 actual;
            actual = Vector3Helper::DivideByScalar(a, div);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Divide did not return the expected value.");
        }

        // A test for Divide (Vector3, Vector3)
        TEST_METHOD(Vector3DivideTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 6.0f, 7.0f);
            Vector3 b = Vector3Helper::FromValues(5.0f, 2.0f, 3.0f);

            Vector3 expected = Vector3Helper::FromValues(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f);
            Vector3 actual;

            actual = Vector3Helper::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector3, float)
        TEST_METHOD(Vector3MultiplyTest2)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float factor = 2.0f;
            Vector3 expected = Vector3Helper::FromValues(2.0f, 4.0f, 6.0f);
            Vector3 actual = Vector3Helper::MultiplyByScalar(a, factor);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector3, Vector3)
        TEST_METHOD(Vector3MultiplyTest3)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(5.0f, 6.0f, 7.0f);

            Vector3 expected = Vector3Helper::FromValues(5.0f, 12.0f, 21.0f);
            Vector3 actual;

            actual = Vector3Helper::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector3)
        TEST_METHOD(Vector3NegateTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);

            Vector3 expected = Vector3Helper::FromValues(-1.0f, -2.0f, -3.0f);
            Vector3 actual;

            actual = Vector3Helper::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Negate did not return the expected value.");
        }

        // A test for Subtract (Vector3, Vector3)
        TEST_METHOD(Vector3SubtractTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 6.0f, 3.0f);
            Vector3 b = Vector3Helper::FromValues(5.0f, 2.0f, 3.0f);

            Vector3 expected = Vector3Helper::FromValues(-4.0f, 4.0f, 0.0f);
            Vector3 actual;

            actual = Vector3Helper::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Vector3Helper::Subtract did not return the expected value.");
        }

        // A test for One
        TEST_METHOD(Vector3OneTest)
        {
            Vector3 val = Vector3Helper::FromValues(1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, Vector3Helper::One, L"Vector3Helper::One was not set correctly.");
        }

        // A test for UnitX
        TEST_METHOD(Vector3UnitXTest)
        {
            Vector3 val = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector3Helper::UnitX, L"Vector3Helper::UnitX was not set correctly.");
        }

        // A test for UnitY
        TEST_METHOD(Vector3UnitYTest)
        {
            Vector3 val = Vector3Helper::FromValues(0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, Vector3Helper::UnitY, L"Vector3Helper::UnitY was not set correctly.");
        }

        // A test for UnitZ
        TEST_METHOD(Vector3UnitZTest)
        {
            Vector3 val = Vector3Helper::FromValues(0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, Vector3Helper::UnitZ, L"Vector3Helper::UnitZ was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector3ZeroTest)
        {
            Vector3 val = Vector3Helper::FromValues(0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector3Helper::Zero, L"Vector3Helper::Zero was not set correctly.");
        }

        // A test for Vector3 (float)
        TEST_METHOD(Vector3ConstructorTest5)
        {
            float value = 1.0f;
            Vector3 target = Vector3Helper::FromValue(value);

            Vector3 expected = Vector3Helper::FromValues(value, value, value);
            Assert::AreEqual(expected, target, L"Vector3Helper::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector3Helper::FromValue(value);
            expected = Vector3Helper::FromValues(value, value, value);
            Assert::AreEqual(expected, target, L"Vector3Helper::cstr did not return the expected value.");
        }

        struct Vector3_2x
        {
            Vector3 a;
            Vector3 b;
        };

        struct Vector3PlusFloat
        {
            Vector3 v;
            float f;
        };

        struct Vector3PlusFloat_2x
        {
            Vector3PlusFloat a;
            Vector3PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Vector3SizeofTest)
        {
            Assert::AreEqual(size_t(12), sizeof(Vector3));
            Assert::AreEqual(size_t(24), sizeof(Vector3_2x));
            Assert::AreEqual(size_t(16), sizeof(Vector3PlusFloat));
            Assert::AreEqual(size_t(32), sizeof(Vector3PlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Vector3FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Vector3, X));
            Assert::AreEqual(size_t(4), offsetof(Vector3, Y));
            Assert::AreEqual(size_t(8), offsetof(Vector3, Z));
        }
    };
}
