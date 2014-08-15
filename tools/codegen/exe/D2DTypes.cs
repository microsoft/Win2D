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
    namespace XmlBindings
    {
        public class D2DTypes
        {
            // XML-bound property. Used for deserialization.
            [XmlElement("Namespace")]
            public List<XmlBindings.Namespace> Namespaces { get; set; }

            // The codegen does not really need to understand what primitives are,
            // but it needs the capacity to override them and arbitrarily replace them
            // with WinRT types.
            [XmlElement("Primitive")]
            public List<XmlBindings.Primitive> Primitives { get; set; }

            // Structs, enums, and interfaces stored directly to D2DTypes (the root node) are in the global namespace.
            [XmlElement("Enum")]
            public List<XmlBindings.Enum> Enums { get; set; }

            [XmlElement("Struct")]
            public List<XmlBindings.Struct> Structs { get; set; }

            [XmlElement("Interface")]
            public List<XmlBindings.Interface> Interfaces { get; set; }
        }
    }

    public class D2DTypes 
    {
        public D2DTypes(XmlBindings.D2DTypes xmlData, Overrides.XmlBindings.Settings overrides, Dictionary<string, QualifiableType> typeDictionary, OutputDataTypes outputDataTypes)
        {
            const Namespace globalNamespace = null; // Using null as the namespace parameter indicates the global namespace.

            m_primitiveList = new List<Primitive>();
            foreach(XmlBindings.Primitive p in xmlData.Primitives)
            {
                m_primitiveList.Add(new Primitive(p, typeDictionary));
            }

            m_structList = new List<Struct>();
            foreach (XmlBindings.Struct s in xmlData.Structs)
            {
                Overrides.XmlBindings.Struct overrideStruct = null;
                if (overrides != null) overrideStruct = overrides.Structs.Find(x => x.Name == s.Name);

                m_structList.Add(new Struct(globalNamespace, s, overrideStruct, typeDictionary, outputDataTypes));
            }

            m_namespaceList = new List<Namespace>();
            foreach (XmlBindings.Namespace n in xmlData.Namespaces)
            {
                Overrides.XmlBindings.Namespace overrideNamespace = null;
                if (overrides != null) overrideNamespace = overrides.Namespaces.Find(x => x.Name == n.Name);

                m_namespaceList.Add(new Namespace(n, overrideNamespace, typeDictionary, outputDataTypes));
            }
        }

        List<Namespace> m_namespaceList;
        List<Primitive> m_primitiveList;
        List<Struct> m_structList;
    }
}
