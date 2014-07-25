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
using System.IO;
using System.Linq;
using System.Collections.Generic;

namespace Copyright
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
        public List<string> Ext = new List<string>();

        // File extensions to skip (optionally semicolon separated).
        public List<string> SkipExt = new List<string>();

        // Directories to skip entirely (optionally semicolon separated).
        public List<string> SkipDir = new List<string>();

        // Root folder for the scan.
        public string RootDir;

        // Other options.
        public bool Validate;
        public bool Verbose;


        // Internal state: private so not settable from the commandline.
        private List<string> unknownExtensions = new List<string>();

        public int UnknownExtensionCount
        {
            get { return unknownExtensions.Count; }
        }


        // Cache the loaded copyright banner text.
        public string[] CopyrightBanner
        {
            get;
            private set;
        }

        public string[] PreviousCopyrightBanner
        {
            get;
            private set;
        }


        public void Initialize()
        {
            Ext = ExpandSemicolons(Ext);
            SkipExt = ExpandSemicolons(SkipExt);
            SkipDir = ExpandSemicolons(SkipDir);

            if (string.IsNullOrEmpty(RootDir))
            {
                RootDir = Directory.GetCurrentDirectory();
            }

            CopyrightBanner = File.ReadAllLines(CopyrightFile);

            if (!string.IsNullOrEmpty(PreviousCopyright))
            {
                PreviousCopyrightBanner = File.ReadAllLines(PreviousCopyright);
            }
        }


        public bool WantDirectory(string dirname)
        {
            string dir = Path.GetFileName(dirname);

            return !SkipDir.Contains(dir, StringComparer.OrdinalIgnoreCase);
        }


        public bool WantFile(string filename)
        {
            string ext = Path.GetExtension(filename).TrimStart('.');

            // Want it?
            if (Ext.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                if (Verbose)
                    Console.WriteLine("Checking {0}", GetRelativeName(filename));

                return true;
            }

            // Skip it?
            if (SkipExt.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                if (Verbose)
                    Console.WriteLine("Skipping {0}", GetRelativeName(filename));

                return false;
            }

            // Warn about it?
            if (!unknownExtensions.Contains(ext, StringComparer.OrdinalIgnoreCase))
            {
                Console.WriteLine("Error: Unknown extension .{0} ({1})", ext, GetRelativeName(filename));

                unknownExtensions.Add(ext);
            }

            return false;
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
