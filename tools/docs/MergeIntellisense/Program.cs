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
using System.Linq;
using System.Collections.Generic;
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
                                select member;

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


        static string GetAssemblyName(XDocument xml)
        {
            return xml.Element("doc").Element("assembly").Element("name").Value;
        }
    }
}
