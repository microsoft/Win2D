//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

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


        inline Vector2 Vector2::Multiply(Vector2 const& value1, float scaleFactor)
        {
            Vector2 ans;

            ans.X = value1.X * scaleFactor;
            ans.Y = value1.Y * scaleFactor;

            return ans;
        }


        inline Vector2 Vector2::Divide(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        inline Vector2 Vector2::Divide(Vector2 const& value1, float divider)
        {
            Vector2 ans;

            float invDiv = 1.0f / divider;

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
            X += value.X;
            Y += value.Y;

            return *this;
        }


        inline Vector2& Vector2::operator -=(Vector2 const& value)
        {
            X -= value.X;
            Y -= value.Y;
        
            return *this;
        }


        inline Vector2& Vector2::operator *=(Vector2 const& value)
        {
            X *= value.X;
            Y *= value.Y;
        
            return *this;
        }


        inline Vector2& Vector2::operator *=(float scaleFactor)
        {
            X *= scaleFactor;
            Y *= scaleFactor;
        
            return *this;
        }


        inline Vector2& Vector2::operator /=(Vector2 const& value)
        {
            X /= value.X;
            Y /= value.Y;
        
            return *this;
        }


        inline Vector2& Vector2::operator /=(float divider)
        {
            X /= divider;
            Y /= divider;
        
            return *this;
        }


        inline bool Vector2::operator ==(Vector2 const& value) const
        {
            return (X == value.X &&
                    Y == value.Y);
        }


        inline bool Vector2::operator !=(Vector2 const& value) const
        {
            return (X != value.X ||
                    Y != value.Y);
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


        inline Vector2 operator *(Vector2 const& value, float scaleFactor)
        {
            Vector2 ans;

            ans.X = value.X * scaleFactor;
            ans.Y = value.Y * scaleFactor;

            return ans;
        }


        inline Vector2 operator *(float scaleFactor, Vector2 const& value)
        {
            Vector2 ans;

            ans.X = value.X * scaleFactor;
            ans.Y = value.Y * scaleFactor;

            return ans;
        }


        inline Vector2 operator /(Vector2 const& value1, Vector2 const& value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        inline Vector2 operator /(Vector2 const& value1, float divider)
        {
            Vector2 ans;

            float invDiv = 1.0f / divider;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;

            return ans;
        }


    //struct Vector3
    //{
    //    float X, Y, Z;

    //    // Constructors.
    //    Vector3();
    //    Vector3(float x, float y, float z);
    //    Vector3(Vector2 const& value, float z);
    //    explicit Vector3(float value);

    //    // Common values.
    //    Vector3 Zero();
    //    Vector3 One();
    //    Vector3 UnitX();
    //    Vector3 UnitY();
    //    Vector3 UnitZ();

    //    // Instance methods.
    //    float Length() const;
    //    float LengthSquared() const;

    //    // methods.
    //    float Distance(Vector3 const& value1, Vector3 const& value2);
    //    float DistanceSquared(Vector3 const& value1, Vector3 const& value2);
    //    float Dot(Vector3 const& vector1, Vector3 const& vector2);
    //    Vector3 Normalize(Vector3 const& value);
    //    Vector3 Cross(Vector3 const& vector1, Vector3 const& vector2);
    //    Vector3 Reflect(Vector3 const& vector, Vector3 const& normal);
    //    Vector3 Min(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Max(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Clamp(Vector3 const& value1, Vector3 const& min, Vector3 const& max);
    //    Vector3 Lerp(Vector3 const& value1, Vector3 const& value2, float amount);
    //    Vector3 Transform(Vector3 const& position, Matrix4x4 const& matrix);
    //    Vector3 TransformNormal(Vector3 const& normal, Matrix4x4 const& matrix);
    //    Vector3 Transform(Vector3 const& value, Quaternion const& rotation);
    //    Vector3 Negate(Vector3 const& value);
    //    Vector3 Add(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Subtract(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Multiply(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Multiply(Vector3 const& value1, float scaleFactor);
    //    Vector3 Divide(Vector3 const& value1, Vector3 const& value2);
    //    Vector3 Divide(Vector3 const& value1, float value2);

    //    // Operator overloads.
    //    Vector3 operator -() const;
    //    Vector3& operator +=(Vector3 const& value);
    //    Vector3& operator -=(Vector3 const& value);
    //    Vector3& operator *=(Vector3 const& value);
    //    Vector3& operator *=(float scaleFactor);
    //    Vector3& operator /=(Vector3 const& value);
    //    Vector3& operator /=(float divider);
    //    bool operator ==(Vector3 const& value) const;
    //    bool operator !=(Vector3 const& value) const;
    //};

    //// Binary operators.
    //Vector3 operator +(Vector3 const& value1, Vector3 const& value2);
    //Vector3 operator -(Vector3 const& value1, Vector3 const& value2);
    //Vector3 operator *(Vector3 const& value1, Vector3 const& value2);
    //Vector3 operator *(Vector3 const& value, float scaleFactor);
    //Vector3 operator *(float scaleFactor, Vector3 const& value);
    //Vector3 operator /(Vector3 const& value1, Vector3 const& value2);
    //Vector3 operator /(Vector3 const& value1, float divider);


    //struct Vector4
    //{
    //    float X, Y, Z, W;

    //    // Constructors.
    //    Vector4();
    //    Vector4(float x, float y, float z, float w);
    //    Vector4(Vector2 const& value, float z, float w);
    //    Vector4(Vector3 const& value, float w);
    //    explicit Vector4(float value);

    //    // Common values.
    //    Vector4 Zero();
    //    Vector4 One();
    //    Vector4 UnitX();
    //    Vector4 UnitY();
    //    Vector4 UnitZ();
    //    Vector4 UnitW();

    //    // Instance methods.
    //    float Length() const;
    //    float LengthSquared() const;

    //    // methods.
    //    float Distance(Vector4 const& value1, Vector4 const& value2);
    //    float DistanceSquared(Vector4 const& value1, Vector4 const& value2);
    //    float Dot(Vector4 const& vector1, Vector4 const& vector2);
    //    Vector4 Normalize(Vector4 const& vector);
    //    Vector4 Min(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Max(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Clamp(Vector4 const& value1, Vector4 const& min, Vector4 const& max);
    //    Vector4 Lerp(Vector4 const& value1, Vector4 const& value2, float amount);
    //    Vector4 Transform(Vector2 const& position, Matrix4x4 const& matrix);
    //    Vector4 Transform(Vector3 const& position, Matrix4x4 const& matrix);
    //    Vector4 Transform(Vector4 const& vector, Matrix4x4 const& matrix);
    //    Vector4 Transform(Vector2 const& value, Quaternion const& rotation);
    //    Vector4 Transform(Vector3 const& value, Quaternion const& rotation);
    //    Vector4 Transform(Vector4 const& value, Quaternion const& rotation);
    //    Vector4 Negate(Vector4 const& value);
    //    Vector4 Add(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Subtract(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Multiply(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Multiply(Vector4 const& value1, float scaleFactor);
    //    Vector4 Divide(Vector4 const& value1, Vector4 const& value2);
    //    Vector4 Divide(Vector4 const& value1, float divider);

    //    // Operator overloads.
    //    Vector4 operator -() const;
    //    Vector4& operator +=(Vector4 const& value);
    //    Vector4& operator -=(Vector4 const& value);
    //    Vector4& operator *=(Vector4 const& value);
    //    Vector4& operator *=(float scaleFactor);
    //    Vector4& operator /=(Vector4 const& value);
    //    Vector4& operator /=(float divider);
    //    bool operator ==(Vector4 const& value) const;
    //    bool operator !=(Vector4 const& value) const;
    //};

    //// Binary operators.
    //Vector4 operator +(Vector4 const& value1, Vector4 const& value2);
    //Vector4 operator -(Vector4 const& value1, Vector4 const& value2);
    //Vector4 operator *(Vector4 const& value1, Vector4 const& value2);
    //Vector4 operator *(Vector4 const& value, float scaleFactor);
    //Vector4 operator *(float scaleFactor, Vector4 const& value);
    //Vector4 operator /(Vector4 const& value1, Vector4 const& value2);
    //Vector4 operator /(Vector4 const& value1, float divider);


    //struct Matrix3x2
    //{
    //    float M11, M12;
    //    float M21, M22;
    //    float M31, M32;

    //    // Constructors.
    //    Matrix3x2();
    //    Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32);

    //    // Common values.
    //    Matrix3x2 Identity();

    //    // Instance methods.
    //    bool IsIdentity() const;
    //    float Determinant() const;
    //    Vector2 Translation() const;
    //    void SetTranslation(Vector2 const& translation);

    //    // methods.
    //    Matrix3x2 CreateTranslation(Vector2 const& position);
    //    Matrix3x2 CreateTranslation(float xPosition, float yPosition);
    //    Matrix3x2 CreateScale(float xScale, float yScale);
    //    Matrix3x2 CreateScale(float xScale, float yScale, Vector2 const& centerPoint);
    //    Matrix3x2 CreateScale(Vector2 const& scales);
    //    Matrix3x2 CreateScale(Vector2 const& scales, Vector2 const& centerPoint);
    //    Matrix3x2 CreateScale(float scale);
    //    Matrix3x2 CreateScale(float scale, Vector2 const& centerPoint);
    //    Matrix3x2 CreateSkew(float radiansX, float radiansY);
    //    Matrix3x2 CreateSkew(float radiansX, float radiansY, Vector2 const& centerPoint);
    //    Matrix3x2 CreateRotation(float radians);
    //    Matrix3x2 CreateRotation(float radians, Vector2 const& centerPoint);
    //    bool Invert(Matrix3x2 const& matrix, _Out_ Matrix3x2* result);
    //    Matrix3x2 Lerp(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2, float amount);
    //    Matrix3x2 Negate(Matrix3x2 const& matrix);
    //    Matrix3x2 Add(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2);
    //    Matrix3x2 Subtract(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2);
    //    Matrix3x2 Multiply(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2);

    //    // Operator overloads.
    //    Matrix3x2 operator -() const;
    //    Matrix3x2& operator +=(Matrix3x2 const& value);
    //    Matrix3x2& operator -=(Matrix3x2 const& value);
    //    Matrix3x2& operator *=(Matrix3x2 const& value);
    //    bool operator ==(Matrix3x2 const& value) const;
    //    bool operator !=(Matrix3x2 const& value) const;
    //};

    //// Binary operators.
    //Matrix3x2 operator +(Matrix3x2 const& value1, Matrix3x2 const& value2);
    //Matrix3x2 operator -(Matrix3x2 const& value1, Matrix3x2 const& value2);
    //Matrix3x2 operator *(Matrix3x2 const& value1, Matrix3x2 const& value2);


    //struct Matrix4x4
    //{
    //    float M11, M12, M13, M14;
    //    float M21, M22, M23, M24;
    //    float M31, M32, M33, M34;
    //    float M41, M42, M43, M44;

    //    // Constructors.
    //    Matrix4x4();
    //    Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
    //    explicit Matrix4x4(Matrix3x2 const& value);

    //    // Common values.
    //    Matrix4x4 Identity();

    //    // Instance methods.
    //    bool IsIdentity() const;
    //    float Determinant() const;
    //    Vector3 Translation() const;
    //    void SetTranslation(Vector3 const& translation);

    //    // methods.
    //    Matrix4x4 CreateBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& cameraUpVector, Vector3 const& cameraForwardVector);
    //    Matrix4x4 CreateConstrainedBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& rotateAxis, Vector3 const& cameraForwardVector, Vector3 const& objectForwardVector);
    //    Matrix4x4 CreateTranslation(Vector3 const& position);
    //    Matrix4x4 CreateTranslation(float xPosition, float yPosition, float zPosition);
    //    Matrix4x4 CreateScale(float xScale, float yScale, float zScale);
    //    Matrix4x4 CreateScale(float xScale, float yScale, float zScale, Vector3 const& centerPoint);
    //    Matrix4x4 CreateScale(Vector3 const& scales);
    //    Matrix4x4 CreateScale(Vector3 const& scales, Vector3 const& centerPoint);
    //    Matrix4x4 CreateScale(float scale);
    //    Matrix4x4 CreateScale(float scale, Vector3 const& centerPoint);
    //    Matrix4x4 CreateRotationX(float radians);
    //    Matrix4x4 CreateRotationX(float radians, Vector3 const& centerPoint);
    //    Matrix4x4 CreateRotationY(float radians);
    //    Matrix4x4 CreateRotationY(float radians, Vector3 const& centerPoint);
    //    Matrix4x4 CreateRotationZ(float radians);
    //    Matrix4x4 CreateRotationZ(float radians, Vector3 const& centerPoint);
    //    Matrix4x4 CreateFromAxisAngle(Vector3 const& axis, float angle);
    //    Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
    //    Matrix4x4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance);
    //    Matrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance);
    //    Matrix4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
    //    Matrix4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
    //    Matrix4x4 CreateLookAt(Vector3 const& cameraPosition, Vector3 const& cameraTarget, Vector3 const& cameraUpVector);
    //    Matrix4x4 CreateWorld(Vector3 const& position, Vector3 const& forward, Vector3 const& up);
    //    Matrix4x4 CreateFromQuaternion(Quaternion const& quaternion);
    //    Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll);
    //    Matrix4x4 CreateShadow(Vector3 const& lightDirection, Plane const& plane);
    //    Matrix4x4 CreateReflection(Plane const& value);
    //    bool Invert(Matrix4x4 const& matrix, _Out_ Matrix4x4* result);
    //    bool Decompose(Matrix4x4 const& matrix, _Out_ Vector3* scale, _Out_ Quaternion* rotation, _Out_ Vector3* translation);
    //    Matrix4x4 Transform(Matrix4x4 const& value, Quaternion const& rotation);
    //    Matrix4x4 Transpose(Matrix4x4 const& matrix);
    //    Matrix4x4 Lerp(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2, float amount);
    //    Matrix4x4 Negate(Matrix4x4 const& matrix);
    //    Matrix4x4 Add(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2);
    //    Matrix4x4 Subtract(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2);
    //    Matrix4x4 Multiply(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2);

    //    // Operator overloads.
    //    Matrix4x4 operator -() const;
    //    Matrix4x4& operator +=(Matrix4x4 const& value);
    //    Matrix4x4& operator -=(Matrix4x4 const& value);
    //    Matrix4x4& operator *=(Matrix4x4 const& value);
    //    bool operator ==(Matrix4x4 const& value) const;
    //    bool operator !=(Matrix4x4 const& value) const;
    //};

    //// Binary operators.
    //Matrix4x4 operator +(Matrix4x4 const& value1, Matrix4x4 const& value2);
    //Matrix4x4 operator -(Matrix4x4 const& value1, Matrix4x4 const& value2);
    //Matrix4x4 operator *(Matrix4x4 const& value1, Matrix4x4 const& value2);


    //struct Plane
    //{
    //    Vector3 Normal;
    //    float D;

    //    // Constructors.
    //    Plane();
    //    Plane(float a, float b, float c, float d);
    //    Plane(Vector3 const& normal, float d);
    //    explicit Plane(Vector4 const& value);
    //    Plane(Vector3 const& point1, Vector3 const& point2, Vector3 const& point3);

    //    // methods.
    //    Plane Normalize(Plane const& value);
    //    Plane Transform(Plane const& plane, Matrix4x4 const& matrix);
    //    Plane Transform(Plane const& plane, Quaternion const& rotation);
    //    float Dot(Plane const& plane, Vector4 const& value);
    //    float DotCoordinate(Plane const& plane, Vector3 const& value);
    //    float DotNormal(Plane const& plane, Vector3 const& value);

    //    // Operator overloads.
    //    bool operator ==(Plane const& value) const;
    //    bool operator !=(Plane const& value) const;
    //};


    //struct Quaternion
    //{
    //    float X, Y, Z, W;

    //    // Constructors.
    //    Quaternion();
    //    Quaternion(float x, float y, float z, float w);
    //    Quaternion(Vector3 const& vectorPart, float scalarPart);

    //    // Common values.
    //    Quaternion Identity();

    //    // Instance methods.
    //    bool IsIdentity() const;
    //    float Length() const;
    //    float LengthSquared() const;

    //    // methods.
    //    Quaternion Normalize(Quaternion const& quaternion);
    //    Quaternion Conjugate(Quaternion const& value);
    //    Quaternion Inverse(Quaternion const& quaternion);
    //    Quaternion CreateFromAxisAngle(Vector3 const& axis, float angle);
    //    Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
    //    Quaternion CreateFromRotationMatrix(Matrix4x4 const& matrix);
    //    float Dot(Quaternion const& quaternion1, Quaternion const& quaternion2);
    //    Quaternion Slerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount);
    //    Quaternion Lerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount);
    //    Quaternion Concatenate(Quaternion const& value1, Quaternion const& value2);
    //    Quaternion Negate(Quaternion const& quaternion);
    //    Quaternion Add(Quaternion const& quaternion1, Quaternion const& quaternion2);
    //    Quaternion Subtract(Quaternion const& quaternion1, Quaternion const& quaternion2);
    //    Quaternion Multiply(Quaternion const& quaternion1, Quaternion const& quaternion2);
    //    Quaternion Multiply(Quaternion const& quaternion1, float scaleFactor);
    //    Quaternion Divide(Quaternion const& quaternion1, Quaternion const& quaternion2);

    //    // Operator overloads.
    //    Quaternion operator -() const;
    //    Quaternion& operator +=(Quaternion const& value);
    //    Quaternion& operator -=(Quaternion const& value);
    //    Quaternion& operator *=(Quaternion const& value);
    //    Quaternion& operator *=(float scaleFactor);
    //    Quaternion& operator /=(Quaternion const& value);
    //    bool operator ==(Quaternion const& value) const;
    //    bool operator !=(Quaternion const& value) const;
    //};

    //    // Binary operators.
    //    Quaternion operator +(Quaternion const& value1, Quaternion const& value2);
    //    Quaternion operator -(Quaternion const& value1, Quaternion const& value2);
    //    Quaternion operator *(Quaternion const& value1, Quaternion const& value2);
    //    Quaternion operator *(Quaternion const& value, float scaleFactor);
    //    Quaternion operator /(Quaternion const& value1, Quaternion const& value2);
    }
}

////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//namespace Windows.Math
//{
//    public struct Vector2 : IEquatable < Vector2 >
//    {
//        public float X;
//        public float Y;
//
//
//        public Vector2 Zero{ get{ return new Vector2(); } }
//        public Vector2 One{ get{ return new Vector2(1.0f, 1.0f); } }
//        public Vector2 UnitX{ get{ return new Vector2(1.0f, 0.0f); } }
//        public Vector2 UnitY{ get{ return new Vector2(0.0f, 1.0f); } }
//
//
//            public Vector2(float x, float y)
//        {
//            X = x;
//            Y = y;
//        }
//
//
//        public Vector2(float value)
//        {
//            X = Y = value;
//        }
//
//
//#if !NO_WINRT
//
//        public implicit operator Windows.Foundation.Point(Vector2 value)
//        {
//            return new Windows.Foundation.Point(value.X, value.Y);
//        }
//
//
//        public implicit operator Windows.Foundation.Size(Vector2 value)
//        {
//            return new Windows.Foundation.Size(value.X, value.Y);
//        }
//
//
//        public implicit operator Vector2(Windows.Foundation.Point value)
//        {
//            Vector2 result;
//
//            result.X = (float)value.X;
//            result.Y = (float)value.Y;
//
//            return result;
//        }
//
//
//        public implicit operator Vector2(Windows.Foundation.Size value)
//        {
//            Vector2 result;
//
//            result.X = (float)value.Width;
//            result.Y = (float)value.Height;
//
//            return result;
//        }
//
//#endif  // !NO_WINRT
//
//
//        public float Length()
//        {
//            float ls = X * X + Y * Y;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float LengthSquared()
//        {
//            return X * X + Y * Y;
//        }
//
//
//        public float Distance(Vector2 value1, Vector2 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//
//            float ls = dx * dx + dy * dy;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float DistanceSquared(Vector2 value1, Vector2 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//
//            return dx * dx + dy * dy;
//        }
//
//
//        public float Dot(Vector2 value1, Vector2 value2)
//        {
//            return value1.X * value2.X +
//                value1.Y * value2.Y;
//        }
//
//
//        public Vector2 Normalize(Vector2 value)
//        {
//            Vector2 ans;
//
//            float ls = value.X * value.X + value.Y * value.Y;
//            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//            ans.X = value.X * invNorm;
//            ans.Y = value.Y * invNorm;
//
//            return ans;
//        }
//
//
//        public Vector2 Reflect(Vector2 vector, Vector2 normal)
//        {
//            Vector2 ans;
//
//            float dot = vector.X * normal.X + vector.Y * normal.Y;
//
//            ans.X = vector.X - 2.0f * dot * normal.X;
//            ans.Y = vector.Y - 2.0f * dot * normal.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Min(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Max(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Clamp(Vector2 value1, Vector2 min, Vector2 max)
//        {
//            Vector2 result;
//
//            // This compare order is very important!!!
//            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
//            float x = value1.X;
//            x = (x > max.X) ? max.X : x;
//            x = (x < min.X) ? min.X : x;
//
//            float y = value1.Y;
//            y = (y > max.Y) ? max.Y : y;
//            y = (y < min.Y) ? min.Y : y;
//
//            result.X = x;
//            result.Y = y;
//
//            return result;
//        }
//
//
//        public Vector2 Lerp(Vector2 value1, Vector2 value2, float amount)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X + (value2.X - value1.X) * amount;
//            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
//
//            return ans;
//        }
//
//
//        public Vector2 Transform(Vector2 position, Matrix3x2 matrix)
//        {
//            Vector2 result;
//
//            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M31;
//            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M32;
//
//            return result;
//        }
//
//
//        public Vector2 Transform(Vector2 position, Matrix4x4 matrix)
//        {
//            Vector2 result;
//
//            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
//            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;
//
//            return result;
//        }
//
//
//        public Vector2 TransformNormal(Vector2 normal, Matrix3x2 matrix)
//        {
//            Vector2 result;
//
//            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
//            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;
//
//            return result;
//        }
//
//
//        public Vector2 TransformNormal(Vector2 normal, Matrix4x4 matrix)
//        {
//            Vector2 result;
//
//            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
//            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;
//
//            return result;
//        }
//
//
//        public Vector2 Transform(Vector2 value, Quaternion rotation)
//        {
//            Vector2 result;
//
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float yy2 = rotation.Y * y2;
//            float zz2 = rotation.Z * z2;
//
//            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2);
//            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2);
//
//            return result;
//        }
//
//
//        public Vector2 Negate(Vector2 value)
//        {
//            Vector2 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Add(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Subtract(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Multiply(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Multiply(Vector2 value1, float scaleFactor)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X * scaleFactor;
//            ans.Y = value1.Y * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector2 Divide(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 Divide(Vector2 value1, float divider)
//        {
//            Vector2 ans;
//
//            float invDiv = 1.0f / divider;
//
//            ans.X = value1.X * invDiv;
//            ans.Y = value1.Y * invDiv;
//
//            return ans;
//        }
//
//
//        public Vector2 operator -(Vector2 value)
//        {
//            Vector2 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 operator +(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 operator -(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 operator *(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 operator *(Vector2 value, float scaleFactor)
//        {
//            Vector2 ans;
//
//            ans.X = value.X * scaleFactor;
//            ans.Y = value.Y * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector2 operator *(float scaleFactor, Vector2 value)
//        {
//            Vector2 ans;
//
//            ans.X = value.X * scaleFactor;
//            ans.Y = value.Y * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector2 operator /(Vector2 value1, Vector2 value2)
//        {
//            Vector2 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//
//            return ans;
//        }
//
//
//        public Vector2 operator /(Vector2 value1, float divider)
//        {
//            Vector2 ans;
//
//            float invDiv = 1.0f / divider;
//
//            ans.X = value1.X * invDiv;
//            ans.Y = value1.Y * invDiv;
//
//            return ans;
//        }
//
//
//        public bool operator ==(Vector2 value1, Vector2 value2)
//        {
//            return (value1.X == value2.X &&
//                value1.Y == value2.Y);
//        }
//
//
//        public bool operator !=(Vector2 value1, Vector2 value2)
//        {
//            return (value1.X != value2.X ||
//                value1.Y != value2.Y);
//        }
//
//
//        public bool Equals(Vector2 other)
//        {
//            return X == other.X &&
//                Y == other.Y;
//        }
//
//
//        public override bool Equals(object obj)
//        {
//            if (obj is Vector2)
//            {
//                return Equals((Vector2)obj);
//            }
//
//            return false;
//        }
//
//
//        public override string ToString()
//        {
//            CultureInfo ci = CultureInfo.CurrentCulture;
//
//            return String.Format(ci, "{{X:{0} Y:{1}}}", X.ToString(ci), Y.ToString(ci));
//        }
//
//
//        public override int GetHashCode()
//        {
//            return X.GetHashCode() + Y.GetHashCode();
//        }
//    }
//}
//
//
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Vector3 : IEquatable < Vector3 >
//    {
//        public float X;
//        public float Y;
//        public float Z;
//
//
//        public Vector3 Zero{ get{ return new Vector3(); } }
//        public Vector3 One{ get{ return new Vector3(1.0f, 1.0f, 1.0f); } }
//        public Vector3 UnitX{ get{ return new Vector3(1.0f, 0.0f, 0.0f); } }
//        public Vector3 UnitY{ get{ return new Vector3(0.0f, 1.0f, 0.0f); } }
//        public Vector3 UnitZ{ get{ return new Vector3(0.0f, 0.0f, 1.0f); } }
//
//
//            public Vector3(float x, float y, float z)
//        {
//            X = x;
//            Y = y;
//            Z = z;
//        }
//
//
//        public Vector3(Vector2 value, float z)
//        {
//            X = value.X;
//            Y = value.Y;
//            Z = z;
//        }
//
//
//        public Vector3(float value)
//        {
//            X = Y = Z = value;
//        }
//
//
//        public float Length()
//        {
//            float ls = X * X + Y * Y + Z * Z;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float LengthSquared()
//        {
//            return X * X + Y * Y + Z * Z;
//        }
//
//
//        public float Distance(Vector3 value1, Vector3 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//            float dz = value1.Z - value2.Z;
//
//            float ls = dx * dx + dy * dy + dz * dz;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float DistanceSquared(Vector3 value1, Vector3 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//            float dz = value1.Z - value2.Z;
//
//            return dx * dx + dy * dy + dz * dz;
//        }
//
//
//        public float Dot(Vector3 vector1, Vector3 vector2)
//        {
//            return vector1.X * vector2.X +
//                vector1.Y * vector2.Y +
//                vector1.Z * vector2.Z;
//        }
//
//
//        public Vector3 Normalize(Vector3 value)
//        {
//            Vector3 ans;
//
//            float ls = value.X * value.X + value.Y * value.Y + value.Z * value.Z;
//            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//            ans.X = value.X * invNorm;
//            ans.Y = value.Y * invNorm;
//            ans.Z = value.Z * invNorm;
//
//            return ans;
//        }
//
//
//        public Vector3 Cross(Vector3 vector1, Vector3 vector2)
//        {
//            Vector3 ans;
//
//            ans.X = vector1.Y * vector2.Z - vector1.Z * vector2.Y;
//            ans.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
//            ans.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;
//
//            return ans;
//        }
//
//
//        public Vector3 Reflect(Vector3 vector, Vector3 normal)
//        {
//            Vector3 ans;
//
//            float dot = vector.X * normal.X + vector.Y * normal.Y + vector.Z * normal.Z;
//
//            ans.X = vector.X - 2.0f * dot * normal.X;
//            ans.Y = vector.Y - 2.0f * dot * normal.Y;
//            ans.Z = vector.Z - 2.0f * dot * normal.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Min(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
//            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Max(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
//            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Clamp(Vector3 value1, Vector3 min, Vector3 max)
//        {
//            // This compare order is very important!!!
//            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
//            Vector3 result;
//
//            float x = value1.X;
//            x = (x > max.X) ? max.X : x;
//            x = (x < min.X) ? min.X : x;
//
//            float y = value1.Y;
//            y = (y > max.Y) ? max.Y : y;
//            y = (y < min.Y) ? min.Y : y;
//
//            float z = value1.Z;
//            z = (z > max.Z) ? max.Z : z;
//            z = (z < min.Z) ? min.Z : z;
//
//            result.X = x;
//            result.Y = y;
//            result.Z = z;
//
//            return result;
//        }
//
//
//        public Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X + (value2.X - value1.X) * amount;
//            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
//            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;
//
//            return ans;
//        }
//
//
//        public Vector3 Transform(Vector3 position, Matrix4x4 matrix)
//        {
//            Vector3 result;
//
//            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
//            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
//            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;
//
//            return result;
//        }
//
//
//        public Vector3 TransformNormal(Vector3 normal, Matrix4x4 matrix)
//        {
//            Vector3 result;
//
//            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21 + normal.Z * matrix.M31;
//            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22 + normal.Z * matrix.M32;
//            result.Z = normal.X * matrix.M13 + normal.Y * matrix.M23 + normal.Z * matrix.M33;
//
//            return result;
//        }
//
//
//        public Vector3 Transform(Vector3 value, Quaternion rotation)
//        {
//            Vector3 result;
//
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wx2 = rotation.W * x2;
//            float wy2 = rotation.W * y2;
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float xz2 = rotation.X * z2;
//            float yy2 = rotation.Y * y2;
//            float yz2 = rotation.Y * z2;
//            float zz2 = rotation.Z * z2;
//
//            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2);
//            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2);
//            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);
//
//            return result;
//        }
//
//
//        public Vector3 Negate(Vector3 value)
//        {
//            Vector3 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//            ans.Z = -value.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Add(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//            ans.Z = value1.Z + value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Subtract(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//            ans.Z = value1.Z - value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Multiply(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//            ans.Z = value1.Z * value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Multiply(Vector3 value1, float scaleFactor)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X * scaleFactor;
//            ans.Y = value1.Y * scaleFactor;
//            ans.Z = value1.Z * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector3 Divide(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//            ans.Z = value1.Z / value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 Divide(Vector3 value1, float value2)
//        {
//            Vector3 ans;
//
//            float invDiv = 1.0f / value2;
//
//            ans.X = value1.X * invDiv;
//            ans.Y = value1.Y * invDiv;
//            ans.Z = value1.Z * invDiv;
//
//            return ans;
//        }
//
//
//        public Vector3 operator -(Vector3 value)
//        {
//            Vector3 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//            ans.Z = -value.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 operator +(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//            ans.Z = value1.Z + value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 operator -(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//            ans.Z = value1.Z - value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 operator *(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//            ans.Z = value1.Z * value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 operator *(Vector3 value, float scaleFactor)
//        {
//            Vector3 ans;
//
//            ans.X = value.X * scaleFactor;
//            ans.Y = value.Y * scaleFactor;
//            ans.Z = value.Z * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector3 operator *(float scaleFactor, Vector3 value)
//        {
//            Vector3 ans;
//
//            ans.X = value.X * scaleFactor;
//            ans.Y = value.Y * scaleFactor;
//            ans.Z = value.Z * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector3 operator /(Vector3 value1, Vector3 value2)
//        {
//            Vector3 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//            ans.Z = value1.Z / value2.Z;
//
//            return ans;
//        }
//
//
//        public Vector3 operator /(Vector3 value, float divider)
//        {
//            Vector3 ans;
//
//            float invDiv = 1.0f / divider;
//
//            ans.X = value.X * invDiv;
//            ans.Y = value.Y * invDiv;
//            ans.Z = value.Z * invDiv;
//
//            return ans;
//        }
//
//
//        public bool operator ==(Vector3 value1, Vector3 value2)
//        {
//            return (value1.X == value2.X &&
//                value1.Y == value2.Y &&
//                value1.Z == value2.Z);
//        }
//
//
//        public bool operator !=(Vector3 value1, Vector3 value2)
//        {
//            return (value1.X != value2.X ||
//                value1.Y != value2.Y ||
//                value1.Z != value2.Z);
//        }
//
//
//        public bool Equals(Vector3 other)
//        {
//            return X == other.X &&
//                Y == other.Y &&
//                Z == other.Z;
//        }
//
//
//        public override bool Equals(object obj)
//        {
//            if (obj is Vector3)
//            {
//                return Equals((Vector3)obj);
//            }
//
//            return false;
//        }
//
//
//        public override string ToString()
//        {
//            CultureInfo ci = CultureInfo.CurrentCulture;
//
//            return String.Format(ci, "{{X:{0} Y:{1} Z:{2}}}", X.ToString(ci), Y.ToString(ci), Z.ToString(ci));
//        }
//
//
//        public override int GetHashCode()
//        {
//            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
//        }
//    }
//}
//
//
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Vector4 : IEquatable < Vector4 >
//    {
//        public float X;
//        public float Y;
//        public float Z;
//        public float W;
//
//
//        public Vector4 Zero{ get{ return new Vector4(); } }
//        public Vector4 One{ get{ return new Vector4(1.0f, 1.0f, 1.0f, 1.0f); } }
//        public Vector4 UnitX{ get{ return new Vector4(1.0f, 0.0f, 0.0f, 0.0f); } }
//        public Vector4 UnitY{ get{ return new Vector4(0.0f, 1.0f, 0.0f, 0.0f); } }
//        public Vector4 UnitZ{ get{ return new Vector4(0.0f, 0.0f, 1.0f, 0.0f); } }
//        public Vector4 UnitW{ get{ return new Vector4(0.0f, 0.0f, 0.0f, 1.0f); } }
//
//
//            public Vector4(float x, float y, float z, float w)
//        {
//            X = x;
//            Y = y;
//            Z = z;
//            W = w;
//        }
//
//
//        public Vector4(Vector2 value, float z, float w)
//        {
//            X = value.X;
//            Y = value.Y;
//            Z = z;
//            W = w;
//        }
//
//
//        public Vector4(Vector3 value, float w)
//        {
//            X = value.X;
//            Y = value.Y;
//            Z = value.Z;
//            W = w;
//        }
//
//
//        public Vector4(float value)
//        {
//            X = Y = Z = W = value;
//        }
//
//
//        public float Length()
//        {
//            float ls = X * X + Y * Y + Z * Z + W * W;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float LengthSquared()
//        {
//            return X * X + Y * Y + Z * Z + W * W;
//        }
//
//
//        public float Distance(Vector4 value1, Vector4 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//            float dz = value1.Z - value2.Z;
//            float dw = value1.W - value2.W;
//
//            float ls = dx * dx + dy * dy + dz * dz + dw * dw;
//
//            return (float)SM.Sqrt((double)ls);
//        }
//
//
//        public float DistanceSquared(Vector4 value1, Vector4 value2)
//        {
//            float dx = value1.X - value2.X;
//            float dy = value1.Y - value2.Y;
//            float dz = value1.Z - value2.Z;
//            float dw = value1.W - value2.W;
//
//            return dx * dx + dy * dy + dz * dz + dw * dw;
//        }
//
//
//        public float Dot(Vector4 vector1, Vector4 vector2)
//        {
//            return vector1.X * vector2.X +
//                vector1.Y * vector2.Y +
//                vector1.Z * vector2.Z +
//                vector1.W * vector2.W;
//        }
//
//
//        public Vector4 Normalize(Vector4 vector)
//        {
//            Vector4 ans;
//
//            float ls = vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z + vector.W * vector.W;
//            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//            ans.X = vector.X * invNorm;
//            ans.Y = vector.Y * invNorm;
//            ans.Z = vector.Z * invNorm;
//            ans.W = vector.W * invNorm;
//
//            return ans;
//        }
//
//
//        public Vector4 Min(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
//            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;
//            ans.W = (value1.W < value2.W) ? value1.W : value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Max(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
//            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
//            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;
//            ans.W = (value1.W > value2.W) ? value1.W : value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Clamp(Vector4 value1, Vector4 min, Vector4 max)
//        {
//            // This compare order is very important!!!
//            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
//            Vector4 result;
//
//            float x = value1.X;
//            x = (x > max.X) ? max.X : x;
//            x = (x < min.X) ? min.X : x;
//
//            float y = value1.Y;
//            y = (y > max.Y) ? max.Y : y;
//            y = (y < min.Y) ? min.Y : y;
//
//            float z = value1.Z;
//            z = (z > max.Z) ? max.Z : z;
//            z = (z < min.Z) ? min.Z : z;
//
//            float w = value1.W;
//            w = (w > max.W) ? max.W : w;
//            w = (w < min.W) ? min.W : w;
//
//            result.X = x;
//            result.Y = y;
//            result.Z = z;
//            result.W = w;
//
//            return result;
//        }
//
//
//        public Vector4 Lerp(Vector4 value1, Vector4 value2, float amount)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X + (value2.X - value1.X) * amount;
//            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
//            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;
//            ans.W = value1.W + (value2.W - value1.W) * amount;
//
//            return ans;
//        }
//
//
//        public Vector4 Transform(Vector2 position, Matrix4x4 matrix)
//        {
//            Vector4 result;
//
//            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
//            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;
//            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + matrix.M43;
//            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + matrix.M44;
//
//            return result;
//        }
//
//
//        public Vector4 Transform(Vector3 position, Matrix4x4 matrix)
//        {
//            Vector4 result;
//
//            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
//            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
//            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;
//            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + position.Z * matrix.M34 + matrix.M44;
//
//            return result;
//        }
//
//
//        public Vector4 Transform(Vector4 vector, Matrix4x4 matrix)
//        {
//            Vector4 result;
//
//            result.X = vector.X * matrix.M11 + vector.Y * matrix.M21 + vector.Z * matrix.M31 + vector.W * matrix.M41;
//            result.Y = vector.X * matrix.M12 + vector.Y * matrix.M22 + vector.Z * matrix.M32 + vector.W * matrix.M42;
//            result.Z = vector.X * matrix.M13 + vector.Y * matrix.M23 + vector.Z * matrix.M33 + vector.W * matrix.M43;
//            result.W = vector.X * matrix.M14 + vector.Y * matrix.M24 + vector.Z * matrix.M34 + vector.W * matrix.M44;
//
//            return result;
//        }
//
//
//        public Vector4 Transform(Vector2 value, Quaternion rotation)
//        {
//            Vector4 result;
//
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wx2 = rotation.W * x2;
//            float wy2 = rotation.W * y2;
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float xz2 = rotation.X * z2;
//            float yy2 = rotation.Y * y2;
//            float yz2 = rotation.Y * z2;
//            float zz2 = rotation.Z * z2;
//
//            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2);
//            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2);
//            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2);
//            result.W = 1.0f;
//
//            return result;
//        }
//
//
//        public Vector4 Transform(Vector3 value, Quaternion rotation)
//        {
//            Vector4 result;
//
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wx2 = rotation.W * x2;
//            float wy2 = rotation.W * y2;
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float xz2 = rotation.X * z2;
//            float yy2 = rotation.Y * y2;
//            float yz2 = rotation.Y * z2;
//            float zz2 = rotation.Z * z2;
//
//            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2);
//            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2);
//            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);
//            result.W = 1.0f;
//
//            return result;
//        }
//
//
//        public Vector4 Transform(Vector4 value, Quaternion rotation)
//        {
//            Vector4 result;
//
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wx2 = rotation.W * x2;
//            float wy2 = rotation.W * y2;
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float xz2 = rotation.X * z2;
//            float yy2 = rotation.Y * y2;
//            float yz2 = rotation.Y * z2;
//            float zz2 = rotation.Z * z2;
//
//            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2);
//            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2);
//            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);
//            result.W = value.W;
//
//            return result;
//        }
//
//
//        public Vector4 Negate(Vector4 value)
//        {
//            Vector4 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//            ans.Z = -value.Z;
//            ans.W = -value.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Add(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//            ans.Z = value1.Z + value2.Z;
//            ans.W = value1.W + value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Subtract(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//            ans.Z = value1.Z - value2.Z;
//            ans.W = value1.W - value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Multiply(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//            ans.Z = value1.Z * value2.Z;
//            ans.W = value1.W * value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Multiply(Vector4 value1, float scaleFactor)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X * scaleFactor;
//            ans.Y = value1.Y * scaleFactor;
//            ans.Z = value1.Z * scaleFactor;
//            ans.W = value1.W * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector4 Divide(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//            ans.Z = value1.Z / value2.Z;
//            ans.W = value1.W / value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 Divide(Vector4 value1, float divider)
//        {
//            Vector4 ans;
//
//            float invDiv = 1.0f / divider;
//
//            ans.X = value1.X * invDiv;
//            ans.Y = value1.Y * invDiv;
//            ans.Z = value1.Z * invDiv;
//            ans.W = value1.W * invDiv;
//
//            return ans;
//        }
//
//
//        public Vector4 operator -(Vector4 value)
//        {
//            Vector4 ans;
//
//            ans.X = -value.X;
//            ans.Y = -value.Y;
//            ans.Z = -value.Z;
//            ans.W = -value.W;
//
//            return ans;
//        }
//
//
//        public Vector4 operator +(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X + value2.X;
//            ans.Y = value1.Y + value2.Y;
//            ans.Z = value1.Z + value2.Z;
//            ans.W = value1.W + value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 operator -(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X - value2.X;
//            ans.Y = value1.Y - value2.Y;
//            ans.Z = value1.Z - value2.Z;
//            ans.W = value1.W - value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 operator *(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X * value2.X;
//            ans.Y = value1.Y * value2.Y;
//            ans.Z = value1.Z * value2.Z;
//            ans.W = value1.W * value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 operator *(Vector4 value1, float scaleFactor)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X * scaleFactor;
//            ans.Y = value1.Y * scaleFactor;
//            ans.Z = value1.Z * scaleFactor;
//            ans.W = value1.W * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector4 operator *(float scaleFactor, Vector4 value1)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X * scaleFactor;
//            ans.Y = value1.Y * scaleFactor;
//            ans.Z = value1.Z * scaleFactor;
//            ans.W = value1.W * scaleFactor;
//
//            return ans;
//        }
//
//
//        public Vector4 operator /(Vector4 value1, Vector4 value2)
//        {
//            Vector4 ans;
//
//            ans.X = value1.X / value2.X;
//            ans.Y = value1.Y / value2.Y;
//            ans.Z = value1.Z / value2.Z;
//            ans.W = value1.W / value2.W;
//
//            return ans;
//        }
//
//
//        public Vector4 operator /(Vector4 value1, float divider)
//        {
//            Vector4 ans;
//
//            float invDiv = 1.0f / divider;
//
//            ans.X = value1.X * invDiv;
//            ans.Y = value1.Y * invDiv;
//            ans.Z = value1.Z * invDiv;
//            ans.W = value1.W * invDiv;
//
//            return ans;
//        }
//
//
//        public bool operator ==(Vector4 value1, Vector4 value2)
//        {
//            return (value1.X == value2.X &&
//                value1.Y == value2.Y &&
//                value1.Z == value2.Z &&
//                value1.W == value2.W);
//        }
//
//
//        public bool operator !=(Vector4 value1, Vector4 value2)
//        {
//            return (value1.X != value2.X ||
//                value1.Y != value2.Y ||
//                value1.Z != value2.Z ||
//                value1.W != value2.W);
//        }
//
//
//        public bool Equals(Vector4 other)
//        {
//            return X == other.X &&
//                Y == other.Y &&
//                Z == other.Z &&
//                W == other.W;
//        }
//
//
//        public override bool Equals(object obj)
//        {
//            if (obj is Vector4)
//            {
//                return Equals((Vector4)obj);
//            }
//
//            return false;
//        }
//
//
//        public override string ToString()
//        {
//            CultureInfo ci = CultureInfo.CurrentCulture;
//
//            return String.Format(ci, "{{X:{0} Y:{1} Z:{2} W:{3}}}", X.ToString(ci), Y.ToString(ci), Z.ToString(ci), W.ToString(ci));
//        }
//
//
//        public override int GetHashCode()
//        {
//            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
//        }
//    }
//}
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Matrix3x2 : IEquatable < Matrix3x2 >
//    {
//        public float M11;
//        public float M12;
//
//        public float M21;
//        public float M22;
//
//        public float M31;
//        public float M32;
//
//
//        private Matrix3x2 _identity = new Matrix3x2
//            (
//            1f, 0f,
//            0f, 1f,
//            0f, 0f
//            );
//
//
//        public Matrix3x2 Identity
//        {
//            get{ return _identity; }
//        }
//
//
//        public bool IsIdentity
//            {
//                get
//                {
//                    return M11 == 1f && M22 == 1f && // Check diagonal element first for early out.
//                    M12 == 0f &&
//                    M21 == 0f &&
//                    M31 == 0f && M32 == 0f;
//                }
//            }
//
//
//            public Vector2 Translation
//                {
//                    get
//                    {
//                        Vector2 ans;
//
//                        ans.X = M31;
//                        ans.Y = M32;
//
//                        return ans;
//                    }
//
//                        set
//                    {
//                        M31 = value.X;
//                        M32 = value.Y;
//                    }
//                }
//
//
//                public Matrix3x2(float m11, float m12,
//                float m21, float m22,
//                float m31, float m32)
//                    {
//                        M11 = m11; M12 = m12;
//                        M21 = m21; M22 = m22;
//                        M31 = m31; M32 = m32;
//                    }
//
//
//                    public Matrix3x2 CreateTranslation(Vector2 position)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = 1.0f; result.M12 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f;
//
//                        result.M31 = position.X;
//                        result.M32 = position.Y;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateTranslation(float xPosition, float yPosition)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = 1.0f; result.M12 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f;
//
//                        result.M31 = xPosition;
//                        result.M32 = yPosition;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(float xScale, float yScale)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = xScale; result.M12 = 0.0f;
//                        result.M21 = 0.0f;   result.M22 = yScale;
//                        result.M31 = 0.0f;   result.M32 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(float xScale, float yScale, Vector2 centerPoint)
//                    {
//                        Matrix3x2 result;
//
//                        float tx = centerPoint.X * (1 - xScale);
//                        float ty = centerPoint.Y * (1 - yScale);
//
//                        result.M11 = xScale; result.M12 = 0.0f;
//                        result.M21 = 0.0f;   result.M22 = yScale;
//                        result.M31 = tx;     result.M32 = ty;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(Vector2 scales)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = scales.X; result.M12 = 0.0f;
//                        result.M21 = 0.0f;     result.M22 = scales.Y;
//                        result.M31 = 0.0f;     result.M32 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(Vector2 scales, Vector2 centerPoint)
//                    {
//                        Matrix3x2 result;
//
//                        float tx = centerPoint.X * (1 - scales.X);
//                        float ty = centerPoint.Y * (1 - scales.Y);
//
//                        result.M11 = scales.X; result.M12 = 0.0f;
//                        result.M21 = 0.0f;     result.M22 = scales.Y;
//                        result.M31 = tx;       result.M32 = ty;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(float scale)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = scale; result.M12 = 0.0f;
//                        result.M21 = 0.0f;  result.M22 = scale;
//                        result.M31 = 0.0f;  result.M32 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateScale(float scale, Vector2 centerPoint)
//                    {
//                        Matrix3x2 result;
//
//                        float tx = centerPoint.X * (1 - scale);
//                        float ty = centerPoint.Y * (1 - scale);
//
//                        result.M11 = scale; result.M12 = 0.0f;
//                        result.M21 = 0.0f;  result.M22 = scale;
//                        result.M31 = tx;    result.M32 = ty;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateSkew(float radiansX, float radiansY)
//                    {
//                        Matrix3x2 result;
//
//                        float xTan = (float)SM.Tan(radiansX);
//                        float yTan = (float)SM.Tan(radiansY);
//
//                        result.M11 = 1.0f; result.M12 = yTan;
//                        result.M21 = xTan; result.M22 = 1.0f;
//                        result.M31 = 0.0f; result.M32 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateSkew(float radiansX, float radiansY, Vector2 centerPoint)
//                    {
//                        Matrix3x2 result;
//
//                        float xTan = (float)SM.Tan(radiansX);
//                        float yTan = (float)SM.Tan(radiansY);
//
//                        float tx = -centerPoint.Y * xTan;
//                        float ty = -centerPoint.X * yTan;
//
//                        result.M11 = 1.0f; result.M12 = yTan;
//                        result.M21 = xTan; result.M22 = 1.0f;
//                        result.M31 = tx;   result.M32 = ty;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateRotation(float radians)
//                    {
//                        Matrix3x2 result;
//
//                        radians = (float)SM.IEEERemainder(radians, SM.PI * 2);
//
//                        float c, s;
//
//                        const float epsilon = 0.001f * (float)SM.PI / 180f;     // 0.1% of a degree
//
//                        if (radians > -epsilon && radians < epsilon)
//                        {
//                            // Exact case for zero rotation.
//                            c = 1;
//                            s = 0;
//                        }
//                        else if (radians > SM.PI / 2 - epsilon && radians < SM.PI / 2 + epsilon)
//                        {
//                            // Exact case for 90 degree rotation.
//                            c = 0;
//                            s = 1;
//                        }
//                        else if (radians < -SM.PI + epsilon || radians > SM.PI - epsilon)
//                        {
//                            // Exact case for 180 degree rotation.
//                            c = -1;
//                            s = 0;
//                        }
//                        else if (radians > -SM.PI / 2 - epsilon && radians < -SM.PI / 2 + epsilon)
//                        {
//                            // Exact case for 270 degree rotation.
//                            c = 0;
//                            s = -1;
//                        }
//                        else
//                        {
//                            // Arbitrary rotation.
//                            c = (float)SM.Cos(radians);
//                            s = (float)SM.Sin(radians);
//                        }
//
//                        // [  c  s ]
//                        // [ -s  c ]
//                        // [  0  0 ]
//                        result.M11 = c; result.M12 = s;
//                        result.M21 = -s; result.M22 = c;
//                        result.M31 = 0.0f; result.M32 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 CreateRotation(float radians, Vector2 centerPoint)
//                    {
//                        Matrix3x2 result;
//
//                        radians = (float)SM.IEEERemainder(radians, SM.PI * 2);
//
//                        float c, s;
//
//                        const float epsilon = 0.001f * (float)SM.PI / 180f;     // 0.1% of a degree
//
//                        if (radians > -epsilon && radians < epsilon)
//                        {
//                            // Exact case for zero rotation.
//                            c = 1;
//                            s = 0;
//                        }
//                        else if (radians > SM.PI / 2 - epsilon && radians < SM.PI / 2 + epsilon)
//                        {
//                            // Exact case for 90 degree rotation.
//                            c = 0;
//                            s = 1;
//                        }
//                        else if (radians < -SM.PI + epsilon || radians > SM.PI - epsilon)
//                        {
//                            // Exact case for 180 degree rotation.
//                            c = -1;
//                            s = 0;
//                        }
//                        else if (radians > -SM.PI / 2 - epsilon && radians < -SM.PI / 2 + epsilon)
//                        {
//                            // Exact case for 270 degree rotation.
//                            c = 0;
//                            s = -1;
//                        }
//                        else
//                        {
//                            // Arbitrary rotation.
//                            c = (float)SM.Cos(radians);
//                            s = (float)SM.Sin(radians);
//                        }
//
//                        float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
//                        float y = centerPoint.Y * (1 - c) - centerPoint.X * s;
//
//                        // [  c  s ]
//                        // [ -s  c ]
//                        // [  x  y ]
//                        result.M11 = c; result.M12 = s;
//                        result.M21 = -s; result.M22 = c;
//                        result.M31 = x; result.M32 = y;
//
//                        return result;
//                    }
//
//
//                    public float Determinant()
//                    {
//                        // There isn't actually any such thing as a determinant for a non-square matrix,
//                        // but this 3x2 type is really just an optimization of a 3x3 where we happen to
//                        // know the rightmost column is always (0, 0, 1). So we expand to 3x3 format:
//                        //
//                        //  [ M11, M12, 0 ]
//                        //  [ M21, M22, 0 ]
//                        //  [ M31, M32, 1 ]
//                        //
//                        // Sum the diagnonal products:
//                        //  (M11 * M22 * 1) + (M12 * 0 * M31) + (0 * M21 * M32)
//                        //
//                        // Subtract the opposite diagonal products:
//                        //  (M31 * M22 * 0) + (M32 * 0 * M11) + (1 * M21 * M12)
//                        //
//                        // Collapse out the constants and oh look, this is just a 2x2 determinant!
//
//                        return (M11 * M22) - (M21 * M12);
//                    }
//
//
//                    public bool Invert(Matrix3x2 matrix, out Matrix3x2 result)
//                    {
//                        float det = (matrix.M11 * matrix.M22) - (matrix.M21 * matrix.M12);
//
//                        if (SM.Abs(det) < float.Epsilon)
//                        {
//                            result = new Matrix3x2(float.NaN, float.NaN, float.NaN, float.NaN, float.NaN, float.NaN);
//                            return false;
//                        }
//
//                        float invDet = 1.0f / det;
//
//                        result.M11 = matrix.M22 * invDet;
//                        result.M12 = -matrix.M12 * invDet;
//                        result.M21 = -matrix.M21 * invDet;
//                        result.M22 = matrix.M11 * invDet;
//                        result.M31 = (matrix.M21 * matrix.M32 - matrix.M31 * matrix.M22) * invDet;
//                        result.M32 = (matrix.M31 * matrix.M12 - matrix.M11 * matrix.M32) * invDet;
//
//                        return true;
//                    }
//
//
//                    public Matrix3x2 Lerp(Matrix3x2 matrix1, Matrix3x2 matrix2, float amount)
//                    {
//                        Matrix3x2 result;
//
//                        // First row
//                        result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
//                        result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
//
//                        // Second row
//                        result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
//                        result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
//
//                        // Third row
//                        result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
//                        result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 Negate(Matrix3x2 matrix)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = -matrix.M11; result.M12 = -matrix.M12;
//                        result.M21 = -matrix.M21; result.M22 = -matrix.M22;
//                        result.M31 = -matrix.M31; result.M32 = -matrix.M32;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 Add(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = matrix1.M11 + matrix2.M11; result.M12 = matrix1.M12 + matrix2.M12;
//                        result.M21 = matrix1.M21 + matrix2.M21; result.M22 = matrix1.M22 + matrix2.M22;
//                        result.M31 = matrix1.M31 + matrix2.M31; result.M32 = matrix1.M32 + matrix2.M32;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 Subtract(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 result;
//
//                        result.M11 = matrix1.M11 - matrix2.M11; result.M12 = matrix1.M12 - matrix2.M12;
//                        result.M21 = matrix1.M21 - matrix2.M21; result.M22 = matrix1.M22 - matrix2.M22;
//                        result.M31 = matrix1.M31 - matrix2.M31; result.M32 = matrix1.M32 - matrix2.M32;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 Multiply(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 result;
//
//                        // First row
//                        result.M11 = matrix1.M11 * matrix2.M11 + matrix1.M12 * matrix2.M21;
//                        result.M12 = matrix1.M11 * matrix2.M12 + matrix1.M12 * matrix2.M22;
//
//                        // Second row
//                        result.M21 = matrix1.M21 * matrix2.M11 + matrix1.M22 * matrix2.M21;
//                        result.M22 = matrix1.M21 * matrix2.M12 + matrix1.M22 * matrix2.M22;
//
//                        // Third row
//                        result.M31 = matrix1.M31 * matrix2.M11 + matrix1.M32 * matrix2.M21 + matrix2.M31;
//                        result.M32 = matrix1.M31 * matrix2.M12 + matrix1.M32 * matrix2.M22 + matrix2.M32;
//
//                        return result;
//                    }
//
//
//                    public Matrix3x2 operator -(Matrix3x2 matrix1)
//                    {
//                        Matrix3x2 m;
//
//                        m.M11 = -matrix1.M11; m.M12 = -matrix1.M12;
//                        m.M21 = -matrix1.M21; m.M22 = -matrix1.M22;
//                        m.M31 = -matrix1.M31; m.M32 = -matrix1.M32;
//
//                        return m;
//                    }
//
//
//                    public Matrix3x2 operator +(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 m;
//
//                        m.M11 = matrix1.M11 + matrix2.M11; m.M12 = matrix1.M12 + matrix2.M12;
//                        m.M21 = matrix1.M21 + matrix2.M21; m.M22 = matrix1.M22 + matrix2.M22;
//                        m.M31 = matrix1.M31 + matrix2.M31; m.M32 = matrix1.M32 + matrix2.M32;
//
//                        return m;
//                    }
//
//
//                    public Matrix3x2 operator -(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 m;
//
//                        m.M11 = matrix1.M11 - matrix2.M11; m.M12 = matrix1.M12 - matrix2.M12;
//                        m.M21 = matrix1.M21 - matrix2.M21; m.M22 = matrix1.M22 - matrix2.M22;
//                        m.M31 = matrix1.M31 - matrix2.M31; m.M32 = matrix1.M32 - matrix2.M32;
//
//                        return m;
//                    }
//
//
//                    public Matrix3x2 operator *(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        Matrix3x2 m;
//
//                        // First row
//                        m.M11 = matrix1.M11 * matrix2.M11 + matrix1.M12 * matrix2.M21;
//                        m.M12 = matrix1.M11 * matrix2.M12 + matrix1.M12 * matrix2.M22;
//
//                        // Second row
//                        m.M21 = matrix1.M21 * matrix2.M11 + matrix1.M22 * matrix2.M21;
//                        m.M22 = matrix1.M21 * matrix2.M12 + matrix1.M22 * matrix2.M22;
//
//                        // Third row
//                        m.M31 = matrix1.M31 * matrix2.M11 + matrix1.M32 * matrix2.M21 + matrix2.M31;
//                        m.M32 = matrix1.M31 * matrix2.M12 + matrix1.M32 * matrix2.M22 + matrix2.M32;
//
//                        return m;
//                    }
//
//
//                    public bool operator ==(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        return (matrix1.M11 == matrix2.M11 && matrix1.M22 == matrix2.M22 && // Check diagonal element first for early out.
//                            matrix1.M12 == matrix2.M12 &&
//                            matrix1.M21 == matrix2.M21 &&
//                            matrix1.M31 == matrix2.M31 && matrix1.M32 == matrix2.M32);
//                    }
//
//
//                    public bool operator !=(Matrix3x2 matrix1, Matrix3x2 matrix2)
//                    {
//                        return (matrix1.M11 != matrix2.M11 || matrix1.M12 != matrix2.M12 ||
//                            matrix1.M21 != matrix2.M21 || matrix1.M22 != matrix2.M22 ||
//                            matrix1.M31 != matrix2.M31 || matrix1.M32 != matrix2.M32);
//                    }
//
//
//                    public bool Equals(Matrix3x2 other)
//                    {
//                        return (M11 == other.M11 && M22 == other.M22 && // Check diagonal element first for early out.
//                            M12 == other.M12 &&
//                            M21 == other.M21 &&
//                            M31 == other.M31 && M32 == other.M32);
//                    }
//
//
//                    public override bool Equals(object obj)
//                    {
//                        if (obj is Matrix3x2)
//                        {
//                            return Equals((Matrix3x2)obj);
//                        }
//
//                        return false;
//                    }
//
//
//                    public override string ToString()
//                    {
//                        CultureInfo ci = CultureInfo.CurrentCulture;
//
//                        return String.Format(ci, "{{ {{M11:{0} M12:{1}}} {{M21:{2} M22:{3}}} {{M31:{4} M32:{5}}} }}",
//                            M11.ToString(ci), M12.ToString(ci),
//                            M21.ToString(ci), M22.ToString(ci),
//                            M31.ToString(ci), M32.ToString(ci));
//                    }
//
//
//                    public override int GetHashCode()
//                    {
//                        return M11.GetHashCode() + M12.GetHashCode() +
//                            M21.GetHashCode() + M22.GetHashCode() +
//                            M31.GetHashCode() + M32.GetHashCode();
//                    }
//    }
//}
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Matrix4x4 : IEquatable < Matrix4x4 >
//    {
//        public float M11;
//        public float M12;
//        public float M13;
//        public float M14;
//
//        public float M21;
//        public float M22;
//        public float M23;
//        public float M24;
//
//        public float M31;
//        public float M32;
//        public float M33;
//        public float M34;
//
//        public float M41;
//        public float M42;
//        public float M43;
//        public float M44;
//
//
//        private Matrix4x4 _identity = new Matrix4x4
//            (
//            1f, 0f, 0f, 0f,
//            0f, 1f, 0f, 0f,
//            0f, 0f, 1f, 0f,
//            0f, 0f, 0f, 1f
//            );
//
//
//        public Matrix4x4 Identity
//        {
//            get{ return _identity; }
//        }
//
//
//        public bool IsIdentity
//            {
//                get
//                {
//                    return M11 == 1f && M22 == 1f && M33 == 1f && M44 == 1f && // Check diagonal element first for early out.
//                    M12 == 0f && M13 == 0f && M14 == 0f &&
//                    M21 == 0f && M23 == 0f && M24 == 0f &&
//                    M31 == 0f && M32 == 0f && M34 == 0f &&
//                    M41 == 0f && M42 == 0f && M43 == 0f;
//                }
//            }
//
//
//            public Vector3 Translation
//                {
//                    get
//                    {
//                        Vector3 ans;
//
//                        ans.X = M41;
//                        ans.Y = M42;
//                        ans.Z = M43;
//
//                        return ans;
//                    }
//
//                        set
//                    {
//                        M41 = value.X;
//                        M42 = value.Y;
//                        M43 = value.Z;
//                    }
//                }
//
//
//                public Matrix4x4(float m11, float m12, float m13, float m14,
//                float m21, float m22, float m23, float m24,
//                float m31, float m32, float m33, float m34,
//                float m41, float m42, float m43, float m44)
//                    {
//                        M11 = m11; M12 = m12; M13 = m13; M14 = m14;
//                        M21 = m21; M22 = m22; M23 = m23; M24 = m24;
//                        M31 = m31; M32 = m32; M33 = m33; M34 = m34;
//                        M41 = m41; M42 = m42; M43 = m43; M44 = m44;
//                    }
//
//
//                    public Matrix4x4(Matrix3x2 value)
//                    {
//                        M11 = value.M11; M12 = value.M12; M13 = 0f; M14 = 0f;
//                        M21 = value.M21; M22 = value.M22; M23 = 0f; M24 = 0f;
//                        M31 = 0f;        M32 = 0f;        M33 = 1f; M34 = 0f;
//                        M41 = value.M31; M42 = value.M32; M43 = 0f; M44 = 1f;
//                    }
//
//
//                    public Matrix4x4 CreateBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 cameraUpVector, Vector3 cameraForwardVector)
//                    {
//                        const float epsilon = 1e-4f;
//
//                        Vector3 zaxis;
//
//                        zaxis.X = objectPosition.X - cameraPosition.X;
//                        zaxis.Y = objectPosition.Y - cameraPosition.Y;
//                        zaxis.Z = objectPosition.Z - cameraPosition.Z;
//
//                        float norm = zaxis.LengthSquared();
//
//                        if (norm < epsilon)
//                        {
//                            zaxis = -cameraForwardVector;
//                        }
//                        else
//                        {
//                            zaxis = Vector3.Multiply(zaxis, 1.0f / (float)SM.Sqrt(norm));
//                        }
//
//                        Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));
//
//                        Vector3 yaxis = Vector3.Cross(zaxis, xaxis);
//
//                        Matrix4x4 result;
//
//                        result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
//                        result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
//                        result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;
//
//                        result.M41 = objectPosition.X;
//                        result.M42 = objectPosition.Y;
//                        result.M43 = objectPosition.Z;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateConstrainedBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 rotateAxis, Vector3 cameraForwardVector, Vector3 objectForwardVector)
//                    {
//                        const float epsilon = 1e-4f;
//                        const float minAngle = 1.0f - (0.1f * ((float)SM.PI / 180.0f)); // 0.1 degrees
//
//                        // Treat the case when object and camera positions are too close.
//                        Vector3 faceDir;
//
//                        faceDir.X = objectPosition.X - cameraPosition.X;
//                        faceDir.Y = objectPosition.Y - cameraPosition.Y;
//                        faceDir.Z = objectPosition.Z - cameraPosition.Z;
//
//                        float norm = faceDir.LengthSquared();
//
//                        if (norm < epsilon)
//                        {
//                            faceDir = -cameraForwardVector;
//                        }
//                        else
//                        {
//                            faceDir = Vector3.Multiply(faceDir, (1.0f / (float)SM.Sqrt(norm)));
//                        }
//
//                        Vector3 yaxis = rotateAxis;
//                        Vector3 xaxis;
//                        Vector3 zaxis;
//
//                        // Treat the case when angle between faceDir and rotateAxis is too close to 0.
//                        float dot = Vector3.Dot(rotateAxis, faceDir);
//
//                        if (SM.Abs(dot) > minAngle)
//                        {
//                            zaxis = objectForwardVector;
//
//                            // Make sure passed values are useful for compute.
//                            dot = Vector3.Dot(rotateAxis, zaxis);
//
//                            if (SM.Abs(dot) > minAngle)
//                            {
//                                zaxis = (SM.Abs(rotateAxis.Z) > minAngle) ? new Vector3(1, 0, 0) : new Vector3(0, 0, -1);
//                            }
//
//                            xaxis = Vector3.Normalize(Vector3.Cross(rotateAxis, zaxis));
//                            zaxis = Vector3.Normalize(Vector3.Cross(xaxis, rotateAxis));
//                        }
//                        else
//                        {
//                            xaxis = Vector3.Normalize(Vector3.Cross(rotateAxis, faceDir));
//                            zaxis = Vector3.Normalize(Vector3.Cross(xaxis, yaxis));
//                        }
//
//                        Matrix4x4 result;
//
//                        result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
//                        result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
//                        result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;
//
//                        result.M41 = objectPosition.X;
//                        result.M42 = objectPosition.Y;
//                        result.M43 = objectPosition.Z;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateTranslation(Vector3 position)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
//
//                        result.M41 = position.X;
//                        result.M42 = position.Y;
//                        result.M43 = position.Z;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateTranslation(float xPosition, float yPosition, float zPosition)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
//
//                        result.M41 = xPosition;
//                        result.M42 = yPosition;
//                        result.M43 = zPosition;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(float xScale, float yScale, float zScale)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
//                        result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
//                        result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
//                        result.M41 = 0.0f;   result.M42 = 0.0f;   result.M43 = 0.0f;   result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(float xScale, float yScale, float zScale, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float tx = centerPoint.X * (1 - xScale);
//                        float ty = centerPoint.Y * (1 - yScale);
//                        float tz = centerPoint.Z * (1 - zScale);
//
//                        result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
//                        result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
//                        result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
//                        result.M41 = tx;     result.M42 = ty;     result.M43 = tz;     result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(Vector3 scales)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
//                        result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
//                        result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
//                        result.M41 = 0.0f;     result.M42 = 0.0f;     result.M43 = 0.0f;     result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(Vector3 scales, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float tx = centerPoint.X * (1 - scales.X);
//                        float ty = centerPoint.Y * (1 - scales.Y);
//                        float tz = centerPoint.Z * (1 - scales.Z);
//
//                        result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
//                        result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
//                        result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
//                        result.M41 = tx;       result.M42 = ty;       result.M43 = tz;       result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(float scale)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
//                        result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
//                        result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
//                        result.M41 = 0.0f;  result.M42 = 0.0f;  result.M43 = 0.0f;  result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateScale(float scale, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float tx = centerPoint.X * (1 - scale);
//                        float ty = centerPoint.Y * (1 - scale);
//                        float tz = centerPoint.Z * (1 - scale);
//
//                        result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
//                        result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
//                        result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
//                        result.M41 = tx;    result.M42 = ty;    result.M43 = tz;    result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationX(float radians)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        // [  1  0  0  0 ]
//                        // [  0  c  s  0 ]
//                        // [  0 -s  c  0 ]
//                        // [  0  0  0  1 ]
//                        result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = c; result.M23 = s; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = -s; result.M33 = c; result.M34 = 0.0f;
//                        result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationX(float radians, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        float y = centerPoint.Y * (1 - c) + centerPoint.Z * s;
//                        float z = centerPoint.Z * (1 - c) - centerPoint.Y * s;
//
//                        // [  1  0  0  0 ]
//                        // [  0  c  s  0 ]
//                        // [  0 -s  c  0 ]
//                        // [  0  y  z  1 ]
//                        result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = c; result.M23 = s; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = -s; result.M33 = c; result.M34 = 0.0f;
//                        result.M41 = 0.0f; result.M42 = y; result.M43 = z; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationY(float radians)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        // [  c  0 -s  0 ]
//                        // [  0  1  0  0 ]
//                        // [  s  0  c  0 ]
//                        // [  0  0  0  1 ]
//                        result.M11 = c; result.M12 = 0.0f; result.M13 = -s; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = s; result.M32 = 0.0f; result.M33 = c; result.M34 = 0.0f;
//                        result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationY(float radians, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        float x = centerPoint.X * (1 - c) - centerPoint.Z * s;
//                        float z = centerPoint.Z * (1 - c) + centerPoint.X * s;
//
//                        // [  c  0 -s  0 ]
//                        // [  0  1  0  0 ]
//                        // [  s  0  c  0 ]
//                        // [  x  0  z  1 ]
//                        result.M11 = c; result.M12 = 0.0f; result.M13 = -s; result.M14 = 0.0f;
//                        result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = s; result.M32 = 0.0f; result.M33 = c; result.M34 = 0.0f;
//                        result.M41 = x; result.M42 = 0.0f; result.M43 = z; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationZ(float radians)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        // [  c  s  0  0 ]
//                        // [ -s  c  0  0 ]
//                        // [  0  0  1  0 ]
//                        // [  0  0  0  1 ]
//                        result.M11 = c; result.M12 = s; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = -s; result.M22 = c; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
//                        result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateRotationZ(float radians, Vector3 centerPoint)
//                    {
//                        Matrix4x4 result;
//
//                        float c = (float)SM.Cos(radians);
//                        float s = (float)SM.Sin(radians);
//
//                        float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
//                        float y = centerPoint.Y * (1 - c) - centerPoint.X * s;
//
//                        // [  c  s  0  0 ]
//                        // [ -s  c  0  0 ]
//                        // [  0  0  1  0 ]
//                        // [  x  y  0  1 ]
//                        result.M11 = c; result.M12 = s; result.M13 = 0.0f; result.M14 = 0.0f;
//                        result.M21 = -s; result.M22 = c; result.M23 = 0.0f; result.M24 = 0.0f;
//                        result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
//                        result.M41 = x; result.M42 = y; result.M43 = 0.0f; result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateFromAxisAngle(Vector3 axis, float angle)
//                    {
//                        // a: angle
//                        // x, y, z: unit vector for axis.
//                        //
//                        // Rotation matrix M can compute by using below equation.
//                        //
//                        //        T               T
//                        //  M = uu + (cos a)( I-uu ) + (sin a)S
//                        //
//                        // Where:
//                        //
//                        //  u = ( x, y, z )
//                        //
//                        //      [  0 -z  y ]
//                        //  S = [  z  0 -x ]
//                        //      [ -y  x  0 ]
//                        //
//                        //      [ 1 0 0 ]
//                        //  I = [ 0 1 0 ]
//                        //      [ 0 0 1 ]
//                        //
//                        //
//                        //     [  xx+cosa*(1-xx)   yx-cosa*yx-sina*z zx-cosa*xz+sina*y ]
//                        // M = [ xy-cosa*yx+sina*z    yy+cosa(1-yy)  yz-cosa*yz-sina*x ]
//                        //     [ zx-cosa*zx-sina*y zy-cosa*zy+sina*x   zz+cosa*(1-zz)  ]
//                        //
//                        float x = axis.X, y = axis.Y, z = axis.Z;
//                        float sa = (float)SM.Sin(angle), ca = (float)SM.Cos(angle);
//                        float xx = x * x, yy = y * y, zz = z * z;
//                        float xy = x * y, xz = x * z, yz = y * z;
//
//                        Matrix4x4 result;
//
//                        result.M11 = xx + ca * (1.0f - xx);   result.M12 = xy - ca * xy + sa * z;     result.M13 = xz - ca * xz - sa * y;     result.M14 = 0.0f;
//                        result.M21 = xy - ca * xy - sa * z;     result.M22 = yy + ca * (1.0f - yy);   result.M23 = yz - ca * yz + sa * x;     result.M24 = 0.0f;
//                        result.M31 = xz - ca * xz + sa * y;     result.M32 = yz - ca * yz - sa * x;     result.M33 = zz + ca * (1.0f - zz);   result.M34 = 0.0f;
//                        result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
//                    {
//                        if (fieldOfView <= 0.0f || fieldOfView >= SM.PI)
//                            throw new ArgumentOutOfRangeException("fieldOfView");
//
//                        if (nearPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        if (farPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("farPlaneDistance");
//
//                        if (nearPlaneDistance >= farPlaneDistance)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        float yScale = 1.0f / (float)SM.Tan(fieldOfView * 0.5f);
//                        float xScale = yScale / aspectRatio;
//
//                        Matrix4x4 result;
//
//                        result.M11 = xScale;
//                        result.M12 = result.M13 = result.M14 = 0.0f;
//
//                        result.M22 = yScale;
//                        result.M21 = result.M23 = result.M24 = 0.0f;
//
//                        result.M31 = result.M32 = 0.0f;
//                        result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//                        result.M34 = -1.0f;
//
//                        result.M41 = result.M42 = result.M44 = 0.0f;
//                        result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
//                    {
//                        if (nearPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        if (farPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("farPlaneDistance");
//
//                        if (nearPlaneDistance >= farPlaneDistance)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        Matrix4x4 result;
//
//                        result.M11 = 2.0f * nearPlaneDistance / width;
//                        result.M12 = result.M13 = result.M14 = 0.0f;
//
//                        result.M22 = 2.0f * nearPlaneDistance / height;
//                        result.M21 = result.M23 = result.M24 = 0.0f;
//
//                        result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//                        result.M31 = result.M32 = 0.0f;
//                        result.M34 = -1.0f;
//
//                        result.M41 = result.M42 = result.M44 = 0.0f;
//                        result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
//                    {
//                        if (nearPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        if (farPlaneDistance <= 0.0f)
//                            throw new ArgumentOutOfRangeException("farPlaneDistance");
//
//                        if (nearPlaneDistance >= farPlaneDistance)
//                            throw new ArgumentOutOfRangeException("nearPlaneDistance");
//
//                        Matrix4x4 result;
//
//                        result.M11 = 2.0f * nearPlaneDistance / (right - left);
//                        result.M12 = result.M13 = result.M14 = 0.0f;
//
//                        result.M22 = 2.0f * nearPlaneDistance / (top - bottom);
//                        result.M21 = result.M23 = result.M24 = 0.0f;
//
//                        result.M31 = (left + right) / (right - left);
//                        result.M32 = (top + bottom) / (top - bottom);
//                        result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//                        result.M34 = -1.0f;
//
//                        result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
//                        result.M41 = result.M42 = result.M44 = 0.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = 2.0f / width;
//                        result.M12 = result.M13 = result.M14 = 0.0f;
//
//                        result.M22 = 2.0f / height;
//                        result.M21 = result.M23 = result.M24 = 0.0f;
//
//                        result.M33 = 1.0f / (zNearPlane - zFarPlane);
//                        result.M31 = result.M32 = result.M34 = 0.0f;
//
//                        result.M41 = result.M42 = 0.0f;
//                        result.M43 = zNearPlane / (zNearPlane - zFarPlane);
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = 2.0f / (right - left);
//                        result.M12 = result.M13 = result.M14 = 0.0f;
//
//                        result.M22 = 2.0f / (top - bottom);
//                        result.M21 = result.M23 = result.M24 = 0.0f;
//
//                        result.M33 = 1.0f / (zNearPlane - zFarPlane);
//                        result.M31 = result.M32 = result.M34 = 0.0f;
//
//                        result.M41 = (left + right) / (left - right);
//                        result.M42 = (top + bottom) / (bottom - top);
//                        result.M43 = zNearPlane / (zNearPlane - zFarPlane);
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateLookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
//                    {
//                        Vector3 zaxis = Vector3.Normalize(cameraPosition - cameraTarget);
//                        Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));
//                        Vector3 yaxis = Vector3.Cross(zaxis, xaxis);
//
//                        Matrix4x4 result;
//
//                        result.M11 = xaxis.X; result.M12 = yaxis.X; result.M13 = zaxis.X; result.M14 = 0.0f;
//                        result.M21 = xaxis.Y; result.M22 = yaxis.Y; result.M23 = zaxis.Y; result.M24 = 0.0f;
//                        result.M31 = xaxis.Z; result.M32 = yaxis.Z; result.M33 = zaxis.Z; result.M34 = 0.0f;
//                        result.M41 = -Vector3.Dot(xaxis, cameraPosition);
//                        result.M42 = -Vector3.Dot(yaxis, cameraPosition);
//                        result.M43 = -Vector3.Dot(zaxis, cameraPosition);
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
//                    {
//                        Vector3 zaxis = Vector3.Normalize(-forward);
//                        Vector3 xaxis = Vector3.Normalize(Vector3.Cross(up, zaxis));
//                        Vector3 yaxis = Vector3.Cross(zaxis, xaxis);
//
//                        Matrix4x4 result;
//
//                        result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
//                        result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
//                        result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;
//                        result.M41 = position.X;
//                        result.M42 = position.Y;
//                        result.M43 = position.Z;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateFromQuaternion(Quaternion quaternion)
//                    {
//                        Matrix4x4 result;
//
//                        float xx = quaternion.X * quaternion.X;
//                        float yy = quaternion.Y * quaternion.Y;
//                        float zz = quaternion.Z * quaternion.Z;
//
//                        float xy = quaternion.X * quaternion.Y;
//                        float wz = quaternion.Z * quaternion.W;
//                        float xz = quaternion.Z * quaternion.X;
//                        float wy = quaternion.Y * quaternion.W;
//                        float yz = quaternion.Y * quaternion.Z;
//                        float wx = quaternion.X * quaternion.W;
//
//                        result.M11 = 1.0f - 2.0f * (yy + zz);   result.M12 = 2.0f * (xy + wz);          result.M13 = 2.0f * (xz - wy);          result.M14 = 0.0f;
//                        result.M21 = 2.0f * (xy - wz);          result.M22 = 1.0f - 2.0f * (zz + xx);   result.M23 = 2.0f * (yz + wx);          result.M24 = 0.0f;
//                        result.M31 = 2.0f * (xz + wy);          result.M32 = 2.0f * (yz - wx);          result.M33 = 1.0f - 2.0f * (yy + xx);   result.M34 = 0.0f;
//                        result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
//                    {
//                        Quaternion q = Quaternion.CreateFromYawPitchRoll(yaw, pitch, roll);
//
//                        return Matrix4x4.CreateFromQuaternion(q);
//                    }
//
//
//                    public Matrix4x4 CreateShadow(Vector3 lightDirection, Plane plane)
//                    {
//                        Plane p = Plane.Normalize(plane);
//
//                        float dot = p.Normal.X * lightDirection.X + p.Normal.Y * lightDirection.Y + p.Normal.Z * lightDirection.Z;
//                        float a = -p.Normal.X;
//                        float b = -p.Normal.Y;
//                        float c = -p.Normal.Z;
//                        float d = -p.D;
//
//                        Matrix4x4 result;
//
//                        result.M11 = a * lightDirection.X + dot;
//                        result.M21 = b * lightDirection.X;
//                        result.M31 = c * lightDirection.X;
//                        result.M41 = d * lightDirection.X;
//
//                        result.M12 = a * lightDirection.Y;
//                        result.M22 = b * lightDirection.Y + dot;
//                        result.M32 = c * lightDirection.Y;
//                        result.M42 = d * lightDirection.Y;
//
//                        result.M13 = a * lightDirection.Z;
//                        result.M23 = b * lightDirection.Z;
//                        result.M33 = c * lightDirection.Z + dot;
//                        result.M43 = d * lightDirection.Z;
//
//                        result.M14 = 0.0f;
//                        result.M24 = 0.0f;
//                        result.M34 = 0.0f;
//                        result.M44 = dot;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 CreateReflection(Plane value)
//                    {
//                        value = Plane.Normalize(value);
//
//                        float a = value.Normal.X;
//                        float b = value.Normal.Y;
//                        float c = value.Normal.Z;
//
//                        float fa = -2.0f * a;
//                        float fb = -2.0f * b;
//                        float fc = -2.0f * c;
//
//                        Matrix4x4 result;
//
//                        result.M11 = fa * a + 1.0f;
//                        result.M12 = fb * a;
//                        result.M13 = fc * a;
//                        result.M14 = 0.0f;
//
//                        result.M21 = fa * b;
//                        result.M22 = fb * b + 1.0f;
//                        result.M23 = fc * b;
//                        result.M24 = 0.0f;
//
//                        result.M31 = fa * c;
//                        result.M32 = fb * c;
//                        result.M33 = fc * c + 1.0f;
//                        result.M34 = 0.0f;
//
//                        result.M41 = fa * value.D;
//                        result.M42 = fb * value.D;
//                        result.M43 = fc * value.D;
//                        result.M44 = 1.0f;
//
//                        return result;
//                    }
//
//
//                    public float Determinant()
//                    {
//                        // | a b c d |     | f g h |     | e g h |     | e f h |     | e f g |
//                        // | e f g h | = a | j k l | - b | i k l | + c | i j l | - d | i j k |
//                        // | i j k l |     | n o p |     | m o p |     | m n p |     | m n o |
//                        // | m n o p |
//                        //
//                        //   | f g h |
//                        // a | j k l | = a ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
//                        //   | n o p |
//                        //
//                        //   | e g h |     
//                        // b | i k l | = b ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
//                        //   | m o p |     
//                        //
//                        //   | e f h |
//                        // c | i j l | = c ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
//                        //   | m n p |
//                        //
//                        //   | e f g |
//                        // d | i j k | = d ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
//                        //   | m n o |
//                        //
//                        // Cost of operation
//                        // 17 adds and 28 muls.
//                        //
//                        // add: 6 + 8 + 3 = 17
//                        // mul: 12 + 16 = 28
//
//                        float a = M11, b = M12, c = M13, d = M14;
//                        float e = M21, f = M22, g = M23, h = M24;
//                        float i = M31, j = M32, k = M33, l = M34;
//                        float m = M41, n = M42, o = M43, p = M44;
//
//                        float kp_lo = k * p - l * o;
//                        float jp_ln = j * p - l * n;
//                        float jo_kn = j * o - k * n;
//                        float ip_lm = i * p - l * m;
//                        float io_km = i * o - k * m;
//                        float in_jm = i * n - j * m;
//
//                        return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
//                            b * (e * kp_lo - g * ip_lm + h * io_km) +
//                            c * (e * jp_ln - f * ip_lm + h * in_jm) -
//                            d * (e * jo_kn - f * io_km + g * in_jm);
//                    }
//
//
//                    public bool Invert(Matrix4x4 matrix, out Matrix4x4 result)
//                    {
//                        //                                       -1
//                        // If you have matrix M, inverse Matrix M   can compute
//                        //
//                        //     -1       1      
//                        //    M   = --------- A
//                        //            det(M)
//                        //
//                        // A is adjugate (adjoint) of M, where,
//                        //
//                        //      T
//                        // A = C
//                        //
//                        // C is Cofactor matrix of M, where,
//                        //           i + j
//                        // C   = (-1)      * det(M  )
//                        //  ij                    ij
//                        //
//                        //     [ a b c d ]
//                        // M = [ e f g h ]
//                        //     [ i j k l ]
//                        //     [ m n o p ]
//                        //
//                        // First Row
//                        //           2 | f g h |
//                        // C   = (-1)  | j k l | = + ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
//                        //  11         | n o p |
//                        //
//                        //           3 | e g h |
//                        // C   = (-1)  | i k l | = - ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
//                        //  12         | m o p |
//                        //
//                        //           4 | e f h |
//                        // C   = (-1)  | i j l | = + ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
//                        //  13         | m n p |
//                        //
//                        //           5 | e f g |
//                        // C   = (-1)  | i j k | = - ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
//                        //  14         | m n o |
//                        //
//                        // Second Row
//                        //           3 | b c d |
//                        // C   = (-1)  | j k l | = - ( b ( kp - lo ) - c ( jp - ln ) + d ( jo - kn ) )
//                        //  21         | n o p |
//                        //
//                        //           4 | a c d |
//                        // C   = (-1)  | i k l | = + ( a ( kp - lo ) - c ( ip - lm ) + d ( io - km ) )
//                        //  22         | m o p |
//                        //
//                        //           5 | a b d |
//                        // C   = (-1)  | i j l | = - ( a ( jp - ln ) - b ( ip - lm ) + d ( in - jm ) )
//                        //  23         | m n p |
//                        //
//                        //           6 | a b c |
//                        // C   = (-1)  | i j k | = + ( a ( jo - kn ) - b ( io - km ) + c ( in - jm ) )
//                        //  24         | m n o |
//                        //
//                        // Third Row
//                        //           4 | b c d |
//                        // C   = (-1)  | f g h | = + ( b ( gp - ho ) - c ( fp - hn ) + d ( fo - gn ) )
//                        //  31         | n o p |
//                        //
//                        //           5 | a c d |
//                        // C   = (-1)  | e g h | = - ( a ( gp - ho ) - c ( ep - hm ) + d ( eo - gm ) )
//                        //  32         | m o p |
//                        //
//                        //           6 | a b d |
//                        // C   = (-1)  | e f h | = + ( a ( fp - hn ) - b ( ep - hm ) + d ( en - fm ) )
//                        //  33         | m n p |
//                        //
//                        //           7 | a b c |
//                        // C   = (-1)  | e f g | = - ( a ( fo - gn ) - b ( eo - gm ) + c ( en - fm ) )
//                        //  34         | m n o |
//                        //
//                        // Fourth Row
//                        //           5 | b c d |
//                        // C   = (-1)  | f g h | = - ( b ( gl - hk ) - c ( fl - hj ) + d ( fk - gj ) )
//                        //  41         | j k l |
//                        //
//                        //           6 | a c d |
//                        // C   = (-1)  | e g h | = + ( a ( gl - hk ) - c ( el - hi ) + d ( ek - gi ) )
//                        //  42         | i k l |
//                        //
//                        //           7 | a b d |
//                        // C   = (-1)  | e f h | = - ( a ( fl - hj ) - b ( el - hi ) + d ( ej - fi ) )
//                        //  43         | i j l |
//                        //
//                        //           8 | a b c |
//                        // C   = (-1)  | e f g | = + ( a ( fk - gj ) - b ( ek - gi ) + c ( ej - fi ) )
//                        //  44         | i j k |
//                        //
//                        // Cost of operation
//                        // 53 adds, 104 muls, and 1 div.
//                        float a = matrix.M11, b = matrix.M12, c = matrix.M13, d = matrix.M14;
//                        float e = matrix.M21, f = matrix.M22, g = matrix.M23, h = matrix.M24;
//                        float i = matrix.M31, j = matrix.M32, k = matrix.M33, l = matrix.M34;
//                        float m = matrix.M41, n = matrix.M42, o = matrix.M43, p = matrix.M44;
//
//                        float kp_lo = k * p - l * o;
//                        float jp_ln = j * p - l * n;
//                        float jo_kn = j * o - k * n;
//                        float ip_lm = i * p - l * m;
//                        float io_km = i * o - k * m;
//                        float in_jm = i * n - j * m;
//
//                        float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
//                        float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
//                        float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
//                        float a14 = -(e * jo_kn - f * io_km + g * in_jm);
//
//                        float det = a * a11 + b * a12 + c * a13 + d * a14;
//
//                        if (SM.Abs(det) < float.Epsilon)
//                        {
//                            result = new Matrix4x4(float.NaN, float.NaN, float.NaN, float.NaN,
//                                float.NaN, float.NaN, float.NaN, float.NaN,
//                                float.NaN, float.NaN, float.NaN, float.NaN,
//                                float.NaN, float.NaN, float.NaN, float.NaN);
//                            return false;
//                        }
//
//                        float invDet = 1.0f / det;
//
//                        result.M11 = a11 * invDet;
//                        result.M21 = a12 * invDet;
//                        result.M31 = a13 * invDet;
//                        result.M41 = a14 * invDet;
//
//                        result.M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
//                        result.M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
//                        result.M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
//                        result.M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;
//
//                        float gp_ho = g * p - h * o;
//                        float fp_hn = f * p - h * n;
//                        float fo_gn = f * o - g * n;
//                        float ep_hm = e * p - h * m;
//                        float eo_gm = e * o - g * m;
//                        float en_fm = e * n - f * m;
//
//                        result.M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
//                        result.M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
//                        result.M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
//                        result.M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;
//
//                        float gl_hk = g * l - h * k;
//                        float fl_hj = f * l - h * j;
//                        float fk_gj = f * k - g * j;
//                        float el_hi = e * l - h * i;
//                        float ek_gi = e * k - g * i;
//                        float ej_fi = e * j - f * i;
//
//                        result.M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
//                        result.M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
//                        result.M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
//                        result.M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;
//
//                        return true;
//                    }
//
//
//                    struct CanonicalBasis
//                    {
//                        public Vector3 Row0;
//                        public Vector3 Row1;
//                        public Vector3 Row2;
//                    };
//
//
//                    struct VectorBasis
//                    {
//                        public unsafe Vector3* Element0;
//                        public unsafe Vector3* Element1;
//                        public unsafe Vector3* Element2;
//                    }
//
//
//                    public bool Decompose(Matrix4x4 matrix, out Vector3 scale, out Quaternion rotation, out Vector3 translation)
//                    {
//                        bool result = true;
//
//                        unsafe
//                        {
//                            fixed(float* pfScales = &scale.X)
//                            {
//                                const float EPSILON = 0.0001f;
//                                float det;
//
//                                VectorBasis vectorBasis;
//                                Vector3** pVectorBasis = (Vector3**)&vectorBasis;
//
//                                Matrix4x4 matTemp = Matrix4x4.Identity;
//                                CanonicalBasis canonicalBasis = new CanonicalBasis();
//                                Vector3* pCanonicalBasis = &canonicalBasis.Row0;
//
//                                canonicalBasis.Row0 = new Vector3(1.0f, 0.0f, 0.0f);
//                                canonicalBasis.Row1 = new Vector3(0.0f, 1.0f, 0.0f);
//                                canonicalBasis.Row2 = new Vector3(0.0f, 0.0f, 1.0f);
//
//                                translation.X = matrix.M41;
//                                translation.Y = matrix.M42;
//                                translation.Z = matrix.M43;
//
//                                pVectorBasis[0] = (Vector3*)&matTemp.M11;
//                                pVectorBasis[1] = (Vector3*)&matTemp.M21;
//                                pVectorBasis[2] = (Vector3*)&matTemp.M31;
//
//                                *(pVectorBasis[0]) = new Vector3(matrix.M11, matrix.M12, matrix.M13);
//                                *(pVectorBasis[1]) = new Vector3(matrix.M21, matrix.M22, matrix.M23);
//                                *(pVectorBasis[2]) = new Vector3(matrix.M31, matrix.M32, matrix.M33);
//
//                                scale.X = pVectorBasis[0]->Length();
//                                scale.Y = pVectorBasis[1]->Length();
//                                scale.Z = pVectorBasis[2]->Length();
//
//                                uint a, b, c;
//                                #region Ranking
//                                float x = pfScales[0], y = pfScales[1], z = pfScales[2];
//                                if (x < y)
//                                {
//                                    if (y < z)
//                                    {
//                                        a = 2;
//                                        b = 1;
//                                        c = 0;
//                                    }
//                                    else
//                                    {
//                                        a = 1;
//
//                                        if (x < z)
//                                        {
//                                            b = 2;
//                                            c = 0;
//                                        }
//                                        else
//                                        {
//                                            b = 0;
//                                            c = 2;
//                                        }
//                                    }
//                                }
//                                else
//                                {
//                                    if (x < z)
//                                    {
//                                        a = 2;
//                                        b = 0;
//                                        c = 1;
//                                    }
//                                    else
//                                    {
//                                        a = 0;
//
//                                        if (y < z)
//                                        {
//                                            b = 2;
//                                            c = 1;
//                                        }
//                                        else
//                                        {
//                                            b = 1;
//                                            c = 2;
//                                        }
//                                    }
//                                }
//                                #endregion
//
//                                if (pfScales[a] < EPSILON)
//                                {
//                                    *(pVectorBasis[a]) = pCanonicalBasis[a];
//                                }
//
//                                *pVectorBasis[a] = Vector3.Normalize(*pVectorBasis[a]);
//
//                                if (pfScales[b] < EPSILON)
//                                {
//                                    uint cc;
//                                    float fAbsX, fAbsY, fAbsZ;
//
//                                    fAbsX = (float)SM.Abs(pVectorBasis[a]->X);
//                                    fAbsY = (float)SM.Abs(pVectorBasis[a]->Y);
//                                    fAbsZ = (float)SM.Abs(pVectorBasis[a]->Z);
//
//                                    #region Ranking
//                                    if (fAbsX < fAbsY)
//                                    {
//                                        if (fAbsY < fAbsZ)
//                                        {
//                                            cc = 0;
//                                        }
//                                        else
//                                        {
//                                            if (fAbsX < fAbsZ)
//                                            {
//                                                cc = 0;
//                                            }
//                                            else
//                                            {
//                                                cc = 2;
//                                            }
//                                        }
//                                    }
//                                    else
//                                    {
//                                        if (fAbsX < fAbsZ)
//                                        {
//                                            cc = 1;
//                                        }
//                                        else
//                                        {
//                                            if (fAbsY < fAbsZ)
//                                            {
//                                                cc = 1;
//                                            }
//                                            else
//                                            {
//                                                cc = 2;
//                                            }
//                                        }
//                                    }
//                                    #endregion
//
//                                    *(pCanonicalBasis + cc) = Vector3.Cross(*pVectorBasis[b], *pVectorBasis[a]);
//                                }
//
//                                *pVectorBasis[b] = Vector3.Normalize(*pVectorBasis[b]);
//
//                                if (pfScales[c] < EPSILON)
//                                {
//                                    *pVectorBasis[b] = Vector3.Cross(*pVectorBasis[c], *pVectorBasis[a]);
//                                }
//
//                                *pVectorBasis[c] = Vector3.Normalize(*pVectorBasis[c]);
//
//                                det = matTemp.Determinant();
//
//                                // use Kramer's rule to check for handedness of coordinate system
//                                if (det < 0.0f)
//                                {
//                                    // switch coordinate system by negating the scale and inverting the basis vector on the x-axis
//                                    pfScales[a] = -pfScales[a];
//                                    *pVectorBasis[a] = -(*pVectorBasis[a]);
//
//                                    det = -det;
//                                }
//
//                                det -= 1.0f;
//                                det *= det;
//
//                                if ((EPSILON < det))
//                                {
//                                    // Non-SRT matrix encountered
//                                    rotation = Quaternion.Identity;
//                                    result = false;
//                                }
//                                else
//                                {
//                                    // generate the quaternion from the matrix
//                                    rotation = Quaternion.CreateFromRotationMatrix(matTemp);
//                                }
//                            }
//                        }
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Transform(Matrix4x4 value, Quaternion rotation)
//                    {
//                        // Compute rotation matrix.
//                        float x2 = rotation.X + rotation.X;
//                        float y2 = rotation.Y + rotation.Y;
//                        float z2 = rotation.Z + rotation.Z;
//
//                        float wx2 = rotation.W * x2;
//                        float wy2 = rotation.W * y2;
//                        float wz2 = rotation.W * z2;
//                        float xx2 = rotation.X * x2;
//                        float xy2 = rotation.X * y2;
//                        float xz2 = rotation.X * z2;
//                        float yy2 = rotation.Y * y2;
//                        float yz2 = rotation.Y * z2;
//                        float zz2 = rotation.Z * z2;
//
//                        float q11 = 1.0f - yy2 - zz2;
//                        float q21 = xy2 - wz2;
//                        float q31 = xz2 + wy2;
//
//                        float q12 = xy2 + wz2;
//                        float q22 = 1.0f - xx2 - zz2;
//                        float q32 = yz2 - wx2;
//
//                        float q13 = xz2 - wy2;
//                        float q23 = yz2 + wx2;
//                        float q33 = 1.0f - xx2 - yy2;
//
//                        Matrix4x4 result;
//
//                        // First row
//                        result.M11 = value.M11 * q11 + value.M12 * q21 + value.M13 * q31;
//                        result.M12 = value.M11 * q12 + value.M12 * q22 + value.M13 * q32;
//                        result.M13 = value.M11 * q13 + value.M12 * q23 + value.M13 * q33;
//                        result.M14 = value.M14;
//
//                        // Second row
//                        result.M21 = value.M21 * q11 + value.M22 * q21 + value.M23 * q31;
//                        result.M22 = value.M21 * q12 + value.M22 * q22 + value.M23 * q32;
//                        result.M23 = value.M21 * q13 + value.M22 * q23 + value.M23 * q33;
//                        result.M24 = value.M24;
//
//                        // Third row
//                        result.M31 = value.M31 * q11 + value.M32 * q21 + value.M33 * q31;
//                        result.M32 = value.M31 * q12 + value.M32 * q22 + value.M33 * q32;
//                        result.M33 = value.M31 * q13 + value.M32 * q23 + value.M33 * q33;
//                        result.M34 = value.M34;
//
//                        // Fourth row
//                        result.M41 = value.M41 * q11 + value.M42 * q21 + value.M43 * q31;
//                        result.M42 = value.M41 * q12 + value.M42 * q22 + value.M43 * q32;
//                        result.M43 = value.M41 * q13 + value.M42 * q23 + value.M43 * q33;
//                        result.M44 = value.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Transpose(Matrix4x4 matrix)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = matrix.M11; result.M12 = matrix.M21; result.M13 = matrix.M31; result.M14 = matrix.M41;
//                        result.M21 = matrix.M12; result.M22 = matrix.M22; result.M23 = matrix.M32; result.M24 = matrix.M42;
//                        result.M31 = matrix.M13; result.M32 = matrix.M23; result.M33 = matrix.M33; result.M34 = matrix.M43;
//                        result.M41 = matrix.M14; result.M42 = matrix.M24; result.M43 = matrix.M34; result.M44 = matrix.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Lerp(Matrix4x4 matrix1, Matrix4x4 matrix2, float amount)
//                    {
//                        Matrix4x4 result;
//
//                        // First row
//                        result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
//                        result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
//                        result.M13 = matrix1.M13 + (matrix2.M13 - matrix1.M13) * amount;
//                        result.M14 = matrix1.M14 + (matrix2.M14 - matrix1.M14) * amount;
//
//                        // Second row
//                        result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
//                        result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
//                        result.M23 = matrix1.M23 + (matrix2.M23 - matrix1.M23) * amount;
//                        result.M24 = matrix1.M24 + (matrix2.M24 - matrix1.M24) * amount;
//
//                        // Third row
//                        result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
//                        result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;
//                        result.M33 = matrix1.M33 + (matrix2.M33 - matrix1.M33) * amount;
//                        result.M34 = matrix1.M34 + (matrix2.M34 - matrix1.M34) * amount;
//
//                        // Fourth row
//                        result.M41 = matrix1.M41 + (matrix2.M41 - matrix1.M41) * amount;
//                        result.M42 = matrix1.M42 + (matrix2.M42 - matrix1.M42) * amount;
//                        result.M43 = matrix1.M43 + (matrix2.M43 - matrix1.M43) * amount;
//                        result.M44 = matrix1.M44 + (matrix2.M44 - matrix1.M44) * amount;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Negate(Matrix4x4 matrix)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = -matrix.M11; result.M12 = -matrix.M12; result.M13 = -matrix.M13; result.M14 = -matrix.M14;
//                        result.M21 = -matrix.M21; result.M22 = -matrix.M22; result.M23 = -matrix.M23; result.M24 = -matrix.M24;
//                        result.M31 = -matrix.M31; result.M32 = -matrix.M32; result.M33 = -matrix.M33; result.M34 = -matrix.M34;
//                        result.M41 = -matrix.M41; result.M42 = -matrix.M42; result.M43 = -matrix.M43; result.M44 = -matrix.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = matrix1.M11 + matrix2.M11; result.M12 = matrix1.M12 + matrix2.M12; result.M13 = matrix1.M13 + matrix2.M13; result.M14 = matrix1.M14 + matrix2.M14;
//                        result.M21 = matrix1.M21 + matrix2.M21; result.M22 = matrix1.M22 + matrix2.M22; result.M23 = matrix1.M23 + matrix2.M23; result.M24 = matrix1.M24 + matrix2.M24;
//                        result.M31 = matrix1.M31 + matrix2.M31; result.M32 = matrix1.M32 + matrix2.M32; result.M33 = matrix1.M33 + matrix2.M33; result.M34 = matrix1.M34 + matrix2.M34;
//                        result.M41 = matrix1.M41 + matrix2.M41; result.M42 = matrix1.M42 + matrix2.M42; result.M43 = matrix1.M43 + matrix2.M43; result.M44 = matrix1.M44 + matrix2.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 result;
//
//                        result.M11 = matrix1.M11 - matrix2.M11; result.M12 = matrix1.M12 - matrix2.M12; result.M13 = matrix1.M13 - matrix2.M13; result.M14 = matrix1.M14 - matrix2.M14;
//                        result.M21 = matrix1.M21 - matrix2.M21; result.M22 = matrix1.M22 - matrix2.M22; result.M23 = matrix1.M23 - matrix2.M23; result.M24 = matrix1.M24 - matrix2.M24;
//                        result.M31 = matrix1.M31 - matrix2.M31; result.M32 = matrix1.M32 - matrix2.M32; result.M33 = matrix1.M33 - matrix2.M33; result.M34 = matrix1.M34 - matrix2.M34;
//                        result.M41 = matrix1.M41 - matrix2.M41; result.M42 = matrix1.M42 - matrix2.M42; result.M43 = matrix1.M43 - matrix2.M43; result.M44 = matrix1.M44 - matrix2.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 result;
//
//                        // First row
//                        result.M11 = matrix1.M11 * matrix2.M11 + matrix1.M12 * matrix2.M21 + matrix1.M13 * matrix2.M31 + matrix1.M14 * matrix2.M41;
//                        result.M12 = matrix1.M11 * matrix2.M12 + matrix1.M12 * matrix2.M22 + matrix1.M13 * matrix2.M32 + matrix1.M14 * matrix2.M42;
//                        result.M13 = matrix1.M11 * matrix2.M13 + matrix1.M12 * matrix2.M23 + matrix1.M13 * matrix2.M33 + matrix1.M14 * matrix2.M43;
//                        result.M14 = matrix1.M11 * matrix2.M14 + matrix1.M12 * matrix2.M24 + matrix1.M13 * matrix2.M34 + matrix1.M14 * matrix2.M44;
//
//                        // Second row
//                        result.M21 = matrix1.M21 * matrix2.M11 + matrix1.M22 * matrix2.M21 + matrix1.M23 * matrix2.M31 + matrix1.M24 * matrix2.M41;
//                        result.M22 = matrix1.M21 * matrix2.M12 + matrix1.M22 * matrix2.M22 + matrix1.M23 * matrix2.M32 + matrix1.M24 * matrix2.M42;
//                        result.M23 = matrix1.M21 * matrix2.M13 + matrix1.M22 * matrix2.M23 + matrix1.M23 * matrix2.M33 + matrix1.M24 * matrix2.M43;
//                        result.M24 = matrix1.M21 * matrix2.M14 + matrix1.M22 * matrix2.M24 + matrix1.M23 * matrix2.M34 + matrix1.M24 * matrix2.M44;
//
//                        // Third row
//                        result.M31 = matrix1.M31 * matrix2.M11 + matrix1.M32 * matrix2.M21 + matrix1.M33 * matrix2.M31 + matrix1.M34 * matrix2.M41;
//                        result.M32 = matrix1.M31 * matrix2.M12 + matrix1.M32 * matrix2.M22 + matrix1.M33 * matrix2.M32 + matrix1.M34 * matrix2.M42;
//                        result.M33 = matrix1.M31 * matrix2.M13 + matrix1.M32 * matrix2.M23 + matrix1.M33 * matrix2.M33 + matrix1.M34 * matrix2.M43;
//                        result.M34 = matrix1.M31 * matrix2.M14 + matrix1.M32 * matrix2.M24 + matrix1.M33 * matrix2.M34 + matrix1.M34 * matrix2.M44;
//
//                        // Fourth row
//                        result.M41 = matrix1.M41 * matrix2.M11 + matrix1.M42 * matrix2.M21 + matrix1.M43 * matrix2.M31 + matrix1.M44 * matrix2.M41;
//                        result.M42 = matrix1.M41 * matrix2.M12 + matrix1.M42 * matrix2.M22 + matrix1.M43 * matrix2.M32 + matrix1.M44 * matrix2.M42;
//                        result.M43 = matrix1.M41 * matrix2.M13 + matrix1.M42 * matrix2.M23 + matrix1.M43 * matrix2.M33 + matrix1.M44 * matrix2.M43;
//                        result.M44 = matrix1.M41 * matrix2.M14 + matrix1.M42 * matrix2.M24 + matrix1.M43 * matrix2.M34 + matrix1.M44 * matrix2.M44;
//
//                        return result;
//                    }
//
//
//                    public Matrix4x4 operator -(Matrix4x4 matrix1)
//                    {
//                        Matrix4x4 m;
//
//                        m.M11 = -matrix1.M11; m.M12 = -matrix1.M12; m.M13 = -matrix1.M13; m.M14 = -matrix1.M14;
//                        m.M21 = -matrix1.M21; m.M22 = -matrix1.M22; m.M23 = -matrix1.M23; m.M24 = -matrix1.M24;
//                        m.M31 = -matrix1.M31; m.M32 = -matrix1.M32; m.M33 = -matrix1.M33; m.M34 = -matrix1.M34;
//                        m.M41 = -matrix1.M41; m.M42 = -matrix1.M42; m.M43 = -matrix1.M43; m.M44 = -matrix1.M44;
//
//                        return m;
//                    }
//
//
//                    public Matrix4x4 operator +(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 m;
//
//                        m.M11 = matrix1.M11 + matrix2.M11; m.M12 = matrix1.M12 + matrix2.M12; m.M13 = matrix1.M13 + matrix2.M13; m.M14 = matrix1.M14 + matrix2.M14;
//                        m.M21 = matrix1.M21 + matrix2.M21; m.M22 = matrix1.M22 + matrix2.M22; m.M23 = matrix1.M23 + matrix2.M23; m.M24 = matrix1.M24 + matrix2.M24;
//                        m.M31 = matrix1.M31 + matrix2.M31; m.M32 = matrix1.M32 + matrix2.M32; m.M33 = matrix1.M33 + matrix2.M33; m.M34 = matrix1.M34 + matrix2.M34;
//                        m.M41 = matrix1.M41 + matrix2.M41; m.M42 = matrix1.M42 + matrix2.M42; m.M43 = matrix1.M43 + matrix2.M43; m.M44 = matrix1.M44 + matrix2.M44;
//
//                        return m;
//                    }
//
//
//                    public Matrix4x4 operator -(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 m;
//
//                        m.M11 = matrix1.M11 - matrix2.M11; m.M12 = matrix1.M12 - matrix2.M12; m.M13 = matrix1.M13 - matrix2.M13; m.M14 = matrix1.M14 - matrix2.M14;
//                        m.M21 = matrix1.M21 - matrix2.M21; m.M22 = matrix1.M22 - matrix2.M22; m.M23 = matrix1.M23 - matrix2.M23; m.M24 = matrix1.M24 - matrix2.M24;
//                        m.M31 = matrix1.M31 - matrix2.M31; m.M32 = matrix1.M32 - matrix2.M32; m.M33 = matrix1.M33 - matrix2.M33; m.M34 = matrix1.M34 - matrix2.M34;
//                        m.M41 = matrix1.M41 - matrix2.M41; m.M42 = matrix1.M42 - matrix2.M42; m.M43 = matrix1.M43 - matrix2.M43; m.M44 = matrix1.M44 - matrix2.M44;
//
//                        return m;
//                    }
//
//
//                    public Matrix4x4 operator *(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        Matrix4x4 m;
//
//                        // First row
//                        m.M11 = matrix1.M11 * matrix2.M11 + matrix1.M12 * matrix2.M21 + matrix1.M13 * matrix2.M31 + matrix1.M14 * matrix2.M41;
//                        m.M12 = matrix1.M11 * matrix2.M12 + matrix1.M12 * matrix2.M22 + matrix1.M13 * matrix2.M32 + matrix1.M14 * matrix2.M42;
//                        m.M13 = matrix1.M11 * matrix2.M13 + matrix1.M12 * matrix2.M23 + matrix1.M13 * matrix2.M33 + matrix1.M14 * matrix2.M43;
//                        m.M14 = matrix1.M11 * matrix2.M14 + matrix1.M12 * matrix2.M24 + matrix1.M13 * matrix2.M34 + matrix1.M14 * matrix2.M44;
//
//                        // Second row
//                        m.M21 = matrix1.M21 * matrix2.M11 + matrix1.M22 * matrix2.M21 + matrix1.M23 * matrix2.M31 + matrix1.M24 * matrix2.M41;
//                        m.M22 = matrix1.M21 * matrix2.M12 + matrix1.M22 * matrix2.M22 + matrix1.M23 * matrix2.M32 + matrix1.M24 * matrix2.M42;
//                        m.M23 = matrix1.M21 * matrix2.M13 + matrix1.M22 * matrix2.M23 + matrix1.M23 * matrix2.M33 + matrix1.M24 * matrix2.M43;
//                        m.M24 = matrix1.M21 * matrix2.M14 + matrix1.M22 * matrix2.M24 + matrix1.M23 * matrix2.M34 + matrix1.M24 * matrix2.M44;
//
//                        // Third row
//                        m.M31 = matrix1.M31 * matrix2.M11 + matrix1.M32 * matrix2.M21 + matrix1.M33 * matrix2.M31 + matrix1.M34 * matrix2.M41;
//                        m.M32 = matrix1.M31 * matrix2.M12 + matrix1.M32 * matrix2.M22 + matrix1.M33 * matrix2.M32 + matrix1.M34 * matrix2.M42;
//                        m.M33 = matrix1.M31 * matrix2.M13 + matrix1.M32 * matrix2.M23 + matrix1.M33 * matrix2.M33 + matrix1.M34 * matrix2.M43;
//                        m.M34 = matrix1.M31 * matrix2.M14 + matrix1.M32 * matrix2.M24 + matrix1.M33 * matrix2.M34 + matrix1.M34 * matrix2.M44;
//
//                        // Fourth row
//                        m.M41 = matrix1.M41 * matrix2.M11 + matrix1.M42 * matrix2.M21 + matrix1.M43 * matrix2.M31 + matrix1.M44 * matrix2.M41;
//                        m.M42 = matrix1.M41 * matrix2.M12 + matrix1.M42 * matrix2.M22 + matrix1.M43 * matrix2.M32 + matrix1.M44 * matrix2.M42;
//                        m.M43 = matrix1.M41 * matrix2.M13 + matrix1.M42 * matrix2.M23 + matrix1.M43 * matrix2.M33 + matrix1.M44 * matrix2.M43;
//                        m.M44 = matrix1.M41 * matrix2.M14 + matrix1.M42 * matrix2.M24 + matrix1.M43 * matrix2.M34 + matrix1.M44 * matrix2.M44;
//
//                        return m;
//                    }
//
//
//                    public bool operator ==(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        return (matrix1.M11 == matrix2.M11 && matrix1.M22 == matrix2.M22 && matrix1.M33 == matrix2.M33 && matrix1.M44 == matrix2.M44 && // Check diagonal element first for early out.
//                            matrix1.M12 == matrix2.M12 && matrix1.M13 == matrix2.M13 && matrix1.M14 == matrix2.M14 &&
//                            matrix1.M21 == matrix2.M21                               && matrix1.M23 == matrix2.M23 && matrix1.M24 == matrix2.M24 &&
//                            matrix1.M31 == matrix2.M31 && matrix1.M32 == matrix2.M32                               && matrix1.M34 == matrix2.M34 &&
//                            matrix1.M41 == matrix2.M41 && matrix1.M42 == matrix2.M42 && matrix1.M43 == matrix2.M43);
//                    }
//
//
//                    public bool operator !=(Matrix4x4 matrix1, Matrix4x4 matrix2)
//                    {
//                        return (matrix1.M11 != matrix2.M11 || matrix1.M12 != matrix2.M12 || matrix1.M13 != matrix2.M13 || matrix1.M14 != matrix2.M14 ||
//                            matrix1.M21 != matrix2.M21 || matrix1.M22 != matrix2.M22 || matrix1.M23 != matrix2.M23 || matrix1.M24 != matrix2.M24 ||
//                            matrix1.M31 != matrix2.M31 || matrix1.M32 != matrix2.M32 || matrix1.M33 != matrix2.M33 || matrix1.M34 != matrix2.M34 ||
//                            matrix1.M41 != matrix2.M41 || matrix1.M42 != matrix2.M42 || matrix1.M43 != matrix2.M43 || matrix1.M44 != matrix2.M44);
//                    }
//
//
//                    public bool Equals(Matrix4x4 other)
//                    {
//                        return (M11 == other.M11 && M22 == other.M22 && M33 == other.M33 && M44 == other.M44 && // Check diagonal element first for early out.
//                            M12 == other.M12 && M13 == other.M13 && M14 == other.M14 &&
//                            M21 == other.M21                     && M23 == other.M23 && M24 == other.M24 &&
//                            M31 == other.M31 && M32 == other.M32                     && M34 == other.M34 &&
//                            M41 == other.M41 && M42 == other.M42 && M43 == other.M43);
//                    }
//
//
//                    public override bool Equals(object obj)
//                    {
//                        if (obj is Matrix4x4)
//                        {
//                            return Equals((Matrix4x4)obj);
//                        }
//
//                        return false;
//                    }
//
//
//                    public override string ToString()
//                    {
//                        CultureInfo ci = CultureInfo.CurrentCulture;
//
//                        return String.Format(ci, "{{ {{M11:{0} M12:{1} M13:{2} M14:{3}}} {{M21:{4} M22:{5} M23:{6} M24:{7}}} {{M31:{8} M32:{9} M33:{10} M34:{11}}} {{M41:{12} M42:{13} M43:{14} M44:{15}}} }}",
//                            M11.ToString(ci), M12.ToString(ci), M13.ToString(ci), M14.ToString(ci),
//                            M21.ToString(ci), M22.ToString(ci), M23.ToString(ci), M24.ToString(ci),
//                            M31.ToString(ci), M32.ToString(ci), M33.ToString(ci), M34.ToString(ci),
//                            M41.ToString(ci), M42.ToString(ci), M43.ToString(ci), M44.ToString(ci));
//                    }
//
//
//                    public override int GetHashCode()
//                    {
//                        return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode() +
//                            M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode() +
//                            M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode() +
//                            M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
//                    }
//    }
//}
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Plane : IEquatable < Plane >
//    {
//        public Vector3 Normal;
//        public float D;
//
//
//        public Plane(float a, float b, float c, float d)
//        {
//            Normal.X = a;
//            Normal.Y = b;
//            Normal.Z = c;
//            D = d;
//        }
//
//
//        public Plane(Vector3 normal, float d)
//        {
//            Normal = normal;
//            D = d;
//        }
//
//
//        public Plane(Vector4 value)
//        {
//            Normal.X = value.X;
//            Normal.Y = value.Y;
//            Normal.Z = value.Z;
//            D = value.W;
//        }
//
//
//        public Plane(Vector3 point1, Vector3 point2, Vector3 point3)
//        {
//            float ax = point2.X - point1.X;
//            float ay = point2.Y - point1.Y;
//            float az = point2.Z - point1.Z;
//
//            float bx = point3.X - point1.X;
//            float by = point3.Y - point1.Y;
//            float bz = point3.Z - point1.Z;
//
//            // N=Cross(a,b)
//            float nx = ay * bz - az * by;
//            float ny = az * bx - ax * bz;
//            float nz = ax * by - ay * bx;
//
//            // Normalize(N)
//            float ls = nx * nx + ny * ny + nz * nz;
//            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//            Normal.X = nx * invNorm;
//            Normal.Y = ny * invNorm;
//            Normal.Z = nz * invNorm;
//
//            // D = - Dot(N, point1)
//            D = -(Normal.X * point1.X + Normal.Y * point1.Y + Normal.Z * point1.Z);
//        }
//
//
//        public Plane Normalize(Plane value)
//        {
//            const float FLT_EPSILON = 1.192092896e-07f; // smallest such that 1.0+FLT_EPSILON != 1.0
//
//            Plane result;
//
//            float f = value.Normal.X * value.Normal.X + value.Normal.Y * value.Normal.Y + value.Normal.Z * value.Normal.Z;
//
//            if (SM.Abs(f - 1.0f) < FLT_EPSILON)
//            {
//                result.Normal = value.Normal;
//                result.D = value.D;
//                return result; // It already normalized, so we don't need to farther process.
//            }
//
//            float fInv = 1.0f / (float)SM.Sqrt(f);
//
//            result.Normal.X = value.Normal.X * fInv;
//            result.Normal.Y = value.Normal.Y * fInv;
//            result.Normal.Z = value.Normal.Z * fInv;
//
//            result.D = value.D * fInv;
//
//            return result;
//        }
//
//
//        public Plane Transform(Plane plane, Matrix4x4 matrix)
//        {
//            Matrix4x4 m;
//            Matrix4x4.Invert(matrix, out m);
//
//            Plane result;
//
//            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z, w = plane.D;
//
//            result.Normal.X = x * m.M11 + y * m.M12 + z * m.M13 + w * m.M14;
//            result.Normal.Y = x * m.M21 + y * m.M22 + z * m.M23 + w * m.M24;
//            result.Normal.Z = x * m.M31 + y * m.M32 + z * m.M33 + w * m.M34;
//
//            result.D = x * m.M41 + y * m.M42 + z * m.M43 + w * m.M44;
//
//            return result;
//        }
//
//
//        public Plane Transform(Plane plane, Quaternion rotation)
//        {
//            // Compute rotation matrix.
//            float x2 = rotation.X + rotation.X;
//            float y2 = rotation.Y + rotation.Y;
//            float z2 = rotation.Z + rotation.Z;
//
//            float wx2 = rotation.W * x2;
//            float wy2 = rotation.W * y2;
//            float wz2 = rotation.W * z2;
//            float xx2 = rotation.X * x2;
//            float xy2 = rotation.X * y2;
//            float xz2 = rotation.X * z2;
//            float yy2 = rotation.Y * y2;
//            float yz2 = rotation.Y * z2;
//            float zz2 = rotation.Z * z2;
//
//            float m11 = 1.0f - yy2 - zz2;
//            float m21 = xy2 - wz2;
//            float m31 = xz2 + wy2;
//
//            float m12 = xy2 + wz2;
//            float m22 = 1.0f - xx2 - zz2;
//            float m32 = yz2 - wx2;
//
//            float m13 = xz2 - wy2;
//            float m23 = yz2 + wx2;
//            float m33 = 1.0f - xx2 - yy2;
//
//            Plane result;
//
//            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z;
//
//            result.Normal.X = x * m11 + y * m21 + z * m31;
//            result.Normal.Y = x * m12 + y * m22 + z * m32;
//            result.Normal.Z = x * m13 + y * m23 + z * m33;
//
//            result.D = plane.D;
//
//            return result;
//        }
//
//
//        public float Dot(Plane plane, Vector4 value)
//        {
//            return plane.Normal.X * value.X +
//                plane.Normal.Y * value.Y +
//                plane.Normal.Z * value.Z +
//                plane.D * value.W;
//        }
//
//
//        public float DotCoordinate(Plane plane, Vector3 value)
//        {
//            return plane.Normal.X * value.X +
//                plane.Normal.Y * value.Y +
//                plane.Normal.Z * value.Z +
//                plane.D;
//        }
//
//
//        public float DotNormal(Plane plane, Vector3 value)
//        {
//            return plane.Normal.X * value.X +
//                plane.Normal.Y * value.Y +
//                plane.Normal.Z * value.Z;
//        }
//
//
//        public bool operator ==(Plane lhs, Plane rhs)
//        {
//            return lhs.Equals(rhs);
//        }
//
//
//        public bool operator !=(Plane lhs, Plane rhs)
//        {
//            return (lhs.Normal.X != rhs.Normal.X ||
//                lhs.Normal.Y != rhs.Normal.Y ||
//                lhs.Normal.Z != rhs.Normal.Z ||
//                lhs.D != rhs.D);
//        }
//
//
//        public bool Equals(Plane other)
//        {
//            return (Normal.X == other.Normal.X &&
//                Normal.Y == other.Normal.Y &&
//                Normal.Z == other.Normal.Z &&
//                D == other.D);
//        }
//
//
//        public override bool Equals(object obj)
//        {
//            if (obj is Plane)
//            {
//                return Equals((Plane)obj);
//            }
//
//            return false;
//        }
//
//
//        public override string ToString()
//        {
//            CultureInfo ci = CultureInfo.CurrentCulture;
//
//            return String.Format(ci, "{{Normal:{0} D:{1}}}", Normal.ToString(), D.ToString(ci));
//        }
//
//
//        public override int GetHashCode()
//        {
//            return Normal.GetHashCode() + D.GetHashCode();
//        }
//    }
//}
////
//// Copyright (c) Microsoft Corporation.  All rights reserved.
////
//
//using System;
//using System.Globalization;
//
//using SM = System.Math;
//
//namespace Windows.Math
//{
//    public struct Quaternion : IEquatable < Quaternion >
//    {
//        public float X;
//        public float Y;
//        public float Z;
//        public float W;
//
//
//        public Quaternion Identity
//        {
//            get{ return new Quaternion(0, 0, 0, 1); }
//        }
//
//
//        public bool IsIdentity
//            {
//                get{ return X == 0f && Y == 0f && Z == 0f && W == 1f; }
//            }
//
//
//            public Quaternion(float x, float y, float z, float w)
//                {
//                    X = x;
//                    Y = y;
//                    Z = z;
//                    W = w;
//                }
//
//
//                public Quaternion(Vector3 vectorPart, float scalarPart)
//                {
//                    X = vectorPart.X;
//                    Y = vectorPart.Y;
//                    Z = vectorPart.Z;
//                    W = scalarPart;
//                }
//
//
//                public float Length()
//                {
//                    float ls = X * X + Y * Y + Z * Z + W * W;
//
//                    return (float)SM.Sqrt((double)ls);
//                }
//
//
//                public float LengthSquared()
//                {
//                    return X * X + Y * Y + Z * Z + W * W;
//                }
//
//
//                public Quaternion Normalize(Quaternion quaternion)
//                {
//                    Quaternion ans;
//
//                    float ls = quaternion.X * quaternion.X + quaternion.Y * quaternion.Y + quaternion.Z * quaternion.Z + quaternion.W * quaternion.W;
//
//                    float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//                    ans.X = quaternion.X * invNorm;
//                    ans.Y = quaternion.Y * invNorm;
//                    ans.Z = quaternion.Z * invNorm;
//                    ans.W = quaternion.W * invNorm;
//
//                    return ans;
//                }
//
//
//                public Quaternion Conjugate(Quaternion value)
//                {
//                    Quaternion ans;
//
//                    ans.X = -value.X;
//                    ans.Y = -value.Y;
//                    ans.Z = -value.Z;
//                    ans.W = value.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion Inverse(Quaternion quaternion)
//                {
//                    //  -1   (       a              -v       )
//                    // q   = ( -------------   ------------- )
//                    //       (  a^2 + |v|^2  ,  a^2 + |v|^2  )
//
//                    Quaternion ans;
//
//                    float ls = quaternion.X * quaternion.X + quaternion.Y * quaternion.Y + quaternion.Z * quaternion.Z + quaternion.W * quaternion.W;
//                    float invNorm = 1.0f / ls;
//
//                    ans.X = -quaternion.X * invNorm;
//                    ans.Y = -quaternion.Y * invNorm;
//                    ans.Z = -quaternion.Z * invNorm;
//                    ans.W = quaternion.W * invNorm;
//
//                    return ans;
//                }
//
//
//                public Quaternion CreateFromAxisAngle(Vector3 axis, float angle)
//                {
//                    Quaternion ans;
//
//                    float halfAngle = angle * 0.5f;
//                    float s = (float)SM.Sin(halfAngle);
//                    float c = (float)SM.Cos(halfAngle);
//
//                    ans.X = axis.X * s;
//                    ans.Y = axis.Y * s;
//                    ans.Z = axis.Z * s;
//                    ans.W = c;
//
//                    return ans;
//                }
//
//
//                public Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
//                {
//                    //  Roll first, about axis the object is facing, then
//                    //  pitch upward, then yaw to face into the new heading
//                    float sr, cr, sp, cp, sy, cy;
//
//                    float halfRoll = roll * 0.5f;
//                    sr = (float)SM.Sin(halfRoll); cr = (float)SM.Cos(halfRoll);
//
//                    float halfPitch = pitch * 0.5f;
//                    sp = (float)SM.Sin(halfPitch); cp = (float)SM.Cos(halfPitch);
//
//                    float halfYaw = yaw * 0.5f;
//                    sy = (float)SM.Sin(halfYaw); cy = (float)SM.Cos(halfYaw);
//
//                    Quaternion result;
//
//                    result.X = cy * sp * cr + sy * cp * sr;
//                    result.Y = sy * cp * cr - cy * sp * sr;
//                    result.Z = cy * cp * sr - sy * sp * cr;
//                    result.W = cy * cp * cr + sy * sp * sr;
//
//                    return result;
//                }
//
//
//                public Quaternion CreateFromRotationMatrix(Matrix4x4 matrix)
//                {
//                    float trace = matrix.M11 + matrix.M22 + matrix.M33;
//
//                    Quaternion q = new Quaternion();
//
//                    if (trace > 0.0f)
//                    {
//                        float s = (float)SM.Sqrt(trace + 1.0f);
//                        q.W = s * 0.5f;
//                        s = 0.5f / s;
//                        q.X = (matrix.M23 - matrix.M32) * s;
//                        q.Y = (matrix.M31 - matrix.M13) * s;
//                        q.Z = (matrix.M12 - matrix.M21) * s;
//                    }
//                    else
//                    {
//                        if (matrix.M11 >= matrix.M22 && matrix.M11 >= matrix.M33)
//                        {
//                            float s = (float)SM.Sqrt(1.0f + matrix.M11 - matrix.M22 - matrix.M33);
//                            float invS = 0.5f / s;
//                            q.X = 0.5f * s;
//                            q.Y = (matrix.M12 + matrix.M21) * invS;
//                            q.Z = (matrix.M13 + matrix.M31) * invS;
//                            q.W = (matrix.M23 - matrix.M32) * invS;
//                        }
//                        else if (matrix.M22 > matrix.M33)
//                        {
//                            float s = (float)SM.Sqrt(1.0f + matrix.M22 - matrix.M11 - matrix.M33);
//                            float invS = 0.5f / s;
//                            q.X = (matrix.M21 + matrix.M12) * invS;
//                            q.Y = 0.5f * s;
//                            q.Z = (matrix.M32 + matrix.M23) * invS;
//                            q.W = (matrix.M31 - matrix.M13) * invS;
//                        }
//                        else
//                        {
//                            float s = (float)SM.Sqrt(1.0f + matrix.M33 - matrix.M11 - matrix.M22);
//                            float invS = 0.5f / s;
//                            q.X = (matrix.M31 + matrix.M13) * invS;
//                            q.Y = (matrix.M32 + matrix.M23) * invS;
//                            q.Z = 0.5f * s;
//                            q.W = (matrix.M12 - matrix.M21) * invS;
//                        }
//                    }
//
//                    return q;
//                }
//
//
//                public float Dot(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    return quaternion1.X * quaternion2.X +
//                        quaternion1.Y * quaternion2.Y +
//                        quaternion1.Z * quaternion2.Z +
//                        quaternion1.W * quaternion2.W;
//                }
//
//
//                public Quaternion Slerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
//                {
//                    const float epsilon = 1e-6f;
//
//                    float t = amount;
//
//                    float cosOmega = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
//                        quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;
//
//                    bool flip = false;
//
//                    if (cosOmega < 0.0f)
//                    {
//                        flip = true;
//                        cosOmega = -cosOmega;
//                    }
//
//                    float s1, s2;
//
//                    if (cosOmega > (1.0f - epsilon))
//                    {
//                        // Too close, do straight linear interpolation.
//                        s1 = 1.0f - t;
//                        s2 = (flip) ? -t : t;
//                    }
//                    else
//                    {
//                        float omega = (float)SM.Acos(cosOmega);
//                        float invSinOmega = (float)(1 / SM.Sin(omega));
//
//                        s1 = (float)SM.Sin((1.0f - t) * omega) * invSinOmega;
//                        s2 = (flip)
//                            ? (float)-SM.Sin(t * omega) * invSinOmega
//                            : (float)SM.Sin(t * omega) * invSinOmega;
//                    }
//
//                    Quaternion ans;
//
//                    ans.X = s1 * quaternion1.X + s2 * quaternion2.X;
//                    ans.Y = s1 * quaternion1.Y + s2 * quaternion2.Y;
//                    ans.Z = s1 * quaternion1.Z + s2 * quaternion2.Z;
//                    ans.W = s1 * quaternion1.W + s2 * quaternion2.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion Lerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
//                {
//                    float t = amount;
//                    float t1 = 1.0f - t;
//
//                    Quaternion r = new Quaternion();
//
//                    float dot = quaternion1.X * quaternion2.X + quaternion1.Y * quaternion2.Y +
//                        quaternion1.Z * quaternion2.Z + quaternion1.W * quaternion2.W;
//
//                    if (dot >= 0.0f)
//                    {
//                        r.X = t1 * quaternion1.X + t * quaternion2.X;
//                        r.Y = t1 * quaternion1.Y + t * quaternion2.Y;
//                        r.Z = t1 * quaternion1.Z + t * quaternion2.Z;
//                        r.W = t1 * quaternion1.W + t * quaternion2.W;
//                    }
//                    else
//                    {
//                        r.X = t1 * quaternion1.X - t * quaternion2.X;
//                        r.Y = t1 * quaternion1.Y - t * quaternion2.Y;
//                        r.Z = t1 * quaternion1.Z - t * quaternion2.Z;
//                        r.W = t1 * quaternion1.W - t * quaternion2.W;
//                    }
//
//                    // Normalize it.
//                    float ls = r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W;
//                    float invNorm = 1.0f / (float)SM.Sqrt((double)ls);
//
//                    r.X *= invNorm;
//                    r.Y *= invNorm;
//                    r.Z *= invNorm;
//                    r.W *= invNorm;
//
//                    return r;
//                }
//
//
//                public Quaternion Concatenate(Quaternion value1, Quaternion value2)
//                {
//                    Quaternion ans;
//
//                    // Concatenate rotation is actually q2 * q1 instead of q1 * q2.
//                    // So that's why value2 goes q1 and value1 goes q2.
//                    float q1x = value2.X;
//                    float q1y = value2.Y;
//                    float q1z = value2.Z;
//                    float q1w = value2.W;
//
//                    float q2x = value1.X;
//                    float q2y = value1.Y;
//                    float q2z = value1.Z;
//                    float q2w = value1.W;
//
//                    // cross(av, bv)
//                    float cx = q1y * q2z - q1z * q2y;
//                    float cy = q1z * q2x - q1x * q2z;
//                    float cz = q1x * q2y - q1y * q2x;
//
//                    float dot = q1x * q2x + q1y * q2y + q1z * q2z;
//
//                    ans.X = q1x * q2w + q2x * q1w + cx;
//                    ans.Y = q1y * q2w + q2y * q1w + cy;
//                    ans.Z = q1z * q2w + q2z * q1w + cz;
//                    ans.W = q1w * q2w - dot;
//
//                    return ans;
//                }
//
//
//                public Quaternion Negate(Quaternion quaternion)
//                {
//                    Quaternion ans;
//
//                    ans.X = -quaternion.X;
//                    ans.Y = -quaternion.Y;
//                    ans.Z = -quaternion.Z;
//                    ans.W = -quaternion.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion Add(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X + quaternion2.X;
//                    ans.Y = quaternion1.Y + quaternion2.Y;
//                    ans.Z = quaternion1.Z + quaternion2.Z;
//                    ans.W = quaternion1.W + quaternion2.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion Subtract(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X - quaternion2.X;
//                    ans.Y = quaternion1.Y - quaternion2.Y;
//                    ans.Z = quaternion1.Z - quaternion2.Z;
//                    ans.W = quaternion1.W - quaternion2.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion Multiply(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    float q1x = quaternion1.X;
//                    float q1y = quaternion1.Y;
//                    float q1z = quaternion1.Z;
//                    float q1w = quaternion1.W;
//
//                    float q2x = quaternion2.X;
//                    float q2y = quaternion2.Y;
//                    float q2z = quaternion2.Z;
//                    float q2w = quaternion2.W;
//
//                    // cross(av, bv)
//                    float cx = q1y * q2z - q1z * q2y;
//                    float cy = q1z * q2x - q1x * q2z;
//                    float cz = q1x * q2y - q1y * q2x;
//
//                    float dot = q1x * q2x + q1y * q2y + q1z * q2z;
//
//                    ans.X = q1x * q2w + q2x * q1w + cx;
//                    ans.Y = q1y * q2w + q2y * q1w + cy;
//                    ans.Z = q1z * q2w + q2z * q1w + cz;
//                    ans.W = q1w * q2w - dot;
//
//                    return ans;
//                }
//
//
//                public Quaternion Multiply(Quaternion quaternion1, float scaleFactor)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X * scaleFactor;
//                    ans.Y = quaternion1.Y * scaleFactor;
//                    ans.Z = quaternion1.Z * scaleFactor;
//                    ans.W = quaternion1.W * scaleFactor;
//
//                    return ans;
//                }
//
//
//                public Quaternion Divide(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    float q1x = quaternion1.X;
//                    float q1y = quaternion1.Y;
//                    float q1z = quaternion1.Z;
//                    float q1w = quaternion1.W;
//
//                    //-------------------------------------
//                    // Inverse part.
//                    float ls = quaternion2.X * quaternion2.X + quaternion2.Y * quaternion2.Y +
//                        quaternion2.Z * quaternion2.Z + quaternion2.W * quaternion2.W;
//                    float invNorm = 1.0f / ls;
//
//                    float q2x = -quaternion2.X * invNorm;
//                    float q2y = -quaternion2.Y * invNorm;
//                    float q2z = -quaternion2.Z * invNorm;
//                    float q2w = quaternion2.W * invNorm;
//
//                    //-------------------------------------
//                    // Multiply part.
//
//                    // cross(av, bv)
//                    float cx = q1y * q2z - q1z * q2y;
//                    float cy = q1z * q2x - q1x * q2z;
//                    float cz = q1x * q2y - q1y * q2x;
//
//                    float dot = q1x * q2x + q1y * q2y + q1z * q2z;
//
//                    ans.X = q1x * q2w + q2x * q1w + cx;
//                    ans.Y = q1y * q2w + q2y * q1w + cy;
//                    ans.Z = q1z * q2w + q2z * q1w + cz;
//                    ans.W = q1w * q2w - dot;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator -(Quaternion quaternion)
//                {
//                    Quaternion ans;
//
//                    ans.X = -quaternion.X;
//                    ans.Y = -quaternion.Y;
//                    ans.Z = -quaternion.Z;
//                    ans.W = -quaternion.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator +(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X + quaternion2.X;
//                    ans.Y = quaternion1.Y + quaternion2.Y;
//                    ans.Z = quaternion1.Z + quaternion2.Z;
//                    ans.W = quaternion1.W + quaternion2.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator -(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X - quaternion2.X;
//                    ans.Y = quaternion1.Y - quaternion2.Y;
//                    ans.Z = quaternion1.Z - quaternion2.Z;
//                    ans.W = quaternion1.W - quaternion2.W;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator *(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    float q1x = quaternion1.X;
//                    float q1y = quaternion1.Y;
//                    float q1z = quaternion1.Z;
//                    float q1w = quaternion1.W;
//
//                    float q2x = quaternion2.X;
//                    float q2y = quaternion2.Y;
//                    float q2z = quaternion2.Z;
//                    float q2w = quaternion2.W;
//
//                    // cross(av, bv)
//                    float cx = q1y * q2z - q1z * q2y;
//                    float cy = q1z * q2x - q1x * q2z;
//                    float cz = q1x * q2y - q1y * q2x;
//
//                    float dot = q1x * q2x + q1y * q2y + q1z * q2z;
//
//                    ans.X = q1x * q2w + q2x * q1w + cx;
//                    ans.Y = q1y * q2w + q2y * q1w + cy;
//                    ans.Z = q1z * q2w + q2z * q1w + cz;
//                    ans.W = q1w * q2w - dot;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator *(Quaternion quaternion1, float scaleFactor)
//                {
//                    Quaternion ans;
//
//                    ans.X = quaternion1.X * scaleFactor;
//                    ans.Y = quaternion1.Y * scaleFactor;
//                    ans.Z = quaternion1.Z * scaleFactor;
//                    ans.W = quaternion1.W * scaleFactor;
//
//                    return ans;
//                }
//
//
//                public Quaternion operator /(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    Quaternion ans;
//
//                    float q1x = quaternion1.X;
//                    float q1y = quaternion1.Y;
//                    float q1z = quaternion1.Z;
//                    float q1w = quaternion1.W;
//
//                    //-------------------------------------
//                    // Inverse part.
//                    float ls = quaternion2.X * quaternion2.X + quaternion2.Y * quaternion2.Y +
//                        quaternion2.Z * quaternion2.Z + quaternion2.W * quaternion2.W;
//                    float invNorm = 1.0f / ls;
//
//                    float q2x = -quaternion2.X * invNorm;
//                    float q2y = -quaternion2.Y * invNorm;
//                    float q2z = -quaternion2.Z * invNorm;
//                    float q2w = quaternion2.W * invNorm;
//
//                    //-------------------------------------
//                    // Multiply part.
//
//                    // cross(av, bv)
//                    float cx = q1y * q2z - q1z * q2y;
//                    float cy = q1z * q2x - q1x * q2z;
//                    float cz = q1x * q2y - q1y * q2x;
//
//                    float dot = q1x * q2x + q1y * q2y + q1z * q2z;
//
//                    ans.X = q1x * q2w + q2x * q1w + cx;
//                    ans.Y = q1y * q2w + q2y * q1w + cy;
//                    ans.Z = q1z * q2w + q2z * q1w + cz;
//                    ans.W = q1w * q2w - dot;
//
//                    return ans;
//                }
//
//
//                public bool operator ==(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    return (quaternion1.X == quaternion2.X &&
//                        quaternion1.Y == quaternion2.Y &&
//                        quaternion1.Z == quaternion2.Z &&
//                        quaternion1.W == quaternion2.W);
//                }
//
//
//                public bool operator !=(Quaternion quaternion1, Quaternion quaternion2)
//                {
//                    return (quaternion1.X != quaternion2.X ||
//                        quaternion1.Y != quaternion2.Y ||
//                        quaternion1.Z != quaternion2.Z ||
//                        quaternion1.W != quaternion2.W);
//                }
//
//
//                public bool Equals(Quaternion other)
//                {
//                    return (X == other.X &&
//                        Y == other.Y &&
//                        Z == other.Z &&
//                        W == other.W);
//                }
//
//
//                public override bool Equals(object obj)
//                {
//                    if (obj is Quaternion)
//                    {
//                        return Equals((Quaternion)obj);
//                    }
//
//                    return false;
//                }
//
//
//                public override string ToString()
//                {
//                    CultureInfo ci = CultureInfo.CurrentCulture;
//
//                    return String.Format(ci, "{{X:{0} Y:{1} Z:{2} W:{3}}}", X.ToString(ci), Y.ToString(ci), Z.ToString(ci), W.ToString(ci));
//                }
//
//
//                public override int GetHashCode()
//                {
//                    return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
//                }
//    }
//}
