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

namespace DocDiff
{
    // Stores the name of a member of our API (class, method, enum value, etc.) plus what XML doc file it was found in.
    class ApiMember : IEquatable<ApiMember>
    {
        public string ApiName { get; private set; }
        public string FileName { get; private set; }


        public ApiMember(string apiName, string fileName)
        {
            ApiName = apiName;
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
