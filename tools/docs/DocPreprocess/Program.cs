// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

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
                var docsrc = PreprocessDocuments(options);

                GenerateIndexTopics(options, docsrc);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Error: {0}\n\n{1}:\n{2}", e.Message, e.GetType(), e.StackTrace);
                return 1;
            }

            return 0;
        }


        static XDocument[] PreprocessDocuments(CommandLineOptions options)
        {
            Console.WriteLine("Preprocessing documentation sources");

            Directory.CreateDirectory(options.DocSrcOutput);

            var allDocuments = (from filename in options.DocSrc
                                select new
                                {
                                    Filename = filename,
                                    Document = XDocument.Load(filename)
                                }).ToList();

            var templates = allDocuments.SelectMany(doc => doc.Document.Descendants("template")).ToList();

            templates.ForEach(template => template.Remove());

            foreach (var doc in allDocuments)
            {
                ExpandTemplates(doc.Document, templates);
                PreprocessDocument(doc.Document, doc.Filename);

                string output = Path.Combine(options.DocSrcOutput, Path.GetFileName(doc.Filename));

                doc.Document.Save(output);
            }

            return allDocuments.Select(doc => doc.Document).ToArray();
        }


        static void PreprocessDocument(XDocument doc, string filename)
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
                               where IsMemberType(member, 'T') && HasAttribute(member, tag.Attribute)
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
            foreach (var member in members.Where(e => HasAttribute(e, tag.Attribute)))
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


        static void ExpandTemplates(XDocument doc, List<XElement> templates)
        {
            foreach (var inheritMarker in doc.Descendants("inherittemplate").ToList())
            {
                var name = inheritMarker.Attribute("name").Value;
                var replacement = inheritMarker.Attribute("replacement")?.Value;

                var template = templates.Single(t => t.Attribute("name").Value == name);

                var clonedTemplate = new XElement(template);

                foreach (var attribute in clonedTemplate.Descendants().Attributes())
                {
                    if (replacement != null)
                        attribute.Value = attribute.Value.Replace(name, replacement);
                }

                inheritMarker.ReplaceWith(clonedTemplate.Elements());
            }
        }


        static void GenerateIndexTopics(CommandLineOptions options, XDocument[] docsrc)
        {
            Console.WriteLine("Processing layout content");

            Directory.CreateDirectory(options.TopicsOutput);

            // Find index topics in the Sandcastle Layout.content file that don't already have handwritten .aml content.
            var layoutContent = XDocument.Load(options.LayoutContent);

            var topics = from element in layoutContent.Descendants("Topic")
                         where HasAttribute(element, "generateTopic")
                         select element;

            // Autogenerate .aml content for these topics.
            foreach (var topic in topics)
            {
                GenerateTopicPage(options, docsrc, topic);
            }
        }


        static readonly XNamespace ns = "http://ddue.schemas.microsoft.com/authoring/2003/5";
        static readonly XNamespace xlink = "http://www.w3.org/1999/xlink";


        static void GenerateTopicPage(CommandLineOptions options, XDocument[] docsrc, XElement parentTopic)
        {
            string topicId = parentTopic.Attribute("id").Value;

            var summaryAttribute = parentTopic.Attribute("summary");
            var summaryElement = summaryAttribute != null ? new XElement(ns + "para", summaryAttribute.Value) : null;

            var topicPage = new XDocument(
                                new XElement("topic", 
                                    new XAttribute("id", topicId),
                                    new XAttribute("revisionNumber", 1),
                                    new XElement(ns + "developerConceptualDocument",
                                        new XAttribute(XNamespace.Xmlns + "xlink", xlink),
                                        new XElement(ns + "section",
                                            new XElement(ns + "content",
                                                summaryElement,
                                                new XElement(ns + "table",

                                                    // Each child of this topic forms one row of the table.
                                                    from childTopic in parentTopic.Elements("Topic")
                                                    select FormatChildTopic(options, docsrc, childTopic)
                                                )
                                            )
                                        )
                                    )
                                )
                            );

            string output = Path.Combine(options.TopicsOutput, topicId + ".aml");

            topicPage.Save(output);
        }


        static object FormatChildTopic(CommandLineOptions options, XDocument[] docsrc, XElement childTopic)
        {
            // Row of the table consists of a link to the child topic, plus topic description text.
            var childTopicRow = FormatTableRow(
                                    new XElement(ns + "link",
                                        new XAttribute(xlink + "href", childTopic.Attribute("id").Value),
                                        (childTopic.Attribute("tocTitle") ?? childTopic.Attribute("title")).Value
                                    ),
                                    GetTopicSummary(options, childTopic)
                                );

            if (childTopic.Attribute("apiParentMode") != null)
            {
                // Insert reference documentation namespace links at the location marked by apiParentMode attribute.
                if (childTopic.Attribute("apiParentMode").Value != "InsertBefore")
                    throw new NotSupportedException();

                var namespaceList = FormatNamespaceList(docsrc);

                return namespaceList.Concat(new XElement[] { childTopicRow });
            }
            else
            {
                // Otherwise just directly insert the row for this child topic.
                return childTopicRow;
            }
        }


        static object GetTopicSummary(CommandLineOptions options, XElement topic)
        {
            // Do we have a .aml file matching this topic?
            string topicId = topic.Attribute("id").Value;

            var filename = options.AmlSrc.FirstOrDefault(aml => topicId.Equals(Path.GetFileNameWithoutExtension(aml), StringComparison.OrdinalIgnoreCase));

            if (filename != null)
            {
                // Include the first paragraph of the .aml article in the index page.
                var doc = XDocument.Load(filename);

                return doc.Descendants(ns + "para").First().Nodes();
            }
            else
            {
                // If no .aml file, look for a summary attribute in the Layout.content metadata.
                var summary = topic.Attribute("summary");

                return (summary != null) ? summary.Value : null;
            }
        }


        static IEnumerable<XElement> FormatNamespaceList(XDocument[] docsrc)
        {
            // Find all the namespaces that we have reference docs for.
            var namespaceList = from doc in docsrc
                                from member in doc.Element("doc").Element("members").Elements()
                                where IsMemberType(member, 'N')
                                orderby MemberName(member)
                                select member;

            // Format them into a table.
            return from member in namespaceList
                   select FormatTableRow(
                       new XElement(ns + "codeEntityReference", member.Attribute("name").Value),
                       GetNamespaceSummary(member)
                   );
        }


        static IEnumerable<object> GetNamespaceSummary(XElement namespaceMember)
        {
            var summary = namespaceMember.Element("summary");

            // If there is more than one paragraph, include only the first one in the summary.
            var paragraphs = summary.Elements("p");

            if (paragraphs.Count() > 1)
            {
                summary = paragraphs.First();
            }

            return ConvertDocSrcToAml(summary);
        }


        static IEnumerable<object> ConvertDocSrcToAml(XElement element)
        {
            return element.Nodes().Select(ConvertNodeToAml);
        }


        static object ConvertNodeToAml(XNode node)
        {
            var childElement = node as XElement;

            if (childElement == null)
                return node;

            switch (childElement.Name.LocalName)
            {
                case "p":
                    return new XElement(ns + "para", ConvertDocSrcToAml(childElement));

                case "see":
                    return new XElement(ns + "codeEntityReference", childElement.Attribute("cref").Value);

                case "a":
                    return new XElement(ns + "link", new XAttribute(xlink + "href", childElement.Attribute("href").Value.Replace(".htm", "")), childElement.Value);

                default:
                    return childElement.Value;
            }
        }


        static XElement FormatTableRow(params object[] columnContent)
        {
            return new XElement(ns + "row", from content in columnContent
                                            select new XElement(ns + "entry", content));
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


        static bool HasAttribute(XElement element, string attributeName)
        {
            var attribute = element.Attribute(attributeName);

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
