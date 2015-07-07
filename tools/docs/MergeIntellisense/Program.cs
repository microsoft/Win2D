// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Linq;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Linq;
using System.IO;
using Shared;

namespace MergeIntellisense
{
    class Program
    {
        [STAThread]
        static int Main(string[] args)
        {
            // Parse commandline options.
            var options = new CommandLineOptions();
            var parser = new CommandLineParser(options);

            if (!parser.ParseCommandLine(args))
            {
                return 1;
            }

            // Run the program logic.
            try
            {
                Run(options);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Error: {0}\n\n{1}:\n{2}", e.Message, e.GetType(), e.StackTrace);
                return 1;
            }

            return 0;
        }


        static void Run(CommandLineOptions options)
        {
            Console.WriteLine("Merging intellisense for {0}", Path.GetFileNameWithoutExtension(options.Output));

            // Load 'em.
            var inputs = options.Inputs.Select(XDocument.Load).ToList();

            // Merge 'em.
            var merged = MergeXml(inputs);

            // Save 'em.
            Directory.CreateDirectory(Path.GetDirectoryName(options.Output));

            merged.Save(options.Output);
        }


        static XDocument MergeXml(IEnumerable<XDocument> inputs)
        {
            // What assembly is this?
            var assemblyName = GetAssemblyName(inputs.First());

            if (!inputs.All(input => GetAssemblyName(input) == assemblyName))
            {
                throw new Exception("All input files must be for the same assembly.");
            }

            // Merge all the member documentation into a single list.
            var mergedMembers = from input in inputs
                                from member in input.Element("doc").Element("members").Elements()
                                where !IsNamespace(member)
                                select member;

            // Remove documentation tags that Intellisense does not use, to minimize the size of the shipping XML files.
            RemoveUnwantedElements(mergedMembers);

            // Generate new Intellisense XML.
            return new XDocument(
                new XElement("doc",
                    new XElement("assembly",
                        new XElement("name", assemblyName)
                    ),
                    new XElement("members",
                        mergedMembers
                    )
                )
            );
        }


        static void RemoveUnwantedElements(IEnumerable<XElement> members)
        {
            string[] tagsSupportedByIntellisense =
            {
                "summary",
                "param",
                "returns",
                "exception",
            };

            string[] tagsToStrip =
            {
                "a",
                "img",
            };

            RemoveElements(from member in members
                           from element in member.Elements()
                           where !tagsSupportedByIntellisense.Contains(element.Name.LocalName, StringComparer.OrdinalIgnoreCase)
                           select element);

            RemoveElements(from element in members.Descendants()
                           where IsMarkedToSkip(element)
                           select element);

            StripElements(from element in members.Descendants()
                          where tagsToStrip.Contains(element.Name.LocalName, StringComparer.OrdinalIgnoreCase)
                          select element);
        }


        static void RemoveElements(IEnumerable<XElement> elementsToRemove)
        {
            foreach (var element in elementsToRemove.ToList())
            {
                element.Remove();
            }
        }


        static void StripElements(IEnumerable<XElement> elementsToStrip)
        {
            foreach (var element in elementsToStrip.ToList())
            {
                element.ReplaceWith(element.Value);
            }
        }


        static string GetAssemblyName(XDocument xml)
        {
            return xml.Element("doc").Element("assembly").Element("name").Value;
        }
        
        
        static bool IsNamespace(XElement member)
        {
            return member.Attribute("name").Value.StartsWith("N:");
        }
        
        
        static bool IsMarkedToSkip(XElement element)
        {
            var attribute = element.Attribute("intellisense");
            
            if (attribute == null)
            {
                return false;
            }

            return !XmlConvert.ToBoolean(attribute.Value);
        }
    }
}
