//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma warning(push)
#pragma warning(disable: 4723) // potential divide by 0
#pragma warning(disable: 4756) // overflow in constant arithmetic

namespace Windows
{
    namespace Math
    {
        inline Vector2::Vector2()
            : X(0), Y(0)
        { }


        inline Vector2::Vector2(float x, float y)
            : X(x), Y(y)
        { }


        inline Vector2::Vector2(float value)
            : X(value), Y(value)
        { }


        inline Vector2::Vector2(Windows::Foundation::Point const& value)
            : X(value.X), Y(value.Y)
        { }


        inline Vector2::Vector2(Windows::Foundation::Size const& value)
            : X(value.Width), Y(value.Height)
        { }


        inline Vector2::operator Windows::Foundation::Point() const
        {
            return Windows::Foundation::Point(X, Y);
        }


        inline Vector2::operator Windows::Foundation::Size() const
        {
            return Windows::Foundation::Size(X, Y);
        }


        inline Vector2 Vector2::Zero()
        {
            return Vector2(0, 0);
        }


        inline Vector2 Vector2::One()
        {
            return Vector2(1, 1);
        }


        inline Vector2 Vector2::UnitX()
        {
            return Vector2(1, 0);
        }


        inline Vector2 Vector2::UnitY()
        {
            return Vector2(0, 1);
        }


        inline float Vector2::Length() const
        {
            float ls = X * X + Y * Y;

            return sqrtf(ls);
        }


        inline float Vector2::LengthSquared() const
        {
            return X * X + Y * Y;
        }


        inline float Vector2::Distance(Vector2 const& value1, Vector2 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;

            float ls = dx * dx + dy * dy;

            return sqrtf(ls);
        }


        inline float Vector2::DistanceSquared(Vector2 const& value1, Vector2 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;

            return dx * dx + dy * dy;
        }


        inline float Vector2::Dot(Vector2 const& value1, Vector2 const& value2)
        {
            return value1.X * value2.X +
                   value1.Y * value2.Y;
        }


        inline Vector2 Vector2::Normalize(Vector2 const& value)
        {
            Vector2 ans;

            float ls = value.X * value.X + value.Y * value.Y;
            float invNorm = 1.0f / sqrtf(ls);

            ans.X = value.X * invNorm;
            ans.Y = value.Y * invNorm;

            return ans;
        }


        inline Vector2 Vector2::Reflect(Vector2 const& vector, Vector2 const& normal)
        {
            Vector2 ans;

            float dot = vector.X * normal.X + vector.Y * normal.Y;

            ans.X = vector.X - 2.0f * dot * normal.X;
            ans.Y = vector.Y - 2.0f * dot * normal.Y;

            return ans;
        }


        inline Vector2 Vector2::Min(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Max(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Clamp(Vector2 const& value1, Vector2 const& min, Vector2 const& max)
        {
            Vector2 result;

            float x = value1.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            result.X = x;
            result.Y = y;

            return result;
        }


        inline Vector2 Vector2::Lerp(Vector2 const& value1, Vector2 const& value2, float amount)
        {
            Vector2 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;

            return ans;
        }


        inline Vector2 Vector2::Transform(Vector2 const& position, Matrix3x2 const& matrix)
        {
            Vector2 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M31;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M32;

            return result;
        }


        inline Vector2 Vector2::Transform(Vector2 const& position, Matrix4x4 const& matrix)
        {
            Vector2 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;

            return result;
        }


        inline Vector2 Vector2::TransformNormal(Vector2 const& normal, Matrix3x2 const& matrix)
        {
            Vector2 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;

            return result;
        }


        inline Vector2 Vector2::TransformNormal(Vector2 const& normal, Matrix4x4 const& matrix)
        {
            Vector2 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;

            return result;
        }


        inline Vector2 Vector2::Transform(Vector2 const& value, Quaternion const& rotation)
        {
            Vector2 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float yy2 = rotation.Y * y2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2);
            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2);

            return result;
        }


        inline Vector2 Vector2::Negate(Vector2 const& value)
        {
            Vector2 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;

            return ans;
        }


