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

using System.Diagnostics;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;
using System.IO;

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

    class EnumValue
    {
        public EnumValue(XmlBindings.EnumValue xmlData, Overrides.XmlBindings.EnumValue overrides)
        {
            m_stylizedName = Formatter.StylizeNameFromUnderscoreSeparators(xmlData.Name);
            m_shouldProject = true;

            if (overrides != null)
            {
                if (overrides.ProjectedNameOverride != null)
                {
                    m_stylizedName = overrides.ProjectedNameOverride;
                }

                m_shouldProject = overrides.ShouldProject;
            }

            m_valueExpression = GetValueExpression(xmlData);

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

        public void OutputCode(bool isLast, bool isFlags, OutputFiles outputFiles)
        {
            if (!m_shouldProject) return;

            outputFiles.IdlFile.WriteIndent();

            //
            // The (int) cast is necessary for values such as "0x80000000" which
            // cannot be stored using a signed int. The default behavior of the
            // MIDL code generation/C++ compilation process is to treat enum
            // values as signed ints.
            //
            outputFiles.IdlFile.Write(m_stylizedName);
            outputFiles.IdlFile.Write(" = ");

            if(!isFlags)
            {
                outputFiles.IdlFile.Write("(int)");
            }

            outputFiles.IdlFile.Write(m_valueExpression);

            string suffix = isLast ? "" : ",";
            outputFiles.IdlFile.Write(suffix);
            outputFiles.IdlFile.WriteLine();
        }
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

    class Enum : QualifiableType
    {
        public Enum(Namespace parentNamespace, XmlBindings.Enum xmlData, Overrides.XmlBindings.Enum overrides, Dictionary<string, QualifiableType> typeDictionary, OutputDataTypes outputDataTypes)
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

                m_enumValues.Add(new EnumValue(valueXml, overridesEnumValue));
            }

            bool shouldProject = false;
            if(overrides != null)
            {
                shouldProject = overrides.ShouldProject;

                if(overrides.ProjectedNameOverride != null)
                {
                    m_stylizedName = Formatter.Prefix + overrides.ProjectedNameOverride;
                }

            }

            // Enums in the global namespace are defined as aliases only. By convention, only enums in a namespace are output.
            if (parentNamespace != null && shouldProject)
            {
                outputDataTypes.AddEnum(this);
            }

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

        // Used for code generation.
        public void OutputCode(OutputFiles outputFiles)
        {
            outputFiles.IdlFile.WriteIndent();
            outputFiles.IdlFile.Write("[version(VERSION)");
            if (m_isFlags)
            {
                outputFiles.IdlFile.Write(", flags");
            }
            outputFiles.IdlFile.Write("]");
            outputFiles.IdlFile.WriteLine();
            
            outputFiles.IdlFile.WriteLine("typedef enum " + m_stylizedName);
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            for (int i = 0; i < m_enumValues.Count; i++)
            {
                bool isLast = i == m_enumValues.Count - 1;
                m_enumValues[i].OutputCode(isLast, m_isFlags, outputFiles);

            }
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("} " + m_stylizedName + ";");
            outputFiles.IdlFile.WriteLine();
        }

        private string m_rawName;
        private string m_stylizedName;
        private List<EnumValue> m_enumValues;
        private bool m_isFlags;
    }

}
