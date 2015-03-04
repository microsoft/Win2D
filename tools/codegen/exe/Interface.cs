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
using System.Diagnostics;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CodeGen
{
    namespace XmlBindings
    {
        public class Parameter
        {
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Type;

            [XmlAttributeAttribute]
            public string Form;

            [XmlAttributeAttribute]
            public bool IsArray;

            [XmlAttributeAttribute]
            public bool IsRetval;
        }

        public class Method
        {
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Return;

            [XmlAttributeAttribute]
            public bool IsConst;

            [XmlAttributeAttribute]
            public string OverloadId;

            [XmlElement("Parameter")]
            public List<Parameter> Parameters { get; set; }
        }

        public class Interface
        {
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string Extends;

            [XmlElement("Method")]
            public List<Method> Methods { get; set; }
        }
    }

    public class Interface : QualifiableType
    {
        public Interface(Namespace parentNamespace, XmlBindings.Interface xmlData, Overrides.XmlBindings.Interface overrides, Dictionary<string, QualifiableType> typeDictionary)
        {
            Debug.Assert(xmlData.Name.StartsWith("I"));
            string unprefixed = xmlData.Name.Substring(1);
            m_stylizedName = Formatter.Prefix + unprefixed;

            m_innerName = "I" + parentNamespace.ApiName + unprefixed;

            m_nativeNameOfInheritanceParent = xmlData.Extends;

            if(overrides != null && overrides.IsProjectedAsAbstract)
            {
                m_stylizedName = "I" + Formatter.Prefix + unprefixed;
            }

            m_methods = new List<Method>();
            foreach(XmlBindings.Method xmlMethod in xmlData.Methods)
            {
                Method m = new Method(xmlMethod);
                m_methods.Add(m);
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

        public string NativeNameOfIntheritanceParent
        {
            get { return m_nativeNameOfInheritanceParent;  }
        }

        public class Method
        {

            public Method(XmlBindings.Method xmlData)
            {
                m_name = xmlData.Name;
                m_nativeReturnType = xmlData.Return;

                m_parameters = new List<Parameter>();
                foreach(XmlBindings.Parameter parameterXml in xmlData.Parameters)
                {
                    Parameter p = new Parameter(parameterXml);
                    m_parameters.Add(p);
                }

                m_isOverload = xmlData.OverloadId != null;

                m_isConst = xmlData.IsConst;
            }

            public string Name { get { return m_name; } }

            public string NativeReturnType { get { return m_nativeReturnType; } }

            public bool IsConst { get { return m_isConst; } }

            public bool IsOverload { get { return m_isOverload; } }

            public bool ReturnsHResult()
            {
                return m_nativeReturnType == null;
            }

            public class Parameter
            {
                public Parameter(XmlBindings.Parameter xmlData)
                {
                    m_name = xmlData.Name;

                    m_nativeTypeName = xmlData.Type;

                    m_form = xmlData.Form;

                    m_isArray = xmlData.IsArray;
                }

                public string Name { get { return m_name; } }

                public string NativeTypeName { get { return m_nativeTypeName; } }

                public string Form { get { return m_form; } }

                public bool IsArray { get { return m_isArray; } }

                string m_name;
                string m_nativeTypeName;
                string m_form;
                bool m_isArray;
            }
            public List<Parameter> Parameters { get { return m_parameters; } }

            string m_name;
            string m_nativeReturnType;
            bool m_isConst;
            bool m_isOverload;
            List<Parameter> m_parameters;
        }

        public List<Method> Methods { get { return m_methods; } }

        string m_stylizedName;
        string m_innerName;
        string m_nativeNameOfInheritanceParent;
        List<Method> m_methods;
    }

}
