// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Linq;

namespace CodeGen
{
    public static class OutputEffectType
    {
        const string stdInfinity = "std::numeric_limits<float>::infinity()";

        public static void OutputEnum(Effects.Property enumProperty, Formatter output)
        {
            OutputVersionConditional(enumProperty.WinVer, output);

            output.WriteLine("[version(VERSION)]");
            output.WriteLine("typedef enum " + enumProperty.TypeNameIdl);
            output.WriteLine("{");
            output.Indent();

            for (int i = 0; i < enumProperty.EnumFields.FieldsList.Count; ++i)
            {
                var enumValue = enumProperty.EnumFields.FieldsList[i];
                if (i != (enumProperty.EnumFields.FieldsList.Count - 1))
                {
                    output.WriteLine(enumValue.Name + " = " + i.ToString() + ",");
                }
                else
                {
                    output.WriteLine(enumValue.Name + " = " + i.ToString());
                }
            }
            output.Unindent();
            output.WriteLine("} " + enumProperty.TypeNameIdl + ";");

            EndVersionConditional(enumProperty.WinVer, output);
        }

        public static void OutputCommonEnums(List<Effects.Effect> effects, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            output.WriteLine("namespace Microsoft.Graphics.Canvas.Effects");
            output.WriteLine("{");
            output.Indent();
            bool isFirstOutput = true;
            foreach (var effect in effects)
            {
                foreach (var property in effect.Properties)
                {
                    // Check if property represent enum that is common to some group of effects
                    if (property.Type == "enum" && property.EnumFields.IsRepresentative && property.ShouldProject)
                    {
                        // Check if any registred property need this enum
                        if (effects.Any(e => e.Properties.Any(p => p.TypeNameIdl == property.TypeNameIdl)))
                        {
                            if (isFirstOutput)
                            {
                                isFirstOutput = false;
                            }
                            else
                            {
                                output.WriteLine();
                            }
                            OutputEnum(property, output);
                        }
                    }
                }
            }
            output.Unindent();
            output.WriteLine("}");
        }

        public static void OutputEffectMakers(List<Effects.Effect> effects, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            output.WriteLine("#include \"pch.h\"");
            output.WriteLine();

            var effectsByVersion = from effect in effects
                                   orderby effect.ClassName
                                   group effect by (effect.Overrides != null ? effect.Overrides.WinVer : null) into versionGroup
                                   orderby versionGroup.Key
                                   select versionGroup;

            foreach (var versionGroup in effectsByVersion)
            {
                OutputVersionConditional(versionGroup.Key, output);

                foreach (var effect in versionGroup)
                {
                    output.WriteLine("#include \"" + effect.ClassName + ".h\"");
                }

                EndVersionConditional(versionGroup.Key, output);
                output.WriteLine();
            }

            output.WriteLine();
            output.WriteLine("std::pair<IID, CanvasEffect::MakeEffectFunction> CanvasEffect::m_effectMakers[] =");
            output.WriteLine("{");

            int longestName = effects.Select(effect => effect.ClassName.Length).Max();

            foreach (var versionGroup in effectsByVersion)
            {
                OutputVersionConditional(versionGroup.Key, output);
                output.Indent();

                foreach (var effect in versionGroup)
                {
                    string padding = new string(' ', longestName - effect.ClassName.Length);

                    output.WriteLine("{ " + effect.ClassName + "::EffectId(), " + padding + "MakeEffect<" + effect.ClassName + "> " + padding + "},");
                }

                output.Unindent();
                EndVersionConditional(versionGroup.Key, output);
                output.WriteLine();
            }

            output.Indent();
            output.WriteLine("{ GUID_NULL, nullptr }");
            output.Unindent();

            output.WriteLine("};");
        }

