// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Linq;
using System.Collections.Generic;
using System.Reflection;
using Windows.Foundation;
using Windows.UI;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;

#if WINDOWS_UWP
using Windows.Graphics.Effects;
using System.Numerics;
#else
using Microsoft.Graphics.Canvas.DirectX;
using Microsoft.Graphics.Canvas.Numerics;
#endif

using NativeComponent;

namespace test.managed
{
    [TestClass]
    public class EffectTests
    {
        [TestMethod]
        public void ReflectOverAllEffects()
        {
            var assembly = typeof(GaussianBlurEffect).GetTypeInfo().Assembly;

            var effectTypes = from type in assembly.DefinedTypes
                              where type.ImplementedInterfaces.Contains(typeof(IGraphicsEffect))
                              where type.AsType() != typeof(PixelShaderEffect)
                              select type;

            var device = new CanvasDevice();
            var device2 = new CanvasDevice();

            foreach (var effectType in effectTypes)
            {
                IGraphicsEffect effect = (IGraphicsEffect)Activator.CreateInstance(effectType.AsType());

                // Test an un-realized effect instance.
                TestEffectSources(effectType, effect, null);
                TestEffectProperties(effectType, effect);

                // Realize the effect (creating a backing D2D effect instance), then test it again.
                var initialSourceImage = RealizeEffect(device, effectType, effect);

                TestEffectSources(effectType, effect, initialSourceImage);
                TestEffectProperties(effectType, effect);

                // Test that interop can successfully transfer property values in both directions.
                TestEffectInterop(device, device2, effectType, effect);
            }
        }


        static void TestEffectSources(TypeInfo effectType, IGraphicsEffect effect, ICanvasImage initialSourceImage)
        {
            var sourceProperties = (from property in effectType.DeclaredProperties
                                    where property.PropertyType == typeof(IGraphicsEffectSource)
                                    select property).ToList();

            if (!sourceProperties.Any())
                return;

            // Should have the same number of strongly typed properties as the effect has sources.
            Assert.AreEqual(sourceProperties.Count, EffectAccessor.GetSourceCount(effect));

            // Initial source values should all be as expected.
            for (int i = 0; i < EffectAccessor.GetSourceCount(effect); i++)
            {
                Assert.AreEqual(EffectAccessor.GetSource(effect, i), initialSourceImage);
                Assert.AreEqual(sourceProperties[i].GetValue(effect), initialSourceImage);
            }

            var testValue1 = new GaussianBlurEffect() { Source = new ColorSourceEffect() };
            var testValue2 = new ShadowEffect() { Source = new ColorSourceEffect() };

            var whichIndexIsProperty = new List<int>();

            // Changing strongly typed properties should change the sources reported by IGraphicsEffectD2D1Interop.
            for (int i = 0; i < EffectAccessor.GetSourceCount(effect); i++)
            {
                // Change a property value, and see which source changes.
                sourceProperties[i].SetValue(effect, testValue1);
                Assert.AreSame(testValue1, sourceProperties[i].GetValue(effect));

                int whichIndexIsThis = 0;
                
                while (EffectAccessor.GetSource(effect, whichIndexIsThis) != testValue1)
                {
                    whichIndexIsThis++;
                    Assert.IsTrue(whichIndexIsThis < EffectAccessor.GetSourceCount(effect));
                }

                whichIndexIsProperty.Add(whichIndexIsThis);

                // Change the same property value again, and make sure the same source changes.
                sourceProperties[i].SetValue(effect, testValue2);
                Assert.AreSame(testValue2, sourceProperties[i].GetValue(effect));
                Assert.AreSame(testValue2, EffectAccessor.GetSource(effect, whichIndexIsThis));

                // Change the property value to null.
                sourceProperties[i].SetValue(effect, null);
                Assert.IsNull(sourceProperties[i].GetValue(effect));
                Assert.IsNull(EffectAccessor.GetSource(effect, whichIndexIsThis));
            }

            // Should not have any duplicate property mappings.
            Assert.AreEqual(whichIndexIsProperty.Count, whichIndexIsProperty.Distinct().Count());
        }


