// Proposed .NET projection of the WinRT vector and matrix API

namespace Windows.Math
{
    public struct Vector2
    {
        public float X;
        public float Y;

        public Vector2(float x, float y)
        public Vector2(float value)

        public static Vector2 Zero { get; }
        public static Vector2 One { get; }
        public static Vector2 UnitX { get; }
        public static Vector2 UnitY { get; }

        public static implicit operator Point(Vector2 value)
        public static implicit operator Size(Vector2 value)
        public static implicit operator Vector2(Point value)
        public static implicit operator Vector2(Size value)

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector2 value1, Vector2 value2)
        public static float DistanceSquared(Vector2 value1, Vector2 value2)
        public static float Dot(Vector2 value1, Vector2 value2)
        public static Vector2 Normalize(Vector2 value)
        public static Vector2 Reflect(Vector2 vector, Vector2 normal)
        public static Vector2 Min(Vector2 value1, Vector2 value2)
        public static Vector2 Max(Vector2 value1, Vector2 value2)
        public static Vector2 Clamp(Vector2 value1, Vector2 min, Vector2 max)
        public static Vector2 Lerp(Vector2 value1, Vector2 value2, float amount)
        public static Vector2 Transform(Vector2 position, Matrix3x2 matrix)
        public static Vector2 Transform(Vector2 position, Matrix4x4 matrix)
        public static Vector2 TransformNormal(Vector2 normal, Matrix3x2 matrix)
        public static Vector2 TransformNormal(Vector2 normal, Matrix4x4 matrix)
        public static Vector2 Transform(Vector2 value, Quaternion rotation)
        public static Vector2 Negate(Vector2 value)
        public static Vector2 Add(Vector2 value1, Vector2 value2)
        public static Vector2 Subtract(Vector2 value1, Vector2 value2)
        public static Vector2 Multiply(Vector2 value1, Vector2 value2)
        public static Vector2 Multiply(Vector2 value1, float value2)
        public static Vector2 Divide(Vector2 value1, Vector2 value2)
        public static Vector2 Divide(Vector2 value1, float value2)

