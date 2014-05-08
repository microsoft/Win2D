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
    namespace XmlBindings
    {
        public class StructField
        {
            // XML-bound properties. Used for deserialization.
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Type;
        }
    }

    public class StructField
    {
        public StructField(XmlBindings.StructField xmlData)
        {
            // The projected classes use a private member variable for each
            // attribute. These are given a prefix, as per convention.
            if (xmlData.Name.StartsWith("_"))
            {
                m_privateMemberName = "m" + xmlData.Name;
            }
            else
            {
                m_privateMemberName = "m_" + xmlData.Name;
            }
            m_propertyName = Formatter.StylizeWithCapitalLeadingLetter(xmlData.Name);

            m_typeName = xmlData.Type;
        }

        public string PropertyName { get { return m_propertyName; } }
        public string PrivateMemberName { get { return m_privateMemberName; } }

        public string TypeName { get { return m_typeName; } }

        string m_propertyName;
        string m_privateMemberName;
        string m_typeName;
    }

    namespace XmlBindings
    {
        public class Struct
        {
            // XML-bound properties. Used for deserialization.
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Extends;

            [XmlElement("Field")]
            public List<StructField> Fields { get; set; }
        }
    }

    class Struct : QualifiableType
    {
        public Struct(Namespace parentNamespace, XmlBindings.Struct xmlData, Overrides.XmlBindings.Struct overrideData, Dictionary<string, QualifiableType> typeDictionary, OutputDataTypes outputDataTypes)
        {
            if (parentNamespace != null)
            {
                m_rawName = parentNamespace.ApiName + "_" + xmlData.Name;
                typeDictionary[parentNamespace.RawName + "::" + xmlData.Name] = this;
            }
            else
            {
                m_rawName = xmlData.Name;
                typeDictionary[xmlData.Name] = this;
            }

            m_stylizedName = Formatter.Prefix + Formatter.StylizeNameFromUnderscoreSeparators(xmlData.Name);
            
            if(overrideData != null)
            {
                if(overrideData.Guid != null)
                {
                    m_guid = overrideData.Guid;
                }

                if(overrideData.ProjectedNameOverride != null)
                {
                    m_stylizedName = overrideData.ProjectedNameOverride;
                }

                if(overrideData.IdlNamespaceQualifier != null)
                {
                    m_idlTypeNameQualifier = overrideData.IdlNamespaceQualifier;
                }
            }

            m_idlInterfaceName = "I" + m_stylizedName;

            m_structFields = new List<StructField>();
            foreach(XmlBindings.StructField structXmlData in xmlData.Fields)
            {
                m_structFields.Add(new StructField(structXmlData));
            }
            if (xmlData.Extends != null)
            {
                m_extendsTypeName = xmlData.Extends;

                // Note: the Extends field is already qualified. See D2DTypes.xml. Example: Extends="D2D1::IImage"
                QualifiableType parentType = typeDictionary[m_extendsTypeName];

                Struct parentAsStruct = parentType as Struct; // Structs should only be deriving from struct types
                m_structFields.InsertRange(0, parentAsStruct.Fields);
                Debug.Assert(parentAsStruct.ExtendsTypeName == null); // Multiple levels in the hierarchy are not supported at this time.
            }

            // For the time being, unions are not output (they are very uncommon).
            bool usesUnions = xmlData.Fields == null;

            // Structs in the global namespace are defined as aliases only. By convention, only structs in a namespace are output.
            if (parentNamespace != null && !usesUnions  && (overrideData != null && overrideData.ShouldProject))
            {
                outputDataTypes.AddStruct(this);
            }
        }

        // The functions and member variables below are used for post-deseralization processing.
        public override string ProjectedName
        {
            get { return m_stylizedName; }
        }

        public override string NativeName
        {
            get { return m_rawName; }
        }

        public override string ProjectedNameIncludingIndirection
        {
            get { return ProjectedName; }
        }

        public override string IdlTypeNameQualifier
        {
            get { return m_idlTypeNameQualifier; }
        }
        
        public List<StructField> Fields { get { return m_structFields; } }

        public string ExtendsTypeName { get {  return m_extendsTypeName; } }

        string m_rawName;
        string m_stylizedName;
        string m_idlInterfaceName;
        string m_idlTypeNameQualifier;
        string m_guid;
        string m_extendsTypeName;
        List<StructField> m_structFields;

        // Used for code generation.
        public void OutputCode(Dictionary<string, QualifiableType> typeDictionary, OutputFiles outputFiles)
        {
            // IDL file
            outputFiles.IdlFile.WriteLine("interface " + m_idlInterfaceName + ";");
            outputFiles.IdlFile.WriteLine("runtimeclass " + m_stylizedName + ";");
            outputFiles.IdlFile.WriteLine();
            outputFiles.IdlFile.WriteLine("[uuid(" + m_guid + "), version(VERSION), exclusiveto(" + m_stylizedName + ")]");
            outputFiles.IdlFile.WriteLine("interface " + m_idlInterfaceName + " : IInspectable");
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            for (int i = 0; i < m_structFields.Count; i++)
            {
                QualifiableType typeObject = typeDictionary[m_structFields[i].TypeName];

                outputFiles.IdlFile.WriteLine("[propget] HRESULT " + m_structFields[i].PropertyName + "([out, retval] " + typeObject.IdlTypeNameQualifier + typeObject.ProjectedNameIncludingIndirection + "* value);");
                outputFiles.IdlFile.WriteLine("[propput] HRESULT " + m_structFields[i].PropertyName + "([in] " + typeObject.IdlTypeNameQualifier + typeObject.ProjectedNameIncludingIndirection + " value);");

                if(i < m_structFields.Count - 1)
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
            for (int i = 0; i < m_structFields.Count; i++)
            {
                QualifiableType typeObject = typeDictionary[m_structFields[i].TypeName];

                outputFiles.CppFile.WriteLine("IFACEMETHOD(get_" + m_structFields[i].PropertyName + ")(_Out_ " + typeObject.ProjectedNameIncludingIndirection + " *pValue) override"); ;
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
                outputFiles.CppFile.WriteLine("*pValue = " + m_structFields[i].PrivateMemberName + typeObject.AccessorSuffix + ";");
                outputFiles.CppFile.WriteLine("return S_OK;");
                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");

                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");
                outputFiles.CppFile.WriteLine();
                outputFiles.CppFile.WriteLine("IFACEMETHOD(put_" + m_structFields[i].PropertyName + ")(" + typeObject.ProjectedNameIncludingIndirection + " value) override");
                outputFiles.CppFile.WriteLine("{");
                outputFiles.CppFile.Indent();
                outputFiles.CppFile.WriteLine(m_structFields[i].PrivateMemberName + " = value;");
                outputFiles.CppFile.WriteLine("return S_OK;");
                outputFiles.CppFile.Unindent();
                outputFiles.CppFile.WriteLine("}");
                outputFiles.CppFile.WriteLine();
            }
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("private:");
            outputFiles.CppFile.Indent();
            for (int i = 0; i < m_structFields.Count; i++)
            {
                QualifiableType typeObject = typeDictionary[m_structFields[i].TypeName];
                outputFiles.CppFile.WriteLine(typeObject.RuntimeClassMemberTypeName + " " + m_structFields[i].PrivateMemberName + ";");
            }
            outputFiles.CppFile.Unindent();
            outputFiles.CppFile.WriteLine("};");
            outputFiles.CppFile.WriteLine();
        }
    }
}