        static void TestEffectProperties(TypeInfo effectType, IGraphicsEffect effect)
        {
            var properties = GetEffectProperties(effectType).ToList();

            IList<object> effectProperties = new ViewIndexerAsList<object>(() => EffectAccessor.GetPropertyCount(effect),
                                                                           i => EffectAccessor.GetProperty(effect, i));

            FilterOutCustomizedEffectProperties(effectType.AsType(), ref properties, ref effectProperties);

            // Should have the same number of strongly typed properties as elements in the properties collection.
            Assert.AreEqual(properties.Count, effectProperties.Count);

            // Store the initial property values.
            var initialValues = effectProperties.ToList();

            var whichIndexIsProperty = new List<int>();

            // Changing strongly typed properties should change the properties collection.
            for (int i = 0; i < effectProperties.Count; i++)
            {
                object testValue1 = GetArbitraryTestValue(properties[i].PropertyType, true);
                object testValue2 = GetArbitraryTestValue(properties[i].PropertyType, false);

                // Change a property value, and see which collection properties match the result.
                properties[i].SetValue(effect, testValue1);
                AssertPropertyValuesAreEqual(testValue1, properties[i].GetValue(effect));

                var matches1 = (from j in Enumerable.Range(0, effectProperties.Count)
                                where BoxedValuesAreEqual(effectProperties[j], Box(testValue1, properties[i]), properties[i])
                                select j).ToList();

                // Change the same property to a different value, and see which collection properties match now.
                properties[i].SetValue(effect, testValue2);
                AssertPropertyValuesAreEqual(testValue2, properties[i].GetValue(effect));

                var matches2 = (from j in Enumerable.Range(0, effectProperties.Count)
                                where BoxedValuesAreEqual(effectProperties[j], Box(testValue2, properties[i]), properties[i])
                                select j).ToList();

                // There should be one and only one property that matched both times. If not,
                // either we don't have 1 <-> 1 mapping between strongly typed properties and
                // collection indices, or something went wrong during the boxing type conversions.
                var intersection = matches1.Intersect(matches2);
                Assert.AreEqual(1, intersection.Count());
                int whichIndexIsThis = intersection.Single();
                whichIndexIsProperty.Add(whichIndexIsThis);

                // Change the property value back to its initial state.
                var unboxedValue = Unbox(initialValues[whichIndexIsThis], properties[i]);
                properties[i].SetValue(effect, unboxedValue);
                AssertPropertyValuesAreEqual(unboxedValue, properties[i].GetValue(effect));
                Assert.IsTrue(BoxedValuesAreEqual(initialValues[whichIndexIsThis], effectProperties[whichIndexIsThis], properties[i]));

                // Validate that IGraphicsEffectD2D1Interop agrees with what we think the type and index of this property is.
                int mappingIndex;
                EffectPropertyMapping mapping;

                EffectAccessor.GetNamedPropertyMapping(effect, properties[i].Name, out mappingIndex, out mapping);

                int expectedMappingIndex = whichIndexIsThis;

                if (effectProperties is FilteredViewOfList<object>)
                    expectedMappingIndex = ((FilteredViewOfList<object>)effectProperties).GetOriginalIndex(expectedMappingIndex);

                Assert.AreEqual(expectedMappingIndex, mappingIndex);

                var expectedMapping = GetExpectedPropertyMapping(properties[i], effectProperties[whichIndexIsThis]);
                Assert.AreEqual(expectedMapping, mapping);
            }

            // Should not have any duplicate property mappings.
            Assert.AreEqual(whichIndexIsProperty.Count, whichIndexIsProperty.Distinct().Count());
        }


        static ICanvasImage RealizeEffect(CanvasDevice device, TypeInfo effectType, IGraphicsEffect effect)
        {
            var dummySourceImage = new CanvasCommandList(device);

            // We can't realize an effect with invalid inputs, so must first set them all to something reasonable.
            foreach (var sourceProperty in effectType.DeclaredProperties.Where(p => p.PropertyType == typeof(IGraphicsEffectSource)))
            {
                sourceProperty.SetValue(effect, dummySourceImage);
            }

            // Add one image to any variable size Sources collection properties.
            foreach (var sourcesProperty in effectType.DeclaredProperties.Where(p => p.Name == "Sources"))
            {
                var sources = sourcesProperty.GetValue(effect) as IList<IGraphicsEffectSource>;

                sources.Clear();
                sources.Add(dummySourceImage);
            }

            EffectAccessor.RealizeEffect(device, effect);

            return dummySourceImage;
        }


        static void TestEffectInterop(CanvasDevice device, CanvasDevice device2, TypeInfo effectType, IGraphicsEffect realizedEffect)
        {
            // One Win2D effect is backed by a realized ID2D1Effect instance, the other is not.
            // Their property values should be the same either way.
            IGraphicsEffect unrealizedEffect = (IGraphicsEffect)Activator.CreateInstance(effectType.AsType());

            PropertyValuesShouldMatch(effectType, realizedEffect, unrealizedEffect);

            // Create a new ID2D1Effect instance, then wrap a Win2D effect around it.
            // We should get the same default property values either way.
            IGraphicsEffect wrappedEffect = EffectAccessor.CreateThenWrapNewEffectOfSameType(device, realizedEffect);

            // Special case: Win2D intentionally defaults to a different DpiCompensationEffect.BorderMode vs. D2D.
            if (wrappedEffect is DpiCompensationEffect)
            {
                var dpiCompensationEffect = wrappedEffect as DpiCompensationEffect;
                Assert.AreEqual(EffectBorderMode.Soft, dpiCompensationEffect.BorderMode);
                dpiCompensationEffect.BorderMode = EffectBorderMode.Hard;
            }

            PropertyValuesShouldMatch(effectType, unrealizedEffect, wrappedEffect);

            // Re-realize the new effect onto a second device.
            // All property values should be successfully transferred across.
            RealizeEffect(device2, effectType, wrappedEffect);

            PropertyValuesShouldMatch(effectType, unrealizedEffect, wrappedEffect);
        }


        static void PropertyValuesShouldMatch(TypeInfo effectType, IGraphicsEffect effect1, IGraphicsEffect effect2)
        {
            foreach (var property in GetEffectProperties(effectType))
            {
                object value1 = property.GetValue(effect1);
                object value2 = property.GetValue(effect2);

                AssertPropertyValuesAreEqual(value1, value2);
            }
        }


