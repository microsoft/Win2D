//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::DirectX::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(Matrix3x2Test)
    {
    public:
        static Matrix3x2 GenerateMatrixNumberFrom1To6()
        {
            Matrix3x2 a;
            a.M11 = 1.0f; a.M12 = 2.0f;
            a.M21 = 3.0f; a.M22 = 4.0f;
            a.M31 = 5.0f; a.M32 = 6.0f;
            return a;
        }

        static Matrix3x2 GenerateTestMatrix()
        {
            Matrix3x2 m = Matrix3x2Helper::CreateRotation(ToRadians(30.0f));
            m.M31 = 111.0f;
            m.M32 = 222.0f;
            return m;
        }

        // A test for Matrix3x2 (float, float, float, float, float, float)
        TEST_METHOD(Matrix3x2ConstructorTest)
        {
            Matrix3x2 val = Matrix3x2Helper::FromValues(1, 2, 3, 4, 5, 6);

            Assert::AreEqual(val.M11, 1.0f);
            Assert::AreEqual(val.M12, 2.0f);
            Assert::AreEqual(val.M21, 3.0f);
            Assert::AreEqual(val.M22, 4.0f);
            Assert::AreEqual(val.M31, 5.0f);
            Assert::AreEqual(val.M32, 6.0f);
        }

        // A test for Identity
        TEST_METHOD(Matrix3x2IdentityTest)
        {
            Matrix3x2 val = Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0);

            Assert::IsTrue(Equal(val, Matrix3x2Helper::Identity), L"Matrix3x2Helper::Indentity was not set correctly.");
        }

        // A test for Determinant
        TEST_METHOD(Matrix3x2DeterminantTest)
        {
            Matrix3x2 target = Matrix3x2Helper::CreateRotation(ToRadians(30.0f));

            float val = 1.0f;
            float det = Matrix3x2Helper::Determinant(target);

            Assert::IsTrue(Equal(val, det), L"Matrix3x2Helper::Determinant was not set correctly.");
        }

        // A test for Determinant
        TEST_METHOD(Matrix3x2DeterminantTest1)
        {
            Matrix3x2 a;
            a.M11 = 5.0f; a.M12 = 2.0f;
            a.M21 = 12.0f; a.M22 = 6.8f;
            a.M31 = 6.5f; a.M32 = 1.0f;
            Matrix3x2 i;
            Assert::IsTrue(Matrix3x2Helper::Invert(a, &i));

            float detA = Matrix3x2Helper::Determinant(a);
            float detI = Matrix3x2Helper::Determinant(i);
            float t = 1.0f / detI;

            // only accurate to 3 precision
            Assert::IsTrue(fabs(detA - t) < 1e-3, L"Matrix3x2Helper::Determinant was not set correctly.");

            // sanity check against 4x4 version
            Assert::AreEqual(Matrix4x4Helper::Determinant(Matrix4x4Helper::FromMatrix3x2(a)), detA);
            Assert::AreEqual(Matrix4x4Helper::Determinant(Matrix4x4Helper::FromMatrix3x2(i)), detI);
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::CreateRotation(ToRadians(30.0f));

            Matrix3x2 expected;
            expected.M11 = 0.8660254f;
            expected.M12 = -0.5f;

            expected.M21 = 0.5f;
            expected.M22 = 0.8660254f;

            expected.M31 = 0;
            expected.M32 = 0;

            Matrix3x2 actual;

            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2Helper::Invert did not return the expected value.");

            Matrix3x2 i = Matrix3x2Helper::Multiply(mtx, actual);
            Assert::IsTrue(Equal(i, Matrix3x2Helper::Identity), L"Matrix3x2Helper::Invert did not return the expected value.");
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertInPlaceTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::Multiply(
                                Matrix3x2Helper::Multiply(
                                    Matrix3x2Helper::CreateRotation(ToRadians(30.0f)),
                                    Matrix3x2Helper::CreateScale(2, 3)
                                ),
                                Matrix3x2Helper::CreateTranslation(23, 42)
                            );

            Matrix3x2 expected;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &expected));

            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &mtx));
            Assert::IsTrue(Equal(expected, mtx), L"Matrix3x2Helper::Invert did not return the expected value.");
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertIdentityTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::Identity;

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));

            Assert::IsTrue(Equal(actual, Matrix3x2Helper::Identity));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTranslationTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::CreateTranslation(23, 42);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));

            Matrix3x2 i = Matrix3x2Helper::Multiply(mtx, actual);
            Assert::IsTrue(Equal(i, Matrix3x2Helper::Identity));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertRotationTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::CreateRotation(2);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));

            Matrix3x2 i = Matrix3x2Helper::Multiply(mtx, actual);
            Assert::IsTrue(Equal(i, Matrix3x2Helper::Identity));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertScaleTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::CreateScale(23, -42);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));

            Matrix3x2 i = Matrix3x2Helper::Multiply(mtx, actual);
            Assert::IsTrue(Equal(i, Matrix3x2Helper::Identity));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertAffineTest)
        {
            Matrix3x2 mtx = Matrix3x2Helper::Multiply(
                                Matrix3x2Helper::Multiply(
                                    Matrix3x2Helper::CreateRotation(2),
                                    Matrix3x2Helper::CreateScale(23, -42)
                                ),
                                Matrix3x2Helper::CreateTranslation(17, 53)
                            );

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2Helper::Invert(mtx, &actual));

            Matrix3x2 i = Matrix3x2Helper::Multiply(mtx, actual);
            Assert::IsTrue(Equal(i, Matrix3x2Helper::Identity));
        }

        // A test for CreateRotation (float)
        TEST_METHOD(Matrix3x2CreateRotationTest)
        {
            float radians = ToRadians(50.0f);

            Matrix3x2 expected = Matrix3x2Helper::Identity;
            expected.M11 = 0.642787635f;
            expected.M12 = 0.766044438f;
            expected.M21 = -0.766044438f;
            expected.M22 = 0.642787635f;

            Matrix3x2 actual;
            actual = Matrix3x2Helper::CreateRotation(radians);
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2Helper::CreateRotation did not return the expected value.");
        }

        // A test for CreateRotation (float, Vector2)
        TEST_METHOD(Matrix3x2CreateRotationCenterTest)
        {
            float radians = ToRadians(30.0f);
            Vector2 center = Vector2Helper::FromValues(23, 42);

            Matrix3x2 rotateAroundZero = Matrix3x2Helper::CreateRotation(radians, Vector2Helper::Zero);
            Matrix3x2 rotateAroundZeroExpected = Matrix3x2Helper::CreateRotation(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            Matrix3x2 rotateAroundCenter = Matrix3x2Helper::CreateRotation(radians, center);
            Matrix3x2 rotateAroundCenterExpected = Matrix3x2Helper::Multiply(Matrix3x2Helper::Multiply(Matrix3x2Helper::CreateTranslation(Vector2Helper::Negate(center)), Matrix3x2Helper::CreateRotation(radians)), Matrix3x2Helper::CreateTranslation(center));
            Assert::IsTrue(Equal(rotateAroundCenter, rotateAroundCenterExpected));
        }

        // A test for CreateRotation (float)
        TEST_METHOD(Matrix3x2CreateRotationRightAngleTest)
        {
            // 90 degree rotations must be exact!
            Matrix3x2 actual = Matrix3x2Helper::CreateRotation(0);
            Assert::AreEqual(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI / 2);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, 1, -1, 0, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI);
            Assert::AreEqual(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 3 / 2);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, -1, 1, 0, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 2);
            Assert::AreEqual(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 5 / 2);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, 1, -1, 0, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(-DirectX::XM_PI / 2);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, -1, 1, 0, 0, 0), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI + delta);
            Assert::IsFalse(Equal(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 0, 0), actual));

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI - delta);
            Assert::IsFalse(Equal(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 0, 0), actual));
        }

        // A test for CreateRotation (float, Vector2)
        TEST_METHOD(Matrix3x2CreateRotationRightAngleCenterTest)
        {
            Vector2 center = Vector2Helper::FromValues(3, 7);

            // 90 degree rotations must be exact!
            Matrix3x2 actual = Matrix3x2Helper::CreateRotation(0, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI / 2, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, 1, -1, 0, 10, 4), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 6, 14), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 3 / 2, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, -1, 1, 0, -4, 10), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 2, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI * 5 / 2, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, 1, -1, 0, 10, 4), actual);

            actual = Matrix3x2Helper::CreateRotation(-DirectX::XM_PI / 2, center);
            Assert::AreEqual(Matrix3x2Helper::FromValues(0, -1, 1, 0, -4, 10), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI + delta, center);
            Assert::IsFalse(Equal(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 6, 14), actual));

            actual = Matrix3x2Helper::CreateRotation(DirectX::XM_PI - delta, center);
            Assert::IsFalse(Equal(Matrix3x2Helper::FromValues(-1, 0, 0, -1, 6, 14), actual));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTest1)
        {
            Matrix3x2 a;
            a.M11 = 0.0f; a.M12 = 2.0f;
            a.M21 = 0.0f; a.M22 = 4.0f;
            a.M31 = 5.0f; a.M32 = 6.0f;

            float detA = Matrix3x2Helper::Determinant(a);
            Assert::IsTrue(Equal(detA, 0.0f), L"Matrix3x2Helper::Invert did not return the expected value.");

            Matrix3x2 actual;
            Assert::IsFalse(Matrix3x2Helper::Invert(a, &actual));

            // all the elements in Actual is NaN
            Assert::IsTrue(
                isnan(actual.M11) && isnan(actual.M12) &&
                isnan(actual.M21) && isnan(actual.M22) &&
                isnan(actual.M31) && isnan(actual.M32)
                , L"Matrix3x2Helper::Invert did not return the expected value.");
        }

        // A test for Lerp (Matrix3x2, Matrix3x2, float)
        TEST_METHOD(Matrix3x2LerpTest)
        {
            Matrix3x2 a;
            a.M11 = 11.0f; a.M12 = 12.0f;
            a.M21 = 21.0f; a.M22 = 22.0f;
            a.M31 = 31.0f; a.M32 = 32.0f;

            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            float t = 0.5f;

            Matrix3x2 expected;
            expected.M11 = a.M11 + (b.M11 - a.M11) * t;
            expected.M12 = a.M12 + (b.M12 - a.M12) * t;

            expected.M21 = a.M21 + (b.M21 - a.M21) * t;
            expected.M22 = a.M22 + (b.M22 - a.M22) * t;

            expected.M31 = a.M31 + (b.M31 - a.M31) * t;
            expected.M32 = a.M32 + (b.M32 - a.M32) * t;

            Matrix3x2 actual;
            actual = Matrix3x2Helper::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2Helper::Lerp did not return the expected value.");
        }

        // A test for Add (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2AddTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            Matrix3x2 expected;
            expected.M11 = a.M11 + b.M11; expected.M12 = a.M12 + b.M12;
            expected.M21 = a.M21 + b.M21; expected.M22 = a.M22 + b.M22;
            expected.M31 = a.M31 + b.M31; expected.M32 = a.M32 + b.M32;

            Matrix3x2 actual;

            actual = Matrix3x2Helper::Add(a, b);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::Add did not return the expected value.");
        }

        // A test for Multiply (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2MultiplyTest3)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            Matrix3x2 expected;
            expected.M11 = a.M11 * b.M11 + a.M12 * b.M21;
            expected.M12 = a.M11 * b.M12 + a.M12 * b.M22;

            expected.M21 = a.M21 * b.M11 + a.M22 * b.M21;
            expected.M22 = a.M21 * b.M12 + a.M22 * b.M22;

            expected.M31 = a.M31 * b.M11 + a.M32 * b.M21 + b.M31;
            expected.M32 = a.M31 * b.M12 + a.M32 * b.M22 + b.M32;
            Matrix3x2 actual;
            actual = Matrix3x2Helper::Multiply(a, b);

            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::Multiply did not return the expected value.");

            // Sanity check by comparison with 4x4 multiply.
            a = Matrix3x2Helper::Multiply(Matrix3x2Helper::CreateRotation(ToRadians(30)), Matrix3x2Helper::CreateTranslation(23, 42));
            b = Matrix3x2Helper::Multiply(Matrix3x2Helper::CreateScale(3, 7), Matrix3x2Helper::CreateTranslation(666, -1));

            actual = Matrix3x2Helper::Multiply(a, b);

            Matrix4x4 a44 = Matrix4x4Helper::FromMatrix3x2(a);
            Matrix4x4 b44 = Matrix4x4Helper::FromMatrix3x2(b);
            Matrix4x4 expected44 = Matrix4x4Helper::Multiply(a44, b44);
            Matrix4x4 actual44 = Matrix4x4Helper::FromMatrix3x2(actual);

            Assert::IsTrue(Equal(expected44, actual44), L"Matrix3x2Helper::Multiply did not return the expected value.");
        }

        // A test for Multiply (Matrix3x2, float)
        TEST_METHOD(Matrix3x2MultiplyTest5)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected = Matrix3x2Helper::FromValues(3, 6, 9, 12, 15, 18);
            Matrix3x2 actual = Matrix3x2Helper::MultiplyByScalar(a, 3);

            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::Multiply did not return the expected value.");
        }

        // A test for Negate (Matrix3x2)
        TEST_METHOD(Matrix3x2NegateTest)
        {
            Matrix3x2 m = GenerateMatrixNumberFrom1To6();

            Matrix3x2 expected;
            expected.M11 = -1.0f; expected.M12 = -2.0f;
            expected.M21 = -3.0f; expected.M22 = -4.0f;
            expected.M31 = -5.0f; expected.M32 = -6.0f;
            Matrix3x2 actual;

            actual = Matrix3x2Helper::Negate(m);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::Negate did not return the expected value.");
        }

        // A test for Subtract (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2SubtractTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected = Matrix3x2Helper::FromValues(0, 0, 0, 0, 0, 0);
            Matrix3x2 actual;

            actual = Matrix3x2Helper::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::Subtract did not return the expected value.");
        }

        // A test for CreateScale (float)
        TEST_METHOD(Matrix3x2CreateScaleTest2)
        {
            float scale = 2.0f;
            Matrix3x2 expected = Matrix3x2Helper::FromValues(
                2.0f, 0.0f,
                0.0f, 2.0f,
                0.0f, 0.0f);
            Matrix3x2 actual = Matrix3x2Helper::CreateScale(scale);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::CreateScale did not return the expected value.");
        }

        // A test for CreateScale (float, float)
        TEST_METHOD(Matrix3x2CreateScaleTest3)
        {
            float xScale = 2.0f;
            float yScale = 3.0f;
            Matrix3x2 expected = Matrix3x2Helper::FromValues(
                2.0f, 0.0f,
                0.0f, 3.0f,
                0.0f, 0.0f);
            Matrix3x2 actual = Matrix3x2Helper::CreateScale(xScale, yScale);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::CreateScale did not return the expected value.");
        }

        // A test for CreateScale (float, float, Vector2)
        TEST_METHOD(Matrix3x2CreateScaleCenterTest3)
        {
            Vector2 scale = Vector2Helper::FromValues(3, 4);
            Vector2 center = Vector2Helper::FromValues(23, 42);

            Matrix3x2 scaleAroundZero = Matrix3x2Helper::CreateScale(scale.X, scale.Y, Vector2Helper::Zero);
            Matrix3x2 scaleAroundZeroExpected = Matrix3x2Helper::CreateScale(scale.X, scale.Y);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            Matrix3x2 scaleAroundCenter = Matrix3x2Helper::CreateScale(scale.X, scale.Y, center);
            Matrix3x2 scaleAroundCenterExpected = Matrix3x2Helper::Multiply(Matrix3x2Helper::Multiply(Matrix3x2Helper::CreateTranslation(Vector2Helper::Negate(center)), Matrix3x2Helper::CreateScale(scale.X, scale.Y)), Matrix3x2Helper::CreateTranslation(center));
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for CreateTranslation (Vector2)
        TEST_METHOD(Matrix3x2CreateTranslationTest1)
        {
            Vector2 position = Vector2Helper::FromValues(2.0f, 3.0f);
            Matrix3x2 expected = Matrix3x2Helper::FromValues(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            Matrix3x2 actual = Matrix3x2Helper::CreateTranslation(position);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::CreateTranslation did not return the expected value.");
        }

        // A test for CreateTranslation (float, float)
        TEST_METHOD(Matrix3x2CreateTranslationTest2)
        {
            float xPosition = 2.0f;
            float yPosition = 3.0f;

            Matrix3x2 expected = Matrix3x2Helper::FromValues(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            Matrix3x2 actual = Matrix3x2Helper::CreateTranslation(xPosition, yPosition);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::CreateTranslation did not return the expected value.");
        }

        // A test for Translation
        TEST_METHOD(Matrix3x2TranslationTest)
        {
            Matrix3x2 a = GenerateTestMatrix();
            Matrix3x2 b = a;

            // Transfomed vector that has same semantics of property must be same.
            Vector2 val = Vector2Helper::FromValues(a.M31, a.M32);
            Assert::AreEqual(val, Matrix3x2Helper::Translation(a), L"Matrix3x2Helper::Translation was not set correctly.");
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewIdentityTest)
        {
            Matrix3x2 expected = Matrix3x2Helper::Identity;
            Matrix3x2 actual = Matrix3x2Helper::CreateSkew(0, 0);
            Assert::AreEqual(expected, actual, L"Matrix3x2Helper::CreateSkew did not return the expected value.");
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewXTest)
        {
            Matrix3x2 expected = Matrix3x2Helper::FromValues(1, 0, -0.414213562373095f, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2Helper::CreateSkew(-DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            expected = Matrix3x2Helper::FromValues(1, 0, 0.414213562373095f, 1, 0, 0);
            actual = Matrix3x2Helper::CreateSkew(DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(0, 0), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, 1), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(0.414213568f, 1), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, -1), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(-0.414213568f, -1), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(3, 10), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(7.14213568f, 10), result));
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewYTest)
        {
            Matrix3x2 expected = Matrix3x2Helper::FromValues(1, -0.414213562373095f, 0, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2Helper::CreateSkew(0, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            expected = Matrix3x2Helper::FromValues(1, 0.414213562373095f, 0, 1, 0, 0);
            actual = Matrix3x2Helper::CreateSkew(0, DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(0, 0), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(1, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(1, 0.414213568f), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(-1, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(-1, -0.414213568f), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(10, 3), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(10, 7.14213568f), result));
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewXYTest)
        {
            Matrix3x2 expected = Matrix3x2Helper::FromValues(1, -0.414213562373095f, 1, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2Helper::CreateSkew(DirectX::XM_PI / 4, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(0, 0), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(1, 0), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(1, -0.414213562373095f), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(0, 1), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(1, 1), result));

            result = Vector2Helper::TransformByMatrix3x2(Vector2Helper::FromValues(1, 1), actual);
            Assert::IsTrue(Equal(Vector2Helper::FromValues(2, 0.585786437626905f), result));
        }

        // A test for CreateSkew (float, float, Vector2)
        TEST_METHOD(Matrix3x2CreateSkewCenterTest)
        {
            float skewX = 1, skewY = 2;
            Vector2 center = Vector2Helper::FromValues(23, 42);

            Matrix3x2 skewAroundZero = Matrix3x2Helper::CreateSkew(skewX, skewY, Vector2Helper::Zero);
            Matrix3x2 skewAroundZeroExpected = Matrix3x2Helper::CreateSkew(skewX, skewY);
            Assert::IsTrue(Equal(skewAroundZero, skewAroundZeroExpected));

            Matrix3x2 skewAroundCenter = Matrix3x2Helper::CreateSkew(skewX, skewY, center);
            Matrix3x2 skewAroundCenterExpected = Matrix3x2Helper::Multiply(Matrix3x2Helper::Multiply(Matrix3x2Helper::CreateTranslation(Vector2Helper::Negate(center)), Matrix3x2Helper::CreateSkew(skewX, skewY)), Matrix3x2Helper::CreateTranslation(center));
            Assert::IsTrue(Equal(skewAroundCenter, skewAroundCenterExpected));
        }

        // A test for IsIdentity()
        TEST_METHOD(Matrix3x2IsIdentityTest)
        {
            Assert::IsTrue(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::Identity));
            Assert::IsTrue(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(0, 0, 0, 1, 0, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 1, 0, 1, 0, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 0, 1, 1, 0, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 0, 0, 0, 0, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 0, 0, 1, 1, 0)));
            Assert::IsFalse(Matrix3x2Helper::IsIdentity(Matrix3x2Helper::FromValues(1, 0, 0, 1, 0, 1)));
        }

        struct Matrix3x2_2x
        {
            Matrix3x2 a;
            Matrix3x2 b;
        };

        struct Matrix3x2PlusFloat
        {
            Matrix3x2 v;
            float f;
        };

        struct Matrix3x2PlusFloat_2x
        {
            Matrix3x2PlusFloat a;
            Matrix3x2PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Matrix3x2SizeofTest)
        {
            Assert::AreEqual(size_t(24), sizeof(Matrix3x2));
            Assert::AreEqual(size_t(48), sizeof(Matrix3x2_2x));
            Assert::AreEqual(size_t(28), sizeof(Matrix3x2PlusFloat));
            Assert::AreEqual(size_t(56), sizeof(Matrix3x2PlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Matrix3x2FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Matrix3x2, M11));
            Assert::AreEqual(size_t(4), offsetof(Matrix3x2, M12));

            Assert::AreEqual(size_t(8), offsetof(Matrix3x2, M21));
            Assert::AreEqual(size_t(12), offsetof(Matrix3x2, M22));

            Assert::AreEqual(size_t(16), offsetof(Matrix3x2, M31));
            Assert::AreEqual(size_t(20), offsetof(Matrix3x2, M32));
        }
    };
}
