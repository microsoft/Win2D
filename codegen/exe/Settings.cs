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

    public class StructOverrides
    {
        public StructOverrides()
        {
            ShouldProject = false;
        }

        // XML bindings
        [XmlAttributeAttribute]
        public string Name;

        // XML bindings
        [XmlAttributeAttribute]
        public bool ShouldProject;

        // XML bindings
        [XmlAttributeAttribute]
        public string Guid;

        // XML bindings
        [XmlAttributeAttribute]
        public string ProjectedNameOverride;
    }

    public class EnumOverrides
    {
        public EnumOverrides()
        {
            ShouldProject = true;
        }

        // XML bindings
        [XmlAttributeAttribute]
        public string Name;

        // XML bindings
        [XmlAttributeAttribute]
        public bool ShouldProject;

        // XML bindings
        [XmlAttributeAttribute]
        public string ProjectedNameOverride;

        [XmlElement("Field")]
        public List<EnumFieldOverrides> Fields { get; set; }
    }


    public class NamespaceOverrides
    {
        // XML bindings
        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string NameOverride;

        [XmlElement("Struct")]
        public List<StructOverrides> Structs { get; set; }

        [XmlElement("Enum")]
        public List<EnumOverrides> Enums { get; set; }
    }

    public class EnumFieldOverrides
    {
        // XML bindings
        [XmlAttributeAttribute]
        public string Name;

        // XML bindings
        [XmlAttributeAttribute]
        public string ProjectedNameOverride;
    }

    [XmlRootAttribute]
    public class Settings
    {
        [XmlElement("Namespace")]
        public List<NamespaceOverrides> Namespaces { get; set; }

        [XmlElement("Prefix")]
        public PrefixNode Prefix;

        public void LoadAndApply(string inputDir, D2DTypes masterDocument, Dictionary<string, QualifiableType> qualifiableTypes)
        {
            XmlSerializer xmlSerializer = new XmlSerializer(typeof(Settings));
            var inputPath = Path.Combine(inputDir, "Settings.xml");
            FileStream fileStream = new FileStream(inputPath, FileMode.Open, FileAccess.Read);
            object rootDocumentObject = xmlSerializer.Deserialize(fileStream);
            Settings document = rootDocumentObject as Settings;

            Formatter.Prefix = document.Prefix.Value;

            foreach (NamespaceOverrides settingsNamespace in document.Namespaces)
            {
                Namespace n = masterDocument.GetNamespace(settingsNamespace.Name);
                n.Overrides = settingsNamespace;

                foreach (StructOverrides settingsStruct in settingsNamespace.Structs)
                {
                    Struct s = (qualifiableTypes[settingsStruct.Name]) as Struct;
                    s.Overrides = settingsStruct;
                }

                foreach (EnumOverrides settingsEnum in settingsNamespace.Enums)
                {
                    Enum e = (qualifiableTypes[settingsEnum.Name]) as Enum;
                    e.Overrides = settingsEnum;

                    foreach (EnumFieldOverrides settingsField in settingsEnum.Fields)
                    {
                        e.GetEnumValueByName(settingsField.Name).ProjectedNameOverride = settingsField.ProjectedNameOverride;
                    }
                }

            }
        }
    }

    public class PrefixNode
    {
        [XmlAttributeAttribute]
        public string Value;
    }
}