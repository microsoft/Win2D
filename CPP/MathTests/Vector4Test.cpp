//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector4Test)
    {
    public:
        // A test for DistanceSquared (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceSquaredTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 64.0f;
            float actual;

            actual = Vector4::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::DistanceSquared did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 8.0f;
            float actual;

            actual = Vector4::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Distance did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceTest1)
        {
            Vector4 a(Vector2(1.051f, 2.05f), 3.478f, 1.0f);
            Vector4 b(Vector3(1.051f, 2.05f, 3.478f), 0.0f);
            b.W = 1.0f;

            float actual = Vector4::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector4::Distance did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        TEST_METHOD(Vector4DotTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = Vector4::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Dot did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        TEST_METHOD(Vector4DotTest1)
        {
            Vector3 a(1.55f, 1.55f, 1);
            Vector3 b(2.5f, 3, 1.5f);
            Vector3 c = Vector3::Cross(a, b);

            Vector4 d(a, 0);
            Vector4 e(c, 0);

            float actual = Vector4::Dot(d, e);
            Assert::IsTrue(Equal(0.0f, actual), L"Vector4::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector4LengthTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target(a, w);

            float expected = sqrtf(30.0f);
            float actual;

            actual = target.Length();

            Assert::IsTrue(Equal(expected, actual), L"Vector4::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector4LengthTest1)
        {
            Vector4 target;

            float expected = 0.0f;
            float actual = target.Length();

            Assert::IsTrue(Equal(expected, actual), L"Vector4::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector4LengthSquaredTest)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target(a, w);

            float expected = 30;
            float actual;

            actual = target.LengthSquared();

            Assert::IsTrue(Equal(expected, actual), L"Vector4::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector4, Vector4)
        TEST_METHOD(Vector4MinTest)
        {
            Vector4 a(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected(-1.0f, 1.0f, -3.0f, 0.0f);
            Vector4 actual;
            actual = Vector4::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Min did not return the expected value.");
        }

        // A test for Max (Vector4, Vector4)
        TEST_METHOD(Vector4MaxTest)
        {
            Vector4 a(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected(2.0f, 4.0f, -1.0f, 1000.0f);
            Vector4 actual;
            actual = Vector4::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Max did not return the expected value.");
        }
        
        TEST_METHOD(Vector4MinMaxCodeCoverageTest)
        {
            Vector4 min = Vector4::Zero();
            Vector4 max = Vector4::One();
            Vector4 actual;

            // Min.
            actual = Vector4::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector4::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector4::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector4::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Clamp (Vector4, Vector4, Vector4)
        TEST_METHOD(Vector4ClampTest)
        {
            Vector4 a(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 min(0.0f, 0.1f, 0.13f, 0.14f);
            Vector4 max(1.0f, 1.1f, 1.13f, 1.14f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector4 expected(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = max;
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector4(-2.0f, 0.5f, 4.0f, -5.0f);
            expected = Vector4(min.X, a.Y, max.Z, min.W);
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector4(0.0f, 0.1f, 0.13f, 0.14f);
            min = Vector4(1.0f, 1.1f, 1.13f, 1.14f);

            // Case W1: specfied value is in the range.
            a = Vector4(0.5f, 0.3f, 0.33f, 0.44f);
            expected = min;
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector4(2.0f, 3.0f, 4.0f, 5.0f);
            expected = min;
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector4(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Clamp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            float t = 0.5f;

            Vector4 expected(3.0f, 4.0f, 5.0f, 6.0f);
            Vector4 actual;

            actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector4LerpTest1)
        {
            Vector4 a(Vector3(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.0f;
            Vector4 expected(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector4LerpTest2)
        {
            Vector4 a(Vector3(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 1.0f;
            Vector4 expected(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector4LerpTest3)
        {
            Vector4 a(Vector3(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 2.0f;
            Vector4 expected(8.0f, 10.0f, 12.0f, 14.0f);
            Vector4 actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector4LerpTest4)
        {
            Vector4 a(Vector3(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = -2.0f;
            Vector4 expected = -(b * 2);
            Vector4 actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector3, Vector3, float)
        TEST_METHOD(Vector4LerpTest5)
        {
            Vector4 a(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 b(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.85f;
            Vector4 expected = a;
            Vector4 actual = Vector4::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Lerp did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformTest1)
        {
            Vector2 v(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected(10.316987f, 22.183012f, 30.3660259f, 1.0f);
            Vector4 actual;

            actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformTest2)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            Vector4 actual;

            actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected(2.19198728f, 1.53349376f, 2.61602545f, 0.0f);
            Vector4 actual;

            actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");

            // 
            v.W = 1.0f;

            expected = Vector4(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test1)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m;
            Vector4 expected(0, 0, 0, 0);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test2)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m = Matrix4x4::Identity();
            Vector4 expected(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = Vector4::Transform(Vector4(v, 1.0f), m);
            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test1)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Matrix4x4 m;
            Vector4 expected(0, 0, 0, 0);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test2)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Matrix4x4 m = Matrix4x4::Identity();
            Vector4 expected(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test)
        {
            Vector2 v(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Vector4 expected = Vector4::Transform(Vector4(v, 0.0f, 1.0f), m);
            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test1)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix4x4 m;
            Vector4 expected(0, 0, 0, 0);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test2)
        {
            Vector2 v(1.0f, 2.0f);
            Matrix4x4 m = Matrix4x4::Identity();
            Vector4 expected(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuatanionTest)
        {
            Vector2 v(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));

            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4::Transform(v, m);
            Vector4 actual;

            actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3Quaternion)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4::Transform(v, m);
            Vector4 actual;

            actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4::Transform(v, m);
            Vector4 actual;

            actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");

            // 
            v.W = 1.0f;
            expected.W = 1.0f;
            actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest1)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q;
            Vector4 expected = v;

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest2)
        {
            Vector4 v(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q = Quaternion::Identity();
            Vector4 expected(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4::Transform(v, m);
            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest1)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Quaternion q;
            Vector4 expected(v, 1.0f);

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest2)
        {
            Vector3 v(1.0f, 2.0f, 3.0f);
            Quaternion q = Quaternion::Identity();
            Vector4 expected(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuaternionTest)
        {
            Vector2 v(1.0f, 2.0f);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4::Transform(v, m);
            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuaternionTest1)
        {
            Vector2 v(1.0f, 2.0f);
            Quaternion q;
            Vector4 expected(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2QuaternionTest2)
        {
            Vector2 v(1.0f, 2.0f);
            Quaternion q = Quaternion::Identity();
            Vector4 expected(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4::Transform(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected(
                0.1825741858350553711523232609336f,
                0.3651483716701107423046465218672f,
                0.5477225575051661134569697828008f,
                0.7302967433402214846092930437344f);
            Vector4 actual;

            actual = Vector4::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest1)
        {
            Vector4 a(1.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected(1.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector4::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest2)
        {
            Vector4 a(0.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected(0.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W), L"Vector4::Normalize did not return the expected value.");
        }

        // A test for operator - (Vector4)
        TEST_METHOD(Vector4UnaryNegationTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected(-1.0f, -2.0f, -3.0f, -4.0f);
            Vector4 actual;

            actual = -a;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator - did not return the expected value.");
        }

        // A test for operator - (Vector4, Vector4)
        TEST_METHOD(Vector4SubtractionTest)
        {
            Vector4 a(1.0f, 6.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 2.0f, 3.0f, 9.0f);

            Vector4 expected(-4.0f, 4.0f, 0.0f, -5.0f);
            Vector4 actual;

            actual = a - b;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator - did not return the expected value.");
        }

        // A test for operator * (Vector4, float)
        TEST_METHOD(Vector4MultiplyTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;

            Vector4 expected(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual;

            actual = a * factor;
            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator * did not return the expected value.");
        }

        // A test for operator * (float, Vector4)
        TEST_METHOD(Vector4MultiplyTest4)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float factor = 2.0f;
            Vector4 expected(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual;

            actual = factor * a;
            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator * did not return the expected value.");
        }

        // A test for operator * (Vector4, Vector4)
        TEST_METHOD(Vector4MultiplyTest1)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected(5.0f, 12.0f, 21.0f, 32.0f);
            Vector4 actual;

            actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator * did not return the expected value.");
        }

        // A test for operator / (Vector4, float)
        TEST_METHOD(Vector4DivisionTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            float div = 2.0f;

            Vector4 expected(0.5f, 1.0f, 1.5f, 2.0f);
            Vector4 actual;

            actual = a / div;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        TEST_METHOD(Vector4DivisionTest1)
        {
            Vector4 a(1.0f, 6.0f, 7.0f, 4.0f);
            Vector4 b(5.0f, 2.0f, 3.0f, 8.0f);

            Vector4 expected(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            Vector4 actual;

            actual = a / b;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        TEST_METHOD(Vector4DivisionTest2)
        {
            Vector4 a(-2.0f, 3.0f, FLT_MAX, NAN);

            float div = 0.0f;

            Vector4 actual = a / div;

            Assert::IsTrue(IsNegativeInfinity(actual.X), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Y), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(actual.Z), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(isnan(actual.W), L"Vector4::operator / did not return the expected value.");
        }

        // A test for operator / (Vector4, Vector4)
        TEST_METHOD(Vector4DivisionTest3)
        {
            Vector4 a(0.047f, -3.0f, -std::numeric_limits<float>::infinity(), -FLT_MAX);
            Vector4 b;

            Vector4 actual = a / b;

            Assert::IsTrue(IsPositiveInfinity(actual.X), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Y), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.Z), L"Vector4::operator / did not return the expected value.");
            Assert::IsTrue(IsNegativeInfinity(actual.W), L"Vector4::operator / did not return the expected value.");
        }

        // A test for operator + (Vector4, Vector4)
        TEST_METHOD(Vector4AdditionTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected(6.0f, 8.0f, 10.0f, 12.0f);
            Vector4 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"Vector4::operator + did not return the expected value.");
        }

        // A test for Vector4 (float, float, float, float)
        TEST_METHOD(Vector4ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target(x, y, z, w);

            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z) && Equal(target.W, w),
                L"Vector4 constructor(x,y,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector2, float, float)
        TEST_METHOD(Vector4ConstructorTest1)
        {
            Vector2 a(1.0f, 2.0f);
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target(a, z, w);
            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, z) && Equal(target.W, w),
                L"Vector4 constructor(Vector2,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector3, float)
        TEST_METHOD(Vector4ConstructorTest2)
        {
            Vector3 a(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target(a, w);

            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, a.Z) && Equal(target.W, w),
                L"Vector4 constructor(Vector3,w) did not return the expected value.");
        }

        // A test for Vector4 ()
        TEST_METHOD(Vector4ConstructorTest4)
        {
            Vector4 a;

            Assert::AreEqual(a.X, 0.0f, L"Vector4::constructor () did not return the expected value.");
            Assert::AreEqual(a.Y, 0.0f, L"Vector4::constructor () did not return the expected value.");
            Assert::AreEqual(a.Z, 0.0f, L"Vector4::constructor () did not return the expected value.");
            Assert::AreEqual(a.W, 0.0f, L"Vector4::constructor () did not return the expected value.");
        }

        // A test for Vector4 ()
        TEST_METHOD(Vector4ConstructorTest5)
        {
            Vector4 target(NAN, FLT_MAX, std::numeric_limits<float>::infinity(), FLT_EPSILON);

            Assert::IsTrue(isnan(target.X), L"Vector4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.Y, L"Vector4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.Z), L"Vector4::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_EPSILON == target.W, L"Vector4::constructor (float, float, float, float) did not return the expected value.");
        }

        // A test for Add (Vector4, Vector4)
        TEST_METHOD(Vector4AddTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected(6.0f, 8.0f, 10.0f, 12.0f);
            Vector4 actual;

            actual = Vector4::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector4::Add did not return the expected value.");
        }

        // A test for Divide (Vector4, float)
        TEST_METHOD(Vector4DivideTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float div = 2.0f;
            Vector4 expected(0.5f, 1.0f, 1.5f, 2.0f);
            Vector4 actual;
            actual = Vector4::Divide(a, div);
            Assert::AreEqual(expected, actual, L"Vector4::Divide did not return the expected value.");
        }

        // A test for Divide (Vector4, Vector4)
        TEST_METHOD(Vector4DivideTest1)
        {
            Vector4 a(1.0f, 6.0f, 7.0f, 4.0f);
            Vector4 b(5.0f, 2.0f, 3.0f, 8.0f);

            Vector4 expected(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            Vector4 actual;

            actual = Vector4::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector4::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector4, float)
        TEST_METHOD(Vector4MultiplyTest2)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 2.0f;
            Vector4 expected(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual = Vector4::Multiply(a, factor);
            Assert::AreEqual(expected, actual, L"Vector4::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector4, Vector4)
        TEST_METHOD(Vector4MultiplyTest3)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected(5.0f, 12.0f, 21.0f, 32.0f);
            Vector4 actual;

            actual = Vector4::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector4::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector4)
        TEST_METHOD(Vector4NegateTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected(-1.0f, -2.0f, -3.0f, -4.0f);
            Vector4 actual;

            actual = Vector4::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector4::Negate did not return the expected value.");
        }

        // A test for operator != (Vector4, Vector4)
        TEST_METHOD(Vector4InequalityTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector4::operator != did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Vector4::operator != did not return the expected value.");
        }

        // A test for operator == (Vector4, Vector4)
        TEST_METHOD(Vector4EqualityTest)
        {
            Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector4::operator == did not return the expected value.");

            // case 2: compare between different values
            b.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Vector4::operator == did not return the expected value.");
        }

        // A test for Subtract (Vector4, Vector4)
        TEST_METHOD(Vector4SubtractTest)
        {
            Vector4 a(1.0f, 6.0f, 3.0f, 4.0f);
            Vector4 b(5.0f, 2.0f, 3.0f, 9.0f);

            Vector4 expected(-4.0f, 4.0f, 0.0f, -5.0f);
            Vector4 actual;

            actual = Vector4::Subtract(a, b);

            Assert::AreEqual(expected, actual, L"Vector4::Subtract did not return the expected value.");
        }

        // A test for UnitW
        TEST_METHOD(Vector4UnitWTest)
        {
            Vector4 val(0.0f, 0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, Vector4::UnitW(), L"Vector4::UnitW was not set correctly.");
        }

        // A test for UnitX()
        TEST_METHOD(Vector4UnitXTest)
        {
            Vector4 val(1.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4::UnitX(), L"Vector4::UnitX() was not set correctly.");
        }

        // A test for UnitY()
        TEST_METHOD(Vector4UnitYTest)
        {
            Vector4 val(0.0f, 1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4::UnitY(), L"Vector4::UnitY() was not set correctly.");
        }

        // A test for UnitZ
        TEST_METHOD(Vector4UnitZTest)
        {
            Vector4 val(0.0f, 0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, Vector4::UnitZ(), L"Vector4::UnitZ was not set correctly.");
        }

        // A test for One
        TEST_METHOD(Vector4OneTest)
        {
            Vector4 val(1.0f, 1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, Vector4::One(), L"Vector4::One was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector4ZeroTest)
        {
            Vector4 val(0.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4::Zero(), L"Vector4::Zero was not set correctly.");
        }

        // A test for Vector4 (float)
        TEST_METHOD(Vector4ConstructorTest6)
        {
            float value = 1.0f;
            Vector4 target(value);

            Vector4 expected(value, value, value, value);
            Assert::AreEqual(expected, target, L"Vector4::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector4(value);
            expected = Vector4(value, value, value, value);
            Assert::AreEqual(expected, target, L"Vector4::cstr did not return the expected value.");
        }

        // A test for Vector4 comparison involving NaN values
        TEST_METHOD(Vector4EqualsNanTest)
        {
            Vector4 a(NAN, 0, 0, 0);
            Vector4 b(0, NAN, 0, 0);
            Vector4 c(0, 0, NAN, 0);
            Vector4 d(0, 0, 0, NAN);

            Assert::IsFalse(a == Vector4::Zero());
            Assert::IsFalse(b == Vector4::Zero());
            Assert::IsFalse(c == Vector4::Zero());
            Assert::IsFalse(d == Vector4::Zero());

            Assert::IsTrue(a != Vector4::Zero());
            Assert::IsTrue(b != Vector4::Zero());
            Assert::IsTrue(c != Vector4::Zero());
            Assert::IsTrue(d != Vector4::Zero());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
        }

        struct Vector4_2x
        {
            Vector4 a;
            Vector4 b;
        };

        struct Vector4PlusFloat
        {
            Vector4 v;
            float f;
        };

        struct Vector4PlusFloat_2x
        {
            Vector4PlusFloat a;
            Vector4PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Vector4SizeofTest)
        {
            Assert::AreEqual(16u, sizeof(Vector4));
            Assert::AreEqual(32u, sizeof(Vector4_2x));
            Assert::AreEqual(20u, sizeof(Vector4PlusFloat));
            Assert::AreEqual(40u, sizeof(Vector4PlusFloat_2x));
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Vector4TypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Vector4>::value);
            Assert::IsFalse(std::is_pod<Vector4>::value);
            Assert::IsFalse(std::is_trivial<Vector4>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Vector4>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Vector4>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Vector4>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Vector4>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Vector4>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Vector4>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Vector4>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Vector4>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Vector4>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Vector4>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Vector4>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Vector4>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Vector4>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Vector4>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Vector4>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Vector4>::value);
            Assert::IsTrue(std::is_trivially_destructible<Vector4>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Vector4>::value);
        }
    };
}
