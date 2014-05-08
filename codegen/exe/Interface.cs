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
    namespace XmlBindings
    {
        public class Interface
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
        }
    }

    class Interface : QualifiableType
    {
        public Interface(Namespace parentNamespace, XmlBindings.Interface xmlData, Overrides.XmlBindings.Interface overrides, Dictionary<string, QualifiableType> typeDictionary)
        {
            Debug.Assert(xmlData.Name.StartsWith("I"));
            string unprefixed = xmlData.Name.Substring(1);
            m_stylizedName = Formatter.Prefix + unprefixed;

            m_innerName = "I" + parentNamespace.ApiName + unprefixed;

            if(overrides != null && overrides.IsProjectedAsAbstract)
            {
                m_stylizedName = "I" + Formatter.Prefix + unprefixed;
            }

            typeDictionary[parentNamespace.RawName + "::" + xmlData.Name] = this;

        }

        public override string ProjectedName
        {
            get { return m_stylizedName; }
        }

        public override string NativeName
        {
            get { return m_innerName; }
        }

        public override string ProjectedNameIncludingIndirection
        {
            get{ return ProjectedName + "*"; }
        }
        
        public override string AccessorSuffix 
        {
            get { return ".Get()"; } 
        }
        public override string RuntimeClassMemberTypeName 
        {
            get { return "ComPtr<" + ProjectedName + ">"; } 
        }

        string m_stylizedName;
        string m_innerName;
    }

}