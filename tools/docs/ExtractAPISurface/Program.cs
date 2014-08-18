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
using Shared;

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
