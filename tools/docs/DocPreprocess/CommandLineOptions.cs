// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Shared;
using System.Collections.Generic;

namespace DocPreprocess
{
    public class CommandLineOptions
    {
        [CommandLineParser.Required]
        public readonly List<string> DocSrc = new List<string>();

        public readonly List<string> AmlSrc = new List<string>();

        public string LayoutContent;

        public string DocSrcOutput;
        public string TopicsOutput;
    }
}
