// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace CheckCode
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

            // Look for common source code formatting errors (note: FileType.Cpp covers all C-family syntaxes, including C#).
            if (options.CheckFormatting && fileType == FileType.Cpp)
            {
                CheckFormatting(filename, fileContents, options);
            }

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


        static void CheckFormatting(string filename, string[] lines, CommandLineOptions options)
        {
            var lineNumbers = Enumerable.Range(0, lines.Length);
            var allButLastLineNumber = Enumerable.Range(0, lines.Length - 1);

            // We want spaces, not tabs.
            OutputFormatWarnings(
                filename,
                lineNumbers.Where(lineNumber => lines[lineNumber].Contains('\t')),
                options,
                "Tabs should be spaces");

            // We want "if (", with a space between the keyword and the open parenthesis.
            OutputFormatWarnings(
                filename,
                lineNumbers.Where(lineNumber => checkForMissingSpaceAfterKeyword.IsMatch(lines[lineNumber])),
                options,
                "Missing space after keyword");

            // If a lambda has no parameters, it should be written as just [], without a following ().
            OutputFormatWarnings(
                filename,
                lineNumbers.Where(lineNumber => checkForLambdaWithNoParameters.IsMatch(lines[lineNumber])),
                options,
                "Lambda with no parameters should not include ()");

            // If the open { of a lambda is placed on the next line after the [] part, these should be aligned the same.
            OutputFormatWarnings(
                filename,
                allButLastLineNumber.Where(lineNumber => IsWrongLambdaIndentation(lines[lineNumber], lines[lineNumber + 1])),
                options,
                "The VS lambda indentation gremlin strikes again!");
        }


        static Regex checkForMissingSpaceAfterKeyword = new Regex(@"(?<![\w_])(catch|do|for|foreach|if|return|switch|using|while)\(");
        static Regex checkForLambdaWithNoParameters = new Regex(@"]\s*\(\)(?!\s*(mutable|->))");


        static bool IsWrongLambdaIndentation(string line, string nextLine)
        {
            return line.Trim().StartsWith("[") &&
                   nextLine.Trim() == "{" &&
                   GetIndentation(line) != GetIndentation(nextLine);
        }


        static int GetIndentation(string line)
        {
            return line.TakeWhile(char.IsWhiteSpace).Count();
        }


        static void OutputFormatWarnings(string filename, IEnumerable<int> linesWithWarnings, CommandLineOptions options, string message)
        {
            foreach (var lineNumber in linesWithWarnings)
            {
                Console.WriteLine("{0}({1}): Warning: {2}", options.GetRelativeName(filename), lineNumber + 1, message);
            }
        }
    }
}
