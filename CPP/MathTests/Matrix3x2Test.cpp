//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
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
            Matrix3x2 m = Matrix3x2::CreateRotation(ToRadians(30.0f));
            m.SetTranslation(Vector2(111.0f, 222.0f));
            return m;
        }

        // A test for Matrix3x2 (float, float, float, float, float, float)
        TEST_METHOD(Matrix3x2ConstructorTest)
        {
            Matrix3x2 val(1, 2, 3, 4, 5, 6);

            Assert::AreEqual(val.M11, 1.0f);
            Assert::AreEqual(val.M12, 2.0f);
            Assert::AreEqual(val.M21, 3.0f);
            Assert::AreEqual(val.M22, 4.0f);
            Assert::AreEqual(val.M31, 5.0f);
            Assert::AreEqual(val.M32, 6.0f);
        }

        // A test for Matrix3x2 ()
        TEST_METHOD(Matrix3x2ConstructorTest2)
        {
            Matrix3x2 val;

            Assert::AreEqual(val.M11, 0.0f);
            Assert::AreEqual(val.M12, 0.0f);
            Assert::AreEqual(val.M21, 0.0f);
            Assert::AreEqual(val.M22, 0.0f);
            Assert::AreEqual(val.M31, 0.0f);
            Assert::AreEqual(val.M32, 0.0f);
        }

        // A test for Identity
        TEST_METHOD(Matrix3x2IdentityTest)
        {
            Matrix3x2 val;
            val.M11 = val.M22 = 1.0f;

            Assert::IsTrue(Equal(val, Matrix3x2::Identity()), L"Matrix3x2::Indentity was not set correctly.");
        }

        // A test for Determinant
        TEST_METHOD(Matrix3x2DeterminantTest)
        {
            Matrix3x2 target = Matrix3x2::CreateRotation(ToRadians(30.0f));

            float val = 1.0f;
            float det = target.Determinant();

            Assert::IsTrue(Equal(val, det), L"Matrix3x2::Determinant was not set correctly.");
        }

        // A test for Determinant
        TEST_METHOD(Matrix3x2DeterminantTest1)
        {
            Matrix3x2 a;
            a.M11 = 5.0f; a.M12 = 2.0f;
            a.M21 = 12.0f; a.M22 = 6.8f;
            a.M31 = 6.5f; a.M32 = 1.0f;
            Matrix3x2 i;
            Assert::IsTrue(Matrix3x2::Invert(a, &i));

            float detA = a.Determinant();
            float detI = i.Determinant();
            float t = 1.0f / detI;

            // only accurate to 3 precision
            Assert::IsTrue(fabs(detA - t) < 1e-3, L"Matrix3x2::Determinant was not set correctly.");

            // sanity check against 4x4 version
            Assert::AreEqual(Matrix4x4(a).Determinant(), detA);
            Assert::AreEqual(Matrix4x4(i).Determinant(), detI);
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateRotation(ToRadians(30.0f));

            Matrix3x2 expected;
            expected.M11 = 0.8660254f;
            expected.M12 = -0.5f;

            expected.M21 = 0.5f;
            expected.M22 = 0.8660254f;

            expected.M31 = 0;
            expected.M32 = 0;

            Matrix3x2 actual;

            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::Invert did not return the expected value.");

            Matrix3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, Matrix3x2::Identity()), L"Matrix3x2::Invert did not return the expected value.");
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertInPlaceTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateRotation(ToRadians(30.0f)) * 
                            Matrix3x2::CreateScale(2, 3) * 
                            Matrix3x2::CreateTranslation(23, 42);

            Matrix3x2 expected;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &expected));

            Assert::IsTrue(Matrix3x2::Invert(mtx, &mtx));
            Assert::IsTrue(Equal(expected, mtx), L"Matrix3x2::Invert did not return the expected value.");
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertIdentityTest)
        {
            Matrix3x2 mtx = Matrix3x2::Identity();

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));

            Assert::IsTrue(Equal(actual, Matrix3x2::Identity()));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTranslationTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateTranslation(23, 42);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));

            Matrix3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, Matrix3x2::Identity()));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertRotationTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateRotation(2);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));

            Matrix3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, Matrix3x2::Identity()));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertScaleTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateScale(23, -42);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));

            Matrix3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, Matrix3x2::Identity()));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertAffineTest)
        {
            Matrix3x2 mtx = Matrix3x2::CreateRotation(2) *
                            Matrix3x2::CreateScale(23, -42) *
                            Matrix3x2::CreateTranslation(17, 53);

            Matrix3x2 actual;
            Assert::IsTrue(Matrix3x2::Invert(mtx, &actual));

            Matrix3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, Matrix3x2::Identity()));
        }

        // A test for CreateRotation (float)
        TEST_METHOD(Matrix3x2CreateRotationTest)
        {
            float radians = ToRadians(50.0f);

            Matrix3x2 expected;
            expected.M11 = 0.642787635f;
            expected.M12 = 0.766044438f;
            expected.M21 = -0.766044438f;
            expected.M22 = 0.642787635f;

            Matrix3x2 actual;
            actual = Matrix3x2::CreateRotation(radians);
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::CreateRotation did not return the expected value.");
        }

        // A test for CreateRotation (float, Vector2)
        TEST_METHOD(Matrix3x2CreateRotationCenterTest)
        {
            float radians = ToRadians(30.0f);
            Vector2 center(23, 42);

            Matrix3x2 rotateAroundZero = Matrix3x2::CreateRotation(radians, Vector2::Zero());
            Matrix3x2 rotateAroundZeroExpected = Matrix3x2::CreateRotation(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            Matrix3x2 rotateAroundCenter = Matrix3x2::CreateRotation(radians, center);
            Matrix3x2 rotateAroundCenterExpected = Matrix3x2::CreateTranslation(-center) * Matrix3x2::CreateRotation(radians) * Matrix3x2::CreateTranslation(center);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));
        }

        // A test for CreateRotation (float)
        TEST_METHOD(Matrix3x2CreateRotationRightAngleTest)
        {
            // 90 degree rotations must be exact!
            Matrix3x2 actual = Matrix3x2::CreateRotation(0);
            Assert::AreEqual(Matrix3x2(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI / 2);
            Assert::AreEqual(Matrix3x2(0, 1, -1, 0, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI);
            Assert::AreEqual(Matrix3x2(-1, 0, 0, -1, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 3 / 2);
            Assert::AreEqual(Matrix3x2(0, -1, 1, 0, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 2);
            Assert::AreEqual(Matrix3x2(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 5 / 2);
            Assert::AreEqual(Matrix3x2(0, 1, -1, 0, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(-DirectX::XM_PI / 2);
            Assert::AreEqual(Matrix3x2(0, -1, 1, 0, 0, 0), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI + delta);
            Assert::IsFalse(Equal(Matrix3x2(-1, 0, 0, -1, 0, 0), actual));

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI - delta);
            Assert::IsFalse(Equal(Matrix3x2(-1, 0, 0, -1, 0, 0), actual));
        }

        // A test for CreateRotation (float, Vector2)
        TEST_METHOD(Matrix3x2CreateRotationRightAngleCenterTest)
        {
            Vector2 center(3, 7);

            // 90 degree rotations must be exact!
            Matrix3x2 actual = Matrix3x2::CreateRotation(0, center);
            Assert::AreEqual(Matrix3x2(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI / 2, center);
            Assert::AreEqual(Matrix3x2(0, 1, -1, 0, 10, 4), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI, center);
            Assert::AreEqual(Matrix3x2(-1, 0, 0, -1, 6, 14), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 3 / 2, center);
            Assert::AreEqual(Matrix3x2(0, -1, 1, 0, -4, 10), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 2, center);
            Assert::AreEqual(Matrix3x2(1, 0, 0, 1, 0, 0), actual);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI * 5 / 2, center);
            Assert::AreEqual(Matrix3x2(0, 1, -1, 0, 10, 4), actual);

            actual = Matrix3x2::CreateRotation(-DirectX::XM_PI / 2, center);
            Assert::AreEqual(Matrix3x2(0, -1, 1, 0, -4, 10), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI + delta, center);
            Assert::IsFalse(Equal(Matrix3x2(-1, 0, 0, -1, 6, 14), actual));

            actual = Matrix3x2::CreateRotation(DirectX::XM_PI - delta, center);
            Assert::IsFalse(Equal(Matrix3x2(-1, 0, 0, -1, 6, 14), actual));
        }

        // A test for Invert (Matrix3x2)
        TEST_METHOD(Matrix3x2InvertTest1)
        {
            Matrix3x2 a;
            a.M11 = 0.0f; a.M12 = 2.0f;
            a.M21 = 0.0f; a.M22 = 4.0f;
            a.M31 = 5.0f; a.M32 = 6.0f;

            float detA = a.Determinant();
            Assert::IsTrue(Equal(detA, 0.0f), L"Matrix3x2::Invert did not return the expected value.");

            Matrix3x2 actual;
            Assert::IsFalse(Matrix3x2::Invert(a, &actual));

            // all the elements in Actual is NaN
            Assert::IsTrue(
                isnan(actual.M11) && isnan(actual.M12) &&
                isnan(actual.M21) && isnan(actual.M22) &&
                isnan(actual.M31) && isnan(actual.M32)
                , L"Matrix3x2::Invert did not return the expected value.");
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
            actual = Matrix3x2::Lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::Lerp did not return the expected value.");
        }

        // A test for operator - (Matrix3x2)
        TEST_METHOD(Matrix3x2UnaryNegationTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();

            Matrix3x2 expected;
            expected.M11 = -1.0f; expected.M12 = -2.0f;
            expected.M21 = -3.0f; expected.M22 = -4.0f;
            expected.M31 = -5.0f; expected.M32 = -6.0f;

            Matrix3x2 actual = -a;
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::operator - did not return the expected value.");
        }

        // A test for operator - (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2SubtractionTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected;

            Matrix3x2 actual = a - b;
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::operator - did not return the expected value.");
        }

        // A test for operator * (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2MultiplyTest1)
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

            Matrix3x2 actual = a * b;
            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::operator * did not return the expected value.");

            // Sanity check by comparison with 4x4 multiply.
            a = Matrix3x2::CreateRotation(ToRadians(30)) * Matrix3x2::CreateTranslation(23, 42);
            b = Matrix3x2::CreateScale(3, 7) * Matrix3x2::CreateTranslation(666, -1);

            actual = a * b;

            Matrix4x4 a44(a);
            Matrix4x4 b44(b);
            Matrix4x4 expected44 = a44 * b44;
            Matrix4x4 actual44(actual);

            Assert::IsTrue(Equal(expected44, actual44), L"Matrix3x2::operator * did not return the expected value.");
        }

        // A test for operator * (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2MultiplyTest4)
        {
            Matrix3x2 a;
            a.M11 = 1.0f; a.M12 = 2.0f;
            a.M21 = 5.0f; a.M22 = -6.0f;
            a.M31 = 9.0f; a.M32 = 10.0f;

            Matrix3x2 b;
            b = Matrix3x2::Identity();

            Matrix3x2 expected = a;
            Matrix3x2 actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::operator * did not return the expected value.");
        }

        // A test for operator + (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2AdditionTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            Matrix3x2 expected;
            expected.M11 = a.M11 + b.M11; expected.M12 = a.M12 + b.M12;
            expected.M21 = a.M21 + b.M21; expected.M22 = a.M22 + b.M22;
            expected.M31 = a.M31 + b.M31; expected.M32 = a.M32 + b.M32;

            Matrix3x2 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"Matrix3x2::operator + did not return the expected value.");
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

            actual = Matrix3x2::Add(a, b);
            Assert::AreEqual(expected, actual, L"Matrix3x2::Add did not return the expected value.");
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
            actual = Matrix3x2::Multiply(a, b);

            Assert::AreEqual(expected, actual, L"Matrix3x2::Multiply did not return the expected value.");

            // Sanity check by comparison with 4x4 multiply.
            a = Matrix3x2::CreateRotation(ToRadians(30)) * Matrix3x2::CreateTranslation(23, 42);
            b = Matrix3x2::CreateScale(3, 7) * Matrix3x2::CreateTranslation(666, -1);

            actual = Matrix3x2::Multiply(a, b);

            Matrix4x4 a44(a);
            Matrix4x4 b44(b);
            Matrix4x4 expected44 = Matrix4x4::Multiply(a44, b44);
            Matrix4x4 actual44(actual);

            Assert::IsTrue(Equal(expected44, actual44), L"Matrix3x2::Multiply did not return the expected value.");
        }

        // A test for Multiply (Matrix3x2, float)
        TEST_METHOD(Matrix3x2MultiplyTest5)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected(3, 6, 9, 12, 15, 18);
            Matrix3x2 actual = Matrix3x2::Multiply(a, 3);

            Assert::AreEqual(expected, actual, L"Matrix3x2::Multiply did not return the expected value.");
        }

        // A test for Multiply (Matrix3x2, float)
        TEST_METHOD(Matrix3x2MultiplyTest6)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected(3, 6, 9, 12, 15, 18);
            Matrix3x2 actual = a * 3;

            Assert::AreEqual(expected, actual, L"Matrix3x2::operator * did not return the expected value.");
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

            actual = Matrix3x2::Negate(m);
            Assert::AreEqual(expected, actual, L"Matrix3x2::Negate did not return the expected value.");
        }

        // A test for operator != (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2InequalityTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Matrix3x2::operator != did not return the expected value.");

            // case 2: compare between different values
            b.M11 = 11.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Matrix3x2::operator != did not return the expected value.");
        }

        // A test for operator == (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2EqualityTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Matrix3x2::operator == did not return the expected value.");

            // case 2: compare between different values
            b.M11 = 11.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Matrix3x2::operator == did not return the expected value.");
        }

        // A test for Subtract (Matrix3x2, Matrix3x2)
        TEST_METHOD(Matrix3x2SubtractTest)
        {
            Matrix3x2 a = GenerateMatrixNumberFrom1To6();
            Matrix3x2 b = GenerateMatrixNumberFrom1To6();
            Matrix3x2 expected;
            Matrix3x2 actual;

            actual = Matrix3x2::Subtract(a, b);
            Assert::AreEqual(expected, actual, L"Matrix3x2::Subtract did not return the expected value.");
        }

        // A test for operator +=
        TEST_METHOD(Matrix3x2OperatorAddEqualsTest)
        {
            Matrix3x2 a(1, 2, 3, 4, 5, 6);
            Matrix3x2 b(7, 8, 9, 8, 7, 6);
            Matrix3x2 expected = Matrix3x2::Add(a, b);
            
            // In-place += operation.
            Matrix3x2& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = Matrix3x2::Add(b, b);
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Matrix3x2OperatorSubtractEqualsTest)
        {
            Matrix3x2 a(1, 2, 3, 4, 5, 6);
            Matrix3x2 b(7, 8, 9, 8, 7, 6);
            Matrix3x2 expected = Matrix3x2::Subtract(a, b);
            
            // In-place -= operation.
            Matrix3x2& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = Matrix3x2::Subtract(b, b);
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Matrix3x2OperatorMultiplyEqualsTest)
        {
            Matrix3x2 a(1, 2, 3, 4, 5, 6);
            Matrix3x2 b(7, 8, 9, 8, 7, 6);
            Matrix3x2 expected = Matrix3x2::Multiply(a, b);
            
            // In-place *= operation.
            Matrix3x2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = Matrix3x2::Multiply(b, b);
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Matrix3x2OperatorMultiplyEqualsTest2)
        {
            Matrix3x2 a(1, 2, 3, 4, 5, 6);
            float b = 23;
            Matrix3x2 expected = Matrix3x2::Multiply(a, b);
            
            // In-place *= operation.
            Matrix3x2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for CreateScale (Vector2)
        TEST_METHOD(Matrix3x2CreateScaleTest1)
        {
            Vector2 scales(2.0f, 3.0f);
            Matrix3x2 expected(
                2.0f, 0.0f,
                0.0f, 3.0f,
                0.0f, 0.0f);
            Matrix3x2 actual = Matrix3x2::CreateScale(scales);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateScale did not return the expected value.");
        }

        // A test for CreateScale (Vector2, Vector2)
        TEST_METHOD(Matrix3x2CreateScaleCenterTest1)
        {
            Vector2 scale(3, 4);
            Vector2 center(23, 42);

            Matrix3x2 scaleAroundZero = Matrix3x2::CreateScale(scale, Vector2::Zero());
            Matrix3x2 scaleAroundZeroExpected = Matrix3x2::CreateScale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            Matrix3x2 scaleAroundCenter = Matrix3x2::CreateScale(scale, center);
            Matrix3x2 scaleAroundCenterExpected = Matrix3x2::CreateTranslation(-center) * Matrix3x2::CreateScale(scale) * Matrix3x2::CreateTranslation(center);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));
        }

        // A test for CreateScale (float)
        TEST_METHOD(Matrix3x2CreateScaleTest2)
        {
            float scale = 2.0f;
            Matrix3x2 expected(
                2.0f, 0.0f,
                0.0f, 2.0f,
                0.0f, 0.0f);
            Matrix3x2 actual = Matrix3x2::CreateScale(scale);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateScale did not return the expected value.");
        }

        // A test for CreateScale (float, Vector2)
        TEST_METHOD(Matrix3x2CreateScaleCenterTest2)
        {
            float scale = 5;
            Vector2 center(23, 42);

            Matrix3x2 scaleAroundZero = Matrix3x2::CreateScale(scale, Vector2::Zero());
            Matrix3x2 scaleAroundZeroExpected = Matrix3x2::CreateScale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            Matrix3x2 scaleAroundCenter = Matrix3x2::CreateScale(scale, center);
            Matrix3x2 scaleAroundCenterExpected = Matrix3x2::CreateTranslation(-center) * Matrix3x2::CreateScale(scale) * Matrix3x2::CreateTranslation(center);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));
        }

        // A test for CreateScale (float, float)
        TEST_METHOD(Matrix3x2CreateScaleTest3)
        {
            float xScale = 2.0f;
            float yScale = 3.0f;
            Matrix3x2 expected(
                2.0f, 0.0f,
                0.0f, 3.0f,
                0.0f, 0.0f);
            Matrix3x2 actual = Matrix3x2::CreateScale(xScale, yScale);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateScale did not return the expected value.");
        }

        // A test for CreateScale (float, float, Vector2)
        TEST_METHOD(Matrix3x2CreateScaleCenterTest3)
        {
            Vector2 scale(3, 4);
            Vector2 center(23, 42);

            Matrix3x2 scaleAroundZero = Matrix3x2::CreateScale(scale.X, scale.Y, Vector2::Zero());
            Matrix3x2 scaleAroundZeroExpected = Matrix3x2::CreateScale(scale.X, scale.Y);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            Matrix3x2 scaleAroundCenter = Matrix3x2::CreateScale(scale.X, scale.Y, center);
            Matrix3x2 scaleAroundCenterExpected = Matrix3x2::CreateTranslation(-center) * Matrix3x2::CreateScale(scale.X, scale.Y) * Matrix3x2::CreateTranslation(center);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));
        }

        // A test for CreateTranslation (Vector2)
        TEST_METHOD(Matrix3x2CreateTranslationTest1)
        {
            Vector2 position(2.0f, 3.0f);
            Matrix3x2 expected(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            Matrix3x2 actual = Matrix3x2::CreateTranslation(position);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateTranslation did not return the expected value.");
        }

        // A test for CreateTranslation (float, float)
        TEST_METHOD(Matrix3x2CreateTranslationTest2)
        {
            float xPosition = 2.0f;
            float yPosition = 3.0f;

            Matrix3x2 expected(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            Matrix3x2 actual = Matrix3x2::CreateTranslation(xPosition, yPosition);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateTranslation did not return the expected value.");
        }

        // A test for Translation
        TEST_METHOD(Matrix3x2TranslationTest)
        {
            Matrix3x2 a = GenerateTestMatrix();
            Matrix3x2 b = a;

            // Transfomed vector that has same semantics of property must be same.
            Vector2 val(a.M31, a.M32);
            Assert::AreEqual(val, a.Translation(), L"Matrix3x2::Translation was not set correctly.");

            // Set value and get value must be same.
            val = Vector2(1.0f, 2.0f);
            a.SetTranslation(val);
            Assert::AreEqual(val, a.Translation(), L"Matrix3x2::Translation was not set correctly.");

            // Make sure it only modifies expected value of matrix.
            Assert::IsTrue(
                a.M11 == b.M11 && a.M12 == b.M12 &&
                a.M21 == b.M21 && a.M22 == b.M22 &&
                a.M31 != b.M31 && a.M32 != b.M32,
                L"Matrix3x2::Translation modified unexpected value of matrix.");
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewIdentityTest)
        {
            Matrix3x2 expected = Matrix3x2::Identity();
            Matrix3x2 actual = Matrix3x2::CreateSkew(0, 0);
            Assert::AreEqual(expected, actual, L"Matrix3x2::CreateSkew did not return the expected value.");
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewXTest)
        {
            Matrix3x2 expected(1, 0, -0.414213562373095f, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2::CreateSkew(-DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            expected = Matrix3x2(1, 0, 0.414213562373095f, 1, 0, 0);
            actual = Matrix3x2::CreateSkew(DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2::Transform(Vector2(0, 0), actual);
            Assert::IsTrue(Equal(Vector2(0, 0), result));

            result = Vector2::Transform(Vector2(0, 1), actual);
            Assert::IsTrue(Equal(Vector2(0.414213568f, 1), result));

            result = Vector2::Transform(Vector2(0, -1), actual);
            Assert::IsTrue(Equal(Vector2(-0.414213568f, -1), result));

            result = Vector2::Transform(Vector2(3, 10), actual);
            Assert::IsTrue(Equal(Vector2(7.14213568f, 10), result));
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewYTest)
        {
            Matrix3x2 expected(1, -0.414213562373095f, 0, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2::CreateSkew(0, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            expected = Matrix3x2(1, 0.414213562373095f, 0, 1, 0, 0);
            actual = Matrix3x2::CreateSkew(0, DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2::Transform(Vector2(0, 0), actual);
            Assert::IsTrue(Equal(Vector2(0, 0), result));

            result = Vector2::Transform(Vector2(1, 0), actual);
            Assert::IsTrue(Equal(Vector2(1, 0.414213568f), result));

            result = Vector2::Transform(Vector2(-1, 0), actual);
            Assert::IsTrue(Equal(Vector2(-1, -0.414213568f), result));

            result = Vector2::Transform(Vector2(10, 3), actual);
            Assert::IsTrue(Equal(Vector2(10, 7.14213568f), result));
        }

        // A test for CreateSkew (float, float)
        TEST_METHOD(Matrix3x2CreateSkewXYTest)
        {
            Matrix3x2 expected(1, -0.414213562373095f, 1, 1, 0, 0);
            Matrix3x2 actual = Matrix3x2::CreateSkew(DirectX::XM_PI / 4, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            Vector2 result = Vector2::Transform(Vector2(0, 0), actual);
            Assert::IsTrue(Equal(Vector2(0, 0), result));

            result = Vector2::Transform(Vector2(1, 0), actual);
            Assert::IsTrue(Equal(Vector2(1, -0.414213562373095f), result));

            result = Vector2::Transform(Vector2(0, 1), actual);
            Assert::IsTrue(Equal(Vector2(1, 1), result));

            result = Vector2::Transform(Vector2(1, 1), actual);
            Assert::IsTrue(Equal(Vector2(2, 0.585786437626905f), result));
        }

        // A test for CreateSkew (float, float, Vector2)
        TEST_METHOD(Matrix3x2CreateSkewCenterTest)
        {
            float skewX = 1, skewY = 2;
            Vector2 center(23, 42);

            Matrix3x2 skewAroundZero = Matrix3x2::CreateSkew(skewX, skewY, Vector2::Zero());
            Matrix3x2 skewAroundZeroExpected = Matrix3x2::CreateSkew(skewX, skewY);
            Assert::IsTrue(Equal(skewAroundZero, skewAroundZeroExpected));

            Matrix3x2 skewAroundCenter = Matrix3x2::CreateSkew(skewX, skewY, center);
            Matrix3x2 skewAroundCenterExpected = Matrix3x2::CreateTranslation(-center) * Matrix3x2::CreateSkew(skewX, skewY) * Matrix3x2::CreateTranslation(center);
            Assert::IsTrue(Equal(skewAroundZero, skewAroundZeroExpected));
        }

        // A test for IsIdentity()
        TEST_METHOD(Matrix3x2IsIdentityTest)
        {
            Assert::IsTrue(Matrix3x2::Identity().IsIdentity());
            Assert::IsTrue(Matrix3x2(1, 0, 0, 1, 0, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(0, 0, 0, 1, 0, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(1, 1, 0, 1, 0, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(1, 0, 1, 1, 0, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(1, 0, 0, 0, 0, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(1, 0, 0, 1, 1, 0).IsIdentity());
            Assert::IsFalse(Matrix3x2(1, 0, 0, 1, 0, 1).IsIdentity());
        }

        // A test for Matrix3x2 comparison involving NaN values
        TEST_METHOD(Matrix3x2EqualsNanTest)
        {
            Matrix3x2 a(NAN, 0, 0, 0, 0, 0);
            Matrix3x2 b(0, NAN, 0, 0, 0, 0);
            Matrix3x2 c(0, 0, NAN, 0, 0, 0);
            Matrix3x2 d(0, 0, 0, NAN, 0, 0);
            Matrix3x2 e(0, 0, 0, 0, NAN, 0);
            Matrix3x2 f(0, 0, 0, 0, 0, NAN);

            Assert::IsFalse(a == Matrix3x2());
            Assert::IsFalse(b == Matrix3x2());
            Assert::IsFalse(c == Matrix3x2());
            Assert::IsFalse(d == Matrix3x2());
            Assert::IsFalse(e == Matrix3x2());
            Assert::IsFalse(f == Matrix3x2());

            Assert::IsTrue(a != Matrix3x2());
            Assert::IsTrue(b != Matrix3x2());
            Assert::IsTrue(c != Matrix3x2());
            Assert::IsTrue(d != Matrix3x2());
            Assert::IsTrue(e != Matrix3x2());
            Assert::IsTrue(f != Matrix3x2());

            Assert::IsFalse(a.IsIdentity());
            Assert::IsFalse(b.IsIdentity());
            Assert::IsFalse(c.IsIdentity());
            Assert::IsFalse(d.IsIdentity());
            Assert::IsFalse(e.IsIdentity());
            Assert::IsFalse(f.IsIdentity());

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
            Assert::IsFalse(e == e);
            Assert::IsFalse(f == f);
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

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Matrix3x2TypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Matrix3x2>::value);
            Assert::IsFalse(std::is_pod<Matrix3x2>::value);
            Assert::IsFalse(std::is_trivial<Matrix3x2>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Matrix3x2>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Matrix3x2>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Matrix3x2>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Matrix3x2>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Matrix3x2>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Matrix3x2>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Matrix3x2>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Matrix3x2>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Matrix3x2>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Matrix3x2>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Matrix3x2>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_trivially_destructible<Matrix3x2>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Matrix3x2>::value);
        }
    };
}
