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
using System.Linq;
using System.Reflection;

namespace ExtractAPISurface
{
    // Fill in things that really ought to be in System.Reflection, but aren't.
    static class Utils
    {
        // Checks whether an attribute is defined on a type, in a way that works in the reflection only context.
        public static bool HasAttribute<TAttribute>(this Type type)
        {
            return type.GetCustomAttributesData().Any(attribute => attribute.AttributeType == typeof(TAttribute));
        }


        // Checks whether the specified type represents a delegate.
        public static bool IsDelegate(this Type type)
        {
            return type.IsSubclassOf(typeof(Delegate));
        }


        // Checks whether a property is public.
        public static bool IsPublic(this PropertyInfo property)
        {
            return property.GetMethod.IsPublic;
        }


        // Checks whether an event is public.
        public static bool IsPublic(this EventInfo eventInfo)
        {
            return eventInfo.AddMethod.IsPublic;
        }
    }
}
