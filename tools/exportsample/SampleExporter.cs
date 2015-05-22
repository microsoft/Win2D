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
            // Delete all the directories under the sample (leaving any files in the root).  This is to allow
            // us to catch (re)moved files.
            foreach (var dir in Directory.EnumerateDirectories(sample.Destination))
            {
                Directory.Delete(dir, true);
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
                CopyPackagesConfigWithAddedWin2DReference(project);
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
            package.SetAttributeValue("id", "Win2D");
            package.SetAttributeValue("version", config.Options.Win2DVersion);

            // The framework name in packages.config does not exactly match then name on disk
            string framework = project.GetFramework();
            switch (framework)
            {
                case "win": framework = "win81"; break;
                case "wpa": framework = "wpa81"; break;
                case "native": framework = "Native"; break;
                default: throw new Exception("Unexpected framework type");
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
                                     where package.Attribute("id").Value == "Win2D"
                                     select package;
            existingReferences.ToList().ForEach(e => e.Remove());
        }

        void CopyReferencedFiles(ProjectProcessor project)
        {
            foreach (var srcDst in project.FilesToCopy)
            {
                var source = srcDst.Key;
                var destination = srcDst.Value;

                if (!copiedFiles.Contains(destination))
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
