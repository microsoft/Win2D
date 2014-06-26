//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System.Diagnostics;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System;

namespace CodeGen
{
    namespace Overrides
    {
        namespace XmlBindings
        {
            public class Struct
            {
                public Struct()
                {
                    ShouldProject = false;
                }

                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public bool ShouldProject;

                [XmlAttributeAttribute]
                public string Guid;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlAttributeAttribute]
                public string IdlNamespaceQualifier;
            }

            public class EnumValue
            {
                public EnumValue()
                {
                    ShouldProject = true;
                }

                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlAttributeAttribute]
                public bool ShouldProject;
            }

            public class Enum
            {
                public Enum()
                {
                    ShouldProject = true;
                }

                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public bool ShouldProject;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlElement("Field")]
                public List<EnumValue> Values { get; set; }
            }

            public class Interface
            {
                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public bool IsProjectedAsAbstract;
            }

            public class Namespace
            {
                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public string NameOverride;

                [XmlElement("Struct")]
                public List<Struct> Structs { get; set; }

                [XmlElement("Enum")]
                public List<Enum> Enums { get; set; }

                [XmlElement("Interface")]
                public List<Interface> Interfaces { get; set; }
            }

            public class PrefixNode
            {
                [XmlAttributeAttribute]
                public string Value;
            }

            public class FilenameBaseNode
            {
                [XmlAttributeAttribute]
                public string Value;
            }

            public class SubnamespaceNode
            {
                [XmlAttributeAttribute]
                public string Value;
            }

            [XmlRootAttribute]
            public class Settings
            {
                [XmlElement("Namespace")]
                public List<Overrides.XmlBindings.Namespace> Namespaces { get; set; }

                [XmlElement("Struct")]
                public List<Struct> Structs { get; set; }

                [XmlElement("Prefix")]
                public PrefixNode Prefix;

                [XmlElement("FilenameBase")]
                public FilenameBaseNode FilenameBase;

                [XmlElement("Subnamespace")]
                public SubnamespaceNode Subnamespace;
            }
        }
    }
}