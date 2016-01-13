// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using System.Xml.Serialization;

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
            foreach (XmlBindings.Primitive p in xmlData.Primitives)
            {
                Overrides.XmlBindings.Primitive overridePrimitive = null;
                if (overrides != null) overridePrimitive = overrides.Primitives.Find(x => x.Name == p.Name);

                m_primitiveList.Add(new Primitive(p, overridePrimitive, typeDictionary));
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

                m_namespaceList.Add(new Namespace(n, overrideNamespace, overrides.RootNamespace.Value, typeDictionary, outputDataTypes));
            }
        }

        List<Namespace> m_namespaceList;
        List<Primitive> m_primitiveList;
        List<Struct> m_structList;
    }
}
