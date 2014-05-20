//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::DirectX::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Vector4Test)
    {
    public:
        // A test for DistanceSquared (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceSquaredTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 64.0f;
            float actual;

            actual = Vector4Helper::DistanceSquared(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::DistanceSquared did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 8.0f;
            float actual;

            actual = Vector4Helper::Distance(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Distance did not return the expected value.");
        }

        // A test for Distance (Vector4, Vector4)
        TEST_METHOD(Vector4DistanceTest1)
        {
            Vector4 a = Vector4Helper::FromVector2(Vector2Helper::FromValues(1.051f, 2.05f), 3.478f, 1.0f);
            Vector4 b = Vector4Helper::FromVector3(Vector3Helper::FromValues(1.051f, 2.05f, 3.478f), 0.0f);
            b.W = 1.0f;

            float actual = Vector4Helper::Distance(a, b);
            Assert::AreEqual(0.0f, actual, L"Vector4Helper::Distance did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        TEST_METHOD(Vector4DotTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            float expected = 70.0f;
            float actual;

            actual = Vector4Helper::Dot(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Dot did not return the expected value.");
        }

        // A test for Dot (Vector4, Vector4)
        TEST_METHOD(Vector4DotTest1)
        {
            Vector3 a = Vector3Helper::FromValues(1.55f, 1.55f, 1);
            Vector3 b = Vector3Helper::FromValues(2.5f, 3, 1.5f);
            Vector3 c = Vector3Helper::Cross(a, b);

            Vector4 d = Vector4Helper::FromVector3(a, 0);
            Vector4 e = Vector4Helper::FromVector3(c, 0);

            float actual = Vector4Helper::Dot(d, e);
            Assert::IsTrue(Equal(0.0f, actual), L"Vector4Helper::Dot did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector4LengthTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = Vector4Helper::FromVector3(a, w);

            float expected = sqrtf(30.0f);
            float actual;

            actual = Vector4Helper::Length(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Length did not return the expected value.");
        }

        // A test for Length ()
        TEST_METHOD(Vector4LengthTest1)
        {
            Vector4 target = Vector4Helper::Zero;

            float expected = 0.0f;
            float actual = Vector4Helper::Length(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Length did not return the expected value.");
        }

        // A test for LengthSquared ()
        TEST_METHOD(Vector4LengthSquaredTest)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = Vector4Helper::FromVector3(a, w);

            float expected = 30;
            float actual;

            actual = Vector4Helper::LengthSquared(target);

            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::LengthSquared did not return the expected value.");
        }

        // A test for Min (Vector4, Vector4)
        TEST_METHOD(Vector4MinTest)
        {
            Vector4 a = Vector4Helper::FromValues(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b = Vector4Helper::FromValues(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected = Vector4Helper::FromValues(-1.0f, 1.0f, -3.0f, 0.0f);
            Vector4 actual;
            actual = Vector4Helper::Min(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Min did not return the expected value.");
        }

        // A test for Max (Vector4, Vector4)
        TEST_METHOD(Vector4MaxTest)
        {
            Vector4 a = Vector4Helper::FromValues(-1.0f, 4.0f, -3.0f, 1000.0f);
            Vector4 b = Vector4Helper::FromValues(2.0f, 1.0f, -1.0f, 0.0f);

            Vector4 expected = Vector4Helper::FromValues(2.0f, 4.0f, -1.0f, 1000.0f);
            Vector4 actual;
            actual = Vector4Helper::Max(a, b);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Max did not return the expected value.");
        }
        
        TEST_METHOD(Vector4MinMaxCodeCoverageTest)
        {
            Vector4 min = Vector4Helper::Zero;
            Vector4 max = Vector4Helper::One;
            Vector4 actual;

            // Min.
            actual = Vector4Helper::Min(min, max);
            Assert::AreEqual(actual, min);

            actual = Vector4Helper::Min(max, min);
            Assert::AreEqual(actual, min);

            // Max.
            actual = Vector4Helper::Max(min, max);
            Assert::AreEqual(actual, max);

            actual = Vector4Helper::Max(max, min);
            Assert::AreEqual(actual, max);
        }

        // A test for Clamp (Vector4, Vector4, Vector4)
        TEST_METHOD(Vector4ClampTest)
        {
            Vector4 a = Vector4Helper::FromValues(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 min = Vector4Helper::FromValues(0.0f, 0.1f, 0.13f, 0.14f);
            Vector4 max = Vector4Helper::FromValues(1.0f, 1.1f, 1.13f, 1.14f);

            // Normal case.
            // Case N1: specfied value is in the range.
            Vector4 expected = Vector4Helper::FromValues(0.5f, 0.3f, 0.33f, 0.44f);
            Vector4 actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case N2: specfied value is bigger than max value.
            a = Vector4Helper::FromValues(2.0f, 3.0f, 4.0f, 5.0f);
            expected = max;
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // Case N3: specfied value is smaller than max value.
            a = Vector4Helper::FromValues(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // Case N4: combination case.
            a = Vector4Helper::FromValues(-2.0f, 0.5f, 4.0f, -5.0f);
            expected = Vector4Helper::FromValues(min.X, a.Y, max.Z, min.W);
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // User specfied min value is bigger than max value.
            max = Vector4Helper::FromValues(0.0f, 0.1f, 0.13f, 0.14f);
            min = Vector4Helper::FromValues(1.0f, 1.1f, 1.13f, 1.14f);

            // Case W1: specfied value is in the range.
            a = Vector4Helper::FromValues(0.5f, 0.3f, 0.33f, 0.44f);
            expected = min;
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // Normal case.
            // Case W2: specfied value is bigger than max and min value.
            a = Vector4Helper::FromValues(2.0f, 3.0f, 4.0f, 5.0f);
            expected = min;
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");

            // Case W3: specfied value is smaller than min and max value.
            a = Vector4Helper::FromValues(-2.0f, -3.0f, -4.0f, -5.0f);
            expected = min;
            actual = Vector4Helper::Clamp(a, min, max);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Clamp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            float t = 0.5f;

            Vector4 expected = Vector4Helper::FromValues(3.0f, 4.0f, 5.0f, 6.0f);
            Vector4 actual;

            actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest1)
        {
            Vector4 a = Vector4Helper::FromVector3(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.0f;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest2)
        {
            Vector4 a = Vector4Helper::FromVector3(Vector3Helper::FromValues(1.0f, 2.0f, 3.0f), 4.0f);
            Vector4 b = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 1.0f;
            Vector4 expected = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest3)
        {
            Vector4 a = Vector4Helper::FromVector3(Vector3Helper::FromValues(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 2.0f;
            Vector4 expected = Vector4Helper::FromValues(8.0f, 10.0f, 12.0f, 14.0f);
            Vector4 actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest4)
        {
            Vector4 a = Vector4Helper::FromVector3(Vector3Helper::FromValues(0.0f, 0.0f, 0.0f), 0.0f);
            Vector4 b = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);

            float t = -2.0f;
            Vector4 expected = Vector4Helper::Negate(Vector4Helper::MultiplyByScalar(b, 2));
            Vector4 actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Lerp (Vector4, Vector4, float)
        TEST_METHOD(Vector4LerpTest5)
        {
            Vector4 a = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);
            Vector4 b = Vector4Helper::FromValues(4.0f, 5.0f, 6.0f, 7.0f);

            float t = 0.85f;
            Vector4 expected = a;
            Vector4 actual = Vector4Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Lerp did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformTest1)
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

            Vector4 expected = Vector4Helper::FromValues(10.316987f, 22.183012f, 30.3660259f, 1.0f);
            Vector4 actual;

            actual = Vector4Helper::TransformVector2(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformTest2)
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

            Vector4 expected = Vector4Helper::FromValues(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            Vector4 actual;

            actual = Vector4Helper::TransformVector3(v, m);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);

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

            Vector4 expected = Vector4Helper::FromValues(2.19198728f, 1.53349376f, 2.61602545f, 0.0f);
            Vector4 actual;

            actual = Vector4Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");

            // 
            v.W = 1.0f;

            expected = Vector4Helper::FromValues(12.19198728f, 21.53349376f, 32.61602545f, 1.0f);
            actual = Vector4Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test1)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m = Matrix4x4Helper::FromValues(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Vector4 expected = Vector4Helper::FromValues(0, 0, 0, 0);

            Vector4 actual = Vector4Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Matrix4x4)
        TEST_METHOD(Vector4TransformVector4Test2)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);
            Matrix4x4 m = Matrix4x4Helper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4Helper::Transform(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test)
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

            Vector4 expected = Vector4Helper::Transform(Vector4Helper::FromVector3(v, 1.0f), m);
            Vector4 actual = Vector4Helper::TransformVector3(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test1)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Matrix4x4 m = Matrix4x4Helper::FromValues(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Vector4 expected = Vector4Helper::FromValues(0, 0, 0, 0);

            Vector4 actual = Vector4Helper::TransformVector3(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Matrix4x4)
        TEST_METHOD(Vector4TransformVector3Test2)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Matrix4x4 m = Matrix4x4Helper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector3(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test)
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

            Vector4 expected = Vector4Helper::Transform(Vector4Helper::FromVector2(v, 0.0f, 1.0f), m);
            Vector4 actual = Vector4Helper::TransformVector2(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test1)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix4x4 m = Matrix4x4Helper::FromValues(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Vector4 expected = Vector4Helper::FromValues(0, 0, 0, 0);

            Vector4 actual = Vector4Helper::TransformVector2(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2Test2)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Matrix4x4 m = Matrix4x4Helper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector2(v, m);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuatanionTest)
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

            Vector4 expected = Vector4Helper::TransformVector2(v, m);
            Vector4 actual;

            actual = Vector4Helper::TransformVector2ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3Quaternion)
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

            Vector4 expected = Vector4Helper::TransformVector3(v, m);
            Vector4 actual;

            actual = Vector4Helper::TransformVector3ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"vector4.Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);

            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            Quaternion q = QuaternionHelper::CreateFromRotationMatrix(m);

            Vector4 expected = Vector4Helper::Transform(v, m);
            Vector4 actual;

            actual = Vector4Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");

            // 
            v.W = 1.0f;
            expected.W = 1.0f;
            actual = Vector4Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest1)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q = QuaternionHelper::FromValues(0, 0, 0, 0);
            Vector4 expected = v;

            Vector4 actual = Vector4Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector4, Quaternion)
        TEST_METHOD(Vector4TransformVector4QuaternionTest2)
        {
            Vector4 v = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);
            Quaternion q = QuaternionHelper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 0.0f);

            Vector4 actual = Vector4Helper::TransformByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest)
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

            Vector4 expected = Vector4Helper::TransformVector3(v, m);
            Vector4 actual = Vector4Helper::TransformVector3ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest1)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Quaternion q = QuaternionHelper::FromValues(0, 0, 0, 0);
            Vector4 expected = Vector4Helper::FromVector3(v, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector3ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector3, Quaternion)
        TEST_METHOD(Vector4TransformVector3QuaternionTest2)
        {
            Vector3 v = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            Quaternion q = QuaternionHelper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector3ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuaternionTest)
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

            Vector4 expected = Vector4Helper::TransformVector2(v, m);
            Vector4 actual = Vector4Helper::TransformVector2ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Quaternion)
        TEST_METHOD(Vector4TransformVector2QuaternionTest1)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Quaternion q = QuaternionHelper::FromValues(0, 0, 0, 0);
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector2ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Transform (Vector2, Matrix4x4)
        TEST_METHOD(Vector4TransformVector2QuaternionTest2)
        {
            Vector2 v = Vector2Helper::FromValues(1.0f, 2.0f);
            Quaternion q = QuaternionHelper::Identity;
            Vector4 expected = Vector4Helper::FromValues(1.0f, 2.0f, 0, 1.0f);

            Vector4 actual = Vector4Helper::TransformVector2ByQuaternion(v, q);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Transform did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected = Vector4Helper::FromValues(
                0.1825741858350553711523232609336f,
                0.3651483716701107423046465218672f,
                0.5477225575051661134569697828008f,
                0.7302967433402214846092930437344f);
            Vector4 actual;

            actual = Vector4Helper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest1)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected = Vector4Helper::FromValues(1.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4Helper::Normalize(a);
            Assert::IsTrue(Equal(expected, actual), L"Vector4Helper::Normalize did not return the expected value.");
        }

        // A test for Normalize (Vector4)
        TEST_METHOD(Vector4NormalizeTest2)
        {
            Vector4 a = Vector4Helper::FromValues(0.0f, 0.0f, 0.0f, 0.0f);

            Vector4 expected = Vector4Helper::FromValues(0.0f, 0.0f, 0.0f, 0.0f);
            Vector4 actual = Vector4Helper::Normalize(a);
            Assert::IsTrue(isnan(actual.X) && isnan(actual.Y) && isnan(actual.Z) && isnan(actual.W), L"Vector4Helper::Normalize did not return the expected value.");
        }

        // A test for Vector4 (float, float, float, float)
        TEST_METHOD(Vector4ConstructorTest)
        {
            float x = 1.0f;
            float y = 2.0f;
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target = Vector4Helper::FromValues(x, y, z, w);

            Assert::IsTrue(Equal(target.X, x) && Equal(target.Y, y) && Equal(target.Z, z) && Equal(target.W, w),
                L"Vector4 constructor = Vector4Helper::FromValues(x,y,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector2, float, float)
        TEST_METHOD(Vector4ConstructorTest1)
        {
            Vector2 a = Vector2Helper::FromValues(1.0f, 2.0f);
            float z = 3.0f;
            float w = 4.0f;

            Vector4 target = Vector4Helper::FromVector2(a, z, w);
            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, z) && Equal(target.W, w),
                L"Vector4 constructor = Vector4Helper::FromValues(Vector2,z,w) did not return the expected value.");
        }

        // A test for Vector4 (Vector3, float)
        TEST_METHOD(Vector4ConstructorTest2)
        {
            Vector3 a = Vector3Helper::FromValues(1.0f, 2.0f, 3.0f);
            float w = 4.0f;

            Vector4 target = Vector4Helper::FromVector3(a, w);

            Assert::IsTrue(Equal(target.X, a.X) && Equal(target.Y, a.Y) && Equal(target.Z, a.Z) && Equal(target.W, w),
                L"Vector4 constructor = Vector4Helper::FromValues(Vector3,w) did not return the expected value.");
        }

        // A test for Vector4 ()
        TEST_METHOD(Vector4ConstructorTest5)
        {
            Vector4 target = Vector4Helper::FromValues(NAN, FLT_MAX, std::numeric_limits<float>::infinity(), FLT_EPSILON);

            Assert::IsTrue(isnan(target.X), L"Vector4Helper::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_MAX == target.Y, L"Vector4Helper::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(IsPositiveInfinity(target.Z), L"Vector4Helper::constructor (float, float, float, float) did not return the expected value.");
            Assert::IsTrue(FLT_EPSILON == target.W, L"Vector4Helper::constructor (float, float, float, float) did not return the expected value.");
        }

        // A test for Add (Vector4, Vector4)
        TEST_METHOD(Vector4AddTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = Vector4Helper::FromValues(6.0f, 8.0f, 10.0f, 12.0f);
            Vector4 actual;

            actual = Vector4Helper::Add(a, b);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Add did not return the expected value.");
        }

        // A test for Divide (Vector4, float)
        TEST_METHOD(Vector4DivideTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            float div = 2.0f;
            Vector4 expected = Vector4Helper::FromValues(0.5f, 1.0f, 1.5f, 2.0f);
            Vector4 actual;
            actual = Vector4Helper::DivideByScalar(a, div);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Divide did not return the expected value.");
        }

        // A test for Divide (Vector4, Vector4)
        TEST_METHOD(Vector4DivideTest1)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 6.0f, 7.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 2.0f, 3.0f, 8.0f);

            Vector4 expected = Vector4Helper::FromValues(1.0f / 5.0f, 6.0f / 2.0f, 7.0f / 3.0f, 4.0f / 8.0f);
            Vector4 actual;

            actual = Vector4Helper::Divide(a, b);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Divide did not return the expected value.");
        }

        // A test for Multiply (Vector4, float)
        TEST_METHOD(Vector4MultiplyTest2)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            float factor = 2.0f;
            Vector4 expected = Vector4Helper::FromValues(2.0f, 4.0f, 6.0f, 8.0f);
            Vector4 actual = Vector4Helper::MultiplyByScalar(a, factor);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Multiply did not return the expected value.");
        }

        // A test for Multiply (Vector4, Vector4)
        TEST_METHOD(Vector4MultiplyTest3)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 6.0f, 7.0f, 8.0f);

            Vector4 expected = Vector4Helper::FromValues(5.0f, 12.0f, 21.0f, 32.0f);
            Vector4 actual;

            actual = Vector4Helper::Multiply(a, b);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Multiply did not return the expected value.");
        }

        // A test for Negate (Vector4)
        TEST_METHOD(Vector4NegateTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);

            Vector4 expected = Vector4Helper::FromValues(-1.0f, -2.0f, -3.0f, -4.0f);
            Vector4 actual;

            actual = Vector4Helper::Negate(a);
            Assert::AreEqual(expected, actual, L"Vector4Helper::Negate did not return the expected value.");
        }

        // A test for Subtract (Vector4, Vector4)
        TEST_METHOD(Vector4SubtractTest)
        {
            Vector4 a = Vector4Helper::FromValues(1.0f, 6.0f, 3.0f, 4.0f);
            Vector4 b = Vector4Helper::FromValues(5.0f, 2.0f, 3.0f, 9.0f);

            Vector4 expected = Vector4Helper::FromValues(-4.0f, 4.0f, 0.0f, -5.0f);
            Vector4 actual;

            actual = Vector4Helper::Subtract(a, b);

            Assert::AreEqual(expected, actual, L"Vector4Helper::Subtract did not return the expected value.");
        }

        // A test for UnitW
        TEST_METHOD(Vector4UnitWTest)
        {
            Vector4 val = Vector4Helper::FromValues(0.0f, 0.0f, 0.0f, 1.0f);
            Assert::AreEqual(val, Vector4Helper::UnitW, L"Vector4Helper::UnitW was not set correctly.");
        }

        // A test for UnitX
        TEST_METHOD(Vector4UnitXTest)
        {
            Vector4 val = Vector4Helper::FromValues(1.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4Helper::UnitX, L"Vector4Helper::UnitX was not set correctly.");
        }

        // A test for UnitY
        TEST_METHOD(Vector4UnitYTest)
        {
            Vector4 val = Vector4Helper::FromValues(0.0f, 1.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4Helper::UnitY, L"Vector4Helper::UnitY was not set correctly.");
        }

        // A test for UnitZ
        TEST_METHOD(Vector4UnitZTest)
        {
            Vector4 val = Vector4Helper::FromValues(0.0f, 0.0f, 1.0f, 0.0f);
            Assert::AreEqual(val, Vector4Helper::UnitZ, L"Vector4Helper::UnitZ was not set correctly.");
        }

        // A test for One
        TEST_METHOD(Vector4OneTest)
        {
            Vector4 val = Vector4Helper::FromValues(1.0f, 1.0f, 1.0f, 1.0f);
            Assert::AreEqual(val, Vector4Helper::One, L"Vector4Helper::One was not set correctly.");
        }

        // A test for Zero
        TEST_METHOD(Vector4ZeroTest)
        {
            Vector4 val = Vector4Helper::FromValues(0.0f, 0.0f, 0.0f, 0.0f);
            Assert::AreEqual(val, Vector4Helper::Zero, L"Vector4Helper::Zero was not set correctly.");
        }

        // A test for Vector4 (float)
        TEST_METHOD(Vector4ConstructorTest6)
        {
            float value = 1.0f;
            Vector4 target = Vector4Helper::FromValue(value);

            Vector4 expected = Vector4Helper::FromValues(value, value, value, value);
            Assert::AreEqual(expected, target, L"Vector4Helper::cstr did not return the expected value.");

            value = 2.0f;
            target = Vector4Helper::FromValue(value);
            expected = Vector4Helper::FromValues(value, value, value, value);
            Assert::AreEqual(expected, target, L"Vector4Helper::cstr did not return the expected value.");
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
            Assert::AreEqual(size_t(16), sizeof(Vector4));
            Assert::AreEqual(size_t(32), sizeof(Vector4_2x));
            Assert::AreEqual(size_t(20), sizeof(Vector4PlusFloat));
            Assert::AreEqual(size_t(40), sizeof(Vector4PlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Vector4FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Vector4, X));
            Assert::AreEqual(size_t(4), offsetof(Vector4, Y));
            Assert::AreEqual(size_t(8), offsetof(Vector4, Z));
            Assert::AreEqual(size_t(12), offsetof(Vector4, W));
        }
    };
}
