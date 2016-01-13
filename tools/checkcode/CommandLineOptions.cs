// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace CheckCode
{
    public class CommandLineOptions
    {
        // Text file containing the copyright banner.
        [CommandLineParser.Required]
        public string CopyrightFile;

        // Optionally specify a file containing previous copyright banner,
        // in which case we will only overwrite things that exactly match this.
        public string PreviousCopyright;

        // File extensions to check for copyright banners (optionally semicolon separated).
        public List<string> CppExt = new List<string>();
        public List<string> XmlExt = new List<string>();

        // File extensions to skip (optionally semicolon separated).
        public List<string> SkipExt = new List<string>();

        // Directories to skip entirely (optionally semicolon separated).
        public List<string> SkipDir = new List<string>();

        // Root folder for the scan.
        public string RootDir;

        // Other options.
        public bool Validate;
        public bool Verbose;
        public bool CheckFormatting;


        // Internal state: private so not settable from the commandline.
        private List<string> unknownExtensions = new List<string>();

        public int UnknownExtensionCount
        {
            get { return unknownExtensions.Count; }
        }


        // Cache the loaded copyright banner text.
        public Dictionary<FileType, string[]> CopyrightBanner
        {
            get;
            private set;
        }

        public Dictionary<FileType, string[]> PreviousCopyrightBanner
        {
            get;
            private set;
        }


        public void Initialize()
        {
            CppExt = ExpandSemicolons(CppExt);
            XmlExt = ExpandSemicolons(XmlExt);
            SkipExt = ExpandSemicolons(SkipExt);
            SkipDir = ExpandSemicolons(SkipDir);

            if (string.IsNullOrEmpty(RootDir))
            {
                RootDir = Directory.GetCurrentDirectory();
            }

            CopyrightBanner = ReadCopyrightBanner(CopyrightFile);

            if (!string.IsNullOrEmpty(PreviousCopyright))
            {
                PreviousCopyrightBanner = ReadCopyrightBanner(PreviousCopyright);
            }
        }


        static Dictionary<FileType, string[]> ReadCopyrightBanner(string filename)
        {
            var copyrightBanner = File.ReadAllLines(filename);

            // Format the copyright differently for C-style vs. XML files.
            return new Dictionary<FileType, string[]>
            {
                { FileType.Cpp, FileType.Cpp.FormatCopyrightBanner(copyrightBanner) },
                { FileType.Xml, FileType.Xml.FormatCopyrightBanner(copyrightBanner) }
            };
        }


        public bool WantDirectory(string dirname)
        {
            string dir = Path.GetFileName(dirname);

            return !SkipDir.Contains(dir, StringComparer.OrdinalIgnoreCase);
        }


        public FileType? WantFile(string filename)
        {
            string ext = Path.GetExtension(filename).TrimStart('.');

            // Want it?
            if (CppExt.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                if (Verbose)
                    Console.WriteLine("Checking {0}", GetRelativeName(filename));

                return FileType.Cpp;
            }

            if (XmlExt.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                if (Verbose)
                    Console.WriteLine("Checking {0}", GetRelativeName(filename));

                return FileType.Xml;
            }

            // Skip it?
            if (SkipExt.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                if (Verbose)
                    Console.WriteLine("Skipping {0}", GetRelativeName(filename));

                return null;
            }

            // Warn about it?
            if (!unknownExtensions.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                Console.WriteLine("Error: Unknown extension .{0} ({1})", ext, GetRelativeName(filename));

                unknownExtensions.Add(ext);
            }

            return null;
        }


        public string GetRelativeName(string filename)
        {
            if (filename.StartsWith(RootDir, StringComparison.OrdinalIgnoreCase))
            {
                return filename.Substring(RootDir.Length).TrimStart('\\', '/');
            }
             
            return filename;
        }


        // Replace ("a;b", "c") -> ("a", "b", "c").
        static List<string> ExpandSemicolons(List<string> strings)
        {
            return strings.Select(s => s.Split(';'))
                          .SelectMany(s => s)
                          .ToList();
        }
    }
}
