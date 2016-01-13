// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices.WindowsRuntime;

namespace ExtractAPISurface
{
    class AssemblyCollection
    {
        List<Assembly> inputAssemblies;
        List<Assembly> referenceAssemblies;


        public AssemblyCollection(CommandLineOptions options)
        {
            // Tell the CLR how to resolve missing assemblies and namespaces.
            AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve += ReflectionOnlyAssemblyResolve;
            WindowsRuntimeMetadata.ReflectionOnlyNamespaceResolve += ReflectionOnlyNamespaceResolve;

            // Load reference assemblies. We won't process these directly, but need them to resolve symbols used by the main input assemblies.
            referenceAssemblies = options.ReferenceAssemblies.Select(LoadAssembly).ToList();

            // Load the main set of input assemblies.
            inputAssemblies = new List<Assembly>();

            foreach (var input in options.InputAssemblies)
            {
                var assembly = LoadAssembly(input);

                // Force the assembly to resolve all dependent types, to avoid infinite
                // recursion if future assemblies depend on types defined by this one.
                foreach (var type in assembly.DefinedTypes) { }

                inputAssemblies.Add(assembly);
            }
        }


        // Gets the input assemblies.
        public IEnumerable<Assembly> InputAssemblies
        {
            get { return inputAssemblies; }
        }


        // Checks if the specified type comes from one of our reference assemblies.
        public bool TypeIsFromReferenceAssembly(Type type)
        {
            return referenceAssemblies.Contains(type.Assembly);
        }


        static Assembly LoadAssembly(string name)
        {
            Console.WriteLine("Loading {0}", name);

            return Assembly.ReflectionOnlyLoadFrom(name);
        }


        static Assembly ReflectionOnlyAssemblyResolve(object sender, ResolveEventArgs args)
        {
            string assemblyName = AppDomain.CurrentDomain.ApplyPolicy(args.Name);

            return Assembly.ReflectionOnlyLoad(assemblyName);
        }


        void ReflectionOnlyNamespaceResolve(object sender, NamespaceResolveEventArgs e)
        {
            foreach (var assembly in referenceAssemblies.Concat(inputAssemblies))
            {
                if (assembly.DefinedTypes.Any(type => type.Namespace == e.NamespaceName))
                {
                    e.ResolvedAssemblies.Add(assembly);
                }
            }
        }
    }
}
