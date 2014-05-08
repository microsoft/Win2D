//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Windows::Math;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTests
{
    TEST_CLASS(PlaneTest)
    {
    public:
        // A test for operator != (Plane, Plane)
        TEST_METHOD(PlaneInequalityTest)
        {
            Plane a(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"Plane::operator != did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"Plane::operator != did not return the expected value.");
        }
        
        // A test for operator == (Plane, Plane)
        TEST_METHOD(PlaneEqualityTest)
        {
            Plane a(1.0f, 2.0f, 3.0f, 4.0f);
            Plane b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"Plane::operator != did not return the expected value.");

            // case 2: compare between different values
            b.Normal.X = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"Plane::operator != did not return the expected value.");
        }
        
        // A test for Plane ()
        TEST_METHOD(PlaneConstructorTest0)
        {
            Plane target;

            Assert::IsTrue(
                target.Normal.X == 0 && target.Normal.Y == 0 && target.Normal.Z == 0 && target.D == 0,
                L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (float, float, float, float)
        TEST_METHOD(PlaneConstructorTest1)
        {
            float a = 1.0f, b = 2.0f, c = 3.0f, d = 4.0f;
            Plane target(a, b, c, d);

            Assert::IsTrue(
                target.Normal.X == a && target.Normal.Y == b && target.Normal.Z == c && target.D == d,
                L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (Vector3, Vector3, Vector3)
        TEST_METHOD(PlaneConstructorTest2)
        {
            Vector3 point1(0.0f, 1.0f, 1.0f);
            Vector3 point2(0.0f, 0.0f, 1.0f);
            Vector3 point3(1.0f, 0.0f, 1.0f);

            Plane target(point1, point2, point3);
            Plane expected(Vector3(0, 0, 1), -1.0f);
            Assert::AreEqual(target, expected, L"Plane::cstor did not return the expected value.");
        }
        
        // A test for Plane (Vector3, Vector3, Vector3)
        TEST_METHOD(PlaneConstructorRegressionTest01)
        {
            Vector3 point1(0.0f, 0.0f, 1.0f);
            Vector3 point2(1.0f, 0.0f, 0.0f);
            Vector3 point3(1.0f, 1.0f, 0.0f);

            Plane target(point1, point2, point3);
            float invRoot2 = (float)(1.0f / sqrt(2));

            Plane expected(Vector3(invRoot2, 0, invRoot2), -invRoot2);
            Assert::IsTrue(Equal(target, expected), L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (Vector3, Vector3, Vector3)
        TEST_METHOD(PlaneConstructorTest3)
        {
            Vector3 normal(1, 2, 3);
            float d = 4;

            Plane target(normal, d);
            Assert::IsTrue(
                target.Normal == normal && target.D == d,
                L"Plane::cstor did not return the expected value.");
        }

        // A test for Plane (Vector4)
        TEST_METHOD(PlaneConstructorTest)
        {
            Vector4 value(1.0f, 2.0f, 3.0f, 4.0f);
            Plane target(value);

            Assert::IsTrue(
                target.Normal.X == value.X && target.Normal.Y == value.Y && target.Normal.Z == value.Z && target.D == value.W,
                L"Plane::cstor did not return the expected value.");
        }

        TEST_METHOD(PlaneDotTest)
        {
            Plane target(2, 3, 4, 5);
            Vector4 value(5, 4, 3, 2);

            float expected = 10 + 12 + 12 + 10;
            float actual = Plane::Dot(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Dot returns unexpected value.");
        }

        TEST_METHOD(PlaneDotCoordinateTest)
        {
            Plane target(2, 3, 4, 5);
            Vector3 value(5, 4, 3);

            float expected = 10 + 12 + 12 + 5;
            float actual = Plane::DotCoordinate(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::DotCoordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneDotNormalTest)
        {
            Plane target(2, 3, 4, 5);
            Vector3 value(5, 4, 3);

            float expected = 10 + 12 + 12;
            float actual = Plane::DotNormal(target, value);
            Assert::IsTrue(Equal(expected, actual), L"Plane::DotCoordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneNormalizeTest)
        {
            Plane target(1, 2, 3, 4);

            float f = target.Normal.LengthSquared();
            float invF = 1.0f / sqrt(f);
            Plane expected(target.Normal * invF, target.D * invF);

            Plane actual = Plane::Normalize(target);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Normalize returns unexpected value.");

            // normalize, normalized normal.
            actual = Plane::Normalize(actual);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Normalize returns unexpected value.");
        }

        TEST_METHOD(PlaneTransformTest1)
        {
            Plane target(1, 2, 3, 4);
            target = Plane::Normalize(target);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            m.M41 = 10.0f;
            m.M42 = 20.0f;
            m.M43 = 30.0f;

            Plane expected;
            Matrix4x4 inv;
            Matrix4x4::Invert(m, &inv);
            Matrix4x4 itm = Matrix4x4::Transpose(inv);
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * itm.M11 + y * itm.M21 + z * itm.M31 + w * itm.M41;
            expected.Normal.Y = x * itm.M12 + y * itm.M22 + z * itm.M32 + w * itm.M42;
            expected.Normal.Z = x * itm.M13 + y * itm.M23 + z * itm.M33 + w * itm.M43;
            expected.D = x * itm.M14 + y * itm.M24 + z * itm.M34 + w * itm.M44;

            Plane actual;
            actual = Plane::Transform(target, m);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Transform did not return the expected value.");
        }

        TEST_METHOD(PlaneTransformTest2)
        {
            Plane target(1, 2, 3, 4);
            target = Plane::Normalize(target);

            Matrix4x4 m =
                Matrix4x4::CreateRotationX(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationY(ToRadians(30.0f)) *
                Matrix4x4::CreateRotationZ(ToRadians(30.0f));
            Quaternion q = Quaternion::CreateFromRotationMatrix(m);

            Plane expected;
            float x = target.Normal.X, y = target.Normal.Y, z = target.Normal.Z, w = target.D;
            expected.Normal.X = x * m.M11 + y * m.M21 + z * m.M31 + w * m.M41;
            expected.Normal.Y = x * m.M12 + y * m.M22 + z * m.M32 + w * m.M42;
            expected.Normal.Z = x * m.M13 + y * m.M23 + z * m.M33 + w * m.M43;
            expected.D = x * m.M14 + y * m.M24 + z * m.M34 + w * m.M44;

            Plane actual;
            actual = Plane::Transform(target, q);
            Assert::IsTrue(Equal(expected, actual), L"Plane::Transform did not return the expected value.");
        }
        
        // A test for Plane comparison involving NaN values
        TEST_METHOD(PlaneEqualsNanTest)
        {
            Plane a(NAN, 0, 0, 0);
            Plane b(0, NAN, 0, 0);
            Plane c(0, 0, NAN, 0);
            Plane d(0, 0, 0, NAN);

            Assert::IsFalse(a == Plane(0, 0, 0, 0));
            Assert::IsFalse(b == Plane(0, 0, 0, 0));
            Assert::IsFalse(c == Plane(0, 0, 0, 0));
            Assert::IsFalse(d == Plane(0, 0, 0, 0));

            Assert::IsTrue(a != Plane(0, 0, 0, 0));
            Assert::IsTrue(b != Plane(0, 0, 0, 0));
            Assert::IsTrue(c != Plane(0, 0, 0, 0));
            Assert::IsTrue(d != Plane(0, 0, 0, 0));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
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
            Assert::AreEqual(sizeof(Plane), sizeof(DirectX::XMFLOAT4));
        }

        // A test of Plane -> DirectXMath interop
        TEST_METHOD(PlaneLoadTest)
        {
            Plane a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadPlane(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.Normal.X, c.x);
            Assert::AreEqual(a.Normal.Y, c.y);
            Assert::AreEqual(a.Normal.Z, c.z);
            Assert::AreEqual(a.D, c.w);
        }

        // A test of DirectXMath -> Plane interop
        TEST_METHOD(PlaneStoreTest)
        {
            DirectX::XMFLOAT4 a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&a);
            Plane c;
            DirectX::XMStorePlane(&c, b);

            Assert::AreEqual(a.x, c.Normal.X);
            Assert::AreEqual(a.y, c.Normal.Y);
            Assert::AreEqual(a.z, c.Normal.Z);
            Assert::AreEqual(a.w, c.D);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(PlaneTypeTraitsTest)
        {
            // We should be standard layout, but not POD or trivial due to the zero-initializing default constructor.
            Assert::IsTrue(std::is_standard_layout<Plane>::value);
            Assert::IsFalse(std::is_pod<Plane>::value);
            Assert::IsFalse(std::is_trivial<Plane>::value);

            // Default constructor is present but not trivial.
            Assert::IsTrue(std::is_default_constructible<Plane>::value);
            Assert::IsFalse(std::is_trivially_default_constructible<Plane>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<Plane>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<Plane>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<Plane>::value);
            Assert::IsTrue(std::is_nothrow_copy_constructible<Plane>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<Plane>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<Plane>::value);
            Assert::IsTrue(std::is_nothrow_move_constructible<Plane>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<Plane>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<Plane>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<Plane>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<Plane>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<Plane>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<Plane>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<Plane>::value);
            Assert::IsTrue(std::is_trivially_destructible<Plane>::value);
            Assert::IsTrue(std::is_nothrow_destructible<Plane>::value);
        }
    };
}
