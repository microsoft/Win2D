// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
                public string Index;

                [XmlAttributeAttribute]
                public bool ShouldProject;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlAttributeAttribute]
                public string ProjectedValueOverride;
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

                [XmlAttributeAttribute]
                public string Namespace;

                [XmlAttributeAttribute]
                public string WinVer;

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
            
            public class EffectProperty
            {
                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlAttributeAttribute]
                public string DefaultValueOverride;

                [XmlAttributeAttribute]
                public bool IsHidden;

                [XmlAttributeAttribute]
                public bool IsHandCoded;

                [XmlAttributeAttribute]
                public bool ConvertRadiansToDegrees;

                [XmlAttributeAttribute]
                public string Type;
            }

            public class Effect
            {
                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;

                [XmlAttributeAttribute]
                public string CLSIDOverride;

                [XmlAttributeAttribute]
                public string WinVer;

                [XmlElement("Input")]
                public List<EffectProperty> Inputs { get; set; }

                [XmlElement("Property")]
                public List<EffectProperty> Properties { get; set; }
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

                [XmlElement("Effect")]
                public List<Effect> Effects { get; set; }
            }

            public class Primitive
            {
                [XmlAttributeAttribute]
                public string Name;

                [XmlAttributeAttribute]
                public string ProjectedNameOverride;
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

            public class RootNamespaceNode
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

                [XmlElement("Primitive")]
                public List<Overrides.XmlBindings.Primitive> Primitives { get; set; }

                [XmlElement("FilenameBase")]
                public FilenameBaseNode FilenameBase;

                [XmlElement("RootNamespace")]
                public RootNamespaceNode RootNamespace;
            }
        }
    }
}
