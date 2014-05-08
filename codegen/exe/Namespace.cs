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
    // Note about namespaces:  the D2D namespace contains only some very basic structs and typedefs.
    // The D2D1 namespace contains all the rest of the types.
    // As far as types are concerned, D2D vs D2D1 namespace is not an important distinction. 
    // Because, both are qualified as D2D1:: in the rest of the XML files.

    // No types in the global namespace should ever be codegenned, because the XML files will only 
    // alias them. They don't specify them completely. So they should be produced manually.

    // There are a couple D2D types, then, that are just typedefed to types in the global namespace.
    // For these, we have manual wrappers for them.

    namespace XmlBindings
    {
        public class Namespace
        {
            [XmlElement("Enum")]
            public List<XmlBindings.Enum> Enums { get; set; }

            [XmlElement("Struct")]
            public List<XmlBindings.Struct> Structs { get; set; }

            [XmlElement("Interface")]
            public List<XmlBindings.Interface> Interfaces { get; set; }

            [XmlElement("Typedef")]
            public List<XmlBindings.Typedef> Typedefs { get; set; }

            [XmlAttributeAttribute]
            public string Name;
        }
    }

    class Namespace
    {
        public Namespace(XmlBindings.Namespace xmlData, Overrides.XmlBindings.Namespace overrides, Dictionary<string, QualifiableType> typeDictionary, OutputDataTypes outputDataTypes)
        {
            m_rawName = xmlData.Name;

            if (overrides != null && overrides.NameOverride != null)
            {
                m_apiName = overrides.NameOverride;
            }
            else
            {
                m_apiName = xmlData.Name;
            }

            m_enums = new List<Enum>();
            foreach(XmlBindings.Enum enumXml in xmlData.Enums)
            {
                Overrides.XmlBindings.Enum overridesEnum = null;
                if(overrides != null) overridesEnum = overrides.Enums.Find(x => x.Name == enumXml.Name);

                m_enums.Add(new Enum(this, enumXml, overridesEnum, typeDictionary, outputDataTypes));
            }

            m_structs = new List<Struct>();
            foreach (XmlBindings.Struct structXml in xmlData.Structs)
            {
                Overrides.XmlBindings.Struct overridesStruct = null;
                if(overrides != null) overridesStruct = overrides.Structs.Find(x => x.Name == structXml.Name);

                m_structs.Add(new Struct(this, structXml, overridesStruct, typeDictionary, outputDataTypes));
            }

            m_interfaces = new List<Interface>();
            foreach (XmlBindings.Interface interfaceXml in xmlData.Interfaces)
            {
                Overrides.XmlBindings.Interface overridesInterface = null;
                if(overrides != null) overridesInterface = overrides.Interfaces.Find(x => x.Name == interfaceXml.Name);

                m_interfaces.Add(new Interface(this, interfaceXml, overridesInterface, typeDictionary));
            }

            foreach (XmlBindings.Typedef t in xmlData.Typedefs)
            {
                // In the types XML, often times types are declared as one type,
                // then typedefs to something else, and referenced thereafter
                // as that second type. And so, typedefs must be handled here.
                //
                // In the XML, the 'Name' field in each typedef is unqualified,
                // but the 'From' field is qualified.
                // For example, <Typedef Name="COLOR_F" From="D2D::COLOR_F"/>
                //
                // So, the entries are added to the type dictionary here
                // under the qualified name.
                //
                string qualified = xmlData.Name + "::" + t.Name;
                typeDictionary[qualified] = typeDictionary[t.From];
            }
        }

        public string ApiName
        {
            get  { return m_apiName; }
        }

        public string RawName
        {
            get {  return m_rawName; }
        }

        string m_rawName;
        string m_apiName;
        List<Enum> m_enums;
        List<Struct> m_structs;
        List<Interface> m_interfaces;

    }

}