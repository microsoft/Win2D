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

        // The codegen does not really need to understand what primitives are,
        // but it needs the capacity to override them and arbitrarily replace them
        // with WinRT types.
        [XmlElement("Primitive")]
        public List<Primitive> Primitives { get; set; }

        // Structs, enums, and interfaces stored directly to D2DTypes (the root node) are in the global namespace.
        [XmlElement("Enum")]
        public List<Enum> Enums { get; set; }

        [XmlElement("Struct")]
        public List<Struct> Structs { get; set; }

        [XmlElement("Interface")]
        public List<Interface> Interfaces { get; set; }

        // The functions below are used for some post-deseralization processing.

        public static Namespace GetNamespaceByName(List<Namespace> list, string name)
        {
            foreach(Namespace n in list)
            {
                if(n.Name == name)
                {
                    return n;
                }
            }

            Debug.Assert(false); // Unexpected; indicates malformed XML
            return null;
        }

        static List<Type> CombineLists_MayBeNull<Type>(List<Type> source, List<Type> dest)
        {
            //
            // The XML deserialization will leave some lists as null if the XML document had no
            // elements for them.
            //
            if (dest == null) return source;

            if (source == null) return dest;

            dest.AddRange(source);
            return dest;
        }

        static List<Namespace> CombineLists_MayBeNull(List<Namespace> source, List<Namespace> dest)
        {
            // This specialization merges items in namespaces. The same namespace may be referenced across 
            // multiple documents.

            if (dest == null) return source;

            if (source == null) return dest;

            foreach (Namespace sourceElement in source)
            {
                Namespace destElement = GetNamespaceByName(dest, sourceElement.Name);
                destElement.Enums.AddRange(sourceElement.Enums);
                destElement.Structs.AddRange(sourceElement.Structs);
                destElement.Interfaces.AddRange(sourceElement.Interfaces);
            }

            return dest;

        }

        public void AddTypes(D2DTypes other)
        {
            Namespaces = CombineLists_MayBeNull(other.Namespaces, Namespaces);
            Enums = CombineLists_MayBeNull(other.Enums, Enums);
            Structs = CombineLists_MayBeNull(other.Structs, Structs);
            Interfaces = CombineLists_MayBeNull(other.Interfaces, Interfaces);
            Primitives = CombineLists_MayBeNull(other.Primitives, Primitives);
        }

        public void CommitAllTypesToDictionary(Dictionary<string, QualifiableType> types)
        {
            if (Primitives != null)
            {
                foreach (Primitive p in Primitives)
                {
                    types[p.Name] = p;
                }
            }

            if(Enums != null)
            {
                foreach(Enum e in Enums)
                {
                    e.Commit(null); // null indicates the global namespace
                    types[e.Name] = e;
                }
            }

            if (Structs != null)
            {
                foreach (Struct s in Structs)
                {
                    s.Commit(null);
                    types[s.Name] = s;
                }
            }

            if (Interfaces != null)
            {
                foreach (Interface i in Interfaces)
                {
                    i.Commit(null, "", types);
                    types[i.Name] = i;
                }
            }

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
            if (Structs != null)
            {
                foreach (Struct s in Structs)
                {
                    s.Resolve(types);
                }
            }

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
