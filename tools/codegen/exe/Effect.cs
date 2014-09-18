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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Xml.Serialization;
using System.IO;

namespace CodeGen
{
    public class Field
    {
        public Field()
        {
            ShouldProject = true;
        }

        [XmlAttribute("name")]
        public string Name { get; set; }

        [XmlAttribute("displayname")]
        public string Displayname { get; set; }

        [XmlAttribute("index")]
        public string Index { get; set; }

        public bool ShouldProject { get; set; }
    }

    public class Fields
    {
        public Fields()
        {
            IsUnique = true;
            IsRepresentative = false;
        }

        public D2DEnum NativeEnum { get; set; }

        public bool IsUnique { get; set; }

        public bool IsRepresentative { get; set; }

        [XmlElement("Field")]
        public List<Field> FieldsList { get; set; }

        public override bool Equals(System.Object obj)
        {
            // If parameter is null return false.
            if (obj == null)
            {
                return false;
            }

            // If parameter cannot be cast to Point return false.
            Fields fields = obj as Fields;
            if ((System.Object)fields == null)
            {
                return false;
            }

            if (fields.FieldsList.Count != this.FieldsList.Count)
            {
                return false;
            }

            // Return true if the fields match:
            bool isSame = true;
            for (int fieldCount = 0; fieldCount < fields.FieldsList.Count; ++fieldCount)
            {
                if (fields.FieldsList[fieldCount].Name != this.FieldsList[fieldCount].Name)
                {
                    isSame = false;
                    break;
                }
            }
            return isSame;
        }

        public override int GetHashCode()
        {
            int hash = 0;

            foreach (var field in FieldsList)
            {
                hash ^= field.GetHashCode();
            }

            return hash;
        }
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
        public Fields EnumFields { get; set; }

        public string EffectName { get; set; }
        public string TypeNameIdl { get; set; }

        public string TypeNameCpp { get; set; }

        public bool ShouldProject { get; set; }

        public string NativePropertyName { get; set; }

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
    }

    public class D2DEnum
    {
        public D2DEnum()
        {
            Enums = new List<string>();
        }

        public string Name { get; set; }
        public List<string> Enums { get; set; }
    }

    public class EffectGenerator
    {
        public void OuptutEffects(string inputEffectsDir, string outputPath)
        {
            string[] filePaths = Directory.GetFiles(inputEffectsDir);

            var overridesXmlData = XmlBindings.Utilities.LoadXmlData<Overrides.XmlBindings.Settings>(inputEffectsDir, "../../Settings.xml");

            List<Effect> effects = new List<Effect>();
            foreach (var xmlFilePaht in filePaths)
            {
                effects.Add(ParseEffectXML(xmlFilePaht));
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
            ResolveSimilarEnums(effects);

            List<D2DEnum> d2dEnums = ParseD2DEffectsEnums(d2dHeaders);

            AssignD2DEnums(effects, d2dEnums);
            AssignEffectsClassNames(effects);
            ResolveTypeNames(effects);
            RegisterUuids(effects);

            OverrideEnums(overridesXmlData.Namespaces.Find(namespaceElement => namespaceElement.Name == "Effects").Enums, effects);

            GenerateOutput(effects, outputPath);
        }

        // Register effect uuids.  
        // Only effects with registered uuid will be generated
        // Other effects still important to parse for finding common enums
        // TODO #2443: Generate static uuids based on class names and following RFC 4122 rules. 
        private void RegisterUuids(List<Effect> effects)
        {
            effects.Find(effect => effect.Properties[0].Value == "Gaussian Blur").Uuid = "6202E54A-E229-489B-9098-C2A1B5AE5170";
            effects.Find(effect => effect.Properties[0].Value == "Saturation").Uuid = "5D33D5C1-EA32-46D2-B32C-636261EB6B27";
            effects.Find(effect => effect.Properties[0].Value == "3D Transform").Uuid = "B31D51C6-5521-4718-906A-A46B3177740F";
            effects.Find(effect => effect.Properties[0].Value == "Blend").Uuid = "342F0890-FE85-4257-8012-7B2F9D7B6A95";
            effects.Find(effect => effect.Properties[0].Value == "Composite").Uuid = "094F52BC-E406-454D-AE53-240E26D4236C";
        }

        private List<Property> GetAllEffectsProperties(List<Effect> effects)
        {
            List<Property> allProperties = new List<Property>();
            foreach (var effect in effects)
            {
                foreach (var property in effect.Properties)
                {
                    allProperties.Add(property);
                }
            }
            return allProperties;
        }

        private void AssignEffectsNamesToProperties(List<Effect> effects)
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

        private void OverrideEnums(List<Overrides.XmlBindings.Enum> enumsOverrides, List<Effect> effects)
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
                    foreach (var enumValue in enumOverride.Values)
                    {
                        if (!enumValue.ShouldProject)
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

        private void ResolveTypeNames(List<Effect> effects)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                if (property.TypeNameIdl != null)
                {
                    string xmlName = property.TypeNameIdl;
                    property.TypeNameCpp = xmlName;
                    if (property.TypeNameIdl.StartsWith("matrix"))
                    {
                        property.TypeNameIdl = "Microsoft.Graphics.Canvas.Numerics." + char.ToUpper(xmlName[0]) + xmlName.Substring(1);
                        property.TypeNameCpp = "Numerics::" + char.ToUpper(xmlName[0]) + xmlName.Substring(1);
                    }
                }
            }
        }

