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
