// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NativeComponent;
using System;
using System.Linq;
using System.Runtime.InteropServices;
using Windows.Foundation;

#if WINDOWS_UWP
using System.Numerics;
using Windows.Graphics.Effects;
#else
using Microsoft.Graphics.Canvas.Numerics;
#endif

namespace test.managed
{
    [TestClass]
    public class PixelShaderEffectTests
    {
        [TestMethod]
        public void PixelShaderEffect_ConstructFromBadBlob_Fails()
        {
            Assert.ThrowsException<ArgumentException>(
                () => new PixelShaderEffect(null));

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(new byte[0]),
                "Unable to load the specified shader. This should be a Direct3D pixel shader compiled for shader model 4.");

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(new byte[] { 1, 2, 3 }),
                "Unable to load the specified shader. This should be a Direct3D pixel shader compiled for shader model 4.");
        }


        [TestMethod]
        public void PixelShaderEffect_ConstructFromVertexShader_Fails()
        {
            const string hlsl =
            @"
                void main() { }
            ";

            byte[] compiledShader = ShaderCompiler.CompileShader(hlsl, "vs_4_0");

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(compiledShader),
                "Unable to load the specified shader. This should be a Direct3D pixel shader compiled for shader model 4.");
        }


        [TestMethod]
        public void PixelShaderEffect_ShaderHasMultipleConstantBuffers_Fails()
        {
            const string hlsl1 =
            @"
                cbuffer a { float4 x; };
                cbuffer b { float4 y; };

                float4 main() : SV_Target
                {
                    return x + y;
                }
            ";

            const string hlsl2 =
            @"
                cbuffer a : register(b1) { float4 x; };

                float4 main() : SV_Target
                {
                    return x;
                }
            ";

            byte[] compiledShader1 = ShaderCompiler.CompileShader(hlsl1, "ps_4_0");
            byte[] compiledShader2 = ShaderCompiler.CompileShader(hlsl2, "ps_4_0");

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(compiledShader1),
                "Unsupported constant buffer layout. There should be a single constant buffer bound to b0.");

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(compiledShader2),
                "Unsupported constant buffer layout. There should be a single constant buffer bound to b0.");
        }


        [TestMethod]
        public void PixelShaderEffect_ShaderHasStructConstant_Fails()
        {
            const string hlsl =
            @"
                struct Foo { float x; };
                
                Foo bar;

                float4 main() : SV_Target
                {
                    return bar.x;
                }
            ";

            byte[] compiledShader = ShaderCompiler.CompileShader(hlsl, "ps_4_0");

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(compiledShader),
                "Shader property 'bar' is an unsupported type.");
        }


        [TestMethod]
        public void PixelShaderEffect_FeatureLevelValidation()
        {
            const string hlsl =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            var effect93 = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0_level_9_3"));
            var effect40 = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            using (var featureLevel93Device = DeviceCreator.CreateDevice(useFeatureLevel93: true))
            using (var canvasDevice = CanvasDevice.CreateFromDirect3D11Device(featureLevel93Device))
            {
                Assert.IsTrue(effect93.IsSupported(canvasDevice));
                Assert.IsFalse(effect40.IsSupported(canvasDevice));

                using (var renderTarget = new CanvasRenderTarget(canvasDevice, 1, 1, 96))
                using (var drawingSession = renderTarget.CreateDrawingSession())
                {
                    drawingSession.DrawImage(effect93);

                    Utils.AssertThrowsException<Exception>(
                        () => drawingSession.DrawImage(effect40),
                        "This shader requires a higher Direct3D feature level than is supported by the device. Check PixelShaderEffect.IsSupported before using it.");
                }
            }
        }


        [TestMethod]
        public void PixelShaderEffect_SourceTextures()
        {
            const string hlslNoTextures =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            const string hlslOneTexture =
            @"
                texture2D t1;
                sampler s1;

                float4 main() : SV_Target
                {
                    return t1.Sample(s1, 0);
                }
            ";

            const string hlslTwoTextures =
            @"
                texture2D t1, t2;
                sampler s1, s2;

                float4 main() : SV_Target
                {
                    return t1.Sample(s1, 0) +
                           t2.Sample(s2, 0);
                }
            ";

            const string hlslEightTextures =
            @"
                texture2D t1, t2, t3, t4, t5, t6, t7, t8;
                sampler s1, s2, s3, s4, s5, s6, s7, s8;

                float4 main() : SV_Target
                {
                    return t1.Sample(s1, 0) +
                           t2.Sample(s2, 0) +
                           t3.Sample(s3, 0) +
                           t4.Sample(s4, 0) +
                           t5.Sample(s5, 0) +
                           t6.Sample(s6, 0) +
                           t7.Sample(s7, 0) +
                           t8.Sample(s8, 0);
                }
            ";

            const string hlslNineTextures =
            @"
                texture2D t1, t2, t3, t4, t5, t6, t7, t8, t9;
                sampler s1, s2, s3, s4, s5, s6, s7, s8, s9;

                float4 main() : SV_Target
                {
                    return t1.Sample(s1, 0) +
                           t2.Sample(s2, 0) +
                           t3.Sample(s3, 0) +
                           t4.Sample(s4, 0) +
                           t5.Sample(s5, 0) +
                           t6.Sample(s6, 0) +
                           t7.Sample(s7, 0) +
                           t8.Sample(s8, 0) +
                           t9.Sample(s8, 0);
                }
            ";

            ValidateSourceProperties(hlslNoTextures, 0);
            ValidateSourceProperties(hlslOneTexture, 1);
            ValidateSourceProperties(hlslTwoTextures, 2);
            ValidateSourceProperties(hlslEightTextures, 8);

            Utils.AssertThrowsException<ArgumentException>(
                () => new PixelShaderEffect(ShaderCompiler.CompileShader(hlslNineTextures, "ps_4_0")),
                "Shader has too many input textures.");
        }


        void ValidateSourceProperties(string hlsl, int expectedSourceCount)
        {
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            ValidateSourceProperty(effect, () => effect.Source1, (s) => effect.Source1 = s, expectedSourceCount, 1);
            ValidateSourceProperty(effect, () => effect.Source2, (s) => effect.Source2 = s, expectedSourceCount, 2);
            ValidateSourceProperty(effect, () => effect.Source3, (s) => effect.Source3 = s, expectedSourceCount, 3);
            ValidateSourceProperty(effect, () => effect.Source4, (s) => effect.Source4 = s, expectedSourceCount, 4);
            ValidateSourceProperty(effect, () => effect.Source5, (s) => effect.Source5 = s, expectedSourceCount, 5);
            ValidateSourceProperty(effect, () => effect.Source6, (s) => effect.Source6 = s, expectedSourceCount, 6);
            ValidateSourceProperty(effect, () => effect.Source7, (s) => effect.Source7 = s, expectedSourceCount, 7);
            ValidateSourceProperty(effect, () => effect.Source8, (s) => effect.Source8 = s, expectedSourceCount, 8);
        }


        void ValidateSourceProperty(PixelShaderEffect effect, Func<IGraphicsEffectSource> getter, Action<IGraphicsEffectSource> setter, int expectedSourceCount, int whichSource)
        {
            // Should be initially null.
            Assert.IsNull(getter());

            // Should always be able to set to null.
            setter(null);
            Assert.IsNull(getter());

            var source = new ColorSourceEffect();

            if (whichSource <= expectedSourceCount)
            {
                // Should be able to set and get non-null values for this source of the shader.
                setter(source);

                Assert.AreEqual(source, getter());
            }
            else
            {
                // Should not be able to set this source.
                Utils.AssertThrowsException<ArgumentException>(
                    () => setter(source),
                    "Source" + whichSource + " must be null when using this pixel shader (shader inputs: " + expectedSourceCount + ").");
            }
        }


        [TestMethod]
        public void PixelShaderEffect_Realize_NoConstants()
        {
            const string hlsl =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            using (var canvasDevice = new CanvasDevice())
            using (var renderTarget = new CanvasRenderTarget(canvasDevice, 1, 1, 96))
            using (var drawingSession = renderTarget.CreateDrawingSession())
            {
                drawingSession.DrawImage(effect);
            }
        }


        [TestMethod]
        public void PixelShaderEffect_Realize_WithConstants()
        {
            const string hlsl =
            @"
                float4 color;

                float4 main() : SV_Target
                {
                    return color;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            using (var canvasDevice = new CanvasDevice())
            using (var renderTarget = new CanvasRenderTarget(canvasDevice, 1, 1, 96))
            using (var drawingSession = renderTarget.CreateDrawingSession())
            {
                drawingSession.DrawImage(effect);
            }
        }


        [TestMethod]
        public void PixelShaderEffect_PropertiesDictionary_Methods()
        {
            const string hlsl =
            @"
                float foo = 23;
                int i = 42;

                float4 main() : SV_Target
                {
                    return foo * i;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Test the various dictionary methods that are forwarded from our
            // Map<> implementation via PixelShaderEffectPropertyMapTraits.

            // Count.
            Assert.AreEqual(2, effect.Properties.Count);

            // HasKey.
            Assert.IsTrue(effect.Properties.ContainsKey("foo"));
            Assert.IsTrue(effect.Properties.ContainsKey("i"));

            Assert.IsFalse(effect.Properties.ContainsKey("bar"));
            Assert.IsFalse(effect.Properties.ContainsKey("I"));
            Assert.IsFalse(effect.Properties.ContainsKey(string.Empty));

            // Lookup.
            Assert.AreEqual(23.0f, effect.Properties["foo"]);
            Assert.AreEqual(42, effect.Properties["i"]);

            object value;

            Utils.AssertThrowsException<ArgumentException>(
                () => value = effect.Properties["bar"],
                "Shader does not have a property named 'bar'.");

            Utils.AssertThrowsException<ArgumentException>(
                () => value = effect.Properties[string.Empty],
                "Shader does not have a property named ''.");

            // GetKeyValuePairs.
            var array = effect.Properties.ToArray();

            Assert.AreEqual(2, array.Length);

            Assert.AreEqual("foo", array[0].Key);
            Assert.AreEqual(23.0f, array[0].Value);

            Assert.AreEqual("i", array[1].Key);
            Assert.AreEqual(42, array[1].Value);

            // Remove and Clear are not supported.
            Assert.ThrowsException<NotImplementedException>(() => effect.Properties.Remove("foo"));
            Assert.ThrowsException<NotImplementedException>(() => effect.Properties.Clear());
        }


        [TestMethod]
        public void PixelShaderEffect_PropertiesDictionary_InsertErrorCases()
        {
            const string hlsl =
            @"
                float foo;
                int i;
                float array[5];

                float4 main() : SV_Target
                {
                    return foo * array[i];
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Setting valid types should work.
            effect.Properties["foo"] = 1f;
            effect.Properties["i"] = 1;
            effect.Properties["array"] = new float[5];

            // Try to insert an unknown property.
            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["bar"] = 1,
                "Shader does not have a property named 'bar'.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties[string.Empty] = 1,
                "Shader does not have a property named ''.");

            // Pass wrong property types.
            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["foo"] = 1,
                "Wrong type. Shader property 'foo' is of type Single.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["foo"] = new Vector3[1],
                "Wrong type. Shader property 'foo' is of type Single.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["i"] = 1f,
                "Wrong type. Shader property 'i' is of type Int32.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["i"] = new Matrix3x2[1],
                "Wrong type. Shader property 'i' is of type Int32.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["array"] = 1,
                "Wrong type. Shader property 'array' is an array of Single.");

            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["array"] = new bool[1],
                "Wrong type. Shader property 'array' is an array of Single.");

            // Pass a wrong array size.
            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["array"] = new float[4],
                "Wrong array size. Shader property 'array' is an array of 5 elements.");

            // Insert null.
            Utils.AssertThrowsException<ArgumentException>(
                () => effect.Properties["foo"] = null,
                "Wrong type. Shader property 'foo' is of type Single.");
        }


        // For non-matrix value types (float, int, bool, Vector2, Vector3, Vector4).
        void TestPropertyType<T>(string hlsl, T initialValue, T[] initialArray, T newValue, T[] newArray)
        {
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Verify initial values.
            Assert.AreEqual(initialValue, effect.Properties["value"]);
            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array"]);

            // Set new values.
            effect.Properties["value"] = newValue;
            effect.Properties["array"] = newArray;

            // Read back the modified values.
            Assert.AreEqual(newValue, effect.Properties["value"]);
            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array"]);
        }


        // For non-matrix types that are accessed as component arrays (int and bool vectors).
        void TestPropertyType<T>(string hlsl, T[] initialValue, T[] initialArray, T[] newValue, T[] newArray)
        {
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Verify initial values.
            CollectionAssert.AreEqual(initialValue, (T[])effect.Properties["value"]);
            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array"]);

            // Set new values.
            effect.Properties["value"] = newValue;
            effect.Properties["array"] = newArray;

            // Read back the modified values.
            CollectionAssert.AreEqual(newValue, (T[])effect.Properties["value"]);
            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array"]);
        }


        // For matrix value types (Matrix3x2, Matrix4x4).
        void TestMatrixPropertyType<T>(string hlsl, T initialValue, T[] initialArray, T newValue, T[] newArray)
        {
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Verify initial values.
            Assert.AreEqual(initialValue, effect.Properties["value_row"]);
            Assert.AreEqual(initialValue, effect.Properties["value_col"]);

            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array_row"]);
            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array_col"]);

            // Set new values.
            effect.Properties["value_row"] = newValue;
            effect.Properties["value_col"] = newValue;

            effect.Properties["array_row"] = newArray;
            effect.Properties["array_col"] = newArray;

            // Read back the modified values.
            Assert.AreEqual(newValue, effect.Properties["value_row"]);
            Assert.AreEqual(newValue, effect.Properties["value_col"]);

            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array_row"]);
            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array_col"]);
        }


        // For matrix types that are accessed as component arrays (int or bool matrix + float matrices other than 3x2 and 4x4).
        void TestMatrixPropertyType<T>(string hlsl, T[] initialValue, T[] initialArray, T[] newValue, T[] newArray)
        {
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Verify initial values.
            CollectionAssert.AreEqual(initialValue, (T[])effect.Properties["value_row"]);
            CollectionAssert.AreEqual(initialValue, (T[])effect.Properties["value_col"]);

            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array_row"]);
            CollectionAssert.AreEqual(initialArray, (T[])effect.Properties["array_col"]);

            // Set new values.
            effect.Properties["value_row"] = newValue;
            effect.Properties["value_col"] = newValue;

            effect.Properties["array_row"] = newArray;
            effect.Properties["array_col"] = newArray;

            // Read back the modified values.
            CollectionAssert.AreEqual(newValue, (T[])effect.Properties["value_row"]);
            CollectionAssert.AreEqual(newValue, (T[])effect.Properties["value_col"]);

            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array_row"]);
            CollectionAssert.AreEqual(newArray, (T[])effect.Properties["array_col"]);
        }


        [TestMethod]
        public void PixelShaderEffect_PropertyTypes()
        {
            // Work around VS2015 bug.
            // The test runner hangs in x64 mode apparently related to total number of individual tests.
            // Merging these ideally separate tests into one avoids the hang.

            PixelShaderEffect_PropertyType_Float();
            PixelShaderEffect_PropertyType_Vector2();
            PixelShaderEffect_PropertyType_Vector3();
            PixelShaderEffect_PropertyType_Vector4();
            PixelShaderEffect_PropertyType_Matrix3x2();
            PixelShaderEffect_PropertyType_Matrix4x4();
            PixelShaderEffect_PropertyType_Matrix2x3();
            PixelShaderEffect_PropertyType_Int();
            PixelShaderEffect_PropertyType_IntVector();
            PixelShaderEffect_PropertyType_IntMatrix();
            PixelShaderEffect_PropertyType_Bool();
            PixelShaderEffect_PropertyType_BoolVector();
            PixelShaderEffect_PropertyType_BoolMatrix();
        }


        public void PixelShaderEffect_PropertyType_Float()
        {
            const string hlsl =
            @"
                float value = 1;
                float array[2] = { 2, 3 };

                float4 main() : SV_Target
                {
                    return value * array[0];
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                1f,
                new float[] { 2, 3 },
                // New values to change the property to.
                4f,
                new float[] { 5, 6 });
        }


        public void PixelShaderEffect_PropertyType_Vector2()
        {
            const string hlsl =
            @"
                float2 value = { 1, 2 };
                float2 array[2] = { 3, 4, 5, 6 };

                float4 main() : SV_Target
                {
                    return value.x * array[0].x;
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                MakeVector2(1, 2),
                new Vector2[]
                {
                    MakeVector2(3, 4),
                    MakeVector2(5, 6),
                },
                // New values to change the property to.
                MakeVector2(7, 8),
                new Vector2[]
                {
                    MakeVector2(9, 10),
                    MakeVector2(11, 12),
                });
        }


        public void PixelShaderEffect_PropertyType_Vector3()
        {
            const string hlsl =
            @"
                float3 value = { 1, 2, 3 };
                float3 array[2] = { 4, 5, 6, 7, 8, 9 };

                float4 main() : SV_Target
                {
                    return value.x * array[0].x;
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                MakeVector3(1, 2, 3),
                new Vector3[]
                {
                    MakeVector3(4, 5, 6),
                    MakeVector3(7, 8, 9),
                },
                // New values to change the property to.
                MakeVector3(10, 11, 12),
                new Vector3[]
                {
                    MakeVector3(13, 14, 15),
                    MakeVector3(16, 17, 18),
                });
        }


        public void PixelShaderEffect_PropertyType_Vector4()
        {
            const string hlsl =
            @"
                float4 value = { 1, 2, 3, 4 };
                float4 array[2] = { 5, 6, 7, 8, 9, 10, 11, 12 };

                float4 main() : SV_Target
                {
                    return value.x * array[0].x;
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                MakeVector4(1, 2, 3, 4),
                new Vector4[]
                {
                    MakeVector4(5, 6, 7, 8),
                    MakeVector4(9, 10, 11, 12),
                },
                // New values to change the property to.
                MakeVector4(13, 14, 15, 16),
                new Vector4[]
                {
                    MakeVector4(17, 18, 19, 20),
                    MakeVector4(21, 22, 23, 24),
                });
        }


        public void PixelShaderEffect_PropertyType_Matrix3x2()
        {
            const string hlsl =
            @"
                row_major    float3x2 value_row = { 1, 2, 3, 4, 5, 6 };
                column_major float3x2 value_col = { 1, 2, 3, 4, 5, 6 };

                row_major    float3x2 array_row[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
                column_major float3x2 array_col[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

                float4 main() : SV_Target
                {
                    return value_row._11 * value_col._11 * array_row[0]._11 * array_col[0]._11;
                }
            ";

            TestMatrixPropertyType(hlsl,
                // Expected initial values.
                MakeMatrix3x2(1, 2, 3, 4, 5, 6),
                new Matrix3x2[]
                {
                    MakeMatrix3x2(7, 8, 9, 10, 11, 12),
                    MakeMatrix3x2(13, 14, 15, 16, 17, 18),
                },
                // New values to change the property to.
                MakeMatrix3x2(19, 20, 21, 22, 23, 24),
                new Matrix3x2[]
                {
                    MakeMatrix3x2(25, 26, 27, 28, 29, 30),
                    MakeMatrix3x2(31, 32, 33, 34, 35, 36),
                });
        }


        public void PixelShaderEffect_PropertyType_Matrix4x4()
        {
            const string hlsl =
            @"
                row_major    float4x4 value_row = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
                column_major float4x4 value_col = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

                row_major    float4x4 array_row[2] = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 };
                column_major float4x4 array_col[2] = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 };

                float4 main() : SV_Target
                {
                    return value_row._11 * value_col._11 * array_row[0]._11 * array_col[0]._11;
                }
            ";

            TestMatrixPropertyType(hlsl,
                // Expected initial values.
                MakeMatrix4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
                new Matrix4x4[]
                {
                    MakeMatrix4x4(17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32),
                    MakeMatrix4x4(33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48),
                },
                // New values to change the property to.
                MakeMatrix4x4(49, 50, 51, 52, 53, 54, 55, 56, 7, 58, 59, 60, 61, 62, 63, 64),
                new Matrix4x4[]
                {
                    MakeMatrix4x4(65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80),
                    MakeMatrix4x4(81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96),
                });
        }


        public void PixelShaderEffect_PropertyType_Matrix2x3()
        {
            const string hlsl =
            @"
                row_major    float2x3 value_row = { 1, 2, 3, 4, 5, 6 };
                column_major float2x3 value_col = { 1, 2, 3, 4, 5, 6 };

                row_major    float2x3 array_row[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
                column_major float2x3 array_col[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

                float4 main() : SV_Target
                {
                    return value_row._11 * value_col._11 * array_row[0]._11 * array_col[0]._11;
                }
            ";

            TestMatrixPropertyType(hlsl,
                // Expected initial values.
                new float[] { 1, 2, 3, 4, 5, 6 },
                new float[] { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 },
                // New values to change the property to.
                new float[] { 19, 20, 21, 22, 23, 24 },
                new float[] { 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36 });
        }


        public void PixelShaderEffect_PropertyType_Int()
        {
            const string hlsl =
            @"
                int value = 1;
                int array[2] = { 2, 3 };

                float4 main() : SV_Target
                {
                    return value * array[0];
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                1,
                new int[] { 2, 3 },
                // New values to change the property to.
                4,
                new int[] { 5, 6 });
        }


        public void PixelShaderEffect_PropertyType_IntVector()
        {
            const string hlsl =
            @"
                int3 value = { 1, 2, 3 };
                int3 array[2] = { 4, 5, 6, 7, 8, 9 };

                float4 main() : SV_Target
                {
                    return value.x * array[0].x;
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                new int[] { 1, 2, 3 },
                new int[] { 4, 5, 6, 7, 8, 9 },
                // New values to change the property to.
                new int[] { 10, 11, 12 },
                new int[] { 13, 14, 15, 16, 17, 18 });
        }


        public void PixelShaderEffect_PropertyType_IntMatrix()
        {
            const string hlsl =
            @"
                row_major    int2x3 value_row = { 1, 2, 3, 4, 5, 6 };
                column_major int2x3 value_col = { 1, 2, 3, 4, 5, 6 };

                row_major    int2x3 array_row[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
                column_major int2x3 array_col[2] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

                float4 main() : SV_Target
                {
                    return value_row._11 * value_col._11 * array_row[0]._11 * array_col[0]._11;
                }
            ";

            TestMatrixPropertyType(hlsl,
                // Expected initial values.
                new int[] { 1, 2, 3, 4, 5, 6 },
                new int[] { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 },
                // New values to change the property to.
                new int[] { 19, 20, 21, 22, 23, 24 },
                new int[] { 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36 });
        }


        public void PixelShaderEffect_PropertyType_Bool()
        {
            const string hlsl =
            @"
                bool value = true;
                bool array[2] = { false, true };

                float4 main() : SV_Target
                {
                    return value * array[0];
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                true,
                new bool[] { false, true },
                // New values to change the property to.
                false,
                new bool[] { true, false });
        }


        public void PixelShaderEffect_PropertyType_BoolVector()
        {
            const string hlsl =
            @"
                bool3 value = { false, false, true };
                bool3 array[2] = { false, true, false, true, true, false };

                float4 main() : SV_Target
                {
                    return value.x * array[0].x;
                }
            ";

            TestPropertyType(hlsl,
                // Expected initial values.
                new bool[] { false, false, true },
                new bool[] { false, true, false, true, true, false },
                // New values to change the property to.
                new bool[] { true, false, false },
                new bool[] { true, false, false, false, true, true });
        }


        public void PixelShaderEffect_PropertyType_BoolMatrix()
        {
            const string hlsl =
            @"
                row_major    bool2x3 value_row = { false, true, false, false, true, true };
                column_major bool2x3 value_col = { false, true, false, false, true, true };

                row_major    bool2x3 array_row[2] = { false, true, false, true, false, false, true, true, true, false, true, false };
                column_major bool2x3 array_col[2] = { false, true, false, true, false, false, true, true, true, false, true, false };

                float4 main() : SV_Target
                {
                    return value_row._11 * value_col._11 * array_row[0]._11 * array_col[0]._11;
                }
            ";

            TestMatrixPropertyType(hlsl,
                // Expected initial values.
                new bool[] { false, true, false, false, true, true },
                new bool[] { false, true, false, true, false, false, true, true, true, false, true, false },
                // New values to change the property to.
                new bool[] { true, false, false, true, true, false },
                new bool[] { true, true, false, true, false, false, true, false, false, false, false, true });
        }


        [TestMethod]
        public void PixelShaderEffect_OneElementArraysAppearAsArraysNotScalars()
        {
            const string hlsl =
            @"
                float foo[1] = { 23 };

                float4 main() : SV_Target
                {
                    return foo[0];
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            CollectionAssert.AreEqual(new float[] { 23 }, (float[])effect.Properties["foo"]);

            effect.Properties["foo"] = new float[] { 42 };

            CollectionAssert.AreEqual(new float[] { 42 }, (float[])effect.Properties["foo"]);
        }


        [TestMethod]
        public void PixelShaderEffect_OneElementVectorsAndMatricesAppearAsScalarsNotArrays()
        {
            const string hlsl =
            @"
                float1 foo = { 23 };
                float1x1 bar = { 42 };

                float4 main() : SV_Target
                {
                    return foo * bar;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            Assert.AreEqual(23f, effect.Properties["foo"]);
            Assert.AreEqual(42f, effect.Properties["bar"]);

            effect.Properties["foo"] = 1f;
            effect.Properties["bar"] = 2f;

            Assert.AreEqual(1f, effect.Properties["foo"]);
            Assert.AreEqual(2f, effect.Properties["bar"]);
        }


        // Directly constructing Microsoft.Graphics.Canvas.Numerics versions of these types,
        // rather than using the System.Numerics ones that already come with constructors.
        static Vector2 MakeVector2(float x, float y)
        {
            return new Vector2()
            {
                X = x,
                Y = y,
            };
        }


        static Vector3 MakeVector3(float x, float y, float z)
        {
            return new Vector3()
            {
                X = x,
                Y = y,
                Z = z,
            };
        }


        static Vector4 MakeVector4(float x, float y, float z, float w)
        {
            return new Vector4()
            {
                X = x,
                Y = y,
                Z = z,
                W = w,
            };
        }


        static Matrix3x2 MakeMatrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)
        {
            return new Matrix3x2()
            {
                M11 = m11, M12 = m12,
                M21 = m21, M22 = m22,
                M31 = m31, M32 = m32,
            };
        }


        static Matrix4x4 MakeMatrix4x4(float m11, float m12, float m13, float m14,
                                       float m21, float m22, float m23, float m24,
                                       float m31, float m32, float m33, float m34,
                                       float m41, float m42, float m43, float m44)
        {
            return new Matrix4x4()
            {
                M11 = m11, M12 = m12, M13 = m13, M14 = m14,
                M21 = m21, M22 = m22, M23 = m23, M24 = m24,
                M31 = m31, M32 = m32, M33 = m33, M34 = m34,
                M41 = m41, M42 = m42, M43 = m43, M44 = m44,
            };
        }


        [TestMethod]
        public void PixelShaderEffect_InterpolationModeAccessors()
        {
            const string hlsl =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Check defaults.
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source1Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source2Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source3Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source4Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source5Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source6Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source7Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear, effect.Source8Interpolation);

            // Setters.
            effect.Source1Interpolation = CanvasImageInterpolation.Anisotropic;
            effect.Source2Interpolation = CanvasImageInterpolation.Linear;
            effect.Source3Interpolation = CanvasImageInterpolation.NearestNeighbor;
            effect.Source4Interpolation = CanvasImageInterpolation.Anisotropic;
            effect.Source5Interpolation = CanvasImageInterpolation.Linear;
            effect.Source6Interpolation = CanvasImageInterpolation.NearestNeighbor;
            effect.Source7Interpolation = CanvasImageInterpolation.Anisotropic;
            effect.Source8Interpolation = CanvasImageInterpolation.Linear;

            // Getters.
            Assert.AreEqual(CanvasImageInterpolation.Anisotropic,     effect.Source1Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear,          effect.Source2Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.NearestNeighbor, effect.Source3Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Anisotropic,     effect.Source4Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear,          effect.Source5Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.NearestNeighbor, effect.Source6Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Anisotropic,     effect.Source7Interpolation);
            Assert.AreEqual(CanvasImageInterpolation.Linear,          effect.Source8Interpolation);

            // Illegal values.
            Assert.ThrowsException<ArgumentException>(() => effect.Source1Interpolation = CanvasImageInterpolation.Cubic);
            Assert.ThrowsException<ArgumentException>(() => effect.Source1Interpolation = CanvasImageInterpolation.HighQualityCubic);
            Assert.ThrowsException<ArgumentException>(() => effect.Source1Interpolation = CanvasImageInterpolation.MultiSampleLinear);

            Assert.AreEqual(CanvasImageInterpolation.Anisotropic, effect.Source1Interpolation);
        }


        [TestMethod]
        public void PixelShaderEffect_CoordinateMappingAccessors()
        {
            const string hlsl =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Check defaults.
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source1Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source2Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source3Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source4Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source5Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source6Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source7Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source8Mapping);

            Assert.AreEqual(0, effect.MaxSamplerOffset);

            // Setters.
            effect.Source1Mapping = SamplerCoordinateMapping.OneToOne;
            effect.Source2Mapping = SamplerCoordinateMapping.Offset;
            effect.Source3Mapping = SamplerCoordinateMapping.OneToOne;
            effect.Source4Mapping = SamplerCoordinateMapping.Offset;
            effect.Source5Mapping = SamplerCoordinateMapping.OneToOne;
            effect.Source6Mapping = SamplerCoordinateMapping.Offset;
            effect.Source7Mapping = SamplerCoordinateMapping.OneToOne;
            effect.Source8Mapping = SamplerCoordinateMapping.Offset;

            effect.MaxSamplerOffset = 23;

            // Getters.
            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source1Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Offset,   effect.Source2Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source3Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Offset,   effect.Source4Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source5Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Offset,   effect.Source6Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source7Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Offset,   effect.Source8Mapping);

            Assert.AreEqual(23, effect.MaxSamplerOffset);
        }


        [TestMethod]
        public void PixelShaderEffect_BorderModeAccessors()
        {
            const string hlsl =
            @"
                float4 main() : SV_Target
                {
                    return 0;
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            // Check defaults.
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source1BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source2BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source3BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source4BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source5BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source6BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source7BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, effect.Source8BorderMode);

            for (int i = 0; i < 8; i++)
            {
                // Setters.
                effect.Source1BorderMode = (i == 0) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source2BorderMode = (i == 1) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source3BorderMode = (i == 2) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source4BorderMode = (i == 3) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source5BorderMode = (i == 4) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source6BorderMode = (i == 5) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source7BorderMode = (i == 6) ? EffectBorderMode.Hard : EffectBorderMode.Soft;
                effect.Source8BorderMode = (i == 7) ? EffectBorderMode.Hard : EffectBorderMode.Soft;

                // Getters.
                Assert.AreEqual((i == 0) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source1BorderMode);
                Assert.AreEqual((i == 1) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source2BorderMode);
                Assert.AreEqual((i == 2) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source3BorderMode);
                Assert.AreEqual((i == 3) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source4BorderMode);
                Assert.AreEqual((i == 4) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source5BorderMode);
                Assert.AreEqual((i == 5) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source6BorderMode);
                Assert.AreEqual((i == 6) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source7BorderMode);
                Assert.AreEqual((i == 7) ? EffectBorderMode.Hard : EffectBorderMode.Soft, effect.Source8BorderMode);
            }
        }


        [TestMethod]
        public void PixelShaderEffect_InputRectTooBigError()
        {
            using (new DisableDebugLayer())
            {
                const string hlsl =
                @"
                    texture2D t1;
                    sampler s1;

                    float4 main() : SV_Target
                    {
                        return t1.Sample(s1, 0);
                    }
                ";

                var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"))
                {
                    Source1Mapping = SamplerCoordinateMapping.Unknown
                };

                var device = new CanvasDevice();
                var renderTarget = new CanvasRenderTarget(device, 1, 1, 96);

                // Drawing with a fixed size input is ok.
                effect.Source1 = new CanvasRenderTarget(device, 1, 1, 96);

                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawImage(effect);
                }

                // Drawing with an infinite sized input is not!
                effect.Source1 = new ColorSourceEffect();

                Assert.ThrowsException<Exception>(() =>
                {
                    using (var ds = renderTarget.CreateDrawingSession())
                    {
                        ds.DrawImage(effect);
                    }
                });

                // But it's ok if we clamp the input back down to finite size.
                effect.Source1 = new CropEffect
                {
                    Source = new ColorSourceEffect(),
                    SourceRectangle = new Rect(0, 0, 100, 100)
                };

                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawImage(effect);
                }

                // Also ok if we change our mapping mode to something other than infinite.
                effect.Source1 = new ColorSourceEffect();
                effect.Source1Mapping = SamplerCoordinateMapping.OneToOne;

                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawImage(effect);
                }
            }
        }


        [TestMethod]
        public void PixelShaderEffect_MaxOffsetAndOffsetMappingValidation()
        {
            const string hlsl =
            @"
                texture2D t1;
                sampler s1;

                float4 main() : SV_Target
                {
                    return t1.Sample(s1, 0);
                }
            ";

            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"))
            {
                Source1 = new ColorSourceEffect()
            };

            var device = new CanvasDevice();
            var renderTarget = new CanvasRenderTarget(device, 1, 1, 96);

            // It's an error to draw with Offset mapping mode but MaxSamplerOffset not set.
            effect.Source1Mapping = SamplerCoordinateMapping.Offset;
            effect.MaxSamplerOffset = 0;

            Utils.AssertThrowsException<ArgumentException>(() => 
            {
                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawImage(effect);
                }
            }, "When PixelShaderEffect.Source1Mapping is set to Offset, MaxSamplerOffset should also be set.");

            // Also an error to draw with MaxSamplerOffset but no Offset mapping mode.
            effect.Source1Mapping = SamplerCoordinateMapping.OneToOne;
            effect.MaxSamplerOffset = 1;

            Utils.AssertThrowsException<ArgumentException>(() => 
            {
                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawImage(effect);
                }
            }, "When PixelShaderEffect.MaxSamplerOffset is set, at least one source should be using SamplerCoordinateMapping.Offset.");

            // Ok if we have both.
            effect.Source1Mapping = SamplerCoordinateMapping.Offset;
            effect.MaxSamplerOffset = 1;

            using (var ds = renderTarget.CreateDrawingSession())
            {
                ds.DrawImage(effect);
            }
        }


