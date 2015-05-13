The documentation build process works like so:

1) ExtractAPISurface tool reflects over the public API exposed by compiled WinRT 
   binaries, and generates clean new C# code which defines only the API surface without 
   any implementation details or ties to WinRT. This generated code lands in obj\docs.

2) This API reference code is fed to the C# compiler, which creates temporary assemblies 
   and empty/placeholder XML documentation files.

3) Real documentation comments are checked into the source tree as XML files in the 
   winrt\docsrc folders.

4) The DocDiff tool compares the contents of the docsrc folders with the placeholder XML 
   files that were generated during step #2. It generates warnings if anything is missing 
   documentation, or if there are stale docs left over from old types that no longer 
   exist in the API.

5) The Sandcastle tool (http://shfb.codeplex.com and https://github.com/EWSoftware/SHFB)
   combines the generated temporary assemblies, docsrc XML files, and overview pages
   generated from .aml files, creating HTML reference documentation which ends up in bin\docs.

6) The MergeIntellisense tool combines all the XML files from the docsrc folders to 
   create a single merged XML per assembly, which is used by Visual Studio intellisense. 
   The merged results are written to bin\intellisense.



To build the documentation:

1) builddocs.cmd



To add a new conceptual doc page (.aml):

1) Author the new .aml file in the docsrc folder. Information on MAML (Microsoft Assistance
   Markup Language) can be found at: http://ewsoftware.github.io/MAMLGuide/. You can also
   copy from an existing .aml file.

2) Add the new .aml to layout.content. Add a new <Topic> node that points to the ID of the doc.

3) Build documentation as normal.