        // Determine if enums are unique to specific effect
        // or general for all effects
        private void DetectCommonEnums(List<Effect> effects)
        {
            List<Property> allProperties = GetAllEffectsProperties(effects);
            for (int propertyIndex = 0; propertyIndex < allProperties.Count; propertyIndex++)
            {
                Fields fields = allProperties[propertyIndex].EnumFields;
                if (fields == null || fields.IsUnique == false)
                    continue;
                for (int propertyIndex2 = propertyIndex + 1; propertyIndex2 < allProperties.Count; propertyIndex2++)
                {
                    Fields fields2 = allProperties[propertyIndex2].EnumFields;
                    if (fields2 == null || fields.FieldsList.Count != fields2.FieldsList.Count)
                        continue;
                    if (fields.Equals(fields2))
                    {
                        fields.IsUnique = false;
                        fields.IsRepresentative = true;
                        fields2.IsUnique = false;
                    }
                }
            }
        }

        private void AssignPropertyNames(List<Effect> effects)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                string className = property.EffectName.Replace(" ", "") + "Effect";
                property.TypeNameIdl = property.Type;
                if (property.TypeNameIdl == "enum")
                {
                    if (!property.EnumFields.IsUnique)
                        property.TypeNameIdl = "Effect" + property.Name;
                    else
                        property.TypeNameIdl = className + property.Name;
                }

                property.NativePropertyName = "D2D1_" + property.EffectName.Replace(" ", "").ToUpper() + "_PROP";
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

        // Detect similar enums names with different enums list
        // It can happen if one Enum Works for 2d and other for 3d cases
        private void ResolveSimilarEnums(List<Effect> effects)
        {
            List<string> duplicateNames = new List<string>();
            HashSet<string> namesSet = new HashSet<string>();
            foreach (var property in GetAllEffectsProperties(effects))
            {
                if (property.Type == "enum" && property.EnumFields.IsRepresentative)
                {
                    if (namesSet.Contains(property.TypeNameIdl))
                    {
                        duplicateNames.Add(property.TypeNameIdl);
                    }
                    else
                    {
                        namesSet.Add(property.TypeNameIdl);
                    }
                }
            }
            foreach (var enumName in duplicateNames)
            {
                foreach (var property in GetAllEffectsProperties(effects))
                {
                    if (property.TypeNameIdl == enumName)
                    {
                        if (property.EffectName.Contains("3D"))
                            property.TypeNameIdl += "3D";
                        else
                            property.TypeNameIdl += "2D";
                    }
                }
            }
        }

