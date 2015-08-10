// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System;
using System.Linq;
using System.Xml.Linq;
using System.IO;
using System.Xml;
using System.Collections.Generic;

namespace DocPreprocess
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
            Console.WriteLine("Preprocessing documentation sources");

            Directory.CreateDirectory(options.OutputPath);

            foreach (var filename in options.DocSrc)
            {
                var doc = XDocument.Load(filename);

                Preprocess(doc, filename);

                string output = Path.Combine(options.OutputPath, Path.GetFileName(filename));

                doc.Save(output);
            }
        }


        static void Preprocess(XDocument doc, string filename)
        {
            var members = doc.Element("doc").Element("members").Elements();

            foreach (var tag in Tag.Tags.Reverse())
            {
                ValidateNoExplicitTags(doc, filename, tag);

                if (tag.PropagateTypeTagsToMembers)
                {
                    PropagateTypeTagsToMembers(members, tag);
                }

                InsertTagText(members, tag);
            }
        }


        static void ValidateNoExplicitTags(XDocument doc, string filename, Tag tag)
        {
            if (doc.Descendants().Any(e => e.Value.Contains(tag.Summary)))
            {
                Console.WriteLine("{0}: warning: docsrc contains explicit {1} tag.", filename, tag.Summary);
            }
        }


        static void PropagateTypeTagsToMembers(IEnumerable<XElement> members, Tag tag)
        {
            // Find all the types (classes, structs) that have this tag.
            var taggedTypes = (from member in members
                               where IsMemberType(member, 'T') && HasTag(member, tag)
                               select MemberName(member) + '.').ToList();

            // Find members (methods, fields) of these types that should also be tagged.
            var membersToTag = from member in members
                               where taggedTypes.Any(type => MemberName(member).StartsWith(type))
                               select member;

            foreach (var member in membersToTag)
            {
                member.SetAttributeValue(tag.Attribute, true);
            }
        }


        static void InsertTagText(IEnumerable<XElement> members, Tag tag)
        {
            foreach (var member in members.Where(e => HasTag(e, tag)))
            {
                // Clear the no-longer-needed attribute.
                member.SetAttributeValue(tag.Attribute, null);

                var summary = GetOrCreateElement(member, "summary");
                var remarks = GetOrCreateElement(member, "remarks");

                // If this doc entry has a plain text summary, we insert before that, but if
                // it already contains <p> formatting we want to go inside the paragraph block.
                var firstSummaryElement = summary.Elements().FirstOrDefault();

                if (firstSummaryElement != null && firstSummaryElement.Name == "p")
                {
                    summary = firstSummaryElement;
                }

                summary.AddFirst(new XElement("b", tag.Summary + " "));
                remarks.AddFirst(new XElement("p", new XElement("i", tag.Description)));
            }
        }


        static bool IsMemberType(XElement member, char memberType)
        {
            // Checks eg. the 'T' part of "T:Microsoft.Graphics.Canvas.Foo"
            return member.Attribute("name").Value.StartsWith(memberType + ":");
        }


        static string MemberName(XElement member)
        {
            var name = member.Attribute("name").Value;

            // Skip eg. the "T:" part of "T:Microsoft.Graphics.Canvas.Foo"
            if (name.Length > 2 && name[1] == ':')
            {
                name = name.Substring(2);
            }

            return name;
        }


        static bool HasTag(XElement member, Tag tag)
        {
            var attribute = member.Attribute(tag.Attribute);

            return attribute != null && XmlConvert.ToBoolean(attribute.Value);
        }


        static XElement GetOrCreateElement(XElement parent, string elementName)
        {
            var element = parent.Element(elementName);

            if (element == null)
            {
                element = new XElement(elementName);
                parent.Add(element);
            }

            return element;
        }
    }
}
