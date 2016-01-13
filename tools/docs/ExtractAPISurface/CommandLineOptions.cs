// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System.Collections.Generic;

namespace ExtractAPISurface
{
    public class CommandLineOptions
    {
        [CommandLineParser.Required]
        public readonly List<string> InputAssemblies = new List<string>();

        [CommandLineParser.Name("ReferenceAssembly")]
        public readonly List<string> ReferenceAssemblies = new List<string>();

        public string OutputPath;
    }
}