        static IEnumerable<PropertyInfo> GetEffectProperties(TypeInfo effectType)
        {
            return from property in effectType.DeclaredProperties
                   where property.Name != "Name"
                   where property.Name != "Sources"
                   where property.PropertyType != typeof(IGraphicsEffectSource)
                   select property;
        }


        static object Box(object value, PropertyInfo property)
        {
            if (value is int ||
                value is bool ||
                value is float[])
            {
                return value;
            }
            else if (value is float)
            {
                if (NeedsRadianToDegreeConversion(property))
                {
                    return (float)value * 180f / (float)Math.PI;
                }
                else
                {
                    return value;
                }
            }
            else if (value is Enum)
            {
                return (uint)(int)value;
            }
            else if (value is Vector2)
            {
                var v = (Vector2)value;

                return new float[] { v.X, v.Y };
            }
            else if (value is Vector3)
            {
                var v = (Vector3)value;

                return new float[] { v.X, v.Y, v.Z };
            }
            else if (value is Vector4)
            {
                var v = (Vector4)value;

                return new float[] { v.X, v.Y, v.Z, v.W };
            }
            else if (value is Matrix3x2)
            {
                var m = (Matrix3x2)value;

                return new float[]
                { 
                    m.M11, m.M12, 
                    m.M21, m.M22, 
                    m.M31, m.M32 
                };
            }
            else if (value is Matrix4x4)
            {
                var m = (Matrix4x4)value;

                return new float[]
                { 
                    m.M11, m.M12, m.M13, m.M14, 
                    m.M21, m.M22, m.M23, m.M24, 
                    m.M31, m.M32, m.M33, m.M34, 
                    m.M41, m.M42, m.M43, m.M44, 
                };
            }
            else if (value is Matrix5x4)
            {
                var m = (Matrix5x4)value;

                return new float[]
                { 
                    m.M11, m.M12, m.M13, m.M14, 
                    m.M21, m.M22, m.M23, m.M24, 
                    m.M31, m.M32, m.M33, m.M34, 
                    m.M41, m.M42, m.M43, m.M44, 
                    m.M51, m.M52, m.M53, m.M54, 
                };
            }
            else if (value is Rect)
            {
                var r = (Rect)value;

                return new float[]
                { 
                    (float)r.Left,
                    (float)r.Top,
                    (float)r.Right,
                    (float)r.Bottom,
                };
            }
            else if (value is Color)
            {
                var c = (Color)value;

                return new float[]
                { 
                    (float)c.R / 255,
                    (float)c.G / 255,
                    (float)c.B / 255,
                    (float)c.A / 255,
                };
            }
            else
            {
                throw new NotSupportedException("Unsupported Box type " + value.GetType());
            }
        }

        
        static object Unbox(object value, PropertyInfo property)
        {
            Type type = property.PropertyType;

            if (type == typeof(int) ||
                type == typeof(bool) ||
                type == typeof(float[]))
            {
                return value;
            }
            else if (type == typeof(float))
            {
                if (NeedsRadianToDegreeConversion(property))
                {
                    return (float)value * (float)Math.PI / 180f;
                }
                else
                {
                    return value;
                }
            }
            else if (type.GetTypeInfo().IsEnum)
            {
                return GetEnumValues(type).GetValue((int)(uint)value);
            }
            else if (type == typeof(Vector2))
            {
                var a = (float[])value;
                Assert.AreEqual(2, a.Length);

                return new Vector2
                { 
                    X = a[0], 
                    Y = a[1], 
                };
            }
            else if (type == typeof(Vector3))
            {
                var a = (float[])value;
                Assert.AreEqual(3, a.Length);

                return new Vector3
                {
                    X = a[0],
                    Y = a[1],
                    Z = a[2],
                };
            }
            else if (type == typeof(Vector4))
            {
                var a = (float[])value;
                Assert.AreEqual(4, a.Length);

                return new Vector4
                {
                    X = a[0],
                    Y = a[1],
                    Z = a[2],
                    W = a[3],
                };
            }
            else if (type == typeof(Matrix3x2))
            {
                var a = (float[])value;
                Assert.AreEqual(6, a.Length);

                return new Matrix3x2
                { 
                    M11 = a[0], M12 = a[1], 
                    M21 = a[2], M22 = a[3], 
                    M31 = a[4], M32 = a[5],
                };
            }
            else if (type == typeof(Matrix4x4))
            {
                var a = (float[])value;
                Assert.AreEqual(16, a.Length);

                return new Matrix4x4
                { 
                    M11 = a[0],  M12 = a[1],  M13 = a[2],  M14 = a[3],
                    M21 = a[4],  M22 = a[5],  M23 = a[6],  M24 = a[7],
                    M31 = a[8],  M32 = a[9],  M33 = a[10], M34 = a[11],
                    M41 = a[12], M42 = a[13], M43 = a[14], M44 = a[15],
                };
            }
            else if (type == typeof(Matrix5x4))
            {
                var a = (float[])value;
                Assert.AreEqual(20, a.Length);

                return new Matrix5x4
                { 
                    M11 = a[0],  M12 = a[1],  M13 = a[2],  M14 = a[3],
                    M21 = a[4],  M22 = a[5],  M23 = a[6],  M24 = a[7],
                    M31 = a[8],  M32 = a[9],  M33 = a[10], M34 = a[11],
                    M41 = a[12], M42 = a[13], M43 = a[14], M44 = a[15],
                    M51 = a[16], M52 = a[17], M53 = a[18], M54 = a[19],
                };
            }
            else if (type == typeof(Rect))
            {
                var a = (float[])value;
                Assert.AreEqual(4, a.Length);

                return new Rect(new Point(a[0], a[1]), 
                                new Point(a[2], a[3]));
            }
            else if (type == typeof(Color))
            {
                var a = ConvertRgbToRgba((float[])value);
                Assert.AreEqual(4, a.Length);

                return Color.FromArgb((byte)(a[3] * 255),
                                      (byte)(a[0] * 255),
                                      (byte)(a[1] * 255),
                                      (byte)(a[2] * 255));
            }
            else
            {
                throw new NotSupportedException("Unsupported Unbox type " + type);
            }
        }


