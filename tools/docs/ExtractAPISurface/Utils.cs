// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Linq;
using System.Reflection;

namespace ExtractAPISurface
{
    // Fill in things that really ought to be in System.Reflection, but aren't.
    static class Utils
    {
        // Checks whether an attribute is defined on a type, in a way that works in the reflection only context.
        public static bool HasAttribute<TAttribute>(this MemberInfo type)
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
