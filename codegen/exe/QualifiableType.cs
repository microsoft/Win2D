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
        public abstract string ProjectedNameIncludingIndirection { get; }
        public abstract string NativeName { get; }

        public virtual string AccessorSuffix { get { return ""; } }
        // For example, to include CComPtr's Get()

        public virtual string RuntimeClassMemberTypeName { get { return ProjectedName; } }
        // For example, ComPtr<T> for interfaces; T for everything else.

        public virtual string IdlTypeNameQualifier { get { return ""; } }
        // Some struct overrides require a namespace qualifier in IDL, such as "Windows.Foundation."
        // This string follows the IDL style, where '.' is used as the qualifying operator.
    }
}
