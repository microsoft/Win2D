//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Diagnostics;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CodeGen
{
    public class Interface : QualifiableType
    {
        //
        // While the codegen does not output code for interfaces, it needs to know about them
        // in order to resolve names; notice how there is a Resolve method which forms the
        // correct name for the types dictionary, but no Output code method.
        //
        // In particular, this is necessary to resolve struct fields, some of which are interfaces.
        // For example, a field of D2D1_LAYER_PARAMETERS is an ID2D1Geometry geometry mask type.
        //

        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string Extends;

        public bool IsProjectedAsAbstract;

        public override string ProjectedName
        {
            get { return m_stylizedName; }
        }

        public override string NativeName
        {
            get { return m_innerName; }
        }
        
        public void Commit(Namespace parentNamespace, string qualifier, Dictionary<string, QualifiableType> types)
        {
            m_parentNamespace = parentNamespace;
        }

        public void Resolve(Dictionary<string, QualifiableType> types)
        {
            Debug.Assert(Name.StartsWith("I"));
            string unprefixed = Name.Substring(1);
            m_stylizedName = Formatter.Prefix + unprefixed;

            m_innerName = "I" + m_parentNamespace.ApiName + unprefixed; // This assumption relies on the assert above.

            if(IsProjectedAsAbstract)
            {
                m_stylizedName = "I" + Formatter.Prefix + unprefixed;
            }

            if (types.ContainsKey(Extends))
            {
                // Note: the Extends field is already qualified. See D2DTypes.xml. Example: Extends="D2D1::IImage"
                m_extendsType = types[Extends];
            }
        }

        string m_stylizedName;
        string m_innerName;
        Namespace m_parentNamespace;
        QualifiableType m_extendsType;
    }

}