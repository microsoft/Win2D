//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector3Test)
    {
    public:
        // A test for Cross (Vector3, Vector3)
        TEST_METHOD(Vector3CrossTest)
        {
            Vector3 a(1.0f, 0.0f, 0.0f);
            Vector3 b(0.0f, 1.0f, 0.0f);

            Vector3 expected(0.0f, 0.0f, 1.0f);
            Vector3 actual;

            actual = Vector3::Cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Cross did not return the expected value.");
        }

        // A test for Cross (Vector3, Vector3)
        TEST_METHOD(Vector3CrossTest1)
        {
            Vector3 a(0.0f, 1.0f, 0.0f);
            Vector3 b(0.0f, 1.0f, 0.0f);

            Vector3 expected(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3::Cross(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Cross did not return the expected value.");
        }

        // A test for Distance (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float expected = sqrtf(27);
            float actual;

            actual = Vector3::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Distance did not return the expected value.");
        }

        // A test for Distance (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceTest1)
        {
            Vector3 a(1.051f, 2.05f, 3.478f);
            Vector3 b(Vector2(1.051f, 0.0f), 1);
            b.Y = 2.05f;
            b.Z = 3.478f;

            float actual = Vector3::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector3::Distance did not return the expected value.");
        }

        // A test for DistanceSquared (Vector3, Vector3)
        TEST_METHOD(Vector3DistanceSquaredTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float expected = 27.0f;
            float actual;

            actual = Vector3::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::DistanceSquared did not return the expected value.");
        }

        // A test for Dot (Vector3, Vector3)
        TEST_METHOD(Vector3DotTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float expected = 32.0f;
            float actual;

            actual = Vector3::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Dot did not return the expected value.");
        }

        // A test for Dot (Vector3, Vector3)
        TEST_METHOD(Vector3DotTest1)
        {
            Vector3 a(1.55f, 1.55f, 1);
            Vector3 b(2.5f, 3, 1.5f);
            Vector3 c = Vector3::Cross(a, b);

            float expected = 0.0f;
            float actual1 = Vector3::Dot(a, c);
            float actual2 = Vector3::Dot(b, c);
            Assert::IsTrue(Equal(expected, actual1), L"Vector3::Dot did not return the expected value.");
            Assert::IsTrue(Equal(expected, actual2), L"Vector3::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector3LengthTest)
        {
            Vector2 a(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target(a, z);

            float expected = sqrtf(14.0f);
            float actual;

            actual = target.Length();
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector3LengthTest1)
        {
            Vector3 target;

            float expected = 0.0f;
            float actual = target.Length();
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector3LengthSquaredTest)
        {
            Vector2 a(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target(a, z);

            float expected = 14.0f;
            float actual;

            actual = target.LengthSquared();
            Assert::IsTrue(Equal(expected, actual), L"Vector3::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector3, Vector3)
        TEST_METHOD(Vector3MinTest)
        {
            Vector3 a(-1.0f, 4.0f, -3.0f);
            Vector3 b(2.0f, 1.0f, -1.0f);

            Vector3 expected(-1.0f, 1.0f, -3.0f);
            Vector3 actual;
            actual = Vector3::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Min did not return the expected value.");
        }

        // A test for Max (Vector3, Vector3)
        TEST_METHOD(Vector3MaxTest)
        {
            Vector3 a(-1.0f, 4.0f, -3.0f);
            Vector3 b(2.0f, 1.0f, -1.0f);

            Vector3 expected(2.0f, 4.0f, -1.0f);
            Vector3 actual;
            actual = Vector3::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Max did not return the expected value.");
        }

        TEST_METHOD(Vector3MinMaxCodeCoverageTest)
        {
            Vector3 min = Vector3::Zero();
            Vector3 max = Vector3::One();
            Vector3 actual;

            // Min.
            actual = Vector3::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector3::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector3::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector3::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float t = 0.5f;

            Vector3 expected(2.5f, 3.5f, 4.5f);
            Vector3 actual;

            actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest1)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float t = 0.0f;
            Vector3 expected(1.0f, 2.0f, 3.0f);
            Vector3 actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest2)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float t = 1.0f;
            Vector3 expected(4.0f, 5.0f, 6.0f);
            Vector3 actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest3)
        {
            Vector3 a(0.0f, 0.0f, 0.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float t = 2.0f;
            Vector3 expected(8.0f, 10.0f, 12.0f);
            Vector3 actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest4)
        {
            Vector3 a(0.0f, 0.0f, 0.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            float t = -2.0f;
            Vector3 expected(-8.0f, -10.0f, -12.0f);
            Vector3 actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector3LerpTest5)
        {
            Vector3 a(1.68f, 2.34f, 5.43f);
            Vector3 b = a;

            float t = 0.18f;
            Vector3 expected(1.68f, 2.34f, 5.43f);
            Vector3 actual = Vector3::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Lerp did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest)
        {
            Vector3 a = Vector3::Normalize(Vector3(1.0f, 1.0f, 1.0f));

            // Reflect on XZ plane.
            Vector3 n(0.0f, 1.0f, 0.0f);
            Vector3 expected(a.X, -a.Y, a.Z);
            Vector3 actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");

            // Reflect on XY plane.
            n = Vector3(0.0f, 0.0f, 1.0f);
            expected = Vector3(a.X, a.Y, -a.Z);
            actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");

            // Reflect on YZ plane.
            n = Vector3(1.0f, 0.0f, 0.0f);
            expected = Vector3(-a.X, a.Y, a.Z);
            actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest1)
        {
            Vector3 n(0.45f, 1.28f, 0.86f);
            n = Vector3::Normalize(n);
            Vector3 a = n;

            Vector3 expected = -n;
            Vector3 actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest2)
        {
            Vector3 n(0.45f, 1.28f, 0.86f);
            n = Vector3::Normalize(n);
            Vector3 a = -n;

            Vector3 expected = n;
            Vector3 actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");
        }

        // A test for Reflect (Vector3, Vector3)
        TEST_METHOD(Vector3ReflectTest3)
        {
            Vector3 n(0.45f, 1.28f, 0.86f);
            Vector3 temp(1.28f, 0.45f, 0.01f);
            // find a perpendicular vector of n
            Vector3 a = Vector3::Cross(temp, n);

            Vector3 expected = a;
            Vector3 actual = Vector3::Reflect(a, n);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Reflect did not return the expected value.");
        }

        // A test for Transform(Vector3, Matrix4x4)
        TEST_METHOD(Vector3TransformTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector3 expected(12.191987f, 21.533493f, 32.616024f);
            Vector3 actual;

            actual = Vector3::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Transform did not return the expected value.");
        }

        // A test for Clamp (Vector3, Vector3, Vector3)
        TEST_METHOD(Vector3ClampTest)
        {
            Vector3 a(0.5f, 0.3f, 0.33f);
            Vector3 min(0.0f, 0.1f, 0.13f);
            Vector3 max(1.0f, 1.1f, 1.13f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector3 expected(0.5f, 0.3f, 0.33f);
            Vector3 actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector3(2.0f, 3.0f, 4.0f);
            expected = max;
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector3(-2.0f, 0.5f, 4.0f);
            expected = Vector3(min.X, a.Y, max.Z);
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector3(0.0f, 0.1f, 0.13f);
            min = Vector3(1.0f, 1.1f, 1.13f);

            // Case W1: specfied value is in the range.
            a = Vector3(0.5f, 0.3f, 0.33f);
            expected = min;
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector3(2.0f, 3.0f, 4.0f);
            expected = min;
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector3(-2.0f, -3.0f, -4.0f);
            expected = min;
            actual = Vector3::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Clamp did not return the expected value.");
        }

        // A test for TransformNormal (Vector3, Matrix4x4)
        TEST_METHOD(Vector3TransformNormalTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector3 expected(2.19198728f, 1.53349364f, 2.61602545f);
            Vector3 actual;

            actual = Vector3::TransformNormal(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::TransformNormal did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector3 expected = Vector3::Transform(v, m);
            Vector3 actual = Vector3::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest1)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Quaternion q;
            Vector3 expected = v;

            Vector3 actual = Vector3::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector3TransformByQuaternionTest2)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Quaternion q = Quaternion::Identity();
            Vector3 expected = v;

            Vector3 actual = Vector3::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            Vector3 expected(
                0.26726124191242438468455348087975f,
                0.53452248382484876936910696175951f,
                0.80178372573727315405366044263926f);
            Vector3 actual;

            actual = Vector3::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest1)
        {
            Vector3 a(1.0f, 0.0f, 0.0f);

            Vector3 expected(1.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector3::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector3)
        TEST_METHOD(Vector3NormalizeTest2)
        {
            Vector3 a(0.0f, 0.0f, 0.0f);

            Vector3 expected(0.0f, 0.0f, 0.0f);
            Vector3 actual = Vector3::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z), L"Vector3::Normalize did not return the expected value.");
        }

        // A test for operator - (Vector3)
        TEST_METHOD(Vector3UnaryNegationTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            Vector3 expected(-1.0f, -2.0f, -3.0f);
            Vector3 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator - did not return the expected value.");
        }

        // A test for operator - (Vector3, Vector3)
        TEST_METHOD(Vector3SubtractionTest)
        {
            Vector3 a(4.0f, 2.0f, 3.0f);

            Vector3 b(1.0f, 5.0f, 7.0f);

            Vector3 expected(3.0f, -3.0f, -4.0f);
            Vector3 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator - did not return the expected value.");
        }

        // A test for operator * (Vector3, float)
        TEST_METHOD(Vector3MultiplyTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            float factor = 2.0f;

            Vector3 expected(2.0f, 4.0f, 6.0f);
            Vector3 actual;

            actual = a * factor;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator * did not return the expected value.");
        }

        // A test for operator * (Vector3, Vector3)
        TEST_METHOD(Vector3MultiplyTest1)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            Vector3 b(4.0f, 5.0f, 6.0f);

            Vector3 expected(4.0f, 10.0f, 18.0f);
            Vector3 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator * did not return the expected value.");
        }

        // A test for operator / (Vector3, float)
        TEST_METHOD(Vector3DivisionTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            float div = 2.0f;

            Vector3 expected(0.5f, 1.0f, 1.5f);
            Vector3 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator / did not return the expected value.");
        }

        // A test for operator / (Vector3, Vector3)
        TEST_METHOD(Vector3DivisionTest1)
        {
            Vector3 a(4.0f, 2.0f, 3.0f);

            Vector3 b(1.0f, 5.0f, 6.0f);

            Vector3 expected(4.0f, 0.4f, 0.5f);
            Vector3 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator / did not return the expected value.");
        }

        // A test for operator / (Vector3, Vector3)
        TEST_METHOD(Vector3DivisionTest2)
        {
            Vector3 a(-2.0f, 3.0f, FLT_MAX);

            float div = 0.0f;

            Vector3 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.X), L"Vector3::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Y), L"Vector3::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Z), L"Vector3::operator / did not return the expected value.");

        }

        // A test for operator / (Vector2, Vector2)
        TEST_METHOD(Vector3DivisionTest3)
        {
            Vector3 a(0.047f, -3.0f, -std::numeric_limits<float>::infinity());
            Vector3 b;

            Vector3 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.X), L"Vector3::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Y), L"Vector3::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Z), L"Vector3::operator / did not return the expected value.");
        }

        // A test for operator + (Vector3, Vector3)
        TEST_METHOD(Vector3AdditionTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(4.0f, 5.0f, 6.0f);

            Vector3 expected(5.0f, 7.0f, 9.0f);
            Vector3 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"Vector3::operator + did not return the expected value.");
        }

        // A test for Vector3 (float, float, float)
        TEST_METHOD(Vector3ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;

            Vector3 target(x, y, z);
            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z), L"Vector3::constructor (x,y,z) did not return the expected value.");
        }

        // A test for Vector3 (Vector2, float)
        TEST_METHOD(Vector3ConstructorTest1)
        {
            Vector2 a(1.0f, 2.0f);

            float z = 3.0f;

            Vector3 target(a, z);
            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, z), L"Vector3::constructor (Vector2,z) did not return the expected value.");
        }

        // A test for Vector3 (Vector3)
        TEST_METHOD(Vector3ConstructorTest2)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            Vector3 target = a;

            Assert::IsTrue(Equal(target.X, 1.0f) && Equal(target.Y, 2.0f) && Equal(target.Z, 3.0f), L"Vector3::constructor (Vector3) did not return the expected value.");
        }

        // A test for Vector3 ()
        TEST_METHOD(Vector3ConstructorTest3)
        {
            Vector3 a;

            Assert::AreEqual(a.X, 0.0f, L"Vector3::constructor () did not return the expected value.");
            Assert::AreEqual(a.Y, 0.0f, L"Vector3::constructor () did not return the expected value.");
            Assert::AreEqual(a.Z, 0.0f, L"Vector3::constructor () did not return the expected value.");
        }

        // A test for Vector3 (float, float, float)
        TEST_METHOD(Vector3ConstructorTest4)
        {
            Vector3 target(NAN, FLT_MAX, std::numeric_limits<float>::infinity());

            Assert::IsTrue(isnan(target.X), L"Vector3::constructor (Vector3) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.Y, L"Vector3::constructor (Vector3) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.Z), L"Vector3::constructor (Vector3) did not return the expected value.");
        }

        // A test for Add (Vector3, Vector3)
        TEST_METHOD(Vector3AddTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(5.0f, 6.0f, 7.0f);

            Vector3 expected(6.0f, 8.0f, 10.0f);
            Vector3 actual;

            actual = Vector3::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector3::Add did not return the expected value.");
        }

        // A test for Divide (Vector3, float)
        TEST_METHOD(Vector3DivideTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            float div = 2.0f;
            Vector3 expected(0.5f, 1.0f, 1.5f);
            Vector3 actual;
            actual = Vector3::Divide(a, div);
            Assert::AreEqual(expected, actual, L"Vector3::Divide did not return the expected value.");
        }

        // A test for Divide (Vector3, Vector3)
        TEST_METHOD(Vector3DivideTest1)
        {
            Vector3 a(1.0f, 6.0f, 7.0f);
            Vector3 b(5.0f, 2.0f, 3.0f);

            Vector3 expected(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f);
            Vector3 actual;

            actual = Vector3::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector3::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector3, float)
        TEST_METHOD(Vector3MultiplyTest2)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            float factor = 2.0f;
            Vector3 expected(2.0f, 4.0f, 6.0f);
            Vector3 actual = Vector3::Multiply(a, factor);
            Assert::AreEqual(expected, actual, L"Vector3::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector3, Vector3)
        TEST_METHOD(Vector3MultiplyTest3)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(5.0f, 6.0f, 7.0f);

            Vector3 expected(5.0f, 12.0f, 21.0f);
            Vector3 actual;

            actual = Vector3::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector3::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector3)
        TEST_METHOD(Vector3NegateTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);

            Vector3 expected(-1.0f, -2.0f, -3.0f);
            Vector3 actual;

            actual = Vector3::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector3::Negate did not return the expected value.");
        }

        // A test for operator != (Vector3, Vector3)
        TEST_METHOD(Vector3InequalityTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector3::operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector3::operator != did not return the expected value.");
        }

        // A test for operator == (Vector3, Vector3)
        TEST_METHOD(Vector3EqualityTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            Vector3 b(1.0f, 2.0f, 3.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector3::operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector3::operator == did not return the expected value.");
        }

        // A test for Subtract (Vector3, Vector3)
        TEST_METHOD(Vector3SubtractTest)
        {
            Vector3 a(1.0f, 6.0f, 3.0f);
            Vector3 b(5.0f, 2.0f, 3.0f);

            Vector3 expected(-4.0f, 4.0f, 0.0f);
            Vector3 actual;

            actual = Vector3::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Vector3::Subtract did not return the expected value.");
        }

        // A test for One
        TEST_METHOD(Vector3OneTest)
        {
            Vector3 val(1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, Vector3::One(), L"Vector3::One was not set correctly.");
        }

        // A test for UnitX()
        TEST_METHOD(Vector3UnitXTest)
        {
            Vector3 val(1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector3::UnitX(), L"Vector3::UnitX() was not set correctly.");
        }

        // A test for UnitY()
        TEST_METHOD(Vector3UnitYTest)
        {
            Vector3 val(0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, Vector3::UnitY(), L"Vector3::UnitY() was not set correctly.");
        }

        // A test for UnitZ
        TEST_METHOD(Vector3UnitZTest)
        {
            Vector3 val(0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, Vector3::UnitZ(), L"Vector3::UnitZ() was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector3ZeroTest)
        {
            Vector3 val(0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector3::Zero(), L"Vector3::Zero() was not set correctly.");
        }

        // A test for operator +=
        TEST_METHOD(Vector3OperatorAddEqualsTest)
        {
            Vector3 a(1, 2, 3);
            Vector3 b(4, 5, 6);
            Vector3 expected = Vector3::Add(a, b);
            
            // In-place += operation.
            Vector3& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector3::Add(b, b);
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Vector3OperatorSubtractEqualsTest)
        {
            Vector3 a(1, 2, 3);
            Vector3 b(4, 5, 6);
            Vector3 expected = Vector3::Subtract(a, b);
            
            // In-place -= operation.
            Vector3& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector3::Subtract(b, b);
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Vector3OperatorMultiplyEqualsTest)
        {
            Vector3 a(1, 2, 3);
            Vector3 b(4, 5, 6);
            Vector3 expected = Vector3::Multiply(a, b);
            
            // In-place *= operation.
            Vector3& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector3::Multiply(b, b);
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Vector3OperatorMultiplyEqualsScalerTest)
        {
            Vector3 a(1, 2, 3);
            float b = 5;
            Vector3 expected = Vector3::Multiply(a, b);
            
            // In-place *= operation.
            Vector3& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for operator /=
        TEST_METHOD(Vector3OperatorDivideEqualsTest)
        {
            Vector3 a(1, 2, 3);
            Vector3 b(4, 5, 6);
            Vector3 expected = Vector3::Divide(a, b);
            
            // In-place /= operation.
            Vector3& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = Vector3::Divide(b, b);
            b /= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator /=
        TEST_METHOD(Vector3OperatorDivideEqualsScalerTest)
        {
            Vector3 a(1, 2, 3);
            float b = 5;
            Vector3 expected = Vector3::Divide(a, b);
            
            // In-place /= operation.
            Vector3& ret = a /= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for Vector3 (float)
        TEST_METHOD(Vector3ConstructorTest5)
        {
            float value = 1.0f;
            Vector3 target(value);

            Vector3 expected(value, value, value);
            Assert::AreEqual(expected, target, L"Vector3::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector3(value);
            expected = Vector3(value, value, value);
            Assert::AreEqual(expected, target, L"Vector3::cstr did not return the expected value.");
        }

        // A test for Vector3 comparison involving NaN values
        TEST_METHOD(Vector3EqualsNanTest)
        {
            Vector3 a(NAN, 0, 0);
            Vector3 b(0, NAN, 0);
            Vector3 c(0, 0, NAN);

            Assert::IsFalse(a == Vector3::Zero());
            Assert::IsFalse(b == Vector3::Zero());
            Assert::IsFalse(c == Vector3::Zero());

            Assert::IsTrue(a != Vector3::Zero());
            Assert::IsTrue(b != Vector3::Zero());
            Assert::IsTrue(c != Vector3::Zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
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
            Assert::AreEqual(sizeof(Vector3), sizeof(DirectX::XMFLOAT3));
        }

        // A test of Vector3 -> DirectXMath interop
        TEST_METHOD(Vector3LoadTest)
        {
            Vector3 a(23, 42, 666);
            DirectX::XMVECTOR b = DirectX::XMLoadVector3(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.X, c.x);
            Assert::AreEqual(a.Y, c.y);
            Assert::AreEqual(a.Z, c.z);
            Assert::AreEqual(0.0f, c.w);
        }

        // A test of DirectXMath -> Vector3 interop
        TEST_METHOD(Vector3StoreTest)
        {
            DirectX::XMFLOAT3 a(23, 42, 666);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&a);
            Vector3 c;
            DirectX::XMStoreVector3(&c, b);

            Assert::AreEqual(a.x, c.X);
            Assert::AreEqual(a.y, c.Y);
            Assert::AreEqual(a.z, c.Z);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Vector3TypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Vector3>::value);
            Assert::IsFalse(std::is_pod<Vector3>::value);
            Assert::IsFalse(std::is_trivial<Vector3>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Vector3>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Vector3>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Vector3>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Vector3>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Vector3>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Vector3>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Vector3>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Vector3>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Vector3>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Vector3>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Vector3>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Vector3>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Vector3>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Vector3>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Vector3>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Vector3>::value);
            Assert::IsTrue(std::is_trivially_destructible<Vector3>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Vector3>::value);
        }
    };
}