        inline Vector2 Vector2::Add(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Subtract(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Multiply(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Multiply(Vector2 const& value1, float value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;

            return ans;
        }


        inline Vector2 Vector2::Divide(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Divide(Vector2 const& value1, float value2)
        {
            Vector2 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;

            return ans;
        }


        inline Vector2 Vector2::operator -() const
        {
            Vector2 ans;

            ans.X = -X;
            ans.Y = -Y;

            return ans;
        }


        inline Vector2& Vector2::operator +=(Vector2 const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Vector2& Vector2::operator -=(Vector2 const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Vector2& Vector2::operator *=(Vector2 const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector2& Vector2::operator *=(float value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector2& Vector2::operator /=(Vector2 const& value)
        {
            *this = *this / value;

            return *this;
        }


        inline Vector2& Vector2::operator /=(float value)
        {
            *this = *this / value;

            return *this;
        }


        inline bool Vector2::operator ==(Vector2 const& value) const
        {
            return X == value.X &&
                   Y == value.Y;
        }


        inline bool Vector2::operator !=(Vector2 const& value) const
        {
            return X != value.X ||
                   Y != value.Y;
        }


        inline Vector2 operator +(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;

            return ans;
        }


        inline Vector2 operator -(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;

            return ans;
        }


        inline Vector2 operator *(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;

            return ans;
        }


        inline Vector2 operator *(Vector2 const& value1, float value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;

            return ans;
        }


        inline Vector2 operator *(float value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1 * value2.X;
            ans.Y = value1 * value2.Y;

            return ans;
        }


        inline Vector2 operator /(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        inline Vector2 operator /(Vector2 const& value1, float value2)
        {
            Vector2 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;

            return ans;
        }


        inline Vector3::Vector3()
            : X(0), Y(0), Z(0)
        { }


        inline Vector3::Vector3(float x, float y, float z)
            : X(x), Y(y), Z(z)
        { }


        inline Vector3::Vector3(Vector2 const& value, float z)
            : X(value.X), Y(value.Y), Z(z)
        { }


        inline Vector3::Vector3(float value)
            : X(value), Y(value), Z(value)
        { }


        inline Vector3 Vector3::Zero()
        {
            return Vector3(0, 0, 0);
        }


        inline Vector3 Vector3::One()
        {
            return Vector3(1, 1, 1);
        }


        inline Vector3 Vector3::UnitX()
        {
            return Vector3(1, 0, 0);
        }


        inline Vector3 Vector3::UnitY()
        {
            return Vector3(0, 1, 0);
        }


        inline Vector3 Vector3::UnitZ()
        {
            return Vector3(0, 0, 1);
        }


        inline float Vector3::Length() const
        {
            float ls = X * X + Y * Y + Z * Z;

            return sqrtf(ls);
        }


        inline float Vector3::LengthSquared() const
        {
            return X * X + Y * Y + Z * Z;
        }


        inline float Vector3::Distance(Vector3 const& value1, Vector3 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;

            float ls = dx * dx + dy * dy + dz * dz;

            return sqrtf(ls);
        }


        inline float Vector3::DistanceSquared(Vector3 const& value1, Vector3 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;

            return dx * dx + dy * dy + dz * dz;
        }


        inline float Vector3::Dot(Vector3 const& vector1, Vector3 const& vector2)
        {
            return vector1.X * vector2.X +
                   vector1.Y * vector2.Y +
                   vector1.Z * vector2.Z;
        }


        inline Vector3 Vector3::Normalize(Vector3 const& value)
        {
            Vector3 ans;

            float ls = value.X * value.X + value.Y * value.Y + value.Z * value.Z;
            float invNorm = 1.0f / sqrtf(ls);

            ans.X = value.X * invNorm;
            ans.Y = value.Y * invNorm;
            ans.Z = value.Z * invNorm;

            return ans;
        }


        inline Vector3 Vector3::Cross(Vector3 const& vector1, Vector3 const& vector2)
        {
            Vector3 ans;

            ans.X = vector1.Y * vector2.Z - vector1.Z * vector2.Y;
            ans.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
            ans.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;

            return ans;
        }


        inline Vector3 Vector3::Reflect(Vector3 const& vector, Vector3 const& normal)
        {
            Vector3 ans;

            float dot = vector.X * normal.X + vector.Y * normal.Y + vector.Z * normal.Z;

            ans.X = vector.X - 2.0f * dot * normal.X;
            ans.Y = vector.Y - 2.0f * dot * normal.Y;
            ans.Z = vector.Z - 2.0f * dot * normal.Z;

            return ans;
        }


        inline Vector3 Vector3::Min(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Max(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Clamp(Vector3 const& value1, Vector3 const& min, Vector3 const& max)
        {
            Vector3 result;

            float x = value1.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value1.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            result.X = x;
            result.Y = y;
            result.Z = z;

            return result;
        }


        inline Vector3 Vector3::Lerp(Vector3 const& value1, Vector3 const& value2, float amount)
        {
            Vector3 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;

            return ans;
        }


        inline Vector3 Vector3::Transform(Vector3 const& position, Matrix4x4 const& matrix)
        {
            Vector3 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;

            return result;
        }


        inline Vector3 Vector3::TransformNormal(Vector3 const& normal, Matrix4x4 const& matrix)
        {
            Vector3 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21 + normal.Z * matrix.M31;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22 + normal.Z * matrix.M32;
            result.Z = normal.X * matrix.M13 + normal.Y * matrix.M23 + normal.Z * matrix.M33;

            return result;
        }


        inline Vector3 Vector3::Transform(Vector3 const& value, Quaternion const& rotation)
        {
            Vector3 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2);
            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2);
            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);

            return result;
        }


        inline Vector3 Vector3::Negate(Vector3 const& value)
        {
            Vector3 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;

            return ans;
        }


        inline Vector3 Vector3::Add(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Subtract(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Multiply(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Multiply(Vector3 const& value1, float value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;

            return ans;
        }


        inline Vector3 Vector3::Divide(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;

            return ans;
        }


        inline Vector3 Vector3::Divide(Vector3 const& value1, float value2)
        {
            Vector3 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;

            return ans;
        }


        inline Vector3 Vector3::operator -() const
        {
            Vector3 ans;

            ans.X = -X;
            ans.Y = -Y;
            ans.Z = -Z;

            return ans;
        }


        inline Vector3& Vector3::operator +=(Vector3 const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Vector3& Vector3::operator -=(Vector3 const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Vector3& Vector3::operator *=(Vector3 const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector3& Vector3::operator *=(float value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector3& Vector3::operator /=(Vector3 const& value)
        {
            *this = *this / value;

            return *this;
        }


        inline Vector3& Vector3::operator /=(float value)
        {
            *this = *this / value;

            return *this;
        }


        inline bool Vector3::operator ==(Vector3 const& value) const
        {
            return X == value.X &&
                   Y == value.Y &&
                   Z == value.Z;
        }


        inline bool Vector3::operator !=(Vector3 const& value) const
        {
            return X != value.X ||
                   Y != value.Y ||
                   Z != value.Z;
        }


        inline Vector3 operator +(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;

            return ans;
        }


        inline Vector3 operator -(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;

            return ans;
        }


        inline Vector3 operator *(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;

            return ans;
        }


        inline Vector3 operator *(Vector3 const& value1, float value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;

            return ans;
        }


        inline Vector3 operator *(float value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1 * value2.X;
            ans.Y = value1 * value2.Y;
            ans.Z = value1 * value2.Z;

            return ans;
        }


        inline Vector3 operator /(Vector3 const& value1, Vector3 const& value2)
        {
            Vector3 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;

            return ans;
        }


        inline Vector3 operator /(Vector3 const& value1, float value2)
        {
            Vector3 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;

            return ans;
        }


        inline Vector4::Vector4()
            : X(0), Y(0), Z(0), W(0)
        { }


        inline Vector4::Vector4(float x, float y, float z, float w)
            : X(x), Y(y), Z(z), W(w)
        { }


        inline Vector4::Vector4(Vector2 const& value, float z, float w)
            : X(value.X), Y(value.Y), Z(z), W(w)
        { }


        inline Vector4::Vector4(Vector3 const& value, float w)
            : X(value.X), Y(value.Y), Z(value.Z), W(w)
        { }


        inline Vector4::Vector4(float value)
            : X(value), Y(value), Z(value), W(value)
        { }


        inline Vector4 Vector4::Zero()
        {
            return Vector4(0, 0, 0, 0);
        }


        inline Vector4 Vector4::One()
        {
            return Vector4(1, 1, 1, 1);
        }


        inline Vector4 Vector4::UnitX()
        {
            return Vector4(1, 0, 0, 0);
        }


        inline Vector4 Vector4::UnitY()
        {
            return Vector4(0, 1, 0, 0);
        }


        inline Vector4 Vector4::UnitZ()
        {
            return Vector4(0, 0, 1, 0);
        }


        inline Vector4 Vector4::UnitW()
        {
            return Vector4(0, 0, 0, 1);
        }


        inline float Vector4::Length() const
        {
            float ls = X * X + Y * Y + Z * Z + W * W;
            
            return sqrtf(ls);
        }


        inline float Vector4::LengthSquared() const
        {
            return X * X + Y * Y + Z * Z + W * W;
        }


        inline float Vector4::Distance(Vector4 const& value1, Vector4 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;
            
            float ls = dx * dx + dy * dy + dz * dz + dw * dw;

            return sqrtf(ls);
        }


        inline float Vector4::DistanceSquared(Vector4 const& value1, Vector4 const& value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;
            
            return dx * dx + dy * dy + dz * dz + dw * dw;
        }


        inline float Vector4::Dot(Vector4 const& vector1, Vector4 const& vector2)
        {
            return vector1.X * vector2.X + 
                   vector1.Y * vector2.Y + 
                   vector1.Z * vector2.Z + 
                   vector1.W * vector2.W;
        }


        inline Vector4 Vector4::Normalize(Vector4 const& vector)
        {
            Vector4 ans;

            float ls = vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z + vector.W * vector.W;
            float invNorm = 1.0f / sqrtf(ls);

            ans.X = vector.X * invNorm;
            ans.Y = vector.Y * invNorm;
            ans.Z = vector.Z * invNorm;
            ans.W = vector.W * invNorm;

            return ans;
        }


        inline Vector4 Vector4::Min(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;
            ans.W = (value1.W < value2.W) ? value1.W : value2.W;

            return ans;
        }


        inline Vector4 Vector4::Max(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;
            ans.W = (value1.W > value2.W) ? value1.W : value2.W;

            return ans;
        }


        inline Vector4 Vector4::Clamp(Vector4 const& value1, Vector4 const& min, Vector4 const& max)
        {
            Vector4 result;

            float x = value1.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value1.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            float w = value1.W;
            w = (w > max.W) ? max.W : w;
            w = (w < min.W) ? min.W : w;

            result.X = x;
            result.Y = y;
            result.Z = z;
            result.W = w;

            return result;
        }


        inline Vector4 Vector4::Lerp(Vector4 const& value1, Vector4 const& value2, float amount)
        {
            Vector4 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;
            ans.W = value1.W + (value2.W - value1.W) * amount;

            return ans;
        }


        inline Vector4 Vector4::Transform(Vector2 const& position, Matrix4x4 const& matrix)
        {
            Vector4 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + matrix.M43;
            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + matrix.M44;

            return result;
        }


        inline Vector4 Vector4::Transform(Vector3 const& position, Matrix4x4 const& matrix)
        {
            Vector4 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;
            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + position.Z * matrix.M34 + matrix.M44;

            return result;
        }


        inline Vector4 Vector4::Transform(Vector4 const& vector, Matrix4x4 const& matrix)
        {
            Vector4 result;

            result.X = vector.X * matrix.M11 + vector.Y * matrix.M21 + vector.Z * matrix.M31 + vector.W * matrix.M41;
            result.Y = vector.X * matrix.M12 + vector.Y * matrix.M22 + vector.Z * matrix.M32 + vector.W * matrix.M42;
            result.Z = vector.X * matrix.M13 + vector.Y * matrix.M23 + vector.Z * matrix.M33 + vector.W * matrix.M43;
            result.W = vector.X * matrix.M14 + vector.Y * matrix.M24 + vector.Z * matrix.M34 + vector.W * matrix.M44;

            return result;
        }


        inline Vector4 Vector4::Transform(Vector2 const& value, Quaternion const& rotation)
        {
            Vector4 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (       xy2 - wz2);
            result.Y = value.X * (       xy2 + wz2) + value.Y * (1.0f - xx2 - zz2);
            result.Z = value.X * (       xz2 - wy2) + value.Y * (       yz2 + wx2);
            result.W = 1.0f;

            return result;
        }


        inline Vector4 Vector4::Transform(Vector3 const& value, Quaternion const& rotation)
        {
            Vector4 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (       xy2 - wz2) + value.Z * (       xz2 + wy2);
            result.Y = value.X * (       xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (       yz2 - wx2);
            result.Z = value.X * (       xz2 - wy2) + value.Y * (       yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);
            result.W = 1.0f;

            return result;
        }


        inline Vector4 Vector4::Transform(Vector4 const& value, Quaternion const& rotation)
        {
            Vector4 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (       xy2 - wz2) + value.Z * (       xz2 + wy2);
            result.Y = value.X * (       xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (       yz2 - wx2);
            result.Z = value.X * (       xz2 - wy2) + value.Y * (       yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);
            result.W = value.W;

            return result;
        }


        inline Vector4 Vector4::Negate(Vector4 const& value)
        {
            Vector4 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;
            ans.W = -value.W;

            return ans;
        }


        inline Vector4 Vector4::Add(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        inline Vector4 Vector4::Subtract(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        inline Vector4 Vector4::Multiply(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;
            ans.W = value1.W * value2.W;

            return ans;
        }


        inline Vector4 Vector4::Multiply(Vector4 const& value1, float value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        inline Vector4 Vector4::Divide(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;
            ans.W = value1.W / value2.W;

            return ans;
        }


        inline Vector4 Vector4::Divide(Vector4 const& value1, float value2)
        {
            Vector4 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;
            ans.W = value1.W * invDiv;

            return ans;
        }


        inline Vector4 Vector4::operator -() const
        {
            Vector4 ans;

            ans.X = -X;
            ans.Y = -Y;
            ans.Z = -Z;
            ans.W = -W;

            return ans;
        }


        inline Vector4& Vector4::operator +=(Vector4 const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Vector4& Vector4::operator -=(Vector4 const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Vector4& Vector4::operator *=(Vector4 const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector4& Vector4::operator *=(float value)
        {
            *this = *this * value;

            return *this;
        }


        inline Vector4& Vector4::operator /=(Vector4 const& value)
        {
            *this = *this / value;

            return *this;
        }


        inline Vector4& Vector4::operator /=(float value)
        {
            *this = *this / value;

            return *this;
        }


        inline bool Vector4::operator ==(Vector4 const& value) const
        {
            return X == value.X &&
                   Y == value.Y &&
                   Z == value.Z &&
                   W == value.W;
        }


        inline bool Vector4::operator !=(Vector4 const& value) const
        {
            return X != value.X ||
                   Y != value.Y ||
                   Z != value.Z ||
                   W != value.W;
        }


        inline Vector4 operator +(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        inline Vector4 operator -(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        inline Vector4 operator *(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;
            ans.W = value1.W * value2.W;

            return ans;
        }


        inline Vector4 operator *(Vector4 const& value1, float value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        inline Vector4 operator *(float value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1 * value2.X;
            ans.Y = value1 * value2.Y;
            ans.Z = value1 * value2.Z;
            ans.W = value1 * value2.W;

            return ans;
        }


        inline Vector4 operator /(Vector4 const& value1, Vector4 const& value2)
        {
            Vector4 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;
            ans.W = value1.W / value2.W;

            return ans;
        }


        inline Vector4 operator /(Vector4 const& value1, float value2)
        {
            Vector4 ans;

            float invDiv = 1.0f / value2;
            
            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;
            ans.W = value1.W * invDiv;

            return ans;
        }


        inline Matrix3x2::Matrix3x2()
            : M11(0), M12(0), M21(0), M22(0), M31(0), M32(0)
        { }


        inline Matrix3x2::Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)
            : M11(m11), M12(m12), M21(m21), M22(m22), M31(m31), M32(m32)
        { }


        inline Matrix3x2 Matrix3x2::Identity()
        {
            return Matrix3x2(1, 0,
                             0, 1,
                             0, 0);
        }


        inline bool Matrix3x2::IsIdentity() const
        {
            return M11 == 1 && M22 == 1 && // Check diagonal element first for early out.
                               M12 == 0 &&
                   M21 == 0             &&
                   M31 == 0 && M32 == 0;
        }


        inline float Matrix3x2::Determinant() const
        {
            return (M11 * M22) - (M21 * M12);
        }


        inline Vector2 Matrix3x2::Translation() const
        {
            Vector2 ans;
                
            ans.X = M31; 
            ans.Y = M32;
                
            return ans;
        }


        inline void Matrix3x2::SetTranslation(Vector2 const& translation)
        {
            M31 = translation.X; 
            M32 = translation.Y;
        }


        inline Matrix3x2 Matrix3x2::CreateTranslation(Vector2 const& position)
        {
            Matrix3x2 result;

            result.M11 = 1.0f; result.M12 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f;

            result.M31 = position.X;
            result.M32 = position.Y;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateTranslation(float xPosition, float yPosition)
        {
            Matrix3x2 result;

            result.M11 = 1.0f; result.M12 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f;

            result.M31 = xPosition;
            result.M32 = yPosition;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(float xScale, float yScale)
        {
            Matrix3x2 result;

            result.M11 = xScale; result.M12 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale;
            result.M31 = 0.0f;   result.M32 = 0.0f;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(float xScale, float yScale, Vector2 const& centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - xScale);
            float ty = centerPoint.Y * (1 - yScale);

            result.M11 = xScale; result.M12 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale;
            result.M31 = tx;     result.M32 = ty;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(Vector2 const& scales)
        {
            Matrix3x2 result;

            result.M11 = scales.X; result.M12 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y;
            result.M31 = 0.0f;     result.M32 = 0.0f;
            
            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(Vector2 const& scales, Vector2 const& centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - scales.X);
            float ty = centerPoint.Y * (1 - scales.Y);

            result.M11 = scales.X; result.M12 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y;
            result.M31 = tx;       result.M32 = ty;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(float scale)
        {
            Matrix3x2 result;

            result.M11 = scale; result.M12 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale;
            result.M31 = 0.0f;  result.M32 = 0.0f;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateScale(float scale, Vector2 const& centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - scale);
            float ty = centerPoint.Y * (1 - scale);

            result.M11 = scale; result.M12 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale;
            result.M31 = tx;    result.M32 = ty;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateSkew(float radiansX, float radiansY)
        {
            Matrix3x2 result;

            float xTan = tanf(radiansX);
            float yTan = tanf(radiansY);

            result.M11 = 1.0f; result.M12 = yTan;
            result.M21 = xTan; result.M22 = 1.0f;
            result.M31 = 0.0f; result.M32 = 0.0f;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateSkew(float radiansX, float radiansY, Vector2 const& centerPoint)
        {
            Matrix3x2 result;

            float xTan = tanf(radiansX);
            float yTan = tanf(radiansY);

            float tx = -centerPoint.Y * xTan;
            float ty = -centerPoint.X * yTan;

            result.M11 = 1.0f; result.M12 = yTan;
            result.M21 = xTan; result.M22 = 1.0f;
            result.M31 = tx;   result.M32 = ty;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateRotation(float radians)
        {
            Matrix3x2 result;

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
            result.M11 =    c; result.M12 =    s;
            result.M21 =   -s; result.M22 =    c;
            result.M31 = 0.0f; result.M32 = 0.0f;

            return result;
        }


        inline Matrix3x2 Matrix3x2::CreateRotation(float radians, Vector2 const& centerPoint)
        {
            Matrix3x2 result;

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

            float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
            float y = centerPoint.Y * (1 - c) - centerPoint.X * s;

            // [  c  s ]
            // [ -s  c ]
            // [  x  y ]
            result.M11 =  c; result.M12 = s;
            result.M21 = -s; result.M22 = c;
            result.M31 =  x; result.M32 = y;

            return result;
        }


        inline bool Matrix3x2::Invert(Matrix3x2 const& matrix, _Out_ Matrix3x2* result)
        {
            float det = (matrix.M11 * matrix.M22) - (matrix.M21 * matrix.M12);

            if (fabs(det) < FLT_EPSILON)
            {
                *result = Matrix3x2(NAN, NAN, NAN, NAN, NAN, NAN);
                return false;
            }

            float invDet = 1.0f / det;

            *result = Matrix3x2
            (
                matrix.M22 * invDet,
                -matrix.M12 * invDet,
                -matrix.M21 * invDet,
                matrix.M11 * invDet,
                (matrix.M21 * matrix.M32 - matrix.M31 * matrix.M22) * invDet,
                (matrix.M31 * matrix.M12 - matrix.M11 * matrix.M32) * invDet
            );

            return true;
        }


        inline Matrix3x2 Matrix3x2::Lerp(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2, float amount)
        {
            Matrix3x2 result;
            
            // First row
            result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
            result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
            
            // Second row
            result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
            result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
            
            // Third row
            result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
            result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;

            return result;
        }


        inline Matrix3x2 Matrix3x2::Negate(Matrix3x2 const& value)
        {
            Matrix3x2 result;

            result.M11 = -value.M11; result.M12 = -value.M12;
            result.M21 = -value.M21; result.M22 = -value.M22;
            result.M31 = -value.M31; result.M32 = -value.M32;

            return result;
        }


        inline Matrix3x2 Matrix3x2::Add(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 result;

            result.M11 = value1.M11 + value2.M11; result.M12 = value1.M12 + value2.M12;
            result.M21 = value1.M21 + value2.M21; result.M22 = value1.M22 + value2.M22;
            result.M31 = value1.M31 + value2.M31; result.M32 = value1.M32 + value2.M32;

            return result;
        }


        inline Matrix3x2 Matrix3x2::Subtract(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 result;

            result.M11 = value1.M11 - value2.M11; result.M12 = value1.M12 - value2.M12;
            result.M21 = value1.M21 - value2.M21; result.M22 = value1.M22 - value2.M22;
            result.M31 = value1.M31 - value2.M31; result.M32 = value1.M32 - value2.M32;

            return result;
        }


        inline Matrix3x2 Matrix3x2::Multiply(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 result;

            // First row
            result.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21;
            result.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22;
            
            // Second row
            result.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21;
            result.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22;
            
            // Third row
            result.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value2.M31;
            result.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value2.M32;

            return result;
        }


        inline Matrix3x2 Matrix3x2::operator -() const
        {
            Matrix3x2 m;

            m.M11 = -M11; m.M12 = -M12;
            m.M21 = -M21; m.M22 = -M22;
            m.M31 = -M31; m.M32 = -M32;

            return m;
        }


        inline Matrix3x2& Matrix3x2::operator +=(Matrix3x2 const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Matrix3x2& Matrix3x2::operator -=(Matrix3x2 const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Matrix3x2& Matrix3x2::operator *=(Matrix3x2 const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline bool Matrix3x2::operator ==(Matrix3x2 const& value) const
        {
            return M11 == value.M11 && M22 == value.M22 && // Check diagonal element first for early out.
                                       M12 == value.M12 &&
                   M21 == value.M21                     &&
                   M31 == value.M31 && M32 == value.M32;
        }


        inline bool Matrix3x2::operator !=(Matrix3x2 const& value) const
        {
            return M11 != value.M11 || M12 != value.M12 ||
                   M21 != value.M21 || M22 != value.M22 ||
                   M31 != value.M31 || M32 != value.M32;
        }


        inline Matrix3x2 operator +(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 m;

            m.M11 = value1.M11 + value2.M11; m.M12 = value1.M12 + value2.M12;
            m.M21 = value1.M21 + value2.M21; m.M22 = value1.M22 + value2.M22;
            m.M31 = value1.M31 + value2.M31; m.M32 = value1.M32 + value2.M32;

            return m;
        }


        inline Matrix3x2 operator -(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 m;

            m.M11 = value1.M11 - value2.M11; m.M12 = value1.M12 - value2.M12;
            m.M21 = value1.M21 - value2.M21; m.M22 = value1.M22 - value2.M22;
            m.M31 = value1.M31 - value2.M31; m.M32 = value1.M32 - value2.M32;

            return m;
        }


        inline Matrix3x2 operator *(Matrix3x2 const& value1, Matrix3x2 const& value2)
        {
            Matrix3x2 m;

            // First row
            m.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21;
            m.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22;
            
            // Second row
            m.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21;
            m.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22;
            
            // Third row
            m.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value2.M31;
            m.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value2.M32;

            return m;
        }


        inline Matrix4x4::Matrix4x4()
            : M11(0), M12(0), M13(0), M14(0),
              M21(0), M22(0), M23(0), M24(0),
              M31(0), M32(0), M33(0), M34(0),
              M41(0), M42(0), M43(0), M44(0)
        { }


        inline Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
            : M11(m11), M12(m12), M13(m13), M14(m14),
              M21(m21), M22(m22), M23(m23), M24(m24),
              M31(m31), M32(m32), M33(m33), M34(m34),
              M41(m41), M42(m42), M43(m43), M44(m44)
        { }


        inline Matrix4x4::Matrix4x4(Matrix3x2 const& value)
            : M11(value.M11), M12(value.M12), M13(0), M14(0),
              M21(value.M21), M22(value.M22), M23(0), M24(0),
              M31(0),         M32(0),         M33(1), M34(0),
              M41(value.M31), M42(value.M32), M43(0), M44(1)
        { }


        inline Matrix4x4 Matrix4x4::Identity()
        {
            return Matrix4x4(1, 0, 0, 0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, 0, 0, 1);
        }


        inline bool Matrix4x4::IsIdentity() const
        {
            return M11 == 1 && M22 == 1 && M33 == 1 && M44 == 1 && // Check diagonal element first for early out.
                               M12 == 0 && M13 == 0 && M14 == 0 &&
                   M21 == 0             && M23 == 0 && M24 == 0 &&
                   M31 == 0 && M32 == 0             && M34 == 0 &&
                   M41 == 0 && M42 == 0 && M43 == 0;
        }


        inline float Matrix4x4::Determinant() const
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

            float a = M11, b = M12, c = M13, d = M14;
            float e = M21, f = M22, g = M23, h = M24;
            float i = M31, j = M32, k = M33, l = M34;
            float m = M41, n = M42, o = M43, p = M44;

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


        inline Vector3 Matrix4x4::Translation() const
        {
            Vector3 ans;
                
            ans.X = M41; 
            ans.Y = M42; 
            ans.Z = M43;
                
            return ans;
        }


        inline void Matrix4x4::SetTranslation(Vector3 const& translation)
        {
            M41 = translation.X; 
            M42 = translation.Y;
            M43 = translation.Z;
        }


        inline Matrix4x4 Matrix4x4::CreateBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& cameraUpVector, Vector3 const& cameraForwardVector)
        {
            const float epsilon = 1e-4f;

            Vector3 zaxis;

            zaxis.X = objectPosition.X - cameraPosition.X;
            zaxis.Y = objectPosition.Y - cameraPosition.Y;
            zaxis.Z = objectPosition.Z - cameraPosition.Z;

            float norm = zaxis.LengthSquared();

            if (norm < epsilon)
            {
                zaxis = -cameraForwardVector;
            }
            else
            {
                zaxis = Vector3::Multiply(zaxis, 1.0f / sqrtf(norm));
            }

            Vector3 xaxis = Vector3::Normalize(Vector3::Cross(cameraUpVector, zaxis));

            Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;

            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateConstrainedBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& rotateAxis, Vector3 const& cameraForwardVector, Vector3 const& objectForwardVector)
        {
            const float epsilon = 1e-4f;
            const float minAngle = 1.0f - (0.1f * (DirectX::XM_PI / 180.0f)); // 0.1 degrees

            // Treat the case when object and camera positions are too close.
            Vector3 faceDir;

            faceDir.X = objectPosition.X - cameraPosition.X;
            faceDir.Y = objectPosition.Y - cameraPosition.Y;
            faceDir.Z = objectPosition.Z - cameraPosition.Z;

            float norm = faceDir.LengthSquared();

            if (norm < epsilon)
            {
                faceDir = -cameraForwardVector;
            }
            else
            {
                faceDir = Vector3::Multiply(faceDir, (1.0f / sqrtf(norm)));
            }

            Vector3 yaxis = rotateAxis;
            Vector3 xaxis;
            Vector3 zaxis;

            // Treat the case when angle between faceDir and rotateAxis is too close to 0.
            float dot = Vector3::Dot(rotateAxis, faceDir);

            if (fabs(dot) > minAngle)
            {
                zaxis = objectForwardVector;

                dot = Vector3::Dot(rotateAxis, zaxis);

                if (fabs(dot) > minAngle)
                {
                    zaxis = (fabs(rotateAxis.Z) > minAngle) ? Vector3(1, 0, 0) : Vector3(0, 0, -1);
                }

                xaxis = Vector3::Normalize(Vector3::Cross(rotateAxis, zaxis));
                zaxis = Vector3::Normalize(Vector3::Cross(xaxis, rotateAxis));
            }
            else
            {
                xaxis = Vector3::Normalize(Vector3::Cross(rotateAxis, faceDir));
                zaxis = Vector3::Normalize(Vector3::Cross(xaxis, yaxis));
            }

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;

            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateTranslation(Vector3 const& position)
        {
            Matrix4x4 result;

            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;

            result.M41 = position.X;
            result.M42 = position.Y;
            result.M43 = position.Z;
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateTranslation(float xPosition, float yPosition, float zPosition)
        {
            Matrix4x4 result;

            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;

            result.M41 = xPosition;
            result.M42 = yPosition;
            result.M43 = zPosition;
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(float xScale, float yScale, float zScale)
        {
            Matrix4x4 result;
            
            result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
            result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
            result.M41 = 0.0f;   result.M42 = 0.0f;   result.M43 = 0.0f;   result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(float xScale, float yScale, float zScale, Vector3 const& centerPoint)
        { 
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - xScale);
            float ty = centerPoint.Y * (1 - yScale);
            float tz = centerPoint.Z * (1 - zScale);

            result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
            result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
            result.M41 = tx;     result.M42 = ty;     result.M43 = tz;     result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(Vector3 const& scales)
        {
            Matrix4x4 result;

            result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
            result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
            result.M41 = 0.0f;     result.M42 = 0.0f;     result.M43 = 0.0f;     result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(Vector3 const& scales, Vector3 const& centerPoint)
        {
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - scales.X);
            float ty = centerPoint.Y * (1 - scales.Y);
            float tz = centerPoint.Z * (1 - scales.Z);

            result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
            result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
            result.M41 = tx;       result.M42 = ty;       result.M43 = tz;       result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(float scale)
        {
            Matrix4x4 result;

            result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
            result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
            result.M41 = 0.0f;  result.M42 = 0.0f;  result.M43 = 0.0f;  result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateScale(float scale, Vector3 const& centerPoint)
        {
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - scale);
            float ty = centerPoint.Y * (1 - scale);
            float tz = centerPoint.Z * (1 - scale);

            result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
            result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
            result.M41 = tx;    result.M42 = ty;    result.M43 = tz;    result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationX(float radians)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            // [  1  0  0  0 ]
            // [  0  c  s  0 ]
            // [  0 -s  c  0 ]
            // [  0  0  0  1 ]
            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 =    c; result.M23 =    s; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 =   -s; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationX(float radians, Vector3 const& centerPoint)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            float y = centerPoint.Y * (1 - c) + centerPoint.Z * s;
            float z = centerPoint.Z * (1 - c) - centerPoint.Y * s;

            // [  1  0  0  0 ]
            // [  0  c  s  0 ]
            // [  0 -s  c  0 ]
            // [  0  y  z  1 ]
            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 =    c; result.M23 =    s; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 =   -s; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 =    y; result.M43 =    z; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationY(float radians)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            // [  c  0 -s  0 ]
            // [  0  1  0  0 ]
            // [  s  0  c  0 ]
            // [  0  0  0  1 ]
            result.M11 =    c; result.M12 = 0.0f; result.M13 =   -s; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 =    s; result.M32 = 0.0f; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationY(float radians, Vector3 const& centerPoint)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            float x = centerPoint.X * (1 - c) - centerPoint.Z * s;
            float z = centerPoint.Z * (1 - c) + centerPoint.X * s;

            // [  c  0 -s  0 ]
            // [  0  1  0  0 ]
            // [  s  0  c  0 ]
            // [  x  0  z  1 ]
            result.M11 =    c; result.M12 = 0.0f; result.M13 =   -s; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 =    s; result.M32 = 0.0f; result.M33 =    c; result.M34 = 0.0f;
            result.M41 =    x; result.M42 = 0.0f; result.M43 =    z; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationZ(float radians)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            // [  c  s  0  0 ]
            // [ -s  c  0  0 ]
            // [  0  0  1  0 ]
            // [  0  0  0  1 ]
            result.M11 =    c; result.M12 =    s; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 =   -s; result.M22 =    c; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateRotationZ(float radians, Vector3 const& centerPoint)
        {
            Matrix4x4 result;

            float c = cosf(radians);
            float s = sinf(radians);

            float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
            float y = centerPoint.Y * (1 - c) - centerPoint.X * s;

            // [  c  s  0  0 ]
            // [ -s  c  0  0 ]
            // [  0  0  1  0 ]
            // [  x  y  0  1 ]
            result.M11 =    c; result.M12 =    s; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 =   -s; result.M22 =    c; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
            result.M41 =    x; result.M42 =    y; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateFromAxisAngle(Vector3 const& axis, float angle)
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
            float x = axis.X, y = axis.Y, z = axis.Z;
            float sa = sinf(angle), ca = cosf(angle);
            float xx = x * x, yy = y * y, zz = z * z;
            float xy = x * y, xz = x * z, yz = y * z;

            Matrix4x4 result;

            result.M11 = xx + ca * ( 1.0f - xx );   result.M12 = xy - ca * xy + sa * z;     result.M13 = xz - ca * xz - sa * y;     result.M14 = 0.0f;
            result.M21 = xy - ca * xy - sa * z;     result.M22 = yy + ca * ( 1.0f - yy );   result.M23 = yz - ca * yz + sa * x;     result.M24 = 0.0f;
            result.M31 = xz - ca * xz + sa * y;     result.M32 = yz - ca * yz - sa * x;     result.M33 = zz + ca * ( 1.0f - zz );   result.M34 = 0.0f;
            result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            if (fieldOfView <= 0.0f || fieldOfView >= DirectX::XM_PI)
                throw ref new Platform::InvalidArgumentException();

            if (nearPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (farPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (nearPlaneDistance >= farPlaneDistance )
                throw ref new Platform::InvalidArgumentException();

            float yScale = 1.0f / tanf(fieldOfView * 0.5f);
            float xScale = yScale / aspectRatio;

            Matrix4x4 result;

            result.M11 = xScale;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = yScale;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = result.M32 = 0.0f;
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (farPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (nearPlaneDistance >= farPlaneDistance)
                throw ref new Platform::InvalidArgumentException();

            Matrix4x4 result;

            result.M11 = 2.0f * nearPlaneDistance / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M31 = result.M32 = 0.0f;
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (farPlaneDistance <= 0.0f)
                throw ref new Platform::InvalidArgumentException();

            if (nearPlaneDistance >= farPlaneDistance)
                throw ref new Platform::InvalidArgumentException();

            Matrix4x4 result;

            result.M11 = 2.0f * nearPlaneDistance / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = (left + right) / (right - left);
            result.M32 = (top + bottom) / (top - bottom);
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M41 = result.M42 = result.M44 = 0.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
        {
            Matrix4x4 result;

            result.M11 = 2.0f / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = result.M42 = 0.0f;
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
        {
            Matrix4x4 result;

            result.M11 = 2.0f / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = (left + right) / (left - right);
            result.M42 = (top + bottom) / (bottom - top);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateLookAt(Vector3 const& cameraPosition, Vector3 const& cameraTarget, Vector3 const& cameraUpVector)
        {
            Vector3 zaxis = Vector3::Normalize(cameraPosition - cameraTarget);
            Vector3 xaxis = Vector3::Normalize(Vector3::Cross(cameraUpVector, zaxis));
            Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = yaxis.X; result.M13 = zaxis.X; result.M14 = 0.0f;
            result.M21 = xaxis.Y; result.M22 = yaxis.Y; result.M23 = zaxis.Y; result.M24 = 0.0f;
            result.M31 = xaxis.Z; result.M32 = yaxis.Z; result.M33 = zaxis.Z; result.M34 = 0.0f;
            result.M41 = -Vector3::Dot(xaxis, cameraPosition);
            result.M42 = -Vector3::Dot(yaxis, cameraPosition);
            result.M43 = -Vector3::Dot(zaxis, cameraPosition);
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateWorld(Vector3 const& position, Vector3 const& forward, Vector3 const& up)
        {
            Vector3 zaxis = Vector3::Normalize(-forward);
            Vector3 xaxis = Vector3::Normalize(Vector3::Cross(up, zaxis));
            Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;
            result.M41 = position.X;
            result.M42 = position.Y;
            result.M43 = position.Z;
            result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateFromQuaternion(Quaternion const& quaternion)
        {
            Matrix4x4 result;

            float xx = quaternion.X * quaternion.X;
            float yy = quaternion.Y * quaternion.Y;
            float zz = quaternion.Z * quaternion.Z;

            float xy = quaternion.X * quaternion.Y;
            float wz = quaternion.Z * quaternion.W;
            float xz = quaternion.Z * quaternion.X;
            float wy = quaternion.Y * quaternion.W;
            float yz = quaternion.Y * quaternion.Z;
            float wx = quaternion.X * quaternion.W;

            result.M11 = 1.0f - 2.0f * (yy + zz);   result.M12 = 2.0f * (xy + wz);          result.M13 = 2.0f * (xz - wy);          result.M14 = 0.0f;
            result.M21 = 2.0f * (xy - wz);          result.M22 = 1.0f - 2.0f * (zz + xx);   result.M23 = 2.0f * (yz + wx);          result.M24 = 0.0f;
            result.M31 = 2.0f * (xz + wy);          result.M32 = 2.0f * (yz - wx);          result.M33 = 1.0f - 2.0f * (yy + xx);   result.M34 = 0.0f;
            result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

            return Matrix4x4::CreateFromQuaternion(q);
        }


        inline Matrix4x4 Matrix4x4::CreateShadow(Vector3 const& lightDirection, Plane const& plane)
        {
            Plane p = Plane::Normalize(plane);

            float dot = p.Normal.X * lightDirection.X + p.Normal.Y * lightDirection.Y + p.Normal.Z * lightDirection.Z;
            float a = -p.Normal.X;
            float b = -p.Normal.Y;
            float c = -p.Normal.Z;
            float d = -p.D;

            Matrix4x4 result;

            result.M11 = a * lightDirection.X + dot;
            result.M21 = b * lightDirection.X;
            result.M31 = c * lightDirection.X;
            result.M41 = d * lightDirection.X;

            result.M12 = a * lightDirection.Y;
            result.M22 = b * lightDirection.Y + dot;
            result.M32 = c * lightDirection.Y;
            result.M42 = d * lightDirection.Y;

            result.M13 = a * lightDirection.Z;
            result.M23 = b * lightDirection.Z;
            result.M33 = c * lightDirection.Z + dot;
            result.M43 = d * lightDirection.Z;

            result.M14 = 0.0f;
            result.M24 = 0.0f;
            result.M34 = 0.0f;
            result.M44 = dot;

            return result;
        }


        inline Matrix4x4 Matrix4x4::CreateReflection(Plane const& value)
        {
            Plane v = Plane::Normalize(value);

            float a = v.Normal.X;
            float b = v.Normal.Y;
            float c = v.Normal.Z;

            float fa = -2.0f * a;
            float fb = -2.0f * b;
            float fc = -2.0f * c;

            Matrix4x4 result;

            result.M11 = fa * a + 1.0f;
            result.M12 = fb * a;
            result.M13 = fc * a;
            result.M14 = 0.0f;

            result.M21 = fa * b;
            result.M22 = fb * b + 1.0f;
            result.M23 = fc * b;
            result.M24 = 0.0f;

            result.M31 = fa * c;
            result.M32 = fb * c;
            result.M33 = fc * c + 1.0f;
            result.M34 = 0.0f;

            result.M41 = fa * v.D;
            result.M42 = fb * v.D;
            result.M43 = fc * v.D;
            result.M44 = 1.0f;

            return result;
        }


        inline bool Matrix4x4::Invert(Matrix4x4 const& matrix, _Out_ Matrix4x4* result)
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
            float a = matrix.M11, b = matrix.M12, c = matrix.M13, d = matrix.M14;
            float e = matrix.M21, f = matrix.M22, g = matrix.M23, h = matrix.M24;
            float i = matrix.M31, j = matrix.M32, k = matrix.M33, l = matrix.M34;
            float m = matrix.M41, n = matrix.M42, o = matrix.M43, p = matrix.M44;

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
                *result = Matrix4x4(NAN, NAN, NAN, NAN,
                                    NAN, NAN, NAN, NAN,
                                    NAN, NAN, NAN, NAN,
                                    NAN, NAN, NAN, NAN);
                return false;
            }

            float invDet = 1.0f / det;

            result->M11 = a11 * invDet;
            result->M21 = a12 * invDet;
            result->M31 = a13 * invDet;
            result->M41 = a14 * invDet;

            result->M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
            result->M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
            result->M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
            result->M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

            float gp_ho = g * p - h * o;
            float fp_hn = f * p - h * n;
            float fo_gn = f * o - g * n;
            float ep_hm = e * p - h * m;
            float eo_gm = e * o - g * m;
            float en_fm = e * n - f * m;

            result->M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
            result->M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
            result->M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
            result->M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

            float gl_hk = g * l - h * k;
            float fl_hj = f * l - h * j;
            float fk_gj = f * k - g * j;
            float el_hi = e * l - h * i;
            float ek_gi = e * k - g * i;
            float ej_fi = e * j - f * i;

            result->M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
            result->M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
            result->M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
            result->M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

            return true;
        }


        inline bool Matrix4x4::Decompose(Matrix4x4 const& matrix, _Out_ Vector3* scale, _Out_ Quaternion* rotation, _Out_ Vector3* translation)
        {
            bool result = true;

            float* pfScales = &scale->X;

            const float EPSILON = 0.0001f;
            float det;

            struct CanonicalBasis
            {
                Vector3 Row0;
                Vector3 Row1;
                Vector3 Row2;
            };

            struct VectorBasis
            {
                Vector3* Element0;
                Vector3* Element1;
                Vector3* Element2;
            };

            VectorBasis vectorBasis;
            Vector3** pVectorBasis = (Vector3**)&vectorBasis;

            Matrix4x4 matTemp = Matrix4x4::Identity();
            CanonicalBasis canonicalBasis;
            Vector3* pCanonicalBasis = &canonicalBasis.Row0;

            canonicalBasis.Row0 = Vector3(1.0f, 0.0f, 0.0f);
            canonicalBasis.Row1 = Vector3(0.0f, 1.0f, 0.0f);
            canonicalBasis.Row2 = Vector3(0.0f, 0.0f, 1.0f);

            translation->X = matrix.M41;
            translation->Y = matrix.M42;
            translation->Z = matrix.M43;

            pVectorBasis[0] = (Vector3*)&matTemp.M11;
            pVectorBasis[1] = (Vector3*)&matTemp.M21;
            pVectorBasis[2] = (Vector3*)&matTemp.M31;

            *(pVectorBasis[0]) = Vector3(matrix.M11, matrix.M12, matrix.M13);
            *(pVectorBasis[1]) = Vector3(matrix.M21, matrix.M22, matrix.M23);
            *(pVectorBasis[2]) = Vector3(matrix.M31, matrix.M32, matrix.M33);

            scale->X = pVectorBasis[0]->Length();
            scale->Y = pVectorBasis[1]->Length();
            scale->Z = pVectorBasis[2]->Length();

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

            *pVectorBasis[a] = Vector3::Normalize(*pVectorBasis[a]);

            if (pfScales[b] < EPSILON)
            {
                int cc;
                float fAbsX, fAbsY, fAbsZ;

                fAbsX = fabs(pVectorBasis[a]->X);
                fAbsY = fabs(pVectorBasis[a]->Y);
                fAbsZ = fabs(pVectorBasis[a]->Z);

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

                *(pCanonicalBasis + cc) = Vector3::Cross(*pVectorBasis[b], *pVectorBasis[a]);
            }

            *pVectorBasis[b] = Vector3::Normalize(*pVectorBasis[b]);

            if (pfScales[c] < EPSILON)
            {
                *pVectorBasis[b] = Vector3::Cross(*pVectorBasis[c], *pVectorBasis[a]);
            }

            *pVectorBasis[c] = Vector3::Normalize(*pVectorBasis[c]);

            det = matTemp.Determinant();

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
                *rotation = Quaternion::Identity();
                result = false;
            }
            else
            {
                *rotation = Quaternion::CreateFromRotationMatrix(matTemp);
            }

            return result;
        }


        inline Matrix4x4 Matrix4x4::Transform(Matrix4x4 const& value, Quaternion const& rotation)
        {
            // Compute rotation matrix.
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            float q11 = 1.0f - yy2 - zz2;
            float q21 = xy2 - wz2;
            float q31 = xz2 + wy2;

            float q12 = xy2 + wz2;
            float q22 = 1.0f - xx2 - zz2;
            float q32 = yz2 - wx2;

            float q13 = xz2 - wy2;
            float q23 = yz2 + wx2;
            float q33 = 1.0f - xx2 - yy2;

            Matrix4x4 result;

            // First row
            result.M11 = value.M11 * q11 + value.M12 * q21 + value.M13 * q31;
            result.M12 = value.M11 * q12 + value.M12 * q22 + value.M13 * q32;
            result.M13 = value.M11 * q13 + value.M12 * q23 + value.M13 * q33;
            result.M14 = value.M14;

            // Second row
            result.M21 = value.M21 * q11 + value.M22 * q21 + value.M23 * q31;
            result.M22 = value.M21 * q12 + value.M22 * q22 + value.M23 * q32;
            result.M23 = value.M21 * q13 + value.M22 * q23 + value.M23 * q33;
            result.M24 = value.M24;

            // Third row
            result.M31 = value.M31 * q11 + value.M32 * q21 + value.M33 * q31;
            result.M32 = value.M31 * q12 + value.M32 * q22 + value.M33 * q32;
            result.M33 = value.M31 * q13 + value.M32 * q23 + value.M33 * q33;
            result.M34 = value.M34;

            // Fourth row
            result.M41 = value.M41 * q11 + value.M42 * q21 + value.M43 * q31;
            result.M42 = value.M41 * q12 + value.M42 * q22 + value.M43 * q32;
            result.M43 = value.M41 * q13 + value.M42 * q23 + value.M43 * q33;
            result.M44 = value.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Transpose(Matrix4x4 const& matrix)
        {
            Matrix4x4 result;

            result.M11 = matrix.M11; result.M12 = matrix.M21; result.M13 = matrix.M31; result.M14 = matrix.M41;
            result.M21 = matrix.M12; result.M22 = matrix.M22; result.M23 = matrix.M32; result.M24 = matrix.M42;
            result.M31 = matrix.M13; result.M32 = matrix.M23; result.M33 = matrix.M33; result.M34 = matrix.M43;
            result.M41 = matrix.M14; result.M42 = matrix.M24; result.M43 = matrix.M34; result.M44 = matrix.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Lerp(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2, float amount)
        {
            Matrix4x4 result;
            
            // First row
            result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
            result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
            result.M13 = matrix1.M13 + (matrix2.M13 - matrix1.M13) * amount;
            result.M14 = matrix1.M14 + (matrix2.M14 - matrix1.M14) * amount;
            
            // Second row
            result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
            result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
            result.M23 = matrix1.M23 + (matrix2.M23 - matrix1.M23) * amount;
            result.M24 = matrix1.M24 + (matrix2.M24 - matrix1.M24) * amount;
            
            // Third row
            result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
            result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;
            result.M33 = matrix1.M33 + (matrix2.M33 - matrix1.M33) * amount;
            result.M34 = matrix1.M34 + (matrix2.M34 - matrix1.M34) * amount;
            
            // Fourth row
            result.M41 = matrix1.M41 + (matrix2.M41 - matrix1.M41) * amount;
            result.M42 = matrix1.M42 + (matrix2.M42 - matrix1.M42) * amount;
            result.M43 = matrix1.M43 + (matrix2.M43 - matrix1.M43) * amount;
            result.M44 = matrix1.M44 + (matrix2.M44 - matrix1.M44) * amount;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Negate(Matrix4x4 const& value)
        {
            Matrix4x4 result;

            result.M11 = -value.M11; result.M12 = -value.M12; result.M13 = -value.M13; result.M14 = -value.M14;
            result.M21 = -value.M21; result.M22 = -value.M22; result.M23 = -value.M23; result.M24 = -value.M24;
            result.M31 = -value.M31; result.M32 = -value.M32; result.M33 = -value.M33; result.M34 = -value.M34;
            result.M41 = -value.M41; result.M42 = -value.M42; result.M43 = -value.M43; result.M44 = -value.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Add(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 result;

            result.M11 = value1.M11 + value2.M11; result.M12 = value1.M12 + value2.M12; result.M13 = value1.M13 + value2.M13; result.M14 = value1.M14 + value2.M14;
            result.M21 = value1.M21 + value2.M21; result.M22 = value1.M22 + value2.M22; result.M23 = value1.M23 + value2.M23; result.M24 = value1.M24 + value2.M24;
            result.M31 = value1.M31 + value2.M31; result.M32 = value1.M32 + value2.M32; result.M33 = value1.M33 + value2.M33; result.M34 = value1.M34 + value2.M34;
            result.M41 = value1.M41 + value2.M41; result.M42 = value1.M42 + value2.M42; result.M43 = value1.M43 + value2.M43; result.M44 = value1.M44 + value2.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Subtract(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 result;

            result.M11 = value1.M11 - value2.M11; result.M12 = value1.M12 - value2.M12; result.M13 = value1.M13 - value2.M13; result.M14 = value1.M14 - value2.M14;
            result.M21 = value1.M21 - value2.M21; result.M22 = value1.M22 - value2.M22; result.M23 = value1.M23 - value2.M23; result.M24 = value1.M24 - value2.M24;
            result.M31 = value1.M31 - value2.M31; result.M32 = value1.M32 - value2.M32; result.M33 = value1.M33 - value2.M33; result.M34 = value1.M34 - value2.M34;
            result.M41 = value1.M41 - value2.M41; result.M42 = value1.M42 - value2.M42; result.M43 = value1.M43 - value2.M43; result.M44 = value1.M44 - value2.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::Multiply(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 result;

            // First row
            result.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21 + value1.M13 * value2.M31 + value1.M14 * value2.M41;
            result.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22 + value1.M13 * value2.M32 + value1.M14 * value2.M42;
            result.M13 = value1.M11 * value2.M13 + value1.M12 * value2.M23 + value1.M13 * value2.M33 + value1.M14 * value2.M43;
            result.M14 = value1.M11 * value2.M14 + value1.M12 * value2.M24 + value1.M13 * value2.M34 + value1.M14 * value2.M44;

            // Second row
            result.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21 + value1.M23 * value2.M31 + value1.M24 * value2.M41;
            result.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22 + value1.M23 * value2.M32 + value1.M24 * value2.M42;
            result.M23 = value1.M21 * value2.M13 + value1.M22 * value2.M23 + value1.M23 * value2.M33 + value1.M24 * value2.M43;
            result.M24 = value1.M21 * value2.M14 + value1.M22 * value2.M24 + value1.M23 * value2.M34 + value1.M24 * value2.M44;

            // Third row
            result.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value1.M33 * value2.M31 + value1.M34 * value2.M41;
            result.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value1.M33 * value2.M32 + value1.M34 * value2.M42;
            result.M33 = value1.M31 * value2.M13 + value1.M32 * value2.M23 + value1.M33 * value2.M33 + value1.M34 * value2.M43;
            result.M34 = value1.M31 * value2.M14 + value1.M32 * value2.M24 + value1.M33 * value2.M34 + value1.M34 * value2.M44;

            // Fourth row
            result.M41 = value1.M41 * value2.M11 + value1.M42 * value2.M21 + value1.M43 * value2.M31 + value1.M44 * value2.M41;
            result.M42 = value1.M41 * value2.M12 + value1.M42 * value2.M22 + value1.M43 * value2.M32 + value1.M44 * value2.M42;
            result.M43 = value1.M41 * value2.M13 + value1.M42 * value2.M23 + value1.M43 * value2.M33 + value1.M44 * value2.M43;
            result.M44 = value1.M41 * value2.M14 + value1.M42 * value2.M24 + value1.M43 * value2.M34 + value1.M44 * value2.M44;

            return result;
        }


        inline Matrix4x4 Matrix4x4::operator -() const
        {
            Matrix4x4 m;

            m.M11 = -M11; m.M12 = -M12; m.M13 = -M13; m.M14 = -M14;
            m.M21 = -M21; m.M22 = -M22; m.M23 = -M23; m.M24 = -M24;
            m.M31 = -M31; m.M32 = -M32; m.M33 = -M33; m.M34 = -M34;
            m.M41 = -M41; m.M42 = -M42; m.M43 = -M43; m.M44 = -M44;

            return m;
        }


        inline Matrix4x4& Matrix4x4::operator +=(Matrix4x4 const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Matrix4x4& Matrix4x4::operator -=(Matrix4x4 const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Matrix4x4& Matrix4x4::operator *=(Matrix4x4 const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline bool Matrix4x4::operator ==(Matrix4x4 const& value) const
        {
            return M11 == value.M11 && M22 == value.M22 && M33 == value.M33 && M44 == value.M44 && // Check diagonal element first for early out.
                                       M12 == value.M12 && M13 == value.M13 && M14 == value.M14 &&
                   M21 == value.M21                     && M23 == value.M23 && M24 == value.M24 &&
                   M31 == value.M31 && M32 == value.M32                     && M34 == value.M34 &&
                   M41 == value.M41 && M42 == value.M42 && M43 == value.M43;
        }


        inline bool Matrix4x4::operator !=(Matrix4x4 const& value) const
        {
            return M11 != value.M11 || M12 != value.M12 || M13 != value.M13 || M14 != value.M14 ||
                   M21 != value.M21 || M22 != value.M22 || M23 != value.M23 || M24 != value.M24 ||
                   M31 != value.M31 || M32 != value.M32 || M33 != value.M33 || M34 != value.M34 ||
                   M41 != value.M41 || M42 != value.M42 || M43 != value.M43 || M44 != value.M44;
        }


        inline Matrix4x4 operator +(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 m;

            m.M11 = value1.M11 + value2.M11; m.M12 = value1.M12 + value2.M12; m.M13 = value1.M13 + value2.M13; m.M14 = value1.M14 + value2.M14;
            m.M21 = value1.M21 + value2.M21; m.M22 = value1.M22 + value2.M22; m.M23 = value1.M23 + value2.M23; m.M24 = value1.M24 + value2.M24;
            m.M31 = value1.M31 + value2.M31; m.M32 = value1.M32 + value2.M32; m.M33 = value1.M33 + value2.M33; m.M34 = value1.M34 + value2.M34;
            m.M41 = value1.M41 + value2.M41; m.M42 = value1.M42 + value2.M42; m.M43 = value1.M43 + value2.M43; m.M44 = value1.M44 + value2.M44;

            return m;
        }


        inline Matrix4x4 operator -(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 m;

            m.M11 = value1.M11 - value2.M11; m.M12 = value1.M12 - value2.M12; m.M13 = value1.M13 - value2.M13; m.M14 = value1.M14 - value2.M14;
            m.M21 = value1.M21 - value2.M21; m.M22 = value1.M22 - value2.M22; m.M23 = value1.M23 - value2.M23; m.M24 = value1.M24 - value2.M24;
            m.M31 = value1.M31 - value2.M31; m.M32 = value1.M32 - value2.M32; m.M33 = value1.M33 - value2.M33; m.M34 = value1.M34 - value2.M34;
            m.M41 = value1.M41 - value2.M41; m.M42 = value1.M42 - value2.M42; m.M43 = value1.M43 - value2.M43; m.M44 = value1.M44 - value2.M44;

            return m;
        }


        inline Matrix4x4 operator *(Matrix4x4 const& value1, Matrix4x4 const& value2)
        {
            Matrix4x4 m;

            // First row
            m.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21 + value1.M13 * value2.M31 + value1.M14 * value2.M41;
            m.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22 + value1.M13 * value2.M32 + value1.M14 * value2.M42;
            m.M13 = value1.M11 * value2.M13 + value1.M12 * value2.M23 + value1.M13 * value2.M33 + value1.M14 * value2.M43;
            m.M14 = value1.M11 * value2.M14 + value1.M12 * value2.M24 + value1.M13 * value2.M34 + value1.M14 * value2.M44;
            
            // Second row
            m.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21 + value1.M23 * value2.M31 + value1.M24 * value2.M41;
            m.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22 + value1.M23 * value2.M32 + value1.M24 * value2.M42;
            m.M23 = value1.M21 * value2.M13 + value1.M22 * value2.M23 + value1.M23 * value2.M33 + value1.M24 * value2.M43;
            m.M24 = value1.M21 * value2.M14 + value1.M22 * value2.M24 + value1.M23 * value2.M34 + value1.M24 * value2.M44;
            
            // Third row
            m.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value1.M33 * value2.M31 + value1.M34 * value2.M41;
            m.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value1.M33 * value2.M32 + value1.M34 * value2.M42;
            m.M33 = value1.M31 * value2.M13 + value1.M32 * value2.M23 + value1.M33 * value2.M33 + value1.M34 * value2.M43;
            m.M34 = value1.M31 * value2.M14 + value1.M32 * value2.M24 + value1.M33 * value2.M34 + value1.M34 * value2.M44;
            
            // Fourth row
            m.M41 = value1.M41 * value2.M11 + value1.M42 * value2.M21 + value1.M43 * value2.M31 + value1.M44 * value2.M41;
            m.M42 = value1.M41 * value2.M12 + value1.M42 * value2.M22 + value1.M43 * value2.M32 + value1.M44 * value2.M42;
            m.M43 = value1.M41 * value2.M13 + value1.M42 * value2.M23 + value1.M43 * value2.M33 + value1.M44 * value2.M43;
            m.M44 = value1.M41 * value2.M14 + value1.M42 * value2.M24 + value1.M43 * value2.M34 + value1.M44 * value2.M44;

            return m;
        }


        inline Plane::Plane()
            : Normal(0, 0, 0), D(0)
        { }


        inline Plane::Plane(float a, float b, float c, float d)
            : Normal(a, b, c), D(d)
        { }


        inline Plane::Plane(Vector3 const& normal, float d)
            : Normal(normal), D(d)
        { }


        inline Plane::Plane(Vector4 const& value)
            : Normal(value.X, value.Y, value.Z), D(value.W)
        { }


        inline Plane::Plane(Vector3 const& point1, Vector3 const& point2, Vector3 const& point3)
        {
            float ax = point2.X - point1.X;
            float ay = point2.Y - point1.Y;
            float az = point2.Z - point1.Z;

            float bx = point3.X - point1.X;
            float by = point3.Y - point1.Y;
            float bz = point3.Z - point1.Z;

            // N=Cross(a,b)
            float nx = ay * bz - az * by;
            float ny = az * bx - ax * bz;
            float nz = ax * by - ay * bx;

            // Normalize(N)
            float ls = nx * nx + ny * ny + nz * nz;
            float invNorm = 1.0f / sqrtf(ls);

            Normal.X = nx * invNorm;
            Normal.Y = ny * invNorm;
            Normal.Z = nz * invNorm;

            // D = - Dot(N, point1)
            D = -(Normal.X * point1.X + Normal.Y * point1.Y + Normal.Z * point1.Z);
        }


        inline Plane Plane::Normalize(Plane const& value)
        {
            Plane result;

            float f = value.Normal.X * value.Normal.X + value.Normal.Y * value.Normal.Y + value.Normal.Z * value.Normal.Z;

            if (fabs(f - 1.0f) < FLT_EPSILON)
            {
                result.Normal = value.Normal;
                result.D = value.D;
                return result;
            }

            float fInv = 1.0f / sqrtf(f);

            result.Normal.X = value.Normal.X * fInv;
            result.Normal.Y = value.Normal.Y * fInv;
            result.Normal.Z = value.Normal.Z * fInv;

            result.D = value.D * fInv;

            return result;
        }


        inline Plane Plane::Transform(Plane const& plane, Matrix4x4 const& matrix)
        {
            Matrix4x4 m;
            Matrix4x4::Invert(matrix, &m);

            Plane result;

            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z, w = plane.D;

            result.Normal.X = x * m.M11 + y * m.M12 + z * m.M13 + w * m.M14;
            result.Normal.Y = x * m.M21 + y * m.M22 + z * m.M23 + w * m.M24;
            result.Normal.Z = x * m.M31 + y * m.M32 + z * m.M33 + w * m.M34;

            result.D = x * m.M41 + y * m.M42 + z * m.M43 + w * m.M44;

            return result;
        }


        inline Plane Plane::Transform(Plane const& plane, Quaternion const& rotation)
        {
            // Compute rotation matrix.
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            float m11 = 1.0f - yy2 - zz2;
            float m21 = xy2 - wz2;
            float m31 = xz2 + wy2;

            float m12 = xy2 + wz2;
            float m22 = 1.0f - xx2 - zz2;
            float m32 = yz2 - wx2;

            float m13 = xz2 - wy2;
            float m23 = yz2 + wx2;
            float m33 = 1.0f - xx2 - yy2;

            Plane result;

            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z;

            result.Normal.X = x * m11 + y * m21 + z * m31;
            result.Normal.Y = x * m12 + y * m22 + z * m32;
            result.Normal.Z = x * m13 + y * m23 + z * m33;

            result.D = plane.D;

            return result;
        }


        inline float Plane::Dot(Plane const& plane, Vector4 const& value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z + 
                   plane.D * value.W;
        }


        inline float Plane::DotCoordinate(Plane const& plane, Vector3 const& value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z + 
                   plane.D;
        }


        inline float Plane::DotNormal(Plane const& plane, Vector3 const& value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z;
        }


        inline bool Plane::operator ==(Plane const& value) const
        {
            return Normal.X == value.Normal.X &&
                   Normal.Y == value.Normal.Y &&
                   Normal.Z == value.Normal.Z &&
                   D == value.D;
        }


        inline bool Plane::operator !=(Plane const& value) const
        {
            return Normal.X != value.Normal.X ||
                   Normal.Y != value.Normal.Y || 
                   Normal.Z != value.Normal.Z ||
                   D != value.D;
        }


        inline Quaternion::Quaternion()
            : X(0), Y(0), Z(0), W(0)
        { }


        inline Quaternion::Quaternion(float x, float y, float z, float w)
            : X(x), Y(y), Z(z), W(w)
        { }


        inline Quaternion::Quaternion(Vector3 const& vectorPart, float scalarPart)
            : X(vectorPart.X), Y(vectorPart.Y), Z(vectorPart.Z), W(scalarPart)
        { }


        inline Quaternion Quaternion::Identity()
        {
            return Quaternion(0, 0, 0, 1);
        }


        inline bool Quaternion::IsIdentity() const
        {
            return X == 0 &&
                   Y == 0 &&
                   Z == 0 &&
                   W == 1;
        }


        inline float Quaternion::Length() const
        {
            float ls = X * X + Y * Y + Z * Z + W * W;

            return sqrtf(ls);
        }


        inline float Quaternion::LengthSquared() const
        {
            return X * X + Y * Y + Z * Z + W * W;
        }


        inline Quaternion Quaternion::Normalize(Quaternion const& value)
        {
            Quaternion ans;

            float ls = value.X * value.X + value.Y * value.Y + value.Z * value.Z + value.W * value.W;
            
            float invNorm = 1.0f / sqrtf(ls);

            ans.X = value.X * invNorm;
            ans.Y = value.Y * invNorm;
            ans.Z = value.Z * invNorm;
            ans.W = value.W * invNorm;

            return ans;
        }


        inline Quaternion Quaternion::Conjugate(Quaternion const& value)
        {
            Quaternion ans;
            
            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;
            ans.W = value.W;

            return ans;
        }


        inline Quaternion Quaternion::Inverse(Quaternion const& value)
        {
            //  -1   (       a              -v       )
            // q   = ( -------------   ------------- )
            //       (  a^2 + |v|^2  ,  a^2 + |v|^2  )

            Quaternion ans;
            
            float ls = value.X * value.X + value.Y * value.Y + value.Z * value.Z + value.W * value.W;
            float invNorm = 1.0f / ls;

            ans.X = -value.X * invNorm;
            ans.Y = -value.Y * invNorm;
            ans.Z = -value.Z * invNorm;
            ans.W = value.W * invNorm;

            return ans;
        }


        inline Quaternion Quaternion::CreateFromAxisAngle(Vector3 const& axis, float angle)
        {
            Quaternion ans;

            float halfAngle = angle * 0.5f;
            float s = sinf(halfAngle);
            float c = cosf(halfAngle);

            ans.X = axis.X * s;
            ans.Y = axis.Y * s;
            ans.Z = axis.Z * s;
            ans.W = c;

            return ans;
        }


        inline Quaternion Quaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
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

            Quaternion result;
            
            result.X = cy * sp * cr + sy * cp * sr;
            result.Y = sy * cp * cr - cy * sp * sr;
            result.Z = cy * cp * sr - sy * sp * cr;
            result.W = cy * cp * cr + sy * sp * sr;

            return result;
        }


        inline Quaternion Quaternion::CreateFromRotationMatrix(Matrix4x4 const& matrix)
        {
            float trace = matrix.M11 + matrix.M22 + matrix.M33;
            
            Quaternion q;
            
            if (trace > 0.0f)
            {
                float s = sqrtf(trace + 1.0f);
                q.W = s * 0.5f;
                s = 0.5f / s;
                q.X = (matrix.M23 - matrix.M32) * s;
                q.Y = (matrix.M31 - matrix.M13) * s;
                q.Z = (matrix.M12 - matrix.M21) * s;
            }
            else
            {
                if (matrix.M11 >= matrix.M22 && matrix.M11 >= matrix.M33)
                {
                    float s = sqrtf(1.0f + matrix.M11 - matrix.M22 - matrix.M33);
                    float invS = 0.5f / s;
                    q.X = 0.5f * s;
                    q.Y = (matrix.M12 + matrix.M21) * invS;
                    q.Z = (matrix.M13 + matrix.M31) * invS;
                    q.W = (matrix.M23 - matrix.M32) * invS;
                }
                else if (matrix.M22 > matrix.M33)
                {
                    float s = sqrtf(1.0f + matrix.M22 - matrix.M11 - matrix.M33);
                    float invS = 0.5f / s;
                    q.X = (matrix.M21 + matrix.M12) * invS;
                    q.Y = 0.5f * s;
                    q.Z = (matrix.M32 + matrix.M23) * invS;
                    q.W = (matrix.M31 - matrix.M13) * invS;
                }
                else
                {
                    float s = sqrtf(1.0f + matrix.M33 - matrix.M11 - matrix.M22);
                    float invS = 0.5f / s;
                    q.X = (matrix.M31 + matrix.M13) * invS;
                    q.Y = (matrix.M32 + matrix.M23) * invS;
                    q.Z = 0.5f * s;
                    q.W = (matrix.M12 - matrix.M21) * invS;
                }
            }

            return q;
        }


        inline float Quaternion::Dot(Quaternion const& quaternion1, Quaternion const& quaternion2)
        {
            return quaternion1.X * quaternion2.X + 
                   quaternion1.Y * quaternion2.Y + 
                   quaternion1.Z * quaternion2.Z + 
                   quaternion1.W * quaternion2.W;
        }


        inline Quaternion Quaternion::Slerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount)
        {
            const float epsilon = 1e-6f;

            float t = amount;
            
            float cosOmega = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
                             quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;

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

            Quaternion ans;

            ans.X = s1 * quaternion1.X + s2 * quaternion2.X;
            ans.Y = s1 * quaternion1.Y + s2 * quaternion2.Y;
            ans.Z = s1 * quaternion1.Z + s2 * quaternion2.Z;
            ans.W = s1 * quaternion1.W + s2 * quaternion2.W;

            return ans;
        }


        inline Quaternion Quaternion::Lerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount)
        {
            float t = amount;
            float t1 = 1.0f - t;

            Quaternion r;

            float dot = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
                        quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;

            if (dot >= 0.0f)
            {
                r.X = t1 * quaternion1.X + t * quaternion2.X;
                r.Y = t1 * quaternion1.Y + t * quaternion2.Y;
                r.Z = t1 * quaternion1.Z + t * quaternion2.Z;
                r.W = t1 * quaternion1.W + t * quaternion2.W;
            }
            else
            {
                r.X = t1 * quaternion1.X - t * quaternion2.X;
                r.Y = t1 * quaternion1.Y - t * quaternion2.Y;
                r.Z = t1 * quaternion1.Z - t * quaternion2.Z;
                r.W = t1 * quaternion1.W - t * quaternion2.W;
            }

            // Normalize it.
            float ls = r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W;
            float invNorm = 1.0f / sqrtf(ls);

            r.X *= invNorm;
            r.Y *= invNorm;
            r.Z *= invNorm;
            r.W *= invNorm;

            return r;
        }


        inline Quaternion Quaternion::Concatenate(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            // Concatenate rotation is actually q2 * q1 instead of q1 * q2.
            // So that's why value2 goes q1 and value1 goes q2.
            float q1x = value2.X;
            float q1y = value2.Y;
            float q1z = value2.Z;
            float q1w = value2.W;

            float q2x = value1.X;
            float q2y = value1.Y;
            float q2z = value1.Z;
            float q2w = value1.W;

            // cross(av, bv)
            float cx = q1y * q2z - q1z * q2y;
            float cy = q1z * q2x - q1x * q2z;
            float cz = q1x * q2y - q1y * q2x;

            float dot = q1x * q2x + q1y * q2y + q1z * q2z;

            ans.X = q1x * q2w + q2x * q1w + cx;
            ans.Y = q1y * q2w + q2y * q1w + cy;
            ans.Z = q1z * q2w + q2z * q1w + cz;
            ans.W = q1w * q2w - dot;

            return ans;
        }


        inline Quaternion Quaternion::Negate(Quaternion const& value)
        {
            Quaternion ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;
            ans.W = -value.W;

            return ans;
        }


        inline Quaternion Quaternion::Add(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        inline Quaternion Quaternion::Subtract(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        inline Quaternion Quaternion::Multiply(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            float q1x = value1.X;
            float q1y = value1.Y;
            float q1z = value1.Z;
            float q1w = value1.W;

            float q2x = value2.X;
            float q2y = value2.Y;
            float q2z = value2.Z;
            float q2w = value2.W;

            // cross(av, bv)
            float cx = q1y * q2z - q1z * q2y;
            float cy = q1z * q2x - q1x * q2z;
            float cz = q1x * q2y - q1y * q2x;

            float dot = q1x * q2x + q1y * q2y + q1z * q2z;

            ans.X = q1x * q2w + q2x * q1w + cx;
            ans.Y = q1y * q2w + q2y * q1w + cy;
            ans.Z = q1z * q2w + q2z * q1w + cz;
            ans.W = q1w * q2w - dot;

            return ans;
        }


        inline Quaternion Quaternion::Multiply(Quaternion const& value1, float value2)
        {
            Quaternion ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        inline Quaternion Quaternion::Divide(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            float q1x = value1.X;
            float q1y = value1.Y;
            float q1z = value1.Z;
            float q1w = value1.W;

            // Inverse part.
            float ls = value2.X * value2.X + value2.Y * value2.Y +
                       value2.Z * value2.Z + value2.W * value2.W;
            float invNorm = 1.0f / ls;

            float q2x = -value2.X * invNorm;
            float q2y = -value2.Y * invNorm;
            float q2z = -value2.Z * invNorm;
            float q2w = value2.W * invNorm;

            // Multiply part.
            float cx = q1y * q2z - q1z * q2y;
            float cy = q1z * q2x - q1x * q2z;
            float cz = q1x * q2y - q1y * q2x;

            float dot = q1x * q2x + q1y * q2y + q1z * q2z;

            ans.X = q1x * q2w + q2x * q1w + cx;
            ans.Y = q1y * q2w + q2y * q1w + cy;
            ans.Z = q1z * q2w + q2z * q1w + cz;
            ans.W = q1w * q2w - dot;

            return ans;
        }


        inline Quaternion Quaternion::operator -() const
        {
            Quaternion ans;

            ans.X = -X;
            ans.Y = -Y;
            ans.Z = -Z;
            ans.W = -W;

            return ans;
        }


        inline Quaternion& Quaternion::operator +=(Quaternion const& value)
        {
            *this = *this + value;

            return *this;
        }


        inline Quaternion& Quaternion::operator -=(Quaternion const& value)
        {
            *this = *this - value;

            return *this;
        }


        inline Quaternion& Quaternion::operator *=(Quaternion const& value)
        {
            *this = *this * value;

            return *this;
        }


        inline Quaternion& Quaternion::operator *=(float value)
        {
            *this = *this * value;

            return *this;
        }


        inline Quaternion& Quaternion::operator /=(Quaternion const& value)
        {
            *this = *this / value;

            return *this;
        }


        inline bool Quaternion::operator ==(Quaternion const& value) const
        {
            return X == value.X &&
                   Y == value.Y &&
                   Z == value.Z &&
                   W == value.W;
        }


        inline bool Quaternion::operator !=(Quaternion const& value) const
        {
            return X != value.X ||
                   Y != value.Y ||
                   Z != value.Z ||
                   W != value.W;
        }


        inline Quaternion operator +(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        inline Quaternion operator -(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        inline Quaternion operator *(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            float q1x = value1.X;
            float q1y = value1.Y;
            float q1z = value1.Z;
            float q1w = value1.W;

            float q2x = value2.X;
            float q2y = value2.Y;
            float q2z = value2.Z;
            float q2w = value2.W;

            // cross(av, bv)
            float cx = q1y * q2z - q1z * q2y;
            float cy = q1z * q2x - q1x * q2z;
            float cz = q1x * q2y - q1y * q2x;

            float dot = q1x * q2x + q1y * q2y + q1z * q2z;

            ans.X = q1x * q2w + q2x * q1w + cx;
            ans.Y = q1y * q2w + q2y * q1w + cy;
            ans.Z = q1z * q2w + q2z * q1w + cz;
            ans.W = q1w * q2w - dot;

            return ans;
        }


        inline Quaternion operator *(Quaternion const& value1, float value2)
        {
            Quaternion ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        inline Quaternion operator /(Quaternion const& value1, Quaternion const& value2)
        {
            Quaternion ans;

            float q1x = value1.X;
            float q1y = value1.Y;
            float q1z = value1.Z;
            float q1w = value1.W;

            // Inverse part.
            float ls = value2.X * value2.X + value2.Y * value2.Y +
                       value2.Z * value2.Z + value2.W * value2.W;
            float invNorm = 1.0f / ls;

            float q2x = -value2.X * invNorm;
            float q2y = -value2.Y * invNorm;
            float q2z = -value2.Z * invNorm;
            float q2w = value2.W * invNorm;

            // Multiply part.
            float cx = q1y * q2z - q1z * q2y;
            float cy = q1z * q2x - q1x * q2z;
            float cz = q1x * q2y - q1y * q2x;

            float dot = q1x * q2x + q1y * q2y + q1z * q2z;

            ans.X = q1x * q2w + q2x * q1w + cx;
            ans.Y = q1y * q2w + q2y * q1w + cy;
            ans.Z = q1z * q2w + q2z * q1w + cz;
            ans.W = q1w * q2w - dot;

            return ans;
        }
    }
}

#pragma warning(pop)
