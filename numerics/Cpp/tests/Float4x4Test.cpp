// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Foundation::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(Float4x4Test)
    {
    public:
        static float4x4 GenerateMatrixNumberFrom1To16()
        {
            float4x4 a;
            a.m11 = 1.0f; a.m12 = 2.0f; a.m13 = 3.0f; a.m14 = 4.0f;
            a.m21 = 5.0f; a.m22 = 6.0f; a.m23 = 7.0f; a.m24 = 8.0f;
            a.m31 = 9.0f; a.m32 = 10.0f; a.m33 = 11.0f; a.m34 = 12.0f;
            a.m41 = 13.0f; a.m42 = 14.0f; a.m43 = 15.0f; a.m44 = 16.0f;
            return a;
        }

        static float4x4 GenerateTestMatrix()
        {
            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f)) *
                make_float4x4_translation(111.0f, 222.0f, 333.0f);
            return m;
        }

        // A test for float4x4 (float, float, float ... x16)
        TEST_METHOD(Float4x4ConstructorTest)
        {
            float4x4 val(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

            Assert::AreEqual(val.m11, 1.0f);
            Assert::AreEqual(val.m12, 2.0f);
            Assert::AreEqual(val.m13, 3.0f);
            Assert::AreEqual(val.m14, 4.0f);
            Assert::AreEqual(val.m21, 5.0f);
            Assert::AreEqual(val.m22, 6.0f);
            Assert::AreEqual(val.m23, 7.0f);
            Assert::AreEqual(val.m24, 8.0f);
            Assert::AreEqual(val.m31, 9.0f);
            Assert::AreEqual(val.m32, 10.0f);
            Assert::AreEqual(val.m33, 11.0f);
            Assert::AreEqual(val.m34, 12.0f);
            Assert::AreEqual(val.m41, 13.0f);
            Assert::AreEqual(val.m42, 14.0f);
            Assert::AreEqual(val.m43, 15.0f);
            Assert::AreEqual(val.m44, 16.0f);
        }

        // A test for float4x4 ()
        TEST_METHOD(Float4x4ConstructorTest2)
        {
            float4x4 val;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            val.m11 = 0;    // avoid warning about unused variable
        }

        // A test for identity
        TEST_METHOD(Float4x4IdentityTest)
        {
            float4x4 val(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

            Assert::IsTrue(Equal(val, float4x4::identity()), L"float4x4::Indentity was not set correctly.");
        }

        // A test for determinant
        TEST_METHOD(Float4x4DeterminantTest)
        {
            float4x4 target =
                    make_float4x4_rotation_x(ToRadians(30.0f)) *
                    make_float4x4_rotation_y(ToRadians(30.0f)) *
                    make_float4x4_rotation_z(ToRadians(30.0f));

            float val = 1.0f;
            float det = determinant(target);

            Assert::IsTrue(Equal(val, det), L"determinant was not set correctly.");
        }

        // A test for determinant
        TEST_METHOD(Float4x4DeterminantTest1)
        {
            float4x4 a;
            a.m11 = 5.0f; a.m12 = 2.0f; a.m13 = 8.25f; a.m14 = 1.0f;
            a.m21 = 12.0f; a.m22 = 6.8f; a.m23 = 2.14f; a.m24 = 9.6f;
            a.m31 = 6.5f; a.m32 = 1.0f; a.m33 = 3.14f; a.m34 = 2.22f;
            a.m41 = 0.0f; a.m42 = 0.86f; a.m43 = 4.0f; a.m44 = 1.0f;
            float4x4 i;
            Assert::IsTrue(invert(a, &i));

            float detA = determinant(a);
            float detI = determinant(i);
            float t = 1.0f / detI;

            // only accurate to 3 precision
            Assert::IsTrue(fabs(detA - t) < 1e-3, L"determinant was not set correctly.");
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertTest)
        {
            float4x4 mtx =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));

            float4x4 expected;
            expected.m11 = 0.74999994f;
            expected.m12 = -0.216506317f;
            expected.m13 = 0.62499994f;
            expected.m14 = 0.0f;

            expected.m21 = 0.433012635f;
            expected.m22 = 0.87499994f;
            expected.m23 = -0.216506317f;
            expected.m24 = 0.0f;

            expected.m31 = -0.49999997f;
            expected.m32 = 0.433012635f;
            expected.m33 = 0.74999994f;
            expected.m34 = 0.0f;

            expected.m41 = 0.0f;
            expected.m42 = 0.0f;
            expected.m43 = 0.0f;
            expected.m44 = 0.99999994f;

            float4x4 actual;

            Assert::IsTrue(invert(mtx, &actual));
            Assert::IsTrue(Equal(expected, actual), L"invert did not return the expected value.");

            // Make sure M*M is identity matrix
            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()), L"invert did not return the expected value.");
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertInPlaceTest)
        {
            float4x4 mtx =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f)) *
                make_float4x4_scale(3, 4, 5) *
                make_float4x4_translation(23, 42, 666);

            float4x4 expected;
            Assert::IsTrue(invert(mtx, &expected));

            Assert::IsTrue(invert(mtx, &mtx));
            Assert::IsTrue(Equal(expected, mtx), L"invert did not return the expected value.");
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertIdentityTest)
        {
            float4x4 mtx = float4x4::identity();

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            Assert::IsTrue(Equal(actual, float4x4::identity()));
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertTranslationTest)
        {
            float4x4 mtx = make_float4x4_translation(23, 42, 666);

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()));
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertRotationTest)
        {
            float4x4 mtx = make_float4x4_from_yaw_pitch_roll(3, 4, 5);

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()));
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertScaleTest)
        {
            float4x4 mtx = make_float4x4_scale(23, 42, -666);

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()));
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertProjectionTest)
        {
            float4x4 mtx = make_float4x4_perspective_field_of_view(1, 1.333f, 0.1f, 666);

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()));
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertAffineTest)
        {
            float4x4 mtx = make_float4x4_from_yaw_pitch_roll(3, 4, 5) *
                            make_float4x4_scale(23, 42, -666) *
                            make_float4x4_translation(17, 53, 89);

            float4x4 actual;
            Assert::IsTrue(invert(mtx, &actual));

            float4x4 i = mtx * actual;
            Assert::IsTrue(Equal(i, float4x4::identity()));
        }

        void DecomposeTest(float yaw, float pitch, float roll, float3 expectedTranslation, float3 expectedScales)
        {
            DecomposeTest(yaw, pitch, roll, expectedTranslation, expectedScales, true);
        }

        void DecomposeTest(float yaw, float pitch, float roll, float3 expectedTranslation, float3 expectedScales, bool expectedResult)
        {
            quaternion expectedRotation = make_quaternion_from_yaw_pitch_roll(
                ToRadians(yaw), ToRadians(pitch), ToRadians(roll));

            float4x4 m = make_float4x4_scale(expectedScales) *
                make_float4x4_from_quaternion(expectedRotation) *
                make_float4x4_translation(expectedTranslation);

            float3 scales;
            quaternion rotation;
            float3 translation;

            bool actualResult = decompose(m, &scales, &rotation, &translation);
            Assert::AreEqual(expectedResult, actualResult, L"decompose did not return expected value.");
            if (expectedResult)
            {
                Assert::IsTrue(Equal(expectedScales, scales), L"decompose did not return expected value.");
            }
            else
            {
                Assert::IsTrue(Equal(fabs(expectedScales.x), fabs(scales.x)) &&
                               Equal(fabs(expectedScales.y), fabs(scales.y)) &&
                               Equal(fabs(expectedScales.z), fabs(scales.z)), L"decompose did not return expected value.");

            }
            Assert::IsTrue(EqualRotation(expectedRotation, rotation), L"decompose did not return expected value.");
            Assert::IsTrue(Equal(expectedTranslation, translation), L"decompose did not return expected value.");
        }
        
        TEST_METHOD(Float4x4DecomposeTest01)
        {
            DecomposeTest(10.0f, 20.0f, 30.0f, float3(10, 20, 30), float3(2, 3, 4));

            const float step = 35.0f;

            for (float yawAngle = -720.0f; yawAngle <= 720.0f; yawAngle += step)
            {
                for (float pitchAngle = -720.0f; pitchAngle <= 720.0f; pitchAngle += step)
                {
                    for (float rollAngle = -720.0f; rollAngle <= 720.0f; rollAngle += step)
                    {
                        DecomposeTest(yawAngle, pitchAngle, rollAngle, float3(10, 20, 30), float3(2, 3, 4));
                    }
                }
            }
        }
        
        TEST_METHOD(Float4x4DecomposeTest02)
        {
            DecomposeTest(10.0f, 20.0f, 30.0f, float3(10, 20, 30), float3(2, 3, 4));

            // Various scales.
            DecomposeTest(0, 0, 0, float3::zero(), float3(1, 2, 3));
            DecomposeTest(0, 0, 0, float3::zero(), float3(1, 3, 2));
            DecomposeTest(0, 0, 0, float3::zero(), float3(2, 1, 3));
            DecomposeTest(0, 0, 0, float3::zero(), float3(2, 3, 1));
            DecomposeTest(0, 0, 0, float3::zero(), float3(3, 1, 2));
            DecomposeTest(0, 0, 0, float3::zero(), float3(3, 2, 1));

            DecomposeTest(0, 0, 0, float3::zero(), float3(-2, 1, 1));

            // Small scales.
            DecomposeTest(0, 0, 0, float3::zero(), float3(1e-6f, 2e-6f, 3e-6f), false);
            DecomposeTest(0, 0, 0, float3::zero(), float3(1e-6f, 3e-6f, 2e-6f), false);
            DecomposeTest(0, 0, 0, float3::zero(), float3(2e-6f, 1e-6f, 3e-6f), false);
            DecomposeTest(0, 0, 0, float3::zero(), float3(2e-6f, 3e-6f, 1e-6f), false);
            DecomposeTest(0, 0, 0, float3::zero(), float3(3e-6f, 1e-6f, 2e-6f), false);
            DecomposeTest(0, 0, 0, float3::zero(), float3(3e-6f, 2e-6f, 1e-6f), false);
        }

        void DecomposeScaleTest(float sx, float sy, float sz)
        {
            float4x4 m = make_float4x4_scale(1, 2e-6f, 1e-6f);
            m.m11 = sx;
            m.m12 = sy;
            m.m13 = sz;

            float3 expectedScales(1, 2e-6f, 1e-6f);
            float3 scales;
            quaternion rotation;
            float3 translation;

            bool actualResult = decompose(m, &scales, &rotation, &translation);
            Assert::AreEqual(false, actualResult, L"decompose did not return expected value.");
            Assert::IsTrue(Equal(fabs(expectedScales.x), fabs(scales.x)) &&
                           Equal(fabs(expectedScales.y), fabs(scales.y)) &&
                           Equal(fabs(expectedScales.z), fabs(scales.z)), L"decompose did not return expected value.");

            Assert::IsTrue(EqualRotation(quaternion::identity(), rotation), L"decompose did not return expected value.");
            Assert::IsTrue(Equal(float3::zero(), translation), L"decompose did not return expected value.");
        }

        TEST_METHOD(Float4x4DecomposeTest03)
        {
            DecomposeScaleTest(1, 2e-6f, 3e-6f);
            DecomposeScaleTest(1, 3e-6f, 2e-6f);
            DecomposeScaleTest(2e-6f, 1, 3e-6f);
            DecomposeScaleTest(2e-6f, 3e-6f, 1);
            DecomposeScaleTest(3e-6f, 1, 2e-6f);
            DecomposeScaleTest(3e-6f, 2e-6f, 1);
        }

        TEST_METHOD(Float4x4TransformTest)
        {
            float4x4 target = GenerateMatrixNumberFrom1To16();

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));

            quaternion q = make_quaternion_from_rotation_matrix(m);

            float4x4 expected = target * m;
            float4x4 actual;
            actual = transform(target, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        // A test for rotation_x (float)
        TEST_METHOD(Float4x4CreateRotationXTest)
        {
            float radians = ToRadians(30.0f);

            float4x4 expected = float4x4::identity();

            expected.m11 = 1.0f;
            expected.m22 = 0.8660254f;
            expected.m23 = 0.5f;
            expected.m32 = -0.5f;
            expected.m33 = 0.8660254f;

            float4x4 actual;

            actual = make_float4x4_rotation_x(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_rotation_x did not return the expected value.");
        }

        // A test for rotation_x (float)
        TEST_METHOD(Float4x4CreateRotationXTest1)
        {
            float radians = 0;

            float4x4 expected = float4x4::identity();
            float4x4 actual = make_float4x4_rotation_x(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_rotation_x did not return the expected value.");
        }

        // A test for rotation_x (float, float3)
        TEST_METHOD(Float4x4CreateRotationXCenterTest)
        {
            float radians = ToRadians(30.0f);
            float3 center(23, 42, 66);

            float4x4 rotateAroundZero = make_float4x4_rotation_x(radians, float3::zero());
            float4x4 rotateAroundZeroExpected = make_float4x4_rotation_x(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            float4x4 rotateAroundCenter = make_float4x4_rotation_x(radians, center);
            float4x4 rotateAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_rotation_x(radians) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(rotateAroundCenter, rotateAroundCenterExpected));
        }

        // A test for rotation_y (float)
        TEST_METHOD(Float4x4CreateRotationYTest)
        {
            float radians = ToRadians(60.0f);

            float4x4 expected = float4x4::identity();

            expected.m11 = 0.49999997f;
            expected.m13 = -0.866025448f;
            expected.m31 = 0.866025448f;
            expected.m33 = 0.49999997f;

            float4x4 actual;
            actual = make_float4x4_rotation_y(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_rotation_y did not return the expected value.");
        }

        // A test for rotation_y (float)
        TEST_METHOD(Float4x4CreateRotationYTest1)
        {
            float radians = ToRadians(-300.0f);

            float4x4 expected = float4x4::identity();

            expected.m11 = 0.49999997f;
            expected.m13 = -0.866025448f;
            expected.m31 = 0.866025448f;
            expected.m33 = 0.49999997f;

            float4x4 actual = make_float4x4_rotation_y(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_rotation_y did not return the expected value.");
        }

        // A test for rotation_y (float, float3)
        TEST_METHOD(Float4x4CreateRotationYCenterTest)
        {
            float radians = ToRadians(30.0f);
            float3 center(23, 42, 66);

            float4x4 rotateAroundZero = make_float4x4_rotation_y(radians, float3::zero());
            float4x4 rotateAroundZeroExpected = make_float4x4_rotation_y(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            float4x4 rotateAroundCenter = make_float4x4_rotation_y(radians, center);
            float4x4 rotateAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_rotation_y(radians) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(rotateAroundCenter, rotateAroundCenterExpected));
        }

        // A test for from_axis_angle(float3,float)
        TEST_METHOD(Float4x4CreateFromAxisAngleTest)
        {
            float radians = ToRadians(-30.0f);

            float4x4 expected = make_float4x4_rotation_x(radians);
            float4x4 actual = make_float4x4_from_axis_angle(float3::unit_x(), radians);
            Assert::IsTrue(Equal(expected, actual));

            expected = make_float4x4_rotation_y(radians);
            actual = make_float4x4_from_axis_angle(float3::unit_y(), radians);
            Assert::IsTrue(Equal(expected, actual));

            expected = make_float4x4_rotation_z(radians);
            actual = make_float4x4_from_axis_angle(float3::unit_z(), radians);
            Assert::IsTrue(Equal(expected, actual));

            expected = make_float4x4_from_quaternion(make_quaternion_from_axis_angle(normalize(float3::one()), radians));
            actual = make_float4x4_from_axis_angle(normalize(float3::one()), radians);
            Assert::IsTrue(Equal(expected, actual));

            const int rotCount = 16;
            for (int i = 0; i < rotCount; ++i)
            {
                float latitude = (2.0f * DirectX::XM_PI) * ((float)i / (float)rotCount);
                for (int j = 0; j < rotCount; ++j)
                {
                    float longitude = -DirectX::XM_PIDIV2 + DirectX::XM_PI * ((float)j / (float)rotCount);

                    float4x4 m = make_float4x4_rotation_z(longitude) * make_float4x4_rotation_y(latitude);
                    float3 axis(m.m11, m.m12, m.m13);
                    for (int k = 0; k < rotCount; ++k)
                    {
                        float rot = (2.0f * DirectX::XM_PI) * ((float)k / (float)rotCount);
                        expected = make_float4x4_from_quaternion(make_quaternion_from_axis_angle(axis, rot));
                        actual = make_float4x4_from_axis_angle(axis, rot);
                        Assert::IsTrue(Equal(expected, actual));
                    }
                }
            }
        }
        
        TEST_METHOD(Float4x4CreateFromYawPitchRollTest1)
        {
            float yawAngle = ToRadians(30.0f);
            float pitchAngle = ToRadians(40.0f);
            float rollAngle = ToRadians(50.0f);

            float4x4 yaw = make_float4x4_from_axis_angle(float3::unit_y(), yawAngle);
            float4x4 pitch = make_float4x4_from_axis_angle(float3::unit_x(), pitchAngle);
            float4x4 roll = make_float4x4_from_axis_angle(float3::unit_z(), rollAngle);

            float4x4 expected = roll * pitch * yaw;
            float4x4 actual = make_float4x4_from_yaw_pitch_roll(yawAngle, pitchAngle, rollAngle);
            Assert::IsTrue(Equal(expected, actual));
        }
        
        TEST_METHOD(Float4x4CreateFromYawPitchRollTest2)
        {
            const float step = 35.0f;

            for (float yawAngle = -720.0f; yawAngle <= 720.0f; yawAngle += step)
            {
                for (float pitchAngle = -720.0f; pitchAngle <= 720.0f; pitchAngle += step)
                {
                    for (float rollAngle = -720.0f; rollAngle <= 720.0f; rollAngle += step)
                    {
                        float yawRad = ToRadians(yawAngle);
                        float pitchRad = ToRadians(pitchAngle);
                        float rollRad = ToRadians(rollAngle);
                        float4x4 yaw = make_float4x4_from_axis_angle(float3::unit_y(), yawRad);
                        float4x4 pitch = make_float4x4_from_axis_angle(float3::unit_x(), pitchRad);
                        float4x4 roll = make_float4x4_from_axis_angle(float3::unit_z(), rollRad);

                        float4x4 expected = roll * pitch * yaw;
                        float4x4 actual = make_float4x4_from_yaw_pitch_roll(yawRad, pitchRad, rollRad);
                        Assert::IsTrue(Equal(expected, actual));
                    }
                }
            }
        }

        TEST_METHOD(Float4x4CreateShadowTest01)
        {
            float3 lightDir = float3::unit_y();
            plane plane(float3::unit_y(), 0);

            float4x4 expected = make_float4x4_scale(1, 0, 1);

            float4x4 actual = make_float4x4_shadow(lightDir, plane);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_shadow did not returned expected value.");
        }
        
        TEST_METHOD(Float4x4CreateShadowTest02)
        {
            // Complex cases.
            plane planes[] =
            {
                plane( 0, 1, 0, 0 ),
                plane( 1, 2, 3, 4 ),
                plane( 5, 6, 7, 8 ),
                plane(-1,-2,-3,-4 ),
                plane(-5,-6,-7,-8 ),
            };

            float3 points[] =
            {
                float3( 1, 2, 3),
                float3( 5, 6, 7),
                float3( 8, 9, 10),
                float3(-1,-2,-3),
                float3(-5,-6,-7),
                float3(-8,-9,-10),
            };

            for (int i = 0; i < sizeof(planes) / sizeof(planes[0]); i++)
            {
                plane plane = normalize(planes[i]);

                // Try various direction of light directions.
                float3 testDirections[]
                {
                    float3( -1.0f, 1.0f, 1.0f ),
                    float3(  0.0f, 1.0f, 1.0f ),
                    float3(  1.0f, 1.0f, 1.0f ),
                    float3( -1.0f, 0.0f, 1.0f ),
                    float3(  0.0f, 0.0f, 1.0f ),
                    float3(  1.0f, 0.0f, 1.0f ),
                    float3( -1.0f,-1.0f, 1.0f ),
                    float3(  0.0f,-1.0f, 1.0f ),
                    float3(  1.0f,-1.0f, 1.0f ),

                    float3( -1.0f, 1.0f, 0.0f ),
                    float3(  0.0f, 1.0f, 0.0f ),
                    float3(  1.0f, 1.0f, 0.0f ),
                    float3( -1.0f, 0.0f, 0.0f ),
                    float3(  0.0f, 0.0f, 0.0f ),
                    float3(  1.0f, 0.0f, 0.0f ),
                    float3( -1.0f,-1.0f, 0.0f ),
                    float3(  0.0f,-1.0f, 0.0f ),
                    float3(  1.0f,-1.0f, 0.0f ),

                    float3( -1.0f, 1.0f,-1.0f ),
                    float3(  0.0f, 1.0f,-1.0f ),
                    float3(  1.0f, 1.0f,-1.0f ),
                    float3( -1.0f, 0.0f,-1.0f ),
                    float3(  0.0f, 0.0f,-1.0f ),
                    float3(  1.0f, 0.0f,-1.0f ),
                    float3( -1.0f,-1.0f,-1.0f ),
                    float3(  0.0f,-1.0f,-1.0f ),
                    float3(  1.0f,-1.0f,-1.0f ),
                };

                for (int j = 0; j < sizeof(testDirections) / sizeof(testDirections[0]); j++)
                {
                    float3 lightDirInfo = testDirections[j];

                    if (length(lightDirInfo) < 0.1f) continue;
                    float3 lightDir = normalize(lightDirInfo);

                    if (dot_normal(plane, lightDir) < 0.1f) continue;

                    float4x4 m = make_float4x4_shadow(lightDir, plane);
                    float3 pp = -plane.d * plane.normal; // origin of the plane.

                    for (int k = 0; k < sizeof(points) / sizeof(points[0]); k++)
                    {
                        float3 point = points[k];

                        float4 v4 = transform4(point, m);

                        float3 sp = float3(v4.x, v4.y, v4.z) / v4.w;

                        // Make sure transformed position is on the plane.
                        float3 v = sp - pp;
                        float d = dot(v, plane.normal);
                        Assert::IsTrue(Equal(d, 0), L"make_float4x4_shadow did not provide expected value.");

                        // make sure direction between transformed position and original position are same as light direction.
                        if (dot(point - pp, plane.normal) > 0.0001f)
                        {
                            float3 dir = normalize(point - sp);
                            Assert::IsTrue(Equal(dir, lightDir), L"make_float4x4_shadow did not provide expected value.");
                        }
                    }
                }
            }
        }

        void CreateReflectionTest(plane plane, float4x4 expected)
        {
            float4x4 actual = make_float4x4_reflection(plane);
            Assert::IsTrue(Equal(actual, expected), L"make_float4x4_reflection did not return expected value.");
        }

        TEST_METHOD(Float4x4CreateReflectionTest01)
        {
            // XY plane.
            CreateReflectionTest(plane(float3::unit_z(), 0), make_float4x4_scale(1, 1, -1));
            // XZ plane.
            CreateReflectionTest(plane(float3::unit_y(), 0), make_float4x4_scale(1, -1, 1));
            // YZ plane.
            CreateReflectionTest(plane(float3::unit_x(), 0), make_float4x4_scale(-1, 1, 1));

            // Complex cases.
            plane planes[] =
            {
                plane( 0, 1, 0, 0 ),
                plane( 1, 2, 3, 4 ),
                plane( 5, 6, 7, 8 ),
                plane(-1,-2,-3,-4 ),
                plane(-5,-6,-7,-8 ),
            };

            float3 points[] =
            {
                float3( 1, 2, 3),
                float3( 5, 6, 7),
                float3(-1,-2,-3),
                float3(-5,-6,-7),
            };

            for (int i = 0; i < sizeof(planes) / sizeof(planes[0]); i++)
            {
                plane plane = normalize(planes[i]);
                float4x4 m = make_float4x4_reflection(plane);
                float3 pp = -plane.d * plane.normal; // Position on the plane.

                for (int j = 0; j < sizeof(points) / sizeof(points[0]); j++)
                {
                    float3 point = points[j];

                    float3 rp = transform(point, m);

                    // Maniually compute refelction point and compare results.
                    float3 v = point - pp;
                    float d = dot(v, plane.normal);
                    float3 vp = point - 2.0f * d * plane.normal;
                    Assert::IsTrue(Equal(rp, vp), L"float4x4::Reflection did not provide expected value.");
                }
            }
        }

        // A test for rotation_z (float)
        TEST_METHOD(Float4x4CreateRotationZTest)
        {
            float radians = ToRadians(50.0f);

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.642787635f;
            expected.m12 = 0.766044438f;
            expected.m21 = -0.766044438f;
            expected.m22 = 0.642787635f;

            float4x4 actual;
            actual = make_float4x4_rotation_z(radians);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_rotation_z did not return the expected value.");
        }

        // A test for rotation_z (float, float3)
        TEST_METHOD(Float4x4CreateRotationZCenterTest)
        {
            float radians = ToRadians(30.0f);
            float3 center(23, 42, 66);

            float4x4 rotateAroundZero = make_float4x4_rotation_z(radians, float3::zero());
            float4x4 rotateAroundZeroExpected = make_float4x4_rotation_z(radians);
            Assert::IsTrue(Equal(rotateAroundZero, rotateAroundZeroExpected));

            float4x4 rotateAroundCenter = make_float4x4_rotation_z(radians, center);
            float4x4 rotateAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_rotation_z(radians) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(rotateAroundCenter, rotateAroundCenterExpected));
        }

        // A test for look_at (float3, float3, float3)
        TEST_METHOD(Float4x4CreateLookAtTest)
        {
            float3 cameraPosition(10.0f, 20.0f, 30.0f);
            float3 cameraTarget(3.0f, 2.0f, -4.0f);
            float3 cameraUpVector(0.0f, 1.0f, 0.0f);

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.979457f;
            expected.m12 = -0.0928267762f;
            expected.m13 = 0.179017f;

            expected.m21 = 0.0f;
            expected.m22 = 0.8877481f;
            expected.m23 = 0.460329473f;

            expected.m31 = -0.201652914f;
            expected.m32 = -0.450872928f;
            expected.m33 = 0.8695112f;

            expected.m41 = -3.74498272f;
            expected.m42 = -3.30050683f;
            expected.m43 = -37.0820961f;

            float4x4 actual = make_float4x4_look_at(cameraPosition, cameraTarget, cameraUpVector);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_look_at did not return the expected value.");
        }

        // A test for world (float3, float3, float3)
        TEST_METHOD(Float4x4CreateWorldTest)
        {
            float3 objectPosition(10.0f, 20.0f, 30.0f);
            float3 objectForwardDirection(3.0f, 2.0f, -4.0f);
            float3 objectUpVector(0.0f, 1.0f, 0.0f);

            float4x4 expected;
            expected.m11 = 0.799999952f;
            expected.m12 = 0;
            expected.m13 = 0.599999964f;
            expected.m14 = 0;

            expected.m21 = -0.2228344f;
            expected.m22 = 0.928476632f;
            expected.m23 = 0.297112525f;
            expected.m24 = 0;

            expected.m31 = -0.557086f;
            expected.m32 = -0.371390671f;
            expected.m33 = 0.742781341f;
            expected.m34 = 0;

            expected.m41 = 10;
            expected.m42 = 20;
            expected.m43 = 30;
            expected.m44 = 1.0f;

            float4x4 actual = make_float4x4_world(objectPosition, objectForwardDirection, objectUpVector);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_world did not return the expected value.");

            Assert::AreEqual(objectPosition, translation(actual));
            Assert::IsTrue(dot(normalize(objectUpVector), float3(actual.m21, actual.m22, actual.m23)) > 0);
            Assert::IsTrue(dot(normalize(objectForwardDirection), float3(-actual.m31, -actual.m32, -actual.m33)) > 0.999f);
        }

        // A test for orthographic (float, float, float, float)
        TEST_METHOD(Float4x4CreateOrthoTest)
        {
            float width = 100.0f;
            float height = 200.0f;
            float zNearPlane = 1.5f;
            float zFarPlane = 1000.0f;

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.02f;
            expected.m22 = 0.01f;
            expected.m33 = -0.00100150227f;
            expected.m43 = -0.00150225335f;

            float4x4 actual;
            actual = make_float4x4_orthographic(width, height, zNearPlane, zFarPlane);
            Assert::IsTrue(Equal(expected, actual), L"float4x4::CreateOrtho did not return the expected value.");
        }

        // A test for orthographic_off_center (float, float, float, float, float, float)
        TEST_METHOD(Float4x4CreateOrthoOffCenterTest)
        {
            float left = 10.0f;
            float right = 90.0f;
            float bottom = 20.0f;
            float top = 180.0f;
            float zNearPlane = 1.5f;
            float zFarPlane = 1000.0f;

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.025f;
            expected.m22 = 0.0125f;
            expected.m33 = -0.00100150227f;
            expected.m41 = -1.25f;
            expected.m42 = -1.25f;
            expected.m43 = -0.00150225335f;

            float4x4 actual;
            actual = make_float4x4_orthographic_off_center(left, right, bottom, top, zNearPlane, zFarPlane);
            Assert::IsTrue(Equal(expected, actual), L"float4x4::CreateOrthoOffCenter did not return the expected value.");
        }

        // A test for perspective (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveTest)
        {
            float width = 100.0f;
            float height = 200.0f;
            float zNearPlane = 1.5f;
            float zFarPlane = 1000.0f;

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.03f;
            expected.m22 = 0.015f;
            expected.m33 = -1.00150228f;
            expected.m34 = -1.0f;
            expected.m43 = -1.50225341f;
            expected.m44 = 0;

            float4x4 actual;
            actual = make_float4x4_perspective(width, height, zNearPlane, zFarPlane);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_perspective did not return the expected value.");
        }

        // A test for perspective (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveTest1)
        {
            try
            {
                float width = 100.0f;
                float height = 200.0f;
                float zNearPlane = 0.0f;
                float zFarPlane = 0.0f;

                float4x4 actual = make_float4x4_perspective(width, height, zNearPlane, zFarPlane);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveTest2)
        {
            try
            {
                float4x4 actual = make_float4x4_perspective(10, 10, -10, 10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveTest3)
        {
            try
            {
                float4x4 actual = make_float4x4_perspective(10, 10, 10, -10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveTest4)
        {
            try
            {
                float4x4 actual = make_float4x4_perspective(10, 10, 10, 1);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest)
        {
            float fieldOfView = ToRadians(30.0f);
            float aspectRatio = 1280.0f / 720.0f;
            float zNearPlane = 1.5f;
            float zFarPlane = 1000.0f;

            float4x4 expected = float4x4::identity();
            expected.m11 = 2.09927845f;
            expected.m22 = 3.73205066f;
            expected.m33 = -1.00150228f;
            expected.m34 = -1.0f;
            expected.m43 = -1.50225341f;
            expected.m44 = 0;
            float4x4 actual;

            actual = make_float4x4_perspective_field_of_view(fieldOfView, aspectRatio, zNearPlane, zFarPlane);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_perspective_field_of_view did not return the expected value.");
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest1)
        {
            try
            {
                float4x4 mtx = make_float4x4_perspective_field_of_view(-1, 1, 1, 10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest2)
        {
            try
            {
                float4x4 mtx = make_float4x4_perspective_field_of_view(DirectX::XM_PI + 0.01f, 1, 1, 10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest3)
        {
            try
            {
                float4x4 mtx = make_float4x4_perspective_field_of_view(DirectX::XM_PIDIV4, 1, -1, 10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest4)
        {
            try
            {
                float4x4 mtx = make_float4x4_perspective_field_of_view(DirectX::XM_PIDIV4, 1, 1, -10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_field_of_view (float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveFieldOfViewTest5)
        {
            try
            {
                float4x4 mtx = make_float4x4_perspective_field_of_view(DirectX::XM_PIDIV4, 1, 10, 1);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_off_center (float, float, float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveOffCenterTest)
        {
            float left = 10.0f;
            float right = 90.0f;
            float bottom = 20.0f;
            float top = 180.0f;
            float zNearPlane = 1.5f;
            float zFarPlane = 1000.0f;

            float4x4 expected = float4x4::identity();
            expected.m11 = 0.0375f;
            expected.m22 = 0.01875f;
            expected.m31 = 1.25f;
            expected.m32 = 1.25f;
            expected.m33 = -1.00150228f;
            expected.m34 = -1.0f;
            expected.m43 = -1.50225341f;
            expected.m44 = 0;

            float4x4 actual;
            actual = make_float4x4_perspective_off_center(left, right, bottom, top, zNearPlane, zFarPlane);
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_perspective_off_center did not return the expected value.");
        }

        // A test for perspective_off_center (float, float, float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveOffCenterTest1)
        {
            try
            {
                float left = 10.0f, right = 90.0f, bottom = 20.0f, top = 180.0f;
                float4x4 actual = make_float4x4_perspective_off_center(left, right, bottom, top, -1, 10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_off_center (float, float, float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveOffCenterTest2)
        {
            try
            {
                float left = 10.0f, right = 90.0f, bottom = 20.0f, top = 180.0f;
                float4x4 actual = make_float4x4_perspective_off_center(left, right, bottom, top, 1, -10);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for perspective_off_center (float, float, float, float, float, float)
        TEST_METHOD(Float4x4CreatePerspectiveOffCenterTest3)
        {
            try
            {
                float left = 10.0f, right = 90.0f, bottom = 20.0f, top = 180.0f;
                float4x4 actual = make_float4x4_perspective_off_center(left, right, bottom, top, 10, 1);

                Assert::Fail(L"should have thrown");
            }
            catch (std::invalid_argument const&)
            {
            }
        }

        // A test for invert (float4x4)
        TEST_METHOD(Float4x4InvertTest1)
        {
            float4x4 a;
            a.m11 = 1.0f; a.m12 = 2.0f; a.m13 = 3.0f; a.m14 = 4.0f;
            a.m21 = 5.0f; a.m22 = 6.0f; a.m23 = 7.0f; a.m24 = 8.0f;
            a.m31 = 9.0f; a.m32 = 10.0f; a.m33 = 11.0f; a.m34 = 12.0f;
            a.m41 = 13.0f; a.m42 = 14.0f; a.m43 = 15.0f; a.m44 = 16.0f;

            float detA = determinant(a);
            Assert::IsTrue(Equal(detA, 0.0f), L"invert did not return the expected value.");

            float4x4 actual;
            Assert::IsFalse(invert(a, &actual));

            // all the elements in Actual is NaN
            Assert::IsTrue(
                isnan(actual.m11) && isnan(actual.m12) && isnan(actual.m13) && isnan(actual.m14) &&
                isnan(actual.m21) && isnan(actual.m22) && isnan(actual.m23) && isnan(actual.m24) &&
                isnan(actual.m31) && isnan(actual.m32) && isnan(actual.m33) && isnan(actual.m34) &&
                isnan(actual.m41) && isnan(actual.m42) && isnan(actual.m43) && isnan(actual.m44)
                , L"invert did not return the expected value.");
        }

        // A test for lerp (float4x4, float4x4, float)
        TEST_METHOD(Float4x4LerpTest)
        {
            float4x4 a;
            a.m11 = 11.0f; a.m12 = 12.0f; a.m13 = 13.0f; a.m14 = 14.0f;
            a.m21 = 21.0f; a.m22 = 22.0f; a.m23 = 23.0f; a.m24 = 24.0f;
            a.m31 = 31.0f; a.m32 = 32.0f; a.m33 = 33.0f; a.m34 = 34.0f;
            a.m41 = 41.0f; a.m42 = 42.0f; a.m43 = 43.0f; a.m44 = 44.0f;

            float4x4 b = GenerateMatrixNumberFrom1To16();

            float t = 0.5f;

            float4x4 expected;
            expected.m11 = a.m11 + (b.m11 - a.m11) * t;
            expected.m12 = a.m12 + (b.m12 - a.m12) * t;
            expected.m13 = a.m13 + (b.m13 - a.m13) * t;
            expected.m14 = a.m14 + (b.m14 - a.m14) * t;

            expected.m21 = a.m21 + (b.m21 - a.m21) * t;
            expected.m22 = a.m22 + (b.m22 - a.m22) * t;
            expected.m23 = a.m23 + (b.m23 - a.m23) * t;
            expected.m24 = a.m24 + (b.m24 - a.m24) * t;

            expected.m31 = a.m31 + (b.m31 - a.m31) * t;
            expected.m32 = a.m32 + (b.m32 - a.m32) * t;
            expected.m33 = a.m33 + (b.m33 - a.m33) * t;
            expected.m34 = a.m34 + (b.m34 - a.m34) * t;

            expected.m41 = a.m41 + (b.m41 - a.m41) * t;
            expected.m42 = a.m42 + (b.m42 - a.m42) * t;
            expected.m43 = a.m43 + (b.m43 - a.m43) * t;
            expected.m44 = a.m44 + (b.m44 - a.m44) * t;

            float4x4 actual;
            actual = lerp(a, b, t);
            Assert::IsTrue(Equal(expected, actual), L"lerp did not return the expected value.");
        }

        // A test for operator - (float4x4)
        TEST_METHOD(Float4x4UnaryNegationTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();

            float4x4 expected;
            expected.m11 = -1.0f; expected.m12 = -2.0f; expected.m13 = -3.0f; expected.m14 = -4.0f;
            expected.m21 = -5.0f; expected.m22 = -6.0f; expected.m23 = -7.0f; expected.m24 = -8.0f;
            expected.m31 = -9.0f; expected.m32 = -10.0f; expected.m33 = -11.0f; expected.m34 = -12.0f;
            expected.m41 = -13.0f; expected.m42 = -14.0f; expected.m43 = -15.0f; expected.m44 = -16.0f;

            float4x4 actual = -a;
            Assert::IsTrue(Equal(expected, actual), L"float4x4::operator - did not return the expected value.");
        }

        // A test for operator - (float4x4, float4x4)
        TEST_METHOD(Float4x4SubtractionTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();
            float4x4 b = GenerateMatrixNumberFrom1To16();
            float4x4 expected(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

            float4x4 actual = a - b;
            Assert::IsTrue(Equal(expected, actual), L"float4x4::operator - did not return the expected value.");
        }

        // A test for operator * (float4x4, float4x4)
        TEST_METHOD(Float4x4MultiplyTest1)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();
            float4x4 b = GenerateMatrixNumberFrom1To16();

            float4x4 expected;
            expected.m11 = a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41;
            expected.m12 = a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42;
            expected.m13 = a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43;
            expected.m14 = a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44;

            expected.m21 = a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41;
            expected.m22 = a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42;
            expected.m23 = a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43;
            expected.m24 = a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44;

            expected.m31 = a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41;
            expected.m32 = a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42;
            expected.m33 = a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43;
            expected.m34 = a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44;

            expected.m41 = a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41;
            expected.m42 = a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42;
            expected.m43 = a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43;
            expected.m44 = a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44;

            float4x4 actual = a * b;
            Assert::IsTrue(Equal(expected, actual), L"float4x4::operator * did not return the expected value.");
        }

        // A test for operator * (float4x4, float4x4)
        TEST_METHOD(Float4x4MultiplyTest4)
        {
            float4x4 a;
            a.m11 = 1.0f; a.m12 = 2.0f; a.m13 = 3.0f; a.m14 = 4.0f;
            a.m21 = 5.0f; a.m22 = -6.0f; a.m23 = 7.0f; a.m24 = -8.0f;
            a.m31 = 9.0f; a.m32 = 10.0f; a.m33 = 11.0f; a.m34 = 12.0f;
            a.m41 = 13.0f; a.m42 = -14.0f; a.m43 = 15.0f; a.m44 = -16.0f;

            float4x4 b;
            b = float4x4::identity();

            float4x4 expected = a;
            float4x4 actual = a * b;

            Assert::IsTrue(Equal(expected, actual), L"float4x4::operator * did not return the expected value.");
        }

        // A test for operator + (float4x4, float4x4)
        TEST_METHOD(Float4x4AdditionTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();
            float4x4 b = GenerateMatrixNumberFrom1To16();

            float4x4 expected;
            expected.m11 = a.m11 + b.m11; expected.m12 = a.m12 + b.m12; expected.m13 = a.m13 + b.m13; expected.m14 = a.m14 + b.m14;
            expected.m21 = a.m21 + b.m21; expected.m22 = a.m22 + b.m22; expected.m23 = a.m23 + b.m23; expected.m24 = a.m24 + b.m24;
            expected.m31 = a.m31 + b.m31; expected.m32 = a.m32 + b.m32; expected.m33 = a.m33 + b.m33; expected.m34 = a.m34 + b.m34;
            expected.m41 = a.m41 + b.m41; expected.m42 = a.m42 + b.m42; expected.m43 = a.m43 + b.m43; expected.m44 = a.m44 + b.m44;

            float4x4 actual;

            actual = a + b;

            Assert::IsTrue(Equal(expected, actual), L"float4x4::operator + did not return the expected value.");
        }

        // A test for transpose (float4x4)
        TEST_METHOD(Float4x4TransposeTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();

            float4x4 expected;
            expected.m11 = a.m11; expected.m12 = a.m21; expected.m13 = a.m31; expected.m14 = a.m41;
            expected.m21 = a.m12; expected.m22 = a.m22; expected.m23 = a.m32; expected.m24 = a.m42;
            expected.m31 = a.m13; expected.m32 = a.m23; expected.m33 = a.m33; expected.m34 = a.m43;
            expected.m41 = a.m14; expected.m42 = a.m24; expected.m43 = a.m34; expected.m44 = a.m44;

            float4x4 actual = transpose(a);
            Assert::IsTrue(Equal(expected, actual), L"transpose did not return the expected value.");
        }

        // A test for transpose (float4x4)
        TEST_METHOD(Float4x4TransposeTest1)
        {
            float4x4 a = float4x4::identity();
            float4x4 expected = float4x4::identity();

            float4x4 actual = transpose(a);
            Assert::IsTrue(Equal(expected, actual), L"transpose did not return the expected value.");
        }

        // A test for float4x4 (quaternion)
        TEST_METHOD(Float4x4FromQuaternionTest1)
        {
            float3 axis = normalize(float3(1.0f, 2.0f, 3.0f));
            quaternion q = make_quaternion_from_axis_angle(axis, ToRadians(30.0f));

            float4x4 expected;
            expected.m11 = 0.875595033f;
            expected.m12 = 0.420031041f;
            expected.m13 = -0.2385524f;
            expected.m14 = 0.0f;

            expected.m21 = -0.38175258f;
            expected.m22 = 0.904303849f;
            expected.m23 = 0.1910483f;
            expected.m24 = 0.0f;

            expected.m31 = 0.295970082f;
            expected.m32 = -0.07621294f;
            expected.m33 = 0.952151954f;
            expected.m34 = 0.0f;

            expected.m41 = 0.0f;
            expected.m42 = 0.0f;
            expected.m43 = 0.0f;
            expected.m44 = 1.0f;

            float4x4 target = make_float4x4_from_quaternion(q);
            Assert::IsTrue(Equal(expected, target), L"float4x4::float4x4(quaternion) did not return the expected value.");
        }

        // A test for from_quaternion (float4x4)
        TEST_METHOD(Float4x4FromQuaternionTest2)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                quaternion quat = make_quaternion_from_axis_angle(float3::unit_x(), angle);

                float4x4 expected = make_float4x4_rotation_x(angle);
                float4x4 actual = make_float4x4_from_quaternion(quat);
                Assert::IsTrue(Equal(expected, actual),
                    L"quaternion::FromQuaternion did not return the expected value.");

                // make sure convert back to quaternion is same as we passed quaternion::
                quaternion q2 = make_quaternion_from_rotation_matrix(actual);
                Assert::IsTrue(EqualRotation(quat, q2),
                    L"quaternion::FromQuaternion did not return the expected value.");
            }
        }

        // A test for from_quaternion (float4x4)
        TEST_METHOD(Float4x4FromQuaternionTest3)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                quaternion quat = make_quaternion_from_axis_angle(float3::unit_y(), angle);

                float4x4 expected = make_float4x4_rotation_y(angle);
                float4x4 actual = make_float4x4_from_quaternion(quat);
                Assert::IsTrue(Equal(expected, actual),
                    L"quaternion::FromQuaternion did not return the expected value.");

                // make sure convert back to quaternion is same as we passed quaternion::
                quaternion q2 = make_quaternion_from_rotation_matrix(actual);
                Assert::IsTrue(EqualRotation(quat, q2),
                    L"quaternion::FromQuaternion did not return the expected value.");
            }
        }

        // A test for from_quaternion (float4x4)
        TEST_METHOD(Float4x4FromQuaternionTest4)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                quaternion quat = make_quaternion_from_axis_angle(float3::unit_z(), angle);

                float4x4 expected = make_float4x4_rotation_z(angle);
                float4x4 actual = make_float4x4_from_quaternion(quat);
                Assert::IsTrue(Equal(expected, actual),
                    L"quaternion::FromQuaternion did not return the expected value.");

                // make sure convert back to quaternion is same as we passed quaternion::
                quaternion q2 = make_quaternion_from_rotation_matrix(actual);
                Assert::IsTrue(EqualRotation(quat, q2),
                    L"quaternion::FromQuaternion did not return the expected value.");
            }
        }

        // A test for from_quaternion (float4x4)
        TEST_METHOD(Float4x4FromQuaternionTest5)
        {
            for (float angle = 0.0f; angle < 720.0f; angle += 10.0f)
            {
                quaternion quat =
                    make_quaternion_from_axis_angle(float3::unit_z(), angle) *
                    make_quaternion_from_axis_angle(float3::unit_y(), angle) *
                    make_quaternion_from_axis_angle(float3::unit_x(), angle);

                float4x4 expected =
                    make_float4x4_rotation_x(angle) *
                    make_float4x4_rotation_y(angle) *
                    make_float4x4_rotation_z(angle);
                float4x4 actual = make_float4x4_from_quaternion(quat);
                Assert::IsTrue(Equal(expected, actual),
                    L"quaternion::FromQuaternion did not return the expected value.");

                // make sure convert back to quaternion is same as we passed quaternion::
                quaternion q2 = make_quaternion_from_rotation_matrix(actual);
                Assert::IsTrue(EqualRotation(quat, q2),
                    L"quaternion::FromQuaternion did not return the expected value.");
            }
        }

        // A test for operator != (float4x4, float4x4)
        TEST_METHOD(Float4x4InequalityTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();
            float4x4 b = GenerateMatrixNumberFrom1To16();

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"float4x4::operator != did not return the expected value.");

            // case 2: compare between different values
            b.m11 = 11.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"float4x4::operator != did not return the expected value.");
        }

        // A test for operator == (float4x4, float4x4)
        TEST_METHOD(Float4x4EqualityTest)
        {
            float4x4 a = GenerateMatrixNumberFrom1To16();
            float4x4 b = GenerateMatrixNumberFrom1To16();

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"float4x4::operator == did not return the expected value.");

            // case 2: compare between different values
            b.m11 = 11.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"float4x4::operator == did not return the expected value.");
        }

        // A test for operator +=
        TEST_METHOD(Float4x4OperatorAddEqualsTest)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            float4x4 b(4, 7, 6, 1, 8, 2, 5, 6, 9, 5, 2, 4, 6, 8, 4, 1);
            float4x4 expected = a + b;
            
            // In-place += operation.
            float4x4& ret = a += b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b + b;
            b += b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator -=
        TEST_METHOD(Float4x4OperatorSubtractEqualsTest)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            float4x4 b(4, 7, 6, 1, 8, 2, 5, 6, 9, 5, 2, 4, 6, 8, 4, 1);
            float4x4 expected = a - b;
            
            // In-place -= operation.
            float4x4& ret = a -= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b - b;
            b -= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float4x4OperatorMultiplyEqualsTest)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            float4x4 b(4, 7, 6, 1, 8, 2, 5, 6, 9, 5, 2, 4, 6, 8, 4, 1);
            float4x4 expected = a * b;
            
            // In-place *= operation.
            float4x4& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
            
            // Pointer aliasing where object is applied to itself.
            expected = b * b;
            b *= b;
            Assert::AreEqual(b, expected);
        }

        // A test for operator *=
        TEST_METHOD(Float4x4OperatorMultiplyEqualsTest2)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            float b = 23;
            float4x4 expected = a * b;
            
            // In-place *= operation.
            float4x4& ret = a *= b;
            
            Assert::AreEqual(a, expected);
            Assert::AreEqual(&a, &ret);
        }

        void CreateBillboardTestMethod(float3 placeDirection, float3 cameraUpVector, float4x4 expectedRotation)
        {
            float3 cameraPosition(3.0f, 4.0f, 5.0f);
            float3 objectPosition = cameraPosition + placeDirection * 10.0f;
            float4x4 expected = expectedRotation * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_billboard(objectPosition, cameraPosition, cameraUpVector, float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_billboard did not return the expected value.");
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest01)
        {
            // Object placed at Forward of camera. result must be same as 180 degrees rotate along y-axis.
            CreateBillboardTestMethod(float3(0, 0, -1), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(180.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest02)
        {
            // Object placed at Backward of camera. This result must be same as 0 degrees rotate along y-axis.
            CreateBillboardTestMethod(float3(0, 0, 1), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(0)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest03)
        {
            // Place object at Right side of camera. This result must be same as 90 degrees rotate along y-axis.
            CreateBillboardTestMethod(float3(1, 0, 0), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(90)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest04)
        {
            // Place object at Left side of camera. This result must be same as -90 degrees rotate along y-axis.
            CreateBillboardTestMethod(float3(-1, 0, 0), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(-90)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest05)
        {
            // Place object at Up side of camera. result must be same as 180 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateBillboardTestMethod(float3(0, 1, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(180)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest06)
        {
            // Place object at Down side of camera. result must be same as 0 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateBillboardTestMethod(float3(0, -1, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(0)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest07)
        {
            // Place object at Right side of camera. result must be same as 90 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateBillboardTestMethod(float3(1, 0, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(90.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest08)
        {
            // Place object at Left side of camera. result must be same as -90 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateBillboardTestMethod(float3(-1, 0, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(-90.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest09)
        {
            // Place object at Up side of camera. result must be same as -90 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateBillboardTestMethod(float3(0, 1, 0), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(-90.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest10)
        {
            // Place object at Down side of camera. result must be same as 90 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateBillboardTestMethod(float3(0, -1, 0), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(90.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest11)
        {
            // Place object at Forward side of camera. result must be same as 180 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateBillboardTestMethod(float3(0, 0, -1), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(180.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTest12)
        {
            // Place object at Backward side of camera. result must be same as 0 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateBillboardTestMethod(float3(0, 0, 1), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(0.0f)));
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTooCloseTest1)
        {
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 cameraPosition = objectPosition;
            float3 cameraUpVector(0, 1, 0);

            // Doesn't pass camera face direction. billboard uses float3(0, 0, -1) direction. Result must be same as 180 degrees rotate along y-axis.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(180.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_billboard(objectPosition, cameraPosition, cameraUpVector, float3(0, 0, 1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_billboard did not return the expected value.");
        }

        // A test for billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateBillboardTooCloseTest2)
        {
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 cameraPosition = objectPosition;
            float3 cameraUpVector(0, 1, 0);

            // Passes float3::Rgiht as camera face direction. Result must be same as -90 degrees rotate along y-axis.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(-90.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_billboard(objectPosition, cameraPosition, cameraUpVector, float3(1, 0, 0));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_billboard did not return the expected value.");
        }

        void CreateConstrainedBillboardTestMethod(float3 placeDirection, float3 rotateAxis, float4x4 expectedRotation)
        {
            float3 cameraPosition(3.0f, 4.0f, 5.0f);
            float3 objectPosition = cameraPosition + placeDirection * 10.0f;
            float4x4 expected = expectedRotation * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");

            // When you move camera along rotateAxis, result must be same.
            cameraPosition += rotateAxis * 10.0f;
            actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");

            cameraPosition -= rotateAxis * 30.0f;
            actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest01)
        {
            // Object placed at Forward of camera. result must be same as 180 degrees rotate along y-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 0, -1), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(180.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest02)
        {
            // Object placed at Backward of camera. This result must be same as 0 degrees rotate along y-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 0, 1), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(0)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest03)
        {
            // Place object at Right side of camera. This result must be same as 90 degrees rotate along y-axis.
            CreateConstrainedBillboardTestMethod(float3(1, 0, 0), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(90)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest04)
        {
            // Place object at Left side of camera. This result must be same as -90 degrees rotate along y-axis.
            CreateConstrainedBillboardTestMethod(float3(-1, 0, 0), float3(0, 1, 0), make_float4x4_rotation_y(ToRadians(-90)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest05)
        {
            // Place object at Up side of camera. result must be same as 180 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 1, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(180)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest06)
        {
            // Place object at Down side of camera. result must be same as 0 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateConstrainedBillboardTestMethod(float3(0, -1, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(0)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest07)
        {
            // Place object at Right side of camera. result must be same as 90 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateConstrainedBillboardTestMethod(float3(1, 0, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(90.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest08)
        {
            // Place object at Left side of camera. result must be same as -90 degrees rotate along z-axis after 90 degrees rotate along x-axis.
            CreateConstrainedBillboardTestMethod(float3(-1, 0, 0), float3(0, 0, 1),
                make_float4x4_rotation_x(ToRadians(90.0f)) * make_float4x4_rotation_z(ToRadians(-90.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest09)
        {
            // Place object at Up side of camera. result must be same as -90 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 1, 0), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(-90.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest10)
        {
            // Place object at Down side of camera. result must be same as 90 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateConstrainedBillboardTestMethod(float3(0, -1, 0), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(90.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest11)
        {
            // Place object at Forward side of camera. result must be same as 180 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 0, -1), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(180.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTest12)
        {
            // Place object at Backward side of camera. result must be same as 0 degrees rotate along x-axis after 90 degrees rotate along z-axis.
            CreateConstrainedBillboardTestMethod(float3(0, 0, 1), float3(-1, 0, 0),
                make_float4x4_rotation_z(ToRadians(90.0f)) * make_float4x4_rotation_x(ToRadians(0.0f)));
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTooCloseTest1)
        {
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 cameraPosition = objectPosition;
            float3 cameraUpVector(0, 1, 0);

            // Doesn't pass camera face direction. constrained_billboard uses float3(0, 0, -1) direction. Result must be same as 180 degrees rotate along y-axis.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(180.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, cameraUpVector, float3(0, 0, 1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardTooCloseTest2)
        {
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 cameraPosition = objectPosition;
            float3 cameraUpVector(0, 1, 0);

            // Passes float3::Rgiht as camera face direction. Result must be same as -90 degrees rotate along y-axis.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(-90.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, cameraUpVector, float3(1, 0, 0), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardAlongAxisTest1)
        {
            // Place camera at up side of object.
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 rotateAxis(0, 1, 0);
            float3 cameraPosition = objectPosition + rotateAxis * 10.0f;

            // In this case, constrained_billboard picks float3(0, 0, -1) as object forward vector.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(180.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardAlongAxisTest2)
        {
            // Place camera at up side of object.
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 rotateAxis(0, 0, -1);
            float3 cameraPosition = objectPosition + rotateAxis * 10.0f;

            // In this case, constrained_billboard picks float3(1, 0, 0) as object forward vector.
            float4x4 expected = make_float4x4_rotation_x(ToRadians(-90.0f)) * make_float4x4_rotation_z(ToRadians(-90.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardAlongAxisTest3)
        {
            // Place camera at up side of object.
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 rotateAxis(0, 1, 0);
            float3 cameraPosition = objectPosition + rotateAxis * 10.0f;

            // User passes correct objectForwardVector.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(180.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardAlongAxisTest4)
        {
            // Place camera at up side of object.
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 rotateAxis(0, 1, 0);
            float3 cameraPosition = objectPosition + rotateAxis * 10.0f;

            // User passes correct objectForwardVector.
            float4x4 expected = make_float4x4_rotation_y(ToRadians(180.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 1, 0));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for constrained_billboard (float3, float3, float3, float3?)
        TEST_METHOD(Float4x4CreateConstrainedBillboardAlongAxisTest5)
        {
            // Place camera at up side of object.
            float3 objectPosition(3.0f, 4.0f, 5.0f);
            float3 rotateAxis(0, 0, -1);
            float3 cameraPosition = objectPosition + rotateAxis * 10.0f;

            // In this case, constrained_billboard picks float3::Right as object forward vector.
            float4x4 expected = make_float4x4_rotation_x(ToRadians(-90.0f)) * make_float4x4_rotation_z(ToRadians(-90.0f)) * make_float4x4_translation(objectPosition);
            float4x4 actual = make_float4x4_constrained_billboard(objectPosition, cameraPosition, rotateAxis, float3(0, 0, -1), float3(0, 0, -1));
            Assert::IsTrue(Equal(expected, actual), L"make_float4x4_constrained_billboard did not return the expected value.");
        }

        // A test for scale (float3)
        TEST_METHOD(Float4x4CreateScaleTest1)
        {
            float3 scales(2.0f, 3.0f, 4.0f);
            float4x4 expected(
                2.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 3.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            float4x4 actual = make_float4x4_scale(scales);
            Assert::AreEqual(expected, actual, L"make_float4x4_scale did not return the expected value.");
        }

        // A test for scale (float3, float3)
        TEST_METHOD(Float4x4CreateScaleCenterTest1)
        {
            float3 scale(3, 4, 5);
            float3 center(23, 42, 666);

            float4x4 scaleAroundZero = make_float4x4_scale(scale, float3::zero());
            float4x4 scaleAroundZeroExpected = make_float4x4_scale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float4x4 scaleAroundCenter = make_float4x4_scale(scale, center);
            float4x4 scaleAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_scale(scale) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for scale (float)
        TEST_METHOD(Float4x4CreateScaleTest2)
        {
            float scale = 2.0f;
            float4x4 expected(
                2.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 2.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            float4x4 actual = make_float4x4_scale(scale);
            Assert::AreEqual(expected, actual, L"make_float4x4_scale did not return the expected value.");
        }

        // A test for scale (float, float3)
        TEST_METHOD(Float4x4CreateScaleCenterTest2)
        {
            float scale = 5;
            float3 center(23, 42, 666);

            float4x4 scaleAroundZero = make_float4x4_scale(scale, float3::zero());
            float4x4 scaleAroundZeroExpected = make_float4x4_scale(scale);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float4x4 scaleAroundCenter = make_float4x4_scale(scale, center);
            float4x4 scaleAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_scale(scale) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for scale (float, float, float)
        TEST_METHOD(Float4x4CreateScaleTest3)
        {
            float xScale = 2.0f;
            float yScale = 3.0f;
            float zScale = 4.0f;
            float4x4 expected(
                2.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 3.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            float4x4 actual = make_float4x4_scale(xScale, yScale, zScale);
            Assert::AreEqual(expected, actual, L"make_float4x4_scale did not return the expected value.");
        }

        // A test for scale (float, float, float, float3)
        TEST_METHOD(Float4x4CreateScaleCenterTest3)
        {
            float3 scale(3, 4, 5);
            float3 center(23, 42, 666);

            float4x4 scaleAroundZero = make_float4x4_scale(scale.x, scale.y, scale.z, float3::zero());
            float4x4 scaleAroundZeroExpected = make_float4x4_scale(scale.x, scale.y, scale.z);
            Assert::IsTrue(Equal(scaleAroundZero, scaleAroundZeroExpected));

            float4x4 scaleAroundCenter = make_float4x4_scale(scale.x, scale.y, scale.z, center);
            float4x4 scaleAroundCenterExpected = make_float4x4_translation(-center) * make_float4x4_scale(scale.x, scale.y, scale.z) * make_float4x4_translation(center);
            Assert::IsTrue(Equal(scaleAroundCenter, scaleAroundCenterExpected));
        }

        // A test for translation (float3)
        TEST_METHOD(Float4x4CreateTranslationTest1)
        {
            float3 position(2.0f, 3.0f, 4.0f);
            float4x4 expected(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                2.0f, 3.0f, 4.0f, 1.0f);

            float4x4 actual = make_float4x4_translation(position);
            Assert::AreEqual(expected, actual, L"make_float4x4_translation did not return the expected value.");
        }

        // A test for translation (float, float, float)
        TEST_METHOD(Float4x4CreateTranslationTest2)
        {
            float xPosition = 2.0f;
            float yPosition = 3.0f;
            float zPosition = 4.0f;

            float4x4 expected(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                2.0f, 3.0f, 4.0f, 1.0f);

            float4x4 actual = make_float4x4_translation(xPosition, yPosition, zPosition);
            Assert::AreEqual(expected, actual, L"make_float4x4_translation did not return the expected value.");
        }

        // A test for translation()
        TEST_METHOD(Float4x4TranslationTest)
        {
            float4x4 a = GenerateTestMatrix();

            // Transfomed vector that has same semantics of property must be same.
            float3 val(a.m41, a.m42, a.m43);
            Assert::AreEqual(val, translation(a), L"translation was not set correctly.");
        }

        // A test for is_identity()
        TEST_METHOD(Float4x4IsIdentityTest)
        {
            Assert::IsTrue(is_identity(float4x4::identity()));
            Assert::IsTrue(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1)));
            Assert::IsFalse(is_identity(float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0)));
        }

        // A test for float4x4 (float3x2)
        TEST_METHOD(Float4x4From3x2Test)
        {
            float3x2 source(1, 2, 3, 4, 5, 6);
            float4x4 result(source);

            Assert::AreEqual(source.m11, result.m11);
            Assert::AreEqual(source.m12, result.m12);
            Assert::AreEqual(0.0f, result.m13);
            Assert::AreEqual(0.0f, result.m14);

            Assert::AreEqual(source.m21, result.m21);
            Assert::AreEqual(source.m22, result.m22);
            Assert::AreEqual(0.0f, result.m23);
            Assert::AreEqual(0.0f, result.m24);

            Assert::AreEqual(0.0f, result.m31);
            Assert::AreEqual(0.0f, result.m32);
            Assert::AreEqual(1.0f, result.m33);
            Assert::AreEqual(0.0f, result.m34);

            Assert::AreEqual(source.m31, result.m41);
            Assert::AreEqual(source.m32, result.m42);
            Assert::AreEqual(0.0f, result.m43);
            Assert::AreEqual(1.0f, result.m44);
        }

        // A test for float4x4 comparison involving NaN values
        TEST_METHOD(Float4x4EqualsNanTest)
        {
            float4x4 a(NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 b(0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 c(0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 d(0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 e(0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 f(0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 g(0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 h(0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0);
            float4x4 i(0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0, 0);
            float4x4 j(0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0, 0);
            float4x4 k(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0, 0);
            float4x4 l(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0, 0);
            float4x4 m(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0, 0);
            float4x4 n(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0, 0);
            float4x4 o(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN, 0);
            float4x4 p(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NAN);

            Assert::IsFalse(a == float4x4());
            Assert::IsFalse(b == float4x4());
            Assert::IsFalse(c == float4x4());
            Assert::IsFalse(d == float4x4());
            Assert::IsFalse(e == float4x4());
            Assert::IsFalse(f == float4x4());
            Assert::IsFalse(g == float4x4());
            Assert::IsFalse(h == float4x4());
            Assert::IsFalse(i == float4x4());
            Assert::IsFalse(j == float4x4());
            Assert::IsFalse(k == float4x4());
            Assert::IsFalse(l == float4x4());
            Assert::IsFalse(m == float4x4());
            Assert::IsFalse(n == float4x4());
            Assert::IsFalse(o == float4x4());
            Assert::IsFalse(p == float4x4());

            Assert::IsTrue(a != float4x4());
            Assert::IsTrue(b != float4x4());
            Assert::IsTrue(c != float4x4());
            Assert::IsTrue(d != float4x4());
            Assert::IsTrue(e != float4x4());
            Assert::IsTrue(f != float4x4());
            Assert::IsTrue(g != float4x4());
            Assert::IsTrue(h != float4x4());
            Assert::IsTrue(i != float4x4());
            Assert::IsTrue(j != float4x4());
            Assert::IsTrue(k != float4x4());
            Assert::IsTrue(l != float4x4());
            Assert::IsTrue(m != float4x4());
            Assert::IsTrue(n != float4x4());
            Assert::IsTrue(o != float4x4());
            Assert::IsTrue(p != float4x4());

            Assert::IsFalse(is_identity(a));
            Assert::IsFalse(is_identity(b));
            Assert::IsFalse(is_identity(c));
            Assert::IsFalse(is_identity(d));
            Assert::IsFalse(is_identity(e));
            Assert::IsFalse(is_identity(f));
            Assert::IsFalse(is_identity(g));
            Assert::IsFalse(is_identity(h));
            Assert::IsFalse(is_identity(i));
            Assert::IsFalse(is_identity(j));
            Assert::IsFalse(is_identity(k));
            Assert::IsFalse(is_identity(l));
            Assert::IsFalse(is_identity(m));
            Assert::IsFalse(is_identity(n));
            Assert::IsFalse(is_identity(o));
            Assert::IsFalse(is_identity(p));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
            Assert::IsFalse(e == e);
            Assert::IsFalse(f == f);
            Assert::IsFalse(g == g);
            Assert::IsFalse(h == h);
            Assert::IsFalse(i == i);
            Assert::IsFalse(j == j);
            Assert::IsFalse(k == k);
            Assert::IsFalse(l == l);
            Assert::IsFalse(m == m);
            Assert::IsFalse(n == n);
            Assert::IsFalse(o == o);
            Assert::IsFalse(p == p);
        }

        struct Matrix4x4_2x
        {
            float4x4 a;
            float4x4 b;
        };

        struct Matrix4x4PlusFloat
        {
            float4x4 v;
            float f;
        };

        struct Matrix4x4PlusFloat_2x
        {
            Matrix4x4PlusFloat a;
            Matrix4x4PlusFloat b;
        };

        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(Float4x4SizeofTest)
        {
            Assert::AreEqual(size_t(64), sizeof(float4x4));
            Assert::AreEqual(size_t(128), sizeof(Matrix4x4_2x));
            Assert::AreEqual(size_t(68), sizeof(Matrix4x4PlusFloat));
            Assert::AreEqual(size_t(136), sizeof(Matrix4x4PlusFloat_2x));
            Assert::AreEqual(sizeof(float4x4), sizeof(DirectX::XMFLOAT4X4));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(Float4x4FieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(float4x4, m11));
            Assert::AreEqual(size_t(4), offsetof(float4x4, m12));
            Assert::AreEqual(size_t(8), offsetof(float4x4, m13));
            Assert::AreEqual(size_t(12), offsetof(float4x4, m14));

            Assert::AreEqual(size_t(16), offsetof(float4x4, m21));
            Assert::AreEqual(size_t(20), offsetof(float4x4, m22));
            Assert::AreEqual(size_t(24), offsetof(float4x4, m23));
            Assert::AreEqual(size_t(28), offsetof(float4x4, m24));

            Assert::AreEqual(size_t(32), offsetof(float4x4, m31));
            Assert::AreEqual(size_t(36), offsetof(float4x4, m32));
            Assert::AreEqual(size_t(40), offsetof(float4x4, m33));
            Assert::AreEqual(size_t(44), offsetof(float4x4, m34));

            Assert::AreEqual(size_t(48), offsetof(float4x4, m41));
            Assert::AreEqual(size_t(52), offsetof(float4x4, m42));
            Assert::AreEqual(size_t(56), offsetof(float4x4, m43));
            Assert::AreEqual(size_t(60), offsetof(float4x4, m44));
        }

        // A test of float4x4 -> DirectXMath interop
        TEST_METHOD(Float4x4LoadTest)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            DirectX::XMMATRIX b = DirectX::XMLoadFloat4x4(&a);
            DirectX::XMFLOAT4X4 c;
            DirectX::XMStoreFloat4x4(&c, b);

            Assert::AreEqual(a.m11, c._11);
            Assert::AreEqual(a.m12, c._12);
            Assert::AreEqual(a.m13, c._13);
            Assert::AreEqual(a.m14, c._14);

            Assert::AreEqual(a.m21, c._21);
            Assert::AreEqual(a.m22, c._22);
            Assert::AreEqual(a.m23, c._23);
            Assert::AreEqual(a.m24, c._24);

            Assert::AreEqual(a.m31, c._31);
            Assert::AreEqual(a.m32, c._32);
            Assert::AreEqual(a.m33, c._33);
            Assert::AreEqual(a.m34, c._34);

            Assert::AreEqual(a.m41, c._41);
            Assert::AreEqual(a.m42, c._42);
            Assert::AreEqual(a.m43, c._43);
            Assert::AreEqual(a.m44, c._44);
        }

        // A test of DirectXMath -> float4x4 interop
        TEST_METHOD(Float4x4StoreTest)
        {
            DirectX::XMFLOAT4X4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
            DirectX::XMMATRIX b = DirectX::XMLoadFloat4x4(&a);
            float4x4 c;
            DirectX::XMStoreFloat4x4(&c, b);

            Assert::AreEqual(a._11, c.m11);
            Assert::AreEqual(a._12, c.m12);
            Assert::AreEqual(a._13, c.m13);
            Assert::AreEqual(a._14, c.m14);

            Assert::AreEqual(a._21, c.m21);
            Assert::AreEqual(a._22, c.m22);
            Assert::AreEqual(a._23, c.m23);
            Assert::AreEqual(a._24, c.m24);

            Assert::AreEqual(a._31, c.m31);
            Assert::AreEqual(a._32, c.m32);
            Assert::AreEqual(a._33, c.m33);
            Assert::AreEqual(a._34, c.m34);

            Assert::AreEqual(a._41, c.m41);
            Assert::AreEqual(a._42, c.m42);
            Assert::AreEqual(a._43, c.m43);
            Assert::AreEqual(a._44, c.m44);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(Float4x4TypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<float4x4>::value);
            Assert::IsFalse(std::is_pod<float4x4>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<float4x4>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<float4x4>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<float4x4>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<float4x4>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<float4x4>::value);

            // Move constructor is present.
            Assert::IsTrue(std::is_move_constructible<float4x4>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<float4x4>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<float4x4>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<float4x4>::value);

            // Move assignment is present.
            Assert::IsTrue(std::is_move_assignable<float4x4>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<float4x4>::value);
            Assert::IsTrue(std::is_trivially_destructible<float4x4>::value);
        }

        // A test to validate interop between WindowsNumerics.h (Windows::Foundation::Numerics) and WinRT (Microsoft::Graphics::Canvas::Numerics)
        TEST_METHOD(Float4x4WinRTInteropTest)
        {
            float4x4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

            Microsoft::Graphics::Canvas::Numerics::Matrix4x4 b = a;

            Assert::AreEqual(a.m11, b.M11);
            Assert::AreEqual(a.m12, b.M12);
            Assert::AreEqual(a.m13, b.M13);
            Assert::AreEqual(a.m14, b.M14);

            Assert::AreEqual(a.m21, b.M21);
            Assert::AreEqual(a.m22, b.M22);
            Assert::AreEqual(a.m23, b.M23);
            Assert::AreEqual(a.m24, b.M24);

            Assert::AreEqual(a.m31, b.M31);
            Assert::AreEqual(a.m32, b.M32);
            Assert::AreEqual(a.m33, b.M33);
            Assert::AreEqual(a.m34, b.M34);

            Assert::AreEqual(a.m41, b.M41);
            Assert::AreEqual(a.m42, b.M42);
            Assert::AreEqual(a.m43, b.M43);
            Assert::AreEqual(a.m44, b.M44);

            float4x4 c = b;

            Assert::AreEqual(a, c);
        }
    };
}
