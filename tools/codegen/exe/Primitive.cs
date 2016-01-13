// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using System.Xml.Serialization;

namespace CodeGen
{
    //
    // Primitive types are the "non-D2D types". For example: UINT, WCHAR, HWND, DWRITE_GLYPH_RUN.
    // By convention, in the XML, they are defined under the root node, not in any namespace.
    // 
    // One key reason this codegen needs to know about primitive types is in order to reference
    // D2D's color type. In the XML, D3DCOLORVALUE is a Primitive, later typedefed to D2D::COLOR_F,
    // later typedefed to D2D1::COLOR_F. The name "D2D1::COLOR_F" is used throughout all the API
    // XML to refer to color.
    //
    namespace XmlBindings
    {
        public class Primitive
        {
            [XmlAttributeAttribute]
            public string Name;
        }

    }

    public class Primitive : QualifiableType
    {
        public Primitive(XmlBindings.Primitive xmlData, Overrides.XmlBindings.Primitive overrides, Dictionary<string, QualifiableType> typeDictionary)
        {
            m_name = xmlData.Name;
            typeDictionary[m_name] = this;

            m_projectedName = m_name;
            if (overrides != null && overrides.ProjectedNameOverride != null)
            {
                m_projectedName = overrides.ProjectedNameOverride;
            }
        }

        public override string ProjectedName
        {
            get { return m_projectedName; } 
        }
        public override string NativeName
        {
            get { return m_name; } 
        }

        public override string ProjectedNameIncludingIndirection
        {
            get { return m_projectedName; }
        }

        string m_name;
        string m_projectedName;
    }
}