        // Some effects have names that starts with 3D or 2D prefix.
        // C++ forbidds class name that starts with digits
        // Replace 3D/2D prefix at the end
        private void AssignEffectsClassNames(List<Effect> effects)
        {
            foreach (var effect in effects)
            {
                string className = effect.Properties[0].Value.Replace(" ", "");
                string prefix = className.Substring(0, 2);
                if (prefix == "2D" || prefix == "3D")
                {
                    effect.ClassName = className.Remove(0, 2) + prefix + "Effect";
                }
                else
                {
                    effect.ClassName = className + "Effect";
                }
                effect.InterfaceName = "I" + effect.ClassName;
            }
        }

        private List<D2DEnum> ParseD2DEffectsEnums(List<string> pathsToHeaders)
        {
            List<D2DEnum> d2dEnums = new List<D2DEnum>();

            foreach (var path in pathsToHeaders)
            {
                StreamReader reader = File.OpenText(path);
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    if (line.Contains("typedef enum") && line.Substring(line.Length - 4) != "PROP")
                    {
                        D2DEnum effectEnum = new D2DEnum();
                        string[] words = line.Split(' ');
                        effectEnum.Name = words[words.Length - 1];

                        //Skip brace
                        reader.ReadLine();

                        while ((line = reader.ReadLine()) != "")
                        {
                            words = line.Split(' ');
                            // Indent size in headers is 8
                            if (words[8] != "" && words[8] != "//")
                                effectEnum.Enums.Add(words[8]);
                        }

                        // Remove last force_dword enum
                        effectEnum.Enums.RemoveAt(effectEnum.Enums.Count - 1);
                        d2dEnums.Add(effectEnum);
                    }
                }
            }

            return d2dEnums;
        }

        private bool IsEnumEqualD2DEnum(Property enumProperty, D2DEnum d2dEnum, bool shouldMatchName)
        {
            // Check if names are the same
            if (FormatEnumValueString(d2dEnum.Name).Contains(FormatEnumValueString(enumProperty.EffectName)) || !shouldMatchName)
            {
                // Check if number of enums values are the same
                if (d2dEnum.Enums.Count == enumProperty.EnumFields.FieldsList.Count)
                {
                    for (int i = 0; i < enumProperty.EnumFields.FieldsList.Count; ++i)
                    {
                        if (!FormatEnumValueString(d2dEnum.Enums[i]).Contains(FormatEnumValueString(enumProperty.EnumFields.FieldsList[i].Displayname)))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        private void AssignD2DEnums(List<Effect> effects, List<D2DEnum> d2dEnums)
        {
            foreach (var property in GetAllEffectsProperties(effects))
            {
                Fields fields = property.EnumFields;
                if (fields == null)
                    continue;

                // Try to find enum for specific effect
                foreach (var d2dEnum in d2dEnums)
                {
                    if (IsEnumEqualD2DEnum(property, d2dEnum, true))
                    {
                        fields.NativeEnum = d2dEnum;
                    }
                }

                // If failed to find enum for specific effect, try general
                if (fields.NativeEnum == null)
                {
                    // Try to find enum without name matching
                    foreach (var d2dEnum in d2dEnums)
                    {
                        if (IsEnumEqualD2DEnum(property, d2dEnum, false))
                        {
                            fields.NativeEnum = d2dEnum;
                        }
                    }
                }
            }
        }

        private void GenerateOutput(List<Effect> effects, string outDirectory)
        {
            using (Formatter commonStreamWriter = new Formatter(Path.Combine(outDirectory, "EffectsCommon.abi.idl")))
            {
                OutputEffectType.OutputCommonEnums(effects, commonStreamWriter);
            }

            foreach (var effect in effects)
            {
                if (effect.Uuid == null)
                    continue;

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

        private string FormatEnumValueString(string inString)
        {
            string result = inString.Replace("_", "");
            result = result.Replace(" ", "");
            result = result.Replace("-", "");
            result = result.ToLower();

            return result;
        }

        private Effect ParseEffectXML(string path)
        {
            Effect effect = null;

            XmlSerializer serializer = new XmlSerializer(typeof(Effect));

            using (StreamReader reader = new StreamReader(path))
            {
                effect = (Effect)serializer.Deserialize(reader);
            }

            return effect;
        }
    }
}
