// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using System.IO;

namespace Mock
{
    public class Program
    {
        static string GetDefaultOutputLocation(string inputDir)
        {
            return Path.GetFullPath(Path.Combine(inputDir, "..", "..", "..", "winrt", "test.internal", "mocks"));
        }

        static void GetInheritanceHierarchy(string keyName, CodeGen.Program.ProcessedInputFiles processedInputFiles, List<CodeGen.Interface> inheritanceHierarchy)
        {
            CodeGen.Interface interfaceType = processedInputFiles.TypeDictionary[keyName] as CodeGen.Interface;
            inheritanceHierarchy.Add(interfaceType);

            string extends = interfaceType.NativeNameOfIntheritanceParent;

            if (extends != "IUnknown")
            {
                GetInheritanceHierarchy(extends, processedInputFiles, inheritanceHierarchy);
            }
        }

        public static bool IsParameterConst(string form, CodeGen.QualifiableType qualifiable)
        {
            //
            // This replicates some of the logic of the D2D codegen.
            //
            switch (form)
            {
                case "pointer":
                case "pointer opt":
                case "pointer ecount opt":
                    return qualifiable is CodeGen.Struct || qualifiable is CodeGen.Primitive;
                default:
                    return false;
            }

        }

        public static string GetIndirection(string form)
        {
            switch (form)
            {
                case null:
                case "in":
                case "out":
                case "value":
                    return "";

                case "deref out pointer":
                case "pointer pointer":
                case "out pointer pointer":
                case "deref opt out pointer":
                case "deref out pointer pointer bcount":
                    return "**";

                case "out pointer":
                case "out pointer inexpressibleArray":
                case "pointer opt":
                case "pointer":
                case "deref out opt pointer":
                case "inout pointer":
                case "out pointer opt":
                case "out pointer part opt":
                case "pointer ecount opt":
                case "pointer ecount":
                case "out ecount pointer":
                case "out bcount pointer":
                    return "*";

                case "reference":
                    return "&";

                default:
                    {
                        System.Diagnostics.Debugger.Break(); // Unexpected form
                        return "";
                    }
            }
        }

        static string GetParameterTypeName_NoKeywordsOrIndirection(string nativeTypeName, CodeGen.QualifiableType qualifiable)
        {
            return qualifiable == null ? nativeTypeName : qualifiable.NativeName;
        }

        static string GetParameterTypeNameIncludingIndirection(
            CodeGen.Interface.Method.Parameter p,
            Dictionary<string, CodeGen.QualifiableType> typeDictionary)
        {
            CodeGen.QualifiableType qualifiable = null;
            if (typeDictionary.ContainsKey(p.NativeTypeName))
                qualifiable = typeDictionary[p.NativeTypeName];

            string constPart = IsParameterConst(p.Form, qualifiable) ? "CONST " : "";

            string mainPart = GetParameterTypeName_NoKeywordsOrIndirection(p.NativeTypeName, qualifiable);

            string indirection = GetIndirection(p.Form);

            return constPart + mainPart + indirection;
        }

        static void OutputClassMethodMockDeclarations(CodeGen.Formatter output, CodeGen.Interface classData, Dictionary<string, CodeGen.QualifiableType> typeDictionary)
        {
            foreach (CodeGen.Interface.Method m in classData.Methods)
            {
                if (m.IsOverload) continue;

                output.WriteIndent();
                output.Write("CALL_COUNTER_WITH_MOCK(" + m.Name + "Method, ");

                if (m.ReturnsHResult())
                {
                    output.Write("HRESULT");
                }
                else
                {
                    output.Write(typeDictionary[m.NativeReturnType].NativeName);
                }
                output.Write("(");

                for (int i=0; i<m.Parameters.Count; i++)
                {
                    output.Write(GetParameterTypeNameIncludingIndirection(m.Parameters[i], typeDictionary));

                    if (m.Parameters[i].IsArray)
                    {
                        output.Write(", UINT32");
                    }

                    if (i < m.Parameters.Count - 1) output.Write(", ");
                }
                output.Write("));");
                output.WriteLine();
            }
            output.WriteLine();
        }

