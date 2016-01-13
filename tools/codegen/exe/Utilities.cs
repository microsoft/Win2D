// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.IO;
using System.Xml.Serialization;

namespace CodeGen
{
    namespace XmlBindings
    {
        public class Utilities
        {
            public static DocumentType LoadXmlData<DocumentType>(string inputDir, string fileName) where DocumentType : class
            {
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(DocumentType));
                var inputPath = Path.Combine(inputDir, fileName);
                FileStream fileStream = new FileStream(inputPath, FileMode.Open, FileAccess.Read);
                object rootDocumentObject = xmlSerializer.Deserialize(fileStream);
                DocumentType document = rootDocumentObject as DocumentType;
                return document;
            }
        }
    }
}
