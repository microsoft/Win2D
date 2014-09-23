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

                [XmlElement("Primitive")]
                public List<Overrides.XmlBindings.Primitive> Primitives { get; set; }

                [XmlElement("FilenameBase")]
                public FilenameBaseNode FilenameBase;

                [XmlElement("Subnamespace")]
                public SubnamespaceNode Subnamespace;
            }
        }
    }
}
