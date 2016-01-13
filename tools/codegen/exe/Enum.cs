// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Xml.Serialization;

namespace CodeGen
{
    namespace XmlBindings
    {
        public class EnumValue
        {
            // XML-bound properties. Used for deserialization.
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Comment;

            [XmlAttributeAttribute]
            public string Value;

            [XmlAttributeAttribute]
            public string NumericalValue;
        }
    }

    public class EnumValue
    {
        public EnumValue(XmlBindings.EnumValue xmlData, string containingEnumName, Overrides.XmlBindings.EnumValue overrides)
        {
            m_rawNameComponent = xmlData.Name;
            m_nativeName = containingEnumName + "_" + m_rawNameComponent;
            m_stylizedName = Formatter.StylizeNameFromUnderscoreSeparators(xmlData.Name);
            m_shouldProject = true;
            m_valueExpression = GetValueExpression(xmlData);

            if (overrides != null)
            {
                if (overrides.ProjectedNameOverride != null)
                {
                    m_stylizedName = overrides.ProjectedNameOverride;
                }

                if (overrides.ProjectedValueOverride != null)
                {
                    m_valueExpression = overrides.ProjectedValueOverride;
                }

                m_shouldProject = overrides.ShouldProject;
            }
        }

        public EnumValue(string nativeName, string rawNameComponent, int valueExpression)
        {
            m_nativeName = nativeName;
            m_rawNameComponent = rawNameComponent;
            m_stylizedName = Formatter.StylizeNameFromUnderscoreSeparators(nativeName);
            m_shouldProject = true;
            m_valueExpression = valueExpression.ToString();
        }

        static string GetValueExpression(XmlBindings.EnumValue xmlData)
        {
            //
            // Value and NumericalValue seem like they should be the same, but they're not. 
            //
            // Value may be an int literal, or some other symbol.
            // NumericalValue is always an int literal (may be hex or dec)
            //
            // NumericalValue is used for Direct2D's internal codegen, in cases where the 
            // symbol used for Value can't be referenced in the event manifest. 
            //
            // And so, the int literal is decided according to the following priority
            // 1. NumericalValue if avail
            // 2. Value, must always be parseable as int literal
            //

            if (xmlData.NumericalValue != null)
            {
                return xmlData.NumericalValue;
            }
            else
            {
                Debug.Assert(xmlData.Value != null);
                return xmlData.Value;
            }
        }

        public void OutputCode(bool isLast, bool isFlags, Formatter idlFile)
        {
            if (!m_shouldProject) return;

            idlFile.WriteIndent();

            //
            // The (int) cast is necessary for values such as "0x80000000" which
            // cannot be stored using a signed int. The default behavior of the
            // MIDL code generation/C++ compilation process is to treat enum
            // values as signed ints.
            //
            idlFile.Write(m_stylizedName);
            idlFile.Write(" = ");

            if (!isFlags)
            {
                idlFile.Write("(int)");
            }

            idlFile.Write(m_valueExpression);

            string suffix = isLast ? "" : ",";
            idlFile.Write(suffix);
            idlFile.WriteLine();
        }

        public string RawNameComponent { get { return m_rawNameComponent; } }

        public string NativeName { get { return m_nativeName; } }

        public string ValueExpression { get { return m_valueExpression; } }

        string m_rawNameComponent;
        string m_nativeName;
        string m_stylizedName;
        string m_valueExpression;
        bool m_shouldProject;

    }

    namespace XmlBindings
    {
        public class Enum
        {
            // XML-bound properties. Used for deserialization.
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Comment;

            [XmlAttributeAttribute]
            public bool IsFlags;

            [XmlElement("Field")]
            public XmlBindings.EnumValue[] EnumValues { get; set; }
        }
    }

    public class Enum : OutputtableType
    {
        class EnumValueComparer : IComparer<EnumValue>  
        {
            private int ParseValueExpression(string valueExpression)
            {
                NumberStyles numberStyle = NumberStyles.Any;
                if (valueExpression.StartsWith("0x"))
                {
                    valueExpression = valueExpression.Remove(0, 2);
                    numberStyle = NumberStyles.HexNumber;
                }
                return int.Parse(valueExpression, numberStyle);
            }

