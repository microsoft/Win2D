// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Collections.Generic;
using Shared;

namespace DocDiff
{
    public class CommandLineOptions
    {
        public readonly List<string> DocSrc = new List<string>();
        public readonly List<string> ApiRef = new List<string>();
        public readonly List<string> AmlSrc = new List<string>();

        public string LogMissingDocs;
        public string DocSrcPath;
    }
}
