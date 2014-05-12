//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml.Serialization;
using System.Globalization;
using System.Diagnostics;

namespace CodeGen
{
    struct OutputFiles
    {
        public Formatter CppFile;
        public Formatter IdlFile;
    }

    class OutputDataTypes
    {
        public OutputDataTypes()
        {
            m_enums = new List<Enum>();
            m_structs = new List<Struct>();
        }

        public void AddEnum(Enum e)
        {
            m_enums.Add(e);
        }

        public void AddStruct(Struct s)
        {
            m_structs.Add(s);
        }

        public void OutputCode(Dictionary<string, QualifiableType> typeDictionary, OutputFiles outputFiles)
        {
            OutputLeadingCode(outputFiles);

            foreach(Enum e in m_enums)
            {
                e.OutputCode(outputFiles);
            }

            foreach(Struct s in m_structs)
            {
                s.OutputCode(typeDictionary, outputFiles);
            }

            OutputEndingCode(outputFiles);
        }

        private void OutputLeadingCode(OutputFiles outputFiles)
        {
            outputFiles.CppFile.WriteLine("//");
            outputFiles.CppFile.WriteLine("// Copyright (c) Microsoft Corporation.  All rights reserved.");
            outputFiles.CppFile.WriteLine("//");
            outputFiles.CppFile.WriteLine("// This file was automatically generated. Please do not edit it manually.");
            outputFiles.CppFile.WriteLine("//");
            outputFiles.CppFile.WriteLine();

            outputFiles.CppFile.WriteLine("#include \"pch.h\"");
            outputFiles.CppFile.WriteLine("#include \"d2dwinrt.h\"");
            outputFiles.CppFile.WriteLine("#include <wrl.h>");
            outputFiles.CppFile.WriteLine("using namespace Microsoft::WRL;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Microsoft::DirectX;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Windows::Foundation;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Windows::UI;");
            outputFiles.CppFile.WriteLine();
            outputFiles.CppFile.WriteLine("// This is placeholder until the inbox math types are ready to use.");
            outputFiles.CppFile.WriteLine("typedef int Matrix3x2;");
            outputFiles.CppFile.WriteLine();
            outputFiles.CppFile.WriteLine("namespace dxrt");
            outputFiles.CppFile.WriteLine("{");
            outputFiles.CppFile.Indent();

            outputFiles.IdlFile.WriteLine("import \"inspectable.idl\";");
            outputFiles.IdlFile.WriteLine("import \"Windows.Foundation.idl\";");
            outputFiles.IdlFile.WriteLine("import \"Windows.UI.idl\";");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("#include \"version.h\"");
            outputFiles.IdlFile.WriteLine();

            outputFiles.IdlFile.WriteLine("namespace Microsoft");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            // Placeholder for math types
            outputFiles.IdlFile.WriteLine("// This is placeholder, until the inbox math types are ready to use.");
            outputFiles.IdlFile.WriteLine("namespace Math");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();
            outputFiles.IdlFile.WriteLine("typedef int Matrix3x2;");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.WriteLine();

            outputFiles.IdlFile.WriteLine("namespace DirectX");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            // Declaration of abstract types

            outputFiles.IdlFile.WriteLine("//");
            outputFiles.IdlFile.WriteLine("// IDxGeometry");
            outputFiles.IdlFile.WriteLine("//");
            outputFiles.IdlFile.WriteLine("[uuid(A0D3D5DC-A2BD-4E5A-AA75-C13B8FF238C2), version(VERSION)]");
            outputFiles.IdlFile.WriteLine("interface IDxGeometry : IInspectable");
            outputFiles.IdlFile.Indent();
            outputFiles.IdlFile.WriteLine("requires Windows.Foundation.IClosable");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.WriteLine();

            outputFiles.IdlFile.WriteLine("//");
            outputFiles.IdlFile.WriteLine("// IDxBrush");
            outputFiles.IdlFile.WriteLine("//");
            outputFiles.IdlFile.WriteLine("[uuid(718BAB7E-2A06-46DB-8127-11F65291554E), version(VERSION)]");
            outputFiles.IdlFile.WriteLine("interface IDxBrush : IInspectable");
            outputFiles.IdlFile.Indent();
            outputFiles.IdlFile.WriteLine("requires Windows.Foundation.IClosable");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.WriteLine();
        }

        private void OutputEndingCode(OutputFiles outputFiles)
        {
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("}");

            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
        }

        List<Enum> m_enums;
        List<Struct> m_structs;
    }
    
    public class Program
    {
        static void Main(string[] args)
        {
            GenerateCode(GetDefaultOutputLocation());
        }
        
        public static string GetDefaultOutputLocation()
        {
            return Path.GetFullPath(Path.Combine(FindInputDirectory(), "..", "..", "winrt", "lib"));
        }

        public static void GenerateCode(string outputDir)
        {
            var filenameBase = "DirectX";

            var inputDir = FindInputDirectory();

            List<string> files = new List<string>();
            files.Add("D2DTypes.xml");
            files.Add("D2DTypes2.xml");
            files.Add("D2DEffectAuthor.xml");
            files.Add("D2DTypes3.xml");

            Overrides.XmlBindings.Settings overridesXmlData = XmlBindings.Utilities.LoadXmlData<Overrides.XmlBindings.Settings>(inputDir, "Settings.xml");
            Formatter.Prefix = overridesXmlData.Prefix.Value;

            List<D2DTypes> typeDocuments = new List<D2DTypes>();
            Dictionary<string, QualifiableType> typeDictionary = new Dictionary<string, QualifiableType>();
            OutputDataTypes outputDataTypes = new OutputDataTypes();
            foreach (string fileName in files)
            {
                XmlBindings.D2DTypes xmlDocument = XmlBindings.Utilities.LoadXmlData<XmlBindings.D2DTypes>(inputDir, fileName);
                typeDocuments.Add(new D2DTypes(xmlDocument, overridesXmlData, typeDictionary, outputDataTypes));
            }

            Directory.CreateDirectory(outputDir);
            OutputFiles outputFiles = new OutputFiles();
            using (Formatter cppStreamWriter = new Formatter(Path.Combine(outputDir, filenameBase + ".codegen.cpp")),
                             idlStreamWriter = new Formatter(Path.Combine(outputDir, filenameBase + ".codegen.idl")))
            {
                outputFiles.CppFile = cppStreamWriter;
                outputFiles.IdlFile = idlStreamWriter;

                outputDataTypes.OutputCode(typeDictionary, outputFiles);
            }
        }

        //
        // The input directory, which contains the various XML spec files, can be found by checking some known locations.
        //
        static string FindInputDirectory()
        {
            string[] candidates = new string[] {
                ".",
                "codegen/exe",
                "../../../../codegen/exe"
            };

            foreach (var candidate in candidates)
            {
                var normalizedCandidate = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, candidate));
                if (IsInputDirectory(normalizedCandidate))
                    return normalizedCandidate;
            }

            throw new FileNotFoundException("Couldn't find the codegen source directory");
        }

        //
        // The input directory is that one that contains Settings.xml.
        //
        static bool IsInputDirectory(string directory)
        {
            return File.Exists(Path.Combine(directory, "Settings.xml"));
        }

    }
}
