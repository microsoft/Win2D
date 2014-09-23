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

namespace CodeGen
{
    public static class OutputEffectType
    {
        public static void OutputEnum(Property enumProperty, Formatter output)
        {
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
        }

        public static void OutputCommonEnums(List<Effect> effects, Formatter output)
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
                        var registeredEffects = effects.Where(EffectGenerator.IsEffectEnabled);
                        // Check if any registred property need this enum
                        if (registeredEffects.Any(e => e.Properties.Any(p => p.TypeNameIdl == property.TypeNameIdl)))
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

        public static void OutputEffectIdl(Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            output.WriteLine("namespace Microsoft.Graphics.Canvas.Effects");
            output.WriteLine("{");
            output.Indent();

            // Output all enums specific to this effect
            foreach (var property in effect.Properties)
            {
                if (property.Type == "enum" && property.EnumFields.IsUnique)
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
            output.WriteLine("requires  Microsoft.Graphics.Canvas.ICanvasImage");
            output.WriteLine("{");
            output.Indent();

            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string")
                    continue;

                output.WriteLine("[propget]");
                output.WriteLine("HRESULT " + property.Name + "([out, retval] " + property.TypeNameIdl + "* value);");
                output.WriteLine();

                output.WriteLine("[propput]");
                output.WriteLine("HRESULT " + property.Name + "([in] " + property.TypeNameIdl + " value);");
                output.WriteLine();
            }

            // Check if inputs specify maximum attribute in xml and if it is marked as unlimited
            if (!(effect.Inputs.Maximum != null && effect.Inputs.Maximum == "0xFFFFFFFF"))
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];

                    output.WriteLine("[propget]");
                    output.WriteLine("HRESULT " + input.Name + "([out, retval] IEffectInput** input);");
                    output.WriteLine();

