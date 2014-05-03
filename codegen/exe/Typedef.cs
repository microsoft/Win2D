//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CodeGen
{
    public class Typedef
    {
        [XmlAttributeAttribute]
        public string Name;

        [XmlAttributeAttribute]
        public string From;
    }
}
