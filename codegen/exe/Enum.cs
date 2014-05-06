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

        // Contains a possible override from Settings.xml.
        public string ProjectedNameOverride;
        
        // The functions and member variables below are used for post-deseralization processing.
        public void Commit(Enum parent)
        {
            m_stylizedName = Formatter.StylizeNameFromUnderscoreSeparators(Name);
            m_parent = parent;
        }

        public void Resolve()
        {
            if(ProjectedNameOverride != null)
            {
                m_stylizedName = ProjectedNameOverride;
            }
        }

        string GetValueAsString()
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

            if (NumericalValue != null)
            {
                return NumericalValue;
            }
            else
            {
                Debug.Assert(Value != null);
                return Value;
            }

        }

        // Used for code generation.
        public void OutputCode(bool isLast, bool isFlags, OutputFiles outputFiles)
        {
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

            outputFiles.IdlFile.Write(GetValueAsString());

            string suffix = isLast ? "" : ",";
            outputFiles.IdlFile.Write(suffix);
            outputFiles.IdlFile.WriteLine();
        }

        Enum m_parent;
        string m_stylizedName;
    }

    public class Enum : QualifiableType
    {
        // XML-bound properties. Used for deserialization.
        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string Comment;

        [XmlAttributeAttribute]
        public bool IsFlags;

        [XmlElement("Field")]
        public EnumValue[] EnumValues { get; set; }

        // Stores the overrides from Settings.xml.
        public EnumOverrides Overrides;

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
            get
            {
                return ProjectedName;
            }
        }

        Dictionary<string, EnumValue> m_enumValueLookupByName;

        public EnumValue GetEnumValueByName(string name)
        {
            return m_enumValueLookupByName[name];
        }

        public void Commit(Namespace parentNamespace)
        {
            if (parentNamespace != null)
            {
                m_rawName =  parentNamespace.ApiName + "_" + Name;
            }
            else
            {
                //
                // Namespace of NULL indicates the global namespace.
                // These types aren't D2D types, and their full native name is
                // exactly what's in the name field (no need to prepend anything).
                //
                m_rawName = Name; 
            }

            m_enumValueLookupByName = new Dictionary<string, EnumValue>();

            if (EnumValues != null)
            {
                foreach (EnumValue enumValue in EnumValues)
                {
                    m_enumValueLookupByName[enumValue.Name] = enumValue;
                    enumValue.Commit(this);
                }
            
            }

            Overrides = new EnumOverrides();
        }

        public void Resolve()
        {
            if (!Overrides.ShouldProject)
            {
                return;
            }

            if (Overrides.ProjectedNameOverride != null)
            {
                m_stylizedName = Overrides.ProjectedNameOverride;
            }
            else
            {
                m_stylizedName = Formatter.Prefix + Formatter.StylizeNameFromUnderscoreSeparators(Name);
            }

            if (EnumValues != null)
            {
                foreach (EnumValue enumValue in EnumValues)
                {
                    enumValue.Resolve();
                }
            }
        }

        // Used for code generation.
        public void OutputCode(OutputFiles outputFiles)
        {
            if (!Overrides.ShouldProject)
            {
                return;
            }

            outputFiles.IdlFile.WriteIndent();
            outputFiles.IdlFile.Write("[version(VERSION)");
            if (IsFlags)
            {
                outputFiles.IdlFile.Write(", flags");
            }
            outputFiles.IdlFile.Write("]");
            outputFiles.IdlFile.WriteLine();
            
            outputFiles.IdlFile.WriteLine("typedef enum " + m_stylizedName);
            outputFiles.IdlFile.WriteLine("{");
            outputFiles.IdlFile.Indent();

            for (int i = 0; i < EnumValues.Length; i++)
            {
                bool isLast = i == EnumValues.Length - 1;
                EnumValues[i].OutputCode(isLast, IsFlags, outputFiles);

            }
            outputFiles.IdlFile.Unindent();
            outputFiles.IdlFile.WriteLine("} " + m_stylizedName + ";");
            outputFiles.IdlFile.WriteLine();
        }

        private string m_rawName;
        private string m_stylizedName;
    }

}