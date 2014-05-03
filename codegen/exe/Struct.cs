//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System.Diagnostics;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CodeGen
{
    public class StructField
    {
        // XML-bound properties. Used for deserialization.
        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string Type;
        
        // The functions and member variables below are used for post-deseralization processing.
        public void Commit()
        {
            // The projected classes use a private member variable for each
            // attribute. These are given a prefix, as per convention.
            if (Name.StartsWith("_"))
            {
                m_privateMemberName = "m" + Name;
            }
            else
            {
                m_privateMemberName = "m_" + Name;
            }
            m_propertyName = Formatter.StylizeWithCapitalLeadingLetter(Name);
        }

        public void Resolve(Dictionary<string, QualifiableType> types)
        {
            // For the time being, array types are not supported. This
            // means that the Type attribute must be defined.
            Debug.Assert(Type != null);

            if (types.ContainsKey(Type))
            {
                m_type = types[Type];
            }
        }
        public string PropertyName { get { return m_propertyName; } }
        public string PrivateMemberName { get { return m_privateMemberName; } }

        public QualifiableType TypeObject { get {  return m_type; } }

        QualifiableType m_type;
        string m_propertyName;
        string m_privateMemberName;
    }

    public class Struct : QualifiableType
    {
        // XML-bound properties. Used for deserialization.
        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string Extends;

        [XmlElement("Field")]
        public List<StructField> Fields { get; set; }

        // Stores the overrides from Settings.xml.
        public StructOverrides Overrides;

        // The functions and member variables below are used for post-deseralization processing.
        public override string ProjectedName
        {
            get { return m_stylizedName; }
        }

        public override string NativeName
        {
            get { return m_rawName; }
        }

        public void Commit(Namespace parentNamespace)
        {
            m_rawName = parentNamespace.ApiName + "_" + Name;

            foreach(StructField sf in Fields)
            {
                sf.Commit();
            }

            Overrides = new StructOverrides();

        }
        public void Resolve(Dictionary<string, QualifiableType> types)
        {
            if (!Overrides.ShouldProject)
            {
                return;
            }

            // If this struct inherits from anything, commit those fields as well.
            if(Extends != null)
            {
                QualifiableType parent = types[Extends];
                Struct parentStruct = parent as Struct; // Structs should only be deriving from struct types

                Fields.InsertRange(0, parentStruct.Fields);
                Debug.Assert(parentStruct.Extends == null); // Multiple levels in the hierarchy are not supported at this time.
            }

            if (Fields != null)
            {
                foreach (StructField sf in Fields)
                {
                    sf.Resolve(types);
                }
            }

            if (Overrides.ProjectedNameOverride != null)
            {
                m_stylizedName = Overrides.ProjectedNameOverride;
            }
            else
            {
                m_stylizedName = Formatter.Prefix + Formatter.StylizeNameFromUnderscoreSeparators(Name);
            }

            m_idlInterfaceName = "I" + m_stylizedName;
        }

        string m_rawName;
        string m_stylizedName;
        string m_idlInterfaceName;

        // Used for code generation.
        public void OutputCode(OutputFiles outputFiles)
        {
            if (!Overrides.ShouldProject)
            {
                return;
            }

            if (Fields == null) // Skips over unions, which are not handled currently
            {
                return;
            }

            // IDL file
            outputFiles.IdlFile.WriteLine("interface " + m_idlInterfaceName + ";");
            outputFiles.IdlFile.WriteLine("runtimeclass " + m_stylizedName + ";");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("[uuid(" + Overrides.Guid + "), version(VERSION), exclusiveto(" + m_stylizedName + ")]");
            outputFiles.IdlFile.WriteLine("interface " + m_idlInterfaceName + " : IInspectable");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            for (int i = 0; i < Fields.Count; i++)
            {
                // TODO: Use the correct types for struct fields. Types are always treated as int for the time being.
                outputFiles.IdlFile.WriteLine("// TODO: Use the correct types for struct fields. Types are always treated as int for the time being.");

                outputFiles.IdlFile.WriteLine("[propget] HRESULT " + Fields[i].PropertyName + "([out, retval] int* value);");
                outputFiles.IdlFile.WriteLine("[propput] HRESULT " + Fields[i].PropertyName + "([in] int value);");

                if(i < Fields.Count - 1)
                {
                    outputFiles.IdlFile.WriteLine();
                }
            }

            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("[version(VERSION), activatable(VERSION)]");
            outputFiles.IdlFile.WriteLine("runtimeclass " + m_stylizedName + "");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();
            outputFiles.IdlFile.WriteLine("[default] interface " + m_idlInterfaceName + ";");
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("}");
            outputFiles.IdlFile.WriteLine();

            // CPP file
            outputFiles.CppFile.WriteLine("class " + m_stylizedName + " : public Microsoft::WRL::RuntimeClass<" + m_idlInterfaceName + ">");
            outputFiles.CppFile.WriteLine("{");

            outputFiles.CppFile.Indent();
            outputFiles.CppFile.WriteLine("InspectableClass(L\"Microsoft.DirectX." + m_stylizedName + "\", BaseTrust);");
            outputFiles.CppFile.WriteLine();
            outputFiles.CppFile.Unindent();

            outputFiles.CppFile.WriteLine("public:");
            outputFiles.CppFile.Indent();
            for (int i = 0; i < Fields.Count; i++)
            {
                outputFiles.CppFile.WriteLine("IFACEMETHOD(get_" + Fields[i].PropertyName + ")(_Out_ int *pValue) override"); ;
                outputFiles.CppFile.WriteLine("{");
                outputFiles.CppFile.Indent();

                outputFiles.CppFile.WriteLine("if (pValue == nullptr)");
                outputFiles.CppFile.WriteLine("{");
                outputFiles.CppFile.Indent();
                outputFiles.CppFile.WriteLine("return E_POINTER;");
                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");
                outputFiles.CppFile.WriteLine("else");
                outputFiles.CppFile.WriteLine("{");
                outputFiles.CppFile.Indent();
                outputFiles.CppFile.WriteLine("*pValue = " + Fields[i].PrivateMemberName + ";");
                outputFiles.CppFile.WriteLine("return S_OK;");
                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");

                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");
                outputFiles.CppFile.WriteLine();
                outputFiles.CppFile.WriteLine("IFACEMETHOD(put_" + Fields[i].PropertyName + ")(int value) override"); ;
                outputFiles.CppFile.WriteLine("{");
                outputFiles.CppFile.Indent();
                outputFiles.CppFile.WriteLine(Fields[i].PrivateMemberName + " = value;");
                outputFiles.CppFile.WriteLine("return S_OK;");
                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");
                outputFiles.CppFile.WriteLine();
            }
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("private:");
            outputFiles.CppFile.Indent();
            for (int i = 0; i < Fields.Count; i++)
            {
                outputFiles.CppFile.WriteLine("int " + Fields[i].PrivateMemberName + ";");
            }
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("};");
            outputFiles.CppFile.WriteLine();
        }
    }
}