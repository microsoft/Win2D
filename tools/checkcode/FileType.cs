// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Linq;

namespace CheckCode
{
    // Different types of file that copyright banners can be applied to.
    public enum FileType
    {
        Cpp,
        Xml,
    }


    static class FileTypeExtensions
    {
        // Adds comment syntax to decorate a copyright banner as appropriate for the specified type of file.
        public static string[] FormatCopyrightBanner(this FileType fileType, string[] copyrightBanner)
        {
            switch (fileType)
            {
                case FileType.Cpp:
                    return FormatCppCopyright(copyrightBanner);

                case FileType.Xml:
                    return FormatXmlCopyright(copyrightBanner);
                
                default:
                    throw new ArgumentException();
            }
        }


        // Adds comment syntax to decorate a copyright banner for a C style file (C++, C#, IDL, etc.)
        static string[] FormatCppCopyright(string[] copyrightBanner)
        {
            var commentedBanner = from line in copyrightBanner
                                  select string.IsNullOrEmpty(line) ? "//" : "// " + line;

            string[] blankLine = { string.Empty };

            return commentedBanner.Concat(blankLine).ToArray();
        }


        // Adds comment syntax to decorate a copyright banner as appropriate for XML.
        static string[] FormatXmlCopyright(string[] copyrightBanner)
        {
            string[] xmlHeader =
            {
                "<?xml version=\"1.0\"?>",
                "<!--"
            };

            string[] xmlFooter =
            {
                "-->"
            };

            return xmlHeader.Concat(copyrightBanner)
                            .Concat(xmlFooter)
                            .ToArray();
        }


        // Extracts any existing copyright banner from the start of a file.
        public static string[] GetExistingCopyrightBanner(this FileType fileType, string[] fileContents)
        {
            string[] existingComment;

            switch (fileType)
            {
                case FileType.Cpp:
                    existingComment = GetCppCopyright(fileContents);
                    break;

                case FileType.Xml:
                    existingComment = GetXmlCopyright(fileContents);
                    break;

                default:
                    throw new ArgumentException();
            }

            var existingBlanks = fileContents.Skip(existingComment.Count())
                                             .TakeWhile(line => line.All(char.IsWhiteSpace));

            return existingComment.Concat(existingBlanks).ToArray();
        }


        // Extracts any existing copyright banner from the start of a C style file (C++, C#, IDL, etc.)
        static string[] GetCppCopyright(string[] fileContents)
        {
            return fileContents.TakeWhile(line => line.TrimStart().StartsWith("//"))
                               .ToArray();
        }


        // Extracts any existing copyright banner from the start of an XML file.
        static string[] GetXmlCopyright(string[] fileContents)
        {
            int i = 0;

            // Include the XML declaration, if any.
            if (fileContents[i].StartsWith("<?xml"))
            {
                i++;
            }

            // Include the XML comment, if any.
            if (fileContents[i].StartsWith("<!--"))
            {
                while (!fileContents[i].Contains("-->"))
                {
                    i++;
                }

                i++;
            }

            return fileContents.Take(i).ToArray();
        }
    }
}
