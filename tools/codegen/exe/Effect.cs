// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Globalization;
using System.Xml.Serialization;
using System.IO;

namespace CodeGen
{
    namespace Effects
    {
        public class EnumValue
        {
            [XmlAttribute("name")]
            public string Name { get; set; }

            [XmlAttribute("displayname")]
            public string Displayname { get; set; }

            public override bool Equals(object obj)
            {
                EnumValue value = obj as EnumValue;

                return (value != null) && (Name == value.Name);
            }

            public override int GetHashCode()
            {
                return Name.GetHashCode();
            }
        }

        public class EnumValues
        {
            public EnumValues()
            {
                Usage = UsageType.UsedByOneEffect;
                IsRepresentative = false;
            }

            [XmlIgnore]
            public Enum D2DEnum { get; set; }

            public enum UsageType
            {
                UsedByOneEffect,
                UsedByMultipleEffects
            }
            public UsageType Usage;

            public bool IsRepresentative { get; set; }

            [XmlElement("Field")]
            public List<EnumValue> FieldsList { get; set; }
        }

        public class Property
        {
            public Property()
            {
                ShouldProject = true;
            }

            [XmlAttribute("name")]
            public string Name { get; set; }

            [XmlAttribute("type")]
            public string Type { get; set; }

            [XmlAttribute("value")]
            public string Value { get; set; }

            [XmlElement("Property")]
            public List<Property> Properties { get; set; }

            // Used only for enum types
            [XmlElement("Fields")]
            public EnumValues EnumFields { get; set; }

            public string EffectName { get; set; }

            public string TypeNameIdl { get; set; }
            public string TypeNameCpp { get; set; }
            public string TypeNameBoxed { get; set; }

            public bool IsArray { get; set; }

            public bool ShouldProject { get; set; }
            public bool IsHidden { get; set; }
            public bool IsHandCoded { get; set; }

            public bool ConvertRadiansToDegrees { get; set; }

            public string NativePropertyName { get; set; }

            public string WinVer { get; set; }

            public List<string> ExcludedEnumIndexes { get; set; }
        }

        public class Input
        {
            [XmlAttribute("name")]
            public string Name { get; set; }

            [XmlAttribute("minimum")]
            public string Minimum { get; set; }

            [XmlAttribute("maximum")]
            public string Maximum { get; set; }
        }

        public class Inputs
        {
            [XmlElement("Input")]
            public List<Input> InputsList { get; set; }

            [XmlAttribute("minimum")]
            public string Minimum { get; set; }

            [XmlAttribute("maximum")]
            public string Maximum { get; set; }
        }

        [XmlRoot("Effect")]
        public class Effect
        {
            [XmlElement("Property")]
            public List<Property> Properties { get; set; }

            [XmlElement("Inputs")]
            public Inputs Inputs { get; set; }

            public string ClassName { get; set; }

            public string InterfaceName { get; set; }

            public string Uuid { get; set; }

            [XmlIgnore]
            public Overrides.XmlBindings.Effect Overrides { get; set; }
        }

        public class D2DEnum
        {
            public D2DEnum()
            {
                Enums = new SortedDictionary<int, string>();
            }

            public string Name { get; set; }
            public SortedDictionary<int, string> Enums { get; set; }
        }
    }

