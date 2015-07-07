// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Xml.Linq;

namespace DocDiff
{
    // Stores docs describing a member of our API (class, method, enum value, etc.) plus what XML file it was found in.
    class ApiMember : IEquatable<ApiMember>
    {
        public XElement MemberElement { get; private set; }
        public string ApiName { get; private set; }
        public string FileName { get; private set; }


        public ApiMember(XElement memberElement, string fileName)
        {
            MemberElement = memberElement;
            ApiName = memberElement.Attribute("name").Value;
            FileName = fileName;
        }


        // Equality comparison looks only at the API name, not what XML file it came from.
        public bool Equals(ApiMember other)
        {
            return ApiName == other.ApiName;
        }


        public override int GetHashCode()
        {
            return ApiName.GetHashCode();
        }
    }
}
