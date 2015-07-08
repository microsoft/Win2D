// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace exportsample
{
    class ProjectProcessor
    {
        public static XNamespace NS = "http://schemas.microsoft.com/developer/msbuild/2003";

        public Dictionary<string, string> FilesToCopy = new Dictionary<string, string>();
        public bool ReferencesWin2DNuGetPackage { get; private set; }

        string fileName;
        public string SourceDirectory { get; private set; }
        public string DestinationDirectory { get; private set; }
        string relativePackagesDirectory;

        Configuration config;
        SampleDirectory sample;
        XDocument doc;

        public static ProjectProcessor Export(string projectFileName, Configuration config, SampleDirectory sample, string destination)
        {
            var project = new ProjectProcessor(projectFileName, config, sample);
            project.Process();
            project.Save(destination);
            return project;
        }

        ProjectProcessor(string projectFileName, Configuration config, SampleDirectory sample)
        {
            this.fileName = projectFileName;
            this.SourceDirectory = Path.GetDirectoryName(fileName);
            this.DestinationDirectory = config.GetDestination(SourceDirectory);
            this.config = config;
            this.sample = sample;

            var relativeSolutionDirectory = GetRelativePath(sample.Destination + "\\", DestinationDirectory);
            this.relativePackagesDirectory = Path.Combine(relativeSolutionDirectory, "packages\\");

            doc = XDocument.Load(projectFileName);
        }

        public void Save(string dest)
        {
            Directory.CreateDirectory(Path.GetDirectoryName(dest));
            doc.Save(dest);
        }

        // Pull out a list of names of projects that are imported by this project
        public List<string> FindImportedProjectsThatNeedExporting()
        {
            var imports = doc.Descendants(NS + "Import");

            List<string> importPaths = new List<string>();

            foreach (var import in imports)
            {
                var importPath = import.Attribute("Project").Value;

                importPath = Expand(importPath);

                // After we've expanded variables we know about, anything that still starts with a $ we leave unmodified
                if (!importPath.StartsWith("$"))
                {
                    // Everything else we convert into a full path
                    importPath = Path.GetFullPath(Path.Combine(SourceDirectory, importPath));
                }

                if (config.ShouldExport(importPath))
                    importPaths.Add(importPath);
            }

            return importPaths;
        }

        public static bool IsValidPathName(string includeFile)
        {
            return !Path.GetInvalidPathChars().Any(includeFile.Contains);
        }

        // Expand variables we know about
        string Expand(string path)
        {
            path = path.Replace("$(MSBuildThisFileDirectory)", SourceDirectory + "\\");

            foreach (var property in config.Properties)
            {
                string prop = string.Format("$({0})", property.Key);

                path = path.Replace(prop, property.Value);
            }

            return path;
        }

        void Process()
        {
            ProcessInlineImports();
            ProcessFileReferences();
            RelocateNuGetReferences();
            ConvertWin2DProjectReferences();
        }

        #region Inline Imports

        void ProcessInlineImports()
        {
            while (ProcessNextInlinedImport())
                ;
        }

        bool ProcessNextInlinedImport()
        {
            var imports = doc.Descendants(NS + "Import");

            foreach (var import in imports)
            {
                var importPath = import.Attribute("Project").Value;

                var inlineImport = config.FindInlineImportFor(importPath);
                if (inlineImport != null)
                {
                    DoInlineImport(import, inlineImport);
                    return true;
                }
            }

            return false;
        }

        void DoInlineImport(XElement import, InlineImport inlineImport)
        {
            var importPath = ExpandPath(SourceDirectory, import.Attribute("Project").Value);

            var inlinedDoc = XDocument.Load(importPath);

            RelocateImports(Path.GetDirectoryName(importPath), inlinedDoc);

            var project = inlinedDoc.Root;

            List<XNode> filteredNodes = new List<XNode>();
            bool inSample = true;
            foreach (var node in project.Nodes())
            {
                if (node is XComment)
                {
                    var comment = (XComment)node;
                    if (comment.Value.Contains("[[NOT IN SAMPLE]]"))
                    {
                        inSample = false;
                        continue;
                    }
                    else if (comment.Value.Contains("[[IN SAMPLE]]"))
                    {
                        inSample = true;
                        continue;
                    }
                }

                if (inSample)
                    filteredNodes.Add(node);
            }

            import.AddBeforeSelf(filteredNodes);

            if (inlineImport.Elements.Count > 0)
            {
                import.AddBeforeSelf(inlineImport.Elements);
            }

            import.Remove();
        }

        void RelocateImports(string directory, XDocument doc)
        {
            foreach (var import in doc.Descendants(NS + "Import"))
            {
                import.SetAttributeValue("Project", ExpandPath(directory, import.Attribute("Project").Value));
            }
        }

        #endregion

        #region File References

        const string MSBuildThisFileDirectory = "$(MSBuildThisFileDirectory)";

        void ProcessFileReferences()
        {
            foreach (var element in doc.Descendants())
            {
                var includeAttribute = element.Attribute("Include");
                if (includeAttribute == null)
                    continue;

                includeAttribute.Value = ProcessFileReference(includeAttribute.Value);
            }

            foreach (var propertyThatIsFileReference in config.FileReferenceProperties)
            {
                foreach (var element in doc.Descendants(NS + propertyThatIsFileReference))
                {
                    element.Value = ProcessFileReference(element.Value);
                }
            }
        }

        string ProcessFileReference(string originalValue)
        {
            string includedFile = originalValue;

            // If this is a reference in a Shared project then it'll start with $(MSBuildThisFileDirectory).
            // Strip this off while we're looking at this.
            if (includedFile.StartsWith(MSBuildThisFileDirectory))
            {
                includedFile = includedFile.Substring(MSBuildThisFileDirectory.Length);
            }

            // Expand properties (eg $(AssetDir))
            includedFile = Expand(includedFile);

            // Ignore anything that isn't a valid path name
            if (!IsValidPathName(includedFile))
                return originalValue;

            // Ignore anything that doesn't exist
            var fullPath = Path.GetFullPath(Path.Combine(SourceDirectory, includedFile));
            if (!File.Exists(fullPath))
                return originalValue;

            // If the file is part of this sample then we can reference it unmodified
            if (fullPath.StartsWith(sample.Source))
            {
                var dest = config.GetDestination(fullPath);
                FilesToCopy.Add(fullPath, dest);
                return originalValue;
            }

            // Otherwise we'll need to copy it somewhere to reference it, and update our element to point to the new location                
            foreach (var entry in config.DuplicateDirectories)
            {
                if (fullPath.StartsWith(entry.Key))
                {
                    var dest = fullPath.Replace(entry.Key, Path.Combine(sample.Destination, entry.Value));
                    FilesToCopy[fullPath] = dest;

                    return GetRelativePath(dest, DestinationDirectory);
                }
            }

            return originalValue;
        }

        #endregion

        #region Relocate NuGet References

        void RelocateNuGetReferences()
        {
            // When projects have existing nuget references these need to be updated to be relative to 
            // the packages directory that'll be in the sample's directory.
            //
            // We can identify these by things that reference "\packages\".  NuGet adds Import, Error 
            // and HintPath elements that might reference these.

            RelocateNuGetReferences(doc.Descendants(NS + "Import"));
            RelocateNuGetReferences(doc.Descendants(NS + "Error"));
            RelocateNuGetReferences(doc.Descendants(NS + "HintPath"));
        }

        static Regex packagesRegex = new Regex(@"[.\\]*\\packages\\");

        void RelocateNuGetReferences(IEnumerable<XElement> elements)
        {
            foreach (var element in elements)
            {
                if (element.Value.Length > 0)
                    element.Value = RelocateNuGetReference(relativePackagesDirectory, element.Value);

                foreach (var attribute in element.Attributes())
                {
                    attribute.Value = RelocateNuGetReference(relativePackagesDirectory, attribute.Value);
                }
            }
        }

        private static string RelocateNuGetReference(string relativePackagesDirectory, string reference)
        {
            var match = packagesRegex.Match(reference);
            if (match.Success)
            {
                return packagesRegex.Replace(reference, relativePackagesDirectory);
            }
            else
            {
                return reference;
            }
        }

        #endregion

        #region Convert Win2D project references to NuGet package references

        void ConvertWin2DProjectReferences()
        {
            ReferencesWin2DNuGetPackage = FindAndRemoveWin2DProjectReferences();

            if (ReferencesWin2DNuGetPackage)
                AddWin2DNuGetPackage();
        }

        bool FindAndRemoveWin2DProjectReferences()
        {
            bool foundAReference = false;

            var toRemove = new List<XElement>();

            foreach (var reference in doc.Descendants(NS + "ProjectReference"))
            {
                if (config.IsWin2DProject(reference.Attribute("Include").Value))
                {
                    toRemove.Add(reference);
                    foundAReference = true;
                }
            }

            toRemove.ForEach(RemoveElementAndParentIfEmpty);

            if (RemoveExistingReferencesToWin2DNuGet())
                foundAReference = true;

            return foundAReference;
        }

        bool RemoveExistingReferencesToWin2DNuGet()
        {
            var elementsToRemove = new List<XElement>();

            elementsToRemove.AddRange(from element in doc.Descendants(NS + "Import")
                                      where element.Attribute("Project").Value.Contains("packages\\Win2D")
                                      select element);

            elementsToRemove.AddRange(from element in doc.Descendants(NS + "Reference")
                                      where element.Attribute("Include").Value.Contains("Microsoft.Graphics.Canvas")
                                      select element);

            elementsToRemove.AddRange(from element in doc.Descendants(NS + "Error")
                                      where element.Attribute("Condition").Value.Contains("packages\\Win2D")
                                      select element);


            if (elementsToRemove.Count == 0)
                return false;

            elementsToRemove.ForEach(RemoveElementAndParentIfEmpty);
            return true;
        }
        
        void AddWin2DNuGetPackage()
        {
            var framework = GetFramework();

            string importsDir = Path.Combine(Win2DPackagePath, "build", framework);

            var propsImport = Path.Combine(importsDir, "Win2D.props");
            var targetsImport = Path.Combine(importsDir, "Win2D.targets");

            doc.Root.AddFirst(MakeImportElement(propsImport));

            // Targets is added after last element (rather than the last node) so that it appears before
            // any comments at the end of the project.
            doc.Root.Elements().Last().AddAfterSelf(MakeImportElement(targetsImport));


            var importsTarget = GetEnsureNuGetPackageBuildImportsTarget();
            importsTarget.Add(MakeCheckImport(propsImport));
            importsTarget.Add(MakeCheckImport(targetsImport));
        }

        public string GetFramework()
        {
            // C++ projects are always native
            if (fileName.EndsWith("vcxproj"))
                return "native";

            // Otherwise we need to look at TargetPlatformIdentifier
            var targetPlatformIdentifier = doc.Descendants(NS + "TargetPlatformIdentifier").FirstOrDefault();

            // No platform == windows
            if (targetPlatformIdentifier == null)
                return "win";

            switch (targetPlatformIdentifier.Value)
            {
                case "Windows":
                    return "win";

                case "WindowsPhoneApp":
                    return "wpa";

                case "UAP":
                    // Since NuGet currently doesn't know about UAP as a framwork moniker,
                    // the nuget package uses 'win'.
                    //
                    // When NuGet learns about UAP, and the Win2D package has been updated accordingly,
                    // this will need to be changed.
                    return "win";
            }

            throw new Exception("Unabled to determine NuGet framework for " + fileName);
        }

        XElement MakeImportElement(string filename)
        {
            var importElement = new XElement(NS + "Import");            
            importElement.SetAttributeValue("Project", filename);
            importElement.SetAttributeValue("Condition", "Exists('" + filename + "')");
            return importElement;
        }

        string Win2DPackagePath
        {
            get
            {
                return Path.Combine(relativePackagesDirectory, "Win2D." + config.Options.Win2DVersion);
            }
        }

        Regex nugetTargetsFile = new Regex(@"packages\\.*\.targets$");

        XElement GetEnsureNuGetPackageBuildImportsTarget()
        {
            // Look for an existing one...
            var targets = from element in doc.Descendants(NS + "Target")
                         where element.Attribute("Name").Value == "EnsureNuGetPackageBuildImports"
                         select element;

            var target = targets.FirstOrDefault();

            if (target != null)
                return target;

            // There isn't an existing one, so we create one...            
            var ensureImports = new XElement(NS + "Target");
            ensureImports.SetAttributeValue("Name", "EnsureNuGetPackageBuildImports");
            ensureImports.SetAttributeValue("BeforeTargets", "PrepareForBuild");

            var propertyGroup = new XElement(NS + "PropertyGroup");
            var errorText = new XElement(NS + "ErrorText");
            errorText.Value = "This project references NuGet package(s) that are missing on this computer. Enable NuGet Package Restore to download them.  For more information, see http://go.microsoft.com/fwlink/?LinkID=322105. The missing file is {0}.";
            propertyGroup.Add(errorText);
            ensureImports.Add(propertyGroup);

            var targetsImports = from element in doc.Descendants(NS + "Import")
                                 where nugetTargetsFile.IsMatch(element.Attribute("Project").Value)
                                 select element;

            var firstImportTarget = targetsImports.First(); // if there isn't an import target then something strange is going on!

            firstImportTarget.AddBeforeSelf(ensureImports);

            return ensureImports;
        }

        XElement MakeCheckImport(string filename)
        {
            var element = new XElement(NS + "Error");
            element.SetAttributeValue("Condition", string.Format("!Exists('{0}')", filename));
            element.SetAttributeValue("Text", string.Format("$([System.String]::Format('$(ErrorText)', '{0}'))", filename));
            return element;
        }

        #endregion

        #region Utils

        string ExpandPath(string root, string path)
        {
            string expandedPath = Expand(path);
            if (!IsValidPathName(expandedPath))
                throw new Exception(string.Format("Couldn't expand path {0}", path));

            return Path.GetFullPath(Path.Combine(root, expandedPath));
        }

        // From http://stackoverflow.com/a/703292/157728
        static string GetRelativePath(string filespec, string folder)
        {
            Uri pathUri = new Uri(filespec);
            // Folders must end in a slash
            if (!folder.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                folder += Path.DirectorySeparatorChar;
            }
            Uri folderUri = new Uri(folder);
            return Uri.UnescapeDataString(folderUri.MakeRelativeUri(pathUri).ToString().Replace('/', Path.DirectorySeparatorChar));
        }

        static void RemoveElementAndParentIfEmpty(XElement element)
        {
            var parent = element.Parent;
            element.Remove();
            if (parent.IsEmpty)
                parent.Remove();
        }

        #endregion
    }
}