            public int Compare(EnumValue x, EnumValue y)
            {
                int valueX = ParseValueExpression(x.ValueExpression);
                int valueY = ParseValueExpression(y.ValueExpression);
                if (valueX < valueY) return -1;
                else if (valueX > valueY) return 1;
                else return 0;
            }
        }

        public Enum(Namespace parentNamespace, string rootProjectedNamespace, XmlBindings.Enum xmlData, Overrides.XmlBindings.Enum overrides, Dictionary<string, QualifiableType> typeDictionary, OutputDataTypes outputDataTypes)
        {
            m_stylizedName = Formatter.Prefix + Formatter.StylizeNameFromUnderscoreSeparators(xmlData.Name);

            if (parentNamespace != null)
            {
                m_rawName = parentNamespace.ApiName + "_" + xmlData.Name;
                typeDictionary[parentNamespace.RawName + "::" + xmlData.Name] = this;
            }
            else
            {
                //
                // Namespace of NULL indicates the global namespace.
                // These types aren't D2D types, and their full native name is
                // exactly what's in the name field (no need to prepend anything).
                //
                m_rawName = xmlData.Name;
                typeDictionary[xmlData.Name] = this;
            }

            m_isFlags = xmlData.IsFlags;

            m_enumValues = new List<EnumValue>();
            foreach (XmlBindings.EnumValue valueXml in xmlData.EnumValues)
            {
                Overrides.XmlBindings.EnumValue overridesEnumValue = null;
                if (overrides != null) overridesEnumValue = overrides.Values.Find(x => x.Name == valueXml.Name);

                m_enumValues.Add(new EnumValue(valueXml, m_rawName, overridesEnumValue));
            }

            Namespace = rootProjectedNamespace;
            
            bool shouldProject = false;
            if (overrides != null)
            {
                shouldProject = overrides.ShouldProject;

                if (overrides.ProjectedNameOverride != null)
                {
                    m_stylizedName = Formatter.Prefix + overrides.ProjectedNameOverride;
                } 

                if (overrides.Namespace != null)
                {
                    Namespace = Namespace + "." + overrides.Namespace;
                }

            }

            // One of the XML files has a mistake where it doesn't properly order its enums.
            if (m_isFlags)
            {
                m_enumValues.Sort(new EnumValueComparer());
            }

            // Enums in the global namespace are defined as aliases only. By convention, only enums in a namespace are output.
            if (parentNamespace != null && shouldProject)
            {
                outputDataTypes.AddEnum(this);
            }

        }
        public Enum(string rawName, List<EnumValue> values, Dictionary<string, QualifiableType> typeDictionary)
        {
            m_rawName = rawName;
            typeDictionary[rawName] = this;
            m_stylizedName = Formatter.Prefix + Formatter.StylizeNameFromUnderscoreSeparators(rawName);

            m_isFlags = false;

            m_enumValues = values;
        }

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
            get { return m_stylizedName; }
        }

        public List<EnumValue> Values
        {
            get { return m_enumValues; }
        }

        // Used for code generation.
        public override void OutputCode(Dictionary<string, QualifiableType> typeDictionary, Formatter idlFile)
        {
            idlFile.WriteIndent();
            idlFile.Write("[version(VERSION)");
            if (m_isFlags)
            {
                idlFile.Write(", flags");
            }
            idlFile.Write("]");
            idlFile.WriteLine();
            
            idlFile.WriteLine("typedef enum " + m_stylizedName);
            idlFile.WriteLine("{");
            idlFile.Indent();

            for (int i = 0; i < m_enumValues.Count; i++)
            {
                bool isLast = i == m_enumValues.Count - 1;
                m_enumValues[i].OutputCode(isLast, m_isFlags, idlFile);

            }
            idlFile.Unindent();
            idlFile.WriteLine("} " + m_stylizedName + ";");
            idlFile.WriteLine();
        }

        private string m_rawName;
        private string m_stylizedName;
        private List<EnumValue> m_enumValues;
        private bool m_isFlags;
    }

}
