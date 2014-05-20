//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Microsoft::DirectX::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(PlaneTest)
    {
    public:
        // A test for Plane (float, float, float, float)
        TEST_METHOD(PlaneConstructorTest1)
        {
            float a = 1.0f, b = 2.0f, c = 3.0f, d = 4.0f;
            Plane target = PlaneHelper::FromValues(a, b, c, d);

            Assert::IsTrue(
                target.Normal.X == a && target.Normal.Y == b && target.Normal.Z == c && target.D == d,
                L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane::CreateFromVertices
        TEST_METHOD(PlaneCreateFromVerticesTest)
        {
            Vector3 point1 = Vector3Helper::FromValues(0.0f, 1.0f, 1.0f);
            Vector3 point2 = Vector3Helper::FromValues(0.0f, 0.0f, 1.0f);
            Vector3 point3 = Vector3Helper::FromValues(1.0f, 0.0f, 1.0f);

            Plane target = PlaneHelper::CreateFromVertices(point1, point2, point3);
            Plane expected = PlaneHelper::FromVector3(Vector3Helper::FromValues(0, 0, 1), -1.0f);
            Assert::AreEqual(target, expected, L"Plane::cstor did not return the expected value.");
        }
        
        // A test for Plane::CreateFromVertices
        TEST_METHOD(PlaneCreateFromVerticesTest2)
        {
            Vector3 point1 = Vector3Helper::FromValues(0.0f, 0.0f, 1.0f);
            Vector3 point2 = Vector3Helper::FromValues(1.0f, 0.0f, 0.0f);
            Vector3 point3 = Vector3Helper::FromValues(1.0f, 1.0f, 0.0f);

            Plane target = PlaneHelper::CreateFromVertices(point1, point2, point3);
            float invRoot2 = (float)(1.0f / sqrt(2));

            Plane expected = PlaneHelper::FromVector3(Vector3Helper::FromValues(invRoot2, 0, invRoot2), -invRoot2);
            Assert::IsTrue(Equal(target, expected), L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (Vector3, Vector3, Vector3)
        TEST_METHOD(PlaneConstructorTest3)
        {
            Vector3 normal = Vector3Helper::FromValues(1, 2, 3);
            float d = 4;

            Plane target = PlaneHelper::FromVector3(normal, d);
            Assert::IsTrue(
                target.Normal == normal && target.D == d,
                L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (Vector4)
        TEST_METHOD(PlaneConstructorTest)
        {
            Vector4 value = Vector4Helper::FromValues(1.0f, 2.0f, 3.0f, 4.0f);
            Plane target = PlaneHelper::FromVector4(value);

            Assert::IsTrue(
                target.Normal.X == value.X && target.Normal.Y == value.Y && target.Normal.Z == value.Z && target.D == value.W,
                L"Plane::cstor did not return the expected value.");
        }

        TEST_METHOD(PlaneDotTest)
        {
            Plane target = PlaneHelper::FromValues(2, 3, 4, 5);
            Vector4 value = Vector4Helper::FromValues(5, 4, 3, 2);

            float expected = 10 + 12 + 12 + 10;
            float actual = PlaneHelper::Dot(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Dot returns unexpected value.");
        }

        TEST_METHOD(PlaneDotCoordinateTest)
        {
            Plane target = PlaneHelper::FromValues(2, 3, 4, 5);
            Vector3 value = Vector3Helper::FromValues(5, 4, 3);

            float expected = 10 + 12 + 12 + 5;
            float actual = PlaneHelper::DotCoordinate(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::DotCoordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneDotNormalTest)
        {
            Plane target = PlaneHelper::FromValues(2, 3, 4, 5);
            Vector3 value = Vector3Helper::FromValues(5, 4, 3);

            float expected = 10 + 12 + 12;
            float actual = PlaneHelper::DotNormal(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::DotCoordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneNormalizeTest)
        {
            Plane target = PlaneHelper::FromValues(1, 2, 3, 4);

            float f = Vector3Helper::LengthSquared(target.Normal);
            float invF = 1.0f / sqrt(f);
            Plane expected = PlaneHelper::FromVector3(Vector3Helper::MultiplyByScalar(target.Normal, invF), target.D * invF);

            Plane actual = PlaneHelper::Normalize(target);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Normalize returns unexpected value.");

            // normalize, normalized normal.
            actual = PlaneHelper::Normalize(actual);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Normalize returns unexpected value.");
        }

        TEST_METHOD(PlaneTransformTest1)
        {
            Plane target = PlaneHelper::FromValues(1, 2, 3, 4);
            target = PlaneHelper::Normalize(target);

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

            Plane expected;
            Matrix4x4 inv;
            Matrix4x4Helper::Invert(m, &inv);
            Matrix4x4 itm = Matrix4x4Helper::Transpose(inv);
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * itm.M11 + y * itm.M21 + z * itm.M31 + w * itm.M41;
            expected.Normal.Y = x * itm.M12 + y * itm.M22 + z * itm.M32 + w * itm.M42;
            expected.Normal.Z = x * itm.M13 + y * itm.M23 + z * itm.M33 + w * itm.M43;
            expected.D = x * itm.M14 + y * itm.M24 + z * itm.M34 + w * itm.M44;

            Plane actual;
            actual = PlaneHelper::Transform(target, m);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Transform did not return the expected value.");
        }

        TEST_METHOD(PlaneTransformTest2)
        {
            Plane target = PlaneHelper::FromValues(1, 2, 3, 4);
            target = PlaneHelper::Normalize(target);

            Matrix4x4 m =
                Matrix4x4Helper::Multiply(
                    Matrix4x4Helper::Multiply(
                        Matrix4x4Helper::CreateRotationX(ToRadians(30.0f)),
                        Matrix4x4Helper::CreateRotationY(ToRadians(30.0f))
                    ),
                    Matrix4x4Helper::CreateRotationZ(ToRadians(30.0f))
                );
            Quaternion q = QuaternionHelper::CreateFromRotationMatrix(m);

            Plane expected;
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * m.M11 + y * m.M21 + z * m.M31 + w * m.M41;
            expected.Normal.Y = x * m.M12 + y * m.M22 + z * m.M32 + w * m.M42;
            expected.Normal.Z = x * m.M13 + y * m.M23 + z * m.M33 + w * m.M43;
            expected.D = x * m.M14 + y * m.M24 + z * m.M34 + w * m.M44;

            Plane actual;
            actual = PlaneHelper::TransformByQuaternion(target, q);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Transform did not return the expected value.");
        }
        
        struct Plane_2x
        {
            Plane a;
            Plane b;
        };

        struct PlanePlusFloat
        {
            Plane v;
            float f;
        };

        struct PlanePlusFloat_2x
        {
            PlanePlusFloat a;
            PlanePlusFloat b;
        };
        
        // A test to make sure these types are blittable directly into GPU buffer memory layouts
        TEST_METHOD(PlaneSizeofTest)
        {
            Assert::AreEqual(size_t(16), sizeof(Plane));
            Assert::AreEqual(size_t(32), sizeof(Plane_2x));
            Assert::AreEqual(size_t(20), sizeof(PlanePlusFloat));
            Assert::AreEqual(size_t(40), sizeof(PlanePlusFloat_2x));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(PlaneFieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(Plane, Normal));
            Assert::AreEqual(size_t(12), offsetof(Plane, D));
        }
    };
}
