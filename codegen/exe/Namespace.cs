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

    public class Namespace
    {
        // XML-bound properties. Used for deserialization.
        [XmlElement("Enum")]
        public List<Enum> Enums { get; set; }

        [XmlElement("Struct")]
        public List<Struct> Structs { get; set; }

        [XmlElement("Interface")]
        public List<Interface> Interfaces { get; set; }

        [XmlElement("Typedef")]
        public List<Typedef> Typedefs { get; set; }

        [XmlAttributeAttribute]
        public string Name;
                
        public NamespaceOverrides Overrides;

        // The functions and member variables below are used for post-deseralization processing.
        public string ApiName
        {
            get 
            { 
                if (Overrides != null && Overrides.Name != null)
                {
                    return Overrides.Name;
                }
                else
                {
                    return Name;
                }
            }
        }

        public void Commit(Dictionary<string, QualifiableType> types)
        {
            string qualifier = Name + "::";
            
            if (Enums != null)
            {
                foreach (Enum e in Enums)
                {
                    e.Commit(this);

                    string qualifiedName = qualifier + e.Name;
                    types[qualifiedName] = e;
                }
            }

            if (Structs != null)
            {
                foreach (Struct s in Structs)
                {
                    s.Commit(this);

                    string qualifiedName = qualifier + s.Name;
                    types[qualifiedName] = s;
                }
            }

            if (Interfaces != null)
            {
                foreach (Interface i in Interfaces)
                {
                    i.Commit(this, qualifier, types);

                    string qualifiedName = qualifier + i.Name;
                    types[qualifiedName] = i;            
                }
            }
        }

        public void Resolve(Dictionary<string, QualifiableType> types)
        {
            if (Enums != null)
            {
                foreach (Enum e in Enums)
                {
                    e.Resolve();
                }
            }

            if (Structs != null)
            {
                foreach (Struct s in Structs)
                {
                    s.Resolve(types);
                }
            }

            if (Interfaces != null)
            {
                foreach (Interface i in Interfaces)
                {
                    i.Resolve(types);
                }
            }
        }

    }

}