        public static Vector2 operator -(Vector2 value)
        public static Vector2 operator +(Vector2 value1, Vector2 value2)
        public static Vector2 operator -(Vector2 value1, Vector2 value2)
        public static Vector2 operator *(Vector2 value1, Vector2 value2)
        public static Vector2 operator *(Vector2 value1, float value2)
        public static Vector2 operator *(float value1, Vector2 value2)
        public static Vector2 operator /(Vector2 value1, Vector2 value2)
        public static Vector2 operator /(Vector2 value1, float value2)
    }


    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public Vector3(float x, float y, float z)
        public Vector3(Vector2 value, float z)
        public Vector3(float value)

        public static Vector3 Zero { get; }
        public static Vector3 One { get; }
        public static Vector3 UnitX { get; }
        public static Vector3 UnitY { get; }
        public static Vector3 UnitZ { get; }

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector3 value1, Vector3 value2)
        public static float DistanceSquared(Vector3 value1, Vector3 value2)
        public static float Dot(Vector3 vector1, Vector3 vector2)
        public static Vector3 Normalize(Vector3 value)
        public static Vector3 Cross(Vector3 vector1, Vector3 vector2)
        public static Vector3 Reflect(Vector3 vector, Vector3 normal)
        public static Vector3 Min(Vector3 value1, Vector3 value2)
        public static Vector3 Max(Vector3 value1, Vector3 value2)
        public static Vector3 Clamp(Vector3 value1, Vector3 min, Vector3 max)
        public static Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
        public static Vector3 Transform(Vector3 position, Matrix4x4 matrix)
        public static Vector3 TransformNormal(Vector3 normal, Matrix4x4 matrix)
        public static Vector3 Transform(Vector3 value, Quaternion rotation)
        public static Vector3 Negate(Vector3 value)
        public static Vector3 Add(Vector3 value1, Vector3 value2)
        public static Vector3 Subtract(Vector3 value1, Vector3 value2)
        public static Vector3 Multiply(Vector3 value1, Vector3 value2)
        public static Vector3 Multiply(Vector3 value1, float value2)
        public static Vector3 Divide(Vector3 value1, Vector3 value2)
        public static Vector3 Divide(Vector3 value1, float value2)

        public static Vector3 operator -(Vector3 value)
        public static Vector3 operator +(Vector3 value1, Vector3 value2)
        public static Vector3 operator -(Vector3 value1, Vector3 value2)
        public static Vector3 operator *(Vector3 value1, Vector3 value2)
        public static Vector3 operator *(Vector3 value1, float value2)
        public static Vector3 operator *(float value1, Vector3 value2)
        public static Vector3 operator /(Vector3 value1, Vector3 value2)
        public static Vector3 operator /(Vector3 value1, float value2)
    }


    public struct Vector4
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public Vector4(float x, float y, float z, float w)
        public Vector4(Vector2 value, float z, float w)
        public Vector4(Vector3 value, float w)
        public Vector4(float value)

        public static Vector4 Zero { get; }
        public static Vector4 One { get; }
        public static Vector4 UnitX { get; }
        public static Vector4 UnitY { get; }
        public static Vector4 UnitZ { get; }
        public static Vector4 UnitW { get; }

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector4 value1, Vector4 value2)
        public static float DistanceSquared(Vector4 value1, Vector4 value2)
        public static float Dot(Vector4 vector1, Vector4 vector2)
        public static Vector4 Normalize(Vector4 vector)
        public static Vector4 Min(Vector4 value1, Vector4 value2)
        public static Vector4 Max(Vector4 value1, Vector4 value2)
        public static Vector4 Clamp(Vector4 value1, Vector4 min, Vector4 max)
        public static Vector4 Lerp(Vector4 value1, Vector4 value2, float amount)
        public static Vector4 Transform(Vector2 position, Matrix4x4 matrix)
        public static Vector4 Transform(Vector3 position, Matrix4x4 matrix)
        public static Vector4 Transform(Vector4 vector, Matrix4x4 matrix)
        public static Vector4 Transform(Vector2 value, Quaternion rotation)
        public static Vector4 Transform(Vector3 value, Quaternion rotation)
        public static Vector4 Transform(Vector4 value, Quaternion rotation)
        public static Vector4 Negate(Vector4 value)
        public static Vector4 Add(Vector4 value1, Vector4 value2)
        public static Vector4 Subtract(Vector4 value1, Vector4 value2)
        public static Vector4 Multiply(Vector4 value1, Vector4 value2)
        public static Vector4 Multiply(Vector4 value1, float value2)
        public static Vector4 Divide(Vector4 value1, Vector4 value2)
        public static Vector4 Divide(Vector4 value1, float value2)

        public static Vector4 operator -(Vector4 value)
        public static Vector4 operator +(Vector4 value1, Vector4 value2)
        public static Vector4 operator -(Vector4 value1, Vector4 value2)
        public static Vector4 operator *(Vector4 value1, Vector4 value2)
        public static Vector4 operator *(Vector4 value1, float value2)
        public static Vector4 operator *(float value1, Vector4 value2)
        public static Vector4 operator /(Vector4 value1, Vector4 value2)
        public static Vector4 operator /(Vector4 value1, float value2)
    }


    public struct Matrix4x4
    {
        public float M11;
        public float M12;
        public float M13;
        public float M14;
        public float M21;
        public float M22;
        public float M23;
        public float M24;
        public float M31;
        public float M32;
        public float M33;
        public float M34;
        public float M41;
        public float M42;
        public float M43;
        public float M44;

        public Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
        public Matrix4x4(Matrix3x2 value)

        public static Matrix4x4 Identity { get; }
        public bool IsIdentity { get; }
        public Vector3 Translation { get; set; }

        public float Determinant()
        public static Matrix4x4 CreateBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 cameraUpVector, Vector3 cameraForwardVector)
        public static Matrix4x4 CreateConstrainedBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 rotateAxis, Vector3 cameraForwardVector, Vector3 objectForwardVector)
        public static Matrix4x4 CreateTranslation(Vector3 position)
        public static Matrix4x4 CreateTranslation(float xPosition, float yPosition, float zPosition)
        public static Matrix4x4 CreateScale(float xScale, float yScale, float zScale)
        public static Matrix4x4 CreateScale(float xScale, float yScale, float zScale, Vector3 centerPoint)
        public static Matrix4x4 CreateScale(Vector3 scales)
        public static Matrix4x4 CreateScale(Vector3 scales, Vector3 centerPoint)
        public static Matrix4x4 CreateScale(float scale)
        public static Matrix4x4 CreateScale(float scale, Vector3 centerPoint)
        public static Matrix4x4 CreateRotationX(float radians)
        public static Matrix4x4 CreateRotationX(float radians, Vector3 centerPoint)
        public static Matrix4x4 CreateRotationY(float radians)
        public static Matrix4x4 CreateRotationY(float radians, Vector3 centerPoint)
        public static Matrix4x4 CreateRotationZ(float radians)
        public static Matrix4x4 CreateRotationZ(float radians, Vector3 centerPoint)
        public static Matrix4x4 CreateFromAxisAngle(Vector3 axis, float angle)
        public static Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix4x4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
        public static Matrix4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
        public static Matrix4x4 CreateLookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
        public static Matrix4x4 CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
        public static Matrix4x4 CreateFromQuaternion(Quaternion quaternion)
        public static Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        public static Matrix4x4 CreateShadow(Vector3 lightDirection, Plane plane)
        public static Matrix4x4 CreateReflection(Plane value)
        public static bool Invert(Matrix4x4 matrix, out Matrix4x4 result)
        public static bool Decompose(Matrix4x3 matrix, out Vector3 scale, out Quaternion rotation, out Vector3 translation)
        public static Matrix4x4 Transform(Matrix4x4 value, Quaternion rotation)
        public static Matrix4x4 Transpose(Matrix4x4 matrix)
        public static Matrix4x4 Lerp(Matrix4x4 matrix1, Matrix4x4 matrix2, float amount)
        public static Matrix4x4 Negate(Matrix4x4 value)
        public static Matrix4x4 Add(Matrix4x4 value1, Matrix4x4 value2)
        public static Matrix4x4 Subtract(Matrix4x4 value1, Matrix4x4 value2)
        public static Matrix4x4 Multiply(Matrix4x4 value1, Matrix4x4 value2)

        public static Matrix4x4 operator -(Matrix4x4 value)
        public static Matrix4x4 operator +(Matrix4x4 value1, Matrix4x4 value2)
        public static Matrix4x4 operator -(Matrix4x4 value1, Matrix4x4 value2)
        public static Matrix4x4 operator *(Matrix4x4 value1, Matrix4x4 value2)
    }


    public struct Matrix3x2
    {
        public float M11;
        public float M12;
        public float M21;
        public float M22;
        public float M31;
        public float M32;

        public Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)

        public static Matrix3x2 Identity { get; }
        public bool IsIdentity { get; }
        public Vector2 Translation { get; set; }

        public float Determinant()
        public static Matrix3x2 CreateTranslation(Vector2 position)
        public static Matrix3x2 CreateTranslation(float xPosition, float yPosition)
        public static Matrix3x2 CreateScale(float xScale, float yScale)
        public static Matrix3x2 CreateScale(float xScale, float yScale, Vector2 centerPoint)
        public static Matrix3x2 CreateScale(Vector2 scales)
        public static Matrix3x2 CreateScale(Vector2 scales, Vector2 centerPoint)
        public static Matrix3x2 CreateScale(float scale)
        public static Matrix3x2 CreateScale(float scale, Vector2 centerPoint)
        public static Matrix3x2 CreateSkew(float radiansX, float radiansY)
        public static Matrix3x2 CreateSkew(float radiansX, float radiansY, Vector2 centerPoint)
        public static Matrix3x2 CreateRotation(float radians)
        public static Matrix3x2 CreateRotation(float radians, Vector2 centerPoint)
        public static bool Invert(Matrix3x2 matrix, out Matrix3x2 result)
        public static Matrix3x2 Lerp(Matrix3x2 matrix1, Matrix3x2 matrix2, float amount)
        public static Matrix3x2 Negate(Matrix3x2 value)
        public static Matrix3x2 Add(Matrix3x2 value1, Matrix3x2 value2)
        public static Matrix3x2 Subtract(Matrix3x2 value1, Matrix3x2 value2)
        public static Matrix3x2 Multiply(Matrix3x2 value1, Matrix3x2 value2)

        public static Matrix3x2 operator -(Matrix3x2 value)
        public static Matrix3x2 operator +(Matrix3x2 value1, Matrix3x2 value2)
        public static Matrix3x2 operator -(Matrix3x2 value1, Matrix3x2 value2)
        public static Matrix3x2 operator *(Matrix3x2 value1, Matrix3x2 value2)
    }


    public struct Quaternion
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public Quaternion(float x, float y, float z, float w)
        public Quaternion(Vector3 vectorPart, float scalarPart)

        public static Quaternion Identity { get; }
        public bool IsIdentity { get; }

        public float Length()
        public float LengthSquared()
        public static Quaternion Normalize(Quaternion value)
        public static Quaternion Conjugate(Quaternion value)
        public static Quaternion Inverse(Quaternion value)
        public static Quaternion CreateFromAxisAngle(Vector3 axis, float angle)
        public static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        public static Quaternion CreateFromRotationMatrix(Matrix4x4 matrix)
        public static float Dot(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion Slerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        public static Quaternion Lerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        public static Quaternion Concatenate(Quaternion value1, Quaternion value2)
        public static Quaternion Negate(Quaternion value)
        public static Quaternion Add(Quaternion value1, Quaternion value2)
        public static Quaternion Subtract(Quaternion value1, Quaternion value2)
        public static Quaternion Multiply(Quaternion value1, Quaternion value2)
        public static Quaternion Multiply(Quaternion value1, float value2)
        public static Quaternion Divide(Quaternion value1, Quaternion value2)

        public static Quaternion operator -(Quaternion value)
        public static Quaternion operator +(Quaternion value1, Quaternion value2)
        public static Quaternion operator -(Quaternion value1, Quaternion value2)
        public static Quaternion operator *(Quaternion value1, Quaternion value2)
        public static Quaternion operator *(Quaternion value1, float value2)
        public static Quaternion operator /(Quaternion value1, Quaternion value2)
    }


    public struct Plane
    {
        public Vector3 Normal;
        public float D;

        public Plane(float a, float b, float c, float d)
        public Plane(Vector3 normal, float d)
        public Plane(Vector4 value)
        public Plane(Vector3 point1, Vector3 point2, Vector3 point3)

        public static Plane Normalize(Plane value)
        public static Plane Transform(Plane plane, Matrix4x4 matrix)
        public static Plane Transform(Plane plane, Quaternion rotation)
        public static float Dot(Plane plane, Vector4 value)
        public static float DotCoordinate(Plane plane, Vector3 value)
        public static float DotNormal(Plane plane, Vector3 value)
    }
}
