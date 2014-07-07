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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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

    class Primitive : QualifiableType
    {
        public Primitive(XmlBindings.Primitive xmlData, Dictionary<string, QualifiableType> typeDictionary)
        {
            m_name = xmlData.Name;
            typeDictionary[m_name] = this;
        }

        public override string ProjectedName
        {
            get { return m_name; } 
        }
        public override string NativeName
        {
            get { return m_name; } 
        }

        public override string ProjectedNameIncludingIndirection
        {
            get { return m_name; }
        }

        string m_name;
    }
}
