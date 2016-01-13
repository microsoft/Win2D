// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System;

namespace ExtractAPISurface
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
            // Load assemblies.
            var assemblies = new AssemblyCollection(options);

            foreach (var assembly in assemblies.InputAssemblies)
            {
                // Generate C# code containing only the public API surface of each assembly.
                using (var generator = new CodeGenerator(options, assemblies, assembly))
                {
                    generator.GenerateAPISurface(assembly);
                }
            }

            CodeGenerator.WritePlaceholderNamespaceSummaries(options);
        }
    }
}