        static bool BoxedValuesAreEqual(object value1, object value2, PropertyInfo property)
        {
            if (value1.GetType() != value2.GetType())
            {
                return false;
            }

            if (value1 is int || 
                value1 is uint ||
                value1 is bool)
            {
                return value1.Equals(value2);
            }
            else if (value1 is float)
            {
                return Math.Abs((float)value1 - (float)value2) < 0.00001;
            }
            else if (value1 is float[])
            {
                float[] a1 = (float[])value1;
                float[] a2 = (float[])value2;

                if (property.PropertyType == typeof(Color))
                {
                    a1 = ConvertRgbToRgba(a1);
                    a2 = ConvertRgbToRgba(a2);
                }

                return a1.SequenceEqual(a2);
            }
            else
            {
                throw new NotSupportedException("Unsupported BoxedValuesAreEqual type " + value1.GetType());
            }
        }


        static void AssertPropertyValuesAreEqual(object value1, object value2)
        {
            if (value1 is float[] && value2 is float[])
            {
                Assert.IsTrue((value1 as float[]).SequenceEqual(value2 as float[]));
            }
            else if (value1 is float)
            {
                Assert.IsTrue(Math.Abs((float)value1 - (float)value2) < 0.000001);
            }
            else
            {
                Assert.AreEqual(value1, value2);
            }
        }


        static float[] ConvertRgbToRgba(float[] value)
        {
            if (value.Length == 3)
            {
                return value.Concat(new float[] { 1 }).ToArray();
            }
            else
            {
                return value;
            }
        }


        static float[] ConvertRgbaToRgb(float[] value)
        {
            return value.Take(3).ToArray();
        }


        static bool NeedsRadianToDegreeConversion(PropertyInfo property)
        {
            string[] anglePropertyNames =
            {
                "Angle",
                "Azimuth",
                "Elevation",
                "LimitingConeAngle",
            };

            return anglePropertyNames.Contains(property.Name);
        }


        static EffectPropertyMapping GetExpectedPropertyMapping(PropertyInfo property, object propertyValue)
        {
            if (NeedsRadianToDegreeConversion(property))
            {
                return EffectPropertyMapping.RadiansToDegrees;
            }
            else if (property.PropertyType == typeof(Rect))
            {
                return EffectPropertyMapping.RectToVector4;
            }
            else if (property.PropertyType == typeof(Color))
            {
                return ((float[])propertyValue).Length == 3 ? EffectPropertyMapping.ColorToVector3 : 
                                                              EffectPropertyMapping.ColorToVector4;
            }
            else
            {
                return EffectPropertyMapping.Direct;
            }
        }


