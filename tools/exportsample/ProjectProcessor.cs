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
    public enum NuGetProjectType
    {
        PackagesConfig,
        Csproj
    }

    public enum TargetPlatformIdentifier
    {
        Windows,
        WindowsPhone,
        UAP
    }

    class ProjectProcessor
    {
        public static XNamespace NS = "http://schemas.microsoft.com/developer/msbuild/2003";

        public Dictionary<string, string> FilesToCopy = new Dictionary<string, string>();
        public bool ReferencesWin2DNuGetPackage { get; private set; }
        public bool IsNative { get; private set; }
        public string Win2DPackage { get; private set; }
        public NuGetProjectType NuGetType { get; private set; }

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

            this.IsNative = fileName.EndsWith("vcxproj");
            this.ReferencesWin2DNuGetPackage = FindAndRemoveWin2DProjectReferences();

            bool isUap = (GetTargetPlatformIdentifier() == TargetPlatformIdentifier.UAP);

            if (isUap && !this.IsNative)
                this.NuGetType = NuGetProjectType.Csproj;
            else
                this.NuGetType = NuGetProjectType.PackagesConfig;

            if (isUap)
                Win2DPackage = "Win2D.uwp";
            else
                Win2DPackage = "Win2D.win81";
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
            path = path.Replace("$(ProjectDir)", DestinationDirectory + "\\");

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
            FlattenConditions();
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
            foreach (var entry in config.DuplicateFiles)
            {
                if (fullPath.StartsWith(entry.Key))
                {
                    var value = entry.Value;

                    var dest = Path.GetFullPath(fullPath.Replace(entry.Key, Path.Combine(sample.Destination, Expand(value))));
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
            if (!ReferencesWin2DNuGetPackage)
                return;

            switch (NuGetType)
            {
                case NuGetProjectType.PackagesConfig:
                    AddWin2DNuGetPackage();
                    break;

                case NuGetProjectType.Csproj:
                    AddWin2DPackageReference();
                    break;

                default:
                    throw new Exception("Unknown project type");
            }
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
        
        // Old style (still used for .vcxproj): Win2D imports are added directly to the project.
        void AddWin2DNuGetPackage()
        {
            var framework = GetFramework();

            string importsDir = Path.Combine(Win2DPackagePath, "build", framework);

            var targetsImport = Path.Combine(importsDir, Win2DPackage + ".targets");

            // Targets is added after last element (rather than the last node) so that it appears before
            // any comments at the end of the project.
            doc.Root.Elements().Last().AddAfterSelf(MakeImportElement(targetsImport));


            var importsTarget = GetEnsureNuGetPackageBuildImportsTarget();
            importsTarget.Add(MakeCheckImport(targetsImport));
        }

        // New style: use a <PackageReference> MSBuild item.
        void AddWin2DPackageReference()
        {
            var newReference = new XElement(NS + "PackageReference");
            newReference.SetAttributeValue("Include", "Win2D.uwp");

            var version = new XElement(NS + "Version");
            version.Value = config.Options.Win2DVersion;
            newReference.Add(version);

            var existingPackageReference = doc.Descendants(NS + "PackageReference").First();
            existingPackageReference.AddAfterSelf(newReference);
        }

        public string GetFramework()
        {
            // C++ projects are always native
            if (IsNative)
                return "native";

            // Otherwise we need to look at TargetPlatformIdentifier
            switch (GetTargetPlatformIdentifier())
            {
                case TargetPlatformIdentifier.Windows:
                    return "win";

                case TargetPlatformIdentifier.WindowsPhone:
                    return "wpa";

                default:
                    throw new Exception(string.Format("Unexpected TargetPlatformIdentifier: {0}", GetTargetPlatformIdentifier()));
            }
        }

        public TargetPlatformIdentifier GetTargetPlatformIdentifier()
        {
            var targetPlatformIdentifier = doc.Descendants(NS + "TargetPlatformIdentifier").FirstOrDefault();

            // No platform == windows
            if (targetPlatformIdentifier == null)
                return TargetPlatformIdentifier.Windows;

            switch (targetPlatformIdentifier.Value)
            {
                case "Windows":
                    return TargetPlatformIdentifier.Windows;

                case "WindowsPhoneApp":
                    return TargetPlatformIdentifier.WindowsPhone;

                case "UAP":
                    return TargetPlatformIdentifier.UAP;
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
                return Path.Combine(relativePackagesDirectory, Win2DPackage + "." + config.Options.Win2DVersion);
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

        #region Remove MSBuild 'Condition' attributes that will always pass or fail

        void FlattenConditions()
        {
            const string propertyRegex = @"'\$\(([\w\s]+)\)'";                                  // matches: '$(property)'
            const string operatorRegex = @"([=!]=)";                                            // matches: == or !=
            const string valueRegex = @"'([\w\s\.]*)'";                                         // matches: 'value'
            const string clauseRegex = propertyRegex + " " + operatorRegex + " " + valueRegex;  // matches: '$(property)' == 'value'
            const string conditionRegex = "^(?:" + clauseRegex + "(?: And |$))+";               // matches: one or more clauses, separated by And

            // Note this is a very simplistic parser. It won't match (and therefore won't flatten)
            // expressions with different spacing, missing quotes, use of Or or ! operators, etc.

            var regex = new Regex(conditionRegex);

            var elementsWithConditions = doc.Descendants().Where(e => e.Attribute("Condition") != null);

            foreach (var element in elementsWithConditions.ToList())
            {
                var match = regex.Match(element.Attribute("Condition").Value);

                if (match.Success)
                {
                    var clauses = from i in Enumerable.Range(0, match.Groups[1].Captures.Count)
                                  select new
                                  {
                                      Property = match.Groups[1].Captures[i].Value,
                                      Operator = match.Groups[2].Captures[i].Value,
                                      Value    = match.Groups[3].Captures[i].Value,
                                  };

                    // Only flatten expressions whose properties are specified in the config.
                    if (!clauses.All(clause => config.PropertiesToFlatten.Contains(clause.Property)))
                        continue;

                    // Evaluate the expression.
                    bool result = clauses.All(clause => EvaluateConditionClause(clause.Property, clause.Operator, clause.Value));

                    if (result)
                    {
                        // If the expression is true, remove the Condition attribute, so the element is now unconditionally included.
                        element.SetAttributeValue("Condition", null);
                    }
                    else
                    {
                        // Otherwise remove the element itself.
                        element.Remove();
                    }
                }
            }
        }

        bool EvaluateConditionClause(string property, string comparisonOperator, string value)
        {
            var propertyValue = doc.Descendants(NS + property).Single().Value;

            switch (comparisonOperator)
            {
                case "==":
                    return (propertyValue == value);

                case "!=":
                    return (propertyValue != value);

                default:
                    throw new NotSupportedException();
            }
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
