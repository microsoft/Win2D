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