    public static class EffectGenerator
    {
        public static void OutputEffects(string inputEffectsDir, Dictionary<string, QualifiableType> typeDictionary, string outputPath)
        {
            string[] filePaths = Directory.GetFiles(inputEffectsDir);

            var overridesXmlData = XmlBindings.Utilities.LoadXmlData<Overrides.XmlBindings.Settings>(inputEffectsDir, "../../Settings.xml");

            List<Effects.Effect> effects = new List<Effects.Effect>();
            foreach (var xmlFilePath in filePaths)
            {
                effects.Add(ParseEffectXML(xmlFilePath));
            }

            string windowsKitPath = Environment.ExpandEnvironmentVariables(@"%WindowsSdkDir%");

            // Check if %WindowsSdkDir% path is set
            if (!Directory.Exists(windowsKitPath))
            {
                // Try the default path 
                windowsKitPath = @"C:\Program Files (x86)\Windows Kits\8.1";
                if (!Directory.Exists(windowsKitPath))
                {
                    throw new Exception(@"Missing WindowsSdkDir environment variable. Please run this application from VS command prompt");
                }
            }

            List<string> d2dHeaders = new List<string>
            {
                windowsKitPath + @"/Include/um/d2d1effects.h",
                windowsKitPath + @"/Include/um/d2d1_1.h"
            };

            AssignEffectsNamesToProperties(effects);
            DetectCommonEnums(effects);
            AssignPropertyNames(effects);

            var overrides = overridesXmlData.Namespaces.Find(namespaceElement => namespaceElement.Name == "Effects");

            List<Enum> d2dEnums = ParseD2DEffectsEnums(d2dHeaders, typeDictionary);

            // Put the known enums into typeDictionary.

            AssignD2DEnums(effects, d2dEnums);
            AssignEffectsClassNames(effects, overrides.Effects, typeDictionary);
            ResolveTypeNames(effects);
            RegisterUuids(effects);
            OverrideEnums(overrides.Enums, effects);
            GenerateOutput(effects, outputPath);
        }

        // Register effect uuids. These are generated by hashing the interface name following RFC 4122.
        private static void RegisterUuids(List<Effects.Effect> effects)
        {
            var salt = new Guid("8DEBAF20-F852-4B20-BE55-4D7EA6DE19BE");

            foreach (var effect in effects)
            {
                var name = "Microsoft.Graphics.Canvas.Effects." + effect.InterfaceName;
                var uuid = UuidHelper.GetVersion5Uuid(salt, name);
                effect.Uuid = uuid.ToString().ToUpper();
            }
        }

        public static bool IsEffectEnabled(Effects.Effect effect)
        {
            switch (effect.Properties[0].Value)
            {
                // TODO #2648: figure out how to project effects that output computation results rather than images.
                case "Histogram":
                    return false;

                default:
                    return true;
            }
        }

        private static List<Effects.Property> GetAllEffectsProperties(List<Effects.Effect> effects)
        {
            List<Effects.Property> allProperties = new List<Effects.Property>();
            foreach (var effect in effects)
            {
                foreach (var property in effect.Properties)
                {
                    allProperties.Add(property);
                }
            }
            return allProperties;
        }

        private static void AssignEffectsNamesToProperties(List<Effects.Effect> effects)
        {
            foreach (var effect in effects)
            {
                foreach (var property in effect.Properties)
                {
                    // Zero property in xml effect description corresponds to effect name
                    property.EffectName = effect.Properties[0].Value;
                }
            }
        }