        static object GetArbitraryTestValue(Type type, bool whichOne)
        {
            if (type == typeof(int))
            {
                return whichOne ? 2 : 7;
            }
            else if (type == typeof(float))
            {
                return whichOne ? 0.5f : 0.75f;
            }
            else if (type == typeof(bool))
            {
                return whichOne;
            }
            else if (type.GetTypeInfo().IsEnum)
            {
                return GetEnumValues(type).GetValue(whichOne ? 0 : 1);
            }
            else if (type == typeof(Vector2))
            {
                return whichOne ? new Vector2 { X = 0.25f, Y = 0.75f } :
                                  new Vector2 { X = 0.5f, Y = 0.125f };
            }
            else if (type == typeof(Vector3))
            {
                return whichOne ? new Vector3 { X = 1, Y = 2, Z = 3 } :
                                  new Vector3 { X = 4, Y = 5, Z = 6 };
            }
            else if (type == typeof(Vector4))
            {
                return whichOne ? new Vector4 { X = 1, Y = 2, Z = 3, W = 4 } :
                                  new Vector4 { X = 5, Y = 6, Z = 7, W = 8 };
            }
            else if (type == typeof(Matrix3x2))
            {
                return whichOne ? new Matrix3x2
                                  { 
                                      M11 = 1, M12 = 2, 
                                      M21 = 3, M22 = 4,  
                                      M31 = 5, M32 = 6  
                                  } :
                                  new Matrix3x2 
                                  { 
                                      M11 = 7,  M12 = 8, 
                                      M21 = 9,  M22 = 10, 
                                      M31 = 11, M32 = 12 
                                  };
            }
            else if (type == typeof(Matrix4x4))
            {
                return whichOne ? new Matrix4x4 
                                  { 
                                      M11 = 1,  M12 = 2,  M13 = 3,  M14 = 4,
                                      M21 = 5,  M22 = 6,  M23 = 7,  M24 = 8,
                                      M31 = 9,  M32 = 10, M33 = 11, M34 = 12,
                                      M41 = 13, M42 = 14, M43 = 15, M44 = 16
                                  } :
                                  new Matrix4x4 
                                  { 
                                      M11 = 11, M12 = 12, M13 = 13, M14 = 14,
                                      M21 = 15, M22 = 16, M23 = 17, M24 = 18,
                                      M31 = 19, M32 = 20, M33 = 21, M34 = 22,
                                      M41 = 23, M42 = 24, M43 = 25, M44 = 26
                                  };
            }
            else if (type == typeof(Matrix5x4))
            {
                return whichOne ? new Matrix5x4 
                                  { 
                                      M11 = 1,  M12 = 2,  M13 = 3,  M14 = 4,
                                      M21 = 5,  M22 = 6,  M23 = 7,  M24 = 8,
                                      M31 = 9,  M32 = 10, M33 = 11, M34 = 12,
                                      M41 = 13, M42 = 14, M43 = 15, M44 = 16,
                                      M51 = 17, M52 = 18, M53 = 19, M54 = 20
                                  } :
                                  new Matrix5x4 
                                  { 
                                      M11 = 11, M12 = 12, M13 = 13, M14 = 14,
                                      M21 = 15, M22 = 16, M23 = 17, M24 = 18,
                                      M31 = 19, M32 = 20, M33 = 21, M34 = 22,
                                      M41 = 23, M42 = 24, M43 = 25, M44 = 26,
                                      M51 = 27, M52 = 28, M53 = 29, M54 = 30
                                  };
            }
            else if (type == typeof(Rect))
            {
                return whichOne ? new Rect(1, 2, 3, 4) :
                                  new Rect(10, 20, 5, 6);
            }
            else if (type == typeof(Color))
            {
                return whichOne ? Colors.CornflowerBlue : Colors.Crimson;
            }
            else if (type == typeof(float[]))
            {
                return whichOne ? new float[] { 1, 2, 3 } :
                                  new float[] { 4, 5, 6, 7, 8, 9 };
            }
            else
            {
                throw new NotSupportedException("Unsupported GetArbitraryTestValue type " + type);
            }
        }


        // Replacement for Enum.GetValues, to work around VS2015 .NET Native reflection bug.
        static object[] GetEnumValues(Type type)
        {
            Assert.IsTrue(type.GetTypeInfo().IsEnum);

            var values = from field in type.GetTypeInfo().DeclaredFields
                         where field.IsStatic
                         orderby field.GetValue(null)
                         select field.GetValue(null);

            return values.ToArray();
        }


        static void FilterOutCustomizedEffectProperties(Type effectType, ref List<PropertyInfo> properties, ref IList<object> effectProperties)
        {
            // Customized properties that our general purpose reflection based property test won't understand.
            string[] propertiesToRemove;
            int[] indexMapping;

            if (effectType == typeof(ArithmeticCompositeEffect))
            {
                // ArithmeticCompositeEffect has strange customized properties.
                // Win2D exposes what D2D treats as a single Vector4 as 4 separate float properties. 
                propertiesToRemove = new string[]
                {
                    "MultiplyAmount",
                    "Source1Amount",
                    "Source2Amount",
                    "Offset"
                };

                indexMapping = new int[] { 1 };
            }
            else if (effectType == typeof(ColorMatrixEffect))
            {
                // ColorMatrixEffect.AlphaMode has special logic to remap enum values between WinRT and D2D.
                propertiesToRemove = new string[] { "AlphaMode", };
                indexMapping = new int[] { 0, 2 };
            }
#if WINDOWS_UWP
            else if (effectType == typeof(SepiaEffect))
            {
                // SepiaEffect.AlphaMode has special logic to remap enum values between WinRT and D2D.
                propertiesToRemove = new string[] { "AlphaMode", };
                indexMapping = new int[] { 0 };
            }
            else if (effectType == typeof(EdgeDetectionEffect))
            {
                // EdgeDetectionEffect.AlphaMode has special logic to remap enum values between WinRT and D2D.
                propertiesToRemove = new string[] { "AlphaMode", };
                indexMapping = new int[] { 0, 1, 2, 3 };
            }
            else if (effectType == typeof(HighlightsAndShadowsEffect))
            {
                // HighlightsAndShadowsEffect.SourceIsLinearGamma projects an enum value as a bool.
                propertiesToRemove = new string[] { "SourceIsLinearGamma", };
                indexMapping = new int[] { 0, 1, 2, 4 };
            }
#endif  // WINDOWS_UWP
            else
            {
                // Other effects do not need special filtering.
                return;
            }

            // Hide the customized properties, so ReflectOverAllEffects test won't see them.
            properties = properties.Where(p => !propertiesToRemove.Contains(p.Name)).ToList();

            effectProperties = new FilteredViewOfList<object>(effectProperties, indexMapping);
        }


        class FilteredViewOfList<T> : IList<T>
        {
            IList<T> underlyingList; 
            IList<int> indexMapping;

