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
            OutputLeadingComment(outputFiles.CppFile);

            outputFiles.CppFile.WriteLine("#include \"pch.h\"");
            outputFiles.CppFile.WriteLine("#include <wrl.h>");
            outputFiles.CppFile.WriteLine("using namespace Microsoft::WRL;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Microsoft::Graphics::Canvas;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Microsoft::Graphics::Canvas::Numerics;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Windows::Foundation;");
            outputFiles.CppFile.WriteLine("using namespace ABI::Windows::UI;");
            outputFiles.CppFile.WriteLine();
            outputFiles.CppFile.WriteLine("namespace canvas");
            outputFiles.CppFile.WriteLine("{");
            outputFiles.CppFile.Indent();

            OutputLeadingComment(outputFiles.IdlFile);

            outputFiles.IdlFile.WriteLine("import \"inspectable.idl\";");
            outputFiles.IdlFile.WriteLine("import \"Windows.Foundation.idl\";");
            outputFiles.IdlFile.WriteLine("import \"Windows.UI.idl\";");
            outputFiles.IdlFile.WriteLine("import \"WinRTNumerics.idl\";");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("#include \"version.h\"");
            outputFiles.IdlFile.WriteLine();

            outputFiles.IdlFile.WriteLine("namespace Microsoft.Graphics.Canvas");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();
        }

        private void OutputLeadingComment(Formatter output)
        {
            output.WriteLine("// Copyright (c) Microsoft Corporation. All rights reserved.");
            output.WriteLine("//");
            output.WriteLine("// Licensed under the Apache License, Version 2.0 (the \"License\"); you may");
            output.WriteLine("// not use these files except in compliance with the License. You may obtain");
            output.WriteLine("// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0");
            output.WriteLine("//");
            output.WriteLine("// Unless required by applicable law or agreed to in writing, software");
            output.WriteLine("// distributed under the License is distributed on an \"AS IS\" BASIS, WITHOUT");
            output.WriteLine("// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the");
            output.WriteLine("// License for the specific language governing permissions and limitations");
            output.WriteLine("// under the License.");
            output.WriteLine();

            output.WriteLine("// This file was automatically generated. Please do not edit it manually.");
            output.WriteLine();
        }

        private void OutputEndingCode(OutputFiles outputFiles)
        {
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("}");

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
            var inputDir = FindInputDirectory();

            GenerateCode(inputDir, GetDefaultOutputLocation(inputDir));
        }
        
        public static string GetDefaultOutputLocation(string inputDir)
        {
            return Path.GetFullPath(Path.Combine(inputDir, "..", "..", "..", "winrt", "lib"));
        }

        public static void GenerateCode(string inputDir, string outputDir)
        {
            List<string> files = new List<string>();
            files.Add("D2DTypes.xml");
            files.Add("D2DTypes2.xml");
            files.Add("D2DEffectAuthor.xml");
            files.Add("D2DTypes3.xml");

            Overrides.XmlBindings.Settings overridesXmlData = XmlBindings.Utilities.LoadXmlData<Overrides.XmlBindings.Settings>(inputDir, "Settings.xml");
            Formatter.Prefix = overridesXmlData.Prefix.Value;
            Formatter.Subnamespace = overridesXmlData.Subnamespace.Value;

            var filenameBase = overridesXmlData.FilenameBase.Value;

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
                "../../../../tools/codegen/exe",
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