#if WINDOWS_UWP

        [TestMethod]
        public void PixelShaderEffect_ShaderReflectionSetsCoordinateMappingDefaults()
        {
            const string hlsl =
            @"
                texture2D t0;
                sampler s0;

                texture2D t1;
                sampler s1;

                texture2D t2;
                sampler s2;

                texture2D t3;
                sampler s3;

                float4 main(float4 texcoord0 : TEXCOORD0, 
                            float4 texcoord1 : TEXCOORD1, 
                            float4 texcoord2 : TEXCOORD2, 
                            float4 texcoord3 : TEXCOORD3) : SV_Target
                {
                    return t0.Sample(s0, texcoord0) +
                           t1.Sample(s1, texcoord1) +
                           t2.Sample(s2, texcoord2) +
                           t3.Sample(s3, texcoord3);
                }

                export float4 ShaderLinkingFunction(float4 input0    : INPUT0, 
                                                    float4 texcoord1 : TEXCOORD1, 
                                                    float4 texcoord2 : TEXCOORD2, 
                                                    float4 input3    : INPUT3)
                {
                    return input0 +
                           t1.Sample(s1, texcoord1) +
                           t2.Sample(s2, texcoord2) +
                           input3;
                }
            ";

            // If we compile without support for shader linking, all mappings default to Unknown.
            var effect = new PixelShaderEffect(ShaderCompiler.CompileShader(hlsl, "ps_4_0"));

            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source1Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source2Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source3Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source4Mapping);

            // But if we include a shader linking function, reflection now has enough
            // info to detect which inputs are simple. These are set to OneToOne mapping.
            effect = new PixelShaderEffect(ShaderCompiler.CompileShaderAndEmbedLinkingFunction(hlsl));

            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source1Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source2Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.Unknown, effect.Source3Mapping);
            Assert.AreEqual(SamplerCoordinateMapping.OneToOne, effect.Source4Mapping);
        }

#endif
    }
}
