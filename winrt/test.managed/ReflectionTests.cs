// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NativeComponent;
using System.Linq;
using System.Reflection;

namespace test.managed
{
    [TestClass]
    public class ReflectionTests
    {
        [TestMethod]
        public void AllFactoriesAreAgile()
        {
            var assembly = typeof(CanvasDevice).GetTypeInfo().Assembly;

            var types = from type in assembly.DefinedTypes
                        where HasActivationFactory(type)
                        select type;

            foreach (var type in types)
            {
                Assert.IsTrue(ReflectionHelper.IsFactoryAgile(type.FullName), type.Name + " factory should be agile");
            }
        }


        static bool HasActivationFactory(TypeInfo type)
        {
            // Non-class types cannot be activatable.
            if (!type.IsClass)
                return false;

            // If it has constructors, it is activatable.
            if (type.DeclaredConstructors.Any())
                return true;

            // If it has static methods, there is an activation factory that can be QI'd for the statics interface.
            if (type.DeclaredMethods.Any(method => method.IsStatic))
                return true;

            // Check for static properties too.
            if (type.DeclaredProperties.Any(property => property.GetMethod.IsStatic))
                return true;

            return false;
        }
    }
}