            public FilteredViewOfList(IList<T> underlyingList, IList<int> indexMapping)
            {
                this.underlyingList = underlyingList;
                this.indexMapping = indexMapping;
            }

            public int Count
            {
                get { return indexMapping.Count; }
            }

            public T this[int index]
            {
                get { return underlyingList[indexMapping[index]]; }
                set { underlyingList[indexMapping[index]] = value; }
            }

            public void CopyTo(T[] array, int arrayIndex)
            {
                for (int i = 0; i < Count; i++)
                {
                    array[arrayIndex + i] = this[i];
                }
            }

            public int GetOriginalIndex(int index)
            {
                return indexMapping[index];
            }

            public void Add(T item) { throw new NotImplementedException(); }
            public void Clear() { throw new NotImplementedException(); }
            public bool Contains(T item) { throw new NotImplementedException(); }
            public int IndexOf(T item) { throw new NotImplementedException(); }
            public void Insert(int index, T item) { throw new NotImplementedException(); }
            public bool IsReadOnly { get { throw new NotImplementedException(); } }
            public bool Remove(T item) { throw new NotImplementedException(); }
            public void RemoveAt(int index) { throw new NotImplementedException(); }
            public IEnumerator<T> GetEnumerator() { throw new NotImplementedException(); }
            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() { throw new NotImplementedException(); }
        }


        class ViewIndexerAsList<T> : IList<T>
        {
            Func<int> getCount;
            Func<int, T> getItem;

            public ViewIndexerAsList(Func<int> getCount, Func<int, T> getItem)
            {
                this.getCount = getCount;
                this.getItem = getItem;
            }

            public int Count
            {
                get { return getCount(); }
            }

            public T this[int index]
            {
                get { return getItem(index); }
                set { throw new NotImplementedException(); }
            }

            public void CopyTo(T[] array, int arrayIndex)
            {
                for (int i = 0; i < Count; i++)
                {
                    array[arrayIndex + i] = getItem(i);
                }
            }

            public void Add(T item) { throw new NotImplementedException(); }
            public void Clear() { throw new NotImplementedException(); }
            public bool Contains(T item) { throw new NotImplementedException(); }
            public int IndexOf(T item) { throw new NotImplementedException(); }
            public void Insert(int index, T item) { throw new NotImplementedException(); }
            public bool IsReadOnly { get { throw new NotImplementedException(); } }
            public bool Remove(T item) { throw new NotImplementedException(); }
            public void RemoveAt(int index) { throw new NotImplementedException(); }
            public IEnumerator<T> GetEnumerator() { throw new NotImplementedException(); }
            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() { throw new NotImplementedException(); }
        }

        
        [TestMethod]
        public void ArithmeticCompositeEffectCustomizations()
        {
            var effect = new ArithmeticCompositeEffect();

            // Verify defaults.
            Assert.AreEqual(1.0f, effect.MultiplyAmount);
            Assert.AreEqual(0.0f, effect.Source1Amount);
            Assert.AreEqual(0.0f, effect.Source2Amount);
            Assert.AreEqual(0.0f, effect.Offset);

            Assert.IsTrue(((float[])EffectAccessor.GetProperty(effect, 0)).SequenceEqual(new float[] { 1, 0, 0, 0 }));

            // Change properties one at a time, and verify that the boxed value changes to match.
            effect.MultiplyAmount = 23;
            Assert.IsTrue(((float[])EffectAccessor.GetProperty(effect, 0)).SequenceEqual(new float[] { 23, 0, 0, 0 }));

            effect.Source1Amount = 42;
            Assert.IsTrue(((float[])EffectAccessor.GetProperty(effect, 0)).SequenceEqual(new float[] { 23, 42, 0, 0 }));

            effect.Source2Amount = -1;
            Assert.IsTrue(((float[])EffectAccessor.GetProperty(effect, 0)).SequenceEqual(new float[] { 23, 42, -1, 0 }));

            effect.Offset = 100;
            Assert.IsTrue(((float[])EffectAccessor.GetProperty(effect, 0)).SequenceEqual(new float[] { 23, 42, -1, 100 }));

            // Validate that IGraphicsEffectD2D1Interop reports the right customizations.
            int index;
            EffectPropertyMapping mapping;

            EffectAccessor.GetNamedPropertyMapping(effect, "MultiplyAmount", out index, out mapping);
            Assert.AreEqual(0, index);
            Assert.AreEqual(EffectPropertyMapping.VectorX, mapping);

            EffectAccessor.GetNamedPropertyMapping(effect, "Source1Amount", out index, out mapping);
            Assert.AreEqual(0, index);
            Assert.AreEqual(EffectPropertyMapping.VectorY, mapping);

            EffectAccessor.GetNamedPropertyMapping(effect, "Source2Amount", out index, out mapping);
            Assert.AreEqual(0, index);
            Assert.AreEqual(EffectPropertyMapping.VectorZ, mapping);

            EffectAccessor.GetNamedPropertyMapping(effect, "Offset", out index, out mapping);
            Assert.AreEqual(0, index);
            Assert.AreEqual(EffectPropertyMapping.VectorW, mapping);
        }


        const uint D2D1_ALPHA_MODE_PREMULTIPLIED = 1;
        const uint D2D1_ALPHA_MODE_STRAIGHT = 2;


