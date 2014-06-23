//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include "pch.h"
#include "Helpers.h"

using namespace Platform::Numerics;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NumericsTests
{
    TEST_CLASS(PlaneTest)
    {
    public:
        // A test for operator != (plane, plane)
        TEST_METHOD(PlaneInequalityTest)
        {
            plane a(1.0f, 2.0f, 3.0f, 4.0f);
            plane b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = false;
            bool actual = a != b;
            Assert::AreEqual(expected, actual, L"plane::operator != did not return the expected value.");

            // case 2: compare between different values
            b.normal.x = 10.0f;
            expected = true;
            actual = a != b;
            Assert::AreEqual(expected, actual, L"plane::operator != did not return the expected value.");
        }
        
        // A test for operator == (plane, plane)
        TEST_METHOD(PlaneEqualityTest)
        {
            plane a(1.0f, 2.0f, 3.0f, 4.0f);
            plane b(1.0f, 2.0f, 3.0f, 4.0f);

            // case 1: compare between same values
            bool expected = true;
            bool actual = a == b;
            Assert::AreEqual(expected, actual, L"plane::operator != did not return the expected value.");

            // case 2: compare between different values
            b.normal.x = 10.0f;
            expected = false;
            actual = a == b;
            Assert::AreEqual(expected, actual, L"plane::operator != did not return the expected value.");
        }
        
        // A test for plane ()
        TEST_METHOD(PlaneConstructorTest0)
        {
            plane target;

            // Default constructor leaves the struct uninitialized, so this 
            // test does nothing more than validate that the constructor exists.

            target.d = 0;    // avoid warning about unused variable
        }

        // A test for plane (float, float, float, float)
        TEST_METHOD(PlaneConstructorTest1)
        {
            float a = 1.0f, b = 2.0f, c = 3.0f, d = 4.0f;
            plane target(a, b, c, d);

            Assert::IsTrue(
                target.normal.x == a && target.normal.y == b && target.normal.z == c && target.d == d,
                L"plane::cstor did not return the expected value.");
        }

        // A test for make_plane_from_vertices
        TEST_METHOD(PlaneCreateFromVerticesTest)
        {
            float3 point1(0.0f, 1.0f, 1.0f);
            float3 point2(0.0f, 0.0f, 1.0f);
            float3 point3(1.0f, 0.0f, 1.0f);

            plane target = make_plane_from_vertices(point1, point2, point3);
            plane expected(float3(0, 0, 1), -1.0f);
            Assert::AreEqual(target, expected, L"plane::cstor did not return the expected value.");
        }
        
        // A test for make_plane_from_vertices
        TEST_METHOD(PlaneCreateFromVerticesTest2)
        {
            float3 point1(0.0f, 0.0f, 1.0f);
            float3 point2(1.0f, 0.0f, 0.0f);
            float3 point3(1.0f, 1.0f, 0.0f);

            plane target = make_plane_from_vertices(point1, point2, point3);
            float invRoot2 = (float)(1.0f / sqrt(2));

            plane expected(float3(invRoot2, 0, invRoot2), -invRoot2);
            Assert::IsTrue(Equal(target, expected), L"plane::cstor did not return the expected value.");
        }

        // A test for plane (float3, float)
        TEST_METHOD(PlaneConstructorTest3)
        {
            float3 normal(1, 2, 3);
            float d = 4;

            plane target(normal, d);
            Assert::IsTrue(
                target.normal == normal && target.d == d,
                L"plane::cstor did not return the expected value.");
        }

        // A test for plane (float4)
        TEST_METHOD(PlaneConstructorTest)
        {
            float4 value(1.0f, 2.0f, 3.0f, 4.0f);
            plane target(value);

            Assert::IsTrue(
                target.normal.x == value.x && target.normal.y == value.y && target.normal.z == value.z && target.d == value.w,
                L"plane::cstor did not return the expected value.");
        }

        TEST_METHOD(PlaneDotTest)
        {
            plane target(2, 3, 4, 5);
            float4 value(5, 4, 3, 2);

            float expected = 10 + 12 + 12 + 10;
            float actual = dot(target, value);
            Assert::IsTrue(Equal(expected, actual), L"dot returns unexpected value.");
        }

        TEST_METHOD(PlaneDotCoordinateTest)
        {
            plane target(2, 3, 4, 5);
            float3 value(5, 4, 3);

            float expected = 10 + 12 + 12 + 5;
            float actual = dot_coordinate(target, value);
            Assert::IsTrue(Equal(expected, actual), L"dot_coordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneDotNormalTest)
        {
            plane target(2, 3, 4, 5);
            float3 value(5, 4, 3);

            float expected = 10 + 12 + 12;
            float actual = dot_normal(target, value);
            Assert::IsTrue(Equal(expected, actual), L"dot_coordinate returns unexpected value.");
        }

        TEST_METHOD(PlaneNormalizeTest)
        {
            plane target(1, 2, 3, 4);

            float f = length_squared(target.normal);
            float invF = 1.0f / sqrt(f);
            plane expected(target.normal * invF, target.d * invF);

            plane actual = normalize(target);
            Assert::IsTrue(Equal(expected, actual), L"normalize returns unexpected value.");

            // normalize, normalized normal.
            actual = normalize(actual);
            Assert::IsTrue(Equal(expected, actual), L"normalize returns unexpected value.");
        }

        TEST_METHOD(PlaneTransformTest1)
        {
            plane target(1, 2, 3, 4);
            target = normalize(target);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            m.m41 = 10.0f;
            m.m42 = 20.0f;
            m.m43 = 30.0f;

            plane expected;
            float4x4 inv;
            invert(m, &inv);
            float4x4 itm = transpose(inv);
            float x = target.normal.x, y = target.normal.y, z = target.normal.z, w = target.d;
            expected.normal.x = x * itm.m11 + y * itm.m21 + z * itm.m31 + w * itm.m41;
            expected.normal.y = x * itm.m12 + y * itm.m22 + z * itm.m32 + w * itm.m42;
            expected.normal.z = x * itm.m13 + y * itm.m23 + z * itm.m33 + w * itm.m43;
            expected.d = x * itm.m14 + y * itm.m24 + z * itm.m34 + w * itm.m44;

            plane actual;
            actual = transform(target, m);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }

        TEST_METHOD(PlaneTransformTest2)
        {
            plane target(1, 2, 3, 4);
            target = normalize(target);

            float4x4 m =
                make_float4x4_rotation_x(ToRadians(30.0f)) *
                make_float4x4_rotation_y(ToRadians(30.0f)) *
                make_float4x4_rotation_z(ToRadians(30.0f));
            quaternion q = make_quaternion_from_rotation_matrix(m);

            plane expected;
            float x = target.normal.x, y = target.normal.y, z = target.normal.z, w = target.d;
            expected.normal.x = x * m.m11 + y * m.m21 + z * m.m31 + w * m.m41;
            expected.normal.y = x * m.m12 + y * m.m22 + z * m.m32 + w * m.m42;
            expected.normal.z = x * m.m13 + y * m.m23 + z * m.m33 + w * m.m43;
            expected.d = x * m.m14 + y * m.m24 + z * m.m34 + w * m.m44;

            plane actual;
            actual = transform(target, q);
            Assert::IsTrue(Equal(expected, actual), L"transform did not return the expected value.");
        }
        
        // A test for plane comparison involving NaN values
        TEST_METHOD(PlaneEqualsNanTest)
        {
            plane a(NAN, 0, 0, 0);
            plane b(0, NAN, 0, 0);
            plane c(0, 0, NAN, 0);
            plane d(0, 0, 0, NAN);

            Assert::IsFalse(a == plane(0, 0, 0, 0));
            Assert::IsFalse(b == plane(0, 0, 0, 0));
            Assert::IsFalse(c == plane(0, 0, 0, 0));
            Assert::IsFalse(d == plane(0, 0, 0, 0));

            Assert::IsTrue(a != plane(0, 0, 0, 0));
            Assert::IsTrue(b != plane(0, 0, 0, 0));
            Assert::IsTrue(c != plane(0, 0, 0, 0));
            Assert::IsTrue(d != plane(0, 0, 0, 0));

            // Counterintuitive result - IEEE rules for NaN comparison are weird!
            Assert::IsFalse(a == a);
            Assert::IsFalse(b == b);
            Assert::IsFalse(c == c);
            Assert::IsFalse(d == d);
        }

        struct Plane_2x
        {
            plane a;
            plane b;
        };

        struct PlanePlusFloat
        {
            plane v;
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
            Assert::AreEqual(size_t(16), sizeof(plane));
            Assert::AreEqual(size_t(32), sizeof(Plane_2x));
            Assert::AreEqual(size_t(20), sizeof(PlanePlusFloat));
            Assert::AreEqual(size_t(40), sizeof(PlanePlusFloat_2x));
            Assert::AreEqual(sizeof(plane), sizeof(DirectX::XMFLOAT4));
        }

        // A test to make sure the fields are laid out how we expect
        TEST_METHOD(PlaneFieldOffsetTest)
        {
            Assert::AreEqual(size_t(0), offsetof(plane, normal));
            Assert::AreEqual(size_t(12), offsetof(plane, d));
        }

        // A test of plane -> DirectXMath interop
        TEST_METHOD(PlaneLoadTest)
        {
            plane a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadPlane(&a);
            DirectX::XMFLOAT4 c;
            DirectX::XMStoreFloat4(&c, b);

            Assert::AreEqual(a.normal.x, c.x);
            Assert::AreEqual(a.normal.y, c.y);
            Assert::AreEqual(a.normal.z, c.z);
            Assert::AreEqual(a.d, c.w);
        }

        // A test of DirectXMath -> plane interop
        TEST_METHOD(PlaneStoreTest)
        {
            DirectX::XMFLOAT4 a(23, 42, 666, -1);
            DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&a);
            plane c;
            DirectX::XMStorePlane(&c, b);

            Assert::AreEqual(a.x, c.normal.x);
            Assert::AreEqual(a.y, c.normal.y);
            Assert::AreEqual(a.z, c.normal.z);
            Assert::AreEqual(a.w, c.d);
        }

        // A test to make sure this type matches our expectations for blittability
        TEST_METHOD(PlaneTypeTraitsTest)
        {
            // We should be trivial, but not POD because we have constructors.
            Assert::IsTrue(std::is_trivial<plane>::value);
            Assert::IsFalse(std::is_pod<plane>::value);

            // Default constructor is present and trivial.
            Assert::IsTrue(std::is_default_constructible<plane>::value);
            Assert::IsTrue(std::is_trivially_default_constructible<plane>::value);
            Assert::IsFalse(std::is_nothrow_default_constructible<plane>::value);

            // Copy constructor is present and trivial.
            Assert::IsTrue(std::is_copy_constructible<plane>::value);
            Assert::IsTrue(std::is_trivially_copy_constructible<plane>::value);

            // Move constructor is present and trivial.
            Assert::IsTrue(std::is_move_constructible<plane>::value);
            Assert::IsTrue(std::is_trivially_move_constructible<plane>::value);

            // Copy assignment is present and trivial.
            Assert::IsTrue(std::is_copy_assignable<plane>::value);
            Assert::IsTrue(std::is_trivially_copy_assignable<plane>::value);
            Assert::IsTrue(std::is_nothrow_copy_assignable<plane>::value);

            // Move assignment is present and trivial.
            Assert::IsTrue(std::is_move_assignable<plane>::value);
            Assert::IsTrue(std::is_trivially_move_assignable<plane>::value);
            Assert::IsTrue(std::is_nothrow_move_assignable<plane>::value);

            // Destruction is present and trivial.
            Assert::IsTrue(std::is_destructible<plane>::value);
            Assert::IsTrue(std::is_trivially_destructible<plane>::value);
        }
    };
}