        public static void OutputEffectIdl(Effects.Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            OutputVersionConditional(effect, output);

            output.WriteLine("namespace Microsoft.Graphics.Canvas.Effects");
            output.WriteLine("{");
            output.Indent();

            // Output all enums specific to this effect
            foreach (var property in effect.Properties)
            {
                if (property.Type == "enum" && 
                    property.EnumFields.Usage == Effects.EnumValues.UsageType.UsedByOneEffect && 
                    property.ShouldProject)
                {
                    OutputEnum(property, output);
                    output.WriteLine();
                }
            }

            output.WriteLine("runtimeclass " + effect.ClassName + ";");
            output.WriteLine();
            output.WriteLine("[version(VERSION), uuid(" + effect.Uuid + "), exclusiveto(" + effect.ClassName + ")]");
            output.WriteLine("interface " + effect.InterfaceName + " : IInspectable");
            output.WriteIndent();
            output.WriteLine("requires ICanvasEffect");
            output.WriteLine("{");
            output.Indent();

            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string" || property.IsHidden)
                    continue;

                output.WriteLine("[propget]");
                if (property.IsArray)
                {
                    output.WriteLine("HRESULT " + property.Name + "([out] UINT32* valueCount, [out, size_is(, *valueCount), retval] " + property.TypeNameIdl + "** valueElements);");
                }
                else
                {
                    output.WriteLine("HRESULT " + property.Name + "([out, retval] " + property.TypeNameIdl + "* value);");
                }
                output.WriteLine();

                output.WriteLine("[propput]");
                if (property.IsArray)
                {
                    output.WriteLine("HRESULT " + property.Name + "([in] UINT32 valueCount, [in, size_is(valueCount)] " + property.TypeNameIdl + "* valueElements);");
                }
                else
                {
                    output.WriteLine("HRESULT " + property.Name + "([in] " + property.TypeNameIdl + " value);");
                }
                output.WriteLine();
            }

            if (EffectHasVariableNumberOfInputs(effect))
            {
                output.WriteLine("[propget]");
                output.WriteLine("HRESULT Sources([out, retval] Windows.Foundation.Collections.IVector<IGRAPHICSEFFECTSOURCE*>** value);");
                output.WriteLine();
            }
            else
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];

                    output.WriteLine("[propget]");
                    output.WriteLine("HRESULT " + input.Name + "([out, retval] IGRAPHICSEFFECTSOURCE** source);");
                    output.WriteLine();

