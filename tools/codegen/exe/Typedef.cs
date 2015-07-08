// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CodeGen
{
    namespace XmlBindings
    {
        public class Typedef
        {
            [XmlAttributeAttribute]
            public string Name;

            [XmlAttributeAttribute]
            public string From;
        }

    }
}
