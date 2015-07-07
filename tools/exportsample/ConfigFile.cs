// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Xml.Linq;

namespace exportsample
{
    public class Configuration
    {
        public CommandLineOptions Options;
        public Dictionary<string, string> Properties = new Dictionary<string, string>();
        public List<SampleDirectory> Samples = new List<SampleDirectory>();
        public List<InlineImport> InlineImports = new List<InlineImport>();
        public Dictionary<string, string> DuplicateDirectories = new Dictionary<string, string>();
        public List<string> FileReferenceProperties = new List<string>();
        public List<string> Win2DProjects = new List<string>();
        public List<string> FilesToCopy = new List<string>();

        public Configuration(CommandLineOptions options)
        {
            this.Options = options;

            var doc = XDocument.Load(options.Config);

            foreach (var element in doc.Element("ExportSamples").Elements())
            {
                switch (element.Name.ToString())
                {
                    case "Property":
                        Properties.Add(
                            element.Attribute("Name").Value,
                            Path.GetFullPath(Path.Combine(options.Root, element.Attribute("Value").Value)));
                        break;

                    case "Sample":
                        Samples.Add(new SampleDirectory(element, options.Root, options.Dest));
                        break;

                    case "InlineImport":
                        InlineImports.Add(new InlineImport(element));
                        break;

                    case "DuplicateIntoSample":
                        DuplicateDirectories.Add(
                            Path.GetFullPath(Path.Combine(options.Root, element.Attribute("Source").Value)),
                            element.Attribute("Destination").Value);
                        break;

                    case "MSBuildPropertyIsFileReference":
                        FileReferenceProperties.Add(element.Attribute("Name").Value);
                        break;

                    case "Win2DProject":
                        Win2DProjects.Add(element.Attribute("Name").Value);
                        break;

                    case "File":
                        FilesToCopy.Add(element.Attribute("Source").Value);
                        break;
                }
            }
        }

        public InlineImport FindInlineImportFor(string importPath)
        {
            foreach (var inlineImport in InlineImports)
            {
                if (importPath.EndsWith(inlineImport.Name))
                    return inlineImport;
            }

            return null;
        }

        public string GetDestination(string source)
        {
            string destination;
            if (TryGetDestination(source, out destination))
            {
                return destination;
            }

            throw new Exception(string.Format("Could not find destination for '{0}'", source));
        }

        public bool ShouldExport(string file)
        {
            string dummyDestination;
            return TryGetDestination(file, out dummyDestination);
        }

        public bool TryGetDestination(string source, out string destination)
        {
            foreach (var directory in Samples)
            {
                if (source.StartsWith(directory.Source))
                {
                    destination = source.Replace(directory.Source, directory.Destination);
                    return true;
                }
            }

            destination = null;
            return false;
        }

        public bool IsWin2DProject(string project)
        {
            foreach (var win2dProject in Win2DProjects)
            {
                if (project.EndsWith(win2dProject))
                    return true;
            }
            return false;
        }
    }


    public class SampleDirectory
    {
        public string Destination;
        public string Source;

        public SampleDirectory(XElement element, string enlistmentRoot, string destinationRoot)
        {
            Destination = element.Attribute("Destination").Value;
            Source = element.Attribute("Source").Value;

            // Preprocess the directories to have absolute paths.  The dirs in the config file are relative to the config file.
            Destination = Path.GetFullPath(Path.Combine(destinationRoot, Destination));
            Source = Path.GetFullPath(Path.Combine(enlistmentRoot, Source));
        }
    }

    public class InlineImport
    {
        public string Name;
        public List<XElement> Elements;
        public InlineImport(XElement element)
        {
            Name = element.Attribute("Name").Value;
            Elements = new List<XElement>(element.Elements());

            // Rename all the elements so that they're in msbuild's namespace
            foreach (var e in Elements)
            {
                foreach (var d in e.DescendantsAndSelf())
                {
                    d.Name = ProjectProcessor.NS + d.Name.LocalName;
                }
            }
        }
    }

}
