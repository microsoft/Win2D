// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace exportsample
{
    class SampleExporter
    {
        public static void Export(Configuration config, SampleDirectory sample)
        {
            new SampleExporter(config, sample).Go();
        }

        Configuration config;
        SampleDirectory sample;
        HashSet<string> remainingProjects;
        HashSet<string> copiedFiles;

        SampleExporter(Configuration config, SampleDirectory sample)
        {
            this.config = config;
            this.sample = sample;
        }

        void Go()
        {
            CleanDestinationDirectory();
            ExportSampleProjects();
            WriteNuGetConfig();
        }

        void CleanDestinationDirectory()
        {
            // Delete all the directories and files under the sample (except for the .sln, which is
            // unique to the exported samples repro).  This is to allow us to catch (re)moved files.
            if (!Directory.Exists(sample.Destination))
                return;

            foreach (var dir in Directory.EnumerateDirectories(sample.Destination))
            {
                Directory.Delete(dir, true);
            }

            foreach (var file in Directory.EnumerateFiles(sample.Destination))
            {
                if (Path.GetExtension(file) != ".sln")
                {
                    File.Delete(file);
                }
            }
        }

        void ExportSampleProjects()
        {
            // Process all the projects.  Any files referenced by a project will be exported.
            var projects = FindProjects();

            remainingProjects = new HashSet<string>(from project in projects select project.FullName);
            copiedFiles = new HashSet<string>();

            while (remainingProjects.Count > 0)
            {
                var project = remainingProjects.First();
                var destination = config.GetDestination(project);

                if (!copiedFiles.Contains(destination) && File.Exists(project))
                {
                    ExportProject(project, destination);
                    copiedFiles.Add(destination);
                }

                remainingProjects.Remove(project);
            }
        }

        void WriteNuGetConfig()
        {
            var doc = new XDocument();

            var configuration = new XElement("configuration");
            doc.Add(configuration);

            if (config.Options.PackageSource != null)
            {
                var packageSources = new XElement("packageSources");
                var add = new XElement("add");
                add.SetAttributeValue("key", "Private Package Source");
                add.SetAttributeValue("value", config.Options.PackageSource);
                packageSources.Add(add);
                configuration.Add(packageSources);
            }

            doc.Save(Path.Combine(sample.Destination, "nuget.config"));
        }

        FileInfo[] FindProjects()
        {
            var sourceDir = Path.GetFullPath(Path.Combine(config.Options.Root, sample.Source));
            return new DirectoryInfo(sourceDir).GetFiles("*proj", SearchOption.AllDirectories);
        }

        void ExportProject(string source, string destination)
        {
            ProcessAndCopyProject(source, destination);
            CopyProjectFilters(source);
        }

        void ProcessAndCopyProject(string source, string destination)
        {
            var project = ProjectProcessor.Export(source, config, sample, destination);

            remainingProjects.UnionWith(project.FindImportedProjectsThatNeedExporting());

            if (project.ReferencesWin2DNuGetPackage)
            {
                switch (project.NuGetType)
                {
                    case NuGetProjectType.PackagesConfig:
                        CopyPackagesConfigWithAddedWin2DReference(project);
                        break;

                    case NuGetProjectType.Csproj:
                        // nothing - .csproj <PackageReference> doesn't require any external files
                        break;

                    default:
                        throw new Exception(string.Format("Unhandled NuGetProjectType: {0}", project.NuGetType));
                }
            }

            // Any referenced files should be copied            
            CopyReferencedFiles(project);
        }

        void CopyPackagesConfigWithAddedWin2DReference(ProjectProcessor project)
        {
            var source = Path.Combine(project.SourceDirectory, "packages.config");

            XDocument packagesConfig;

            if (File.Exists(source))
            {
                packagesConfig = XDocument.Load(source);
            }
            else
            {
                packagesConfig = new XDocument();
                packagesConfig.Add(new XElement("packages"));
            }

            RemoveExistingWin2DReferences(packagesConfig);

            var package = new XElement("package");
            package.SetAttributeValue("id", project.Win2DPackage);
            package.SetAttributeValue("version", config.Options.Win2DVersion);

            // The framework name in packages.config does not exactly match then name on disk
            string framework;
            if (project.IsNative)
            {
                framework = "Native";
            }
            else
            {
                switch (project.GetTargetPlatformIdentifier())
                {
                    case TargetPlatformIdentifier.Windows:
                        framework = "win81";
                        break;

                    case TargetPlatformIdentifier.WindowsPhone:
                        framework = "wpa81";
                        break;

                    default:
                        throw new Exception(string.Format("Unexpected TargetPlatformIdentifier", project.GetTargetPlatformIdentifier()));
                }
            }

            package.SetAttributeValue("targetFramework", framework);

            packagesConfig.Root.Add(package);

            var destination = Path.Combine(project.DestinationDirectory, "packages.config");
            packagesConfig.Save(destination);
            copiedFiles.Add(destination);
        }

        private static void RemoveExistingWin2DReferences(XDocument packagesConfig)
        {
            var existingReferences = from package in packagesConfig.Descendants("package")
                                     where package.Attribute("id").Value.StartsWith("Win2D")
                                     select package;
            existingReferences.ToList().ForEach(e => e.Remove());
        }

        void CopyReferencedFiles(ProjectProcessor project)
        {
            foreach (var srcDst in project.FilesToCopy)
            {
                var source = srcDst.Key;
                var destination = srcDst.Value;

                if (!copiedFiles.Contains(destination) && !destination.EndsWith("proj"))
                {
                    CopyFile(source, destination);
                    copiedFiles.Add(destination);
                }
            }
        }

        void CopyProjectFilters(string projectFileName)
        {
            string filters = projectFileName + ".filters";
            if (File.Exists(filters))
            {
                var dest = config.GetDestination(filters);
                CopyFile(filters, dest);
            }
        }

        void CopyFile(string fileName, string dest)
        {
            Directory.CreateDirectory(Path.GetDirectoryName(dest));
            File.Copy(fileName, dest, true);
        }
    }
}