        static void OutputClassMethodDefinitions(CodeGen.Formatter output, CodeGen.Interface classData, Dictionary<string, CodeGen.QualifiableType> typeDictionary)
        {
            output.WriteLine("//");
            output.WriteLine("// " + classData.NativeName);
            output.WriteLine("//");
            output.WriteLine();
            foreach (CodeGen.Interface.Method m in classData.Methods)
            {
                if (m.IsOverload) continue;

                output.WriteIndent();
                if (m.ReturnsHResult())
                {
                    output.Write("STDMETHOD(" + m.Name + ")(");
                }
                else
                {
                    output.Write("STDMETHOD_(" + typeDictionary[m.NativeReturnType].NativeName + ", " + m.Name + ")(");
                }
                output.Indent();

                for (int i = 0; i < m.Parameters.Count; i++)
                {
                    output.WriteLine();

                    string parameterString = GetParameterTypeNameIncludingIndirection(m.Parameters[i], typeDictionary) + " " + m.Parameters[i].Name;

                    output.WriteIndent();

                    output.Write(parameterString);

                    if (m.Parameters[i].IsArray)
                    {
                        output.Write(",");
                        output.WriteLine();
                        output.WriteIndent();
                        output.Write("UINT32 " + m.Parameters[i].Name + "Count");
                    }

                    if (i < m.Parameters.Count - 1)
                    {
                        output.Write(",");
                    }
                }

                string constString = m.IsConst ? " const" : "";
                output.Write(")" + constString + " override");
                output.WriteLine();

                output.Unindent();

                output.WriteLine("{");
                output.Indent();

                output.WriteIndent();

                if (m.NativeReturnType != "void")
                {
                    output.Write("return ");
                }
                output.Write(m.Name + "Method.WasCalled(");

                for (int i = 0; i < m.Parameters.Count; i++)
                {
                    output.Write(m.Parameters[i].Name);

                    if (m.Parameters[i].IsArray)
                    {
                        output.Write(", " + m.Parameters[i].Name + "Count");
                    }

                    if (i < m.Parameters.Count - 1) output.Write(", ");
                }

                output.Write(");");
                output.WriteLine();

                output.Unindent();
                output.WriteLine("}");
                output.WriteLine();
            }
        }

        static void OutputMock(string name, string inputDir, string outputDir)
        {
            CodeGen.Program.ProcessedInputFiles processedInputFiles = CodeGen.Program.ProcessInputFiles(inputDir);

            List<CodeGen.Interface> inheritanceHierarchy = new List<CodeGen.Interface>();
            GetInheritanceHierarchy("D2D1::I" + name, processedInputFiles, inheritanceHierarchy);

            // Strip the revision number out of the mock name.
            string mockTypeName = "MockD2D" + name;
            if (mockTypeName.EndsWith("1") || mockTypeName.EndsWith("2") || mockTypeName.EndsWith("3"))
            {
                mockTypeName = mockTypeName.Remove(mockTypeName.Length - 1);
            }

            string outputFileName = mockTypeName + ".h";

            string outputPath = Path.Combine(outputDir, outputFileName);

            using (CodeGen.Formatter output = new CodeGen.Formatter(outputPath))
            {
                CodeGen.OutputDataTypes.OutputLeadingComment(output);

                output.WriteLine("#pragma once");
                output.WriteLine();

                output.WriteLine("namespace canvas");
                output.WriteLine("{");
                output.Indent();

                output.WriteLine("class " + mockTypeName + " : public RuntimeClass<");
                output.Indent();
                output.WriteLine("RuntimeClassFlags<ClassicCom>,");

                output.WriteIndent();
                if (inheritanceHierarchy.Count > 1)
                {
                    output.Write("ChainInterfaces<");
                }
                for (int i = 0; i < inheritanceHierarchy.Count; i++)
                {
                    output.Write(inheritanceHierarchy[i].NativeName);
                    if (i < inheritanceHierarchy.Count - 1) output.Write(", ");
                }
                if (inheritanceHierarchy.Count > 1)
                {
                    output.Write(">");
                }
                output.Write(">");
                output.WriteLine();
                output.Unindent();
                output.WriteLine("{");
                output.WriteLine("public:");
                output.Indent();

                foreach (CodeGen.Interface classData in inheritanceHierarchy)
                {
                    OutputClassMethodMockDeclarations(output, classData, processedInputFiles.TypeDictionary);
                }
                foreach (CodeGen.Interface classData in inheritanceHierarchy)
                {
                    OutputClassMethodDefinitions(output, classData, processedInputFiles.TypeDictionary);
                }

                output.Unindent();
                output.WriteLine("};");

                output.Unindent();
                output.WriteLine("}");
            }
        }

        public static void GenerateMocks(string inputDir, string outputDir)
        {
            Directory.CreateDirectory(outputDir);

            OutputMock("GeometrySink", inputDir, outputDir);
            OutputMock("GeometryRealization", inputDir, outputDir);
            OutputMock("PathGeometry1", inputDir, outputDir);
            OutputMock("EllipseGeometry", inputDir, outputDir);
            OutputMock("RectangleGeometry", inputDir, outputDir);
            OutputMock("RoundedRectangleGeometry", inputDir, outputDir);
            OutputMock("TransformedGeometry", inputDir, outputDir);
            OutputMock("GeometryGroup", inputDir, outputDir);
            OutputMock("DrawInfo", inputDir, outputDir);
            OutputMock("EffectContext", inputDir, outputDir);
            OutputMock("TransformGraph", inputDir, outputDir);
            OutputMock("BorderTransform", inputDir, outputDir);
            OutputMock("Ink", inputDir, outputDir);
            OutputMock("InkStyle", inputDir, outputDir);
        }

        static void Main(string[] args)
        {
            var inputDir = CodeGen.Program.FindInputDirectory();
            GenerateMocks(inputDir, GetDefaultOutputLocation(inputDir));
        }
    }
}