                    output.WriteLine("[propput]");
                    output.WriteLine("HRESULT " + input.Name + "([in] IGRAPHICSEFFECTSOURCE* source);");
                    output.WriteLine();
                }
            }

            output.Unindent();
            output.WriteLine("};");
            output.WriteLine();

            string staticAttribute = "";

            if (HasStatics(effect))
            {
                string staticsInterfaceName = effect.InterfaceName + "Statics";
                string staticsUuid = EffectGenerator.GenerateUuid(staticsInterfaceName);

                staticAttribute = ", static(" + staticsInterfaceName + ", VERSION)";

                output.WriteLine("[version(VERSION), uuid(" + staticsUuid + "), exclusiveto(" + effect.ClassName + ")]");
                output.WriteLine("interface " + staticsInterfaceName + " : IInspectable");
                output.WriteLine("{");
                output.Indent();

                foreach (var customIdl in effect.Overrides.CustomStaticMethodIdl)
                {
                    output.WriteLine(customIdl.Trim());
                }

                if (!string.IsNullOrEmpty(effect.Overrides.IsSupportedCheck))
                {
                    output.WriteLine("[propget] HRESULT IsSupported([out, retval] boolean* value);");
                }

                output.Unindent();
                output.WriteLine("}");
                output.WriteLine();
            }
            
            output.WriteLine("[STANDARD_ATTRIBUTES, activatable(VERSION)" + staticAttribute + "]");

            output.WriteLine("runtimeclass " + effect.ClassName);
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("[default] interface " + effect.InterfaceName + ";");
            output.Unindent();
            output.WriteLine("}");
            output.Unindent();
            output.WriteLine("}");

            EndVersionConditional(effect, output);
        }

        public static void OutputEffectHeader(Effects.Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            output.WriteLine("#pragma once");
            output.WriteLine();

            OutputVersionConditional(effect, output);

            output.WriteLine("namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects ");
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("using namespace ::Microsoft::WRL;");
            output.WriteLine("using namespace ABI::Microsoft::Graphics::Canvas;");
            output.WriteLine();
            output.WriteLine("class " + effect.ClassName + " : public RuntimeClass<");
            output.Indent();
            output.WriteLine(effect.InterfaceName + ",");
            output.WriteLine("MixIn<" + effect.ClassName + ", CanvasEffect>>,");
            output.WriteLine("public CanvasEffect");
            output.Unindent();
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_" + effect.ClassName + ", BaseTrust);");
            output.WriteLine();
            output.Unindent();
            output.WriteLine("public:");
            output.Indent();
            output.WriteLine(effect.ClassName + "(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);");
            output.WriteLine();
            output.WriteLine("static IID const& EffectId() { return " + GetEffectCLSID(effect) + "; }");
            output.WriteLine();

            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string" || property.IsHidden)
                    continue;

                var propertyMacro = property.IsArray ? "EFFECT_ARRAY_PROPERTY" : "EFFECT_PROPERTY";

                output.WriteLine(propertyMacro + "(" + property.Name + ", " + property.TypeNameCpp + ");");
            }

            if (EffectHasVariableNumberOfInputs(effect))
            {
                output.WriteLine("EFFECT_SOURCES_PROPERTY();");
            }
            else
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];
                    output.WriteLine("EFFECT_PROPERTY(" + input.Name + ", IGraphicsEffectSource*);");
                }
            }

            bool hasPropertyMapping = effect.Properties.Any(IsGeneratablePropertyMapping);
            bool hasHandCodedPropertyMapping = effect.Properties.Any(p => p.IsHandCoded);

            if (hasPropertyMapping || hasHandCodedPropertyMapping)
            {
                output.WriteLine();

                if (hasPropertyMapping)
                    output.WriteLine("EFFECT_PROPERTY_MAPPING();");

                if (hasHandCodedPropertyMapping)
                    output.WriteLine("EFFECT_PROPERTY_MAPPING_HANDCODED();");
            }

            output.Unindent();
            output.WriteLine("};");

            if (HasStatics(effect))
            {
                output.WriteLine();
                output.WriteLine("class " + effect.ClassName + "Factory");
                output.Indent();
                output.WriteLine(": public AgileActivationFactory<I" + effect.ClassName + "Statics>");
                output.WriteLine(", private LifespanTracker<" + effect.ClassName + "Factory>");
                output.Unindent();
                output.WriteLine("{");
                output.Indent();
                output.WriteLine("InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_" + effect.ClassName + ", BaseTrust);");
                output.WriteLine();
                output.Unindent();
                output.WriteLine("public:");
                output.Indent();
                output.WriteLine("IFACEMETHODIMP ActivateInstance(IInspectable**) override;");

                foreach (var customDecl in effect.Overrides.CustomStaticMethodDecl)
                {
                    output.WriteLine(customDecl.Trim());
                }

                if (!string.IsNullOrEmpty(effect.Overrides.IsSupportedCheck))
                {
                    output.WriteLine("IFACEMETHOD(get_IsSupported)(boolean* value) override;");
                }

                output.Unindent();
                output.WriteLine("};");
            }
            
            output.Unindent();
            output.WriteLine("}}}}}");

            EndVersionConditional(effect, output);
        }

        public static void OutputEffectCpp(Effects.Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            bool isInputSizeFixed = !EffectHasVariableNumberOfInputs(effect);
            int inputsCount = isInputSizeFixed ? effect.Inputs.InputsList.Count : 0;

            output.WriteLine("#include \"pch.h\"");
            output.WriteLine("#include \"" + effect.ClassName + ".h\"");
            output.WriteLine();

            OutputVersionConditional(effect, output);

            output.WriteLine("namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects");
            output.WriteLine("{");
            output.Indent();
            output.WriteLine(effect.ClassName + "::" + effect.ClassName + "(ICanvasDevice* device, ID2D1Effect* effect)");
            output.WriteIndent();
            output.WriteLine(": CanvasEffect(EffectId(), "
                             + (effect.Properties.Count(p => (!p.IsHandCoded && !p.IsHdrAlias)) - 4) + ", "
                             + inputsCount + ", "
                             + isInputSizeFixed.ToString().ToLower() + ", "
                             + "device, effect, static_cast<" + effect.InterfaceName + "*>(this))");
            output.WriteLine("{");
            output.Indent();

            if (effect.Overrides != null && !string.IsNullOrEmpty(effect.Overrides.IsSupportedCheck))
            {
                output.WriteLine("if (!SharedDeviceState::GetInstance()->Is" + effect.Overrides.IsSupportedCheck + "Supported())");
                output.Indent();
                output.WriteLine("ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);");
                output.Unindent();
                output.WriteLine();
            }
            
            output.WriteLine("if (!effect)");
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("// Set default values");

            foreach (var property in effect.Properties)
            {
                WritePropertyInitialization(output, property);
            }
            output.Unindent();
            output.WriteLine("}");
            output.Unindent();
            output.WriteLine("}");
            output.WriteLine();
            foreach (var property in effect.Properties)
            {
                WritePropertyImplementation(effect, output, property);
            }

            if (EffectHasVariableNumberOfInputs(effect))
            {
                output.WriteLine("IMPLEMENT_EFFECT_SOURCES_PROPERTY(" + effect.ClassName + ")");
                output.WriteLine();
            }
            else
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];
                    output.WriteLine("IMPLEMENT_EFFECT_SOURCE_PROPERTY(" + effect.ClassName + ",");
                    output.Indent();
                    output.WriteLine(input.Name + ",");
                    output.WriteLine(i.ToString() + ")");
                    output.Unindent();
                    output.WriteLine();
                }
            }

            WritePropertyMapping(effect, output);

            if (HasStatics(effect))
            {
                output.WriteLine("IFACEMETHODIMP " + effect.ClassName + "Factory::ActivateInstance(IInspectable** instance)");
                output.WriteLine("{");
                output.Indent();
                output.WriteLine("return ExceptionBoundary([&]");
                output.WriteLine("{");
                output.Indent();
                output.WriteLine("auto effect = Make<" + effect.ClassName + ">();");
                output.WriteLine("CheckMakeResult(effect);");
                output.WriteLine();
                output.WriteLine("ThrowIfFailed(effect.CopyTo(instance));");
                output.Unindent();
                output.WriteLine("});");
                output.Unindent();
                output.WriteLine("}");
                output.WriteLine();

                if (!string.IsNullOrEmpty(effect.Overrides.IsSupportedCheck))
                {
                    output.WriteLine("IFACEMETHODIMP " + effect.ClassName + "Factory::get_IsSupported(_Out_ boolean* result)");
                    output.WriteLine("{");
                    output.Indent();
                    output.WriteLine("return ExceptionBoundary([&]");
                    output.WriteLine("{");
                    output.Indent();
                    output.WriteLine("CheckInPointer(result);");
                    output.WriteLine("*result = SharedDeviceState::GetInstance()->Is" + effect.Overrides.IsSupportedCheck + "Supported();");
                    output.Unindent();
                    output.WriteLine("});");
                    output.Unindent();
                    output.WriteLine("}");
                    output.WriteLine();
                }

                output.WriteLine("ActivatableClassWithFactory(" + effect.ClassName + ", " + effect.ClassName + "Factory);");
            }
            else
            {
                output.WriteLine("ActivatableClassWithFactory(" + effect.ClassName + ", SimpleAgileActivationFactory<" + effect.ClassName + ">);");
            }

            output.Unindent();
            output.WriteLine("}}}}}");

            EndVersionConditional(effect, output);
        }

        private static string GetEffectCLSID(Effects.Effect effect)
        {
            if (effect.Overrides != null && !string.IsNullOrEmpty(effect.Overrides.CLSIDOverride))
            {
                return effect.Overrides.CLSIDOverride;
            }
            else
            {
                return "CLSID_D2D1" + EffectGenerator.FormatClassName(effect.Properties[0].Value);
            }
        }

        private static bool HasStatics(Effects.Effect effect)
        {
            if (effect.Overrides == null)
                return false;

            return effect.Overrides.CustomStaticMethodIdl.Count > 0 ||
                   effect.Overrides.CustomStaticMethodDecl.Count > 0 ||
                   !string.IsNullOrEmpty(effect.Overrides.IsSupportedCheck);
        }

        private static void WritePropertyInitialization(Formatter output, Effects.Property property)
        {
            // Property with type string describes 
            // name/author/category/description of effect but not input type
            if (property.Type == "string" || property.IsHandCoded || property.IsHdrAlias)
                return;

            string defaultValue = property.Properties.Find(internalProperty => internalProperty.Name == "Default").Value;

            string setFunction = property.IsArray ? "SetArrayProperty" : "SetBoxedProperty";

            string customConversion = null;
            if (property.ConvertColorHdrToVector3)
                customConversion = "ConvertColorHdrToVector3";

            output.WriteLine(setFunction + "<" + (customConversion?? property.TypeNameBoxed) + ">(" + property.NativePropertyName + ", " + FormatPropertyValue(property, defaultValue) + ");");
        }

        private static void WritePropertyImplementation(Effects.Effect effect, Formatter output, Effects.Property property)
        {
            // Property with type string describes 
            // name/author/category/description of effect but not input type
            if (property.Type == "string" || property.IsHandCoded || property.IsHidden)
                return;

            var min = property.Properties.Find(internalProperty => internalProperty.Name == "Min");
            var max = property.Properties.Find(internalProperty => internalProperty.Name == "Max");

            bool isWithUnsupported = (property.ExcludedEnumIndexes != null) && (property.ExcludedEnumIndexes.Count != 0);

            string customConversion = null;

            if (property.ConvertRadiansToDegrees)
            {
                customConversion = "ConvertRadiansToDegrees";
            }
            else if (property.ConvertColorHdrToVector3)
            {
                customConversion = "ConvertColorHdrToVector3";
            }
            else if (property.Name == "AlphaMode")
            {
                customConversion = "ConvertAlphaMode";
            }

            bool isValidation = ((min != null) || (max != null) || isWithUnsupported) && (customConversion == null);

            string implementMacro = property.IsArray ? "IMPLEMENT_EFFECT_ARRAY_PROPERTY" : "IMPLEMENT_EFFECT_PROPERTY";

            if (isValidation)
            {
                implementMacro += "_WITH_VALIDATION";
            }

            output.WriteLine(implementMacro + "(" + effect.ClassName + ",");
            output.Indent();
            output.WriteLine(property.Name + ",");
            output.WriteLine((customConversion ?? property.TypeNameBoxed) + ",");

            if (!property.IsArray)
            {
                output.WriteLine(property.TypeNameCpp + ",");
            }

            if (isValidation)
            {
                output.WriteLine(property.NativePropertyName + ",");

                var validationChecks = new List<string>();

                if (min != null)
                {
                    AddValidationChecks(validationChecks, property, min.Value, ">=");
                }

                if (max != null)
                {
                    AddValidationChecks(validationChecks, property, max.Value, "<=");
                }

                if (isWithUnsupported)
                {
                    foreach (var index in property.ExcludedEnumIndexes)
                    {
                        validationChecks.Add("value != static_cast<" + property.TypeNameCpp + ">(" + index + ")");
                    }
                }

                output.WriteLine("(" + string.Join(") && (", validationChecks) + "))");
            }
            else
            {
                output.WriteLine(property.NativePropertyName + ")");
            }

            output.Unindent();
            output.WriteLine();
        }

        static void AddValidationChecks(List<string> validationChecks, Effects.Property property, string minOrMax, string comparisonOperator)
        {
            if (property.Type.StartsWith("vector"))
            {
                // Expand out a separate "value.X >= limit" check for each component of the vector.
                int componentIndex = 0;

                foreach (var componentMinOrMax in SplitVectorValue(minOrMax))
                {
                    var componentName = "XYZW"[componentIndex++];

                    validationChecks.Add("value." + componentName + " " + comparisonOperator + " " + componentMinOrMax);
                }
            }
            else
            {
                // Simple "value >= limit" check.
                validationChecks.Add("value " + comparisonOperator + " " + FormatPropertyValue(property, minOrMax));
            }
        }

        static string FormatPropertyValue(Effects.Property property, string value)
        {
            if (property.Type == "enum")
            {
                if (property.EnumFields.D2DEnum != null)
                {
                    bool valueFound = false;
                    foreach (EnumValue v in property.EnumFields.D2DEnum.Values)
                    {
                        if (v.ValueExpression == value)
                        {
                            value = v.NativeName;
                            valueFound = true;
                            break;
                        }
                    }
                    System.Diagnostics.Debug.Assert(valueFound);
                }
                else if (property.IsHidden)
                {
                    value = property.NativePropertyName.Replace("PROP_", "") + "_" + property.EnumFields.FieldsList[Int32.Parse(value)].Name.ToUpper();
                }
                else
                {
                    value = property.TypeNameCpp + "::" + property.EnumFields.FieldsList[Int32.Parse(value)].Name;
                }
            }
            else if (property.Type.StartsWith("matrix") || property.Type.StartsWith("vector"))
            {
                var values = SplitVectorValue(value);

                if (property.TypeNameCpp == "Color")
                {
                    values = ConvertVectorToColor(values);
                }
                else if (property.TypeNameCpp == "Rect")
                {
                    values = ConvertVectorToRect(values);
                }

                value = property.TypeNameCpp + "{ " + string.Join(", ", values) + " }";
            }
            else if (property.Type == "float")
            {
                if (!value.Contains('.'))
                {
                    value += ".0";
                }

                value += "f";
            }
            else if (property.Type == "bool")
            {
                value = "static_cast<boolean>(" + value + ")";
            }
            else if (property.IsArray)
            {
                value = "{ " + value + " }";
            }
            else if (value == "null")
            {
                value = "static_cast<" + property.TypeNameCpp + ">(nullptr)";
            }

            return value;
        }

        static IEnumerable<string> SplitVectorValue(string value)
        {
            // Convert "( 1.0, 2.0, 3 )" to "1.0f", "2.0f", "3"
            return value.TrimStart('(')
                        .TrimEnd(')')
                        .Replace(" ", "")
                        .Replace("inf", stdInfinity)
                        .Split(',')
                        .Select(v => v.Contains('.') ? v + 'f' : v);
        }

        static IEnumerable<string> ConvertVectorToColor(IEnumerable<string> values)
        {
            // Convert 0-1 range to 0-255.
            var colorValues = from value in values
                              select StringToFloat(value) * 255;

            // Add an alpha component if the input was a 3-component vector.
            if (colorValues.Count() < 4)
            {
                colorValues = colorValues.Concat(new float[] { 255 });
            }

            // Rearrange RGBA to ARGB.
            var rgb = colorValues.Take(3);
            var a = colorValues.Skip(3);

            return from value in a.Concat(rgb)
                   select value.ToString();
        }

        static string FloatToString(float value)
        {
            if (float.IsInfinity(value))
            {
                return (value < 0 ? "-" : "") + stdInfinity;
            }
            else
            {
                return value.ToString();
            }
        }

        static IEnumerable<string> ConvertVectorToRect(IEnumerable<string> values)
        {
            // This is in left/top/right/bottom format.
            var rectValues = values.Select(StringToFloat).ToArray();

            // Convert to x/y/w/h.
            rectValues[2] -= rectValues[0];
            rectValues[3] -= rectValues[1];

            return from value in rectValues
                   select FloatToString(value);
        }

        static float StringToFloat(string value)
        {
            if (value == stdInfinity)
            {
                return float.PositiveInfinity;
            }
            else if (value == "-" + stdInfinity)
            {
                return float.NegativeInfinity;
            }
            else
            {
                return float.Parse(value.Replace("f", ""));
            }
        }

        private static void WritePropertyMapping(Effects.Effect effect, Formatter output)
        {
            var query = from property in effect.Properties
                        where IsGeneratablePropertyMapping(property)
                        select new
                        {
                            Name = property.Name,
                            Index = property.NativePropertyName,
                            Mapping = GetPropertyMapping(property)
                        };

            var properties = query.ToList();

            if (properties.Any())
            {
                output.WriteLine("IMPLEMENT_EFFECT_PROPERTY_MAPPING(" + effect.ClassName + ",");
                output.Indent();

                int maxNameLength = properties.Select(property => property.Name.Length).Max();
                int maxIndexLength = properties.Select(property => property.Index.Length).Max();
                int maxMappingLength = properties.Select(property => property.Mapping.Length).Max();

                foreach (var property in properties)
                {
                    string nameAlignment = new string(' ', maxNameLength - property.Name.Length);
                    string indexAlignment = new string(' ', maxIndexLength - property.Index.Length);
                    string mappingAlignment = new string(' ', maxMappingLength - property.Mapping.Length);

                    string suffix = property.Equals(properties.Last()) ? ")" : ",";

                    output.WriteLine("{ L\"" + property.Name + "\", " + nameAlignment + 
                                     property.Index + ", " + indexAlignment + 
                                     property.Mapping + mappingAlignment + " }" + suffix);
                }

                output.Unindent();
                output.WriteLine();
            }
        }

        private static string GetPropertyMapping(Effects.Property property)
        {
            if (property.ConvertRadiansToDegrees)
            {
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES";
            }
            else if (property.Name == "AlphaMode")
            {
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE";
            }
            else if (property.TypeNameCpp == "Color")
            {
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR" + property.Type.Single(char.IsDigit);
            }
            else if (property.IsHdrAlias)
            {
                // The platform doesn't natively support HDR colors, so we set the mapping type to unknown.
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN";
            }
            else if (property.TypeNameCpp == "Rect")
            {
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4";
            }
            else
            {
                return "GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT";
            }
        }

        private static bool IsGeneratablePropertyMapping(Effects.Property property)
        {
            return property.Type != "string" && !property.IsHidden && !property.IsHandCoded;
        }

        static bool EffectHasVariableNumberOfInputs(Effects.Effect effect)
        {
            return effect.Inputs.Maximum == "0xFFFFFFFF";
        }

        private static void OutputVersionConditional(Effects.Effect effect, Formatter output)
        {
            if (effect.Overrides != null)
            {
                OutputVersionConditional(effect.Overrides.WinVer, output);
            }
        }

        private static void EndVersionConditional(Effects.Effect effect, Formatter output)
        {
            if (effect.Overrides != null)
            {
                EndVersionConditional(effect.Overrides.WinVer, output);
            }
        }

        private static void OutputVersionConditional(string winVer, Formatter output)
        {
            if (!string.IsNullOrEmpty(winVer))
            {
                output.WriteLine(string.Format("#if (defined {0}) && (WINVER >= {0})", winVer));
                output.WriteLine();
            }
        }

        private static void EndVersionConditional(string winVer, Formatter output)
        {
            if (!string.IsNullOrEmpty(winVer))
            {
                output.WriteLine();
                output.WriteLine(string.Format("#endif // {0}", winVer));
            }
        }
    }
}
