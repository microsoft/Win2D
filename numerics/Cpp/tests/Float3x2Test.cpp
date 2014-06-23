//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Platform::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(Float3x2Test)
    {
    public:
        static float3x2 GenerateMatrixNumberFrom1To6()
        {
            float3x2 a;
            a.m11 = 1.0f; a.m12 = 2.0f;
            a.m21 = 3.0f; a.m22 = 4.0f;
            a.m31 = 5.0f; a.m32 = 6.0f;
            return a;
        }

        static float3x2 GenerateTestMatrix()
        {
            float3x2 m = make_float3x2_rotation(ToRadians(30.0f)) *
                         make_float3x2_translation(111.0f, 222.0f);
            return m;
        }

        // A test for float3x2 (float, float, float, float, float, float)
        TEST_METHOD(Float3x2ConstructorTest)
        {
            float3x2 val(1, 2, 3, 4, 5, 6);

            Assert::AreEqual(val.m11, 1.0f);
            Assert::AreEqual(val.m12, 2.0f);
            Assert::AreEqual(val.m21, 3.0f);
            Assert::AreEqual(val.m22, 4.0f);
            Assert::AreEqual(val.m31, 5.0f);
            Assert::AreEqual(val.m32, 6.0f);
        }

        // A test for float3x2 ()
        TEST_METHOD(Float3x2ConstructorTest2)
        {
            float3x2 val;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            val.m11 = 0;    // avoid warning about unused variable
        }

        // A test for identity
        TEST_METHOD(Float3x2IdentityTest)
        {
            float3x2 val = float3x2(0, 0, 0, 0, 0, 0);
            val.m11 = val.m22 = 1.0f;

            Assert::IsTrue(Equal(val, float3x2::identity()), L"float3x2::Indentity was not set correctly.");
        }

        // A test for determinant
        TEST_METHOD(Float3x2DeterminantTest)
        {
            float3x2 target = make_float3x2_rotation(ToRadians(30.0f));

            float val = 1.0f;
            float det = determinant(target);

            Assert::IsTrue(Equal(val, det), L"determinant was not set correctly.");
        }

        // A test for determinant
        TEST_METHOD(Float3x2DeterminantTest1)
        {
            float3x2 a;
            a.m11 = 5.0f; a.m12 = 2.0f;
            a.m21 = 12.0f; a.m22 = 6.8f;
            a.m31 = 6.5f; a.m32 = 1.0f;
            float3x2 i;
            Assert::IsTrue(invert(a, &i));

            float detA = determinant(a);
            float detI = determinant(i);
            float t = 1.0f / detI;

            // only accurate to 3 precision
            Assert::IsTrue(fabs(detA - t) < 1e-3, L"determinant was not set correctly.");

            // sanity check against 4x4 version
            Assert::AreEqual(determinant(float4x4(a)), detA);
            Assert::AreEqual(determinant(float4x4(i)), detI);
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertTest)
        {
            float3x2 mtx = make_float3x2_rotation(ToRadians(30.0f));

            float3x2 expected;
            expected.m11 = 0.8660254f;
            expected.m12 = -0.5f;

            expected.m21 = 0.5f;
            expected.m22 = 0.8660254f;

            expected.m31 = 0;
            expected.m32 = 0;

            float3x2 actual;

            Assert::IsTrue(invert(mtx, &actual));
            Assert::IsTrue(Equal(expected, actual), L"invert did not return the expected value.");

            float3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, float3x2::identity()), L"invert did not return the expected value.");
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertInPlaceTest)
        {
            float3x2 mtx = make_float3x2_rotation(ToRadians(30.0f)) * 
                            make_float3x2_scale(2, 3) * 
                            make_float3x2_translation(23, 42);

            float3x2 expected;
            Assert::IsTrue(invert(mtx, &expected));

            Assert::IsTrue(invert(mtx, &mtx));
            Assert::IsTrue(Equal(expected, mtx), L"invert did not return the expected value.");
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertIdentityTest)
        {
            float3x2 mtx = float3x2::identity();

            float3x2 actual;
            Assert::IsTrue(invert(mtx, &actual));

            Assert::IsTrue(Equal(actual, float3x2::identity()));
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertTranslationTest)
        {
            float3x2 mtx = make_float3x2_translation(23, 42);

            float3x2 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, float3x2::identity()));
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertRotationTest)
        {
            float3x2 mtx = make_float3x2_rotation(2);

            float3x2 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, float3x2::identity()));
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertScaleTest)
        {
            float3x2 mtx = make_float3x2_scale(23, -42);

            float3x2 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, float3x2::identity()));
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertAffineTest)
        {
            float3x2 mtx = make_float3x2_rotation(2) *
                            make_float3x2_scale(23, -42) *
                            make_float3x2_translation(17, 53);

            float3x2 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float3x2 i = mtx * actual;
            Assert::IsTrue(Equal(i, float3x2::identity()));
        }

        // A test for rotation (float)
        TEST_METHOD(Float3x2CreateRotationTest)
        {
            float radians = ToRadians(50.0f);

            float3x2 expected = float3x2::identity();
            expected.m11 = 0.642787635f;
            expected.m12 = 0.766044438f;
            expected.m21 = -0.766044438f;
            expected.m22 = 0.642787635f;

            float3x2 actual;
            actual = make_float3x2_rotation(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float3x2_rotation did not return the expected value.");
        }

        // A test for rotation (float, float2)
        TEST_METHOD(Float3x2CreateRotationCenterTest)
        {
            float radians = ToRadians(30.0f);
            float2 center(23, 42);

            float3x2 rotateAroundZero = make_float3x2_rotation(radians, float2::zero());
            float3x2 rotateAroundZeroExpected = make_float3x2_rotation(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            float3x2 rotateAroundCenter = make_float3x2_rotation(radians, center);
            float3x2 rotateAroundCenterExpected = make_float3x2_translation(-center) * make_float3x2_rotation(radians) * make_float3x2_translation(center);
            Assert::IsTrue(Equal(rotateAroundCenter, rotateAroundCenterExpected));
        }

        // A test for rotation (float)
        TEST_METHOD(Float3x2CreateRotationRightAngleTest)
        {
            // 90 degree rotations must be exact!
            float3x2 actual = make_float3x2_rotation(0);
            Assert::AreEqual(float3x2(1, 0, 0, 1, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI / 2);
            Assert::AreEqual(float3x2(0, 1, -1, 0, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI);
            Assert::AreEqual(float3x2(-1, 0, 0, -1, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 3 / 2);
            Assert::AreEqual(float3x2(0, -1, 1, 0, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 2);
            Assert::AreEqual(float3x2(1, 0, 0, 1, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 5 / 2);
            Assert::AreEqual(float3x2(0, 1, -1, 0, 0, 0), actual);

            actual = make_float3x2_rotation(-DirectX::XM_PI / 2);
            Assert::AreEqual(float3x2(0, -1, 1, 0, 0, 0), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = make_float3x2_rotation(DirectX::XM_PI + delta);
            Assert::IsFalse(Equal(float3x2(-1, 0, 0, -1, 0, 0), actual));

            actual = make_float3x2_rotation(DirectX::XM_PI - delta);
            Assert::IsFalse(Equal(float3x2(-1, 0, 0, -1, 0, 0), actual));
        }

        // A test for rotation (float, float2)
        TEST_METHOD(Float3x2CreateRotationRightAngleCenterTest)
        {
            float2 center(3, 7);

            // 90 degree rotations must be exact!
            float3x2 actual = make_float3x2_rotation(0, center);
            Assert::AreEqual(float3x2(1, 0, 0, 1, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI / 2, center);
            Assert::AreEqual(float3x2(0, 1, -1, 0, 10, 4), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI, center);
            Assert::AreEqual(float3x2(-1, 0, 0, -1, 6, 14), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 3 / 2, center);
            Assert::AreEqual(float3x2(0, -1, 1, 0, -4, 10), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 2, center);
            Assert::AreEqual(float3x2(1, 0, 0, 1, 0, 0), actual);

            actual = make_float3x2_rotation(DirectX::XM_PI * 5 / 2, center);
            Assert::AreEqual(float3x2(0, 1, -1, 0, 10, 4), actual);

            actual = make_float3x2_rotation(-DirectX::XM_PI / 2, center);
            Assert::AreEqual(float3x2(0, -1, 1, 0, -4, 10), actual);

            // But merely close-to-90 rotations should not be excessively clamped.
            float delta = ToRadians(0.01f);

            actual = make_float3x2_rotation(DirectX::XM_PI + delta, center);
            Assert::IsFalse(Equal(float3x2(-1, 0, 0, -1, 6, 14), actual));

            actual = make_float3x2_rotation(DirectX::XM_PI - delta, center);
            Assert::IsFalse(Equal(float3x2(-1, 0, 0, -1, 6, 14), actual));
        }

        // A test for invert (float3x2)
        TEST_METHOD(Float3x2InvertTest1)
        {
            float3x2 a;
            a.m11 = 0.0f; a.m12 = 2.0f;
            a.m21 = 0.0f; a.m22 = 4.0f;
            a.m31 = 5.0f; a.m32 = 6.0f;

            float detA = determinant(a);
            Assert::IsTrue(Equal(detA, 0.0f), L"invert did not return the expected value.");

            float3x2 actual;
            Assert::IsFalse(invert(a, &actual));

            // all the elements in Actual is NaN
            Assert::IsTrue(
                isnan(actual.m11) && isnan(actual.m12) &&
                isnan(actual.m21) && isnan(actual.m22) &&
                isnan(actual.m31) && isnan(actual.m32)
                , L"invert did not return the expected value.");
        }

        // A test for lerp (float3x2, float3x2, float)
        TEST_METHOD(Float3x2LerpTest)
        {
            float3x2 a;
            a.m11 = 11.0f; a.m12 = 12.0f;
            a.m21 = 21.0f; a.m22 = 22.0f;
            a.m31 = 31.0f; a.m32 = 32.0f;

            float3x2 b = GenerateMatrixNumberFrom1To6();

            float t = 0.5f;

            float3x2 expected;
            expected.m11 = a.m11 + (b.m11 - a.m11) * t;
            expected.m12 = a.m12 + (b.m12 - a.m12) * t;

            expected.m21 = a.m21 + (b.m21 - a.m21) * t;
            expected.m22 = a.m22 + (b.m22 - a.m22) * t;

            expected.m31 = a.m31 + (b.m31 - a.m31) * t;
            expected.m32 = a.m32 + (b.m32 - a.m32) * t;

            float3x2 actual;
            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for operator - (float3x2)
        TEST_METHOD(Float3x2UnaryNegationTest)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();

            float3x2 expected;
            expected.m11 = -1.0f; expected.m12 = -2.0f;
            expected.m21 = -3.0f; expected.m22 = -4.0f;
            expected.m31 = -5.0f; expected.m32 = -6.0f;

            float3x2 actual = -a;
            Assert::IsTrue(Equal(expected, actual), L"float3x2::operator - did not return the expected value.");
        }

        // A test for operator - (float3x2, float3x2)
        TEST_METHOD(Float3x2SubtractionTest)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();
            float3x2 b = GenerateMatrixNumberFrom1To6();
            float3x2 expected = float3x2(0, 0, 0, 0, 0, 0);

            float3x2 actual = a - b;
            Assert::IsTrue(Equal(expected, actual), L"float3x2::operator - did not return the expected value.");
        }

        // A test for operator * (float3x2, float3x2)
        TEST_METHOD(Float3x2MultiplyTest1)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();
            float3x2 b = GenerateMatrixNumberFrom1To6();

            float3x2 expected;
            expected.m11 = a.m11 * b.m11 + a.m12 * b.m21;
            expected.m12 = a.m11 * b.m12 + a.m12 * b.m22;

            expected.m21 = a.m21 * b.m11 + a.m22 * b.m21;
            expected.m22 = a.m21 * b.m12 + a.m22 * b.m22;

            expected.m31 = a.m31 * b.m11 + a.m32 * b.m21 + b.m31;
            expected.m32 = a.m31 * b.m12 + a.m32 * b.m22 + b.m32;

            float3x2 actual = a * b;
            Assert::IsTrue(Equal(expected, actual), L"float3x2::operator * did not return the expected value.");

            // Sanity check by comparison with 4x4 multiply.
            a = make_float3x2_rotation(ToRadians(30)) * make_float3x2_translation(23, 42);
            b = make_float3x2_scale(3, 7) * make_float3x2_translation(666, -1);

            actual = a * b;

            float4x4 a44(a);
            float4x4 b44(b);
            float4x4 expected44 = a44 * b44;
            float4x4 actual44(actual);

            Assert::IsTrue(Equal(expected44, actual44), L"float3x2::operator * did not return the expected value.");
        }

        // A test for operator * (float3x2, float3x2)
        TEST_METHOD(Float3x2MultiplyTest4)
        {
            float3x2 a;
            a.m11 = 1.0f; a.m12 = 2.0f;
            a.m21 = 5.0f; a.m22 = -6.0f;
            a.m31 = 9.0f; a.m32 = 10.0f;

            float3x2 b;
            b = float3x2::identity();

            float3x2 expected = a;
            float3x2 actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"float3x2::operator * did not return the expected value.");
        }

        // A test for operator + (float3x2, float3x2)
        TEST_METHOD(Float3x2AdditionTest)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();
            float3x2 b = GenerateMatrixNumberFrom1To6();

            float3x2 expected;
            expected.m11 = a.m11 + b.m11; expected.m12 = a.m12 + b.m12;
            expected.m21 = a.m21 + b.m21; expected.m22 = a.m22 + b.m22;
            expected.m31 = a.m31 + b.m31; expected.m32 = a.m32 + b.m32;

            float3x2 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"float3x2::operator + did not return the expected value.");
        }

        // A test for operator != (float3x2, float3x2)
        TEST_METHOD(Float3x2InequalityTest)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();
            float3x2 b = GenerateMatrixNumberFrom1To6();

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"float3x2::operator != did not return the expected value.");

            // case 2: compare between different values
            b.m11 = 11.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"float3x2::operator != did not return the expected value.");
        }

        // A test for operator == (float3x2, float3x2)
        TEST_METHOD(Float3x2EqualityTest)
        {
            float3x2 a = GenerateMatrixNumberFrom1To6();
            float3x2 b = GenerateMatrixNumberFrom1To6();

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"float3x2::operator == did not return the expected value.");

            // case 2: compare between different values
            b.m11 = 11.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"float3x2::operator == did not return the expected value.");
        }

        // A test for operator +=
        TEST_METHOD(Float3x2OperatorAddEqualsTest)
        {
            float3x2 a(1, 2, 3, 4, 5, 6);
            float3x2 b(7, 8, 9, 8, 7, 6);
            float3x2 expected = a + b;
            
            // In-place += operation.
            float3x2& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Float3x2OperatorSubtractEqualsTest)
        {
            float3x2 a(1, 2, 3, 4, 5, 6);
            float3x2 b(7, 8, 9, 8, 7, 6);
            float3x2 expected = a - b;
            
            // In-place -= operation.
            float3x2& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float3x2OperatorMultiplyEqualsTest)
        {
            float3x2 a(1, 2, 3, 4, 5, 6);
            float3x2 b(7, 8, 9, 8, 7, 6);
            float3x2 expected = a * b;
            
            // In-place *= operation.
            float3x2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);

            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float3x2OperatorMultiplyEqualsTest2)
        {
            float3x2 a(1, 2, 3, 4, 5, 6);
            float b = 23;
            float3x2 expected = a * b;
            
            // In-place *= operation.
            float3x2& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual<void*>(&a, &ret);
        }

        // A test for scale (float2)
        TEST_METHOD(Float3x2CreateScaleTest1)
        {
            float2 scales(2.0f, 3.0f);
            float3x2 expected(
                2.0f, 0.0f,
                0.0f, 3.0f,
                0.0f, 0.0f);
            float3x2 actual = make_float3x2_scale(scales);
            Assert::AreEqual(expected, actual, L"make_float3x2_scale did not return the expected value.");
        }

        // A test for scale (float2, float2)
        TEST_METHOD(Float3x2CreateScaleCenterTest1)
        {
            float2 scale(3, 4);
            float2 center(23, 42);

            float3x2 scaleAroundZero = make_float3x2_scale(scale, float2::zero());
            float3x2 scaleAroundZeroExpected = make_float3x2_scale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float3x2 scaleAroundCenter = make_float3x2_scale(scale, center);
            float3x2 scaleAroundCenterExpected = make_float3x2_translation(-center) * make_float3x2_scale(scale) * make_float3x2_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for scale (float)
        TEST_METHOD(Float3x2CreateScaleTest2)
        {
            float scale = 2.0f;
            float3x2 expected(
                2.0f, 0.0f,
                0.0f, 2.0f,
                0.0f, 0.0f);
            float3x2 actual = make_float3x2_scale(scale);
            Assert::AreEqual(expected, actual, L"make_float3x2_scale did not return the expected value.");
        }

        // A test for scale (float, float2)
        TEST_METHOD(Float3x2CreateScaleCenterTest2)
        {
            float scale = 5;
            float2 center(23, 42);

            float3x2 scaleAroundZero = make_float3x2_scale(scale, float2::zero());
            float3x2 scaleAroundZeroExpected = make_float3x2_scale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float3x2 scaleAroundCenter = make_float3x2_scale(scale, center);
            float3x2 scaleAroundCenterExpected = make_float3x2_translation(-center) * make_float3x2_scale(scale) * make_float3x2_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for scale (float, float)
        TEST_METHOD(Float3x2CreateScaleTest3)
        {
            float xScale = 2.0f;
            float yScale = 3.0f;
            float3x2 expected(
                2.0f, 0.0f,
                0.0f, 3.0f,
                0.0f, 0.0f);
            float3x2 actual = make_float3x2_scale(xScale, yScale);
            Assert::AreEqual(expected, actual, L"make_float3x2_scale did not return the expected value.");
        }

        // A test for scale (float, float, float2)
        TEST_METHOD(Float3x2CreateScaleCenterTest3)
        {
            float2 scale(3, 4);
            float2 center(23, 42);

            float3x2 scaleAroundZero = make_float3x2_scale(scale.x, scale.y, float2::zero());
            float3x2 scaleAroundZeroExpected = make_float3x2_scale(scale.x, scale.y);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float3x2 scaleAroundCenter = make_float3x2_scale(scale.x, scale.y, center);
            float3x2 scaleAroundCenterExpected = make_float3x2_translation(-center) * make_float3x2_scale(scale.x, scale.y) * make_float3x2_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for translation (float2)
        TEST_METHOD(Float3x2CreateTranslationTest1)
        {
            float2 position(2.0f, 3.0f);
            float3x2 expected(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            float3x2 actual = make_float3x2_translation(position);
            Assert::AreEqual(expected, actual, L"make_float3x2_translation did not return the expected value.");
        }

        // A test for translation (float, float)
        TEST_METHOD(Float3x2CreateTranslationTest2)
        {
            float xPosition = 2.0f;
            float yPosition = 3.0f;

            float3x2 expected(
                1.0f, 0.0f,
                0.0f, 1.0f,
                2.0f, 3.0f);

            float3x2 actual = make_float3x2_translation(xPosition, yPosition);
            Assert::AreEqual(expected, actual, L"make_float3x2_translation did not return the expected value.");
        }

        // A test for translation()
        TEST_METHOD(Float3x2TranslationTest)
        {
            float3x2 a = GenerateTestMatrix();

            // Transfomed vector that has same semantics of property must be same.
            float2 val(a.m31, a.m32);
            Assert::AreEqual(val, translation(a), L"translation was not set correctly.");
        }

        // A test for skew (float, float)
        TEST_METHOD(Float3x2CreateSkewIdentityTest)
        {
            float3x2 expected = float3x2::identity();
            float3x2 actual = make_float3x2_skew(0, 0);
            Assert::AreEqual(expected, actual, L"make_float3x2_skew did not return the expected value.");
        }

        // A test for skew (float, float)
        TEST_METHOD(Float3x2CreateSkewXTest)
        {
            float3x2 expected(1, 0, -0.414213562373095f, 1, 0, 0);
            float3x2 actual = make_float3x2_skew(-DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            expected = float3x2(1, 0, 0.414213562373095f, 1, 0, 0);
            actual = make_float3x2_skew(DirectX::XM_PI / 8, 0);
            Assert::IsTrue(Equal(expected, actual));

            float2 result = transform(float2(0, 0), actual);
            Assert::IsTrue(Equal(float2(0, 0), result));

            result = transform(float2(0, 1), actual);
            Assert::IsTrue(Equal(float2(0.414213568f, 1), result));

            result = transform(float2(0, -1), actual);
            Assert::IsTrue(Equal(float2(-0.414213568f, -1), result));

            result = transform(float2(3, 10), actual);
            Assert::IsTrue(Equal(float2(7.14213568f, 10), result));
        }

        // A test for skew (float, float)
        TEST_METHOD(Float3x2CreateSkewYTest)
        {
            float3x2 expected(1, -0.414213562373095f, 0, 1, 0, 0);
            float3x2 actual = make_float3x2_skew(0, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            expected = float3x2(1, 0.414213562373095f, 0, 1, 0, 0);
            actual = make_float3x2_skew(0, DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            float2 result = transform(float2(0, 0), actual);
            Assert::IsTrue(Equal(float2(0, 0), result));

            result = transform(float2(1, 0), actual);
            Assert::IsTrue(Equal(float2(1, 0.414213568f), result));

            result = transform(float2(-1, 0), actual);
            Assert::IsTrue(Equal(float2(-1, -0.414213568f), result));

            result = transform(float2(10, 3), actual);
            Assert::IsTrue(Equal(float2(10, 7.14213568f), result));
        }

        // A test for skew (float, float)
        TEST_METHOD(Float3x2CreateSkewXYTest)
        {
            float3x2 expected(1, -0.414213562373095f, 1, 1, 0, 0);
            float3x2 actual = make_float3x2_skew(DirectX::XM_PI / 4, -DirectX::XM_PI / 8);
            Assert::IsTrue(Equal(expected, actual));

            float2 result = transform(float2(0, 0), actual);
            Assert::IsTrue(Equal(float2(0, 0), result));

            result = transform(float2(1, 0), actual);
            Assert::IsTrue(Equal(float2(1, -0.414213562373095f), result));

            result = transform(float2(0, 1), actual);
            Assert::IsTrue(Equal(float2(1, 1), result));

            result = transform(float2(1, 1), actual);
            Assert::IsTrue(Equal(float2(2, 0.585786437626905f), result));
        }

        // A test for skew (float, float, float2)
        TEST_METHOD(Float3x2CreateSkewCenterTest)
        {
            float skewX = 1, skewY = 2;
            float2 center(23, 42);

            float3x2 skewAroundZero = make_float3x2_skew(skewX, skewY, float2::zero());
            float3x2 skewAroundZeroExpected = make_float3x2_skew(skewX, skewY);
            Assert::IsTrue(Equal(skewAroundZero, skewAroundZeroExpected));

            float3x2 skewAroundCenter = make_float3x2_skew(skewX, skewY, center);
            float3x2 skewAroundCenterExpected = make_float3x2_translation(-center) * make_float3x2_skew(skewX, skewY) * make_float3x2_translation(center);
            Assert::IsTrue(Equal(skewAroundCenter, skewAroundCenterExpected));
        }

        // A test for is_identity()
        TEST_METHOD(Float3x2IsIdentityTest)
        {
            Assert::IsTrue(is_identity(float3x2::identity()));
            Assert::IsTrue(is_identity(float3x2(1, 0, 0, 1, 0, 0)));
            Assert::IsFalse(is_identity(float3x2(0, 0, 0, 1, 0, 0)));
            Assert::IsFalse(is_identity(float3x2(1, 1, 0, 1, 0, 0)));
            Assert::IsFalse(is_identity(float3x2(1, 0, 1, 1, 0, 0)));
            Assert::IsFalse(is_identity(float3x2(1, 0, 0, 0, 0, 0)));
            Assert::IsFalse(is_identity(float3x2(1, 0, 0, 1, 1, 0)));
            Assert::IsFalse(is_identity(float3x2(1, 0, 0, 1, 0, 1)));
        }

        // A test for float3x2 comparison involving NaN values
        TEST_METHOD(Float3x2EqualsNanTest)
        {
            float3x2 a(NAN, 0, 0, 0, 0, 0);
            float3x2 b(0, NAN, 0, 0, 0, 0);
            float3x2 c(0, 0, NAN, 0, 0, 0);
            float3x2 d(0, 0, 0, NAN, 0, 0);
            float3x2 e(0, 0, 0, 0, NAN, 0);
            float3x2 f(0, 0, 0, 0, 0, NAN);

            Assert::IsFalse(a == float3x2());
            Assert::IsFalse(b == float3x2());
            Assert::IsFalse(c == float3x2());
            Assert::IsFalse(d == float3x2());
            Assert::IsFalse(e == float3x2());
            Assert::IsFalse(f == float3x2());

            Assert::IsTrue(a != float3x2());
            Assert::IsTrue(b != float3x2());
            Assert::IsTrue(c != float3x2());
            Assert::IsTrue(d != float3x2());
            Assert::IsTrue(e != float3x2());
            Assert::IsTrue(f != float3x2());

            Assert::IsFalse(is_identity(a));
            Assert::IsFalse(is_identity(b));
            Assert::IsFalse(is_identity(c));
            Assert::IsFalse(is_identity(d));
            Assert::IsFalse(is_identity(e));
            Assert::IsFalse(is_identity(f));

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
            float3x2 a;
            float3x2 b;
        };

        struct Matrix3x2PlusFloat
        {
            float3x2 v;
            float f;
        };

        struct Matrix3x2PlusFloat_2x
        {
            Matrix3x2PlusFloat a;
            Matrix3x2PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Float3x2SizeofTest)
        {
            Assert::AreEqual(size_t(24), sizeof(float3x2));
            Assert::AreEqual(size_t(48), sizeof(Matrix3x2_2x));
            Assert::AreEqual(size_t(28), sizeof(Matrix3x2PlusFloat));
            Assert::AreEqual(size_t(56), sizeof(Matrix3x2PlusFloat_2x));
            Assert::AreEqual(sizeof(float3x2), sizeof(DirectX::XMFLOAT4) + sizeof(DirectX::XMFLOAT2));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Float3x2FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(float3x2, m11));
            Assert::AreEqual(size_t(4), offsetof(float3x2, m12));

            Assert::AreEqual(size_t(8), offsetof(float3x2, m21));
            Assert::AreEqual(size_t(12), offsetof(float3x2, m22));

            Assert::AreEqual(size_t(16), offsetof(float3x2, m31));
            Assert::AreEqual(size_t(20), offsetof(float3x2, m32));
        }

        // A test of float3x2 -> DirectXMath interop
        TEST_METHOD(Float3x2LoadTest)
        {
            float3x2 a(1, 2, 3, 4, 5, 6);
            DirectX::XMMATRIX b = DirectX::XMLoadFloat3x2(&a);
            DirectX::XMFLOAT4X4 c;
            DirectX::XMStoreFloat4x4(&c, b);

            Assert::AreEqual(a.m11, c._11);
            Assert::AreEqual(a.m12, c._12);
            Assert::AreEqual(0.0f, c._13);
            Assert::AreEqual(0.0f, c._14);

            Assert::AreEqual(a.m21, c._21);
            Assert::AreEqual(a.m22, c._22);
            Assert::AreEqual(0.0f, c._23);
            Assert::AreEqual(0.0f, c._24);

            Assert::AreEqual(0.0f, c._31);
            Assert::AreEqual(0.0f, c._32);
            Assert::AreEqual(1.0f, c._33);
            Assert::AreEqual(0.0f, c._13);

            Assert::AreEqual(a.m31, c._41);
            Assert::AreEqual(a.m32, c._42);
            Assert::AreEqual(0.0f, c._43);
            Assert::AreEqual(1.0f, c._44);

            float4x4 expanded(a);
            float4x4 expanded2;
            DirectX::XMStoreFloat4x4(&expanded2, b);
            Assert::AreEqual(expanded, expanded2);
        }

        // A test of DirectXMath -> float3x2 interop
        TEST_METHOD(Float3x2StoreTest)
        {
            DirectX::XMFLOAT4X4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            DirectX::XMMATRIX b = DirectX::XMLoadFloat4x4(&a);
            float3x2 c;
            DirectX::XMStoreFloat3x2(&c, b);

            Assert::AreEqual(a._11, c.m11);
            Assert::AreEqual(a._12, c.m12);

            Assert::AreEqual(a._21, c.m21);
            Assert::AreEqual(a._22, c.m22);

            Assert::AreEqual(a._41, c.m31);
            Assert::AreEqual(a._42, c.m32);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Float3x2TypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<float3x2>::value);
            Assert::IsFalse(std::is_pod<float3x2>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<float3x2>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<float3x2>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<float3x2>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<float3x2>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<float3x2>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<float3x2>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<float3x2>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<float3x2>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<float3x2>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<float3x2>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<float3x2>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<float3x2>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<float3x2>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<float3x2>::value);
            Assert::IsTrue(std::is_trivially_destructible<float3x2>::value);
        }
    };
}
