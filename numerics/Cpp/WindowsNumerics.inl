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

#pragma warning(push)
#pragma warning(disable: 4723) // potential divide by 0
#pragma warning(disable: 4756) // overflow in constant arithmetic


namespace DirectX
{
    inline XMVECTOR XM_CALLCONV XMLoadFloat2(_In_ Windows::Foundation::Numerics::float2 const* pSource)
    {
        return XMLoadFloat2(reinterpret_cast<XMFLOAT2 const*>(pSource));
    }


    inline XMVECTOR XM_CALLCONV XMLoadFloat3(_In_ Windows::Foundation::Numerics::float3 const* pSource)
    {
        return XMLoadFloat3(reinterpret_cast<XMFLOAT3 const*>(pSource));
    }


    inline XMVECTOR XM_CALLCONV XMLoadFloat4(_In_ Windows::Foundation::Numerics::float4 const* pSource)
    {
        return XMLoadFloat4(reinterpret_cast<XMFLOAT4 const*>(pSource));
    }


    inline XMMATRIX XM_CALLCONV XMLoadFloat3x2(_In_ Windows::Foundation::Numerics::float3x2 const* pSource)
    {
        XMVECTOR abcd = XMLoadFloat4(reinterpret_cast<XMFLOAT4 const*>(&pSource->m11));
        XMVECTOR ef = XMLoadFloat2(reinterpret_cast<XMFLOAT2 const*>(&pSource->m31));

        XMMATRIX m;

        m.r[0] = XMVectorPermute<0, 1, 4, 5>(abcd, g_XMZero);
        m.r[1] = XMVectorPermute<2, 3, 4, 5>(abcd, g_XMZero);
        m.r[2] = g_XMIdentityR2;
        m.r[3] = XMVectorPermute<0, 1, 6, 7>(ef, g_XMIdentityR3);

        return m;
    }


    inline XMMATRIX XM_CALLCONV XMLoadFloat4x4(_In_ Windows::Foundation::Numerics::float4x4 const* pSource)
    {
        return XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4 const*>(pSource));
    }


    inline XMVECTOR XM_CALLCONV XMLoadPlane(_In_ Windows::Foundation::Numerics::plane const* pSource)
    {
        return XMLoadFloat4(reinterpret_cast<XMFLOAT4 const*>(pSource));
    }


    inline XMVECTOR XM_CALLCONV XMLoadQuaternion(_In_ Windows::Foundation::Numerics::quaternion const* pSource)
    {
        return XMLoadFloat4(reinterpret_cast<XMFLOAT4 const*>(pSource));
    }


    inline void XM_CALLCONV XMStoreFloat2(_Out_ Windows::Foundation::Numerics::float2* pDestination, _In_ FXMVECTOR value)
    {
        XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(pDestination), value);
    }


    inline void XM_CALLCONV XMStoreFloat3(_Out_ Windows::Foundation::Numerics::float3* pDestination, _In_ FXMVECTOR value)
    {
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(pDestination), value);
    }


    inline void XM_CALLCONV XMStoreFloat4(_Out_ Windows::Foundation::Numerics::float4* pDestination, _In_ FXMVECTOR value)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(pDestination), value);
    }


    inline void XM_CALLCONV XMStoreFloat3x2(_Out_ Windows::Foundation::Numerics::float3x2* pDestination, _In_ FXMMATRIX value)
    {
        XMVECTOR abcd = XMVectorPermute<0, 1, 4, 5>(value.r[0], value.r[1]);
        XMVECTOR ef = value.r[3];

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&pDestination->m11), abcd);
        XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(&pDestination->m31), ef);
    }


    inline void XM_CALLCONV XMStoreFloat4x4(_Out_ Windows::Foundation::Numerics::float4x4* pDestination, _In_ FXMMATRIX value)
    {
        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(pDestination), value);
    }


    inline void XM_CALLCONV XMStorePlane(_Out_ Windows::Foundation::Numerics::plane* pDestination, _In_ FXMVECTOR value)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(pDestination), value);
    }


    inline void XM_CALLCONV XMStoreQuaternion(_Out_ Windows::Foundation::Numerics::quaternion* pDestination, _In_ FXMVECTOR value)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(pDestination), value);
    }
}


namespace Windows { namespace Foundation { namespace Numerics
{
    inline float2::float2(float x, float y)
        : x(x), y(y)
    { }


    inline float2::float2(float value)
        : x(value), y(value)
    { }


#ifdef _WINDOWS_NUMERICS_CX_PROJECTION_


    inline float2::operator float2(Windows::Foundation::Point value)
    {
        return float2(value.X, value.Y);
    }


    inline float2::operator float2(Windows::Foundation::Size value)
    {
        return float2(value.Width, value.Height);
    }


    inline float2::operator Windows::Foundation::Point(float2 value)
    {
        return Windows::Foundation::Point(value.x, value.y);
    }


    inline float2::operator Windows::Foundation::Size(float2 value)
    {
        return Windows::Foundation::Size(value.x, value.y);
    }


#elif defined __cplusplus_winrt // && !_WINDOWS_NUMERICS_CX_PROJECTION_


    inline float2::float2(Windows::Foundation::Point const& value)
        : x(value.X), y(value.Y)
    { }


    inline float2::float2(Windows::Foundation::Size const& value)
        : x(value.Width), y(value.Height)
    { }


    inline float2::operator Windows::Foundation::Point() const
    {
        return Windows::Foundation::Point(x, y);
    }


    inline float2::operator Windows::Foundation::Size() const
    {
        return Windows::Foundation::Size(x, y);
    }


#endif  // __cpluspluswinrt && !_WINDOWS_NUMERICS_CX_PROJECTION_


    inline float2 float2::zero()
    {
        return float2(0, 0);
    }


    inline float2 float2::one()
    {
        return float2(1, 1);
    }


    inline float2 float2::unit_x()
    {
        return float2(1, 0);
    }


    inline float2 float2::unit_y()
    {
        return float2(0, 1);
    }


    inline float2 operator +(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = value1.x + value2.x;
        ans.y = value1.y + value2.y;

        return ans;
    }


    inline float2 operator -(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = value1.x - value2.x;
        ans.y = value1.y - value2.y;

        return ans;
    }


    inline float2 operator *(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = value1.x * value2.x;
        ans.y = value1.y * value2.y;

        return ans;
    }


    inline float2 operator *(float2 const& value1, float value2)
    {
        float2 ans;

        ans.x = value1.x * value2;
        ans.y = value1.y * value2;

        return ans;
    }


    inline float2 operator *(float value1, float2 const& value2)
    {
        float2 ans;

        ans.x = value1 * value2.x;
        ans.y = value1 * value2.y;

        return ans;
    }


    inline float2 operator /(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = value1.x / value2.x;
        ans.y = value1.y / value2.y;

        return ans;
    }


    inline float2 operator /(float2 const& value1, float value2)
    {
        float2 ans;

        float invDiv = 1.0f / value2;

        ans.x = value1.x * invDiv;
        ans.y = value1.y * invDiv;

        return ans;
    }


    inline float2 operator -(float2 const& value)
    {
        float2 ans;

        ans.x = -value.x;
        ans.y = -value.y;

        return ans;
    }


