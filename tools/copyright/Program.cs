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
using Shared;

namespace Copyright
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
                Console.Error.WriteLine("Error: {0}", e.Message);
                return 1;
            }

            return 0;
        }


        static void Run(CommandLineOptions options)
        {
            options.Initialize();

            // Recurse through the directory tree.
            int badFileCount = CheckDirectory(options.RootDir, options);

            // Report errors if anything went wrong.
            if (options.Validate && badFileCount > 0)
            {
                throw new Exception(string.Format("Encountered files with wrong copyright banner ({0})", badFileCount));
            }

            if (options.UnknownExtensionCount > 0)
            {
                throw new Exception(string.Format("Encountered unknown file extensions ({0})", options.UnknownExtensionCount));
            }
        }


        static int CheckDirectory(string dirname, CommandLineOptions options)
        {
            int badFileCount = 0;

            // Recurse into subdirectories.
            foreach (var subdir in Directory.GetDirectories(dirname))
            { 
                if (options.WantDirectory(subdir))
                {
                    badFileCount += CheckDirectory(subdir, options);
                }
            }

            // Check files.
            foreach (var filename in Directory.GetFiles(dirname))
            { 
                FileType? fileType = options.WantFile(filename);

                if (fileType.HasValue)
                {
                    badFileCount += CheckFile(filename, options, fileType.Value);
                }
            }

            return badFileCount;
        }


        static int CheckFile(string filename, CommandLineOptions options, FileType fileType)
        {
            var fileContents = File.ReadAllLines(filename);

            if (Enumerable.SequenceEqual(options.CopyrightBanner[fileType], fileContents.Take(options.CopyrightBanner[fileType].Length)))
            {
                // This file already has the correct copyright.
                return 0;
            }
            else
            {
                if (options.Validate)
                {
                    // Just report that the copyright does not match.
                    Console.WriteLine("Error: Wrong copyright: {0}", options.GetRelativeName(filename));
                }
                else
                {
                    // Find any existing copyright.
                    var existingCopyright = fileType.GetExistingCopyrightBanner(fileContents);

                    if (options.PreviousCopyrightBanner != null && !Enumerable.SequenceEqual(existingCopyright, options.PreviousCopyrightBanner[fileType]))
                    {
                        // Warn if what we tried to overwrite doesn't match the expected previous (then don't actually edit anything).
                        Console.WriteLine("Warning: Skipping {0}: doesn't match previous copyright", options.GetRelativeName(filename));
                    }
                    else
                    {
                        // Remove the old copyright.
                        var withoutOldCopyright = fileContents.Skip(existingCopyright.Count());

                        // Insert the new copyright.
                        var withNewCopyright = options.CopyrightBanner[fileType].Concat(withoutOldCopyright);

                        // Write out the new file.
                        Console.WriteLine("Warning: Updating copyright: {0}", options.GetRelativeName(filename));

                        File.WriteAllLines(filename, withNewCopyright);
                    }
                }

                return 1;
            }
        }
    }
}
