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
    [XmlRootAttribute]
    public class D2DTypes 
    {
        // XML-bound property. Used for deserialization.
        [XmlElement("Namespace")]
        public List<Namespace> Namespaces { get; set; }


        // The functions below are used for some post-deseralization processing.
        public Namespace GetNamespace(string name)
        {
            foreach (Namespace n in Namespaces)
            {
                if(name == n.Name)
                {
                    return n;
                }
            }
            // Reaching here means that the input XML is malformed.
            Debug.Assert(false);
            return null;
        }

        public void AddTypes(D2DTypes other)
        {
            if (other.Namespaces != null)
            {
                if (Namespaces == null)
                {
                    Namespaces = other.Namespaces;
                }
                else
                {
                    foreach (Namespace otherNamespace in other.Namespaces)
                    {
                        Namespace n = GetNamespace(otherNamespace.Name);
                        n.Enums.AddRange(otherNamespace.Enums);
                        n.Structs.AddRange(otherNamespace.Structs);
                        n.Interfaces.AddRange(otherNamespace.Interfaces);
                        n.Typedefs.AddRange(otherNamespace.Typedefs);
                    }
                }
            }
        }

        public void CommitAllTypesToDictionary(Dictionary<string, QualifiableType> types)
        {
            if (Namespaces == null)
            {
                return;
            }

            foreach (Namespace n in Namespaces)
            {
                n.Commit(types);
            }
        }

        public void ResolveTypesAgainstDictionary(Dictionary<string, QualifiableType> types)
        {
            if (Namespaces == null)
            {
                return;
            }

            foreach (Namespace n in Namespaces)
            {
                n.Resolve(types);
            }
        }

        // The functions below are used for code generation.
        void OutputLeadingCode(OutputFiles outputFiles)
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
            outputFiles.CppFile.WriteLine();
            outputFiles.CppFile.WriteLine("namespace dxrt");
            outputFiles.CppFile.WriteLine("{");
            outputFiles.CppFile.Indent();

            outputFiles.IdlFile.WriteLine("import \"inspectable.idl\";");
            outputFiles.IdlFile.WriteLine("import \"Windows.Foundation.idl\";");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("#include \"version.h\"");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("namespace Microsoft");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();
            outputFiles.IdlFile.WriteLine("namespace DirectX");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();
        }

        void OutputEndingCode(OutputFiles outputFiles)
        {
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("}");

            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
        }

        public void OutputCode(OutputFiles outputFiles)
        {
            OutputLeadingCode(outputFiles);

            // Output all enums first, then structs.

            if (Namespaces != null)
            {
                foreach (Namespace n in Namespaces)
                {
                    if (n.Enums != null)
                    {
                        foreach (Enum e in n.Enums)
                        {
                            e.OutputCode(outputFiles);
                        }
                    }
                }

                foreach (Namespace n in Namespaces)
                {
                    if (n.Structs != null)
                    {
                        foreach (Struct s in n.Structs)
                        {
                            s.OutputCode(outputFiles);
                        }
                    }
                }
            }

            OutputEndingCode(outputFiles);
        }
    }
}
