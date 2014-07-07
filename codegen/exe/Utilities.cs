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
using System.IO;

namespace CodeGen
{
    namespace XmlBindings
    {
        class Utilities
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