        [TestMethod]
        public void ColorMatrixEffectCustomizations()
        {
            var effect = new ColorMatrixEffect();

            // Verify defaults.
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 1));

            // Change the property, and verify that the boxed value changes to match.
            effect.AlphaMode = CanvasAlphaMode.Straight;
            Assert.AreEqual(D2D1_ALPHA_MODE_STRAIGHT, EffectAccessor.GetProperty(effect, 1));

            effect.AlphaMode = CanvasAlphaMode.Premultiplied;
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 1));

            // Verify unsupported value throws.
            Assert.ThrowsException<ArgumentException>(() => { effect.AlphaMode = CanvasAlphaMode.Ignore; });
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);

            // Validate that IGraphicsEffectD2D1Interop reports the right customizations.
            int index;
            EffectPropertyMapping mapping;

            EffectAccessor.GetNamedPropertyMapping(effect, "AlphaMode", out index, out mapping);
            Assert.AreEqual(1, index);
            Assert.AreEqual(EffectPropertyMapping.ColorMatrixAlphaMode, mapping);
        }


#if WINDOWS_UWP


        [TestMethod]
        public void SepiaEffectCustomizations()
        {
            var effect = new SepiaEffect();

            // Verify defaults.
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 1));

            // Change the property, and verify that the boxed value changes to match.
            effect.AlphaMode = CanvasAlphaMode.Straight;
            Assert.AreEqual(D2D1_ALPHA_MODE_STRAIGHT, EffectAccessor.GetProperty(effect, 1));

            effect.AlphaMode = CanvasAlphaMode.Premultiplied;
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 1));

            // Verify unsupported value throws.
            Assert.ThrowsException<ArgumentException>(() => { effect.AlphaMode = CanvasAlphaMode.Ignore; });
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);

            // Validate that IGraphicsEffectD2D1Interop reports the right customizations.
            int index;
            EffectPropertyMapping mapping;

            EffectAccessor.GetNamedPropertyMapping(effect, "AlphaMode", out index, out mapping);
            Assert.AreEqual(1, index);
            Assert.AreEqual(EffectPropertyMapping.ColorMatrixAlphaMode, mapping);
        }


        [TestMethod]
        public void EdgeDetectionEffectCustomizations()
        {
            var effect = new EdgeDetectionEffect();

            // Verify defaults.
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 4));

            // Change the property, and verify that the boxed value changes to match.
            effect.AlphaMode = CanvasAlphaMode.Straight;
            Assert.AreEqual(D2D1_ALPHA_MODE_STRAIGHT, EffectAccessor.GetProperty(effect, 4));

            effect.AlphaMode = CanvasAlphaMode.Premultiplied;
            Assert.AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, EffectAccessor.GetProperty(effect, 4));

            // Verify unsupported value throws.
            Assert.ThrowsException<ArgumentException>(() => { effect.AlphaMode = CanvasAlphaMode.Ignore; });
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, effect.AlphaMode);

            // Validate that IGraphicsEffectD2D1Interop reports the right customizations.
            int index;
            EffectPropertyMapping mapping;

            EffectAccessor.GetNamedPropertyMapping(effect, "AlphaMode", out index, out mapping);
            Assert.AreEqual(4, index);
            Assert.AreEqual(EffectPropertyMapping.ColorMatrixAlphaMode, mapping);
        }


        [TestMethod]
        public void HighlightsAndShadowsEffectCustomizations()
        {
            const uint D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR = 0;
            const uint D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB = 1;

            var effect = new HighlightsAndShadowsEffect();

            // Verify defaults.
            Assert.IsFalse(effect.SourceIsLinearGamma);
            Assert.AreEqual(D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB, EffectAccessor.GetProperty(effect, 3));

            // Change the property, and verify that the boxed value changes to match.
            effect.SourceIsLinearGamma = true;
            Assert.IsTrue(effect.SourceIsLinearGamma);
            Assert.AreEqual(D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR, EffectAccessor.GetProperty(effect, 3));

            effect.SourceIsLinearGamma = false;
            Assert.IsFalse(effect.SourceIsLinearGamma);
            Assert.AreEqual(D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB, EffectAccessor.GetProperty(effect, 3));

            // Validate that IGraphicsEffectD2D1Interop reports the right customizations.
            int index;
            EffectPropertyMapping mapping;

            EffectAccessor.GetNamedPropertyMapping(effect, "SourceIsLinearGamma", out index, out mapping);
            Assert.AreEqual(3, index);
            Assert.AreEqual(EffectPropertyMapping.Unknown, mapping);
        }


        [TestMethod]
        public void StraightenEffectDoesNotSupportHighQualityInterpolation()
        {
            var effect = new StraightenEffect();

            var supportedModes = from mode in Enum.GetValues(typeof(CanvasImageInterpolation)).Cast<CanvasImageInterpolation>()
                                 where mode != CanvasImageInterpolation.HighQualityCubic
                                 select mode;

            foreach (var interpolationMode in supportedModes)
            {
                effect.InterpolationMode = interpolationMode;
                Assert.AreEqual(interpolationMode, effect.InterpolationMode);
            }

            Assert.ThrowsException<ArgumentException>(() =>
            {
                effect.InterpolationMode = CanvasImageInterpolation.HighQualityCubic;
            });
        }