    inline float2& operator +=(float2& value1, float2 const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline float2& operator -=(float2& value1, float2 const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline float2& operator *=(float2& value1, float2 const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float2& operator *=(float2& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float2& operator /=(float2& value1, float2 const& value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline float2& operator /=(float2& value1, float value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline bool operator ==(float2 const& value1, float2 const& value2)
    {
        return value1.x == value2.x &&
               value1.y == value2.y;
    }


    inline bool operator !=(float2 const& value1, float2 const& value2)
    {
        return value1.x != value2.x ||
               value1.y != value2.y;
    }


    inline float length(float2 const& value)
    {
        float ls = value.x * value.x + value.y * value.y;

        return sqrtf(ls);
    }


    inline float length_squared(float2 const& value)
    {
        return value.x * value.x + value.y * value.y;
    }


    inline float distance(float2 const& value1, float2 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;

        float ls = dx * dx + dy * dy;

        return sqrtf(ls);
    }


    inline float distance_squared(float2 const& value1, float2 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;

        return dx * dx + dy * dy;
    }


    inline float dot(float2 const& value1, float2 const& value2)
    {
        return value1.x * value2.x +
               value1.y * value2.y;
    }


    inline float2 normalize(float2 const& value)
    {
        float2 ans;

        float ls = value.x * value.x + value.y * value.y;
        float invNorm = 1.0f / sqrtf(ls);

        ans.x = value.x * invNorm;
        ans.y = value.y * invNorm;

        return ans;
    }


    inline float2 reflect(float2 const& vector, float2 const& normal)
    {
        float2 ans;

        float dot = vector.x * normal.x + vector.y * normal.y;

        ans.x = vector.x - 2.0f * dot * normal.x;
        ans.y = vector.y - 2.0f * dot * normal.y;

        return ans;
    }


    inline float2 (min)(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = (value1.x < value2.x) ? value1.x : value2.x;
        ans.y = (value1.y < value2.y) ? value1.y : value2.y;

        return ans;
    }


    inline float2 (max)(float2 const& value1, float2 const& value2)
    {
        float2 ans;

        ans.x = (value1.x > value2.x) ? value1.x : value2.x;
        ans.y = (value1.y > value2.y) ? value1.y : value2.y;

        return ans;
    }


    inline float2 clamp(float2 const& value1, float2 const& min, float2 const& max)
    {
        float2 result;

        float x = value1.x;
        x = (x > max.x) ? max.x : x;
        x = (x < min.x) ? min.x : x;

        float y = value1.y;
        y = (y > max.y) ? max.y : y;
        y = (y < min.y) ? min.y : y;

        result.x = x;
        result.y = y;

        return result;
    }


    inline float2 lerp(float2 const& value1, float2 const& value2, float amount)
    {
        float2 ans;

        ans.x = value1.x + (value2.x - value1.x) * amount;
        ans.y = value1.y + (value2.y - value1.y) * amount;

        return ans;
    }


    inline float2 transform(float2 const& position, float3x2 const& matrix)
    {
        float2 result;

        result.x = position.x * matrix.m11 + position.y * matrix.m21 + matrix.m31;
        result.y = position.x * matrix.m12 + position.y * matrix.m22 + matrix.m32;

        return result;
    }


    inline float2 transform(float2 const& position, float4x4 const& matrix)
    {
        float2 result;

        result.x = position.x * matrix.m11 + position.y * matrix.m21 + matrix.m41;
        result.y = position.x * matrix.m12 + position.y * matrix.m22 + matrix.m42;

        return result;
    }


    inline float2 transform_normal(float2 const& normal, float3x2 const& matrix)
    {
        float2 result;

        result.x = normal.x * matrix.m11 + normal.y * matrix.m21;
        result.y = normal.x * matrix.m12 + normal.y * matrix.m22;

        return result;
    }


    inline float2 transform_normal(float2 const& normal, float4x4 const& matrix)
    {
        float2 result;

        result.x = normal.x * matrix.m11 + normal.y * matrix.m21;
        result.y = normal.x * matrix.m12 + normal.y * matrix.m22;

        return result;
    }


    inline float2 transform(float2 const& value, quaternion const& rotation)
    {
        float2 result;

        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float yy2 = rotation.y * y2;
        float zz2 = rotation.z * z2;

        result.x = value.x * (1.0f - yy2 - zz2) + value.y * (xy2 - wz2);
        result.y = value.x * (xy2 + wz2) + value.y * (1.0f - xx2 - zz2);

        return result;
    }


    inline float3::float3(float x, float y, float z)
        : x(x), y(y), z(z)
    { }


    inline float3::float3(float2 value, float z)
        : x(value.x), y(value.y), z(z)
    { }


    inline float3::float3(float value)
        : x(value), y(value), z(value)
    { }


    inline float3 float3::zero()
    {
        return float3(0, 0, 0);
    }


    inline float3 float3::one()
    {
        return float3(1, 1, 1);
    }


    inline float3 float3::unit_x()
    {
        return float3(1, 0, 0);
    }


    inline float3 float3::unit_y()
    {
        return float3(0, 1, 0);
    }


    inline float3 float3::unit_z()
    {
        return float3(0, 0, 1);
    }


    inline float3 operator +(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = value1.x + value2.x;
        ans.y = value1.y + value2.y;
        ans.z = value1.z + value2.z;

        return ans;
    }


    inline float3 operator -(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = value1.x - value2.x;
        ans.y = value1.y - value2.y;
        ans.z = value1.z - value2.z;

        return ans;
    }


    inline float3 operator *(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = value1.x * value2.x;
        ans.y = value1.y * value2.y;
        ans.z = value1.z * value2.z;

        return ans;
    }


    inline float3 operator *(float3 const& value1, float value2)
    {
        float3 ans;

        ans.x = value1.x * value2;
        ans.y = value1.y * value2;
        ans.z = value1.z * value2;

        return ans;
    }


    inline float3 operator *(float value1, float3 const& value2)
    {
        float3 ans;

        ans.x = value1 * value2.x;
        ans.y = value1 * value2.y;
        ans.z = value1 * value2.z;

        return ans;
    }


    inline float3 operator /(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = value1.x / value2.x;
        ans.y = value1.y / value2.y;
        ans.z = value1.z / value2.z;

        return ans;
    }


    inline float3 operator /(float3 const& value1, float value2)
    {
        float3 ans;

        float invDiv = 1.0f / value2;

        ans.x = value1.x * invDiv;
        ans.y = value1.y * invDiv;
        ans.z = value1.z * invDiv;

        return ans;
    }


    inline float3 operator -(float3 const& value)
    {
        float3 ans;

        ans.x = -value.x;
        ans.y = -value.y;
        ans.z = -value.z;

        return ans;
    }


    inline float3& operator +=(float3& value1, float3 const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline float3& operator -=(float3& value1, float3 const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline float3& operator *=(float3& value1, float3 const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float3& operator *=(float3& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float3& operator /=(float3& value1, float3 const& value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline float3& operator /=(float3& value1, float value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline bool operator ==(float3 const& value1, float3 const& value2)
    {
        return value1.x == value2.x &&
               value1.y == value2.y &&
               value1.z == value2.z;
    }


    inline bool operator !=(float3 const& value1, float3 const& value2)
    {
        return value1.x != value2.x ||
               value1.y != value2.y ||
               value1.z != value2.z;
    }


    inline float length(float3 const& value)
    {
        float ls = value.x * value.x + value.y * value.y + value.z * value.z;

        return sqrtf(ls);
    }


    inline float length_squared(float3 const& value)
    {
        return value.x * value.x + value.y * value.y + value.z * value.z;
    }


    inline float distance(float3 const& value1, float3 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;
        float dz = value1.z - value2.z;

        float ls = dx * dx + dy * dy + dz * dz;

        return sqrtf(ls);
    }


    inline float distance_squared(float3 const& value1, float3 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;
        float dz = value1.z - value2.z;

        return dx * dx + dy * dy + dz * dz;
    }


    inline float dot(float3 const& vector1, float3 const& vector2)
    {
        return vector1.x * vector2.x +
               vector1.y * vector2.y +
               vector1.z * vector2.z;
    }


    inline float3 normalize(float3 const& value)
    {
        float3 ans;

        float ls = value.x * value.x + value.y * value.y + value.z * value.z;
        float invNorm = 1.0f / sqrtf(ls);

        ans.x = value.x * invNorm;
        ans.y = value.y * invNorm;
        ans.z = value.z * invNorm;

        return ans;
    }


    inline float3 cross(float3 const& vector1, float3 const& vector2)
    {
        float3 ans;

        ans.x = vector1.y * vector2.z - vector1.z * vector2.y;
        ans.y = vector1.z * vector2.x - vector1.x * vector2.z;
        ans.z = vector1.x * vector2.y - vector1.y * vector2.x;

        return ans;
    }


    inline float3 reflect(float3 const& vector, float3 const& normal)
    {
        float3 ans;

        float dot = vector.x * normal.x + vector.y * normal.y + vector.z * normal.z;

        ans.x = vector.x - 2.0f * dot * normal.x;
        ans.y = vector.y - 2.0f * dot * normal.y;
        ans.z = vector.z - 2.0f * dot * normal.z;

        return ans;
    }


    inline float3 (min)(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = (value1.x < value2.x) ? value1.x : value2.x;
        ans.y = (value1.y < value2.y) ? value1.y : value2.y;
        ans.z = (value1.z < value2.z) ? value1.z : value2.z;

        return ans;
    }


    inline float3 (max)(float3 const& value1, float3 const& value2)
    {
        float3 ans;

        ans.x = (value1.x > value2.x) ? value1.x : value2.x;
        ans.y = (value1.y > value2.y) ? value1.y : value2.y;
        ans.z = (value1.z > value2.z) ? value1.z : value2.z;

        return ans;
    }


    inline float3 clamp(float3 const& value1, float3 const& min, float3 const& max)
    {
        float3 result;

        float x = value1.x;
        x = (x > max.x) ? max.x : x;
        x = (x < min.x) ? min.x : x;

        float y = value1.y;
        y = (y > max.y) ? max.y : y;
        y = (y < min.y) ? min.y : y;

        float z = value1.z;
        z = (z > max.z) ? max.z : z;
        z = (z < min.z) ? min.z : z;

        result.x = x;
        result.y = y;
        result.z = z;

        return result;
    }


    inline float3 lerp(float3 const& value1, float3 const& value2, float amount)
    {
        float3 ans;

        ans.x = value1.x + (value2.x - value1.x) * amount;
        ans.y = value1.y + (value2.y - value1.y) * amount;
        ans.z = value1.z + (value2.z - value1.z) * amount;

        return ans;
    }


    inline float3 transform(float3 const& position, float4x4 const& matrix)
    {
        float3 result;

        result.x = position.x * matrix.m11 + position.y * matrix.m21 + position.z * matrix.m31 + matrix.m41;
        result.y = position.x * matrix.m12 + position.y * matrix.m22 + position.z * matrix.m32 + matrix.m42;
        result.z = position.x * matrix.m13 + position.y * matrix.m23 + position.z * matrix.m33 + matrix.m43;

        return result;
    }


    inline float3 transform_normal(float3 const& normal, float4x4 const& matrix)
    {
        float3 result;

        result.x = normal.x * matrix.m11 + normal.y * matrix.m21 + normal.z * matrix.m31;
        result.y = normal.x * matrix.m12 + normal.y * matrix.m22 + normal.z * matrix.m32;
        result.z = normal.x * matrix.m13 + normal.y * matrix.m23 + normal.z * matrix.m33;

        return result;
    }


    inline float3 transform(float3 const& value, quaternion const& rotation)
    {
        float3 result;

        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        result.x = value.x * (1.0f - yy2 - zz2) + value.y * (xy2 - wz2) + value.z * (xz2 + wy2);
        result.y = value.x * (xy2 + wz2) + value.y * (1.0f - xx2 - zz2) + value.z * (yz2 - wx2);
        result.z = value.x * (xz2 - wy2) + value.y * (yz2 + wx2) + value.z * (1.0f - xx2 - yy2);

        return result;
    }


    inline float4::float4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    { }


    inline float4::float4(float2 value, float z, float w)
        : x(value.x), y(value.y), z(z), w(w)
    { }


    inline float4::float4(float3 value, float w)
        : x(value.x), y(value.y), z(value.z), w(w)
    { }


    inline float4::float4(float value)
        : x(value), y(value), z(value), w(value)
    { }


    inline float4 float4::zero()
    {
        return float4(0, 0, 0, 0);
    }


    inline float4 float4::one()
    {
        return float4(1, 1, 1, 1);
    }


    inline float4 float4::unit_x()
    {
        return float4(1, 0, 0, 0);
    }


    inline float4 float4::unit_y()
    {
        return float4(0, 1, 0, 0);
    }


    inline float4 float4::unit_z()
    {
        return float4(0, 0, 1, 0);
    }


    inline float4 float4::unit_w()
    {
        return float4(0, 0, 0, 1);
    }


    inline float4 operator +(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = value1.x + value2.x;
        ans.y = value1.y + value2.y;
        ans.z = value1.z + value2.z;
        ans.w = value1.w + value2.w;

        return ans;
    }


    inline float4 operator -(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = value1.x - value2.x;
        ans.y = value1.y - value2.y;
        ans.z = value1.z - value2.z;
        ans.w = value1.w - value2.w;

        return ans;
    }


    inline float4 operator *(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = value1.x * value2.x;
        ans.y = value1.y * value2.y;
        ans.z = value1.z * value2.z;
        ans.w = value1.w * value2.w;

        return ans;
    }


    inline float4 operator *(float4 const& value1, float value2)
    {
        float4 ans;

        ans.x = value1.x * value2;
        ans.y = value1.y * value2;
        ans.z = value1.z * value2;
        ans.w = value1.w * value2;

        return ans;
    }


    inline float4 operator *(float value1, float4 const& value2)
    {
        float4 ans;

        ans.x = value1 * value2.x;
        ans.y = value1 * value2.y;
        ans.z = value1 * value2.z;
        ans.w = value1 * value2.w;

        return ans;
    }


    inline float4 operator /(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = value1.x / value2.x;
        ans.y = value1.y / value2.y;
        ans.z = value1.z / value2.z;
        ans.w = value1.w / value2.w;

        return ans;
    }


    inline float4 operator /(float4 const& value1, float value2)
    {
        float4 ans;

        float invDiv = 1.0f / value2;
        
        ans.x = value1.x * invDiv;
        ans.y = value1.y * invDiv;
        ans.z = value1.z * invDiv;
        ans.w = value1.w * invDiv;

        return ans;
    }


    inline float4 operator -(float4 const& value)
    {
        float4 ans;

        ans.x = -value.x;
        ans.y = -value.y;
        ans.z = -value.z;
        ans.w = -value.w;

        return ans;
    }


    inline float4& operator +=(float4& value1, float4 const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline float4& operator -=(float4& value1, float4 const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline float4& operator *=(float4& value1, float4 const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float4& operator *=(float4& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float4& operator /=(float4& value1, float4 const& value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline float4& operator /=(float4& value1, float value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline bool operator ==(float4 const& value1, float4 const& value2)
    {
        return value1.x == value2.x &&
               value1.y == value2.y &&
               value1.z == value2.z &&
               value1.w == value2.w;
    }


    inline bool operator !=(float4 const& value1, float4 const& value2)
    {
        return value1.x != value2.x ||
               value1.y != value2.y ||
               value1.z != value2.z ||
               value1.w != value2.w;
    }


    inline float length(float4 const& value)
    {
        float ls = value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
        
        return sqrtf(ls);
    }


    inline float length_squared(float4 const& value)
    {
        return value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
    }


    inline float distance(float4 const& value1, float4 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;
        float dz = value1.z - value2.z;
        float dw = value1.w - value2.w;
        
        float ls = dx * dx + dy * dy + dz * dz + dw * dw;

        return sqrtf(ls);
    }


    inline float distance_squared(float4 const& value1, float4 const& value2)
    {
        float dx = value1.x - value2.x;
        float dy = value1.y - value2.y;
        float dz = value1.z - value2.z;
        float dw = value1.w - value2.w;
        
        return dx * dx + dy * dy + dz * dz + dw * dw;
    }


    inline float dot(float4 const& vector1, float4 const& vector2)
    {
        return vector1.x * vector2.x + 
               vector1.y * vector2.y + 
               vector1.z * vector2.z + 
               vector1.w * vector2.w;
    }


    inline float4 normalize(float4 const& vector)
    {
        float4 ans;

        float ls = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
        float invNorm = 1.0f / sqrtf(ls);

        ans.x = vector.x * invNorm;
        ans.y = vector.y * invNorm;
        ans.z = vector.z * invNorm;
        ans.w = vector.w * invNorm;

        return ans;
    }


    inline float4 (min)(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = (value1.x < value2.x) ? value1.x : value2.x;
        ans.y = (value1.y < value2.y) ? value1.y : value2.y;
        ans.z = (value1.z < value2.z) ? value1.z : value2.z;
        ans.w = (value1.w < value2.w) ? value1.w : value2.w;

        return ans;
    }


    inline float4 (max)(float4 const& value1, float4 const& value2)
    {
        float4 ans;

        ans.x = (value1.x > value2.x) ? value1.x : value2.x;
        ans.y = (value1.y > value2.y) ? value1.y : value2.y;
        ans.z = (value1.z > value2.z) ? value1.z : value2.z;
        ans.w = (value1.w > value2.w) ? value1.w : value2.w;

        return ans;
    }


    inline float4 clamp(float4 const& value1, float4 const& min, float4 const& max)
    {
        float4 result;

        float x = value1.x;
        x = (x > max.x) ? max.x : x;
        x = (x < min.x) ? min.x : x;

        float y = value1.y;
        y = (y > max.y) ? max.y : y;
        y = (y < min.y) ? min.y : y;

        float z = value1.z;
        z = (z > max.z) ? max.z : z;
        z = (z < min.z) ? min.z : z;

        float w = value1.w;
        w = (w > max.w) ? max.w : w;
        w = (w < min.w) ? min.w : w;

        result.x = x;
        result.y = y;
        result.z = z;
        result.w = w;

        return result;
    }


    inline float4 lerp(float4 const& value1, float4 const& value2, float amount)
    {
        float4 ans;

        ans.x = value1.x + (value2.x - value1.x) * amount;
        ans.y = value1.y + (value2.y - value1.y) * amount;
        ans.z = value1.z + (value2.z - value1.z) * amount;
        ans.w = value1.w + (value2.w - value1.w) * amount;

        return ans;
    }


    inline float4 transform(float4 const& vector, float4x4 const& matrix)
    {
        float4 result;

        result.x = vector.x * matrix.m11 + vector.y * matrix.m21 + vector.z * matrix.m31 + vector.w * matrix.m41;
        result.y = vector.x * matrix.m12 + vector.y * matrix.m22 + vector.z * matrix.m32 + vector.w * matrix.m42;
        result.z = vector.x * matrix.m13 + vector.y * matrix.m23 + vector.z * matrix.m33 + vector.w * matrix.m43;
        result.w = vector.x * matrix.m14 + vector.y * matrix.m24 + vector.z * matrix.m34 + vector.w * matrix.m44;

        return result;
    }


    inline float4 transform4(float3 const& position, float4x4 const& matrix)
    {
        float4 result;

        result.x = position.x * matrix.m11 + position.y * matrix.m21 + position.z * matrix.m31 + matrix.m41;
        result.y = position.x * matrix.m12 + position.y * matrix.m22 + position.z * matrix.m32 + matrix.m42;
        result.z = position.x * matrix.m13 + position.y * matrix.m23 + position.z * matrix.m33 + matrix.m43;
        result.w = position.x * matrix.m14 + position.y * matrix.m24 + position.z * matrix.m34 + matrix.m44;

        return result;
    }


    inline float4 transform4(float2 const& position, float4x4 const& matrix)
    {
        float4 result;

        result.x = position.x * matrix.m11 + position.y * matrix.m21 + matrix.m41;
        result.y = position.x * matrix.m12 + position.y * matrix.m22 + matrix.m42;
        result.z = position.x * matrix.m13 + position.y * matrix.m23 + matrix.m43;
        result.w = position.x * matrix.m14 + position.y * matrix.m24 + matrix.m44;

        return result;
    }


    inline float4 transform(float4 const& value, quaternion const& rotation)
    {
        float4 result;

        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        result.x = value.x * (1.0f - yy2 - zz2) + value.y * (       xy2 - wz2) + value.z * (       xz2 + wy2);
        result.y = value.x * (       xy2 + wz2) + value.y * (1.0f - xx2 - zz2) + value.z * (       yz2 - wx2);
        result.z = value.x * (       xz2 - wy2) + value.y * (       yz2 + wx2) + value.z * (1.0f - xx2 - yy2);
        result.w = value.w;

        return result;
    }


    inline float4 transform4(float3 const& value, quaternion const& rotation)
    {
        float4 result;

        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        result.x = value.x * (1.0f - yy2 - zz2) + value.y * (       xy2 - wz2) + value.z * (       xz2 + wy2);
        result.y = value.x * (       xy2 + wz2) + value.y * (1.0f - xx2 - zz2) + value.z * (       yz2 - wx2);
        result.z = value.x * (       xz2 - wy2) + value.y * (       yz2 + wx2) + value.z * (1.0f - xx2 - yy2);
        result.w = 1.0f;

        return result;
    }


    inline float4 transform4(float2 const& value, quaternion const& rotation)
    {
        float4 result;

        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        result.x = value.x * (1.0f - yy2 - zz2) + value.y * (       xy2 - wz2);
        result.y = value.x * (       xy2 + wz2) + value.y * (1.0f - xx2 - zz2);
        result.z = value.x * (       xz2 - wy2) + value.y * (       yz2 + wx2);
        result.w = 1.0f;

        return result;
    }


    inline float3x2::float3x2(float m11, float m12, float m21, float m22, float m31, float m32)
        : m11(m11), m12(m12), m21(m21), m22(m22), m31(m31), m32(m32)
    { }


    inline float3x2 float3x2::identity()
    {
        return float3x2(1, 0,
                        0, 1,
                        0, 0);
    }


    inline float3x2 make_float3x2_translation(float2 const& position)
    {
        float3x2 result;

        result.m11 = 1.0f; result.m12 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f;

        result.m31 = position.x;
        result.m32 = position.y;

        return result;
    }


    inline float3x2 make_float3x2_translation(float xPosition, float yPosition)
    {
        float3x2 result;

        result.m11 = 1.0f; result.m12 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f;

        result.m31 = xPosition;
        result.m32 = yPosition;

        return result;
    }


    inline float3x2 make_float3x2_scale(float xScale, float yScale)
    {
        float3x2 result;

        result.m11 = xScale; result.m12 = 0.0f;
        result.m21 = 0.0f;   result.m22 = yScale;
        result.m31 = 0.0f;   result.m32 = 0.0f;

        return result;
    }


    inline float3x2 make_float3x2_scale(float xScale, float yScale, float2 const& centerPoint)
    {
        float3x2 result;

        float tx = centerPoint.x * (1 - xScale);
        float ty = centerPoint.y * (1 - yScale);

        result.m11 = xScale; result.m12 = 0.0f;
        result.m21 = 0.0f;   result.m22 = yScale;
        result.m31 = tx;     result.m32 = ty;

        return result;
    }


    inline float3x2 make_float3x2_scale(float2 const& scales)
    {
        float3x2 result;

        result.m11 = scales.x; result.m12 = 0.0f;
        result.m21 = 0.0f;     result.m22 = scales.y;
        result.m31 = 0.0f;     result.m32 = 0.0f;
        
        return result;
    }


    inline float3x2 make_float3x2_scale(float2 const& scales, float2 const& centerPoint)
    {
        float3x2 result;

        float tx = centerPoint.x * (1 - scales.x);
        float ty = centerPoint.y * (1 - scales.y);

        result.m11 = scales.x; result.m12 = 0.0f;
        result.m21 = 0.0f;     result.m22 = scales.y;
        result.m31 = tx;       result.m32 = ty;

        return result;
    }


    inline float3x2 make_float3x2_scale(float scale)
    {
        float3x2 result;

        result.m11 = scale; result.m12 = 0.0f;
        result.m21 = 0.0f;  result.m22 = scale;
        result.m31 = 0.0f;  result.m32 = 0.0f;

        return result;
    }


    inline float3x2 make_float3x2_scale(float scale, float2 const& centerPoint)
    {
        float3x2 result;

        float tx = centerPoint.x * (1 - scale);
        float ty = centerPoint.y * (1 - scale);

        result.m11 = scale; result.m12 = 0.0f;
        result.m21 = 0.0f;  result.m22 = scale;
        result.m31 = tx;    result.m32 = ty;

        return result;
    }


    inline float3x2 make_float3x2_skew(float radiansX, float radiansY)
    {
        float3x2 result;

        float xTan = tanf(radiansX);
        float yTan = tanf(radiansY);

        result.m11 = 1.0f; result.m12 = yTan;
        result.m21 = xTan; result.m22 = 1.0f;
        result.m31 = 0.0f; result.m32 = 0.0f;

        return result;
    }


    inline float3x2 make_float3x2_skew(float radiansX, float radiansY, float2 const& centerPoint)
    {
        float3x2 result;

        float xTan = tanf(radiansX);
        float yTan = tanf(radiansY);

        float tx = -centerPoint.y * xTan;
        float ty = -centerPoint.x * yTan;

        result.m11 = 1.0f; result.m12 = yTan;
        result.m21 = xTan; result.m22 = 1.0f;
        result.m31 = tx;   result.m32 = ty;

        return result;
    }


    inline float3x2 make_float3x2_rotation(float radians)
    {
        float3x2 result;

        radians = fmodf(radians, DirectX::XM_2PI);

        if (radians < 0)
            radians += DirectX::XM_2PI;

        float c, s;

        const float epsilon = 0.001f * DirectX::XM_PI / 180.0f;     // 0.1% of a degree

        if (radians < epsilon || radians > DirectX::XM_2PI - epsilon)
        {
            // Exact case for zero rotation.
            c = 1;
            s = 0;
        }
        else if (radians > DirectX::XM_PIDIV2 - epsilon && radians < DirectX::XM_PIDIV2 + epsilon)
        {
            // Exact case for 90 degree rotation.
            c = 0;
            s = 1;
        }
        else if (radians > DirectX::XM_PI - epsilon && radians < DirectX::XM_PI + epsilon)
        {
            // Exact case for 180 degree rotation.
            c = -1;
            s = 0;
        }
        else if (radians > DirectX::XM_PI + DirectX::XM_PIDIV2 - epsilon && radians < DirectX::XM_PI + DirectX::XM_PIDIV2 + epsilon)
        {
            // Exact case for 270 degree rotation.
            c = 0;
            s = -1;
        }
        else
        {
            // Arbitrary rotation.
            c = cosf(radians);
            s = sinf(radians);
        }

        // [  c  s ]
        // [ -s  c ]
        // [  0  0 ]
        result.m11 =    c; result.m12 =    s;
        result.m21 =   -s; result.m22 =    c;
        result.m31 = 0.0f; result.m32 = 0.0f;

        return result;
    }


    inline float3x2 make_float3x2_rotation(float radians, float2 const& centerPoint)
    {
        float3x2 result;

        radians = fmodf(radians, DirectX::XM_2PI);

        if (radians < 0)
            radians += DirectX::XM_2PI;

        float c, s;

        const float epsilon = 0.001f * DirectX::XM_PI / 180.0f;     // 0.1% of a degree

        if (radians < epsilon || radians > DirectX::XM_2PI - epsilon)
        {
            // Exact case for zero rotation.
            c = 1;
            s = 0;
        }
        else if (radians > DirectX::XM_PIDIV2 - epsilon && radians < DirectX::XM_PIDIV2 + epsilon)
        {
            // Exact case for 90 degree rotation.
            c = 0;
            s = 1;
        }
        else if (radians > DirectX::XM_PI - epsilon && radians < DirectX::XM_PI + epsilon)
        {
            // Exact case for 180 degree rotation.
            c = -1;
            s = 0;
        }
        else if (radians > DirectX::XM_PI + DirectX::XM_PIDIV2 - epsilon && radians < DirectX::XM_PI + DirectX::XM_PIDIV2 + epsilon)
        {
            // Exact case for 270 degree rotation.
            c = 0;
            s = -1;
        }
        else
        {
            // Arbitrary rotation.
            c = cosf(radians);
            s = sinf(radians);
        }

        float x = centerPoint.x * (1 - c) + centerPoint.y * s;
        float y = centerPoint.y * (1 - c) - centerPoint.x * s;

        // [  c  s ]
        // [ -s  c ]
        // [  x  y ]
        result.m11 =  c; result.m12 = s;
        result.m21 = -s; result.m22 = c;
        result.m31 =  x; result.m32 = y;

        return result;
    }


    inline float3x2 operator +(float3x2 const& value1, float3x2 const& value2)
    {
        float3x2 m;

        m.m11 = value1.m11 + value2.m11; m.m12 = value1.m12 + value2.m12;
        m.m21 = value1.m21 + value2.m21; m.m22 = value1.m22 + value2.m22;
        m.m31 = value1.m31 + value2.m31; m.m32 = value1.m32 + value2.m32;

        return m;
    }


    inline float3x2 operator -(float3x2 const& value1, float3x2 const& value2)
    {
        float3x2 m;

        m.m11 = value1.m11 - value2.m11; m.m12 = value1.m12 - value2.m12;
        m.m21 = value1.m21 - value2.m21; m.m22 = value1.m22 - value2.m22;
        m.m31 = value1.m31 - value2.m31; m.m32 = value1.m32 - value2.m32;

        return m;
    }


    inline float3x2 operator *(float3x2 const& value1, float3x2 const& value2)
    {
        float3x2 m;

        // First row
        m.m11 = value1.m11 * value2.m11 + value1.m12 * value2.m21;
        m.m12 = value1.m11 * value2.m12 + value1.m12 * value2.m22;
        
        // Second row
        m.m21 = value1.m21 * value2.m11 + value1.m22 * value2.m21;
        m.m22 = value1.m21 * value2.m12 + value1.m22 * value2.m22;
        
        // Third row
        m.m31 = value1.m31 * value2.m11 + value1.m32 * value2.m21 + value2.m31;
        m.m32 = value1.m31 * value2.m12 + value1.m32 * value2.m22 + value2.m32;

        return m;
    }


    inline float3x2 operator *(float3x2 const& value1, float value2)
    {
        float3x2 m;

        m.m11 = value1.m11 * value2; m.m12 = value1.m12 * value2;
        m.m21 = value1.m21 * value2; m.m22 = value1.m22 * value2;
        m.m31 = value1.m31 * value2; m.m32 = value1.m32 * value2;

        return m;
    }


    inline float3x2 operator -(float3x2 const& value)
    {
        float3x2 m;

        m.m11 = -value.m11; m.m12 = -value.m12;
        m.m21 = -value.m21; m.m22 = -value.m22;
        m.m31 = -value.m31; m.m32 = -value.m32;

        return m;
    }


    inline float3x2& operator +=(float3x2& value1, float3x2 const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline float3x2& operator -=(float3x2& value1, float3x2 const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline float3x2& operator *=(float3x2& value1, float3x2 const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float3x2& operator *=(float3x2& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline bool operator ==(float3x2 const& value1, float3x2 const& value2)
    {
        return value1.m11 == value2.m11 && value1.m22 == value2.m22 && // Check diagonal element first for early out.
                                           value1.m12 == value2.m12 &&
               value1.m21 == value2.m21                             &&
               value1.m31 == value2.m31 && value1.m32 == value2.m32;
    }


    inline bool operator !=(float3x2 const& value1, float3x2 const& value2)
    {
        return value1.m11 != value2.m11 || value1.m12 != value2.m12 ||
               value1.m21 != value2.m21 || value1.m22 != value2.m22 ||
               value1.m31 != value2.m31 || value1.m32 != value2.m32;
    }


    inline bool is_identity(float3x2 const& value)
    {
        return value.m11 == 1 && value.m22 == 1 && // Check diagonal element first for early out.
                                 value.m12 == 0 &&
               value.m21 == 0                   &&
               value.m31 == 0 && value.m32 == 0;
    }


    inline float determinant(float3x2 const& value)
    {
        return (value.m11 * value.m22) - (value.m21 * value.m12);
    }


    inline float2 translation(float3x2 const& value)
    {
        float2 ans;
            
        ans.x = value.m31; 
        ans.y = value.m32;
            
        return ans;
    }


    inline bool invert(float3x2 const& matrix, _Out_ float3x2* result)
    {
        float det = (matrix.m11 * matrix.m22) - (matrix.m21 * matrix.m12);

        if (fabs(det) < FLT_EPSILON)
        {
            const float nan = std::numeric_limits<float>::quiet_NaN();
            *result = float3x2(nan, nan, nan, nan, nan, nan);
            return false;
        }

        float invDet = 1.0f / det;

        *result = float3x2
        (
            matrix.m22 * invDet,
            -matrix.m12 * invDet,
            -matrix.m21 * invDet,
            matrix.m11 * invDet,
            (matrix.m21 * matrix.m32 - matrix.m31 * matrix.m22) * invDet,
            (matrix.m31 * matrix.m12 - matrix.m11 * matrix.m32) * invDet
        );

        return true;
    }


    inline float3x2 lerp(float3x2 const& matrix1, float3x2 const& matrix2, float amount)
    {
        float3x2 result;
        
        // First row
        result.m11 = matrix1.m11 + (matrix2.m11 - matrix1.m11) * amount;
        result.m12 = matrix1.m12 + (matrix2.m12 - matrix1.m12) * amount;
        
        // Second row
        result.m21 = matrix1.m21 + (matrix2.m21 - matrix1.m21) * amount;
        result.m22 = matrix1.m22 + (matrix2.m22 - matrix1.m22) * amount;
        
        // Third row
        result.m31 = matrix1.m31 + (matrix2.m31 - matrix1.m31) * amount;
        result.m32 = matrix1.m32 + (matrix2.m32 - matrix1.m32) * amount;

        return result;
    }


    inline float4x4::float4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
        : m11(m11), m12(m12), m13(m13), m14(m14),
          m21(m21), m22(m22), m23(m23), m24(m24),
          m31(m31), m32(m32), m33(m33), m34(m34),
          m41(m41), m42(m42), m43(m43), m44(m44)
    { }


    inline float4x4::float4x4(float3x2 value)
        : m11(value.m11), m12(value.m12), m13(0), m14(0),
          m21(value.m21), m22(value.m22), m23(0), m24(0),
          m31(0),         m32(0),         m33(1), m34(0),
          m41(value.m31), m42(value.m32), m43(0), m44(1)
    { }


    inline float4x4 float4x4::identity()
    {
        return float4x4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1);
    }


    inline float4x4 make_float4x4_billboard(float3 const& objectPosition, float3 const& cameraPosition, float3 const& cameraUpVector, float3 const& cameraForwardVector)
    {
        const float epsilon = 1e-4f;

        float3 zaxis;

        zaxis.x = objectPosition.x - cameraPosition.x;
        zaxis.y = objectPosition.y - cameraPosition.y;
        zaxis.z = objectPosition.z - cameraPosition.z;

        float norm = length_squared(zaxis);

        if (norm < epsilon)
        {
            zaxis = -cameraForwardVector;
        }
        else
        {
            zaxis = zaxis / sqrtf(norm);
        }

        float3 xaxis = normalize(cross(cameraUpVector, zaxis));

        float3 yaxis = cross(zaxis, xaxis);

        float4x4 result;

        result.m11 = xaxis.x; result.m12 = xaxis.y; result.m13 = xaxis.z; result.m14 = 0.0f;
        result.m21 = yaxis.x; result.m22 = yaxis.y; result.m23 = yaxis.z; result.m24 = 0.0f;
        result.m31 = zaxis.x; result.m32 = zaxis.y; result.m33 = zaxis.z; result.m34 = 0.0f;

        result.m41 = objectPosition.x;
        result.m42 = objectPosition.y;
        result.m43 = objectPosition.z;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_constrained_billboard(float3 const& objectPosition, float3 const& cameraPosition, float3 const& rotateAxis, float3 const& cameraForwardVector, float3 const& objectForwardVector)
    {
        const float epsilon = 1e-4f;
        const float minAngle = 1.0f - (0.1f * (DirectX::XM_PI / 180.0f)); // 0.1 degrees

        // Treat the case when object and camera positions are too close.
        float3 faceDir;

        faceDir.x = objectPosition.x - cameraPosition.x;
        faceDir.y = objectPosition.y - cameraPosition.y;
        faceDir.z = objectPosition.z - cameraPosition.z;

        float norm = length_squared(faceDir);

        if (norm < epsilon)
        {
            faceDir = -cameraForwardVector;
        }
        else
        {
            faceDir = faceDir / sqrtf(norm);
        }

        float3 yaxis = rotateAxis;
        float3 xaxis;
        float3 zaxis;

        // Treat the case when angle between faceDir and rotateAxis is too close to 0.
        if (fabs(dot(rotateAxis, faceDir)) > minAngle)
        {
            zaxis = objectForwardVector;

            if (fabs(dot(rotateAxis, zaxis)) > minAngle)
            {
                zaxis = (fabs(rotateAxis.z) > minAngle) ? float3(1, 0, 0) : float3(0, 0, -1);
            }

            xaxis = normalize(cross(rotateAxis, zaxis));
            zaxis = normalize(cross(xaxis, rotateAxis));
        }
        else
        {
            xaxis = normalize(cross(rotateAxis, faceDir));
            zaxis = normalize(cross(xaxis, yaxis));
        }

        float4x4 result;

        result.m11 = xaxis.x; result.m12 = xaxis.y; result.m13 = xaxis.z; result.m14 = 0.0f;
        result.m21 = yaxis.x; result.m22 = yaxis.y; result.m23 = yaxis.z; result.m24 = 0.0f;
        result.m31 = zaxis.x; result.m32 = zaxis.y; result.m33 = zaxis.z; result.m34 = 0.0f;

        result.m41 = objectPosition.x;
        result.m42 = objectPosition.y;
        result.m43 = objectPosition.z;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_translation(float3 const& position)
    {
        float4x4 result;

        result.m11 = 1.0f; result.m12 = 0.0f; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 = 0.0f; result.m33 = 1.0f; result.m34 = 0.0f;

        result.m41 = position.x;
        result.m42 = position.y;
        result.m43 = position.z;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_translation(float xPosition, float yPosition, float zPosition)
    {
        float4x4 result;

        result.m11 = 1.0f; result.m12 = 0.0f; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 = 0.0f; result.m33 = 1.0f; result.m34 = 0.0f;

        result.m41 = xPosition;
        result.m42 = yPosition;
        result.m43 = zPosition;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float xScale, float yScale, float zScale)
    {
        float4x4 result;
        
        result.m11 = xScale; result.m12 = 0.0f;   result.m13 = 0.0f;   result.m14 = 0.0f;
        result.m21 = 0.0f;   result.m22 = yScale; result.m23 = 0.0f;   result.m24 = 0.0f;
        result.m31 = 0.0f;   result.m32 = 0.0f;   result.m33 = zScale; result.m34 = 0.0f;
        result.m41 = 0.0f;   result.m42 = 0.0f;   result.m43 = 0.0f;   result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float xScale, float yScale, float zScale, float3 const& centerPoint)
    { 
        float4x4 result;

        float tx = centerPoint.x * (1 - xScale);
        float ty = centerPoint.y * (1 - yScale);
        float tz = centerPoint.z * (1 - zScale);

        result.m11 = xScale; result.m12 = 0.0f;   result.m13 = 0.0f;   result.m14 = 0.0f;
        result.m21 = 0.0f;   result.m22 = yScale; result.m23 = 0.0f;   result.m24 = 0.0f;
        result.m31 = 0.0f;   result.m32 = 0.0f;   result.m33 = zScale; result.m34 = 0.0f;
        result.m41 = tx;     result.m42 = ty;     result.m43 = tz;     result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float3 const& scales)
    {
        float4x4 result;

        result.m11 = scales.x; result.m12 = 0.0f;     result.m13 = 0.0f;     result.m14 = 0.0f;
        result.m21 = 0.0f;     result.m22 = scales.y; result.m23 = 0.0f;     result.m24 = 0.0f;
        result.m31 = 0.0f;     result.m32 = 0.0f;     result.m33 = scales.z; result.m34 = 0.0f;
        result.m41 = 0.0f;     result.m42 = 0.0f;     result.m43 = 0.0f;     result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float3 const& scales, float3 const& centerPoint)
    {
        float4x4 result;

        float tx = centerPoint.x * (1 - scales.x);
        float ty = centerPoint.y * (1 - scales.y);
        float tz = centerPoint.z * (1 - scales.z);

        result.m11 = scales.x; result.m12 = 0.0f;     result.m13 = 0.0f;     result.m14 = 0.0f;
        result.m21 = 0.0f;     result.m22 = scales.y; result.m23 = 0.0f;     result.m24 = 0.0f;
        result.m31 = 0.0f;     result.m32 = 0.0f;     result.m33 = scales.z; result.m34 = 0.0f;
        result.m41 = tx;       result.m42 = ty;       result.m43 = tz;       result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float scale)
    {
        float4x4 result;

        result.m11 = scale; result.m12 = 0.0f;  result.m13 = 0.0f;  result.m14 = 0.0f;
        result.m21 = 0.0f;  result.m22 = scale; result.m23 = 0.0f;  result.m24 = 0.0f;
        result.m31 = 0.0f;  result.m32 = 0.0f;  result.m33 = scale; result.m34 = 0.0f;
        result.m41 = 0.0f;  result.m42 = 0.0f;  result.m43 = 0.0f;  result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_scale(float scale, float3 const& centerPoint)
    {
        float4x4 result;

        float tx = centerPoint.x * (1 - scale);
        float ty = centerPoint.y * (1 - scale);
        float tz = centerPoint.z * (1 - scale);

        result.m11 = scale; result.m12 = 0.0f;  result.m13 = 0.0f;  result.m14 = 0.0f;
        result.m21 = 0.0f;  result.m22 = scale; result.m23 = 0.0f;  result.m24 = 0.0f;
        result.m31 = 0.0f;  result.m32 = 0.0f;  result.m33 = scale; result.m34 = 0.0f;
        result.m41 = tx;    result.m42 = ty;    result.m43 = tz;    result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_x(float radians)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        // [  1  0  0  0 ]
        // [  0  c  s  0 ]
        // [  0 -s  c  0 ]
        // [  0  0  0  1 ]
        result.m11 = 1.0f; result.m12 = 0.0f; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 =    c; result.m23 =    s; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 =   -s; result.m33 =    c; result.m34 = 0.0f;
        result.m41 = 0.0f; result.m42 = 0.0f; result.m43 = 0.0f; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_x(float radians, float3 const& centerPoint)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        float y = centerPoint.y * (1 - c) + centerPoint.z * s;
        float z = centerPoint.z * (1 - c) - centerPoint.y * s;

        // [  1  0  0  0 ]
        // [  0  c  s  0 ]
        // [  0 -s  c  0 ]
        // [  0  y  z  1 ]
        result.m11 = 1.0f; result.m12 = 0.0f; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 =    c; result.m23 =    s; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 =   -s; result.m33 =    c; result.m34 = 0.0f;
        result.m41 = 0.0f; result.m42 =    y; result.m43 =    z; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_y(float radians)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        // [  c  0 -s  0 ]
        // [  0  1  0  0 ]
        // [  s  0  c  0 ]
        // [  0  0  0  1 ]
        result.m11 =    c; result.m12 = 0.0f; result.m13 =   -s; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 =    s; result.m32 = 0.0f; result.m33 =    c; result.m34 = 0.0f;
        result.m41 = 0.0f; result.m42 = 0.0f; result.m43 = 0.0f; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_y(float radians, float3 const& centerPoint)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        float x = centerPoint.x * (1 - c) - centerPoint.z * s;
        float z = centerPoint.z * (1 - c) + centerPoint.x * s;

        // [  c  0 -s  0 ]
        // [  0  1  0  0 ]
        // [  s  0  c  0 ]
        // [  x  0  z  1 ]
        result.m11 =    c; result.m12 = 0.0f; result.m13 =   -s; result.m14 = 0.0f;
        result.m21 = 0.0f; result.m22 = 1.0f; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 =    s; result.m32 = 0.0f; result.m33 =    c; result.m34 = 0.0f;
        result.m41 =    x; result.m42 = 0.0f; result.m43 =    z; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_z(float radians)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        // [  c  s  0  0 ]
        // [ -s  c  0  0 ]
        // [  0  0  1  0 ]
        // [  0  0  0  1 ]
        result.m11 =    c; result.m12 =    s; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 =   -s; result.m22 =    c; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 = 0.0f; result.m33 = 1.0f; result.m34 = 0.0f;
        result.m41 = 0.0f; result.m42 = 0.0f; result.m43 = 0.0f; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_rotation_z(float radians, float3 const& centerPoint)
    {
        float4x4 result;

        float c = cosf(radians);
        float s = sinf(radians);

        float x = centerPoint.x * (1 - c) + centerPoint.y * s;
        float y = centerPoint.y * (1 - c) - centerPoint.x * s;

        // [  c  s  0  0 ]
        // [ -s  c  0  0 ]
        // [  0  0  1  0 ]
        // [  x  y  0  1 ]
        result.m11 =    c; result.m12 =    s; result.m13 = 0.0f; result.m14 = 0.0f;
        result.m21 =   -s; result.m22 =    c; result.m23 = 0.0f; result.m24 = 0.0f;
        result.m31 = 0.0f; result.m32 = 0.0f; result.m33 = 1.0f; result.m34 = 0.0f;
        result.m41 =    x; result.m42 =    y; result.m43 = 0.0f; result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_from_axis_angle(float3 const& axis, float angle)
    {
        // a: angle
        // x, y, z: unit vector for axis.
        //
        // Rotation matrix M can be computed by below equation.
        //
        //  M = uu + (cos a)( I-uu ) + (sin a)S
        //
        // Where:
        //
        //  u = ( x, y, z )
        //
        //      [  0 -z  y ]
        //  S = [  z  0 -x ]
        //      [ -y  x  0 ]
        //
        //      [ 1 0 0 ]
        //  I = [ 0 1 0 ]
        //      [ 0 0 1 ]
        //
        //     [  xx+cosa*(1-xx)   yx-cosa*yx-sina*z zx-cosa*xz+sina*y ]
        // M = [ xy-cosa*yx+sina*z    yy+cosa(1-yy)  yz-cosa*yz-sina*x ]
        //     [ zx-cosa*zx-sina*y zy-cosa*zy+sina*x   zz+cosa*(1-zz)  ]
        //
        float x = axis.x, y = axis.y, z = axis.z;
        float sa = sinf(angle), ca = cosf(angle);
        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;

        float4x4 result;

        result.m11 = xx + ca * ( 1.0f - xx );   result.m12 = xy - ca * xy + sa * z;     result.m13 = xz - ca * xz - sa * y;     result.m14 = 0.0f;
        result.m21 = xy - ca * xy - sa * z;     result.m22 = yy + ca * ( 1.0f - yy );   result.m23 = yz - ca * yz + sa * x;     result.m24 = 0.0f;
        result.m31 = xz - ca * xz + sa * y;     result.m32 = yz - ca * yz - sa * x;     result.m33 = zz + ca * ( 1.0f - zz );   result.m34 = 0.0f;
        result.m41 = 0.0f;                      result.m42 = 0.0f;                      result.m43 = 0.0f;                      result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_perspective_field_of_view(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
    {
        if (fieldOfView <= 0.0f || fieldOfView >= DirectX::XM_PI)
            throw std::invalid_argument("fieldOfView");

        if (nearPlaneDistance <= 0.0f)
            throw std::invalid_argument("nearPlaneDistance");

        if (farPlaneDistance <= 0.0f)
            throw std::invalid_argument("farPlaneDistance");

        if (nearPlaneDistance >= farPlaneDistance )
            throw std::invalid_argument("nearPlaneDistance");

        float yScale = 1.0f / tanf(fieldOfView * 0.5f);
        float xScale = yScale / aspectRatio;

        float4x4 result;

        result.m11 = xScale;
        result.m12 = result.m13 = result.m14 = 0.0f;

        result.m22 = yScale;
        result.m21 = result.m23 = result.m24 = 0.0f;

        result.m31 = result.m32 = 0.0f;
        result.m33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
        result.m34 = -1.0f;

        result.m41 = result.m42 = result.m44 = 0.0f;
        result.m43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

        return result;
    }


    inline float4x4 make_float4x4_perspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
    {
        if (nearPlaneDistance <= 0.0f)
            throw std::invalid_argument("nearPlaneDistance");

        if (farPlaneDistance <= 0.0f)
            throw std::invalid_argument("farPlaneDistance");

        if (nearPlaneDistance >= farPlaneDistance)
            throw std::invalid_argument("nearPlaneDistance");

        float4x4 result;

        result.m11 = 2.0f * nearPlaneDistance / width;
        result.m12 = result.m13 = result.m14 = 0.0f;

        result.m22 = 2.0f * nearPlaneDistance / height;
        result.m21 = result.m23 = result.m24 = 0.0f;

        result.m33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
        result.m31 = result.m32 = 0.0f;
        result.m34 = -1.0f;

        result.m41 = result.m42 = result.m44 = 0.0f;
        result.m43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

        return result;
    }


    inline float4x4 make_float4x4_perspective_off_center(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
    {
        if (nearPlaneDistance <= 0.0f)
            throw std::invalid_argument("nearPlaneDistance");

        if (farPlaneDistance <= 0.0f)
            throw std::invalid_argument("farPlaneDistance");

        if (nearPlaneDistance >= farPlaneDistance)
            throw std::invalid_argument("nearPlaneDistance");

        float4x4 result;

        result.m11 = 2.0f * nearPlaneDistance / (right - left);
        result.m12 = result.m13 = result.m14 = 0.0f;

        result.m22 = 2.0f * nearPlaneDistance / (top - bottom);
        result.m21 = result.m23 = result.m24 = 0.0f;

        result.m31 = (left + right) / (right - left);
        result.m32 = (top + bottom) / (top - bottom);
        result.m33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
        result.m34 = -1.0f;

        result.m43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
        result.m41 = result.m42 = result.m44 = 0.0f;

        return result;
    }


    inline float4x4 make_float4x4_orthographic(float width, float height, float zNearPlane, float zFarPlane)
    {
        float4x4 result;

        result.m11 = 2.0f / width;
        result.m12 = result.m13 = result.m14 = 0.0f;

        result.m22 = 2.0f / height;
        result.m21 = result.m23 = result.m24 = 0.0f;

        result.m33 = 1.0f / (zNearPlane - zFarPlane);
        result.m31 = result.m32 = result.m34 = 0.0f;

        result.m41 = result.m42 = 0.0f;
        result.m43 = zNearPlane / (zNearPlane - zFarPlane);
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_orthographic_off_center(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
    {
        float4x4 result;

        result.m11 = 2.0f / (right - left);
        result.m12 = result.m13 = result.m14 = 0.0f;

        result.m22 = 2.0f / (top - bottom);
        result.m21 = result.m23 = result.m24 = 0.0f;

        result.m33 = 1.0f / (zNearPlane - zFarPlane);
        result.m31 = result.m32 = result.m34 = 0.0f;

        result.m41 = (left + right) / (left - right);
        result.m42 = (top + bottom) / (bottom - top);
        result.m43 = zNearPlane / (zNearPlane - zFarPlane);
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_look_at(float3 const& cameraPosition, float3 const& cameraTarget, float3 const& cameraUpVector)
    {
        float3 zaxis = normalize(cameraPosition - cameraTarget);
        float3 xaxis = normalize(cross(cameraUpVector, zaxis));
        float3 yaxis = cross(zaxis, xaxis);

        float4x4 result;

        result.m11 = xaxis.x; result.m12 = yaxis.x; result.m13 = zaxis.x; result.m14 = 0.0f;
        result.m21 = xaxis.y; result.m22 = yaxis.y; result.m23 = zaxis.y; result.m24 = 0.0f;
        result.m31 = xaxis.z; result.m32 = yaxis.z; result.m33 = zaxis.z; result.m34 = 0.0f;
        result.m41 = -dot(xaxis, cameraPosition);
        result.m42 = -dot(yaxis, cameraPosition);
        result.m43 = -dot(zaxis, cameraPosition);
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_world(float3 const& position, float3 const& forward, float3 const& up)
    {
        float3 zaxis = normalize(-forward);
        float3 xaxis = normalize(cross(up, zaxis));
        float3 yaxis = cross(zaxis, xaxis);

        float4x4 result;

        result.m11 = xaxis.x; result.m12 = xaxis.y; result.m13 = xaxis.z; result.m14 = 0.0f;
        result.m21 = yaxis.x; result.m22 = yaxis.y; result.m23 = yaxis.z; result.m24 = 0.0f;
        result.m31 = zaxis.x; result.m32 = zaxis.y; result.m33 = zaxis.z; result.m34 = 0.0f;
        result.m41 = position.x;
        result.m42 = position.y;
        result.m43 = position.z;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_from_quaternion(quaternion const& quaternion)
    {
        float4x4 result;

        float xx = quaternion.x * quaternion.x;
        float yy = quaternion.y * quaternion.y;
        float zz = quaternion.z * quaternion.z;

        float xy = quaternion.x * quaternion.y;
        float wz = quaternion.z * quaternion.w;
        float xz = quaternion.z * quaternion.x;
        float wy = quaternion.y * quaternion.w;
        float yz = quaternion.y * quaternion.z;
        float wx = quaternion.x * quaternion.w;

        result.m11 = 1.0f - 2.0f * (yy + zz);   result.m12 = 2.0f * (xy + wz);          result.m13 = 2.0f * (xz - wy);          result.m14 = 0.0f;
        result.m21 = 2.0f * (xy - wz);          result.m22 = 1.0f - 2.0f * (zz + xx);   result.m23 = 2.0f * (yz + wx);          result.m24 = 0.0f;
        result.m31 = 2.0f * (xz + wy);          result.m32 = 2.0f * (yz - wx);          result.m33 = 1.0f - 2.0f * (yy + xx);   result.m34 = 0.0f;
        result.m41 = 0.0f;                      result.m42 = 0.0f;                      result.m43 = 0.0f;                      result.m44 = 1.0f;

        return result;
    }


    inline float4x4 make_float4x4_from_yaw_pitch_roll(float yaw, float pitch, float roll)
    {
        quaternion q = make_quaternion_from_yaw_pitch_roll(yaw, pitch, roll);

        return make_float4x4_from_quaternion(q);
    }


    inline float4x4 make_float4x4_shadow(float3 const& lightDirection, plane const& plane)
    {
        auto p = normalize(plane);

        float dot = p.normal.x * lightDirection.x + p.normal.y * lightDirection.y + p.normal.z * lightDirection.z;
        float a = -p.normal.x;
        float b = -p.normal.y;
        float c = -p.normal.z;
        float d = -p.d;

        float4x4 result;

        result.m11 = a * lightDirection.x + dot;
        result.m21 = b * lightDirection.x;
        result.m31 = c * lightDirection.x;
        result.m41 = d * lightDirection.x;

        result.m12 = a * lightDirection.y;
        result.m22 = b * lightDirection.y + dot;
        result.m32 = c * lightDirection.y;
        result.m42 = d * lightDirection.y;

        result.m13 = a * lightDirection.z;
        result.m23 = b * lightDirection.z;
        result.m33 = c * lightDirection.z + dot;
        result.m43 = d * lightDirection.z;

        result.m14 = 0.0f;
        result.m24 = 0.0f;
        result.m34 = 0.0f;
        result.m44 = dot;

        return result;
    }


    inline float4x4 make_float4x4_reflection(plane const& value)
    {
        plane v = normalize(value);

        float a = v.normal.x;
        float b = v.normal.y;
        float c = v.normal.z;

        float fa = -2.0f * a;
        float fb = -2.0f * b;
        float fc = -2.0f * c;

        float4x4 result;

        result.m11 = fa * a + 1.0f;
        result.m12 = fb * a;
        result.m13 = fc * a;
        result.m14 = 0.0f;

        result.m21 = fa * b;
        result.m22 = fb * b + 1.0f;
        result.m23 = fc * b;
        result.m24 = 0.0f;

        result.m31 = fa * c;
        result.m32 = fb * c;
        result.m33 = fc * c + 1.0f;
        result.m34 = 0.0f;

        result.m41 = fa * v.d;
        result.m42 = fb * v.d;
        result.m43 = fc * v.d;
        result.m44 = 1.0f;

        return result;
    }


    inline float4x4 operator +(float4x4 const& value1, float4x4 const& value2)
    {
        float4x4 m;

        m.m11 = value1.m11 + value2.m11; m.m12 = value1.m12 + value2.m12; m.m13 = value1.m13 + value2.m13; m.m14 = value1.m14 + value2.m14;
        m.m21 = value1.m21 + value2.m21; m.m22 = value1.m22 + value2.m22; m.m23 = value1.m23 + value2.m23; m.m24 = value1.m24 + value2.m24;
        m.m31 = value1.m31 + value2.m31; m.m32 = value1.m32 + value2.m32; m.m33 = value1.m33 + value2.m33; m.m34 = value1.m34 + value2.m34;
        m.m41 = value1.m41 + value2.m41; m.m42 = value1.m42 + value2.m42; m.m43 = value1.m43 + value2.m43; m.m44 = value1.m44 + value2.m44;

        return m;
    }


    inline float4x4 operator -(float4x4 const& value1, float4x4 const& value2)
    {
        float4x4 m;

        m.m11 = value1.m11 - value2.m11; m.m12 = value1.m12 - value2.m12; m.m13 = value1.m13 - value2.m13; m.m14 = value1.m14 - value2.m14;
        m.m21 = value1.m21 - value2.m21; m.m22 = value1.m22 - value2.m22; m.m23 = value1.m23 - value2.m23; m.m24 = value1.m24 - value2.m24;
        m.m31 = value1.m31 - value2.m31; m.m32 = value1.m32 - value2.m32; m.m33 = value1.m33 - value2.m33; m.m34 = value1.m34 - value2.m34;
        m.m41 = value1.m41 - value2.m41; m.m42 = value1.m42 - value2.m42; m.m43 = value1.m43 - value2.m43; m.m44 = value1.m44 - value2.m44;

        return m;
    }


    inline float4x4 operator *(float4x4 const& value1, float4x4 const& value2)
    {
        float4x4 m;

        // First row
        m.m11 = value1.m11 * value2.m11 + value1.m12 * value2.m21 + value1.m13 * value2.m31 + value1.m14 * value2.m41;
        m.m12 = value1.m11 * value2.m12 + value1.m12 * value2.m22 + value1.m13 * value2.m32 + value1.m14 * value2.m42;
        m.m13 = value1.m11 * value2.m13 + value1.m12 * value2.m23 + value1.m13 * value2.m33 + value1.m14 * value2.m43;
        m.m14 = value1.m11 * value2.m14 + value1.m12 * value2.m24 + value1.m13 * value2.m34 + value1.m14 * value2.m44;
        
        // Second row
        m.m21 = value1.m21 * value2.m11 + value1.m22 * value2.m21 + value1.m23 * value2.m31 + value1.m24 * value2.m41;
        m.m22 = value1.m21 * value2.m12 + value1.m22 * value2.m22 + value1.m23 * value2.m32 + value1.m24 * value2.m42;
        m.m23 = value1.m21 * value2.m13 + value1.m22 * value2.m23 + value1.m23 * value2.m33 + value1.m24 * value2.m43;
        m.m24 = value1.m21 * value2.m14 + value1.m22 * value2.m24 + value1.m23 * value2.m34 + value1.m24 * value2.m44;
        
        // Third row
        m.m31 = value1.m31 * value2.m11 + value1.m32 * value2.m21 + value1.m33 * value2.m31 + value1.m34 * value2.m41;
        m.m32 = value1.m31 * value2.m12 + value1.m32 * value2.m22 + value1.m33 * value2.m32 + value1.m34 * value2.m42;
        m.m33 = value1.m31 * value2.m13 + value1.m32 * value2.m23 + value1.m33 * value2.m33 + value1.m34 * value2.m43;
        m.m34 = value1.m31 * value2.m14 + value1.m32 * value2.m24 + value1.m33 * value2.m34 + value1.m34 * value2.m44;
        
        // Fourth row
        m.m41 = value1.m41 * value2.m11 + value1.m42 * value2.m21 + value1.m43 * value2.m31 + value1.m44 * value2.m41;
        m.m42 = value1.m41 * value2.m12 + value1.m42 * value2.m22 + value1.m43 * value2.m32 + value1.m44 * value2.m42;
        m.m43 = value1.m41 * value2.m13 + value1.m42 * value2.m23 + value1.m43 * value2.m33 + value1.m44 * value2.m43;
        m.m44 = value1.m41 * value2.m14 + value1.m42 * value2.m24 + value1.m43 * value2.m34 + value1.m44 * value2.m44;

        return m;
    }


    inline float4x4 operator *(float4x4 const& value1, float value2)
    {
        float4x4 m;

        m.m11 = value1.m11 * value2; m.m12 = value1.m12 * value2; m.m13 = value1.m13 * value2; m.m14 = value1.m14 * value2;
        m.m21 = value1.m21 * value2; m.m22 = value1.m22 * value2; m.m23 = value1.m23 * value2; m.m24 = value1.m24 * value2;
        m.m31 = value1.m31 * value2; m.m32 = value1.m32 * value2; m.m33 = value1.m33 * value2; m.m34 = value1.m34 * value2;
        m.m41 = value1.m41 * value2; m.m42 = value1.m42 * value2; m.m43 = value1.m43 * value2; m.m44 = value1.m44 * value2;

        return m;
    }


    inline float4x4 operator -(float4x4 const& value)
    {
        float4x4 m;

        m.m11 = -value.m11; m.m12 = -value.m12; m.m13 = -value.m13; m.m14 = -value.m14;
        m.m21 = -value.m21; m.m22 = -value.m22; m.m23 = -value.m23; m.m24 = -value.m24;
        m.m31 = -value.m31; m.m32 = -value.m32; m.m33 = -value.m33; m.m34 = -value.m34;
        m.m41 = -value.m41; m.m42 = -value.m42; m.m43 = -value.m43; m.m44 = -value.m44;

        return m;
    }


    inline float4x4& operator +=(float4x4& value1, float4x4 const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline float4x4& operator -=(float4x4& value1, float4x4 const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline float4x4& operator *=(float4x4& value1, float4x4 const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline float4x4& operator *=(float4x4& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline bool operator ==(float4x4 const& value1, float4x4 const& value2)
    {
        return value1.m11 == value2.m11 && value1.m22 == value2.m22 && value1.m33 == value2.m33 && value1.m44 == value2.m44 && // Check diagonal element first for early out.
                                           value1.m12 == value2.m12 && value1.m13 == value2.m13 && value1.m14 == value2.m14 &&
               value1.m21 == value2.m21                             && value1.m23 == value2.m23 && value1.m24 == value2.m24 &&
               value1.m31 == value2.m31 && value1.m32 == value2.m32                             && value1.m34 == value2.m34 &&
               value1.m41 == value2.m41 && value1.m42 == value2.m42 && value1.m43 == value2.m43;
    }


    inline bool operator !=(float4x4 const& value1, float4x4 const& value2)
    {
        return value1.m11 != value2.m11 || value1.m12 != value2.m12 || value1.m13 != value2.m13 || value1.m14 != value2.m14 ||
               value1.m21 != value2.m21 || value1.m22 != value2.m22 || value1.m23 != value2.m23 || value1.m24 != value2.m24 ||
               value1.m31 != value2.m31 || value1.m32 != value2.m32 || value1.m33 != value2.m33 || value1.m34 != value2.m34 ||
               value1.m41 != value2.m41 || value1.m42 != value2.m42 || value1.m43 != value2.m43 || value1.m44 != value2.m44;
    }


    inline bool is_identity(float4x4 const& value)
    {
        return value.m11 == 1 && value.m22 == 1 && value.m33 == 1 && value.m44 == 1 && // Check diagonal element first for early out.
                                 value.m12 == 0 && value.m13 == 0 && value.m14 == 0 &&
               value.m21 == 0                   && value.m23 == 0 && value.m24 == 0 &&
               value.m31 == 0 && value.m32 == 0                   && value.m34 == 0 &&
               value.m41 == 0 && value.m42 == 0 && value.m43 == 0;
    }


    inline float determinant(float4x4 const& value)
    {
        // | a b c d |     | f g h |     | e g h |     | e f h |     | e f g |
        // | e f g h | = a | j k l | - b | i k l | + c | i j l | - d | i j k |
        // | i j k l |     | n o p |     | m o p |     | m n p |     | m n o |
        // | m n o p |
        //
        //   | f g h |
        // a | j k l | = a ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
        //   | n o p |
        //
        //   | e g h |     
        // b | i k l | = b ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
        //   | m o p |     
        //
        //   | e f h |
        // c | i j l | = c ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
        //   | m n p |
        //
        //   | e f g |
        // d | i j k | = d ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
        //   | m n o |

        float a = value.m11, b = value.m12, c = value.m13, d = value.m14;
        float e = value.m21, f = value.m22, g = value.m23, h = value.m24;
        float i = value.m31, j = value.m32, k = value.m33, l = value.m34;
        float m = value.m41, n = value.m42, o = value.m43, p = value.m44;

        float kp_lo = k * p - l * o;
        float jp_ln = j * p - l * n;
        float jo_kn = j * o - k * n;
        float ip_lm = i * p - l * m;
        float io_km = i * o - k * m;
        float in_jm = i * n - j * m;

        return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
               b * (e * kp_lo - g * ip_lm + h * io_km) +
               c * (e * jp_ln - f * ip_lm + h * in_jm) -
               d * (e * jo_kn - f * io_km + g * in_jm);
    }


    inline float3 translation(float4x4 const& value)
    {
        float3 ans;
            
        ans.x = value.m41; 
        ans.y = value.m42;
        ans.z = value.m43;
            
        return ans;
    }


    inline bool invert(float4x4 const& matrix, _Out_ float4x4* result)
    {
        //                                       -1
        // If you have matrix M, inverse Matrix M   can compute
        //
        //     -1       1      
        //    M   = --------- A
        //            det(M)
        //
        // A is adjugate (adjoint) of M, where,
        //
        //      T
        // A = C
        //
        // C is Cofactor matrix of M, where,
        //           i + j
        // C   = (-1)      * det(M  )
        //  ij                    ij
        //
        //     [ a b c d ]
        // M = [ e f g h ]
        //     [ i j k l ]
        //     [ m n o p ]
        //
        // First Row
        //           2 | f g h |
        // C   = (-1)  | j k l | = + ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
        //  11         | n o p |
        //
        //           3 | e g h |
        // C   = (-1)  | i k l | = - ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
        //  12         | m o p |
        //
        //           4 | e f h |
        // C   = (-1)  | i j l | = + ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
        //  13         | m n p |
        //
        //           5 | e f g |
        // C   = (-1)  | i j k | = - ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
        //  14         | m n o |
        //
        // Second Row
        //           3 | b c d |
        // C   = (-1)  | j k l | = - ( b ( kp - lo ) - c ( jp - ln ) + d ( jo - kn ) )
        //  21         | n o p |
        //
        //           4 | a c d |
        // C   = (-1)  | i k l | = + ( a ( kp - lo ) - c ( ip - lm ) + d ( io - km ) )
        //  22         | m o p |
        //
        //           5 | a b d |
        // C   = (-1)  | i j l | = - ( a ( jp - ln ) - b ( ip - lm ) + d ( in - jm ) )
        //  23         | m n p |
        //
        //           6 | a b c |
        // C   = (-1)  | i j k | = + ( a ( jo - kn ) - b ( io - km ) + c ( in - jm ) )
        //  24         | m n o |
        //
        // Third Row
        //           4 | b c d |
        // C   = (-1)  | f g h | = + ( b ( gp - ho ) - c ( fp - hn ) + d ( fo - gn ) )
        //  31         | n o p |
        //
        //           5 | a c d |
        // C   = (-1)  | e g h | = - ( a ( gp - ho ) - c ( ep - hm ) + d ( eo - gm ) )
        //  32         | m o p |
        //
        //           6 | a b d |
        // C   = (-1)  | e f h | = + ( a ( fp - hn ) - b ( ep - hm ) + d ( en - fm ) )
        //  33         | m n p |
        //
        //           7 | a b c |
        // C   = (-1)  | e f g | = - ( a ( fo - gn ) - b ( eo - gm ) + c ( en - fm ) )
        //  34         | m n o |
        //
        // Fourth Row
        //           5 | b c d |
        // C   = (-1)  | f g h | = - ( b ( gl - hk ) - c ( fl - hj ) + d ( fk - gj ) )
        //  41         | j k l |
        //
        //           6 | a c d |
        // C   = (-1)  | e g h | = + ( a ( gl - hk ) - c ( el - hi ) + d ( ek - gi ) )
        //  42         | i k l |
        //
        //           7 | a b d |
        // C   = (-1)  | e f h | = - ( a ( fl - hj ) - b ( el - hi ) + d ( ej - fi ) )
        //  43         | i j l |
        //
        //           8 | a b c |
        // C   = (-1)  | e f g | = + ( a ( fk - gj ) - b ( ek - gi ) + c ( ej - fi ) )
        //  44         | i j k |
        //
        float a = matrix.m11, b = matrix.m12, c = matrix.m13, d = matrix.m14;
        float e = matrix.m21, f = matrix.m22, g = matrix.m23, h = matrix.m24;
        float i = matrix.m31, j = matrix.m32, k = matrix.m33, l = matrix.m34;
        float m = matrix.m41, n = matrix.m42, o = matrix.m43, p = matrix.m44;

        float kp_lo = k * p - l * o;
        float jp_ln = j * p - l * n;
        float jo_kn = j * o - k * n;
        float ip_lm = i * p - l * m;
        float io_km = i * o - k * m;
        float in_jm = i * n - j * m;

        float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
        float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
        float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
        float a14 = -(e * jo_kn - f * io_km + g * in_jm);

        float det = a * a11 + b * a12 + c * a13 + d * a14;

        if (fabs(det) < FLT_EPSILON)
        {
            const float nan = std::numeric_limits<float>::quiet_NaN();

            *result = float4x4(nan, nan, nan, nan,
                               nan, nan, nan, nan,
                               nan, nan, nan, nan,
                               nan, nan, nan, nan);
            return false;
        }

        float invDet = 1.0f / det;

        result->m11 = a11 * invDet;
        result->m21 = a12 * invDet;
        result->m31 = a13 * invDet;
        result->m41 = a14 * invDet;

        result->m12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
        result->m22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
        result->m32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
        result->m42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

        float gp_ho = g * p - h * o;
        float fp_hn = f * p - h * n;
        float fo_gn = f * o - g * n;
        float ep_hm = e * p - h * m;
        float eo_gm = e * o - g * m;
        float en_fm = e * n - f * m;

        result->m13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
        result->m23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
        result->m33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
        result->m43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

        float gl_hk = g * l - h * k;
        float fl_hj = f * l - h * j;
        float fk_gj = f * k - g * j;
        float el_hi = e * l - h * i;
        float ek_gi = e * k - g * i;
        float ej_fi = e * j - f * i;

        result->m14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
        result->m24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
        result->m34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
        result->m44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

        return true;
    }


    inline bool decompose(float4x4 const& matrix, _Out_ float3* scale, _Out_ quaternion* rotation, _Out_ float3* translation)
    {
        bool result = true;

        float* pfScales = &scale->x;

        const float EPSILON = 0.0001f;
        float det;

        struct CanonicalBasis
        {
            float3 Row0;
            float3 Row1;
            float3 Row2;
        };

        struct VectorBasis
        {
            float3* Element0;
            float3* Element1;
            float3* Element2;
        };

        VectorBasis vectorBasis;
        float3** pVectorBasis = (float3**)&vectorBasis;

        float4x4 matTemp = float4x4::identity();
        CanonicalBasis canonicalBasis;
        float3* pCanonicalBasis = &canonicalBasis.Row0;

        canonicalBasis.Row0 = float3(1.0f, 0.0f, 0.0f);
        canonicalBasis.Row1 = float3(0.0f, 1.0f, 0.0f);
        canonicalBasis.Row2 = float3(0.0f, 0.0f, 1.0f);

        translation->x = matrix.m41;
        translation->y = matrix.m42;
        translation->z = matrix.m43;

        pVectorBasis[0] = (float3*)&matTemp.m11;
        pVectorBasis[1] = (float3*)&matTemp.m21;
        pVectorBasis[2] = (float3*)&matTemp.m31;

        *(pVectorBasis[0]) = float3(matrix.m11, matrix.m12, matrix.m13);
        *(pVectorBasis[1]) = float3(matrix.m21, matrix.m22, matrix.m23);
        *(pVectorBasis[2]) = float3(matrix.m31, matrix.m32, matrix.m33);

        scale->x = length(*pVectorBasis[0]);
        scale->y = length(*pVectorBasis[1]);
        scale->z = length(*pVectorBasis[2]);

        int a, b, c;
        float x = pfScales[0], y = pfScales[1], z = pfScales[2];
        if (x < y)
        {
            if (y < z)
            {
                a = 2;
                b = 1;
                c = 0;
            }
            else
            {
                a = 1;

                if (x < z)
                {
                    b = 2;
                    c = 0;
                }
                else
                {
                    b = 0;
                    c = 2;
                }
            }
        }
        else
        {
            if (x < z)
            {
                a = 2;
                b = 0;
                c = 1;
            }
            else
            {
                a = 0;

                if (y < z)
                {
                    b = 2;
                    c = 1;
                }
                else
                {
                    b = 1;
                    c = 2;
                }
            }
        }

        if (pfScales[a] < EPSILON)
        {
            *(pVectorBasis[a]) = pCanonicalBasis[a];
        }

        *pVectorBasis[a] = normalize(*pVectorBasis[a]);

        if (pfScales[b] < EPSILON)
        {
            int cc;
            float fAbsX, fAbsY, fAbsZ;

            fAbsX = fabs(pVectorBasis[a]->x);
            fAbsY = fabs(pVectorBasis[a]->y);
            fAbsZ = fabs(pVectorBasis[a]->z);

            if (fAbsX < fAbsY)
            {
                if (fAbsY < fAbsZ)
                {
                    cc = 0;
                }
                else
                {
                    if (fAbsX < fAbsZ)
                    {
                        cc = 0;
                    }
                    else
                    {
                        cc = 2;
                    }
                }
            }
            else
            {
                if (fAbsX < fAbsZ)
                {
                    cc = 1;
                }
                else
                {
                    if (fAbsY < fAbsZ)
                    {
                        cc = 1;
                    }
                    else
                    {
                        cc = 2;
                    }
                }
            }

            *(pCanonicalBasis + cc) = cross(*pVectorBasis[b], *pVectorBasis[a]);
        }

        *pVectorBasis[b] = normalize(*pVectorBasis[b]);

        if (pfScales[c] < EPSILON)
        {
            *pVectorBasis[b] = cross(*pVectorBasis[c], *pVectorBasis[a]);
        }

        *pVectorBasis[c] = normalize(*pVectorBasis[c]);

        det = determinant(matTemp);

        // use Kramer's rule to check for handedness of coordinate system
        if (det < 0.0f)
        {
            // switch coordinate system by negating the scale and inverting the basis vector on the x-axis
            pfScales[a] = -pfScales[a];
            *pVectorBasis[a] = -(*pVectorBasis[a]);

            det = -det;
        }

        det -= 1.0f;
        det *= det;

        if ((EPSILON < det))
        {
            // Non-SRT matrix encountered
            *rotation = quaternion::identity();
            result = false;
        }
        else
        {
            *rotation = make_quaternion_from_rotation_matrix(matTemp);
        }

        return result;
    }


    inline float4x4 transform(float4x4 const& value, quaternion const& rotation)
    {
        // Compute rotation matrix.
        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        float q11 = 1.0f - yy2 - zz2;
        float q21 = xy2 - wz2;
        float q31 = xz2 + wy2;

        float q12 = xy2 + wz2;
        float q22 = 1.0f - xx2 - zz2;
        float q32 = yz2 - wx2;

        float q13 = xz2 - wy2;
        float q23 = yz2 + wx2;
        float q33 = 1.0f - xx2 - yy2;

        float4x4 result;

        // First row
        result.m11 = value.m11 * q11 + value.m12 * q21 + value.m13 * q31;
        result.m12 = value.m11 * q12 + value.m12 * q22 + value.m13 * q32;
        result.m13 = value.m11 * q13 + value.m12 * q23 + value.m13 * q33;
        result.m14 = value.m14;

        // Second row
        result.m21 = value.m21 * q11 + value.m22 * q21 + value.m23 * q31;
        result.m22 = value.m21 * q12 + value.m22 * q22 + value.m23 * q32;
        result.m23 = value.m21 * q13 + value.m22 * q23 + value.m23 * q33;
        result.m24 = value.m24;

        // Third row
        result.m31 = value.m31 * q11 + value.m32 * q21 + value.m33 * q31;
        result.m32 = value.m31 * q12 + value.m32 * q22 + value.m33 * q32;
        result.m33 = value.m31 * q13 + value.m32 * q23 + value.m33 * q33;
        result.m34 = value.m34;

        // Fourth row
        result.m41 = value.m41 * q11 + value.m42 * q21 + value.m43 * q31;
        result.m42 = value.m41 * q12 + value.m42 * q22 + value.m43 * q32;
        result.m43 = value.m41 * q13 + value.m42 * q23 + value.m43 * q33;
        result.m44 = value.m44;

        return result;
    }


    inline float4x4 transpose(float4x4 const& matrix)
    {
        float4x4 result;

        result.m11 = matrix.m11; result.m12 = matrix.m21; result.m13 = matrix.m31; result.m14 = matrix.m41;
        result.m21 = matrix.m12; result.m22 = matrix.m22; result.m23 = matrix.m32; result.m24 = matrix.m42;
        result.m31 = matrix.m13; result.m32 = matrix.m23; result.m33 = matrix.m33; result.m34 = matrix.m43;
        result.m41 = matrix.m14; result.m42 = matrix.m24; result.m43 = matrix.m34; result.m44 = matrix.m44;

        return result;
    }


    inline float4x4 lerp(float4x4 const& matrix1, float4x4 const& matrix2, float amount)
    {
        float4x4 result;
        
        // First row
        result.m11 = matrix1.m11 + (matrix2.m11 - matrix1.m11) * amount;
        result.m12 = matrix1.m12 + (matrix2.m12 - matrix1.m12) * amount;
        result.m13 = matrix1.m13 + (matrix2.m13 - matrix1.m13) * amount;
        result.m14 = matrix1.m14 + (matrix2.m14 - matrix1.m14) * amount;
        
        // Second row
        result.m21 = matrix1.m21 + (matrix2.m21 - matrix1.m21) * amount;
        result.m22 = matrix1.m22 + (matrix2.m22 - matrix1.m22) * amount;
        result.m23 = matrix1.m23 + (matrix2.m23 - matrix1.m23) * amount;
        result.m24 = matrix1.m24 + (matrix2.m24 - matrix1.m24) * amount;
        
        // Third row
        result.m31 = matrix1.m31 + (matrix2.m31 - matrix1.m31) * amount;
        result.m32 = matrix1.m32 + (matrix2.m32 - matrix1.m32) * amount;
        result.m33 = matrix1.m33 + (matrix2.m33 - matrix1.m33) * amount;
        result.m34 = matrix1.m34 + (matrix2.m34 - matrix1.m34) * amount;
        
        // Fourth row
        result.m41 = matrix1.m41 + (matrix2.m41 - matrix1.m41) * amount;
        result.m42 = matrix1.m42 + (matrix2.m42 - matrix1.m42) * amount;
        result.m43 = matrix1.m43 + (matrix2.m43 - matrix1.m43) * amount;
        result.m44 = matrix1.m44 + (matrix2.m44 - matrix1.m44) * amount;

        return result;
    }


    inline plane::plane(float a, float b, float c, float d)
        : normal(a, b, c), d(d)
    { }


    inline plane::plane(float3 normal, float d)
        : normal(normal), d(d)
    { }


    inline plane::plane(float4 value)
        : normal(value.x, value.y, value.z), d(value.w)
    { }


    inline plane make_plane_from_vertices(float3 const& point1, float3 const& point2, float3 const& point3)
    {
        plane result;

        float ax = point2.x - point1.x;
        float ay = point2.y - point1.y;
        float az = point2.z - point1.z;

        float bx = point3.x - point1.x;
        float by = point3.y - point1.y;
        float bz = point3.z - point1.z;

        // N=Cross(a,b)
        float nx = ay * bz - az * by;
        float ny = az * bx - ax * bz;
        float nz = ax * by - ay * bx;

        // Normalize(N)
        float ls = nx * nx + ny * ny + nz * nz;
        float invNorm = 1.0f / sqrtf(ls);

        result.normal.x = nx * invNorm;
        result.normal.y = ny * invNorm;
        result.normal.z = nz * invNorm;

        // D = - Dot(N, point1)
        result.d = -(result.normal.x * point1.x + result.normal.y * point1.y + result.normal.z * point1.z);

        return result;
    }


    inline bool operator ==(plane const& value1, plane const& value2)
    {
        return value1.normal.x == value2.normal.x &&
               value1.normal.y == value2.normal.y &&
               value1.normal.z == value2.normal.z &&
               value1.d == value2.d;
    }


    inline bool operator !=(plane const& value1, plane const& value2)
    {
        return value1.normal.x != value2.normal.x ||
               value1.normal.y != value2.normal.y || 
               value1.normal.z != value2.normal.z ||
               value1.d != value2.d;
    }


    inline plane normalize(plane const& value)
    {
        plane result;

        float f = value.normal.x * value.normal.x + value.normal.y * value.normal.y + value.normal.z * value.normal.z;

        if (fabs(f - 1.0f) < FLT_EPSILON)
        {
            result.normal = value.normal;
            result.d = value.d;
            return result;
        }

        float fInv = 1.0f / sqrtf(f);

        result.normal.x = value.normal.x * fInv;
        result.normal.y = value.normal.y * fInv;
        result.normal.z = value.normal.z * fInv;

        result.d = value.d * fInv;

        return result;
    }


    inline plane transform(plane const& plane, float4x4 const& matrix)
    {
        float4x4 m;
        invert(matrix, &m);

        Numerics::plane result;

        float x = plane.normal.x, y = plane.normal.y, z = plane.normal.z, w = plane.d;

        result.normal.x = x * m.m11 + y * m.m12 + z * m.m13 + w * m.m14;
        result.normal.y = x * m.m21 + y * m.m22 + z * m.m23 + w * m.m24;
        result.normal.z = x * m.m31 + y * m.m32 + z * m.m33 + w * m.m34;

        result.d = x * m.m41 + y * m.m42 + z * m.m43 + w * m.m44;

        return result;
    }


    inline plane transform(plane const& plane, quaternion const& rotation)
    {
        // Compute rotation matrix.
        float x2 = rotation.x + rotation.x;
        float y2 = rotation.y + rotation.y;
        float z2 = rotation.z + rotation.z;

        float wx2 = rotation.w * x2;
        float wy2 = rotation.w * y2;
        float wz2 = rotation.w * z2;
        float xx2 = rotation.x * x2;
        float xy2 = rotation.x * y2;
        float xz2 = rotation.x * z2;
        float yy2 = rotation.y * y2;
        float yz2 = rotation.y * z2;
        float zz2 = rotation.z * z2;

        float m11 = 1.0f - yy2 - zz2;
        float m21 = xy2 - wz2;
        float m31 = xz2 + wy2;

        float m12 = xy2 + wz2;
        float m22 = 1.0f - xx2 - zz2;
        float m32 = yz2 - wx2;

        float m13 = xz2 - wy2;
        float m23 = yz2 + wx2;
        float m33 = 1.0f - xx2 - yy2;

        Numerics::plane result;

        float x = plane.normal.x, y = plane.normal.y, z = plane.normal.z;

        result.normal.x = x * m11 + y * m21 + z * m31;
        result.normal.y = x * m12 + y * m22 + z * m32;
        result.normal.z = x * m13 + y * m23 + z * m33;

        result.d = plane.d;

        return result;
    }


    inline float dot(plane const& plane, float4 const& value)
    {
        return plane.normal.x * value.x + 
               plane.normal.y * value.y + 
               plane.normal.z * value.z + 
               plane.d * value.w;
    }


    inline float dot_coordinate(plane const& plane, float3 const& value)
    {
        return plane.normal.x * value.x + 
               plane.normal.y * value.y + 
               plane.normal.z * value.z + 
               plane.d;
    }


    inline float dot_normal(plane const& plane, float3 const& value)
    {
        return plane.normal.x * value.x + 
               plane.normal.y * value.y + 
               plane.normal.z * value.z;
    }


    inline quaternion::quaternion(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    { }


    inline quaternion::quaternion(float3 vectorPart, float scalarPart)
        : x(vectorPart.x), y(vectorPart.y), z(vectorPart.z), w(scalarPart)
    { }


    inline quaternion quaternion::identity()
    {
        return quaternion(0, 0, 0, 1);
    }


    inline quaternion make_quaternion_from_axis_angle(float3 const& axis, float angle)
    {
        quaternion ans;

        float halfAngle = angle * 0.5f;
        float s = sinf(halfAngle);
        float c = cosf(halfAngle);

        ans.x = axis.x * s;
        ans.y = axis.y * s;
        ans.z = axis.z * s;
        ans.w = c;

        return ans;
    }


    inline quaternion make_quaternion_from_yaw_pitch_roll(float yaw, float pitch, float roll)
    {
        // Roll first, about axis the object is facing, then
        // pitch upward, then yaw to face into the new heading
        float sr, cr, sp, cp, sy, cy;

        float halfRoll = roll * 0.5f;
        sr = sinf(halfRoll); cr = cosf(halfRoll);

        float halfPitch = pitch * 0.5f;
        sp = sinf(halfPitch); cp = cosf(halfPitch);

        float halfYaw = yaw * 0.5f;
        sy = sinf(halfYaw); cy = cosf(halfYaw);

        quaternion result;
        
        result.x = cy * sp * cr + sy * cp * sr;
        result.y = sy * cp * cr - cy * sp * sr;
        result.z = cy * cp * sr - sy * sp * cr;
        result.w = cy * cp * cr + sy * sp * sr;

        return result;
    }


    inline quaternion make_quaternion_from_rotation_matrix(float4x4 const& matrix)
    {
        float trace = matrix.m11 + matrix.m22 + matrix.m33;
        
        quaternion q;
        
        if (trace > 0.0f)
        {
            float s = sqrtf(trace + 1.0f);
            q.w = s * 0.5f;
            s = 0.5f / s;
            q.x = (matrix.m23 - matrix.m32) * s;
            q.y = (matrix.m31 - matrix.m13) * s;
            q.z = (matrix.m12 - matrix.m21) * s;
        }
        else
        {
            if (matrix.m11 >= matrix.m22 && matrix.m11 >= matrix.m33)
            {
                float s = sqrtf(1.0f + matrix.m11 - matrix.m22 - matrix.m33);
                float invS = 0.5f / s;
                q.x = 0.5f * s;
                q.y = (matrix.m12 + matrix.m21) * invS;
                q.z = (matrix.m13 + matrix.m31) * invS;
                q.w = (matrix.m23 - matrix.m32) * invS;
            }
            else if (matrix.m22 > matrix.m33)
            {
                float s = sqrtf(1.0f + matrix.m22 - matrix.m11 - matrix.m33);
                float invS = 0.5f / s;
                q.x = (matrix.m21 + matrix.m12) * invS;
                q.y = 0.5f * s;
                q.z = (matrix.m32 + matrix.m23) * invS;
                q.w = (matrix.m31 - matrix.m13) * invS;
            }
            else
            {
                float s = sqrtf(1.0f + matrix.m33 - matrix.m11 - matrix.m22);
                float invS = 0.5f / s;
                q.x = (matrix.m31 + matrix.m13) * invS;
                q.y = (matrix.m32 + matrix.m23) * invS;
                q.z = 0.5f * s;
                q.w = (matrix.m12 - matrix.m21) * invS;
            }
        }

        return q;
    }


    inline quaternion operator +(quaternion const& value1, quaternion const& value2)
    {
        quaternion ans;

        ans.x = value1.x + value2.x;
        ans.y = value1.y + value2.y;
        ans.z = value1.z + value2.z;
        ans.w = value1.w + value2.w;

        return ans;
    }


    inline quaternion operator -(quaternion const& value1, quaternion const& value2)
    {
        quaternion ans;

        ans.x = value1.x - value2.x;
        ans.y = value1.y - value2.y;
        ans.z = value1.z - value2.z;
        ans.w = value1.w - value2.w;

        return ans;
    }


    inline quaternion operator *(quaternion const& value1, quaternion const& value2)
    {
        quaternion ans;

        float q1x = value1.x;
        float q1y = value1.y;
        float q1z = value1.z;
        float q1w = value1.w;

        float q2x = value2.x;
        float q2y = value2.y;
        float q2z = value2.z;
        float q2w = value2.w;

        // cross(av, bv)
        float cx = q1y * q2z - q1z * q2y;
        float cy = q1z * q2x - q1x * q2z;
        float cz = q1x * q2y - q1y * q2x;

        float dot = q1x * q2x + q1y * q2y + q1z * q2z;

        ans.x = q1x * q2w + q2x * q1w + cx;
        ans.y = q1y * q2w + q2y * q1w + cy;
        ans.z = q1z * q2w + q2z * q1w + cz;
        ans.w = q1w * q2w - dot;

        return ans;
    }


    inline quaternion operator *(quaternion const& value1, float value2)
    {
        quaternion ans;

        ans.x = value1.x * value2;
        ans.y = value1.y * value2;
        ans.z = value1.z * value2;
        ans.w = value1.w * value2;

        return ans;
    }


    inline quaternion operator /(quaternion const& value1, quaternion const& value2)
    {
        quaternion ans;

        float q1x = value1.x;
        float q1y = value1.y;
        float q1z = value1.z;
        float q1w = value1.w;

        // Inverse part.
        float ls = value2.x * value2.x + value2.y * value2.y +
                   value2.z * value2.z + value2.w * value2.w;
        float invNorm = 1.0f / ls;

        float q2x = -value2.x * invNorm;
        float q2y = -value2.y * invNorm;
        float q2z = -value2.z * invNorm;
        float q2w = value2.w * invNorm;

        // Multiply part.
        float cx = q1y * q2z - q1z * q2y;
        float cy = q1z * q2x - q1x * q2z;
        float cz = q1x * q2y - q1y * q2x;

        float dot = q1x * q2x + q1y * q2y + q1z * q2z;

        ans.x = q1x * q2w + q2x * q1w + cx;
        ans.y = q1y * q2w + q2y * q1w + cy;
        ans.z = q1z * q2w + q2z * q1w + cz;
        ans.w = q1w * q2w - dot;

        return ans;
    }


    inline quaternion operator -(quaternion const& value)
    {
        quaternion ans;

        ans.x = -value.x;
        ans.y = -value.y;
        ans.z = -value.z;
        ans.w = -value.w;

        return ans;
    }


    inline quaternion& operator +=(quaternion& value1, quaternion const& value2)
    {
        value1 = value1 + value2;

        return value1;
    }


    inline quaternion& operator -=(quaternion& value1, quaternion const& value2)
    {
        value1 = value1 - value2;

        return value1;
    }


    inline quaternion& operator *=(quaternion& value1, quaternion const& value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline quaternion& operator *=(quaternion& value1, float value2)
    {
        value1 = value1 * value2;

        return value1;
    }


    inline quaternion& operator /=(quaternion& value1, quaternion const& value2)
    {
        value1 = value1 / value2;

        return value1;
    }


    inline bool operator ==(quaternion const& value1, quaternion const& value2)
    {
        return value1.x == value2.x &&
               value1.y == value2.y &&
               value1.z == value2.z &&
               value1.w == value2.w;
    }


    inline bool operator !=(quaternion const& value1, quaternion const& value2)
    {
        return value1.x != value2.x ||
               value1.y != value2.y ||
               value1.z != value2.z ||
               value1.w != value2.w;
    }


    inline bool is_identity(quaternion const& value)
    {
        return value.x == 0 &&
               value.y == 0 &&
               value.z == 0 &&
               value.w == 1;
    }


    inline float length(quaternion const& value)
    {
        float ls = value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;

        return sqrtf(ls);
    }


    inline float length_squared(quaternion const& value)
    {
        return value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
    }


    inline float dot(quaternion const& quaternion1, quaternion const& quaternion2)
    {
        return quaternion1.x * quaternion2.x + 
               quaternion1.y * quaternion2.y + 
               quaternion1.z * quaternion2.z + 
               quaternion1.w * quaternion2.w;
    }


    inline quaternion normalize(quaternion const& value)
    {
        quaternion ans;

        float ls = value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
        
        float invNorm = 1.0f / sqrtf(ls);

        ans.x = value.x * invNorm;
        ans.y = value.y * invNorm;
        ans.z = value.z * invNorm;
        ans.w = value.w * invNorm;

        return ans;
    }


    inline quaternion conjugate(quaternion const& value)
    {
        quaternion ans;
        
        ans.x = -value.x;
        ans.y = -value.y;
        ans.z = -value.z;
        ans.w = value.w;

        return ans;
    }


    inline quaternion inverse(quaternion const& value)
    {
        //  -1   (       a              -v       )
        // q   = ( -------------   ------------- )
        //       (  a^2 + |v|^2  ,  a^2 + |v|^2  )

        quaternion ans;
        
        float ls = value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
        float invNorm = 1.0f / ls;

        ans.x = -value.x * invNorm;
        ans.y = -value.y * invNorm;
        ans.z = -value.z * invNorm;
        ans.w = value.w * invNorm;

        return ans;
    }


    inline quaternion slerp(quaternion const& quaternion1, quaternion const& quaternion2, float amount)
    {
        const float epsilon = 1e-6f;

        float t = amount;
        
        float cosOmega = quaternion1.x * quaternion2.x + quaternion1.y * quaternion2.y +
                         quaternion1.z * quaternion2.z + quaternion1.w * quaternion2.w;

        bool flip = false;
        
        if (cosOmega < 0.0f)
        {
            flip = true;
            cosOmega = -cosOmega;
        }

        float s1, s2;
        
        if (cosOmega > (1.0f - epsilon))
        {
            // Too close, do straight linear interpolation.
            s1 = 1.0f - t;
            s2 = (flip) ? -t : t;
        }
        else
        {
            float omega = acosf(cosOmega);
            float invSinOmega = 1.0f / sinf(omega);

            s1 = sinf((1.0f - t) * omega) * invSinOmega;
            s2 = (flip)
                ? -sinf(t * omega) * invSinOmega
                : sinf(t * omega) * invSinOmega;
        }

        quaternion ans;

        ans.x = s1 * quaternion1.x + s2 * quaternion2.x;
        ans.y = s1 * quaternion1.y + s2 * quaternion2.y;
        ans.z = s1 * quaternion1.z + s2 * quaternion2.z;
        ans.w = s1 * quaternion1.w + s2 * quaternion2.w;

        return ans;
    }


    inline quaternion lerp(quaternion const& quaternion1, quaternion const& quaternion2, float amount)
    {
        float t = amount;
        float t1 = 1.0f - t;

        quaternion r;

        float dot = quaternion1.x * quaternion2.x + quaternion1.y * quaternion2.y +
                    quaternion1.z * quaternion2.z + quaternion1.w * quaternion2.w;

        if (dot >= 0.0f)
        {
            r.x = t1 * quaternion1.x + t * quaternion2.x;
            r.y = t1 * quaternion1.y + t * quaternion2.y;
            r.z = t1 * quaternion1.z + t * quaternion2.z;
            r.w = t1 * quaternion1.w + t * quaternion2.w;
        }
        else
        {
            r.x = t1 * quaternion1.x - t * quaternion2.x;
            r.y = t1 * quaternion1.y - t * quaternion2.y;
            r.z = t1 * quaternion1.z - t * quaternion2.z;
            r.w = t1 * quaternion1.w - t * quaternion2.w;
        }

        // Normalize it.
        float ls = r.x * r.x + r.y * r.y + r.z * r.z + r.w * r.w;
        float invNorm = 1.0f / sqrtf(ls);

        r.x *= invNorm;
        r.y *= invNorm;
        r.z *= invNorm;
        r.w *= invNorm;

        return r;
    }


    inline quaternion concatenate(quaternion const& value1, quaternion const& value2)
    {
        quaternion ans;

        // Concatenate rotation is actually q2 * q1 instead of q1 * q2.
        // So that's why value2 goes q1 and value1 goes q2.
        float q1x = value2.x;
        float q1y = value2.y;
        float q1z = value2.z;
        float q1w = value2.w;

        float q2x = value1.x;
        float q2y = value1.y;
        float q2z = value1.z;
        float q2w = value1.w;

        // cross(av, bv)
        float cx = q1y * q2z - q1z * q2y;
        float cy = q1z * q2x - q1x * q2z;
        float cz = q1x * q2y - q1y * q2x;

        float dot = q1x * q2x + q1y * q2y + q1z * q2z;

        ans.x = q1x * q2w + q2x * q1w + cx;
        ans.y = q1y * q2w + q2y * q1w + cy;
        ans.z = q1z * q2w + q2z * q1w + cz;
        ans.w = q1w * q2w - dot;

        return ans;
    }
}}}


#pragma warning(pop)
