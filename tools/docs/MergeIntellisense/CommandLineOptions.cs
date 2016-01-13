// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System.Collections.Generic;

namespace MergeIntellisense
{
    public class CommandLineOptions
    {
        [CommandLineParser.Required]
        public string Output;

        [CommandLineParser.Required]
        public readonly List<string> Inputs = new List<string>();
    }
}
