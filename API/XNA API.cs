// For reference, here is the old XNA API for vector and matrix types

namespace Microsoft.Xna.Framework
{
    public struct Vector2
    {
        public float X;
        public float Y;

        public static Vector2 Zero { get; }
        public static Vector2 One { get; }
        public static Vector2 UnitX { get; }
        public static Vector2 UnitY { get; }

        public Vector2(float x, float y)
        public Vector2(float value)

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector2 value1, Vector2 value2)
        public static float DistanceSquared(Vector2 value1, Vector2 value2)
        public static float Dot(Vector2 value1, Vector2 value2)
        public void Normalize()
        public static Vector2 Normalize(Vector2 value)
        public static Vector2 Reflect(Vector2 vector, Vector2 normal)
        public static Vector2 Min(Vector2 value1, Vector2 value2)
        public static Vector2 Max(Vector2 value1, Vector2 value2)
        public static Vector2 Clamp(Vector2 value1, Vector2 min, Vector2 max)
        public static Vector2 Lerp(Vector2 value1, Vector2 value2, float amount)
        public static Vector2 Barycentric(Vector2 value1, Vector2 value2, Vector2 value3, float amount1, float amount2)
        public static Vector2 SmoothStep(Vector2 value1, Vector2 value2, float amount)
        public static Vector2 CatmullRom(Vector2 value1, Vector2 value2, Vector2 value3, Vector2 value4, float amount)
        public static Vector2 Hermite(Vector2 value1, Vector2 tangent1, Vector2 value2, Vector2 tangent2, float amount)
        public static Vector2 Transform(Vector2 position, Matrix matrix)
        public static Vector2 TransformNormal(Vector2 normal, Matrix matrix)
        public static Vector2 Transform(Vector2 value, Quaternion rotation)
        public static void Transform(Vector2[] sourceArray, ref Matrix matrix, Vector2[] destinationArray)
        public static void Transform(Vector2[] sourceArray, int sourceIndex, ref Matrix matrix, Vector2[] destinationArray, int destinationIndex, int length)
        public static void TransformNormal(Vector2[] sourceArray, ref Matrix matrix, Vector2[] destinationArray)
        public static void TransformNormal(Vector2[] sourceArray, int sourceIndex, ref Matrix matrix, Vector2[] destinationArray, int destinationIndex, int length)
        public static void Transform(Vector2[] sourceArray, ref Quaternion rotation, Vector2[] destinationArray)
        public static void Transform(Vector2[] sourceArray, int sourceIndex, ref Quaternion rotation, Vector2[] destinationArray, int destinationIndex, int length)
        public static Vector2 Negate(Vector2 value)
        public static Vector2 Add(Vector2 value1, Vector2 value2)
        public static Vector2 Subtract(Vector2 value1, Vector2 value2)
        public static Vector2 Multiply(Vector2 value1, Vector2 value2)
        public static Vector2 Multiply(Vector2 value1, float scaleFactor)
        public static Vector2 Divide(Vector2 value1, Vector2 value2)
        public static Vector2 Divide(Vector2 value1, float divider)