#endif  // WINDOWS_UWP


        [TestMethod]
        public void Transform3DEffectDoesNotSupportHighQualityInterpolation()
        {
            var effect = new Transform3DEffect();

            var supportedModes = from mode in Enum.GetValues(typeof(CanvasImageInterpolation)).Cast<CanvasImageInterpolation>()
                                 where mode != CanvasImageInterpolation.HighQualityCubic
                                 select mode;

            foreach (var interpolationMode in supportedModes)
            {
                effect.InterpolationMode = interpolationMode;
                Assert.AreEqual(interpolationMode, effect.InterpolationMode);
            }

            Assert.ThrowsException<ArgumentException>(() =>
            {
                effect.InterpolationMode = CanvasImageInterpolation.HighQualityCubic;
            });
        }


        [TestMethod]
        public void Transform2DEffectSupportsAllInterpolationModes()
        {
            var effect = new Transform2DEffect();

            foreach (var interpolationMode in Enum.GetValues(typeof(CanvasImageInterpolation)).Cast<CanvasImageInterpolation>())
            {
                effect.InterpolationMode = interpolationMode;
                Assert.AreEqual(interpolationMode, effect.InterpolationMode);
            }
        }


        class NotACanvasImage : IGraphicsEffectSource { }


        [TestMethod]
        public void EffectExceptionMessages()
        {
            var effect = new GaussianBlurEffect();

            using (var device = new CanvasDevice())
            using (var renderTarget = new CanvasRenderTarget(device, 1, 1, 96))
            using (var drawingSession = renderTarget.CreateDrawingSession())
            {
                // Null source.
                Utils.AssertThrowsException< ArgumentException>(
                    () => drawingSession.DrawImage(effect),  
                    "Effect source #0 is null.");

                // Null source (tree 2 deep).
                effect.Source = new GaussianBlurEffect();

                Utils.AssertThrowsException<ArgumentException>(
                    () => drawingSession.DrawImage(effect), 
                    "Effect source #0 is null.");

                // Invalid source type.
                effect.Source = new NotACanvasImage();

                Utils.AssertThrowsException<InvalidCastException>(
                    () => drawingSession.DrawImage(effect), 
                    "Effect source #0 is an unsupported type. To draw an effect using Win2D, all its sources must be Win2D ICanvasImage objects.");

                // Invalid source type (tree 2 deep).
                effect.Source = new GaussianBlurEffect { Source = new NotACanvasImage() };

                Utils.AssertThrowsException<InvalidCastException>(
                    () => drawingSession.DrawImage(effect), 
                    "Effect source #0 is an unsupported type. To draw an effect using Win2D, all its sources must be Win2D ICanvasImage objects.");

                // But I can set invalid source types as long as I don't draw with them,
                // even when the effect is previously realized.
                effect.Source = new ColorSourceEffect();
                drawingSession.DrawImage(effect);

                effect.Source = new NotACanvasImage();  // no exception

                effect.Source = new ColorSourceEffect();
                drawingSession.DrawImage(effect);

                effect.Source = new GaussianBlurEffect { Source = new NotACanvasImage() };  // no exception
            }
        }


        [TestMethod]
        public void EffectDrawFailsWhenSourceIsBitmapOnWrongDevice()
        {
            var effect = new GaussianBlurEffect();

            using (var device = new CanvasDevice())
            using (var renderTarget = new CanvasRenderTarget(device, 1, 1, 96))
            using (var drawingSession = renderTarget.CreateDrawingSession())
            using (var otherDevice = new CanvasDevice())
            using (var bitmapOnOtherDevice = new CanvasRenderTarget(otherDevice, 1, 1, 96))
            {
                // Source bitmap is on the wrong device.
                effect.Source = bitmapOnOtherDevice;

                Utils.AssertThrowsException<ArgumentException>(
                    () => drawingSession.DrawImage(effect),
                    "Effect source #0 is associated with a different device.");

                // Source is another effect, whose source is a bitmap on the wrong device.
                effect.Source = new GaussianBlurEffect { Source = bitmapOnOtherDevice };

                Utils.AssertThrowsException<ArgumentException>(
                    () => drawingSession.DrawImage(effect),
                    "Effect source #0 is associated with a different device.");
            }
        }


        [TestMethod]
        public void EffectPropertyDefaults()
        {
            // We have customised the default value of DpiCompensationEffect border mode property.
            Assert.AreEqual(EffectBorderMode.Hard, new DpiCompensationEffect().BorderMode);

            // Other effects should still have the standard D2D default value.
            Assert.AreEqual(EffectBorderMode.Soft, new GaussianBlurEffect().BorderMode);
            Assert.AreEqual(EffectBorderMode.Soft, new Transform3DEffect().BorderMode);
        }


        [TestMethod]
        public void EffectName()
        {
            var effect = new GaussianBlurEffect();
            Assert.AreEqual(string.Empty, effect.Name);

            effect.Name = "hello";
            Assert.AreEqual("hello", effect.Name);

            effect.Name = "world";
            Assert.AreEqual("world", effect.Name);

            effect.Name = string.Empty;
            Assert.AreEqual(string.Empty, effect.Name);

            Assert.ThrowsException<ArgumentNullException>(() => { effect.Name = null; });
        }
    }
}
