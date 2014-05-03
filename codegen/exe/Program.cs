//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml.Serialization;
using System.Globalization;
using System.Diagnostics;

namespace CodeGen
{
    public struct OutputFiles
    {
        public Formatter CppFile;
        public Formatter IdlFile;
    }

    public class Program
    {
        static void Main(string[] args)
        {

            var inputDir = FindInputDirectory();

            var filenameBase = "DirectX";
            var outputDir = Path.GetFullPath(Path.Combine(inputDir, "..", "..", "winrt", "lib"));
            
            List<string> files = new List<string>();
            files.Add("D2DTypes.xml");
            files.Add("D2DTypes2.xml");
            files.Add("D2DEffectAuthor.xml");
            files.Add("D2DTypes3.xml");

            D2DTypes combinedDocument = new D2DTypes();
            foreach(string fileName in files)
            {
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(D2DTypes));
                var inputPath = Path.Combine(inputDir, fileName);
                FileStream fileStream = new FileStream(inputPath, FileMode.Open, FileAccess.Read);
                object rootDocumentObject = xmlSerializer.Deserialize(fileStream);
                D2DTypes document = rootDocumentObject as D2DTypes;
                combinedDocument.AddTypes(document);
            }
            
            Dictionary<string, QualifiableType> qualifiableTypes = new Dictionary<string, QualifiableType>();
            combinedDocument.CommitAllTypesToDictionary(qualifiableTypes);

            Settings settings = new Settings();
            settings.LoadAndApply(inputDir, combinedDocument, qualifiableTypes);
            
            combinedDocument.ResolveTypesAgainstDictionary(qualifiableTypes);

            Directory.CreateDirectory(outputDir);

            OutputFiles outputFiles = new OutputFiles();

            using (Formatter cppStreamWriter = new Formatter(Path.Combine(outputDir, filenameBase + ".codegen.cpp")),
                             idlStreamWriter = new Formatter(Path.Combine(outputDir, filenameBase + ".codegen.idl")))
            {
                outputFiles.CppFile = cppStreamWriter;
                outputFiles.IdlFile = idlStreamWriter;

                combinedDocument.OutputCode(outputFiles);
            }

            // Below is a verification step.
            // TODO: Consider moving this out of the main codegen process.
            Formatter.FormattingTest();
        }

        //
        // The input directory, which contains the various XML spec files, can be found by checking some known locations.
        //
        static string FindInputDirectory()
        {
            string[] candidates = new string[] {
                ".",
                "codegen/exe",
                "../../../../codegen/exe"
            };

            foreach (var candidate in candidates)
            {
                var normalizedCandidate = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, candidate));
                if (IsInputDirectory(normalizedCandidate))
                    return normalizedCandidate;
            }

            throw new FileNotFoundException("Couldn't find the codegen source directory");
        }

        //
        // The input directory is that one that contains Settings.xml.
        //
        static bool IsInputDirectory(string directory)
        {
            return File.Exists(Path.Combine(directory, "Settings.xml"));
        }

    }
}
