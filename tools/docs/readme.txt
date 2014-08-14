The documentation build process works like so:

1) ExtractAPISurface tool reflects over the public API exposed by compiled WinRT 
   binaries, and generates clean new C# code which defines only the API surface without 
   any implementation details or ties to WinRT. This generated code lands in obj\docs.

2) This API reference code is fed to the C# compiler, which creates temporary assemblies 
   and empty/placeholder XML documentation files.

3) Real documentation comments are checked into the source tree as XML files in the 
   docsrc folders.

4) The DocDiff tool compares the contents of the docsrc folders with the placeholder XML 
   files that were generated during step #2. It generates warnings if anything is missing 
   documentation, or if there are stale docs left over from old types that no longer 
   exist in the API.

5) The Sandcastle tool (http://shfb.codeplex.com) combines the generated temporary 
   assemblies and docsrc XML files to create HTML reference documentation, which ends up 
   in bin\docs.

6) The MergeIntellisense tool combines all the XML files from the docsrc folders to 
   create a single merged XML per assembly, which is used by Visual Studio intellisense. 
   The merged results are written to bin\intellisense.

To build the documentation:

    msbuild BuildDocs.proj