        public static Vector2 operator -(Vector2 value)
        public static Vector2 operator +(Vector2 value1, Vector2 value2)
        public static Vector2 operator -(Vector2 value1, Vector2 value2)
        public static Vector2 operator *(Vector2 value1, Vector2 value2)
        public static Vector2 operator *(Vector2 value, float scaleFactor)
        public static Vector2 operator *(float scaleFactor, Vector2 value)
        public static Vector2 operator /(Vector2 value1, Vector2 value2)
        public static Vector2 operator /(Vector2 value1, float divider)
    }


    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public static Vector3 Zero { get; }
        public static Vector3 One { get; }
        public static Vector3 UnitX { get; }
        public static Vector3 UnitY { get; }
        public static Vector3 UnitZ { get; }
        public static Vector3 Up { get; }
        public static Vector3 Down { get; }
        public static Vector3 Right { get; }
        public static Vector3 Left { get; }
        public static Vector3 Forward { get; }
        public static Vector3 Backward { get; }

        public Vector3(float x, float y, float z)
        public Vector3(Vector2 value, float z)
        public Vector3(float value)

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector3 value1, Vector3 value2)
        public static float DistanceSquared(Vector3 value1, Vector3 value2)
        public static float Dot(Vector3 vector1, Vector3 vector2)
        public void Normalize()
        public static Vector3 Normalize(Vector3 value)
        public static Vector3 Cross(Vector3 vector1, Vector3 vector2)
        public static Vector3 Reflect(Vector3 vector, Vector3 normal)
        public static Vector3 Min(Vector3 value1, Vector3 value2)
        public static Vector3 Max(Vector3 value1, Vector3 value2)
        public static Vector3 Clamp(Vector3 value1, Vector3 min, Vector3 max)
        public static Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
        public static Vector3 Barycentric(Vector3 value1, Vector3 value2, Vector3 value3, float amount1, float amount2)
        public static Vector3 SmoothStep(Vector3 value1, Vector3 value2, float amount)
        public static Vector3 CatmullRom(Vector3 value1, Vector3 value2, Vector3 value3, Vector3 value4, float amount)
        public static Vector3 Hermite(Vector3 value1, Vector3 tangent1, Vector3 value2, Vector3 tangent2, float amount)
        public static Vector3 Transform(Vector3 position, Matrix matrix)
        public static Vector3 TransformNormal(Vector3 normal, Matrix matrix)
        public static Vector3 Transform(Vector3 value, Quaternion rotation)
        public static void Transform(Vector3[] sourceArray, ref Matrix matrix, Vector3[] destinationArray)
        public static void Transform(Vector3[] sourceArray, int sourceIndex, ref Matrix matrix, Vector3[] destinationArray, int destinationIndex, int length)
        public static void TransformNormal(Vector3[] sourceArray, ref Matrix matrix, Vector3[] destinationArray)
        public static void TransformNormal(Vector3[] sourceArray, int sourceIndex, ref Matrix matrix, Vector3[] destinationArray, int destinationIndex, int length)
        public static void Transform(Vector3[] sourceArray, ref Quaternion rotation, Vector3[] destinationArray)
        public static void Transform(Vector3[] sourceArray, int sourceIndex, ref Quaternion rotation, Vector3[] destinationArray, int destinationIndex, int length)
        public static Vector3 Negate(Vector3 value)
        public static Vector3 Add(Vector3 value1, Vector3 value2)
        public static Vector3 Subtract(Vector3 value1, Vector3 value2)
        public static Vector3 Multiply(Vector3 value1, Vector3 value2)
        public static Vector3 Multiply(Vector3 value1, float scaleFactor)
        public static Vector3 Divide(Vector3 value1, Vector3 value2)
        public static Vector3 Divide(Vector3 value1, float value2)

        public static Vector3 operator -(Vector3 value)
        public static Vector3 operator +(Vector3 value1, Vector3 value2)
        public static Vector3 operator -(Vector3 value1, Vector3 value2)
        public static Vector3 operator *(Vector3 value1, Vector3 value2)
        public static Vector3 operator *(Vector3 value, float scaleFactor)
        public static Vector3 operator *(float scaleFactor, Vector3 value)
        public static Vector3 operator /(Vector3 value1, Vector3 value2)
        public static Vector3 operator /(Vector3 value, float divider)
    }


    public struct Vector4
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public static Vector4 Zero { get; }
        public static Vector4 One { get; }
        public static Vector4 UnitX { get; }
        public static Vector4 UnitY { get; }
        public static Vector4 UnitZ { get; }
        public static Vector4 UnitW { get; }

        public Vector4(float x, float y, float z, float w)
        public Vector4(Vector2 value, float z, float w)
        public Vector4(Vector3 value, float w)
        public Vector4(float value)

        public float Length()
        public float LengthSquared()
        public static float Distance(Vector4 value1, Vector4 value2)
        public static float DistanceSquared(Vector4 value1, Vector4 value2)
        public static float Dot(Vector4 vector1, Vector4 vector2)
        public void Normalize()
        public static Vector4 Normalize(Vector4 vector)
        public static Vector4 Min(Vector4 value1, Vector4 value2)
        public static Vector4 Max(Vector4 value1, Vector4 value2)
        public static Vector4 Clamp(Vector4 value1, Vector4 min, Vector4 max)
        public static Vector4 Lerp(Vector4 value1, Vector4 value2, float amount)
        public static Vector4 Barycentric(Vector4 value1, Vector4 value2, Vector4 value3, float amount1, float amount2)
        public static Vector4 SmoothStep(Vector4 value1, Vector4 value2, float amount)
        public static Vector4 CatmullRom(Vector4 value1, Vector4 value2, Vector4 value3, Vector4 value4, float amount)
        public static Vector4 Hermite(Vector4 value1, Vector4 tangent1, Vector4 value2, Vector4 tangent2, float amount)
        public static Vector4 Transform(Vector2 position, Matrix matrix)
        public static Vector4 Transform(Vector3 position, Matrix matrix)
        public static Vector4 Transform(Vector4 vector, Matrix matrix)
        public static Vector4 Transform(Vector2 value, Quaternion rotation)
        public static Vector4 Transform(Vector3 value, Quaternion rotation)
        public static Vector4 Transform(Vector4 value, Quaternion rotation)
        public static void Transform(Vector4[] sourceArray, ref Matrix matrix, Vector4[] destinationArray)
        public static void Transform(Vector4[] sourceArray, int sourceIndex, ref Matrix matrix, Vector4[] destinationArray, int destinationIndex, int length)
        public static void Transform(Vector4[] sourceArray, ref Quaternion rotation, Vector4[] destinationArray)
        public static void Transform(Vector4[] sourceArray, int sourceIndex, ref Quaternion rotation, Vector4[] destinationArray, int destinationIndex, int length)
        public static Vector4 Negate(Vector4 value)
        public static Vector4 Add(Vector4 value1, Vector4 value2)
        public static Vector4 Subtract(Vector4 value1, Vector4 value2)
        public static Vector4 Multiply(Vector4 value1, Vector4 value2)
        public static Vector4 Multiply(Vector4 value1, float scaleFactor)
        public static Vector4 Divide(Vector4 value1, Vector4 value2)
        public static Vector4 Divide(Vector4 value1, float divider)

        public static Vector4 operator -(Vector4 value)
        public static Vector4 operator +(Vector4 value1, Vector4 value2)
        public static Vector4 operator -(Vector4 value1, Vector4 value2)
        public static Vector4 operator *(Vector4 value1, Vector4 value2)
        public static Vector4 operator *(Vector4 value1, float scaleFactor)
        public static Vector4 operator *(float scaleFactor, Vector4 value1)
        public static Vector4 operator /(Vector4 value1, Vector4 value2)
        public static Vector4 operator /(Vector4 value1, float divider)
    }


    public struct Matrix
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

        public static Matrix Identity { get; }

        public Vector3 Up { get; set; }
        public Vector3 Down { get; set; }
        public Vector3 Right { get; set; }
        public Vector3 Left { get; set; }
        public Vector3 Forward { get; set; }
        public Vector3 Backward { get; set; }
        public Vector3 Translation { get; set; }

        public Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)

        public static Matrix CreateBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 cameraUpVector, Vector3? cameraForwardVector)
        public static Matrix CreateConstrainedBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 rotateAxis, Vector3? cameraForwardVector, Vector3? objectForwardVector)
        public static Matrix CreateTranslation(Vector3 position)
        public static Matrix CreateTranslation(float xPosition, float yPosition, float zPosition)
        public static Matrix CreateScale(float xScale, float yScale, float zScale)
        public static Matrix CreateScale(Vector3 scales)
        public static Matrix CreateScale(float scale)
        public static Matrix CreateRotationX(float radians)
        public static Matrix CreateRotationY(float radians)
        public static Matrix CreateRotationZ(float radians)
        public static Matrix CreateFromAxisAngle(Vector3 axis, float angle)
        public static Matrix CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
        public static Matrix CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
        public static Matrix CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
        public static Matrix CreateLookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
        public static Matrix CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
        public static Matrix CreateFromQuaternion(Quaternion quaternion)
        public static Matrix CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        public static Matrix CreateShadow(Vector3 lightDirection, Plane plane)
        public static Matrix CreateReflection(Plane value)
        public float Determinant()
        public static Matrix Invert(Matrix matrix)
        public bool Decompose(out Vector3 scale, out Quaternion rotation, out Vector3 translation)
        public static Matrix Transform(Matrix value, Quaternion rotation)
        public static Matrix Transpose(Matrix matrix)
        public static Matrix Lerp(Matrix matrix1, Matrix matrix2, float amount)
        public static Matrix Negate(Matrix matrix)
        public static Matrix Add(Matrix matrix1, Matrix matrix2)
        public static Matrix Subtract(Matrix matrix1, Matrix matrix2)
        public static Matrix Multiply(Matrix matrix1, Matrix matrix2)
        public static Matrix Multiply(Matrix matrix1, float scaleFactor)
        public static Matrix Divide(Matrix matrix1, Matrix matrix2)
        public static Matrix Divide(Matrix matrix1, float divider)

        public static Matrix operator -(Matrix matrix1)
        public static Matrix operator +(Matrix matrix1, Matrix matrix2)
        public static Matrix operator -(Matrix matrix1, Matrix matrix2)
        public static Matrix operator *(Matrix matrix1, Matrix matrix2)
        public static Matrix operator *(Matrix matrix, float scaleFactor)
        public static Matrix operator *(float scaleFactor, Matrix matrix)
        public static Matrix operator /(Matrix matrix1, Matrix matrix2)
        public static Matrix operator /(Matrix matrix1, float divider)
    }


    public struct Quaternion
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public static Quaternion Identity { get; }

        public Quaternion(float x, float y, float z, float w)
        public Quaternion(Vector3 vectorPart, float scalarPart)

        public float Length()
        public float LengthSquared()
        public void Normalize()
        public static Quaternion Normalize(Quaternion quaternion)
        public void Conjugate()
        public static Quaternion Conjugate(Quaternion value)
        public static Quaternion Inverse(Quaternion quaternion)
        public static Quaternion CreateFromAxisAngle(Vector3 axis, float angle)
        public static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        public static Quaternion CreateFromRotationMatrix(Matrix matrix)
        public static float Dot(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion Slerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        public static Quaternion Lerp(Quaternion quaternion1, Quaternion quaternion2, float amount)
        public static Quaternion Concatenate(Quaternion value1, Quaternion value2)
        public static Quaternion Negate(Quaternion quaternion)
        public static Quaternion Add(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion Subtract(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion Multiply(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion Multiply(Quaternion quaternion1, float scaleFactor)
        public static Quaternion Divide(Quaternion quaternion1, Quaternion quaternion2)

        public static Quaternion operator -(Quaternion quaternion)
        public static Quaternion operator +(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion operator -(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion operator *(Quaternion quaternion1, Quaternion quaternion2)
        public static Quaternion operator *(Quaternion quaternion1, float scaleFactor)
        public static Quaternion operator /(Quaternion quaternion1, Quaternion quaternion2)
    }


    public struct Plane
    {
        public Vector3 Normal;
        public float D;

        public Plane(float a, float b, float c, float d)
        public Plane(Vector3 normal, float d)
        public Plane(Vector4 value)
        public Plane(Vector3 point1, Vector3 point2, Vector3 point3)

        public void Normalize()
        public static Plane Normalize(Plane value)
        public static Plane Transform(Plane plane, Matrix matrix)
        public static Plane Transform(Plane plane, Quaternion rotation)
        public float Dot(Vector4 value)
        public float DotCoordinate(Vector3 value)
        public float DotNormal(Vector3 value)
    }


    public static class MathHelper
    {
        public const float E = 2.718282f;
        public const float Log2E = 1.442695f;
        public const float Log10E = 0.4342945f;
        public const float Pi = 3.141593f;
        public const float TwoPi = 6.283185f;
        public const float PiOver2 = 1.570796f;
        public const float PiOver4 = 0.7853982f;

        public static float ToRadians(float degrees)
        public static float ToDegrees(float radians)
        public static float Distance(float value1, float value2)
        public static float Min(float value1, float value2)
        public static float Max(float value1, float value2)
        public static float Clamp(float value, float min, float max)
        public static float Lerp(float value1, float value2, float amount)
        public static float Barycentric(float value1, float value2, float value3, float amount1, float amount2)
        public static float SmoothStep(float value1, float value2, float amount)
        public static float Hermite(float value1, float tangent1, float value2, float tangent2, float amount)
        public static float WrapAngle(float angle)
    }
}
