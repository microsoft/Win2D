// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Linq;
using System.Collections.Generic;
using System.Xml.Linq;
using System.IO;
using Shared;

namespace DocDiff
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
            Console.WriteLine("Diffing documentation sources to make sure they match the latest API");

            // Load the XML doc files.
            var docSrc = LoadApiMembers(options.DocSrc, options.DocSrcPath);
            var apiRef = LoadApiMembers(options.ApiRef, string.Empty);

            // Diff them.
            var docsWithSummaries = from doc in docSrc
                                    where !string.IsNullOrEmpty((string)doc.MemberElement.Element("summary"))
                                    select doc;

            var docsThatArentNamespaces = from doc in docSrc
                                          where !doc.MemberElement.Attribute("name").Value.StartsWith("N:")
                                          select doc;

            var missingDocs = apiRef.Except(docsWithSummaries);
            var orphanedDocs = docsThatArentNamespaces.Except(apiRef);

            // Report missing docs for things that were found in the API but are not documented.
            int missingCount = missingDocs.Count();

            if (missingCount > 0)
            {
                Console.WriteLine("warning: missing {0} doc {1} (see {2} for details)",
                                  missingCount,
                                  (missingCount == 1) ? "entry" : "entries",
                                  Path.GetFileName(options.LogMissingDocs));
                
                Console.WriteLine("Logging missing doc details to {0}",
                                  Path.GetFullPath(options.LogMissingDocs));

                var xml = new XDocument(
                    new XElement("doc",
                        new XElement("members",
                            from member in missingDocs
                            select member.MemberElement
                        )
                    )
                );

                xml.Save(options.LogMissingDocs);
            }

            // Report any unwanted docs for APIs that do not exist.
            foreach (var orphaned in orphanedDocs)
            {
                Console.WriteLine("{0}: warning: orphaned docs: {1}", Path.GetFullPath(orphaned.FileName), orphaned.ApiName);
            }

            // Report any <see cref=""/> references to things that don't exist.
            ValidateReferences(docSrc, options.AmlSrc);
        }


        static void ValidateReferences(IEnumerable<ApiMember> docs, IEnumerable<string> amlSrc)
        {
            // Find all <see cref=""/> elements in the documentation.
            var references = from doc in docs
                             from element in doc.MemberElement.Descendants()
                             where element.Name == "see" || element.Name == "seealso"
                             select new
                             {
                                 Target = element.Attribute("cref").Value,
                                 FileName = doc.FileName,
                             };

            // Also look for <codeEntityReference> elements in the AML conceptual content.
            var codeEntityReference = XName.Get("codeEntityReference", "http://ddue.schemas.microsoft.com/authoring/2003/5");

            var amlReferences = from filename in amlSrc
                                from reference in LoadXDocument(filename).Descendants(codeEntityReference)
                                select new
                                {
                                    Target = reference.Value.Trim(),
                                    FileName = filename,
                                };

            // Report any references with invalid targets.
            var badReferences = from reference in references.Concat(amlReferences)
                                where !IsReferenceValid(reference.Target, docs)
                                select reference;

            foreach (var reference in badReferences)
            {
                Console.WriteLine("{0}: warning: bad cref target: {1}", Path.GetFullPath(reference.FileName), reference.Target);
            }
        }


        static bool IsReferenceValid(string referenceTarget, IEnumerable<ApiMember> docs)
        {
            if (referenceTarget.StartsWith("O:"))
            {
                // If the reference target is an overload group, there should be more than one method of that name.
                string methodName = 'M' + referenceTarget.Substring(1);
                int methodCount = docs.Count(doc => doc.ApiName == methodName || doc.ApiName.StartsWith(methodName + '('));
                return methodCount > 1;
            }
            else
            {
                // Otherwise look for an exact match.
                return docs.Any(doc => doc.ApiName == referenceTarget);
            }
        }


        // Loads API information from XML documentation files.
        static IEnumerable<ApiMember> LoadApiMembers(IEnumerable<string> filenames, string path)
        {
            var members = from filename in filenames
                          from member in LoadXDocument(Path.Combine(path, filename)).Element("doc").Element("members").Elements()
                          select new ApiMember(member, filename);

            return members.ToList();
        }


        static XDocument LoadXDocument(string filename)
        {
            try
            {
                return XDocument.Load(filename);
            }
            catch (Exception e)
            {
                throw new Exception(filename + ": " + e);
            }
        }
    }
}
