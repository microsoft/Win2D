// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

    public class Struct : OutputtableType
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
            
            if (overrideData != null)
            {
                if (overrideData.Guid != null)
                {
                    m_guid = overrideData.Guid;
                }

                if (overrideData.ProjectedNameOverride != null)
                {
                    if (overrideData.ShouldProject)
                        m_stylizedName = Formatter.Prefix + overrideData.ProjectedNameOverride;
                    else
                        m_stylizedName = overrideData.ProjectedNameOverride;
                }

                if (overrideData.IdlNamespaceQualifier != null)
                {
                    m_idlTypeNameQualifier = overrideData.IdlNamespaceQualifier;
                }
            }

            m_idlInterfaceName = "I" + m_stylizedName;

            m_structFields = new List<StructField>();
            foreach (XmlBindings.StructField structXmlData in xmlData.Fields)
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
        public override void OutputCode(Dictionary<string, QualifiableType> typeDictionary, Formatter idlFile)
        {
            Debug.Assert(!RequiresClassProjection(typeDictionary));
            
            OutputValueTypeProjectionCode(typeDictionary, idlFile);
        }

        bool RequiresClassProjection(Dictionary<string, QualifiableType> typeDictionary)
        {
            // Precondition: this method may only be called from OutputCode, not from
            // the constructor. If it is called from the constructor, some of the field
            // types may not be present in the dictionary yet.

            // The projection type depends on whether this struct has any field which is a ref class.
            foreach (StructField f in m_structFields)
            {
                QualifiableType fieldType = typeDictionary[f.TypeName];
                if (fieldType is Interface)
                {
                    return true;
                }
            }

            return false;
        }

        void OutputValueTypeProjectionCode(Dictionary<string, QualifiableType> typeDictionary, Formatter idlFile)
        {
            // This outputs to the IDL file only. There is no output to the .cpp file.
            idlFile.WriteLine("[version(VERSION)]");
            idlFile.WriteLine("typedef struct " + m_stylizedName);
            idlFile.WriteLine("{");
            idlFile.Indent();
            foreach (StructField structField in m_structFields)
            {
                QualifiableType typeObject = typeDictionary[structField.TypeName];
                idlFile.WriteLine(typeObject.IdlTypeNameQualifier + typeObject.ProjectedName + " " + structField.PropertyName + ";");
            }
            idlFile.Unindent();
            idlFile.WriteLine("} " + m_stylizedName + ";");
            idlFile.WriteLine();

        }
    }
}
