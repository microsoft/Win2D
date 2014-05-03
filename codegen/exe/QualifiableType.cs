//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CodeGen
{
    public abstract class QualifiableType
    {
        public abstract string ProjectedName { get; }
        public abstract string NativeName { get; }
    }
}