        private static void OverrideEnums(List<Overrides.XmlBindings.Enum> enumsOverrides, List<Effects.Effect> effects)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                var enumOverride = enumsOverrides.Find(element => element.Name == property.TypeNameIdl);
                if (enumOverride != null)
                {
                    if (enumOverride.ProjectedNameOverride != null)
                    {
                        property.TypeNameIdl = enumOverride.ProjectedNameOverride;
                        if (enumOverride.Namespace != null)
                        {
                            property.TypeNameIdl = enumOverride.Namespace + "." + property.TypeNameIdl;
                        }
                        property.TypeNameCpp = enumOverride.ProjectedNameOverride;
                    }
                    property.ShouldProject = enumOverride.ShouldProject;
                    property.WinVer = enumOverride.WinVer;
                    foreach (var enumValue in enumOverride.Values)
                    {
                        if (!enumValue.ShouldProject && property.ExcludedEnumIndexes != null)
                        {
                            property.ExcludedEnumIndexes.Add(enumValue.Index);
                        }
                        if (enumValue.ProjectedNameOverride != null)
                        {
                            var enumToOverride = property.EnumFields.FieldsList.Find(enumEntry => enumEntry.Name == enumValue.Name);
                            if (enumToOverride != null)
                            {
                                enumToOverride.Name = enumValue.ProjectedNameOverride;
                            }
                        }
                    }
                }
            }
        }

        private static void ResolveTypeNames(List<Effects.Effect> effects)
        {
            var typeRenames = new Dictionary<string, string[]>
            {
                // D2D name                 IDL name   C++ name
                { "bool",   new string[] { "boolean", "boolean" } },
                { "int32",  new string[] { "INT32",   "int32_t" } },
                { "uint32", new string[] { "INT32",   "int32_t" } },
            };

            foreach (var property in GetAllEffectsProperties(effects))
            {
                if (property.TypeNameIdl != null)
                {
                    string xmlName = property.TypeNameIdl;

                    if (typeRenames.ContainsKey(xmlName))
                    {
                        // Specially remapped type, where D2D format XML files don't match WinRT type naming.
                        property.TypeNameIdl = typeRenames[xmlName][0];
                        property.TypeNameCpp = typeRenames[xmlName][1];
                        property.TypeNameBoxed = typeRenames[xmlName][1];
                    }
                    else if (xmlName.StartsWith("matrix") || xmlName.StartsWith("vector"))
                    {
                        if (property.Name.Contains("Rect"))
                        {
                            // D2D passes rectangle properties as float4, but we remap them to use strongly typed Rect.
                            property.TypeNameIdl = "Windows.Foundation.Rect";
                            property.TypeNameCpp = "Rect";
                        }
                        else if (property.Name.Contains("Color") && xmlName.StartsWith("vector"))
                        {
                            // D2D passes color properties as float3 or float4, but we remap them to use strongly typed Color.
                            property.TypeNameIdl = "Windows.UI.Color";
                            property.TypeNameCpp = "Color";
                        }
                        else
                        {
                            // Vector or matrix type.
                            property.TypeNameIdl = char.ToUpper(xmlName[0]) + xmlName.Substring(1);
                            property.TypeNameCpp = property.TypeNameIdl;

                            // Matrix5x4 is defined locally as part of Effects, but other math types live in the Numerics namespace.
                            if (!xmlName.Contains("5x4"))
                            {
                                property.TypeNameIdl = "NUMERICS." + property.TypeNameIdl;
                                property.TypeNameCpp = "Numerics::" + property.TypeNameCpp;
                            }
                        }

                        // Convert eg. "matrix3x2" to 6, or "vector3" to 3.
                        var sizeSuffix = xmlName.SkipWhile(char.IsLetter).ToArray();
                        var sizeElements = new string(sizeSuffix).Split('x').Select(int.Parse);
                        var size = sizeElements.Aggregate((a, b) => a * b);

                        property.TypeNameBoxed = "float[" + size + "]";
                    }
                    else if (xmlName == "blob")
                    {
                        // The D2D "blob" type projects as an array of floats.
                        property.TypeNameIdl = "float";
                        property.TypeNameCpp = "float";
                        property.TypeNameBoxed = "float";
                        property.IsArray = true;
                    }
                    else
                    {
                        // Any other type.
                        property.TypeNameCpp = xmlName;
                        property.TypeNameBoxed = xmlName;

                        // Enums are internally stored as uints.
                        if (property.Type == "enum")
                        {
                            property.TypeNameBoxed = "uint32_t";
                        }
                    }
                }
            }
        }

        // Determine if enums are unique to specific effect
        // or general for all effects
        private static void DetectCommonEnums(List<Effects.Effect> effects)
        {
            List<Effects.Property> allProperties = GetAllEffectsProperties(effects);
            for (int propertyIndex = 0; propertyIndex < allProperties.Count; propertyIndex++)
            {
                Effects.EnumValues fields = allProperties[propertyIndex].EnumFields;
                if (fields == null || fields.Usage != Effects.EnumValues.UsageType.UsedByOneEffect)
                    continue;
                for (int propertyIndex2 = propertyIndex + 1; propertyIndex2 < allProperties.Count; propertyIndex2++)
                {
                    Effects.EnumValues fields2 = allProperties[propertyIndex2].EnumFields;
                    if (fields2 == null)
                        continue;

                    // Allow merging if either enum is a subset of the values of the other,
                    // but not if their value sets are distinct in both directions.
                    var delta1 = fields.FieldsList.Except(fields2.FieldsList);
                    var delta2 = fields2.FieldsList.Except(fields.FieldsList);

                    if (!delta1.Any() || !delta2.Any())
                    {
                        fields.Usage = Effects.EnumValues.UsageType.UsedByMultipleEffects;
                        fields.IsRepresentative = true;
                        fields2.Usage = Effects.EnumValues.UsageType.UsedByMultipleEffects;
                        fields2.FieldsList = fields.FieldsList;

                        // Exclude any enum values that are supported by one enum but not the other.
                        allProperties[propertyIndex2].ExcludedEnumIndexes.AddRange(delta1.Select(field => fields.FieldsList.IndexOf(field).ToString()));
                        allProperties[propertyIndex].ExcludedEnumIndexes.AddRange(delta2.Select(field => fields2.FieldsList.IndexOf(field).ToString()));
                    }
                }
            }
        }

        private static void AssignPropertyNames(List<Effects.Effect> effects)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                string className = property.EffectName.Replace(" ", "") + "Effect";
                property.TypeNameIdl = property.Type;
                if (property.TypeNameIdl == "enum")
                {
                    if (property.EnumFields.Usage == Effects.EnumValues.UsageType.UsedByMultipleEffects)
                        property.TypeNameIdl = "Effect" + property.Name;
                    else
                    {
                        Debug.Assert(property.EnumFields.Usage == Effects.EnumValues.UsageType.UsedByOneEffect);
                        property.TypeNameIdl = className + property.Name;                    
                    }
                }

                property.NativePropertyName = "D2D1_" + property.EffectName.Replace(" ", "").Replace("-", "").ToUpper() + "_PROP";
                foreach (Char c in property.Name)
                {
                    if (Char.IsUpper(c))
                    {
                        property.NativePropertyName += "_";
                    }
                    property.NativePropertyName += c.ToString().ToUpper();
                }
            }
        }

        // Some effects have names that starts with 3D or 2D prefix.
        // C++ forbidds class name that starts with digits
        // Replace 3D/2D prefix at the end
        private static void AssignEffectsClassNames(
            List<Effects.Effect> effects, 
            List<Overrides.XmlBindings.Effect> overrides,
            Dictionary<string, QualifiableType> typeDictionary)
        {
            foreach (var effect in effects)
            {
                string className = FormatClassName(effect.Properties[0].Value);

                string prefix = className.Substring(0, 2);
                if (prefix == "2D" || prefix == "3D")
                {
                    effect.ClassName = className.Remove(0, 2) + prefix + "Effect";
                }
                else
                {
                    effect.ClassName = className + "Effect";
                }

                // Apply effect and property name overrides based on XML settings
                var effectOverride = overrides.Find(o => o.Name == effect.ClassName);
                if (effectOverride != null)
                {
                    ApplyEffectOverrides(effect, effectOverride, typeDictionary);
                }

                effect.InterfaceName = "I" + effect.ClassName;
            }
        }

        private static void ApplyEffectOverrides(Effects.Effect effect, Overrides.XmlBindings.Effect effectOverride, Dictionary<string, QualifiableType> typeDictionary)
        {
            effect.Overrides = effectOverride;

            // Override the effect name?
            if (effectOverride.ProjectedNameOverride != null)
            {
                effect.ClassName = effectOverride.ProjectedNameOverride;
            }

            // Override input names?
            foreach (var inputOverride in effectOverride.Inputs)
            {
                var input = effect.Inputs.InputsList.Find(p => p.Name == inputOverride.Name);
                input.Name = inputOverride.ProjectedNameOverride;
            }

            foreach (var propertyOverride in effectOverride.Properties)
            {
                var property = effect.Properties.Find(p => p.Name == propertyOverride.Name);

                if (property != null)
                {
                    // Override settings of an existing property.
                    if (propertyOverride.ProjectedNameOverride != null)
                    {
                        property.Name = propertyOverride.ProjectedNameOverride;
                    }

                    if (propertyOverride.DefaultValueOverride != null)
                    {
                        var defaultProperty = property.Properties.Single(p => p.Name == "Default");
                        defaultProperty.Value = propertyOverride.DefaultValueOverride;
                    }

                    property.IsHidden = propertyOverride.IsHidden;
                    property.ConvertRadiansToDegrees = propertyOverride.ConvertRadiansToDegrees;
                }

                if (property == null || propertyOverride.IsHandCoded)
                {
                    // Add a custom property that is part of our API surface but not defined by D2D.
                    effect.Properties.Add(new Effects.Property
                    {
                        Name = propertyOverride.ProjectedNameOverride ?? propertyOverride.Name,
                        TypeNameIdl = string.IsNullOrEmpty(propertyOverride.Type) ? property.TypeNameIdl : propertyOverride.Type,
                        IsHandCoded = true
                    });

                    // If we are masking a real D2D property with an alternative
                    // hand-coded version, mark the real D2D property as hidden.
                    if (property != null)
                    {
                        property.IsHidden = true;
                    }
                }
            }
        }

        private static List<Enum> ParseD2DEffectsEnums(List<string> pathsToHeaders, Dictionary<string, QualifiableType> typeDictionary)
        {
            List<Enum> d2dEnums = new List<Enum>();

            foreach (var path in pathsToHeaders)
            {
                StreamReader reader = File.OpenText(path);
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    if (line.Contains("typedef enum") && line.Substring(line.Length - 4) != "PROP")
                    {
                        char[] separator = { ' ' };
                        string[] words = line.Split(separator, StringSplitOptions.RemoveEmptyEntries);
                        string enumName = words[words.Length - 1];

                        // Skip brace
                        reader.ReadLine();

                        List<EnumValue> enumValues = new List<EnumValue>();

                        while ((line = reader.ReadLine()) != "")
                        {
                            words = line.TrimEnd(',').Split(separator, StringSplitOptions.RemoveEmptyEntries);

                            // Looking for definitions of the form "EnumEntry = value"
                            if (words.Length == 3 &&
                                words[1] == "=" &&
                                !words[0].StartsWith("//") &&
                                !words[0].Contains("FORCE_DWORD"))
                            {
                                NumberStyles numberStyle = 0;

                                if (words[2].StartsWith("0x"))
                                {
                                    words[2] = words[2].Substring(2);
                                    numberStyle = NumberStyles.HexNumber;
                                }

                                int value;
                                if (!int.TryParse(words[2], numberStyle, null, out value))
                                {
                                    value = enumValues.Count;
                                }

                                string nativeValueName = words[0];
                                Debug.Assert(nativeValueName.StartsWith("D2D1_"));
                                string rawValueNameComponent = nativeValueName.Substring(5);
                                enumValues.Add(new EnumValue(nativeValueName, rawValueNameComponent, value));
                            }
                        }

                        Debug.Assert(enumName.StartsWith("D2D1_"));
                        enumName = enumName.Substring(5);
                        Enum effectEnum;
                        string key = "D2D1::" + enumName;
                        if(typeDictionary.ContainsKey(key))
                        {
                            effectEnum = typeDictionary[key] as Enum;
                        }
                        else
                        {
                            effectEnum = new Enum(enumName, enumValues, typeDictionary);
                        }
                        d2dEnums.Add(effectEnum);
                    }
                }
            }

            return d2dEnums;
        }

        private static bool IsEnumEqualD2DEnum(Effects.Property enumProperty, Enum d2dEnum, bool shouldMatchName)
        {
            // Check if names are the same
            if (FormatEnumValueString(d2dEnum.NativeName).Contains(FormatEnumValueString(enumProperty.EffectName)) || !shouldMatchName)
            {
                // Check if number of enums values are the same
                if (d2dEnum.Values.Count == enumProperty.EnumFields.FieldsList.Count)
                {
                    var d2dEnumValues = d2dEnum.Values;

                    for (int i = 0; i < enumProperty.EnumFields.FieldsList.Count; ++i)
                    {
                        if (!FormatEnumValueString(d2dEnumValues[i].NativeName).Contains(FormatEnumValueString(enumProperty.EnumFields.FieldsList[i].Displayname)))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        private static void AssignD2DEnums(List<Effects.Effect> effects, List<Enum> d2dEnums)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                Effects.EnumValues fields = property.EnumFields;
                if (fields == null)
                    continue;

                // Try to find enum for specific effect
                foreach (var d2dEnum in d2dEnums)
                {
                    if (IsEnumEqualD2DEnum(property, d2dEnum, true))
                    {
                        fields.D2DEnum = d2dEnum;
                    }
                }

                // If failed to find enum for specific effect, try general
                if (fields.D2DEnum == null)
                {
                    // Try to find enum without name matching
                    foreach (var d2dEnum in d2dEnums)
                    {
                        if (IsEnumEqualD2DEnum(property, d2dEnum, false))
                        {
                            fields.D2DEnum = d2dEnum;
                        }
                    }
                }
            }
        }

        private static void GenerateOutput(List<Effects.Effect> effects, string outDirectory)
        {
            using (Formatter commonStreamWriter = new Formatter(Path.Combine(outDirectory, "EffectsCommon.abi.idl")))
            {
                OutputEffectType.OutputCommonEnums(effects, commonStreamWriter);
            }

            using (Formatter effectMakersStreamWriter = new Formatter(Path.Combine(outDirectory, "EffectMakers.cpp")))
            {
                OutputEffectType.OutputEffectMakers(effects, effectMakersStreamWriter);
            }

            foreach (var effect in effects.Where(IsEffectEnabled))
            {
                Directory.CreateDirectory(outDirectory);
                using (Formatter idlStreamWriter = new Formatter(Path.Combine(outDirectory, effect.ClassName + ".abi.idl")),
                                 hStreamWriter = new Formatter(Path.Combine(outDirectory, effect.ClassName + ".h")),
                                 cppStreamWriter = new Formatter(Path.Combine(outDirectory, effect.ClassName + ".cpp")))
                {
                    OutputEffectType.OutputEffectIdl(effect, idlStreamWriter);
                    OutputEffectType.OutputEffectHeader(effect, hStreamWriter);
                    OutputEffectType.OutputEffectCpp(effect, cppStreamWriter);
                }
            }
        }

        private static string FormatEnumValueString(string inString)
        {
            string result = inString.Replace("_", "");
            result = result.Replace(" ", "");
            result = result.Replace("-", "");
            result = result.ToLower();

            return result;
        }

        public static string FormatClassName(string name)
        {
            return name.Replace(" ", "")
                       .Replace("-", "")
                       .Replace("DPI", "Dpi")
                       .Replace("toAlpha", "ToAlpha");
        }

        private static Effects.Effect ParseEffectXML(string path)
        {
            Effects.Effect effect = null;

            XmlSerializer serializer = new XmlSerializer(typeof(Effects.Effect));

            using (StreamReader reader = new StreamReader(path))
            {
                effect = (Effects.Effect)serializer.Deserialize(reader);
            }

            return effect;
        }
    }
}