                    output.WriteLine("[propput]");
                    output.WriteLine("HRESULT " + input.Name + "([in] IEffectInput* input);");
                    output.WriteLine();
                }
            }

            output.Unindent();
            output.WriteLine("};");
            output.WriteLine();

            output.WriteLine("[version(VERSION), activatable(VERSION)]");
            output.WriteLine("runtimeclass " + effect.ClassName);
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("[default] interface " + effect.InterfaceName + ";");
            output.WriteLine("interface IEffect;");
            output.Unindent();
            output.WriteLine("}");
            output.Unindent();
            output.WriteLine("}");
        }

        public static void OutputEffectHeader(Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            output.WriteLine("#pragma once");
            output.WriteLine();
            output.WriteLine("#include <CanvasImage.h>");
            output.WriteLine();
            output.WriteLine("#include \"ClosablePtr.h\"");
            output.WriteLine("#include \"ResourceManager.h\"");
            output.WriteLine("#include \"effects\\CanvasEffect.h\"");
            output.WriteLine();
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
            output.WriteLine(effect.ClassName + "();");
            output.WriteLine();

            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string")
                    continue;

                output.WriteLine("PROPERTY(" + property.Name + ", " + property.TypeNameCpp + ");");
            }

            if (!(effect.Inputs.Maximum != null && effect.Inputs.Maximum == "0xFFFFFFFF"))
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];
                    output.WriteLine("PROPERTY(" + input.Name + ", IEffectInput*);");
                }
            }
            output.Unindent();
            output.WriteLine("};");
            output.Unindent();
            output.WriteLine("}}}}}");
        }

        public static void OutputEffectCpp(Effect effect, Formatter output)
        {
            OutputDataTypes.OutputLeadingComment(output);

            bool isInputSizeFixed = true;
            if (effect.Inputs.Maximum != null && effect.Inputs.Maximum == "0xFFFFFFFF")
                isInputSizeFixed = false;

            output.WriteLine("#include \"pch.h\"");
            output.WriteLine("#include \"..\\CanvasEffect.h\"");
            output.WriteLine("#include \"" + effect.ClassName + ".h\"");
            output.WriteLine();
            output.WriteLine("namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects");
            output.WriteLine("{");
            output.Indent();
            output.WriteLine(effect.ClassName + "::" + effect.ClassName + "()");
            output.WriteIndent();
            int inputsCoutn = effect.Inputs.InputsList.Count;
            if (effect.Inputs.Maximum != null && effect.Inputs.Maximum == "0xFFFFFFFF")
                inputsCoutn = 0;
            output.WriteLine(": CanvasEffect(CLSID_D2D1"
                             + effect.Properties[0].Value.Replace(" ", "") + ", "
                             + (effect.Properties.Count - 4) + ", "
                             + inputsCoutn + ", "
                             + isInputSizeFixed.ToString().ToLower() + ")");
            output.WriteLine("{");
            output.Indent();
            output.WriteLine("// Set default values");

            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string")
                    continue;

                string defaultValue = property.Properties.Find(internalProperty => internalProperty.Name == "Default").Value;

                if (property.Type == "enum")
                {
                    string d2dDefaultEnum = property.EnumFields.NativeEnum.Enums[Int32.Parse(defaultValue)];
                    output.WriteLine("SetProperty(" + property.NativePropertyName + ", static_cast<uint32_t>(" + d2dDefaultEnum + "), true);");
                }
                else if (property.Type.StartsWith("matrix"))
                {
                    output.WriteLine(property.TypeNameCpp + " default" + property.Name + " = { " + FixSpacing(defaultValue.Substring(1, defaultValue.Length - 2)) + " };");
                    output.WriteLine("SetProperty(" + property.NativePropertyName + ", " + " default" + property.Name + ", true);");
                }
                else
                {
                    string typeExtension = "";
                    if (property.Type == "float")
                        typeExtension = "f";
                    output.WriteLine("SetProperty(" + property.NativePropertyName + ", " + defaultValue + typeExtension + ", true);");
                }
            }
            output.Unindent();
            output.WriteLine("}");
            output.WriteLine();
            foreach (var property in effect.Properties)
            {
                // Property with type string describes 
                // name/author/category/description of effect but not input type
                if (property.Type == "string")
                    continue;

                bool isValidation = false;
                if (property.Properties.Find(internalProperty => internalProperty.Name == "Min") != null)
                    isValidation = true;

                bool isWithUnsupported = false;
                if (property.ExcludedEnumIndexes != null && property.ExcludedEnumIndexes.Count != 0)
                    isWithUnsupported = true;

                if (property.Type == "enum" && !isWithUnsupported)
                {
                    output.WriteLine("IMPLEMENT_ENUM_PROPERTY(" + effect.ClassName + ",");
                }
                else if (isValidation)
                {
                    output.WriteLine("IMPLEMENT_PROPERTY_WITH_VALIDATION(" + effect.ClassName + ",");
                }
                else if (isWithUnsupported)
                {
                    output.WriteLine("IMPLEMENT_ENUM_PROPERTY_WITH_UNSUPPORTED(" + effect.ClassName + ",");
                }
                else
                {
                    output.WriteLine("IMPLEMENT_PROPERTY(" + effect.ClassName + ",");
                }

                output.Indent();
                output.WriteLine(property.Name + ",");
                output.WriteLine(property.TypeNameCpp + ",");

                if (isValidation)
                {
                    output.WriteLine(property.NativePropertyName + ",");
                    output.WriteLine(property.Properties.Find(internalProperty => internalProperty.Name == "Min").Value + ",");
                    output.WriteLine(property.Properties.Find(internalProperty => internalProperty.Name == "Max").Value + ")");
                }
                else if (isWithUnsupported)
                {
                    output.WriteLine(property.NativePropertyName + ",");
                    var last = property.ExcludedEnumIndexes.Last();
                    foreach (var index in property.ExcludedEnumIndexes)
                    {
                        if (index == last)
                        {
                            output.WriteLine(index + "U)");
                        }
                        else
                        {
                            output.WriteLine(index + "U,");
                        }
                    }
                }
                else
                {
                    output.WriteLine(property.NativePropertyName + ")");
                }

                output.Unindent();
                output.WriteLine();
            }

            if (!(effect.Inputs.Maximum != null && effect.Inputs.Maximum == "0xFFFFFFFF"))
            {
                for (int i = 0; i < effect.Inputs.InputsList.Count; ++i)
                {
                    var input = effect.Inputs.InputsList[i];
                    output.WriteLine("IMPLEMENT_INPUT_PROPERTY(" + effect.ClassName + ",");
                    output.Indent();
                    output.WriteLine(input.Name + ",");
                    output.WriteLine(i.ToString() + ")");
                    output.Unindent();
                    output.WriteLine();
                }
            }

            output.WriteLine("ActivatableClass(" + effect.ClassName + ");");

            output.Unindent();
            output.WriteLine("}}}}}");
        }

        public static string FixSpacing(string input)
        {
            string result = "";

            for (int i = 0; i < input.Length; ++i)
            {
                if (Char.IsDigit(input[i]) && result.Length != 0 && Char.IsPunctuation(result.Last()))
                {
                    result += ' ';
                }
                result += input[i];
            }

            return result;
        }

    }
}
